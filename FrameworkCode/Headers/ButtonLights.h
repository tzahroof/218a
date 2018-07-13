/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------

Header File: 
	ButtonLights.h 

Description:
	Used for turning on/off lights in the start button 

Notes/Warnings:

Revision History:	
	11/14/17 EK: Started coding 
******************************************************************************/
#ifndef ButtonLights_H
#define ButtonLights_H

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

void ButtonLightsInit (void);
void ButtonLights( bool newState ); // true = lights on, false = lights off 

#endif // ButtonLights_H
