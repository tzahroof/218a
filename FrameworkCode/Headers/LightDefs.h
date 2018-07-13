/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------

Header File: 
	LightDefs.h

Description:
	Defines hex codes corresponding to the following binary numbers:
	LIGHT_0 = (b) 00000000
	LIGHT_1 = (b) 00000011
	LIGHT_2 = (b) 00000111
	LIGHT_3 = (b) 00001111
	And so on...
	This is used to control the fader lights in a convenient way.

Revision History:	
	11/20/17 (?) EK: Started coding 
	12/04/17     EK: Cleaned up 
******************************************************************************/
#ifndef LIGHT_DEFS_H
#define LIGHT_DEFS_H

#define LIGHT_0 0x00
#define LIGHT_1 0x03
#define LIGHT_2 0x07
#define LIGHT_3 0x0f
#define LIGHT_4 0x1f
#define LIGHT_5 0x3f
#define LIGHT_6 0x7f
#define LIGHT_7 0xff
#define NUMBER_LIGHTS 0x07 // There are 7 fader lights

#endif // LIGHT_DEFS_H

