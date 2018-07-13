/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------
File:
   HypeLights.c

Description:
   This service controls the LED Strip

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 11/12/17 15:00 vsal    Created Skeleton
****************************************************************************/

//#define TEST

/*----------------------------- Include Files -----------------------------*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"
#include "ES_ShortTimer.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"	// Define PART_TM4C123GH6PM in project
#include "driverlib/gpio.h"
#include "HypeLights.h"
#include "Glasses.h"  // for fader lights in celly 
#include "LightDefs.h" 
#include "PWM16Tiva.h"

/*----------------------------- Module Defines ----------------------------*/
#define QUARTER_SEC 100

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
static uint8_t MyPriority;
static HypeState_t CurrentState;

/*-------------------- Private Function Prototypes ------------------------*/
static void UpdateFaderLights(void); 
static uint8_t LightsArray[] = {LIGHT_0, LIGHT_1, LIGHT_2, LIGHT_3, 
                                LIGHT_4, LIGHT_5, LIGHT_6, LIGHT_7};



/* -------------------------- Public Functions -----------------------------*/
/****************************************************************************
 Function
     InitHypeLights

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, initializes Tiva ports
     For use with the RGB LED Strip
 Notes

 Author
 Vincent Salpietro, 11/11/2017
****************************************************************************/
bool InitHypeLights ( uint8_t Priority )
{
    // Initialize the priority of the service to the input parameter value
    MyPriority = Priority;
    return true;
}


/****************************************************************************
 Function
     PostHypeLights

 Parameters
     EF_Event ThisEvent ,the event to post to the queue

 Returns
     bool false if the Enqueue operation failed, true otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
      Vincent Salpietro, 11/11/2017
****************************************************************************/
bool PostHypeLights( ES_Event ThisEvent )
{
    return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunHypeLights

 Parameters
   ES_Event : the event to process

 Returns
   ES_Event, ES_NO_EVENT if no error ES_ERROR otherwise

 Description
   Runs state machine that controls the frequency and color of the RGB Strip
 Notes
   
 Author
  Vincent Salpietro, 11/11/2017
****************************************************************************/
ES_Event RunHypeLights( ES_Event ThisEvent )
{
    ES_Event ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors
    static uint8_t CellyColor;

    switch(CurrentState)
    {
        // If Current State is Off
        case Off:
            // If Hype event received
            if( ThisEvent.EventType == ES_HypeLights )
            {
                // Set PWM duty cycle to 50% for one color
                PWM_TIVA_SetDuty( 50, 0 );
                PWM_TIVA_SetDuty( 0, 1 );
                PWM_TIVA_SetDuty( 50, 2 );
                // Set CurrentState to Hype1
                CurrentState = Hype1;
            }
            // If CalibrationLight event received
            if( ThisEvent.EventType == ES_Celebration )
            {
                CellyColor = 0;
                // Set PWM duty cycle to 50% for red
                PWM_TIVA_SetDuty( 50, CellyColor );
                PWM_TIVA_SetDuty( 0, 1 );
                PWM_TIVA_SetDuty( 0, 2 );
                // Start Celebration timer
                ES_Timer_InitTimer(CELLY_TIMER,QUARTER_SEC);
                // Set CurrentState to Celly
                CurrentState = CellyLights;
            }
            break;
        // If Current State is Hype1
        case Hype1:
            // If Hype event received
            if( ThisEvent.EventType == ES_HypeLights )
            {
                // Set PWM duty cycle to 50% for two colors
                PWM_TIVA_SetDuty( 0, 0 );
                PWM_TIVA_SetDuty( 50, 1 );
                PWM_TIVA_SetDuty( 50, 2 );               
                // Set CurrentState to Hype2
                CurrentState = Hype2;
            }
            // If CalibrationLight event received
            if( ThisEvent.EventType == ES_Celebration )
            {
                CellyColor = 0;
                // Set PWM duty cycle to 50% for red
                PWM_TIVA_SetDuty( 0, CellyColor );
                PWM_TIVA_SetDuty( 0, 1 );
                PWM_TIVA_SetDuty( 0, 2 );
                // Start Celebration timer
                ES_Timer_InitTimer(CELLY_TIMER,QUARTER_SEC);
                // Set CurrentState to Celly
                CurrentState = CellyLights;
            }     
            break;

        // If Current State is Hype2
        case Hype2:
            // If Hype event received
            if( ThisEvent.EventType == ES_HypeLights )
            {
                // Set PWM duty cycle to 50% for three colors
                PWM_TIVA_SetDuty( 50, 0 );
                PWM_TIVA_SetDuty( 50, 1 );
                PWM_TIVA_SetDuty( 0, 2 );
                // Set CurrentState to Hype3
                CurrentState = Hype3;
            }
            // If CalibrationLight event received
            if( ThisEvent.EventType == ES_Celebration )
            {
                CellyColor = 0;
                // Set PWM duty cycle to 50% for red
                PWM_TIVA_SetDuty( 50, CellyColor );
                PWM_TIVA_SetDuty( 0, 1 );
                PWM_TIVA_SetDuty( 0, 2 );
                // Start Celebration timer
                ES_Timer_InitTimer(CELLY_TIMER,QUARTER_SEC);
                // Set CurrentState to Celly
                CurrentState = CellyLights;
            } 
            break;
        
        // If Current State is Hype3_Celebration
        case Hype3:
            // If CalibrationLight event received
            if( ThisEvent.EventType == ES_Celebration )
            {
                CellyColor = 0;
                // Set PWM duty cycle to 50% for red
                PWM_TIVA_SetDuty( 50, CellyColor );
                PWM_TIVA_SetDuty( 0, 1 );
                PWM_TIVA_SetDuty( 0, 2 );
                // Start Celebration timer
                ES_Timer_InitTimer(CELLY_TIMER,QUARTER_SEC);
                // Set CurrentState to Celly
                CurrentState = CellyLights;
            }
            break;
            
        // If Current State is Celebration    
        case CellyLights:
            // If StopLights event received
            if( ThisEvent.EventType == ES_CelebrationComplete )
            {
                // Set PWM duty cycle to 0% for three colors
                PWM_TIVA_SetDuty( 0, 0 );
                PWM_TIVA_SetDuty( 0, 1 );
                PWM_TIVA_SetDuty( 0, 2 );
                // Set CurrentState to Off
                CurrentState = Off;
                ES_Event FaderEvent;
                
                
                FaderEvent.EventType = ES_FaderChange;
                FaderEvent.EventParam = 920;
                PostGlasses(FaderEvent);
            }
            // If Celly Timer Timeout
            if(ThisEvent.EventType == ES_TIMEOUT && 
                                    ThisEvent.EventParam == CELLY_TIMER)
			{
                // Set PWM duty cycle to 0% for three colors
                PWM_TIVA_SetDuty( 0, 0 );
                PWM_TIVA_SetDuty( 0, 1 );
                PWM_TIVA_SetDuty( 0, 2 );
                UpdateFaderLights(); 
                if(CellyColor == 2)
                {
                    CellyColor = 0;
                }
                else
                {
                    CellyColor++;
                }
                // Set PWM duty cycle to 50% for selected color
                PWM_TIVA_SetDuty( 50, CellyColor );
                // Start Celebration timer
                ES_Timer_InitTimer(CELLY_TIMER,QUARTER_SEC);                
            }
            break;
    
    }
    return ReturnEvent;
}

/*****************************************************************************/
/***********************       Private Functions      ************************/
/*****************************************************************************/



/****************************************************************************
 Function
    UpdateFaderLights

 Parameters
   none

 Returns
   none

 Description
    Increments the number of fader lights on (starting from 0) each time 
    it is called. After 7 lights are on, it will wrap around back to 0. 
    Called during the celebration stage every time the hype lights are 
    changed.
   
 Author
    Erez Krimsky
****************************************************************************/
static void UpdateFaderLights(void)
{
    static uint8_t currentStep = 0; 
    if (currentStep > NUMBER_LIGHTS)
    {
        currentStep = 0;
    }
    
    FaderLights(LightsArray[currentStep]); 
    currentStep++;
}


/* --------------------------- Test Harness --------------------------------*/
#ifdef TEST
int main(void)
{ 
	TERMIO_Init();
	InitHypeLights(MyPriority);
	ES_Event ReturnEvent;
	for(int count = 0; count < 3; count++)
	{
		getchar();
		ReturnEvent.EventType = ES_HypeLights;
		RunHypeLights(ReturnEvent);
	}

	return 0;
}
#endif


/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
