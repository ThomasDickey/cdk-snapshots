#!/bin/sh

#
# Description:
#		This demonstrates the CDK command line
# interface to the celendar widget.
#

#
# Create some global variables.
#
CDK_CALENDAR="../cdkcalendar"
CDK_LABEL="../cdklabel"
date="/tmp/cal.$$"
tmp="/tmp/tmp.$$"
xpos=CENTER
ypos=CENTER

#
# Get today's date.
#
day=`date +%d`
month=`date +%m`
year=`date +%Y`

#
# Chop up the command line.
#
set -- `getopt d:m:y:X:Y: $*`
if [ $? != 0 ]
then
   echo $USAGE
   exit 2
fi
for c in $*
do
    case $c in
         -d) day=$2; shift 2;;
         -m) month=$2; shift 2;;
         -y) year=$2; shift 2;;
         -X) xpos=$2; shift 2;;
         -Y) ypos=$2; shift 2;;
         --) shift; break;;
    esac
done

#
# Create the title and buttons.
#
title="<C><#HL(22)>
<C>Select a date
<C><#HL(22)>"
buttons=" OK 
Cancel "

#
# Create the calendar widget.
#
${CDK_CALENDAR} -B "${buttons}" -d ${day} -m ${month} -y ${year} -T "${title}" -X ${xpos} -Y ${ypos} -O ${date} -S
selected=$?
if [ $selected -lt 0 ]; then
   exit;
fi
answer=`cat ${date}`

#
# Create the message for the label widget.
#
echo "<C>You chose the following date" > ${tmp}
echo " " >> ${tmp}
echo "<C><#HL(10)>" >> ${tmp}
echo "<C>${answer}" >> ${tmp}
echo "<C><#HL(10)>" >> ${tmp}
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
rm -f ${tmp} ${date}
