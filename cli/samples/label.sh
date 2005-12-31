#!/bin/sh
# $Id: label.sh,v 1.3 2005/12/27 15:53:06 tom Exp $

#
# Description:
#		This demonstrates the CDK command line
# interface to the label widget.
#

#
# Create some global variables.
#
CDK_LABEL="${CDK_BINDIR=..}/cdklabel"

dfTmp="${TMPDIR=/tmp}/label.$$"
tmp="${TMPDIR=/tmp}/tmp.$$"
top="${TMPDIR=/tmp}/dfTop.$$"
bottom="${TMPDIR=/tmp}/dfBottom.$$"

#
# Get the filesystem information.
#
getDiskInfo()
{
   fileName=$1;
   command="df"

   #
   # Determine the type of operating system.
   #
   machine=`uname -s`
   if [ "$machine" = "SunOS" ]; then
      level=`uname -r`

      if [ "$level" -gt 4 ]; then
         command="df -kl"
      fi
   else
      if [ "$machine" = "AIX" ]; then
         command="df -i"
      fi
   fi

   #
   # Run the command.
   #
   ${command} > ${fileName}
}

#
# Get the disk information.
#
getDiskInfo ${dfTmp}

#
# Bold the first line of the df command.
#
head -1 ${dfTmp} | awk '{printf "</B>%s\n", $0}' > ${top}
tail +2 ${dfTmp} > ${bottom}

#
# Create the message for the label widget.
#
echo "<C>This is the current" > ${tmp}
echo "<C>status of your local filesystems." >> ${tmp}
echo "<C><#HL(2)>" >> ${tmp}
cat ${top} >> ${tmp}
cat ${bottom} >> ${tmp}
echo "<C><#HL(2)>" >> ${tmp}
echo "" >> ${tmp}
echo "<C>Hit </R>space<!R> to continue." >> ${tmp}

#
# Create the label widget to display the information.
#
${CDK_LABEL} -f "${tmp}" -p " " -c "ls" -S

#
# Clean up.
#
rm -f ${tmp} ${output} ${dfTmp} ${top} ${bottom}
