#!/bin/sh

#
# Description:
#		This demonstrates the CDK command line
# interface to the slider widget.
#

#
# Create some global variables.
#
CDK_SLIDER="../cdkslider"
CDK_LABEL="../cdklabel"
output="/tmp/slider_output.$$"
tmp="/tmp/tmp.$$"

#
# Create the title.
#
title="<C>Enter a value using the cursor keys"
label="Value: "
buttons=" OK 
 Cancel "

#
# Create the slider box.
#
${CDK_SLIDER} -f 20 -l 0 -h 100 -i 5 -L "${label}" -T "${title}" -F "</R>*" -B "${buttons}" -O ${output}
selected=$?
if [ $selected -lt 0 ]; then
   exit;
fi
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
