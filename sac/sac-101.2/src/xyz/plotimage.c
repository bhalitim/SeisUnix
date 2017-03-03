#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysym.h>
#include <X11/Xresource.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "complex.h"
#include "proto.h"
#include <string.h>
#include "mach.h"
#include "gtm.h"
#include "contouring.h"
#include "gdm.h"


void plotimage ( array , nxsize , xmin , xmax , nysize , ymin , ymax ,
		 jxstart , jxstop , jystart , jystop , image_width ,
		 image_height , w_width , w_height , xstartloc , ystartloc ,
		 cbarxoffset , cbaryoffset , ypmax , lbinary , lcbar , nerr )

float array[];     /* image data to plot--dimensioned nxsize by nysize */
long int nxsize;   /* number of points in the x direction */
float xmin, xmax;  /* x data starting and ending values */
long int  nysize;  /* number of points in the y direction */
float ymin, ymax;  /* y data starting and ending values */
long int jxstart, jxstop, jystart, jystop;   /* data window specifications */
unsigned int image_width, image_height;      /* size in pixels of the image */
unsigned int w_width, w_height;  /* size of the window in pixels */

/* the location of the upper left corner of the image on the window */
unsigned int xstartloc, ystartloc, cbarxoffset,cbaryoffset; 

float ypmax;
long lbinary;
long lcbar;
long int *nerr;
{

	float dmax, dmin, range;
	float wxmin, wxmax, wymin, wymax;
	float dx, dy;
	unsigned int width, height;
	long idx;
	float *wdata, *plot_data;
	long pdata_alloc = FALSE;
	long wdata_alloc = FALSE;
	char **colorbar_data, **image_data;


	/*=====================================================================
	 * PURPOSE:  To do an image plot with limited options.
	 *           Need to add options to allow user to specify size of
         *           image.  Default is the size of the raw data.
	 *=====================================================================
	 * INPUT ARGUMENTS:
	 *      array:  Two-dimensional array of data. [fa]  
	 *     nxsize:  Number of elements in the x (horizontal) direction. [i]
	 *     nysize:  Number of elements in the y (vertical) direction. [i]
	 *=====================================================================
	 * OUTPUT ARGUMENTS:
	 *    nerr:    Error return flag.  Set to 0 if no error occurred. [i]
	 *=====================================================================
	 * MODULE/LEVEL: contouring/5
	 *=====================================================================
	 * GLOBAL INPUT:
	 *    gtm:         xvpmin, xvpmax, yvpmin, yvpmax
	 *=====================================================================
	 * SUBROUTINES CALLED:
	 *     saclib:  move, draw, setcolorname
	 *=====================================================================
	 * MODIFICATION HISTORY:
	 *=====================================================================
	 * DOCUMENTED/REVIEWED:
	 ==*=================================================================== */
	/* PROCEDURE: */
	*nerr = 0;

        width = jxstop - jxstart + 1;
        height= jystop - jystart + 1;

        dx = (xmax - xmin) / (float)(nxsize - 1);
        dy = (ymax - ymin) / (float)(nysize - 1);

        wxmin = (jxstart == 1) ? xmin: xmin + ((float)(jxstart-1)*dx); 
        wxmax = (jxstop == nxsize) ? xmax: xmax - ((float)(nxsize - jxstop)*dx);
        wymin = (jystart == 1) ? ymin: ymin + ((float)(jystart-1)*dy);
        wymax = (jystop == nysize) ? ymax: ymax - ((float)(nysize - jystop)*dy);

 
        /* if image needs to be resized, do it here, interpolating to the
	   desired size.  image may need to be resized because either: 
	   the user asked for it, or it won't fit in the plotting window. */


        /* make a copy of the input data that is cut to the requested window */

	if( (jxstart != 1) || (jxstop != nxsize) ||
            (jystart != 1) || (jystop != nysize)){

	    if((wdata = (float *)malloc(width*height*sizeof(float))) == NULL){
		printf("error getting memory for windowed data--plotimage\n");
		*nerr = 0301;
		goto L_8888;
	    }
	    wdata_alloc = TRUE;
	    window_data ( array , nxsize , nysize , wdata ,
			  jxstart , jxstop , jystart , jystop ) ;
	}
	else{
	    wdata = array;
	    wdata_alloc = FALSE;
	}

       /* dmax = -VLARGE;
        dmin =  VLARGE;
        for(idx = 0; idx < height*width; idx++){
	    if( wdata[idx] < dmin ) dmin = wdata[idx];
	    if( wdata[idx] > dmax ) dmax = wdata[idx];          
        } */      

	if ( ( image_height * image_width ) != ( height * width ) ) {
	    plot_data = (float *)malloc(image_height*image_width*sizeof(float));
	    if ( plot_data == NULL ) {
		printf("error getting memory for image height resize--plotimage\n");
		*nerr = 0301;
		goto L_8888;
	    }
	    pdata_alloc = TRUE;
	    scaleimage(wdata, width, height, plot_data, image_width,
		       image_height, wxmin, wxmax, wymin, wymax, nerr);
	    if(*nerr != 0) {
		printf("error scaling image\n");
		goto L_8888;
	    }
	} else {
	    plot_data = wdata;
	    pdata_alloc = FALSE;
	}


	if(lbinary){
	    for(idx=0; idx<image_width*image_height; idx++)
		plot_data[idx] = plot_data[idx] > 0.0 ? 1.0: 0.0;
	}

	/* find the max and min */
	dmax = -VLARGE;
	dmin =  VLARGE;

	for (idx=0; idx< (image_height*image_width); idx++){
	    if( plot_data[idx] < dmin ) dmin = plot_data[idx];
	    if( plot_data[idx] > dmax ) dmax = plot_data[idx];          
	}

        range = dmax - dmin;

	if(lcbar){
	    /*  Color bar  fill an entire scan line with one color */
	    /*  fill_colorbar needs to allocate the memory for the colorbar,
		due to the fact that the sgf needs three bytes for each
		element, where X only needs one. */

	    colorbar_data = fill_colorbar ( cmgdm.npscimage , (long) 20 ,
					    cmgdm.nctsize , (long) 7 , nerr ) ;

	    /*  Plot the colorbar image */
	    put_image ( colorbar_data , xstartloc - cbarxoffset ,
			ystartloc + cbaryoffset , (unsigned int) 20 ,
			(unsigned int) cmgdm.npscimage , nerr ) ;

	    label_cbar ( xstartloc - cbarxoffset , ystartloc + cbaryoffset ,
			(unsigned int) 20 , (unsigned int) cmgdm.npscimage ,
			w_width , w_height , dmin , dmax , ypmax , nerr ) ;
        }


	/*  Fill the image data array */
	/*  fill_image needs to allocate the memory for the image, due to the
	    differences in sgf versus X images.   */

	image_data = fill_image ( image_height , image_width , plot_data ,
				  dmin , range , cmgdm.npscimage ,
				  cmgdm.nctsize , (long) 7 , nerr ) ;

	/*  Plot the data image */
	put_image(image_data,xstartloc,ystartloc,image_width,image_height,nerr);

L_8888:
	/* release temporary arrays */

	if(pdata_alloc)free(plot_data);
	if(wdata_alloc)free(wdata);

	return;

} /* end of function */






