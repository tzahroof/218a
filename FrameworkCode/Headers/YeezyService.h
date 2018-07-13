/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------

Header File: 
	YeezyService.h 

Description:
	Yeezy service header file corresponding to YeezyService.c which controls
	general high level operation of the yeezy device.

Revision History:	
	11/13/17 EK: Started coding 
	12/04/17 EK: Cleaned up 

******************************************************************************/
#ifndef YeezyService_H
#define YeezyService_H

// the common headers for C99 types 
#include <stdint.h>
#include <stdbool.h>

// Framework Files
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"
#include "ES_ShortTimer.h"

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

bool InitYeezy ( uint8_t Priority );
bool PostYeezy( ES_Event ThisEvent );
ES_Event RunYeezy( ES_Event ThisEvent );

#endif // YeezyService_H
