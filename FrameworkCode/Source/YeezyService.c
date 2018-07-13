/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------
 Module
   YeezyService.c

 Description
   'Central' service for Yeezy machine.



 History
 When          		Who     What/Why
 -------------- 	---     --------
11/12/17			EK      Started Coding               
 
****************************************************************************/
// #define TEST

/*  -------------------------- Header Files -----------------------------*/ 
#include "YeezyService.h"
#include "Record.h" // for spinning the Record
#include "Needle.h"
#include "Audio.h"
#include "HypeLights.h"
#include "ButtonLights.h" 
#include "TimeDefs.h"

/* ----------------------------- Defines ---------------------------------*/


/*---------------------------- Module Functions ---------------------------*/
// NONE

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable

typedef enum {Welcome, Run, Celebration, Reset} YeezyState_t;

static uint8_t MyPriority;
static YeezyState_t CurrentState;

// add a deferral queue for up to 3 pending deferrals +1 to allow for overhead
static ES_Event DeferralQueue[3+1];


/****************************************************************************
 Function
     InitYeezy

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, and does any 
     other required initialization for this service
 Notes

 Author
     E. Krimsky, 11/12/17
****************************************************************************/
bool InitYeezy( uint8_t Priority )
{
    MyPriority = Priority;

    //Put us into the initial pseudo-state to set up for the initial transition
    CurrentState = Reset;

    // Hardware initializations
    RecordInit(); 
    ButtonLightsInit(); 
	
	// setup defferal queue
	ES_InitDeferralQueueWith( DeferralQueue, ARRAY_SIZE(DeferralQueue) ); 

	// Set first timer, if initial
    bool returnValue = false;
    if (ES_Timer_InitTimer(RESET_TIMER, 1) == ES_Timer_OK)  
    {
    	returnValue = true;
    } 
    return returnValue; 
}

/****************************************************************************
 Function
     PostYeezy

 Parameters
     uint8_t : the priorty of this service

 Returns
     bool, false if error in initialization, true otherwise

 Description
     Saves away the priority, and does any 
     other required initialization for this service
 Notes

 Author
     E. Krimsky, 11/12/17
****************************************************************************/
bool PostYeezy( ES_Event ThisEvent)
{
	return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
     RunYeezy

 Parameters
     uint8_t : the priorty of this service

 Returns
     ES_Event

 Description
     Runs the state machine for this module
 Notes

 Author
     E. Krimsky, 11/12/17
****************************************************************************/
ES_Event RunYeezy( ES_Event ThisEvent)
{
	ES_Event ReturnEvent;
	ReturnEvent.EventType = ES_NO_EVENT;
	YeezyState_t NextState = CurrentState; 
	switch(CurrentState)
	{
		case Welcome:
			if (ThisEvent.EventType == ES_ButtonDown) 
			{	
				ES_Timer_InitTimer(RUN_TIMER, RUN_TIME);
				RecordStart();  // Spin the record 
				ES_Event NewAudio; 
				NewAudio.EventType = ES_NewAudio; 
				PostAudio(NewAudio);

				// Move needle to record 
				ES_Event NeedleUpdate;
				NeedleUpdate.EventType = ES_MoveToRecord;
				PostNeedle(NeedleUpdate); 

				// Turn off the button lights 
				ButtonLights(false); 
				NextState = Run;
			}
			else 
			{
				ReturnEvent.EventType = ES_ERROR; 
			}
			break;
		case Run:
			if (ThisEvent.EventType == ES_ButtonDown) 
			{
				// reset run time to 1 ms 
				ES_Timer_InitTimer(RUN_TIMER, 1);
			} 
			else if (ThisEvent.EventType == ES_TIMEOUT &&
									 ThisEvent.EventParam == RUN_TIMER)
			{
                // Post New Audio
				ES_Event NewAudio; // WIll want to give it some type??
				NewAudio.EventType = ES_NewAudio; 
				PostAudio(NewAudio);
				// TODO: Check with Vince about posting something to lights
				ES_Timer_InitTimer(CELEBRATION_TIMER, CELEBRATION_TIME);
				NextState = Celebration;
			}
			else 
			{
				ReturnEvent.EventType = ES_ERROR; 
				printf("\r\nFailure In YeezyService, bad event in Run");

			}
			break;
		case Celebration:
			if (ThisEvent.EventType == ES_ButtonDown)
			{
				ES_Timer_InitTimer(CELEBRATION_TIMER, 1);
			}
			else if (ThisEvent.EventType == ES_TIMEOUT &&
						ThisEvent.EventParam == CELEBRATION_TIMER) 
			{
				ES_Event NewEvent;
				NewEvent.EventType = ES_CelebrationComplete;
				PostAudio(NewEvent);
				PostHypeLights(NewEvent);
				PostNeedle(NewEvent); // Will make it move off record 

				// Stop the record 				
				RecordStop(); 
				
				ES_Timer_InitTimer(RESET_TIMER, RESET_TIME);
               
				NextState = Reset;
			} 
			else
			{
				ReturnEvent.EventType = ES_ERROR; 
			}
			break;
		case Reset:
			if (ThisEvent.EventType == ES_TIMEOUT &&
					ThisEvent.EventParam == RESET_TIMER)
			{				
				// for sunglasses we may want to shut off slider lights
				ES_Event NewEvent;
				NewEvent.EventType = ES_Reset;
				PostAudio(NewEvent);
				PostHypeLights(NewEvent);
				ButtonLights(true); 
				NextState = Welcome;
			} 
			else if (ThisEvent.EventType != ES_ButtonDown)
			{
				ReturnEvent.EventType = ES_ERROR; 
			}
			break; 
		default:
			ReturnEvent.EventType = ES_ERROR; // Case we didnt account for 
	} // end switch 
	CurrentState = NextState;
	return ReturnEvent;
}

/* ------------------------------ End of file ------------------------------*/
