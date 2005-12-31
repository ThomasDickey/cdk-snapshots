#!/bin/sh
# $Id: fselect.sh,v 1.4 2005/12/27 17:56:05 tom Exp $

#
# Description:
#	This demonstrates the CDK command line
#	interface to the file selection widget.
#

#
# Create some global variables.
#
CDK_FSELECT="${CDK_BINDIR=..}/cdkfselect"
CDK_LABEL="${CDK_BINDIR=..}/cdklabel"

directory="."
label="File: "
title="<C>Select a file"

buttons=" OK 
 Cancel "

xpos="CENTER"
ypos="CENTER"

width=0
height=-5

tmp="${TMPDIR=/tmp}/tmp.$$"
file="${TMPDIR=/tmp}/fs.$$"

#
# Chop up the command line.
#
set -- `getopt d:L:T:X:Y:W:H: $*`
if [ $? != 0 ]
then
   echo $USAGE
   exit 2
fi
for c in $*
do
    case $c in
         -d) directory=$2; shift 2;;
         -T) title=$2; shift 2;;
         -L) label=$2; shift 2;;
         -X) xpos=$2; shift 2;;
         -Y) ypos=$2; shift 2;;
         -W) width=$2; shift 2;;
         -H) height=$2; shift 2;;
         --) shift; break;;
    esac
done

#
# Create the CDK file selector.
#
${CDK_FSELECT} -d "${directory}" -T "${title}" -L "${label}" -X ${xpos} -Y ${ypos} -W ${width} -H ${height} -B "${buttons}" 2> ${file}
selected=$?
test $selected = 255 && exit 1

answer=`cat ${file}`

#
# Diplay the file the user selected.
#
echo "<C>You selected the following file" > ${tmp}
echo " " >> ${tmp}
echo "<C><#HL(10)>" >> ${tmp}
echo "<C></B>${answer}" >> ${tmp}
echo "<C><#HL(10)>" >> ${tmp}
echo " " >> ${tmp}
echo "<C>You chose button #${selected}" >> ${tmp}
echo " " >> ${tmp}
echo "<C>Press </R>space<!R> to continue." >> ${tmp}

${CDK_LABEL} -f ${tmp} -p " "

#
# Clean up.
#
rm -f ${tmp} ${file}
