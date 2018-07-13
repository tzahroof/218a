/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------

Header File: 
	Record.h 

Description:
	For starting and stopping the motor that controls the "record" that spins
	in our project. Only 1 directional control is required (no H-bridge).
	Speed control of the DC motor will be performed with PWM control on a 
	power mosfet

Notes/Warnings:
	

Revision History:	
	11/12/17 	EK: Started Coding
******************************************************************************/
#ifndef Record_H
#define Record_H

// the common headers for C99 types 
#include <stdint.h>
#include <stdbool.h>


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

// Need acces to PWM library 
#include "PWM16Tiva.h"

#define ALL_BITS (0xff<<2)
#define NOT_USED 0x00

void RecordInit(void); // Initialize motor hardware 
void RecordStart(void); // Start the record
void RecordStop(void);  // stop the record

#endif // Record_H
