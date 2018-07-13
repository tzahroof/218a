/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------
 Module
   Record.c

 Description
   This module starts and stops the record in our project. 

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 11/13/17        EK       Started Coding 
 
****************************************************************************/
//#define TEST
#include "Record.h"


/*----------------------------- Defines ------------------------------------*/
#define MOTOR_PIN BIT6HI // (PWM Channel ten)
#define MOTOR_CHANNEL 10 // pwm chanel
#define MOTOR_GROUP 5 // the group this channel is in 


/*--------------------------- Module Variables -----------------------------*/
static uint8_t motorSpeed = 25; // duty cycle 
static uint16_t freq = 100; // Hz, frequency for PWM 


/* -------------------------- Public Functions -----------------------------*/

/****************************************************************************
Function:
    RecordInit
    
Parameters:
    None

Description:
	Initializes the output lines for the record motor. Sets PWM frequency

Author:
    EK
****************************************************************************/
void RecordInit()
{

    /* Enable eripheral clock, wait till its ready to set direction */
    HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R0; // check syntax
    while ((HWREG(SYSCTL_PRGPIO) & SYSCTL_RCGCGPIO_R0) != SYSCTL_RCGCGPIO_R0)
        ;

    /* Digital Enable */
    HWREG(GPIO_PORTA_BASE+GPIO_O_DEN) |= MOTOR_PIN;

    /* Set Pins as Outputs */
    HWREG(GPIO_PORTA_BASE+GPIO_O_DIR) |= MOTOR_PIN;
  
    // start with the pin set low 
    HWREG(GPIO_PORTA_BASE+(GPIO_O_DATA + ALL_BITS)) &= ~MOTOR_PIN;
    PWM_TIVA_Init(11); 
    PWM_TIVA_SetFreq(freq, MOTOR_GROUP);
    
}


/****************************************************************************
Function:
    RecordStart
    
Parameters:
    None

Description:
	Starts the record motor

Author:
    EK
****************************************************************************/
void RecordStart()
{
	// set the duty cylce to motorSpeed as defined above
	PWM_TIVA_SetDuty(motorSpeed, MOTOR_CHANNEL);
}

/****************************************************************************
Function:
    RecordStop
    
Parameters:
    None

Description:
	Stops the record motor

Author:
    EK
****************************************************************************/
void RecordStop()
{
	// Set duty cycle to zero -- makes the channel low 
	PWM_TIVA_SetDuty(0, MOTOR_CHANNEL);
}


/* -------------------------- END SOURCE CODE ----------------------------*/
#ifdef TEST
/* test Harness for testing this module */
#include "termio.h"
int main(void)
{
    TERMIO_Init();
    puts("\r\n In test harness for Record\r\n");
    puts("\r\n Hit Keyboard to stop motor\r\n");
    RecordInit();   // Init Hardware 
 
    RecordStart();
    while(!kbhit())
    {
    }
    RecordStop();
    puts("\r\n Keyboard hit \r\n");
    return 0;
}
#endif 
