#!/bin/sh
# $Id: entry.sh,v 1.3 2005/12/27 15:53:06 tom Exp $

#
# Description:
#		This demonstrates the CDK command line
# interface to the entry widget.
#

#
# Create some global variables.
#
CDK_ENTRY="${CDK_BINDIR=..}/cdkentry"
CDK_LABEL="${CDK_BINDIR=..}/cdklabel"

output="${TMPDIR=/tmp}/entry_output.$$"
tmp="${TMPDIR=/tmp}/tmp.$$"

#
# Create the title.
#
title="<C>  Type a simple string.  "
buttons=" OK 
 Cancel "

#
# Create the entry box.
#
${CDK_ENTRY} -f 20 -T "${title}" -B "${buttons}" -F "</5>_ " -O ${output} -S
selected=$?
test $selected = 255 && exit 1

answer=`cat ${output}`

#
# Create the message for the label widget.
#
echo "<C>Here is the string you typed in" > ${tmp}
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
