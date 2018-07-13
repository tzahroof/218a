/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------
File:
    Glasses.c

Description:
    Detects events from the fader slider and updates glasses position based
    off reading. Also updates the fader lights based off the slider position.

Author:
    Rohun Kulkarni


*****************************************************************************/
// #define TEST

/*----------------------------- Include Files -----------------------------*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"
#include "ES_ShortTimer.h"
#include "ES_Port.h"


#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "utils/uartstdio.h"

#include "BITDEFS.H"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "PWM16Tiva.h"
#include "ADMulti.h"

// Header File
#include "Glasses.h"
#include "Servo.h"
#include "ShiftRegisterWrite.h"
#include "LightDefs.h"

// Test Harness for testing this module
#ifdef TEST
	#include "termio.h"
#endif

/*----------------------------- Module Defines ----------------------------*/
#define AD_MAX 3700 //max ADC 
#define AD_MIN 900	// min ADC
#define FADER_MAX 3800
#define FADER_MIN AD_MIN
#define MIN_ANGLE 30//Minimum angle allowable for the glasses servo
#define MAX_ANGLE 65
#define GLASSES_CHANNEL 6
#define AD_TOL 100
#define LIGHTS_SR 0 // the 0th shift register 
#define NUM_LIGHTS 7


/* --------------------------- Module Variables -----------------------------*/
static uint8_t MyPriority;
static uint32_t LastInputState[3];
static float DEGREE_PER_AD = (MAX_ANGLE - MIN_ANGLE)/
                                                  (FADER_MAX-FADER_MIN + 0.0);


/* ------------------------ Private Function Prototypes ---------------------*/
static uint16_t Fader2Angle(uint32_t);
static uint8_t DetermineLights( uint32_t A2D_Value );
static uint32_t FilterADInput(uint32_t);

/* -------------------------- Public Functions -----------------------------*/

/****************************************************************************
 Function
     InitGlassesService

 Parameters
     uint8_t, priority of this service

 Returns
     bool, false if error in intialization

 Description
     Saves away the priority, initializes Tiva ports
     For use with the Sunglasses Servo
 Notes

 Author
Rohun Kulkarni, 11/12/2017


****************************************************************************/
bool InitGlasses(uint8_t Priority) 
{
    MyPriority = Priority;
	bool returnValue;

    // Initialize the shift register for the fader lights 
    // Maybe choose to move this elsewhere in the code 
    SR_Init(LIGHTS_SR);

	ADC_MultiRead(LastInputState); // read AD line. Set as last input state
    
	ES_Event ThisEvent;
	ThisEvent.EventType = ES_INIT; // post init event
	if (ES_PostToService( MyPriority, ThisEvent) == true)
    {   
        returnValue =  true;
    } 
    else
    { 
        returnValue = false;
    }
	ThisEvent.EventType = ES_INIT;
	PostGlasses(ThisEvent);
	return returnValue;
}

/****************************************************************************
 Function
	PostGlassesEvent

 Parameters
    ES_Event

 Returns
    bool
 Description
     Takes an event and posts it to this service.
 Notes

 Author
    Rohun Kulkarni, 11/12/2017
****************************************************************************/
void PostGlasses(ES_Event ThisEvent) 
{
	 ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
     RunGlasses

 Parameters
     ES_Event 

 Returns
    ES_NO_EVENT

 Description
     Implements GlassesServiceSM
 Notes

 Author
Rohun Kulkarni, 11/12/2017
****************************************************************************/
ES_Event RunGlasses(ES_Event ThisEvent) 
{
    ES_Event ReturnEvent; 
    ReturnEvent.EventType = ES_NO_EVENT; 
    
    if (ThisEvent.EventType == ES_INIT)
    {
        ADC_MultiRead(LastInputState);
        //Updates the fader change 
        ES_Event FaderEvent;
        FaderEvent.EventType = ES_FaderChange;
        FaderEvent.EventParam = FilterADInput(LastInputState[0]);
        PostGlasses(FaderEvent);
    }
    
    // if Fader position is changed    
    if (ThisEvent.EventType == ES_FaderChange)
    {       
        // read AD input line from fader
        uint16_t newAngle;
        //convert fader AD value to angle.
        newAngle = Fader2Angle(ThisEvent.EventParam);	
        //write this new angle to servo.
        ServoWriteAngle(newAngle,GLASSES_CHANNEL);   
        FaderLights(DetermineLights(ThisEvent.EventParam));
    }
    return ReturnEvent;		
}

/****************************************************************************
 Function
     Check4GlassesEvents

 Parameters
    void
 Returns
    void 

 Description
     Checks for Glasses events and posts to Glasses service
 Notes

 Author
Rohun Kulkarni, 11/12/2017
****************************************************************************/
bool Check4GlassesEvent(void)
{
	bool ReturnVal = false;
    uint32_t CurrentInputState[3];
    ADC_MultiRead(CurrentInputState);  //read current Fader Analog value
    
    //if the fader state is diff from the previous state by some tolerance
	if (abs(CurrentInputState[0] - LastInputState[0]) > AD_TOL)
    {   
		ES_Event ThisEvent; 
		ThisEvent.EventType = ES_FaderChange;  //new fader change event 
         //Analog value is PARAM for event
         //Filter the AD input to reduce noise from the potentiometer slider
        ThisEvent.EventParam = FilterADInput(CurrentInputState[0]);   
		PostGlasses(ThisEvent);   //Post FaderChange to Glasses Queue
		LastInputState[0] = CurrentInputState[0]; //update last input
        ReturnVal = true;
	} 
    return ReturnVal;
}

/****************************************************************************
 Function
     Fader2Angle

 Parameters
    uint32_t fader input
 Returns
    uint8_t angle output

 Description
     converts fader voltage to an angle output
    
 Notes
     Assumes fader potentiometer set up to minimum 1.65V, max 3.3V
 Author
Rohun Kulkarni, 11/12/2017
****************************************************************************/
uint16_t Fader2Angle(uint32_t AD_Input)
{
	uint16_t returnAngle; 
    //Filters the AD_Input to reduce the noise from jerking
        
    
    //Angle = (Conversion factor * (Analog input - offset))+ minimum angle
	returnAngle = (DEGREE_PER_AD*(AD_Input-AD_MIN))+ MIN_ANGLE; 

    
    //Prevents angle going beyond min and max. Keeps glasses from breaking
    if (returnAngle > MAX_ANGLE) {
        returnAngle = MAX_ANGLE;
    } 
    else if (returnAngle < MIN_ANGLE)
    {
        returnAngle = MIN_ANGLE;
    }
    
    return returnAngle;
}


/****************************************************************************
 Function
     FaderLights

 Parameters
    uint8_t byte containing which lights to light up 

 Returns
    none

 Description
     Lights up the fader lights using shift register 
    
 Author
    Erez Krimsky, 11/15/2017
****************************************************************************/
void FaderLights(uint8_t WhichLights)
{
    SR_Write(WhichLights, LIGHTS_SR);
}

/*****************************************************************************/
/***********************       Private Functions      ************************/
/*****************************************************************************/


/****************************************************************************
 Function
     DetermineLights

 Parameters
    uint32_t value that was output from A to D 

 Returns
    uint8_t which lights to light up 

    
 Author
    Erez Krimsky, 11/15/2017
****************************************************************************/
static uint8_t DetermineLights(uint32_t A2D_Value)
{
    uint16_t binWidth = (FADER_MAX - FADER_MIN)/(NUM_LIGHTS+1);
    uint8_t numLights = 7-((A2D_Value - FADER_MIN)/binWidth);
    switch(numLights)
    {
        case(0):
            return LIGHT_0;
        case(1):
            return LIGHT_1;
        case(2):
            return LIGHT_2;
        case(3):
            return LIGHT_3;
        case(4):
            return LIGHT_4;
        case(5):
            return LIGHT_5;
        case(6):
            return LIGHT_6;
        case(7):
            return LIGHT_7;
        default:
              return LIGHT_0;
        
    }
}

/****************************************************************************
 Function
     Fader2Angle

 Parameters
    uint32_t fader input
 Returns
    uint8_t angle output

 Description
     converts fader voltage to an angle output
    
 Notes
     Assumes fader potentiometer set up to minimum 1.65V, max 3.3V
 Author
Rohun Kulkarni, 11/12/2017
****************************************************************************/
static uint32_t FilterADInput(uint32_t AD_Input)
{
    //Filters the AD_Input to reduce the noise from jerking
    if(AD_Input< AD_MIN){
        AD_Input = AD_MIN;
    } 
    else if (AD_Input > AD_MAX)
    {
        AD_Input = AD_MAX;
    }
    return AD_Input;
}

/***************************** END SOURCE *********************************/
