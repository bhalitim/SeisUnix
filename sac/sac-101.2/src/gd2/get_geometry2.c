/*******************************************************************************
** PURPOSE:
*    To get the geometry of the SGF device.
*
** INPUT ARGUMENTS:
*  
*  
*
** GLOBAL INPUT:
*    gd2.h:  XW, YW
*              
*
** GLOBAL OUTPUT:
*  
*
** SUBROUTINES CALLED:
*  
*
** LOCAL VARIABLES:
*  
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../inc/complex.h"
#include "../../inc/proto.h"
#include <string.h>
#include "../../inc/mach.h"
#include "../../inc/gd2.h"


void get_geometry2( width_return, height_return, nerr)
unsigned int *width_return, *height_return;
long *nerr;
{

    *nerr = 0;
    *width_return = (long)XW;
    *height_return =(long)YW;

}

/*******************************************************************************
** MODIFICATION HISTORY:
*
*
*    940823:  Original Version
*******************************************************************************/
