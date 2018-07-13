/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------
 File
    Servo.c

 Description
   This module contains functions to write a servo to an angle.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 11/13/17 18:03 RK     first pass
 11/14/17       RK     finished and tested.  Edited servo constants, 
                       fixed conversion.
									
****************************************************************************/

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

// Header File
#include "Servo.h"

//for testing
#include "termio.h"


/* ------ #Defines ----------- */
#define SERVO_0_DEG_TICKS 1000
#define SERVO_MAX_ANGLE 180
#define SERVO_TICK_PER_ANGLE 8
#define TICK_US_SCALE 1.25f // 1 tick / 0.8 seconds = 1.25


// depending on the actual button configuration


/* --------------------------- Module Variables -----------------------------*/
/* ------------------------ Private Function Prototypes ---------------------*/
static uint16_t servoAngle2PWM(float newAngle); 
		
							
/****************************************************************************
Function:
   ServoWriteAngle
    
Parameters:
    uint16_t NewAngle, uint16_t channel

Description:
   Writes angle to the the servo by converting to a pwm.
   Returns false if newAngle is greater than max servo angle.

Author:
    RK
****************************************************************************/
bool ServoWriteAngle(float  newAngle, uint16_t whichChannel)
{
	uint16_t PWMout;
    
	PWMout = servoAngle2PWM(newAngle);
	PWM_TIVA_SetPulseWidth(PWMout, whichChannel);
    if (newAngle > SERVO_MAX_ANGLE){
        return false;
    } else {
        return true;
    }
    

   // printf("angle write called \r\n");
}

/***************************************************************************
 private functions
 ***************************************************************************/

/****************************************************************************
 Function
     servoAngle2PWM 

 Parameters
     uint8_t, angle from 0 to 180 degrees

 Returns
     uint8_t, pulse width from 1 to 2 ms

 Description
     Takes angles and converts them into pulsewidth for the servo to use.
		 PWM Out = servo ticks/angle (i.e. slope) * new angle + offset from 0 
     degrees. 
     For newAngle > servo max angle, outputs pwm for servo max angle.
 
 Notes
		Taken empirically from HS311 Servo. Max angle is  180 degrees.
    Note 1 servo tick is 0.8us, so the pulsewidth output for 1250 ticks is 1ms.

****************************************************************************/
static uint16_t servoAngle2PWM(float newAngle)
{
	uint16_t PWticks;
    if (newAngle <= SERVO_MAX_ANGLE){ // if angle is within range for servo
        PWticks = (TICK_US_SCALE*(SERVO_TICK_PER_ANGLE*newAngle)) + 
                                                         SERVO_0_DEG_TICKS;
    } 
    else 
    { // if angle is greater than max value
        PWticks = (TICK_US_SCALE*(SERVO_TICK_PER_ANGLE*
                                         SERVO_MAX_ANGLE)) + SERVO_0_DEG_TICKS;        
    }
    return PWticks;
}
/*------------------------------ Test Harness ------------------------------*/

#ifdef TEST
	int main(void) {
		_HW_Timer_Init(ES_Timer_RATE_1mS);
		TERMIO_Init();
		clrScrn();
		printf("\r\nStarting Servo Test Harness\r\n");
		if(PWM_TIVA_Init(0) != true){
    printf("Failed PWM_TIVA_Init(%d)\n\r",NUM_CHAN2_TEST);
  }else{
    printf("Passed PWM_TIVA_Init(%d)\n\r",NUM_CHAN2_TEST);
  }
	
	while (true) {
		for (int i = 0; i <= 160; i++) {
			servoWriteAngle(i,0)
			printf("Motor Angle ON = %i", i);

			}
		}
	}
	
#endif

/*------------------------------ End of file ------------------------------*/
