#!/bin/sh

#
# Description:
#		This demonstrates the CDK command line
# interface to the multiple line entry widget.
#

#
# Create some global variables.
#
CDK_MENTRY="../cdkmentry"
CDK_LABEL="../cdklabel"
output="/tmp/mentry_output.$$"
tmp="/tmp/tmp.$$"

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
if [ $selected -lt 0 ]; then
   exit;
fi
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
