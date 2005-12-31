#!/bin/sh
# $Id: template.sh,v 1.3 2005/12/27 15:53:06 tom Exp $

#
# Description:
#		This demonstrates the CDK command line
# interface to the template widget.
#

#
# Create some global variables.
#
CDK_TEMPLATE="${CDK_BINDIR=..}/cdktemplate"
CDK_LABEL="${CDK_BINDIR=..}/cdklabel"

output="${TMPDIR=/tmp}/template_output.$$"
tmp="${TMPDIR=/tmp}/tmp.$$"

#
# Create the title.
#
title="<C>               Type in the                 
<C>IP Address for this machine."
buttons=" OK 
 Cancel "

#
# Create the template box.
#
${CDK_TEMPLATE} -p "###.###.###.###" -o "___.___.___.___" -T "${title}" -P -B "${buttons}" 2> ${output}
selected=$?
test $selected = 255 && exit 1

answer=`cat ${output}`

#
# Create the message for the label widget.
#
echo "<C>Here is the IP you typed in" > ${tmp}
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
