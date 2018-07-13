/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------

Header File: 
	ShiftRegisterWrite.h 

Description:
	For interfacing 74HCT595 with Tiva Launchpad Microcontroller

Notes/Warnings:
	There is support for using 4 shift registers. Hardware definitions need
	to be filled in the 5 corresponding arrays in the source (.c) file

Revision History:	
	11/08/17 EK: Adding modification for multiple register, no testing yet
    11/10/17 EK: Code tested
    12/04/17 EK: Cleaned up
******************************************************************************/
#ifndef ShiftRegisterWrite_H
#define ShiftRegisterWrite_H

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

#define NOT_USED 0x00

void SR_Init(uint8_t whichRegister); // select which register to init
void SR_Write(uint8_t NewValue, uint8_t whichRegister); 
void SR_Write16(uint16_t NewValue, uint8_t whichRegister); //write 2 bytes

#endif // ShiftRegisterWrite_H
