/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------
File:
    ButtonService.c

Description:
    Detects and reports button down event. Written to be used in the
    Events & Services framework.

Author:
    Erez Krimsky

Date:
    11/13/17 EK: Edited to work with rest of framework, needs testing

*****************************************************************************/
// #define TEST

/* Other Services */
#include "ButtonService.h"
#include "YeezyService.h"

/*----------------------------- Defines ------------------------------------*/
#define INPUT_PORT_BASE GPIO_PORTB_BASE // using port B for button
#define BUTTON_PIN BIT3HI
#define DEBOUNCE_TIME 50 // 50 msec for debouncing 

// Assuming an input pulldown button configuration 
#define BUTTON_DOWN true

/*--------------------------- Module Variables -----------------------------*/
static uint8_t MyPriority;
static bool LastButtonState;
typedef enum { Debouncing, Ready2Sample
            	} ButtonState_t ;
static ButtonState_t CurrentState; 

/*------------------------ Private Function Prototypes ---------------------*/
static bool ReadButton(void); 
		
/* -------------------------- Public Functions -----------------------------*/
                
/*****************************************************************************
Function:
    InitButtonService
	
Description:
    Initializes hardware pins for button

Author:
    EK
******************************************************************************/
bool InitButton( uint8_t Priority)
{

    bool returnValue = false;
    MyPriority = Priority; // assign to static variable 

    // Will use port B, bit 3 
    HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R1; // PORT B
    while ((HWREG(SYSCTL_PRGPIO) & SYSCTL_PRGPIO_R1) != SYSCTL_PRGPIO_R1)
        ;
    HWREG(INPUT_PORT_BASE+GPIO_O_DEN) |= BUTTON_PIN; // Digital Enable
    HWREG(INPUT_PORT_BASE+GPIO_O_DIR) &= ~BUTTON_PIN; // Set output (clear bit)

    // Read the state of the input 
    LastButtonState = ReadButton(); 
    
    // Initialize button into debouncing state, could be pressed at startup
    CurrentState = Debouncing;

    // If debounce timer is set correctly
    if (ES_Timer_InitTimer(DEBOUNCE_TIMER, DEBOUNCE_TIME) == ES_Timer_OK)
    {
        returnValue = true; 
    }
	return returnValue; 
}


/******************************************************************************
Function:
    Post function for this service in the ES framework

Description:
    Post method for ES framework
******************************************************************************/
bool PostButton( ES_Event ThisEvent )
{
    return ES_PostToService( MyPriority, ThisEvent);
}


/******************************************************************************
Function:
    RunButtonService

Description:
    Runs the button 2-state, state machine 

Author:
    EK
******************************************************************************/
ES_Event RunButton( ES_Event ThisEvent )
{
    ES_Event ReturnEvent;
    ReturnEvent.EventType = ES_NO_EVENT; // assume no errors	
    ButtonState_t NextState = CurrentState; 
    switch(CurrentState)
    {
        case Debouncing :
        	/* if we are done debouncing */
			if ( (ThisEvent.EventType == ES_TIMEOUT) && 
				(ThisEvent.EventParam == DEBOUNCE_TIMER) ) 
			{
				NextState = Ready2Sample; 
			}
            break;
        case Ready2Sample :
            if ( ThisEvent.EventType == ES_ButtonDown)
        	{
        		ES_Timer_InitTimer(DEBOUNCE_TIMER, DEBOUNCE_TIME); 
        		NextState = Debouncing;
                ES_Event ButtonEvent;
                ButtonEvent.EventType = ES_ButtonDown;
                PostYeezy(ButtonEvent); 
        	}
            else if ( ThisEvent.EventType == ES_ButtonUp)
        	{
        		ES_Timer_InitTimer(DEBOUNCE_TIMER, DEBOUNCE_TIME); 
        		NextState = Debouncing;
        	}
        	break;     
    } // end switch
    CurrentState = NextState; 
    return ReturnEvent; 
}

/******************************************************************************
Function:
    CheckButtonEvents

Description:
    Event checker routine to look for changes in button state and post events
    if the state has changed and the button is down

Author:
    EK
******************************************************************************/
bool CheckButtonEvents(void)
{
	bool returnValue = false; // initialize to false
	bool CurrentButtonState = ReadButton(); // read the state
	if (CurrentButtonState != LastButtonState) // check for change 
	{	
        returnValue = true;
		if (CurrentButtonState == BUTTON_DOWN) //Check with defs 
		{
            ES_Event ButtonEvent;
            ButtonEvent.EventType = ES_ButtonDown;
            PostButton(ButtonEvent);  
		} 
        else 
        {
            ES_Event ButtonEvent;
            ButtonEvent.EventType = ES_ButtonUp;
            PostButton(ButtonEvent);           
        }
        // Could add implementation for button up if wanted 
	}
	// Set last button state for next function call
	LastButtonState = CurrentButtonState; // store away the last state 
	return returnValue; 
}

/*****************************************************************************/
/***********************       Private Functions      ************************/
/*****************************************************************************/

/******************************************************************************
Function:
    ReadButton
******************************************************************************/
static bool ReadButton(void)
{
    bool returnValue = false; 
    if ( (HWREG(INPUT_PORT_BASE+(GPIO_O_DATA + ALL_BITS)) & BUTTON_PIN) != 0)
    {
        returnValue = true;
    }
    return returnValue; 
}

/***************************** END SOURCE *********************************/

/* --------------------------- Test Harness --------------------------------*/
#ifdef TEST
#include "termio.h"
int main(void)
{
    TERMIO_Init();
    puts("\r\n In test harness for ButtonService\r\n");
    InitButtonService(0); // Incorporate into framework
    while(!kbhit())
    {
        if (CheckButtonEvents())
        {
            printf("\n\rButton Event!")
        } 
    }
    return 0;
}
#endif 
