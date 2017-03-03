#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "complex.h"
#include "proto.h"
#include <string.h>
#define	IPULSE	100

#include "mach.h"
#include "mem.h"
#include "sam.h"
#include "dfir.h"
void /*FUNCTION*/ fdhp(memptr, mxmptr, userData, nerr)
long int memptr[], mxmptr, *nerr;
float *userData ;
{
	long int idx, jdx;
	float rfl, rfh, dummy ;
        char s1[3];


	long *const Memptr = &memptr[0] - 1;


	/*=====================================================================
	 * PURPOSE:  Creates filter response files in memory by evaluating
	 *           filter command line parameters for highpass.
	 *=====================================================================
	 * OUTPUT ARGUMENTS:
	 *      NERR:  Error return flag
	 *=====================================================================
	 * MODULE/LEVEL:  SAM/2
	 *=====================================================================
	 * GLOBAL INPUT:
	 *    MACH:
	 *    SAM:     KTPIIR, MTPIIR, KPROTYP, MPROTYP, NDATPTS, NIMPPTS 
	 *             FDDELTA
	 *=====================================================================
	 * GLOBAL OUTPUT:
	 *    SAM:     CFHP, ITPHP, NPOLHP, NPASHP, TBWHP, ATNHP
	 *=====================================================================
	 * GLOBAL COUPLING:
	 *=====================================================================
	 * SUBROUTINES CALLED:
	 *    SACLIB:  ALLAMB, INSPECT, DESIGN, ZERO, APPLY
	 *=====================================================================
	 * LOCAL VARIABLES:
	 *  mxmptr:  Number of entries in memptr. [i]
	 *  dummy:   Dummy array for passing to inspect with digital data. [r]
	 *  ipulse:  Array location of the impulse response onset. [i]
	 *=====================================================================
	 * GLOBAL VARIABLES:
	 *  dfir:
	 *         nsects:  Number of second order sections in impulse. [i]
	 *         sn:       Numerator coefficients. [r]
	 *         sd:       Denominator coefficients. [r]
	 *         filttype: Filter type. [character*2]
	 *=====================================================================
	 * MODIFICATION HISTORY:
	 *  901219 Added included file dfir for saving coefficients, nsects, and
	 *         filter type.
	 *  901116 Added delta option for selecting sampling interval.
	 *  901001 Original version.
	 *===================================================================== */
	/*     PROCEDURE: */
	*nerr = 0;
	strcpy( cmfir3.filttype, "HP" );
        strcpy( s1, "  ");

	/* - Loop on each token in command: */

	while ( lcmore( nerr ) ){
	    /* -- CORNER v:  define new corner frequency. */
	    if( lkrrc( "CORNER$",8, 0., VLARGE, &cmsam.cfhp ) )
	    { /* do nothing */ }

	    /* -- BU/BE/C1/C2:  change type of IIR filter to perform. */
	    else if( lclist( (char*)kmsam.ktpiir,9, MTPIIR, &cmsam.itphp ) )
	    { /* do nothing */ }

	    /* -- NPOLES n:  define npoles of poles in filter. */
	    else if( lkirc( "NPOLES$",8, 1, 10, &cmsam.npolhp ) )
	    { /* do nothing */ }

	    /* -- TRANBW v:  define new transition bandwidth. */
	    else if( lkrrc( "TRANBW$",8, 0., VLARGE, &cmsam.tbwhp ) )
	    { /* do nothing */ }

	    /* -- ATTEN v:  define new filter attenuation factor. */
	    else if( lkrrc( "ATTEN$",7, 1., VLARGE, &cmsam.atnhp ) )
	    { /* do nothing */ }

	    /* -- PASSES n:  Set number of filter passes. */
	    else if( lkirc( "PASSES$",8, 1, 2, &cmsam.npashp ) )
	    { /* do nothing */ }

	    /* -- DELTA v:  Set the delta. */
	    else if( lkrrc( "D#ELTA$",8, VSMALL, VLARGE, &cmsam.fddelta ) )
	    { /* do nothing */ }

	    /* -- Bad syntax. */
	    else{
		cfmt( "ILLEGAL OPTION:$",17 );
		cresp();
	    }
	}

	/* - The above loop is over when one of two conditions has been met:
	 *   (1) An error in parsing has occurred.  In this case NERR is > 0 .
	 *   (2) All the tokens in the command have been successfully parsed. */

	if( *nerr != 0 )
	    goto L_8888;

        /* set userData */
        userData[ 0 ] = 2 ;
        userData[ 1 ] = cmsam.itphp ;
        userData[ 2 ] = cmsam.npolhp ;
        userData[ 3 ] = cmsam.npashp ;
        userData[ 4 ] = cmsam.tbwhp ;
        userData[ 5 ] = cmsam.atnhp ;
        userData[ 6 ] = cmsam.fddelta ;
        userData[ 7 ] = cmsam.cfhp ;
        userData[ 8 ] = -12345.0 ;


	/* - Compute each filter response prototype and put result in memory. */

	/* -- Allocate and load x and y data arrays for analog signals.
	 *    This will be memptr(1 thru 6) */

	jdx = 1;
	for( idx = 0; idx < 3; idx++ ){
	    allamb( &cmmem, NDATPTS, &Memptr[jdx], nerr );
	    if( *nerr != 0 )
		goto L_8888;
	    allamb( &cmmem, NDATPTS, &Memptr[jdx + 1], nerr );
	    if( *nerr != 0 )
		goto L_8888;


            strncpy(s1,kmsam.ktpiir[cmsam.itphp - 1],2);

            rfh = 1./ (2.*cmsam.fddelta);
            rfl = .001*rfh;

	    inspect( cmsam.npolhp, "HP", s1, 
	     cmsam.atnhp, cmsam.tbwhp, cmsam.cfhp, 0.0, cmsam.fddelta, 
	     (char*)kmsam.kprotyp[idx], NDATPTS, &rfl, &rfh,
             "LINEAR", cmmem.sacmem[Memptr[jdx]], cmmem.sacmem[Memptr[jdx+1]] );
	    jdx = jdx + 2;
	}

	/* -- Allocate and load y data array only for digital signals.
	 *    This will be memptr(7 thru 9) */

	for( idx = 3; idx < MPROTYP; idx++ ){
	    allamb( &cmmem, NDATPTS, &Memptr[idx + 4], nerr );
	    if( *nerr != 0 )
		goto L_8888;

            strncpy(s1,kmsam.ktpiir[cmsam.itphp - 1],2);

            rfh = 1./ (2.*cmsam.fddelta);
            rfl = .001*rfh;

	    inspect( cmsam.npolhp, "HP", s1, 
	     cmsam.atnhp, cmsam.tbwhp, cmsam.cfhp, 0.0, cmsam.fddelta, 
	     (char*)kmsam.kprotyp[idx], NDATPTS, &rfl, &rfh,
             "LINEAR", cmmem.sacmem[Memptr[idx + 4]], (float*)&dummy );
	}

	/* - Create the impulse response */

	allamb( &cmmem, NIMPPTS, &Memptr[mxmptr], nerr );
	if( *nerr != 0 )
	    goto L_8888;

        strncpy(s1,kmsam.ktpiir[cmsam.itphp - 1],2);

	design( cmsam.npolhp, "HP", s1, 
	 cmsam.atnhp, cmsam.tbwhp, cmsam.cfhp, 0.0, cmsam.fddelta, cmfir3.sn, 
	 cmfir3.sd, &cmfir3.nsects );

	zero( cmmem.sacmem[Memptr[mxmptr]], NIMPPTS );
	*(cmmem.sacmem[Memptr[mxmptr]] + IPULSE - 1) = 1.;

	apply( cmmem.sacmem[Memptr[mxmptr]], NIMPPTS, FALSE,
		cmfir3.sn, cmfir3.sd, cmfir3.nsects );

L_8888:
	return;

} /* end of function */

