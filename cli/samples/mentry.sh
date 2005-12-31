#!/bin/sh
# $Id: mentry.sh,v 1.3 2005/12/27 15:53:06 tom Exp $

#
# Description:
#		This demonstrates the CDK command line
# interface to the multiple line entry widget.
#

#
# Create some global variables.
#
CDK_MENTRY="${CDK_BINDIR=..}/cdkmentry"
CDK_LABEL="${CDK_BINDIR=..}/cdklabel"

output="${TMPDIR=/tmp}/mentry_output.$$"
tmp="${TMPDIR=/tmp}/tmp.$$"

#
# Create the title.
#
title="<C>Type in a short message."
buttons=" OK 
 Cancel "

#
# Create the mentry box.
#
${CDK_MENTRY} -s 5 -v 10 -f 20 -T "${title}" -B "${buttons}" -F '_' -O ${output}
selected=$?
test $selected = 255 && exit 1

answer=`cat ${output}`

#
# Create the message for the label widget.
#
echo "<C>Here is the message you typed in" > ${tmp}
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
