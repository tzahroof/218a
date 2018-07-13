/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------
File:
    Needle.c

Description:
    Detects and reports button down event. Written to be used in Ed Carryer's 
    Events & Services framework.

Author:
    Erez Krimsky

Date:
    11/13/17
    11/20/17 - Fixing needle direction

******************************************************************************/

#include "Needle.h"
#include "Servo.h"
#include "TimeDefs.h"

/* -------------------- Readability defines -------------------------------- */
#define NEEDLE_GROUP 3 //PWM group 
#define NEEDLE_CHANNEL 7
#define START_ANGLE 0
#define RECORD_ANGLE 20
#define END_ANGLE 40
#define MOVE_TO_RECORD_TIME 500 // 1/2 second for now 
#define INC_TIME 80 // time between increments 
#define OFFSET 170 // Used for switching direction -- write OFFSET - desired


/* --------------------------- Module Variables -----------------------------*/
static uint8_t MyPriority;
typedef enum { Waiting, Moving
            	} NeedleState_t;
static NeedleState_t CurrentState; 
static const float AngleIncrement = (INC_TIME*(END_ANGLE-RECORD_ANGLE))/
                                           (RUN_TIME + CELEBRATION_TIME + 0.0);
static float CurrentAngle; 




/* --------------------- Private Function Prototypes ------------------------*/
static void MoveNeedle(float newAngle);

/* -------------------------- Public Functions ------------------------------*/
                
/******************************************************************************
Function:
    InitNeedle
	
Description:
	Initializes needle service for ES framework	
	
******************************************************************************/
bool InitNeedle( uint8_t Priority )
{
	bool returnValue;
	returnValue = true; // for now 

    MyPriority = Priority; // assign to static variable 
    CurrentState = Waiting;
    MoveNeedle(START_ANGLE);
    printf("\r\n Needle increment amount: %f degrees", AngleIncrement); 
    return returnValue;
}


/******************************************************************************
Function:
    PostNeedle
	
Description:	
	Post function for needle service in ES framework
	
******************************************************************************/
bool PostNeedle( ES_Event ThisEvent )
{
	return ES_PostToService( MyPriority, ThisEvent);
}

/******************************************************************************
Function:
    RunNeedle
	
Description:
	Run function for needle service in ES framework. Increments the needle
	angle with time. 
	
******************************************************************************/
ES_Event RunNeedle( ES_Event ThisEvent )
{
	ES_Event ReturnEvent; 
	ReturnEvent.EventType = ES_NO_EVENT; 
	NeedleState_t NextState = CurrentState;
	switch(CurrentState)
	{
		case(Waiting):
			if (ThisEvent.EventType == ES_MoveToRecord)
			{
                printf("\r\n Needle moving to record"); 
				MoveNeedle(RECORD_ANGLE);
				ES_Timer_InitTimer(NEEDLE_TIMER, MOVE_TO_RECORD_TIME); 
			}
			else if (ThisEvent.EventType == ES_TIMEOUT)
			{
                printf("\r\n Needle on record, incrementing now "); 
				MoveNeedle(CurrentAngle + AngleIncrement);
				ES_Timer_InitTimer(NEEDLE_TIMER, INC_TIME);  
				NextState = Moving;
			}
			break;
		case(Moving):
			if (ThisEvent.EventType == ES_TIMEOUT)
			{
                printf("\r\n incrementing needle...");
				MoveNeedle(CurrentAngle + AngleIncrement);
				ES_Timer_InitTimer(NEEDLE_TIMER, INC_TIME); 
			}
			else if (ThisEvent.EventType == ES_CelebrationComplete)
			{
				ES_Timer_StopTimer(NEEDLE_TIMER);
                printf("\r\n Needle moving to start position"); 
                MoveNeedle(START_ANGLE);
				NextState = Waiting;
			}
			break;
	} //end switch
	CurrentState = NextState;
	return ReturnEvent;
}


/* -------------------------- Private Functions ------------------------------*/


/******************************************************************************
Function:
    MoveNeedle
	
Description:	
	Moves needle to the requested angle
	
******************************************************************************/
static void MoveNeedle(float DesiredAngle)
{
	// Can add bool return to know if it is maxed out 
	if (DesiredAngle > END_ANGLE)
	{
		DesiredAngle = END_ANGLE;
	}
	// 11/20/17 -- offset - desired angle to get right direction
    ServoWriteAngle(OFFSET - DesiredAngle, NEEDLE_CHANNEL); 
	CurrentAngle = DesiredAngle;
}


/******************************* END OF SOURCE ******************************/
