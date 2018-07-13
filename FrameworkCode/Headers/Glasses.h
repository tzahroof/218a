/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------

Header File: 
	Glasses.h 

Description:
	For interfacing with servo controlled glasses

Notes/Warnings:


Revision History:	
   11/12/17 RK: Code written not tested.
   12/04/17 EK: Cleaned up 
******************************************************************************/
#ifndef Glasses_H
#define Glasses_H

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

// the headers to access the Events and Services Framework
#include "ES_Configure.h"
#include "ES_Events.h"

#define NOT_USED 0x00

bool InitGlasses( uint8_t Priority );
ES_Event RunGlasses( ES_Event ThisEvent );
void PostGlasses( ES_Event ThisEvent );
bool Check4GlassesEvent( void );
void FaderLights( uint8_t WhichLights );

#endif // Glasses_H
