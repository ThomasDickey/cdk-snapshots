#!/bin/sh

#
# Description:
#		This demonstrates the CDK command line
# interface to the entry widget.
#

#
# Create some global variables.
#
CDK_ENTRY="../cdkentry"
CDK_LABEL="../cdklabel"
output="/tmp/entry_output.$$"
tmp="/tmp/tmp.$$"

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
if [ $selected -lt 0 ]; then
   exit;
fi
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
