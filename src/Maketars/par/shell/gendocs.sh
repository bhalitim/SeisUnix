#! /bin/sh
# /*********************** self documentation **********************/
# GENDOCS - generate complete list of selfdocs in latex form
#
# Usage: gendocs -o  output filename is:  selfdocs.tex
#
# /**************** end self doc ********************************/

# Author: John Stockwell -- 5 Jan 1992

PATH=/bin:/usr/bin:/usr/ucb

##################$############################################################
# test for CWPROOT
###############################################################################
if test "${CWPROOT}" = ""
then
	echo "The environment variable \"CWPROOT\" "
	echo "is not set in the user's working shell environment."
	echo "To set this variable in C-shell, use the command: "
	echo "  setenv  CWPROOT  /your/cwp/root/path"
	echo "To set this variable in Bourne or Korn-shell, use the command:"
	echo "  export  CWPROOT=/your/cwp/root/path" ; exit 1

fi

################################################################################
# test for CWPSRC, use value if set, define as $CWPROOT if not set
# (CWPSRC allows one set of source code and documentation for multiple machines)
################################################################################
if test "${CWPSRC}" = ""
then
CWPSRC=$CWPROOT
fi

WORKINGDIR=`pwd`
ROOT=${CWPSRC}
OUTFILE=selfdocs.tex
FILE=$WORKINGDIR/$OUTFILE
DIR=${ROOT}/src
STRIP=$DIR/doc/Stripped
HEADALL=${ROOT}/src/doc/Headers/Headers.all

SHELLS="cwp.shell par.shell su.shell psplot.shell "

LIBS="psplot.lib xplot.lib Xtcwp.lib Xmcwp.lib su.lib cwp.lib tri.lib "

MAINS="par.main su.main.amplitudes \
su.main.attributes_parameter_estimation \
su.main.convolution_correlation \
su.main.data_compression \
su.main.data_conversion \
su.main.datuming \
su.main.decon_shaping \
su.main.dip_moveout \
su.main.filters \
su.main.gocad \
su.main.headers \
su.main.interp_extrap \
su.main.migration_inversion \
su.main.multicomponent \
su.main.noise \
su.main.operations \
su.main.picking \
su.main.stacking \
su.main.statics \
su.main.stretching_moveout_resamp \
su.main.supromax \
su.main.synthetics_waveforms_testpatterns \
su.main.tapering \
su.main.transforms \
su.main.velocity_analysis \
su.main.well_logs \
su.main.windowing_sorting_muting \
su.graphics.psplot su.graphics.xplot \
psplot.main xplot.main Xtcwp.main Xmcwp.main su.graphics.psplot \
su.graphics.xplot xtri tri.main tri.graphics.psplot "

CWPMAINS="cwp.main"


# check for arguments
for i
do
	case $i in
	-o)	# inform user that file is being built
		echo
		echo "Beginning generation of" 
		echo "$OUTFILE"
		echo "This will take about a minute"
		echo "             --- Please standby"
		echo

		rm $FILE

		# begin building selfdocs.tex
echo "%selfdocs.tex --- complete list of CWP Free program selfdocs" >> $FILE
		echo "% generated by --- GENDOCS" >> $FILE
		echo " " >> $FILE
		echo "\\documentstyle[12pt]{article}" >> $FILE
		echo " " >> $FILE
		echo "\\textwidth 6.25in" >> $FILE
		echo "\\textheight 8.75in" >> $FILE
		echo "\\oddsidemargin .125in" >> $FILE
		echo "\\evensidemargin .125in" >> $FILE
		echo "%\topmargin -.5in" >> $FILE
		echo " " >> $FILE
#
#		echo "\\title{Complete Listing of CWP Free" >> $FILE 
#		echo "Program Self-Documentations}" >> $FILE
#		echo "\\author{{\\em generated by GENDOCS}, \\\\\ " >> $FILE
#		echo "a shell script by John Stockwell \\\\\ " >> $FILE
#		echo "Center for Wave Phenomena \\\\\ " >> $FILE
#		echo "Colorado School of Mines}" >> $FILE
#		echo "\\date{\\""\today}" >> $FILE


		echo "\\begin{document}" >> $FILE
#		echo "\\maketitle " >> $FILE

## if you do not have titlepagesd.tex, uncomment the lines commented out
## above and comment out the next line
		echo "\\input /usr/local/cwp/src/su/tutorial/titlepagesd.tex" >> $FILE
		echo "" >> $FILE
		cd $DIR/doc

		
		echo "\\pagebreak" >> $FILE
		echo "\\section*{Names and Short descriptions of the Codes}" \
			>> $FILE
		echo " CWPROOT = $CWPROOT" >> $FILE
		echo >> $FILE
		echo "\\begin{verbatim}" >> $FILE

		cat $HEADALL >> $FILE

		echo "\\end{verbatim}" >> $FILE

		echo "\\begin{verbatim}" >> $FILE
		echo "" >> $FILE
		echo "To search on a program name fragment, type:" >> $FILE
		echo "      suname name_fragment <CR>" >> $FILE
		echo "" >> $FILE
		echo "For more information type: program_name <CR>" >> $FILE
		echo "" >> $FILE
		echo "  Items labeled with an asterisk (*) are C programs \
				that may">> $FILE
		echo "  or may not have this self documentation feature. " \
				>> $FILE
		echo "" >> $FILE
		echo "  Items labeled with a pound sign (#) are shell \
				scripts that may,">>$FILE
		echo "  or may not have the self documentation feature." \
				>> $FILE
		echo "" >> $FILE

		echo "\\end{verbatim}" >> $FILE

		echo "\\pagebreak" >> $FILE

		echo "\\section*{Self Documentations}" >> $FILE


		# use the Stripped versions
		cd $DIR/doc/Stripped

		echo "Mains: " >> $FILE

		for i in $CWPMAINS
		do
			for j in $STRIP/*.$i
			do
				echo "\\begin{verbatim}" >> $FILE
				echo "______" >> $FILE
				cat $j >> $FILE
				echo "\\end{verbatim}" >> $FILE
			done
			echo "\\pagebreak" >> $FILE
		done
		

		for i in $MAINS
		do
			for j in $STRIP/*.$i
			do
				echo "\\begin{verbatim}" >> $FILE
				cat $j >> $FILE
				echo "\\end{verbatim}" >> $FILE
				echo "\\pagebreak" >> $FILE
			done
		done

		echo "Shells: " >> $FILE

		for i in $SHELLS
		do
			for j in $STRIP/*.$i
			do
				echo "\\begin{verbatim}" >> $FILE
				echo "______" >> $FILE
				cat $j >> $FILE
				echo "\\end{verbatim}" >> $FILE
			done
			echo "\\pagebreak" >> $FILE
		done

		echo "Libs: " >> $FILE

		for i in $LIBS
		do
			for j in $STRIP/*.$i
			do
				echo "\\begin{verbatim}" >> $FILE
				cat $j >> $FILE
				echo "\\end{verbatim}" >> $FILE
				echo "\\pagebreak" >> $FILE
			done
		done

		echo "\\end{document}" >> $FILE

		echo "The document $OUTFILE is now ready."
		exit 1
		;;
	esac
done

# else echo a usage message
echo
echo "GENDOCS - Shell to GENerate the complete listing of CWP selfDOCS"
echo
echo
echo "Output is the file in LaTeX format called:    selfdocs.tex" 
echo
echo
echo "Usage:  gendocs -o"  

exit 0