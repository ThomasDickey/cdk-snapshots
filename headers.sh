#! /bin/sh
# $Id: headers.sh,v 1.6 2005/03/25 23:30:29 tom Exp $
#
# Adjust includes for header files that reside in a subdirectory of
# /usr/include, etc.
#
# Options:
#	-c CFG	specify an alternate name for config.h
#	-d DIR	target directory
#	-i    	create/update the headers.sed script
#	-p PKG	specify the package name
#	-s DIR	source directory
#	-x PRG	install-program (plus options, the whole value in quotes)
#
# Other parameters are assumed to be provided only for the install scenario.

TMPSED=headers.sed

OPT_C=config.h
OPT_D=
OPT_I=n
OPT_P=
OPT_S=
OPT_X=install

while test $# != 0
do
	case $1 in
	-c)	# CFG	specify an alternate name for config.h
		shift
		OPT_C="$1"
		;;
	-d)	# DIR	target directory
		shift
		OPT_D="$1"
		;;
	-i)	# create the headers.sed script
		if test "$OPT_I" = n
		then
			rm -f $TMPSED
		fi
		OPT_I=y

		if ( test -n "$OPT_D" && test -d "$OPT_D" )
		then
			if ( test -n "$OPT_S" && test -d "$OPT_S" )
			then
				LEAF=`basename $OPT_D`
				case $OPT_D in
				/*/include/$LEAF)
					END=`basename $OPT_D`
					for i in $OPT_S/*.h
					do
						NAME=`basename $i`
						echo "s%<$NAME>%<$END/$NAME>%g" >> $TMPSED
					done
					;;
				*)
					echo "" >> $TMPSED
					;;
				esac
				OPT_S=
			fi
		fi

		if test -n "$OPT_P"
		then
			for name in `
			egrep "#define[ 	][ 	]*[A-Z]" $OPT_C \
				| sed	-e 's/^#define[ 	][ 	]*//' \
					-e 's/[ 	].*//' \
				| fgrep -v GCC_ \
				| sort -u \
				| egrep -v "^${OPT_P}_"`
			do
				echo "s%\\<$name\\>%${OPT_P}_$name%g" >>$TMPSED
			done
			OPT_P=
		fi
		;;
	-p)	# PKG	specify the package name
		shift
		OPT_P="$1"
		;;
	-s)	# DIR	source directory
		shift
		OPT_S="$1"
		;;
	-x)	# PRG	install-program (plus options, the whole value in quotes)
		shift
		OPT_X="$1"
		;;
	*)
		FILE=$1

		SHOW=`basename $FILE`
		TMPSRC=${TMPDIR-/tmp}/${SHOW}$$

		echo "	... $SHOW"
		test -f $OPT_S/$FILE && FILE="$OPT_S/$FILE"

		if test -f "$FILE"
		then
			rm -f $TMPSRC
			sed -f $TMPSED $FILE > $TMPSRC
			NAME=`basename $FILE`

			# Just in case someone gzip'd manpages, remove the conflicting copy.
			test -f $OPT_D/$NAME.gz && rm -f $OPT_D/$NAME.gz

			eval $OPT_X $TMPSRC $OPT_D/$NAME
			rm -f $TMPSRC
		fi
		;;
	esac
	shift
done
