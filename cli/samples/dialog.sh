#!/bin/sh
# $Id: dialog.sh,v 1.3 2005/12/27 15:53:06 tom Exp $

#
# Description:
#		This demonstrates the CDK command line
# interface to the dialog widget.
#

#
# Create some global variables.
#
CDK_DIALOG="${CDK_BINDIR=..}/cdkdialog"
CDK_LABEL="${CDK_BINDIR=..}/cdklabel"

output="${TMPDIR=/tmp}/dialog_output.$$"
tmp="${TMPDIR=/tmp}/tmp.$$"

#
# Create the message for the scrolling list.
#
message="<C>Pick which command
<C>you wish to run."

#
# Create the button labels.
#
commands="</B>who
</B>w
</B>uptime
</B>date
</B>pwd
</B>whoami
</B>df
</B>fortune"

#
# Create the dialog box.
#
${CDK_DIALOG} -m "${message}" -B "${commands}" 2> ${output}
selection=$?
if [ "$selection" -eq 255 ]; then
   exit;
fi

#
# Create the message for the label widget.
#
echo "<C>Here is the result of the command" > ${tmp}
echo " " >> ${tmp}

#
# Determine which command to run.
#
if [ "${selection}" -eq 0 ]; then
   who | awk '{printf "</B>%s\n", $0}' >> ${tmp}
elif [ "${selection}" -eq 1 ]; then
   w | awk '{printf "</B>%s\n", $0}' >> ${tmp}
elif [ "${selection}" -eq 2 ]; then
   uptime | awk '{printf "<C></B>%s\n", $0}' >> ${tmp}
elif [ "${selection}" -eq 3 ]; then
   date | awk '{printf "<C></B>%s\n", $0}' >> ${tmp}
elif [ "${selection}" -eq 4 ]; then
   pwd | awk '{printf "<C></B>%s\n", $0}' >> ${tmp}
elif [ "${selection}" -eq 5 ]; then
   whoami | awk '{printf "<C></B>%s\n", $0}' >> ${tmp}
elif [ "${selection}" -eq 6 ]; then
   #
   # We will use the label demo to do this.
   #
   ./label.sh
   rm -f ${tmp} ${output} ${fileSystemList}
   exit 0;
elif [ "${selection}" -eq 7 ]; then
   fortune | awk '{printf "</B>%s\n", $0}' >> ${tmp}
fi
echo " " >> ${tmp}
echo "<C>Hit </R>space<!R> to continue." >> ${tmp}

#
# Create the label widget to display the information.
#
${CDK_LABEL} -f ${tmp} -p " "

#
# Clean up.
#
rm -f ${tmp} ${output} ${fileSystemList}
