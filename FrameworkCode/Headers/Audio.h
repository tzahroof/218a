/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------

Header File: 
	Audio.h 

Description:
	Audio service (plays music) and also checks for if the user has put their
	hands up to block the IR sensors.

Notes/Warnings:


Revision History:	
	11/13/17 EK: Started coding 

******************************************************************************/
#ifndef Audio_H
#define Audio_H

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

bool InitAudio ( uint8_t Priority );
bool PostAudio( ES_Event ThisEvent );
ES_Event RunAudio( ES_Event ThisEvent );
bool CheckHandsUp(void);

#endif // Audio.h
