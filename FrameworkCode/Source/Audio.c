/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------
 Module
   Audio.c

 Description
   - Service that plays music and checks for Hands Up (IR Sensor blocked)
   - Uses Port D from the Tiva to activate music tracks


 History
 When          		Who     What/Why
 -------------- 	---     --------
11/12/17			TZ      Started Coding
12/04/17            EK 		Clean up, #include TimeDefs.h
*****************************************************************************/
// #define TEST

/*  ------ Header Files -----*/ 
#include "Audio.h"
#include "HypeLights.h"
#include "ADMulti.h" 
#include "TimeDefs.h" 

// the common headers for C99 types 
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

// Framework Files
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"
#include "ES_ShortTimer.h"

// the headers to access the GPIO subsystem
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"

// the headers to access the TivaWare Library
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "BITDEFS.H"

/*----------------------------- Defines ------------------------------------*/
#define WELCOME_TRACK_PIN BIT2LO 
#define WELCOME_TRACK_PIN_INIT BIT2HI 

#define MAIN_TRACK_PIN BIT3LO 
#define MAIN_TRACK_PIN_INIT BIT3HI 

#define CELLY_TRACK_PIN BIT6LO 
#define CELLY_TRACK_PIN_INIT BIT6HI 

#define RESET_TRACK_PIN_LO BIT7LO 
#define RESET_TRACK_PIN_HI BIT7HI 

#define HAND_LIGHT_PIN_LO BIT3LO
#define HAND_LIGHT_PIN_HI BIT3HI

#define HAND_SENSOR_1_ON BIT1HI 
#define HAND_SENSOR_2_ON BIT2HI 

#define VOLTAGE_THRESHOLD 1500 // For checking if hands up (analog)

typedef enum { Welcome, Run, Celly, Reset } AudioState_t ;

/*---------------------------- Module Variables ---------------------------*/
static uint8_t MyPriority;
static AudioState_t CurrentState;
static bool HandsCanBeReceived;


/*------------------------ Private Function Prototypes ---------------------*/
static void PulseResetPin(void);
static void PlayTrack( uint8_t SongNumber );
static void TurnOnHandLights(bool HandLightsOnBool);


/* -------------------------- Public Functions -----------------------------*/

/****************************************************************************
 Function
     InitAudio

 Parameters
     uint8_t Priority

 Returns
     boolean

 Description Notes
 	 Sets the Priority, Initializes Port D and E, the Track Pins, the 
 	 IR Sensors, and the Hands Up Light and sets the CurrentState to Reset

 Author
      Tariq Zahroof, 11/13/2017
****************************************************************************/
bool InitAudio(uint8_t Priority) 
{
	MyPriority = Priority;
	
	//init port D and the music pins
	HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R3;
	while((HWREG(SYSCTL_PRGPIO) & SYSCTL_PRGPIO_R3) != SYSCTL_PRGPIO_R3)
		;
	HWREG(GPIO_PORTD_BASE + GPIO_O_DEN) |= (WELCOME_TRACK_PIN_INIT |
			 MAIN_TRACK_PIN_INIT | CELLY_TRACK_PIN_INIT | RESET_TRACK_PIN_HI);

	HWREG(GPIO_PORTD_BASE + GPIO_O_DIR) |= (WELCOME_TRACK_PIN_INIT | 
		     MAIN_TRACK_PIN_INIT | CELLY_TRACK_PIN_INIT | RESET_TRACK_PIN_HI);

	//init Port E
	HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R4;
	while((HWREG(SYSCTL_PRGPIO) & SYSCTL_PRGPIO_R4) != SYSCTL_PRGPIO_R4)
		;

    //init Hands Up light
	HWREG(GPIO_PORTE_BASE + GPIO_O_DEN) |= HAND_LIGHT_PIN_HI;
	HWREG(GPIO_PORTE_BASE + GPIO_O_DIR) |= HAND_LIGHT_PIN_HI;
  
	// Hand sensors initialized with ADC mult init in main

	//set currentstate to reset
	CurrentState = Reset;
    
    //set HandsCanBeReceived to false
    HandsCanBeReceived = false;
  
	return true;
}

/******************************************************************************
Function:
    Post function for this service in the ES framework

Description:
    Post method for ES framework
******************************************************************************/
bool PostAudio(ES_Event ThisEvent)
{
	return ES_PostToService(MyPriority,ThisEvent);
}


/****************************************************************************
 Function
     RunAudio

 Parameters
     ES_Event ThisEvent ,the event to post to the queue

 Returns
     Event with parameter ES_NO_EVENT

 Description
     Plays different music, Changes states, and looks for Hands Up to post
     in HypeLights

 Notes

 Author
      Tariq Zahroof, 11/13/2017
****************************************************************************/
ES_Event RunAudio(ES_Event ThisEvent) 
{
	ES_Event ReturnEvent;
	ReturnEvent.EventType = ES_NO_EVENT;  
	switch(CurrentState)
	{
		case Welcome:
			if (ThisEvent.EventType == ES_NewAudio)
			{
				PlayTrack(MAIN_TRACK_PIN);
				ES_Timer_InitTimer(HANDS_UP_TIMER,TEN_SEC);
				CurrentState = Run;
			}
			break;

		case Run:
			if (ThisEvent.EventType == ES_TIMEOUT && 
				ThisEvent.EventParam == HANDS_UP_TIMER)
			{
                HandsCanBeReceived = true;
				TurnOnHandLights(true);
				ES_Timer_InitTimer(HANDS_UP_TIMER, TEN_SEC);

			} 
			else if (ThisEvent.EventType == ES_HandsDetected)
			{
				TurnOnHandLights(false);
                ES_Event NewEvent;
                NewEvent.EventType = ES_HypeLights;
				PostHypeLights(NewEvent);

			} 
			else if (ThisEvent.EventType == ES_NewAudio) 
			{
                PlayTrack(CELLY_TRACK_PIN);
                TurnOnHandLights(false);
				ES_Event NewEvent;
                NewEvent.EventType = ES_Celebration;
				PostHypeLights(NewEvent);
				CurrentState = Celly;
			}
			break;

		case Celly:
			if (ThisEvent.EventType == ES_CelebrationComplete) 
			{
				PulseResetPin();
				CurrentState = Reset;
			}
			break;

		case Reset:
			if (ThisEvent.EventType == ES_Reset) 
			{
				PlayTrack(WELCOME_TRACK_PIN);
				CurrentState = Welcome;
			}
			break;
	} // end switch
	return ReturnEvent;
}

/****************************************************************************
 Function
     CheckHandsUp

 Parameters
     void

 Returns
     bool

 Description
     Checks if both Sensor 1 and 2 Pin is blocked by the Digital pin being On.
     Posts ES_HandsDetected

 Notes

 Author
      Tariq Zahroof, 11/14/2017
****************************************************************************/
bool CheckHandsUp(void) 
{

	bool ReturnBool = false;
    uint32_t ConversionResults[3];
    ADC_MultiRead(ConversionResults);
    
    /*
    * ConversionResults[1] is HandSensor 1
    * ConversionResults[2] is HandSensor 2
    */
    if((ConversionResults[1] > VOLTAGE_THRESHOLD) && 
    	   (ConversionResults[2] > VOLTAGE_THRESHOLD) && HandsCanBeReceived)           
    {
		ReturnBool = true;
        HandsCanBeReceived = false;
		ES_Event NewEvent;
		NewEvent.EventType 	= ES_HandsDetected;
		PostAudio(NewEvent);
	}
	return ReturnBool;
}

/*****************************************************************************/
/***********************       Private Functions      ************************/
/*****************************************************************************/

/*****************************************************************************
 Function
     PulseResetPin

 Parameters
     void

 Returns
     void

 Description
     Sets the Reset_Track_Pin low to stop all music, and then initializes 
     all pins to high

 Notes

 Author
      Tariq Zahroof, 11/13/2017
****************************************************************************/
static void PulseResetPin(void) 
{
	HWREG(GPIO_PORTD_BASE + (GPIO_O_DATA + ALL_BITS)) |= (RESET_TRACK_PIN_HI 
														| RESET_TRACK_PIN_LO);
	HWREG(GPIO_PORTD_BASE + (GPIO_O_DATA + ALL_BITS)) &= RESET_TRACK_PIN_LO;
	HWREG(GPIO_PORTD_BASE + (GPIO_O_DATA + ALL_BITS)) |= RESET_TRACK_PIN_HI;
}

/****************************************************************************
 Function
     PlayTrack

 Parameters
     uint8_t SongNumber

 Returns
     void

 Description
     Pulses the reset pin to turn all music off, then Sets the SongNumber 
     Pin low to play a track
 Notes

 Author
      Tariq Zahroof, 11/13/2017
****************************************************************************/
static void PlayTrack(uint8_t SongNumber)
{
	PulseResetPin();
	HWREG(GPIO_PORTD_BASE + (GPIO_O_DATA + ALL_BITS)) &= SongNumber;
}


/****************************************************************************
 Function
     TurnOnHandLights

 Parameters
     boolean HandLightsOnBool

 Returns
     void

 Description
     Turns on the lights if HandLightsOnBool is true (and turns them off 
     if false)
 Notes

 Author
      Tariq Zahroof, 11/14/2017
****************************************************************************/
static void TurnOnHandLights(bool HandLightsOnBool)
{
	//Lights are turned on by grounding the Tiva pin
	if(HandLightsOnBool)
	{
		HWREG(GPIO_PORTE_BASE + (GPIO_O_DATA + ALL_BITS)) |= HAND_LIGHT_PIN_HI;
	} 
	else
	{
		HWREG(GPIO_PORTE_BASE + (GPIO_O_DATA + ALL_BITS)) &= HAND_LIGHT_PIN_LO;
	}
}
