/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------

Header File: 
	Needle.h 

Description:
	Module for controlling the position of the record needle. 

Notes/Warnings:
	None

Revision History:	
	11/13/17 EK: Started coding
******************************************************************************/

#ifndef NEEDLE_H
#define NEEDLE_H

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

// Framework includes
#include "ES_Configure.h"
#include "ES_Events.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"
#include "ES_ShortTimer.h"
#include "ES_Port.h"

// Need acces to PWM library 
#include "PWM16Tiva.h"

// Public Function Prototypes 
bool InitNeedle( uint8_t Priority );
bool PostNeedle( ES_Event ThisEvent );
ES_Event RunNeedle( ES_Event ThisEvent );

#endif // NEEDLE_H
