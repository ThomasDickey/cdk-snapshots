#!/bin/sh
# $Id: scale.sh,v 1.3 2005/12/27 15:53:06 tom Exp $

#
# Description:
#		This demonstrates the CDK command line
# interface to the scale widget.
#

#
# Create some global variables.
#
CDK_SCALE="${CDK_BINDIR=..}/cdkscale"
CDK_LABEL="${CDK_BINDIR=..}/cdklabel"

output="${TMPDIR=/tmp}/scale_output.$$"
tmp="${TMPDIR=/tmp}/tmp.$$"

#
# Create the title.
#
title="<C>Enter a value using the cursor keys."
label="</R>Value"
buttons=" OK 
 Cancel "

#
# Create the scale box.
#
${CDK_SCALE} -f 20 -l 0 -h 100 -i 5 -L "${label}" -T "${title}" -B "${buttons}" -O ${output}
selected=$?
test $selected = 255 && exit 1

answer=`cat ${output}`

#
# Create the message for the label widget.
#
echo "<C>Here is the value you selected" > ${tmp}
echo " " >> ${tmp}
echo "<C></R>${answer}" >> ${tmp}
echo " " >> ${tmp}
echo "<C>You chose button #${selected}" >> ${tmp}
echo " " >> ${tmp}
echo "<C>Hit </R>space<!R> to continue." >> ${tmp}

#
# Create the label widget to display the information.
#
${CDK_LABEL} -f ${tmp} -p " "

#
# Clean up.
#
rm -f ${tmp} ${output}
