#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../inc/complex.h"
#include "../../inc/proto.h"

void /*FUNCTION*/ idttmf(ndttm1, nsec, nmsec, ndttm2)
long int ndttm1[];
long int nsec, nmsec ;
long int ndttm2[];
{
	long int nexday;

	long *const Ndttm1 = &ndttm1[0] - 1;
	long *const Ndttm2 = &ndttm2[0] - 1;


	/*=====================================================================
	 * PURPOSE:
	 *=====================================================================
	 * INPUT ARGUMENTS:
	 *=====================================================================
	 * OUTPUT ARGUMENTS:
	 *    NERR:    Error flag. Set to 0 if no error occurred.
	 *             Potential error numbers:
	 *=====================================================================
	 * MODULE/LEVEL:  SERVICE/4
	 *=====================================================================
	 * GLOBAL INPUT:
	 *=====================================================================
	 * GLOBAL OUTPUT:
	 *=====================================================================
	 * GLOBAL COUPLING:
	 *=====================================================================
	 * SUBROUTINES CALLED:
	 *=====================================================================
	 * LOCAL VARIABLES:
	 *=====================================================================
	 * ASSUMPTIONS:
	 *=====================================================================
	 * LIMITATIONS:
	 *=====================================================================
	 * KNOWN ERRORS:
	 *=====================================================================
	 * EXTERNAL DEPENDENCIES:
	 *===================================================================== */
	/* PROCEDURE: */
	inctimf( Ndttm1[3], Ndttm1[4], Ndttm1[5], Ndttm1[6], nsec, nmsec, &Ndttm2[3], 
	 &Ndttm2[4], &Ndttm2[5], &Ndttm2[6], &nexday );
	incdat( Ndttm1[1], Ndttm1[2], nexday, &Ndttm2[1], &Ndttm2[2] );

L_8888:
	return;

	/*=====================================================================
	 * MODIFICATION HISTORY:
	 *    810000:  Original version.
	 *===================================================================== */

} /* end of function */

