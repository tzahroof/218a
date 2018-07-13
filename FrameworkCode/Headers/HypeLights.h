/*****************************************************************************
ME 218A - Team 5 
Stanford University, Department of Mechanical Engineering
Fall 2017
------------------------------------------------------------------------------

Header File: 
	HypeLights.h 

Description:
	HypeLights header file corresponding to HypeLights.c which controls the
	behavior of the hype lights (i.e. led strip lights)

Revision History:	
	12/04/17 EK: Cleaned up 
******************************************************************************/

#ifndef HypeLights_H
#define HypeLights_H

// Event Definitions
#include "ES_Configure.h" /* gets us event definitions */
#include "ES_Types.h"     /* gets bool type for returns */

// typedefs for the states
// State definitions for use with the query function
typedef enum { Off, Hype1, Hype2, Hype3, CellyLights } HypeState_t ;

// Public Function Prototypes
bool InitHypeLights ( uint8_t Priority );
bool PostHypeLights( ES_Event ThisEvent );
ES_Event RunHypeLights( ES_Event ThisEvent );

#endif /* HypeLights_H */
