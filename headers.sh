#! /bin/sh
# $Id: headers.sh,v 1.2 2001/04/21 16:25:37 tom Exp $
#
# Adjust includes for header files that reside in a subdirectory of
# /usr/include, etc.

TMPSED=headers.sed

if test $# = 2 ; then
	DST=$1
	REF=$2

	case $DST in
	/*/include/*)
		END=`basename $DST`
		for i in $REF/*.h
		do
			NAME=`basename $i`
			echo "s/<$NAME>/<$END\/$NAME>/" >> $TMPSED
		done
		;;
	*)
		echo "" >> $TMPSED
		;;
	esac
else
	PRG=""
	while test $# != 3
	do
		PRG="$PRG $1"; shift
	done

	DST=$1
	REF=$2
	SRC=$3

	SHOW=`basename $SRC`
	TMPSRC=${TMPDIR-/tmp}/${SHOW}$$

	echo "	... $SHOW"
	test -f $REF/$SRC && SRC="$REF/$SRC"

	rm -f $TMPSRC
	sed -f $TMPSED $SRC > $TMPSRC
	NAME=`basename $SRC`
	eval $PRG $TMPSRC $DST/$NAME
	rm -f $TMPSRC
fi
