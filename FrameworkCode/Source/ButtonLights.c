/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------
 Module
   ButtonLights.c

 Description
   'Central' service for Yeezy machine.



 History
 When          		Who     What/Why
 -------------- 	---     --------
11/14/17			EK      Started Coding               
 
****************************************************************************/

#include "ButtonLights.h"
#include <stdio.h>
#define BUTTON_LIGHT_PIN BIT5HI

/****************************************************************************
 Function
     ButtonLightsInit

 Parameters
     none

 Returns
     none

 Description
     Initializes hardware pins for the start button lights 

 Notes

 Author
     E. Krimsky, 11/14/17
****************************************************************************/
void ButtonLightsInit(void)
{
	// Will use port A, bit 5
	if ((HWREG(SYSCTL_PRGPIO) & SYSCTL_PRGPIO_R0) != SYSCTL_PRGPIO_R0)
	{
    	HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R0; // PORT B
    	while ((HWREG(SYSCTL_PRGPIO) & SYSCTL_PRGPIO_R0) != SYSCTL_PRGPIO_R0)
        ;
	}
    
    HWREG(GPIO_PORTA_BASE+GPIO_O_DEN) |= BUTTON_LIGHT_PIN; // Digital Enable
    HWREG(GPIO_PORTA_BASE+GPIO_O_DIR) |= BUTTON_LIGHT_PIN; // Set output 

    // start with the pin set low 
    HWREG(GPIO_PORTA_BASE+(GPIO_O_DATA + ALL_BITS)) &= ~BUTTON_LIGHT_PIN;
}

/****************************************************************************
 Function
     ButtonLights

 Parameters
     bool, if true turn lights on, if false, turn lights off 

 Returns
     none

     
 Notes

 Author
     E. Krimsky, 11/14/17
****************************************************************************/
void ButtonLights(bool newState)
{
	if (newState == true)
	{
        //printf("\r\n Button lights on");
		HWREG(GPIO_PORTA_BASE+(GPIO_O_DATA + ALL_BITS)) |= BUTTON_LIGHT_PIN;
	} 
	else
	{
        //printf("\r\n Button lights off");
		HWREG(GPIO_PORTA_BASE+(GPIO_O_DATA + ALL_BITS)) &= ~BUTTON_LIGHT_PIN;
	}	
}

/*********************** END OF SOURCE *************************************/
