#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../inc/complex.h"
#include "../../inc/proto.h"
#include <string.h>
#include "../../inc/mach.h"
#include "../../inc/com.h"
long /*FUNCTION*/ lkchar(kkey, kkey_s, mchar, kchar, kchar_s, 
	 nchar)
char *kkey;   int kkey_s;
long int mchar;
char *kchar;   int kchar_s;
long int *nchar;
{
	long lkchar_v;
	long int nchstr;



	/*=====================================================================
	 * PURPOSE:  To parse a "keyed character string" command construct.
	 *=====================================================================
	 * FUNCTION VALUE:
	 *    lkchar:  .TRUE. if the construct was found at the current
	 *             command symbol, .FALSE. otherwise. [l]
	 *=====================================================================
	 * INPUT ARGUMENTS:
	 *    kkey:    Keyword to search for. [c]
	 *             Append a dollar sign ("$") to end of keyword.
	 *             Append a pound sign ("#") to end of minimum allowed
	 *             abbreviation if it is more than a single character.
	 *    mchar:   Maxiumum length of KCHAR. [i]
	 *=====================================================================
	 * OUTPUT ARGUMENTS:
	 *    kchar:   Output character string. [c]
	 *    nchar:   Length of KCHAR. [i]
	 *=====================================================================
	 * MODULE/LEVEL:  cpf/4
	 *=====================================================================
	 * GLOBAL INPUT:
	 *    mach:
	 *    com:     jcom, ncom, kcom, itypcm, inumbr, ialpha, flnum
	 *=====================================================================
	 * GLOBAL OUTPUT:
	 *    com:     jcom
	 *=====================================================================
	 * SUBROUTINES CALLED:
	 *    sac:     lckey, copykc
	 *=====================================================================
	 * LOCAL VARIABLES:
	 *    nchstr:  Number of characters in current command token.
	 *=====================================================================
	 * MODIFICATION HISTORY:
	 *    830208:  Allowed for a numeric token to be used as alphanumeric.
	 *    820423:  Adjustments due to new command parsing system.
	 *    820312:  Factored test for key to LCCKEY function.
	 *    810928:  Modified due to changes in TOKENS.
	 *    810416:  Removed option to use underline as substitute for blank.
	 *    810208:  Original version.
	 *=====================================================================
	 * DOCUMENTED/REVIEWED:  820426
	 *===================================================================== */
	/* PROCEDURE: */
	/* - Check for key. */
	lkchar_v = lckey( kkey,kkey_s );

	/* - Return if key was not found. */
	if( !lkchar_v )
		return( lkchar_v );

	/* - Logic for quoted string case. Quotes can occur for two reasons:
	 *   (1) Text contained embedded blanks. Quotes were supplied by user.
	 *   (2) Text had no embedded blanks but was longer than MCPW, the
	 *       maximum number of characters per word. Quotes are added
	 *       by TOKENS in this case. */

	if( strcmp ( kmcom.kcom[ cmcom.jcom - 1 ] , "'       " ) == 0 ||
	    strcmp ( kmcom.kcom[ cmcom.jcom - 1 ] , "\"       " ) == 0 ||
	    strcmp ( kmcom.kcom[ cmcom.jcom - 1 ] , "#       " ) == 0 ) {

		/* -- Determine number of characters in string. */
		cmcom.jcom = cmcom.jcom + 1;
		nchstr = (long)( Flnum[cmcom.jcom] + 0.1 );

		/* -- Handle case when string is too long. */
		cmcom.jcom = cmcom.jcom + 1;
		if( nchstr > mchar ){
			*nchar = mchar;
			copykc( (char*)kmcom.kcom[cmcom.jcom - 1],9, mchar, kchar);
			kchar[ mchar-1 ] = '\0' ;
		}

		/* -- Case when string length is ok. */
		else{
			*nchar = nchstr;
			copykc( (char*)kmcom.kcom[cmcom.jcom - 1],9, *nchar, kchar);
			subscpy( kchar, *nchar, mchar - 1, kchar_s - 1, " " );
		}

		/* -- Adjust counter in current command. */
		cmcom.jcom = cmcom.jcom + (nchstr - 1)/MCPW + 1;

		/* - Case where length of text is less than or equal to MCPW. */

	}
	else{

		/* -- Copy current command word to output string. */
		subscpy( kchar, 0, mchar - 1, kchar_s - 1, kmcom.kcom[cmcom.jcom - 1]);

		/* -- Determine length of output string to first blank. */
		*nchar = indexc( kchar,kchar_s, ' ' );

		/* -- Increment command counter. */
		cmcom.jcom = cmcom.jcom + 1;

	}

	return( lkchar_v );

} /* end of function */

