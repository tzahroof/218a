/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------

Header File: 
	Servo.h 

Description:
	Header file for user defined servo function in Servo.c 

Revision History:	
	11/10/17 Rohun: Wrote code 
	12/04/17 EK: Cleaned up 
******************************************************************************/


#ifndef Servo_H
#define Servo_H

// the common headers for C99 types
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

bool ServoWriteAngle(float newAngle, uint16_t Channel);

#endif // Servo_H
