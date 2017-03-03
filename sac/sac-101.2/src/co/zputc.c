/*
CDOC
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 
 *** Function:	zputc_.c(str,array,pnumc,strlen)		D. Trimmer
 
 *** Purpose:	Puts a character string into an integer or real array.
 
 *** Inputs:	str	A character array
		array	A real or integer array
		pnumc	pointer to # of characters to copy
		strlen  length of str--required by F77.
 
 *** Returns:	none
 
 *** Notes:	This function is to be called by a FORTRAN routine.  The
		'_' is appended to the name for compatibility with FORTRAN.
 
		The argument strlen is generated by F77.
 
 *** History:	07/24/84   Under development--D. Trimmer
		07/24/84   Tested--D. Trimmer
 
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
CEND
*/
 

zputc(str,strlen,array,pnumc)
char str[];		/* source character array */
long strlen;		/* dummy required by F77 */
long array[];		/* sink array for characters */
long pnumc;		/* # characters to copy */
 
{
	char  *parray;		/* pointer into array */
	char  *pstr;		/* pointer into string */
	long  i;		/* index */
 
	parray = (char *) &array[0];	/* character pointer into array */
	pstr   = &str[0];		/* character pointer into str */
	for (i=0;i<pnumc;++i)
		*parray++ = *pstr++;	/* copy characters */
 
	return;
}
 
