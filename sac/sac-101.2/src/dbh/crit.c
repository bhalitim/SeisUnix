/** 
 * @file   crit.c
 * 
 * @brief  Criterion for picking mem order.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "complex.h"
#include "proto.h"

/** 
 * Criterion for picking mem order.  Produces graph of normalized
 *   rms prediction error as a function of prediction order.
 * 
 * @param r 
 *    Input vector of correlation coefficients
 * @param n 
 *    Length of vector \p r
 * @param x 
 *    Output vector of rms prediction error. Max 100
 *
 * @return Nothing
 *
 * \author  David Harris
 *
 * \date 900807:  Last Modified
 * 
 */

void 
crit(float     r[], 
     long int  n, 
     float     x[])
{
	long int i, i_, nr;
	float a[100], reflct[100];

	float *const A = &a[0] - 1;
	float *const R = &r[0] - 1;
	float *const Reflct = &reflct[0] - 1;
	float *const X = &x[0] - 1;

	zero( x, 100 );
	if( n > 100 ){
		nr = 100;
		}
	else{
		nr = n;
		}

	/*  Levinson's recursion to obtain reflection coefficients
	 * */
	levin( r, a, reflct, nr );

	/*  Recursion to compute normalized error
	 * */
	X[1] = 1.;
	for( i = 2; i <= nr; i++ ){
		i_ = i - 1;
		X[i] = X[i - 1]*sqrt( 1. - powi(Reflct[i - 1],2) );
		/*             I */
		}

	return;
}

