#!/bin/sh

#
# Description:
#		This demonstrates the CDK command line
# interface to the template widget.
#

#
# Create some global variables.
#
CDK_TEMPLATE="../cdktemplate"
CDK_LABEL="../cdklabel"
output="/tmp/template_output.$$"
tmp="/tmp/tmp.$$"

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
if [ $selected -lt 0 ]; then
   exit;
fi
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
