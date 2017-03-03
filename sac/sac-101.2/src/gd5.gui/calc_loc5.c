#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <X11/Xresource.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../inc/complex.h"
#include "../../inc/proto.h"
#include <string.h>
#include "../../inc/mach.h"
#include "../../inc/gd5.gui.h"


void calc_loc5(xloc,yloc,cbarxoffset,cbaryoffset,w_width,w_height,xpmn,xpmx,xmin,first,last,ypmn,ypdel,nerr)
unsigned int *xloc, *yloc, *cbarxoffset, *cbaryoffset;
unsigned int w_width, w_height;
float xpmn,xpmx,xmin,first,last,ypmn,ypdel;
long int *nerr;
{
      *nerr = 0;

      *xloc = (xpmn + (((xmin-first)/(last-first))*(xpmx-xpmn)))*(float)w_width;
      *yloc = (ypdel * (float)w_height)+ypmn*(float)w_height;
      *cbarxoffset = 45;
      *cbaryoffset = 0;

L_8888:
	return ;

} /* end of function */






