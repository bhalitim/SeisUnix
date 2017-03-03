/** 
 * @file   xcorrelate.c
 * 
 * @brief   Compute Auto and Cross Correlation
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "complex.h"
#include "proto.h"

#define	MCORLN	4096
#define	MWINLN	2048

#include "mach.h"
#include "dfm.h"
#include "hdr.h"
#include "mem.h"
#include "sam.h"


/** 
 * PURPOSE: To parse and execute the action command CORRELATE.
 *          This command computes cross and auto-correlations.
 *
 * @param nerr
 *     Error Return flag
 *     - 0 on Success
 *     - Non-Zero on Error
 *
 * @note Local Variables:
 *   - MWINLN:  Maximum length of each data window. [ip]
 *   - MCORLN:  Maximum length of correlation function. [ip]
 *   - NDXMAS:  Index in SACMEM array for master signal. [i]
 *   - NDXSIG:  Index in SACMEM array for current signal. [i]
 *   - NDXCOR:  Index in SACMEM array for unshifted correlation. [i]
 *   - NDSFILES: Number of files in a given data-set. [i]
 *   - NDSFLNUM: File number (senquental) in the current data set. [i]
 *
 * @date    961204:  Updated to allow full range of correlation instead of 
 *             just the central half.  Also set the begin times so that
 *             the difference between the begin times of the given plot 
 *             and the master plot is preserved.  Also sets nzyear and 
 *             nzhour to undefined because an absolute time carries
 *             very little meaning for the correlation.  maf 961204
 * @date   920110:  Added DATA-SET update logic.
 * @date   870925:  Fixed bug when signals were of different length.
 *             Now output signals are all equal in length to the
 *               maximum length of the input signals.
 * @date   870312:  Added ability to choose master file by name.
 * @date   870209:  Converted to an internal command.
 * @date   830000:  Original XSC version.
 *
 * @date   870925:   DOCUMENTED/REVIEWED:  
 */
void 
xcorrelate(long int *nerr) {
	char kermsg[131], ktemp1[MCPFN+1];
	long int ic1, ic2, iwinln, iwinmx, j, j1, j2, j_, jdfl, jdfl_, 
	 ndxcor, ndxmas, ndxsig, ndxx, ndxy, 
	 nfft, notusd, nrerr, ntused, nzeros,
	 nlen,		/* npts of non-master signal */
	 nlenmx,	/* max npts of all signals */
	 nlenMaster,	/* npts of master */
	 nlenCombined;	/* nlen + nlenMaster - 1 */
	float masterBegin ;	/* begin time of master.  maf 961204 */

	*nerr = 0;

	/* - Loop on each token in command: */
	while ( lcmore( nerr ) ){

		/* -- "MASTER name|n":  determine which file to copy from. */
		if( lckey( "MASTER$",8 ) ){
			if( lcirc( 1, cmdfm.ndfl, &cmsam.imast ) )
			{ /* do nothing */ }
			else if( lcchar( MCPFN, ktemp1,MCPFN+1, &notusd ) ){
				ic1 = 0;
				ic2 = 0;
				for( jdfl = 1; jdfl <= cmdfm.ndfl; jdfl++ ){
					jdfl_ = jdfl - 1;
					if(lnxtcl( kmdfm.kdfl,MAXCHARS, &ic1, &ic2 )){
					    if( memcmp(ktemp1,kmdfm.kdfl+ic1 - 1,min(ic2,MAXCHARS-1) - 
					      ic1 + 1) == 0 ){
						  cmsam.imast = jdfl;
						  goto L_1200;
					    } /* end if( memcmp( ... */
				        } /* end if(lnxtcl( ... */
					ic1 = ic2 + 1;
				} /* end for ( jdfl ... */
				ictok( -1 );
				cfmt( "BAD FILE NAME:$",16 );
				cresp();
			} /* end else if( lcchar( ... */
			else{
				cfmt( "NEED A FILE NAME OR A NUMBER:$",31 );
				cresp();
			} /* end else */
L_1200:
			;
		} /* end if( lckey( "MASTER$",8 ) ) */

		/* -- "NUMBER n":  set number of windows. */
		else if( lkint( "NUMBER$",8, &cmsam.nwin ) )
		{ /* do nothing */ }

		/* -- "LENGTH ON|OFF|v":  set window length in seconds. */
		else if( lklogr( "LENGTH$",8, &cmsam.lwinln, &cmsam.winln ) ){
			if( cmsam.winln <= 0. )
				cmsam.lwinln = FALSE;
		}

		/* -- "TYPE char":  set window (taper) type. */
		else if( lklist( "TYPE$",6, (char*)kmsam.kwintp,9, MWINTP, 
		 &cmsam.iwintp ) )
		{ /* do nothing */ }

		/* -- Bad syntax. */
		else{
			cfmt( "ILLEGAL OPTION:$",17 );
			cresp();
		}

	} /* end while */

	/* - The above loop is over when one of two conditions has been met:
	 *   (1) An error in parsing has occurred.  In this case NERR is > 0 .
	 *   (2) All the tokens in the command have been successfully parsed. */

	if( *nerr != 0 )
		goto L_8888;

	/* CHECKING PHASE: */

	/* - Test for a non-null data file list. */

	vflist( nerr );
	if( *nerr != 0 )
		goto L_8888;

	/* - Make sure each file is an evenly spaced time series file. */

	vfeven( nerr );
	if( *nerr != 0 )
		goto L_8888;

	/* - Find longest signal. */

	nlenmx = 0;
        iwinmx = 0;
	for( jdfl = 1; jdfl <= cmdfm.ndfl; jdfl++ ){
		jdfl_ = jdfl - 1;
		getfil( jdfl, FALSE, &ntused, &ntused, &ntused, nerr );
		if( *nerr != 0 )
			goto L_8888;
		if( cmsam.lwinln ){
			iwinln = (long)( cmsam.winln/ *delta + 0.1 );
		}
		else{
			iwinln = *npts/cmsam.nwin;
		}

		nlenmx = max( nlenmx, *npts );
                iwinmx = max( iwinmx, iwinln);
	} /* end for */

	/* - EXECUTION PHASE: */

	/* - Allocate temporary blocks for the master signal and correlation function. */

	allamb( &cmmem, nlenmx, &ndxmas, nerr );
	if( *nerr != 0 )
		goto L_8888;

        nfft = 8;

	while ( nfft < ( 2 * iwinmx - 1 ) )	/* cleaned up.  maf 961204 */
	    nfft *= 2 ;

	allamb( &cmmem, nfft, &ndxcor, nerr );
	if( *nerr != 0 ){
		relamb( cmmem.sacmem, ndxmas, &nrerr );
		*nerr = 919;
		setmsg( "ERROR", *nerr );
		goto L_8888;
		}

	/* - Get the master signal and copy to first temporary block.
	 *   Pad with zeros if necessary. */

				 /* nlen became nlenMaster.  maf 961204 */
	getfil( cmsam.imast, TRUE, &nlenMaster, &ndxy, &ndxx, nerr );
	if( *nerr != 0 )
		goto L_7777;	/* L_8888 became L_7777.  maf 961204 */
	copy( (long*)cmmem.sacmem[ndxy], (long*)cmmem.sacmem[ndxmas], nlenmx );
	nzeros = nlenmx - nlenMaster;	/* nlen became nlenMaster.  maf 961204 */
	if( nzeros > 0 )
		fill( cmmem.sacmem[ndxmas]+nlenMaster, nzeros, 0. );

	masterBegin = *begin ;	/* remember when the master begins. maf 961204 */

	/* - Perform the requested function on each file in DFL. */

	for( jdfl = 1; jdfl <= cmdfm.ndfl; jdfl++ ){
		jdfl_ = jdfl - 1;

		/* -- Get next file from the memory manager.
		 *    (Header is moved into common blocks CMHDR and KMHDR.) */
		getfil( jdfl, TRUE, &nlen, &ndxy, &ndxx, nerr );
		if( *nerr != 0 )
			goto L_7777;	/* L_8888 became L_7777.  maf 961204 */
		nlenCombined = nlen + nlenMaster - 1 ;	/* added maf 961204 */

		/* -- Allocate a new block, copy signal to it, and pad with zeros if necessary. */
		allamb( &cmmem, 2 * nlenmx, &ndxsig, nerr ); /* nlenmx became 2*nlenmx. maf 961204 */
		if( *nerr != 0 )
			goto L_7777;	/* L_8888 became L_7777.  maf 961204 */
		copy( (long*)cmmem.sacmem[ndxy], (long*)cmmem.sacmem[ndxsig], nlenmx );
		nzeros = 2 * nlenmx - nlen;	/* nlenmx became 2*nlenmx. maf 961204 */
		if( nzeros > 0 )
			fill( cmmem.sacmem[ndxsig]+nlen, nzeros, 0. );

		/* -- Update dfl indices to point to this new block and release old one. */
		Nlndta[jdfl] = nlenCombined;	/* nlenmx became nlenCombined. maf 961204 */
		cmdfm.ndxdta[jdfl_][0] = ndxsig;
		relamb( cmmem.sacmem, ndxy, nerr );

		/* -- Compute length of each window. */
		if( cmsam.lwinln ){
			iwinln = (long)( cmsam.winln/ *delta + 0.1 );
		}
		else{
			iwinln = nlenmx/cmsam.nwin;
		}

		/* -- Compute the (unshifted) correlation. */
		crscor( cmmem.sacmem[ndxmas], cmmem.sacmem[ndxsig], nlenmx, cmsam.nwin, 
		 iwinln, (char*)kmsam.kwintp[cmsam.iwintp - 1], cmmem.sacmem[ndxcor], 
		 &nfft, kermsg,131 );
		if( memcmp(kermsg,"        ",8) != 0 ){
			*nerr = 1;
			setmsg( "ERROR", *nerr );
			apcmsg( kermsg,131 );
			goto L_7777;	/* L_8888 became L_7777.  maf 961204 */
		}

		/* -- Perform a circular shift to align the correlation in the output block. */
		/*	overhauled to allow full range of correlation.  maf 961204 */
		for ( j = 0 ; j <= nlenMaster - 2 ; j++ )
                    *(cmmem.sacmem[ndxsig]+j) = *(cmmem.sacmem[ndxcor]+nfft-nlenMaster + j + 1 );
		for ( j = 0 ; j <= nlen - 1 ; j++ )
                    *(cmmem.sacmem[ndxsig]+nlenMaster + j - 1 ) = *(cmmem.sacmem[ndxcor]+j);
		
		/* Pad with zeros if needed.  maf 961204 */
                nzeros = 2 * nlenmx - 1 - nlenCombined ;
                if( nzeros > 0 )
                    fill( cmmem.sacmem[ndxsig]+nlenCombined, nzeros, 0. );


		/* -- Update any header fields that may have changed. */
		/*	overhauled to preserve differences in begin times.  maf 961204 */
		*npts = nlenCombined;
		*begin = -(float)( nlenMaster - 1 )**delta + *begin - masterBegin ;
		*ennd = *begin + *delta*(float)( nlenCombined - 1 );
		extrma( cmmem.sacmem[ndxsig], 1, nlenCombined, depmin, depmax, depmen );
		*nzyear = cmhdr.nundef ;
		*nzhour = cmhdr.nundef ;

		/* -- Return file to memory manager. */
		putfil( jdfl, nerr );
		if( *nerr != 0 )
			goto L_7777;	/* L_8888 became L_7777.  maf 961204 */

	} /* end for(jdfl) */

	/* - Release temporary blocks. */
L_7777:	/* added  maf 961204 */
	relamb( cmmem.sacmem, ndxmas, nerr );
	if( *nerr != 0 )
		goto L_8888;
	relamb( cmmem.sacmem, ndxcor, nerr );
	if( *nerr != 0 )
		goto L_8888;

	/* - Calculate and set new range of dependent variable. */

	setrng();

L_8888:
	return;

}

