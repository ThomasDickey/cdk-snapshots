#! /bin/sh
# $Id: headers.sh,v 1.3 2001/12/04 01:55:24 tom Exp $
#
# Adjust includes for header files that reside in a subdirectory of
# /usr/include, etc.

TMPSED=headers.sed

if test $# = 2 ; then
	rm -f $TMPSED
	DST=$1
	REF=$2
	LEAF=`basename $DST`
	case $DST in
	/*/include/$LEAF)
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

	# Just in case someone gzip'd manpages, remove the conflicting copy.
	test -f $DST/$NAME.gz && rm -f $DST/$NAME.gz

	eval $PRG $TMPSRC $DST/$NAME
	rm -f $TMPSRC
fi
