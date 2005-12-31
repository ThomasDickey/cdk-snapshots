#! /bin/sh
# $Id: manlinks.sh,v 1.5 2005/12/29 00:22:00 tom Exp $
# install/uninstall aliases for one or more manpages,
#
#  Copyright 2002,2005 Thomas Dickey
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#  3. All advertising materials mentioning features or use of this software
#     must display the following acknowledgment:
#       This product includes software developed by Thomas Dickey
#       and contributors.
#  4. Neither the name of Thomas Dickey, nor the names of contributors
#     may be used to endorse or promote products derived from this software
#     without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THOMAS DICKEY AND CONTRIBUTORS ``AS IS'' AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED.  IN NO EVENT SHALL THOMAS DICKEY OR CONTRIBUTORS BE LIABLE
#  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
#  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
#  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
#  OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
#  SUCH DAMAGE.

CDPATH=

verb=$1
shift

srcdir=$1
shift

mandir=$1
shift

mansect=$1
shift

parent=`pwd`
script=$srcdir/manlinks.sed

for source in $* ; do
case $source in #(vi
*.orig|*.rej) ;; #(vi
*.[0-9]*)
	cd $parent
	section=`expr "$source" : '.*\.\([0-9]\)[xm]*'`;
	aliases=
	inalias=$source
	test ! -f $inalias && inalias="$srcdir/$inalias"
	if test ! -f $inalias ; then
		echo "	.. skipped $source"
		continue
	fi
	aliases=`sed -f $script $inalias | sort -u`

	suffix=`basename $source | sed -e 's/^[^.]*//'`
	cd $mandir || exit 1
	if test $verb = installing ; then
		test -n "$aliases" && (
			if test "$suffix" = ".$mansect" ; then
				target=`basename $source`
			else
				target=`basename $source $suffix`.$mansect
				suffix=".$mansect"
			fi
			for cf_alias in $aliases
			do
				cf_doit=no
				if test -f $cf_alias${suffix} ; then
					if ( cmp -s $target $cf_alias${suffix} )
					then
						:
					else
						cf_doit=yes
					fi
				else
					cf_doit=yes
				fi
				if test $cf_doit = yes ; then
					echo "	... alias $cf_alias${suffix}"
					rm -f $cf_alias${suffix}
					if (ln -s $target $cf_alias${suffix}) ; then
						:
					else
						echo ".so $target" > $cf_alias${suffix}
					fi
				fi
			done
		)
	elif test $verb = removing ; then
		suffix=".$mansect"
		test -n "$aliases" && (
			for cf_alias in $aliases
			do
				echo "	... alias $cf_alias${suffix}"
				rm -f $cf_alias${suffix}
			done
		)
	fi
	;;
esac
done
exit 0
