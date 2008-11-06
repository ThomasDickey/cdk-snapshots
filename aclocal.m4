dnl $Id: aclocal.m4,v 1.39 2008/11/01 16:45:01 tom Exp $
dnl macros used for CDK configure script
dnl -- Thomas E. Dickey
dnl ---------------------------------------------------------------------------
dnl ---------------------------------------------------------------------------
dnl CF_AC_PREREQ version: 2 updated: 1997/09/06 13:24:56
dnl ------------
dnl Conditionally generate script according to whether we're using the release
dnl version of autoconf, or a patched version (using the ternary component as
dnl the patch-version).
define(CF_AC_PREREQ,
[CF_PREREQ_COMPARE(
AC_PREREQ_CANON(AC_PREREQ_SPLIT(AC_ACVERSION)),
AC_PREREQ_CANON(AC_PREREQ_SPLIT([$1])), [$1], [$2], [$3])])dnl
dnl ---------------------------------------------------------------------------
dnl CF_ADD_CFLAGS version: 7 updated: 2004/04/25 17:48:30
dnl -------------
dnl Copy non-preprocessor flags to $CFLAGS, preprocessor flags to $CPPFLAGS
dnl The second parameter if given makes this macro verbose.
dnl
dnl Put any preprocessor definitions that use quoted strings in $EXTRA_CPPFLAGS,
dnl to simplify use of $CPPFLAGS in compiler checks, etc., that are easily
dnl confused by the quotes (which require backslashes to keep them usable).
AC_DEFUN([CF_ADD_CFLAGS],
[
cf_fix_cppflags=no
cf_new_cflags=
cf_new_cppflags=
cf_new_extra_cppflags=

for cf_add_cflags in $1
do
case $cf_fix_cppflags in
no)
	case $cf_add_cflags in #(vi
	-undef|-nostdinc*|-I*|-D*|-U*|-E|-P|-C) #(vi
		case $cf_add_cflags in
		-D*)
			cf_tst_cflags=`echo ${cf_add_cflags} |sed -e 's/^-D[[^=]]*='\''\"[[^"]]*//'`

			test "${cf_add_cflags}" != "${cf_tst_cflags}" \
			&& test -z "${cf_tst_cflags}" \
			&& cf_fix_cppflags=yes

			if test $cf_fix_cppflags = yes ; then
				cf_new_extra_cppflags="$cf_new_extra_cppflags $cf_add_cflags"
				continue
			elif test "${cf_tst_cflags}" = "\"'" ; then
				cf_new_extra_cppflags="$cf_new_extra_cppflags $cf_add_cflags"
				continue
			fi
			;;
		esac
		case "$CPPFLAGS" in
		*$cf_add_cflags) #(vi
			;;
		*) #(vi
			cf_new_cppflags="$cf_new_cppflags $cf_add_cflags"
			;;
		esac
		;;
	*)
		cf_new_cflags="$cf_new_cflags $cf_add_cflags"
		;;
	esac
	;;
yes)
	cf_new_extra_cppflags="$cf_new_extra_cppflags $cf_add_cflags"

	cf_tst_cflags=`echo ${cf_add_cflags} |sed -e 's/^[[^"]]*"'\''//'`

	test "${cf_add_cflags}" != "${cf_tst_cflags}" \
	&& test -z "${cf_tst_cflags}" \
	&& cf_fix_cppflags=no
	;;
esac
done

if test -n "$cf_new_cflags" ; then
	ifelse($2,,,[CF_VERBOSE(add to \$CFLAGS $cf_new_cflags)])
	CFLAGS="$CFLAGS $cf_new_cflags"
fi

if test -n "$cf_new_cppflags" ; then
	ifelse($2,,,[CF_VERBOSE(add to \$CPPFLAGS $cf_new_cppflags)])
	CPPFLAGS="$cf_new_cppflags $CPPFLAGS"
fi

if test -n "$cf_new_extra_cppflags" ; then
	ifelse($2,,,[CF_VERBOSE(add to \$EXTRA_CPPFLAGS $cf_new_extra_cppflags)])
	EXTRA_CPPFLAGS="$cf_new_extra_cppflags $EXTRA_CPPFLAGS"
fi

AC_SUBST(EXTRA_CPPFLAGS)

])dnl
dnl ---------------------------------------------------------------------------
dnl CF_ADD_INCDIR version: 9 updated: 2008/02/09 13:15:34
dnl -------------
dnl Add an include-directory to $CPPFLAGS.  Don't add /usr/include, since it's
dnl redundant.  We don't normally need to add -I/usr/local/include for gcc,
dnl but old versions (and some misinstalled ones) need that.  To make things
dnl worse, gcc 3.x may give error messages if -I/usr/local/include is added to
dnl the include-path).
AC_DEFUN([CF_ADD_INCDIR],
[
if test -n "$1" ; then
  for cf_add_incdir in $1
  do
	while test $cf_add_incdir != /usr/include
	do
	  if test -d $cf_add_incdir
	  then
		cf_have_incdir=no
		if test -n "$CFLAGS$CPPFLAGS" ; then
		  # a loop is needed to ensure we can add subdirs of existing dirs
		  for cf_test_incdir in $CFLAGS $CPPFLAGS ; do
			if test ".$cf_test_incdir" = ".-I$cf_add_incdir" ; then
			  cf_have_incdir=yes; break
			fi
		  done
		fi

		if test "$cf_have_incdir" = no ; then
          if test "$cf_add_incdir" = /usr/local/include ; then
			if test "$GCC" = yes
			then
			  cf_save_CPPFLAGS=$CPPFLAGS
			  CPPFLAGS="$CPPFLAGS -I$cf_add_incdir"
			  AC_TRY_COMPILE([#include <stdio.h>],
				  [printf("Hello")],
				  [],
				  [cf_have_incdir=yes])
			  CPPFLAGS=$cf_save_CPPFLAGS
			fi
		  fi
		fi

		if test "$cf_have_incdir" = no ; then
		  CF_VERBOSE(adding $cf_add_incdir to include-path)
		  ifelse($2,,CPPFLAGS,$2)="-I$cf_add_incdir $ifelse($2,,CPPFLAGS,[$]$2)"

          cf_top_incdir=`echo $cf_add_incdir | sed -e 's%/include/.*$%/include%'`
          test "$cf_top_incdir" = "$cf_add_incdir" && break
          cf_add_incdir="$cf_top_incdir"
		else
		  break
		fi
	  fi
	done
  done
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_ADD_LIBDIR version: 6 updated: 2008/02/09 13:15:34
dnl -------------
dnl	Adds to the library-path
dnl
dnl	Some machines have trouble with multiple -L options.
dnl
dnl $1 is the (list of) directory(s) to add
dnl $2 is the optional name of the variable to update (default LDFLAGS)
dnl
AC_DEFUN([CF_ADD_LIBDIR],
[
if test -n "$1" ; then
  for cf_add_libdir in $1
  do
    if test $cf_add_libdir = /usr/lib ; then
      :
    elif test -d $cf_add_libdir
    then
      cf_have_libdir=no
      if test -n "$LDFLAGS$LIBS" ; then
        # a loop is needed to ensure we can add subdirs of existing dirs
        for cf_test_libdir in $LDFLAGS $LIBS ; do
          if test ".$cf_test_libdir" = ".-L$cf_add_libdir" ; then
            cf_have_libdir=yes; break
          fi
        done
      fi
      if test "$cf_have_libdir" = no ; then
        CF_VERBOSE(adding $cf_add_libdir to library-path)
        ifelse($2,,LDFLAGS,$2)="-L$cf_add_libdir $ifelse($2,,LDFLAGS,[$]$2)"
      fi
    fi
  done
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_ADD_SUBDIR_PATH version: 2 updated: 2007/07/29 10:12:59
dnl ------------------
dnl Append to a search-list for a nonstandard header/lib-file
dnl	$1 = the variable to return as result
dnl	$2 = the package name
dnl	$3 = the subdirectory, e.g., bin, include or lib
dnl $4 = the directory under which we will test for subdirectories
dnl $5 = a directory that we do not want $4 to match
AC_DEFUN([CF_ADD_SUBDIR_PATH],
[
test "$4" != "$5" && \
test -d "$4" && \
ifelse([$5],NONE,,[(test $5 = NONE || test -d $5) &&]) {
	test -n "$verbose" && echo "	... testing for $3-directories under $4"
	test -d $4/$3 &&          $1="[$]$1 $4/$3"
	test -d $4/$3/$2 &&       $1="[$]$1 $4/$3/$2"
	test -d $4/$3/$2/$3 &&    $1="[$]$1 $4/$3/$2/$3"
	test -d $4/$2/$3 &&       $1="[$]$1 $4/$2/$3"
	test -d $4/$2/$3/$2 &&    $1="[$]$1 $4/$2/$3/$2"
}
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_ANSI_CC_CHECK version: 9 updated: 2001/12/30 17:53:34
dnl ----------------
dnl This is adapted from the macros 'fp_PROG_CC_STDC' and 'fp_C_PROTOTYPES'
dnl in the sharutils 4.2 distribution.
AC_DEFUN([CF_ANSI_CC_CHECK],
[
AC_CACHE_CHECK(for ${CC-cc} option to accept ANSI C, cf_cv_ansi_cc,[
cf_cv_ansi_cc=no
cf_save_CFLAGS="$CFLAGS"
cf_save_CPPFLAGS="$CPPFLAGS"
# Don't try gcc -ansi; that turns off useful extensions and
# breaks some systems' header files.
# AIX			-qlanglvl=ansi
# Ultrix and OSF/1	-std1
# HP-UX			-Aa -D_HPUX_SOURCE
# SVR4			-Xc
# UnixWare 1.2		(cannot use -Xc, since ANSI/POSIX clashes)
for cf_arg in "-DCC_HAS_PROTOS" \
	"" \
	-qlanglvl=ansi \
	-std1 \
	-Ae \
	"-Aa -D_HPUX_SOURCE" \
	-Xc
do
	CF_ADD_CFLAGS($cf_arg)
	AC_TRY_COMPILE(
[
#ifndef CC_HAS_PROTOS
#if !defined(__STDC__) || (__STDC__ != 1)
choke me
#endif
#endif
],[
	int test (int i, double x);
	struct s1 {int (*f) (int a);};
	struct s2 {int (*f) (double a);};],
	[cf_cv_ansi_cc="$cf_arg"; break])
done
CFLAGS="$cf_save_CFLAGS"
CPPFLAGS="$cf_save_CPPFLAGS"
])

if test "$cf_cv_ansi_cc" != "no"; then
if test ".$cf_cv_ansi_cc" != ".-DCC_HAS_PROTOS"; then
	CF_ADD_CFLAGS($cf_cv_ansi_cc)
else
	AC_DEFINE(CC_HAS_PROTOS)
fi
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_ANSI_CC_REQD version: 4 updated: 2008/03/23 14:48:54
dnl ---------------
dnl For programs that must use an ANSI compiler, obtain compiler options that
dnl will make it recognize prototypes.  We'll do preprocessor checks in other
dnl macros, since tools such as unproto can fake prototypes, but only part of
dnl the preprocessor.
AC_DEFUN([CF_ANSI_CC_REQD],
[AC_REQUIRE([CF_ANSI_CC_CHECK])
if test "$cf_cv_ansi_cc" = "no"; then
	AC_MSG_ERROR(
[Your compiler does not appear to recognize prototypes.
You have the following choices:
	a. adjust your compiler options
	b. get an up-to-date compiler
	c. use a wrapper such as unproto])
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_ARG_DISABLE version: 3 updated: 1999/03/30 17:24:31
dnl --------------
dnl Allow user to disable a normally-on option.
AC_DEFUN([CF_ARG_DISABLE],
[CF_ARG_OPTION($1,[$2],[$3],[$4],yes)])dnl
dnl ---------------------------------------------------------------------------
dnl CF_ARG_ENABLE version: 3 updated: 1999/03/30 17:24:31
dnl -------------
dnl Allow user to enable a normally-off option.
AC_DEFUN([CF_ARG_ENABLE],
[CF_ARG_OPTION($1,[$2],[$3],[$4],no)])dnl
dnl ---------------------------------------------------------------------------
dnl CF_ARG_OPTION version: 3 updated: 1997/10/18 14:42:41
dnl -------------
dnl Restricted form of AC_ARG_ENABLE that ensures user doesn't give bogus
dnl values.
dnl
dnl Parameters:
dnl $1 = option name
dnl $2 = help-string
dnl $3 = action to perform if option is not default
dnl $4 = action if perform if option is default
dnl $5 = default option value (either 'yes' or 'no')
AC_DEFUN([CF_ARG_OPTION],
[AC_ARG_ENABLE($1,[$2],[test "$enableval" != ifelse($5,no,yes,no) && enableval=ifelse($5,no,no,yes)
  if test "$enableval" != "$5" ; then
ifelse($3,,[    :]dnl
,[    $3]) ifelse($4,,,[
  else
    $4])
  fi],[enableval=$5 ifelse($4,,,[
  $4
])dnl
  ])])dnl
dnl ---------------------------------------------------------------------------
dnl CF_CHECK_CACHE version: 11 updated: 2008/03/23 14:45:59
dnl --------------
dnl Check if we're accidentally using a cache from a different machine.
dnl Derive the system name, as a check for reusing the autoconf cache.
dnl
dnl If we've packaged config.guess and config.sub, run that (since it does a
dnl better job than uname).  Normally we'll use AC_CANONICAL_HOST, but allow
dnl an extra parameter that we may override, e.g., for AC_CANONICAL_SYSTEM
dnl which is useful in cross-compiles.
dnl
dnl Note: we would use $ac_config_sub, but that is one of the places where
dnl autoconf 2.5x broke compatibility with autoconf 2.13
AC_DEFUN([CF_CHECK_CACHE],
[
if test -f $srcdir/config.guess || test -f $ac_aux_dir/config.guess ; then
	ifelse([$1],,[AC_CANONICAL_HOST],[$1])
	system_name="$host_os"
else
	system_name="`(uname -s -r) 2>/dev/null`"
	if test -z "$system_name" ; then
		system_name="`(hostname) 2>/dev/null`"
	fi
fi
test -n "$system_name" && AC_DEFINE_UNQUOTED(SYSTEM_NAME,"$system_name")
AC_CACHE_VAL(cf_cv_system_name,[cf_cv_system_name="$system_name"])

test -z "$system_name" && system_name="$cf_cv_system_name"
test -n "$cf_cv_system_name" && AC_MSG_RESULT(Configuring for $cf_cv_system_name)

if test ".$system_name" != ".$cf_cv_system_name" ; then
	AC_MSG_RESULT(Cached system name ($system_name) does not agree with actual ($cf_cv_system_name))
	AC_MSG_ERROR("Please remove config.cache and try again.")
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_CHECK_CFLAGS version: 2 updated: 2001/12/30 19:09:58
dnl ---------------
dnl Conditionally add to $CFLAGS and $CPPFLAGS values which are derived from
dnl a build-configuration such as imake.  These have the pitfall that they
dnl often contain compiler-specific options which we cannot use, mixed with
dnl preprocessor options that we usually can.
AC_DEFUN([CF_CHECK_CFLAGS],
[
CF_VERBOSE(checking additions to CFLAGS)
cf_check_cflags="$CFLAGS"
cf_check_cppflags="$CPPFLAGS"
CF_ADD_CFLAGS($1,yes)
if test "$cf_check_cflags" != "$CFLAGS" ; then
AC_TRY_LINK([#include <stdio.h>],[printf("Hello world");],,
	[CF_VERBOSE(test-compile failed.  Undoing change to \$CFLAGS)
	 if test "$cf_check_cppflags" != "$CPPFLAGS" ; then
		 CF_VERBOSE(but keeping change to \$CPPFLAGS)
	 fi
	 CFLAGS="$cf_check_flags"])
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_CURSES_CHTYPE version: 6 updated: 2003/11/06 19:59:57
dnl ----------------
dnl Test if curses defines 'chtype' (usually a 'long' type for SysV curses).
AC_DEFUN([CF_CURSES_CHTYPE],
[
AC_REQUIRE([CF_CURSES_CPPFLAGS])dnl
AC_CACHE_CHECK(for chtype typedef,cf_cv_chtype_decl,[
	AC_TRY_COMPILE([#include <${cf_cv_ncurses_header-curses.h}>],
		[chtype foo],
		[cf_cv_chtype_decl=yes],
		[cf_cv_chtype_decl=no])])
if test $cf_cv_chtype_decl = yes ; then
	AC_DEFINE(HAVE_TYPE_CHTYPE)
	AC_CACHE_CHECK(if chtype is scalar or struct,cf_cv_chtype_type,[
		AC_TRY_COMPILE([#include <${cf_cv_ncurses_header-curses.h}>],
			[chtype foo; long x = foo],
			[cf_cv_chtype_type=scalar],
			[cf_cv_chtype_type=struct])])
	if test $cf_cv_chtype_type = scalar ; then
		AC_DEFINE(TYPE_CHTYPE_IS_SCALAR)
	fi
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_CURSES_CPPFLAGS version: 9 updated: 2006/02/04 19:44:43
dnl ------------------
dnl Look for the curses headers.
AC_DEFUN([CF_CURSES_CPPFLAGS],[

AC_CACHE_CHECK(for extra include directories,cf_cv_curses_incdir,[
cf_cv_curses_incdir=no
case $host_os in #(vi
hpux10.*) #(vi
	test -d /usr/include/curses_colr && \
	cf_cv_curses_incdir="-I/usr/include/curses_colr"
	;;
sunos3*|sunos4*)
	test -d /usr/5lib && \
	test -d /usr/5include && \
	cf_cv_curses_incdir="-I/usr/5include"
	;;
esac
])
test "$cf_cv_curses_incdir" != no && CPPFLAGS="$cf_cv_curses_incdir $CPPFLAGS"

CF_CURSES_HEADER
CF_TERM_HEADER
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_CURSES_FUNCS version: 13 updated: 2007/04/28 09:15:55
dnl ---------------
dnl Curses-functions are a little complicated, since a lot of them are macros.
AC_DEFUN([CF_CURSES_FUNCS],
[
AC_REQUIRE([CF_CURSES_CPPFLAGS])dnl
AC_REQUIRE([CF_XOPEN_CURSES])
AC_REQUIRE([CF_CURSES_TERM_H])
for cf_func in $1
do
	CF_UPPER(cf_tr_func,$cf_func)
	AC_MSG_CHECKING(for ${cf_func})
	CF_MSG_LOG(${cf_func})
	AC_CACHE_VAL(cf_cv_func_$cf_func,[
		eval cf_result='$ac_cv_func_'$cf_func
		if test ".$cf_result" != ".no"; then
			AC_TRY_LINK([
#ifdef HAVE_XCURSES
#include <xcurses.h>
char * XCursesProgramName = "test";
#else
#include <${cf_cv_ncurses_header-curses.h}>
#if defined(NCURSES_VERSION) && defined(HAVE_NCURSESW_TERM_H)
#include <ncursesw/term.h>
#else
#if defined(NCURSES_VERSION) && defined(HAVE_NCURSES_TERM_H)
#include <ncurses/term.h>
#else
#ifdef HAVE_TERM_H
#include <term.h>
#endif
#endif
#endif
#endif],
			[
#ifndef ${cf_func}
long foo = (long)(&${cf_func});
${cf_cv_main_return-return}(foo == 0);
#endif
			],
			[cf_result=yes],
			[cf_result=no])
		fi
		eval 'cf_cv_func_'$cf_func'=$cf_result'
	])
	# use the computed/retrieved cache-value:
	eval 'cf_result=$cf_cv_func_'$cf_func
	AC_MSG_RESULT($cf_result)
	if test $cf_result != no; then
		AC_DEFINE_UNQUOTED(HAVE_${cf_tr_func})
	fi
done
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_CURSES_HEADER version: 1 updated: 2005/12/31 13:28:25
dnl ----------------
dnl Find a "curses" header file, e.g,. "curses.h", or one of the more common
dnl variations of ncurses' installs.
dnl
dnl See also CF_NCURSES_HEADER, which sets the same cache variable.
AC_DEFUN([CF_CURSES_HEADER],[
AC_CACHE_CHECK(if we have identified curses headers,cf_cv_ncurses_header,[
cf_cv_ncurses_header=none
for cf_header in \
	curses.h \
	ncurses.h \
	ncurses/curses.h \
	ncurses/ncurses.h
do
AC_TRY_COMPILE([#include <${cf_header}>],
	[initscr(); tgoto("?", 0,0)],
	[cf_cv_ncurses_header=$cf_header; break],[])
done
])

if test "$cf_cv_ncurses_header" = none ; then
	AC_MSG_ERROR(No curses header-files found)
fi

# cheat, to get the right #define's for HAVE_NCURSES_H, etc.
AC_CHECK_HEADERS($cf_cv_ncurses_header)
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_CURSES_LIBS version: 28 updated: 2008/10/30 20:16:05
dnl --------------
dnl Look for the curses libraries.  Older curses implementations may require
dnl termcap/termlib to be linked as well.  Call CF_CURSES_CPPFLAGS first.
AC_DEFUN([CF_CURSES_LIBS],[

AC_REQUIRE([CF_CURSES_CPPFLAGS])dnl
AC_MSG_CHECKING(if we have identified curses libraries)
AC_TRY_LINK([#include <${cf_cv_ncurses_header-curses.h}>],
    [initscr(); tgoto("?", 0,0)],
    cf_result=yes,
    cf_result=no)
AC_MSG_RESULT($cf_result)

if test "$cf_result" = no ; then
case $host_os in #(vi
freebsd*) #(vi
    AC_CHECK_LIB(mytinfo,tgoto,[LIBS="-lmytinfo $LIBS"])
    ;;
hpux10.*) #(vi
    AC_CHECK_LIB(cur_colr,initscr,[
        LIBS="-lcur_colr $LIBS"
        ac_cv_func_initscr=yes
        ],[
    AC_CHECK_LIB(Hcurses,initscr,[
        # HP's header uses __HP_CURSES, but user claims _HP_CURSES.
        LIBS="-lHcurses $LIBS"
        CPPFLAGS="-D__HP_CURSES -D_HP_CURSES $CPPFLAGS"
        ac_cv_func_initscr=yes
        ])])
    ;;
linux*) # Suse Linux does not follow /usr/lib convention
    CF_ADD_LIBDIR(/lib)
    ;;
sunos3*|sunos4*)
    if test -d /usr/5lib ; then
      CF_ADD_LIBDIR(/usr/5lib)
      LIBS="$LIBS -lcurses -ltermcap"
    fi
    ac_cv_func_initscr=yes
    ;;
esac

if test ".$ac_cv_func_initscr" != .yes ; then
    cf_save_LIBS="$LIBS"
    cf_term_lib=""
    cf_curs_lib=""

    if test ".${cf_cv_ncurses_version-no}" != .no
    then
        cf_check_list="ncurses curses cursesX"
    else
        cf_check_list="cursesX curses ncurses"
    fi

    # Check for library containing tgoto.  Do this before curses library
    # because it may be needed to link the test-case for initscr.
    AC_CHECK_FUNC(tgoto,[cf_term_lib=predefined],[
        for cf_term_lib in $cf_check_list termcap termlib unknown
        do
            AC_CHECK_LIB($cf_term_lib,tgoto,[break])
        done
    ])

    # Check for library containing initscr
    test "$cf_term_lib" != predefined && test "$cf_term_lib" != unknown && LIBS="-l$cf_term_lib $cf_save_LIBS"
 	for cf_curs_lib in $cf_check_list xcurses jcurses pdcurses unknown
    do
        AC_CHECK_LIB($cf_curs_lib,initscr,[break])
    done
    test $cf_curs_lib = unknown && AC_MSG_ERROR(no curses library found)

    LIBS="-l$cf_curs_lib $cf_save_LIBS"
    if test "$cf_term_lib" = unknown ; then
        AC_MSG_CHECKING(if we can link with $cf_curs_lib library)
        AC_TRY_LINK([#include <${cf_cv_ncurses_header-curses.h}>],
            [initscr()],
            [cf_result=yes],
            [cf_result=no])
        AC_MSG_RESULT($cf_result)
        test $cf_result = no && AC_MSG_ERROR(Cannot link curses library)
    elif test "$cf_curs_lib" = "$cf_term_lib" ; then
        :
    elif test "$cf_term_lib" != predefined ; then
        AC_MSG_CHECKING(if we need both $cf_curs_lib and $cf_term_lib libraries)
        AC_TRY_LINK([#include <${cf_cv_ncurses_header-curses.h}>],
            [initscr(); tgoto((char *)0, 0, 0);],
            [cf_result=no],
            [
            LIBS="-l$cf_curs_lib -l$cf_term_lib $cf_save_LIBS"
            AC_TRY_LINK([#include <${cf_cv_ncurses_header-curses.h}>],
                [initscr()],
                [cf_result=yes],
                [cf_result=error])
            ])
        AC_MSG_RESULT($cf_result)
    fi
fi
fi

])dnl
dnl ---------------------------------------------------------------------------
dnl CF_CURSES_SETBEGYX version: 1 updated: 2002/07/22 22:38:19
dnl ------------------
dnl See if we can define a function to set a window's beginning y/x coordinates.
AC_DEFUN([CF_CURSES_SETBEGYX],[
AC_MSG_CHECKING(how to define setbegyx)
cf_result=ERR
for cf_check in \
	'((win)->_begy = (y), (win)->_begx = (x), OK)'
do
	AC_TRY_COMPILE([
#include <${cf_cv_ncurses_header-curses.h}>
],[
WINDOW *win = 0;
#define setbegyx(win,y,x) $cf_check
setbegyx(win, 2, 3);
],[cf_result=$cf_check; break])
done
AC_MSG_RESULT("$cf_result")
AC_DEFINE_UNQUOTED(setbegyx(win,y,x),$cf_result)
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_CURSES_TERM_H version: 6 updated: 2003/11/06 19:59:57
dnl ----------------
dnl SVr4 curses should have term.h as well (where it puts the definitions of
dnl the low-level interface).  This may not be true in old/broken implementations,
dnl as well as in misconfigured systems (e.g., gcc configured for Solaris 2.4
dnl running with Solaris 2.5.1).
AC_DEFUN([CF_CURSES_TERM_H],
[
AC_CACHE_CHECK(for term.h, cf_cv_term_header,[

AC_REQUIRE([CF_CURSES_CPPFLAGS])dnl
# If we found <ncurses/curses.h>, look for <ncurses/term.h>, but always look
# for <term.h> if we do not find the variant.
for cf_header in \
	`echo ${cf_cv_ncurses_header-curses.h} | sed -e 's%/.*%/%'`term.h \
	term.h
do
	AC_TRY_COMPILE([
#include <${cf_cv_ncurses_header-curses.h}>
#include <${cf_header}>],
	[WINDOW *x],
	[cf_cv_term_header=$cf_header
	 break],
	[cf_cv_term_header=no])
done
])

case $cf_cv_term_header in #(vi
term.h) #(vi
	AC_DEFINE(HAVE_TERM_H)
	;;
ncurses/term.h)
	AC_DEFINE(HAVE_NCURSES_TERM_H)
	;;
ncursesw/term.h)
	AC_DEFINE(HAVE_NCURSESW_TERM_H)
	;;
esac
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_DIRNAME version: 4 updated: 2002/12/21 19:25:52
dnl ----------
dnl "dirname" is not portable, so we fake it with a shell script.
AC_DEFUN([CF_DIRNAME],[$1=`echo $2 | sed -e 's%/[[^/]]*$%%'`])dnl
dnl ---------------------------------------------------------------------------
dnl CF_DISABLE_ECHO version: 10 updated: 2003/04/17 22:27:11
dnl ---------------
dnl You can always use "make -n" to see the actual options, but it's hard to
dnl pick out/analyze warning messages when the compile-line is long.
dnl
dnl Sets:
dnl	ECHO_LT - symbol to control if libtool is verbose
dnl	ECHO_LD - symbol to prefix "cc -o" lines
dnl	RULE_CC - symbol to put before implicit "cc -c" lines (e.g., .c.o)
dnl	SHOW_CC - symbol to put before explicit "cc -c" lines
dnl	ECHO_CC - symbol to put before any "cc" line
dnl
AC_DEFUN([CF_DISABLE_ECHO],[
AC_MSG_CHECKING(if you want to see long compiling messages)
CF_ARG_DISABLE(echo,
	[  --disable-echo          display "compiling" commands],
	[
    ECHO_LT='--silent'
    ECHO_LD='@echo linking [$]@;'
    RULE_CC='	@echo compiling [$]<'
    SHOW_CC='	@echo compiling [$]@'
    ECHO_CC='@'
],[
    ECHO_LT=''
    ECHO_LD=''
    RULE_CC='# compiling'
    SHOW_CC='# compiling'
    ECHO_CC=''
])
AC_MSG_RESULT($enableval)
AC_SUBST(ECHO_LT)
AC_SUBST(ECHO_LD)
AC_SUBST(RULE_CC)
AC_SUBST(SHOW_CC)
AC_SUBST(ECHO_CC)
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_DISABLE_LEAKS version: 4 updated: 2006/12/16 15:10:42
dnl ----------------
dnl Combine no-leak checks with the libraries or tools that are used for the
dnl checks.
AC_DEFUN([CF_DISABLE_LEAKS],[

AC_REQUIRE([CF_WITH_DMALLOC])
AC_REQUIRE([CF_WITH_DBMALLOC])
AC_REQUIRE([CF_WITH_VALGRIND])

AC_MSG_CHECKING(if you want to perform memory-leak testing)
AC_ARG_ENABLE(leaks,
	[  --disable-leaks         test: free permanent memory, analyze leaks],
	[with_no_leaks=yes],
	: ${with_no_leaks:=no})
AC_MSG_RESULT($with_no_leaks)

if test "$with_no_leaks" = yes ; then
	AC_DEFINE(NO_LEAKS)
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_FIND_LIBRARY version: 9 updated: 2008/03/23 14:48:54
dnl ---------------
dnl Look for a non-standard library, given parameters for AC_TRY_LINK.  We
dnl prefer a standard location, and use -L options only if we do not find the
dnl library in the standard library location(s).
dnl	$1 = library name
dnl	$2 = library class, usually the same as library name
dnl	$3 = includes
dnl	$4 = code fragment to compile/link
dnl	$5 = corresponding function-name
dnl	$6 = flag, nonnull if failure should not cause an error-exit
dnl
dnl Sets the variable "$cf_libdir" as a side-effect, so we can see if we had
dnl to use a -L option.
AC_DEFUN([CF_FIND_LIBRARY],
[
	eval 'cf_cv_have_lib_'$1'=no'
	cf_libdir=""
	AC_CHECK_FUNC($5,
		eval 'cf_cv_have_lib_'$1'=yes',[
		cf_save_LIBS="$LIBS"
		AC_MSG_CHECKING(for $5 in -l$1)
		LIBS="-l$1 $LIBS"
		AC_TRY_LINK([$3],[$4],
			[AC_MSG_RESULT(yes)
			 eval 'cf_cv_have_lib_'$1'=yes'
			],
			[AC_MSG_RESULT(no)
			CF_LIBRARY_PATH(cf_search,$2)
			for cf_libdir in $cf_search
			do
				AC_MSG_CHECKING(for -l$1 in $cf_libdir)
				LIBS="-L$cf_libdir -l$1 $cf_save_LIBS"
				AC_TRY_LINK([$3],[$4],
					[AC_MSG_RESULT(yes)
			 		 eval 'cf_cv_have_lib_'$1'=yes'
					 break],
					[AC_MSG_RESULT(no)
					 LIBS="$cf_save_LIBS"])
			done
			])
		])
eval 'cf_found_library=[$]cf_cv_have_lib_'$1
ifelse($6,,[
if test $cf_found_library = no ; then
	AC_MSG_ERROR(Cannot link $1 library)
fi
])
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_FIND_LINKAGE version: 12 updated: 2007/07/29 20:13:53
dnl ---------------
dnl Find a library (specifically the linkage used in the code fragment),
dnl searching for it if it is not already in the library path.
dnl See also CF_ADD_SEARCHPATH.
dnl
dnl Parameters (4-on are optional):
dnl     $1 = headers for library entrypoint
dnl     $2 = code fragment for library entrypoint
dnl     $3 = the library name without the "-l" option or ".so" suffix.
dnl     $4 = action to perform if successful (default: update CPPFLAGS, etc)
dnl     $5 = action to perform if not successful
dnl     $6 = module name, if not the same as the library name
dnl     $7 = extra libraries
dnl
dnl Sets these variables:
dnl     $cf_cv_find_linkage_$3 - yes/no according to whether linkage is found
dnl     $cf_cv_header_path_$3 - include-directory if needed
dnl     $cf_cv_library_path_$3 - library-directory if needed
dnl     $cf_cv_library_file_$3 - library-file if needed, e.g., -l$3
AC_DEFUN([CF_FIND_LINKAGE],[

# If the linkage is not already in the $CPPFLAGS/$LDFLAGS configuration, these
# will be set on completion of the AC_TRY_LINK below.
cf_cv_header_path_$3=
cf_cv_library_path_$3=

CF_MSG_LOG([Starting [FIND_LINKAGE]($3,$6)])

AC_TRY_LINK([$1],[$2],
    cf_cv_find_linkage_$3=yes,[
    cf_cv_find_linkage_$3=no

    CF_MSG_LOG([Searching for headers in [FIND_LINKAGE]($3,$6)])

    cf_save_CPPFLAGS="$CPPFLAGS"
    cf_test_CPPFLAGS="$CPPFLAGS"

    CF_HEADER_PATH(cf_search,ifelse([$6],,[$3],[$6]))
    for cf_cv_header_path_$3 in $cf_search
    do
      if test -d $cf_cv_header_path_$3 ; then
        CF_VERBOSE(... testing $cf_cv_header_path_$3)
        CPPFLAGS="$cf_save_CPPFLAGS -I$cf_cv_header_path_$3"
        AC_TRY_COMPILE([$1],[$2],[
            CF_VERBOSE(... found $3 headers in $cf_cv_header_path_$3)
            cf_cv_find_linkage_$3=maybe
            cf_test_CPPFLAGS="$CPPFLAGS"
            break],[
            CPPFLAGS="$cf_save_CPPFLAGS"
            ])
      fi
    done

    if test "$cf_cv_find_linkage_$3" = maybe ; then

      CF_MSG_LOG([Searching for $3 library in [FIND_LINKAGE]($3,$6)])

      cf_save_LIBS="$LIBS"
      cf_save_LDFLAGS="$LDFLAGS"

      ifelse([$6],,,[
        CPPFLAGS="$cf_test_CPPFLAGS"
        LIBS="-l$3 $7 $cf_save_LIBS"
        AC_TRY_LINK([$1],[$2],[
            CF_VERBOSE(... found $3 library in system)
            cf_cv_find_linkage_$3=yes])
            CPPFLAGS="$cf_save_CPPFLAGS"
            LIBS="$cf_save_LIBS"
            ])

      if test "$cf_cv_find_linkage_$3" != yes ; then
        CF_LIBRARY_PATH(cf_search,$3)
        for cf_cv_library_path_$3 in $cf_search
        do
          if test -d $cf_cv_library_path_$3 ; then
            CF_VERBOSE(... testing $cf_cv_library_path_$3)
            CPPFLAGS="$cf_test_CPPFLAGS"
            LIBS="-l$3 $7 $cf_save_LIBS"
            LDFLAGS="$cf_save_LDFLAGS -L$cf_cv_library_path_$3"
            AC_TRY_LINK([$1],[$2],[
                CF_VERBOSE(... found $3 library in $cf_cv_library_path_$3)
                cf_cv_find_linkage_$3=yes
                cf_cv_library_file_$3="-l$3"
                break],[
                CPPFLAGS="$cf_save_CPPFLAGS"
                LIBS="$cf_save_LIBS"
                LDFLAGS="$cf_save_LDFLAGS"
                ])
          fi
        done
        LIBS="$cf_save_LIBS"
        CPPFLAGS="$cf_save_CPPFLAGS"
        LDFLAGS="$cf_save_LDFLAGS"
      fi

    else
      cf_cv_find_linkage_$3=no
    fi
    ],$7)

if test "$cf_cv_find_linkage_$3" = yes ; then
ifelse([$4],,[
  CF_ADD_INCDIR($cf_cv_header_path_$3)
  CF_ADD_LIBDIR($cf_cv_library_path_$3)
  LIBS="-l$3 $LIBS"
],[$4])
else
ifelse([$5],,AC_MSG_WARN(Cannot find $3 library),[$5])
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_FUNC_LSTAT version: 2 updated: 1997/09/07 13:00:21
dnl -------------
dnl A conventional existence-check for 'lstat' won't work with the Linux
dnl version of gcc 2.7.0, since the symbol is defined only within <sys/stat.h>
dnl as an inline function.
dnl
dnl So much for portability.
AC_DEFUN([CF_FUNC_LSTAT],
[
AC_MSG_CHECKING(for lstat)
AC_CACHE_VAL(ac_cv_func_lstat,[
AC_TRY_LINK([
#include <sys/types.h>
#include <sys/stat.h>],
	[lstat(".", (struct stat *)0)],
	[ac_cv_func_lstat=yes],
	[ac_cv_func_lstat=no])
	])
AC_MSG_RESULT($ac_cv_func_lstat )
if test $ac_cv_func_lstat = yes; then
	AC_DEFINE(HAVE_LSTAT)
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_GCC_ATTRIBUTES version: 11 updated: 2007/07/29 09:55:12
dnl -----------------
dnl Test for availability of useful gcc __attribute__ directives to quiet
dnl compiler warnings.  Though useful, not all are supported -- and contrary
dnl to documentation, unrecognized directives cause older compilers to barf.
AC_DEFUN([CF_GCC_ATTRIBUTES],
[
if test "$GCC" = yes
then
cat > conftest.i <<EOF
#ifndef GCC_PRINTF
#define GCC_PRINTF 0
#endif
#ifndef GCC_SCANF
#define GCC_SCANF 0
#endif
#ifndef GCC_NORETURN
#define GCC_NORETURN /* nothing */
#endif
#ifndef GCC_UNUSED
#define GCC_UNUSED /* nothing */
#endif
EOF
if test "$GCC" = yes
then
	AC_CHECKING([for $CC __attribute__ directives])
cat > conftest.$ac_ext <<EOF
#line __oline__ "${as_me-configure}"
#include "confdefs.h"
#include "conftest.h"
#include "conftest.i"
#if	GCC_PRINTF
#define GCC_PRINTFLIKE(fmt,var) __attribute__((format(printf,fmt,var)))
#else
#define GCC_PRINTFLIKE(fmt,var) /*nothing*/
#endif
#if	GCC_SCANF
#define GCC_SCANFLIKE(fmt,var)  __attribute__((format(scanf,fmt,var)))
#else
#define GCC_SCANFLIKE(fmt,var)  /*nothing*/
#endif
extern void wow(char *,...) GCC_SCANFLIKE(1,2);
extern void oops(char *,...) GCC_PRINTFLIKE(1,2) GCC_NORETURN;
extern void foo(void) GCC_NORETURN;
int main(int argc GCC_UNUSED, char *argv[[]] GCC_UNUSED) { return 0; }
EOF
	for cf_attribute in scanf printf unused noreturn
	do
		CF_UPPER(cf_ATTRIBUTE,$cf_attribute)
		cf_directive="__attribute__(($cf_attribute))"
		echo "checking for $CC $cf_directive" 1>&AC_FD_CC
		case $cf_attribute in
		scanf|printf)
		cat >conftest.h <<EOF
#define GCC_$cf_ATTRIBUTE 1
EOF
			;;
		*)
		cat >conftest.h <<EOF
#define GCC_$cf_ATTRIBUTE $cf_directive
EOF
			;;
		esac
		if AC_TRY_EVAL(ac_compile); then
			test -n "$verbose" && AC_MSG_RESULT(... $cf_attribute)
			cat conftest.h >>confdefs.h
		fi
	done
else
	fgrep define conftest.i >>confdefs.h
fi
rm -rf conftest*
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_GCC_VERSION version: 4 updated: 2005/08/27 09:53:42
dnl --------------
dnl Find version of gcc
AC_DEFUN([CF_GCC_VERSION],[
AC_REQUIRE([AC_PROG_CC])
GCC_VERSION=none
if test "$GCC" = yes ; then
	AC_MSG_CHECKING(version of $CC)
	GCC_VERSION="`${CC} --version| sed -e '2,$d' -e 's/^.*(GCC) //' -e 's/^[[^0-9.]]*//' -e 's/[[^0-9.]].*//'`"
	test -z "$GCC_VERSION" && GCC_VERSION=unknown
	AC_MSG_RESULT($GCC_VERSION)
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_GCC_WARNINGS version: 23 updated: 2008/07/26 17:54:02
dnl ---------------
dnl Check if the compiler supports useful warning options.  There's a few that
dnl we don't use, simply because they're too noisy:
dnl
dnl	-Wconversion (useful in older versions of gcc, but not in gcc 2.7.x)
dnl	-Wredundant-decls (system headers make this too noisy)
dnl	-Wtraditional (combines too many unrelated messages, only a few useful)
dnl	-Wwrite-strings (too noisy, but should review occasionally).  This
dnl		is enabled for ncurses using "--enable-const".
dnl	-pedantic
dnl
dnl Parameter:
dnl	$1 is an optional list of gcc warning flags that a particular
dnl		application might want to use, e.g., "no-unused" for
dnl		-Wno-unused
dnl Special:
dnl	If $with_ext_const is "yes", add a check for -Wwrite-strings
dnl
AC_DEFUN([CF_GCC_WARNINGS],
[
AC_REQUIRE([CF_GCC_VERSION])
CF_INTEL_COMPILER(GCC,INTEL_COMPILER,CFLAGS)

cat > conftest.$ac_ext <<EOF
#line __oline__ "${as_me-configure}"
int main(int argc, char *argv[[]]) { return (argv[[argc-1]] == 0) ; }
EOF

if test "$INTEL_COMPILER" = yes
then
# The "-wdXXX" options suppress warnings:
# remark #1419: external declaration in primary source file
# remark #1682: implicit conversion of a 64-bit integral type to a smaller integral type (potential portability problem)
# remark #1683: explicit conversion of a 64-bit integral type to a smaller integral type (potential portability problem)
# remark #1684: conversion from pointer to same-sized integral type (potential portability problem)
# remark #193: zero used for undefined preprocessing identifier
# remark #593: variable "curs_sb_left_arrow" was set but never used
# remark #810: conversion from "int" to "Dimension={unsigned short}" may lose significant bits
# remark #869: parameter "tw" was never referenced
# remark #981: operands are evaluated in unspecified order
# warning #269: invalid format string conversion

	AC_CHECKING([for $CC warning options])
	cf_save_CFLAGS="$CFLAGS"
	EXTRA_CFLAGS="-Wall"
	for cf_opt in \
		wd1419 \
		wd1682 \
		wd1683 \
		wd1684 \
		wd193 \
		wd279 \
		wd593 \
		wd810 \
		wd869 \
		wd981
	do
		CFLAGS="$cf_save_CFLAGS $EXTRA_CFLAGS -$cf_opt"
		if AC_TRY_EVAL(ac_compile); then
			test -n "$verbose" && AC_MSG_RESULT(... -$cf_opt)
			EXTRA_CFLAGS="$EXTRA_CFLAGS -$cf_opt"
		fi
	done
	CFLAGS="$cf_save_CFLAGS"

elif test "$GCC" = yes
then
	AC_CHECKING([for $CC warning options])
	cf_save_CFLAGS="$CFLAGS"
	EXTRA_CFLAGS="-W -Wall"
	cf_warn_CONST=""
	test "$with_ext_const" = yes && cf_warn_CONST="Wwrite-strings"
	for cf_opt in \
		Wbad-function-cast \
		Wcast-align \
		Wcast-qual \
		Winline \
		Wmissing-declarations \
		Wmissing-prototypes \
		Wnested-externs \
		Wpointer-arith \
		Wshadow \
		Wstrict-prototypes \
		Wundef $cf_warn_CONST $1
	do
		CFLAGS="$cf_save_CFLAGS $EXTRA_CFLAGS -$cf_opt"
		if AC_TRY_EVAL(ac_compile); then
			test -n "$verbose" && AC_MSG_RESULT(... -$cf_opt)
			case $cf_opt in #(vi
			Wcast-qual) #(vi
				CPPFLAGS="$CPPFLAGS -DXTSTRINGDEFINES"
				;;
			Winline) #(vi
				case $GCC_VERSION in
				[[34]].*)
					CF_VERBOSE(feature is broken in gcc $GCC_VERSION)
					continue;;
				esac
				;;
			esac
			EXTRA_CFLAGS="$EXTRA_CFLAGS -$cf_opt"
		fi
	done
	CFLAGS="$cf_save_CFLAGS"
fi
rm -f conftest*

AC_SUBST(EXTRA_CFLAGS)
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_GETOPT_HEADER version: 2 updated: 2000/02/19 01:46:34
dnl ----------------
dnl Check for getopt's variables which are commonly defined in stdlib.h or
dnl (nonstandard) in getopt.h
AC_DEFUN([CF_GETOPT_HEADER],
[
AC_CACHE_CHECK(for header declaring getopt variables,cf_cv_getopt_header,[
cf_cv_getopt_header=none
for cf_header in stdio.h stdlib.h getopt.h
do
AC_TRY_COMPILE([
#include <$cf_header>],
[int x = optind; char *y = optarg],
[cf_cv_getopt_header=$cf_header
 break])
done
])
case $cf_cv_getopt_header in #(vi
getopt.h) #(vi
	AC_DEFINE(HAVE_GETOPT_H)
	AC_DEFINE(HAVE_GETOPT_HEADER)
	;;
stdlib.h) #(vi
	AC_DEFINE(HAVE_STDLIB_H)
	AC_DEFINE(HAVE_GETOPT_HEADER)
	;;
esac
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_GNU_SOURCE version: 6 updated: 2005/07/09 13:23:07
dnl -------------
dnl Check if we must define _GNU_SOURCE to get a reasonable value for
dnl _XOPEN_SOURCE, upon which many POSIX definitions depend.  This is a defect
dnl (or misfeature) of glibc2, which breaks portability of many applications,
dnl since it is interwoven with GNU extensions.
dnl
dnl Well, yes we could work around it...
AC_DEFUN([CF_GNU_SOURCE],
[
AC_CACHE_CHECK(if we must define _GNU_SOURCE,cf_cv_gnu_source,[
AC_TRY_COMPILE([#include <sys/types.h>],[
#ifndef _XOPEN_SOURCE
make an error
#endif],
	[cf_cv_gnu_source=no],
	[cf_save="$CPPFLAGS"
	 CPPFLAGS="$CPPFLAGS -D_GNU_SOURCE"
	 AC_TRY_COMPILE([#include <sys/types.h>],[
#ifdef _XOPEN_SOURCE
make an error
#endif],
	[cf_cv_gnu_source=no],
	[cf_cv_gnu_source=yes])
	CPPFLAGS="$cf_save"
	])
])
test "$cf_cv_gnu_source" = yes && CPPFLAGS="$CPPFLAGS -D_GNU_SOURCE"
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_HEADER_PATH version: 8 updated: 2002/11/10 14:46:59
dnl --------------
dnl Construct a search-list for a nonstandard header-file
AC_DEFUN([CF_HEADER_PATH],
[CF_SUBDIR_PATH($1,$2,include)
test "$includedir" != NONE && \
test "$includedir" != "/usr/include" && \
test -d "$includedir" && {
	test -d $includedir &&    $1="[$]$1 $includedir"
	test -d $includedir/$2 && $1="[$]$1 $includedir/$2"
}

test "$oldincludedir" != NONE && \
test "$oldincludedir" != "/usr/include" && \
test -d "$oldincludedir" && {
	test -d $oldincludedir    && $1="[$]$1 $oldincludedir"
	test -d $oldincludedir/$2 && $1="[$]$1 $oldincludedir/$2"
}

])dnl
dnl ---------------------------------------------------------------------------
dnl CF_HELP_MESSAGE version: 3 updated: 1998/01/14 10:56:23
dnl ---------------
dnl Insert text into the help-message, for readability, from AC_ARG_WITH.
AC_DEFUN([CF_HELP_MESSAGE],
[AC_DIVERT_HELP([$1])dnl
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_INCLUDE_DIRS version: 5 updated: 2006/10/14 15:23:15
dnl ---------------
dnl Construct the list of include-options according to whether we're building
dnl in the source directory or using '--srcdir=DIR' option.  If we're building
dnl with gcc, don't append the includedir if it happens to be /usr/include,
dnl since that usually breaks gcc's shadow-includes.
AC_DEFUN([CF_INCLUDE_DIRS],
[
CPPFLAGS="-I. -I../include $CPPFLAGS"
if test "$srcdir" != "."; then
	CPPFLAGS="-I\${srcdir}/../include $CPPFLAGS"
fi
if test "$GCC" != yes; then
	CPPFLAGS="$CPPFLAGS -I\${includedir}"
elif test "$includedir" != "/usr/include"; then
	if test "$includedir" = '${prefix}/include' ; then
		if test $prefix != /usr ; then
			CPPFLAGS="$CPPFLAGS -I\${includedir}"
		fi
	else
		CPPFLAGS="$CPPFLAGS -I\${includedir}"
	fi
fi
AC_SUBST(CPPFLAGS)
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_INCLUDE_PATH version: 4 updated: 2001/04/15 21:21:45
dnl ---------------
dnl	Adds to the include-path
dnl
dnl	Autoconf 1.11 should have provided a way to add include path options to
dnl	the cpp-tests.
dnl
AC_DEFUN([CF_INCLUDE_PATH],
[
for cf_path in $1
do
	cf_result="no"
	AC_MSG_CHECKING(for directory $cf_path)
	if test -d $cf_path
	then
		INCLUDES="$INCLUDES -I$cf_path"
		ac_cpp="${ac_cpp} -I$cf_path"
		CFLAGS="$CFLAGS -I$cf_path"
		cf_result="yes"
		case $cf_path in
		/usr/include|/usr/include/*)
			;;
		*)
			CF_DIRNAME(cf_temp,$cf_path)
			case $cf_temp in
			*/include)
				INCLUDES="$INCLUDES -I$cf_temp"
				ac_cpp="${ac_cpp} -I$cf_temp"
				CFLAGS="$CFLAGS -I$cf_temp"
				;;
			esac
		esac
	fi
	AC_MSG_RESULT($cf_result)
done
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_INTEL_COMPILER version: 3 updated: 2005/08/06 18:37:29
dnl -----------------
dnl Check if the given compiler is really the Intel compiler for Linux.  It
dnl tries to imitate gcc, but does not return an error when it finds a mismatch
dnl between prototypes, e.g., as exercised by CF_MISSING_CHECK.
dnl
dnl This macro should be run "soon" after AC_PROG_CC or AC_PROG_CPLUSPLUS, to
dnl ensure that it is not mistaken for gcc/g++.  It is normally invoked from
dnl the wrappers for gcc and g++ warnings.
dnl
dnl $1 = GCC (default) or GXX
dnl $2 = INTEL_COMPILER (default) or INTEL_CPLUSPLUS
dnl $3 = CFLAGS (default) or CXXFLAGS
AC_DEFUN([CF_INTEL_COMPILER],[
ifelse($2,,INTEL_COMPILER,[$2])=no

if test "$ifelse($1,,[$1],GCC)" = yes ; then
	case $host_os in
	linux*|gnu*)
		AC_MSG_CHECKING(if this is really Intel ifelse($1,GXX,C++,C) compiler)
		cf_save_CFLAGS="$ifelse($3,,CFLAGS,[$3])"
		ifelse($3,,CFLAGS,[$3])="$ifelse($3,,CFLAGS,[$3]) -no-gcc"
		AC_TRY_COMPILE([],[
#ifdef __INTEL_COMPILER
#else
make an error
#endif
],[ifelse($2,,INTEL_COMPILER,[$2])=yes
cf_save_CFLAGS="$cf_save_CFLAGS -we147 -no-gcc"
],[])
		ifelse($3,,CFLAGS,[$3])="$cf_save_CFLAGS"
		AC_MSG_RESULT($ifelse($2,,INTEL_COMPILER,[$2]))
		;;
	esac
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_LIBRARY_PATH version: 7 updated: 2002/11/10 14:46:59
dnl ---------------
dnl Construct a search-list for a nonstandard library-file
AC_DEFUN([CF_LIBRARY_PATH],
[CF_SUBDIR_PATH($1,$2,lib)])dnl
dnl ---------------------------------------------------------------------------
dnl CF_LIB_PREFIX version: 8 updated: 2008/09/13 11:34:16
dnl -------------
dnl Compute the library-prefix for the given host system
dnl $1 = variable to set
AC_DEFUN([CF_LIB_PREFIX],
[
	case $cf_cv_system_name in #(vi
	OS/2*|os2*) #(vi
        LIB_PREFIX=''
        ;;
	*)	LIB_PREFIX='lib'
        ;;
	esac
ifelse($1,,,[$1=$LIB_PREFIX])
	AC_SUBST(LIB_PREFIX)
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_LOCALE version: 4 updated: 2003/02/16 08:16:04
dnl ---------
dnl Check if we have setlocale() and its header, <locale.h>
dnl The optional parameter $1 tells what to do if we do have locale support.
AC_DEFUN([CF_LOCALE],
[
AC_MSG_CHECKING(for setlocale())
AC_CACHE_VAL(cf_cv_locale,[
AC_TRY_LINK([#include <locale.h>],
	[setlocale(LC_ALL, "")],
	[cf_cv_locale=yes],
	[cf_cv_locale=no])
	])
AC_MSG_RESULT($cf_cv_locale)
test $cf_cv_locale = yes && { ifelse($1,,AC_DEFINE(LOCALE),[$1]) }
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_MAKEFLAGS version: 12 updated: 2006/10/21 08:27:03
dnl ------------
dnl Some 'make' programs support ${MAKEFLAGS}, some ${MFLAGS}, to pass 'make'
dnl options to lower-levels.  It's very useful for "make -n" -- if we have it.
dnl (GNU 'make' does both, something POSIX 'make', which happens to make the
dnl ${MAKEFLAGS} variable incompatible because it adds the assignments :-)
AC_DEFUN([CF_MAKEFLAGS],
[
AC_CACHE_CHECK(for makeflags variable, cf_cv_makeflags,[
	cf_cv_makeflags=''
	for cf_option in '-${MAKEFLAGS}' '${MFLAGS}'
	do
		cat >cf_makeflags.tmp <<CF_EOF
SHELL = /bin/sh
all :
	@ echo '.$cf_option'
CF_EOF
		cf_result=`${MAKE-make} -k -f cf_makeflags.tmp 2>/dev/null | sed -e 's,[[ 	]]*$,,'`
		case "$cf_result" in
		.*k)
			cf_result=`${MAKE-make} -k -f cf_makeflags.tmp CC=cc 2>/dev/null`
			case "$cf_result" in
			.*CC=*)	cf_cv_makeflags=
				;;
			*)	cf_cv_makeflags=$cf_option
				;;
			esac
			break
			;;
		.-)	;;
		*)	echo "given option \"$cf_option\", no match \"$cf_result\""
			;;
		esac
	done
	rm -f cf_makeflags.tmp
])

AC_SUBST(cf_cv_makeflags)
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_MAKE_TAGS version: 2 updated: 2000/10/04 09:18:40
dnl ------------
dnl Generate tags/TAGS targets for makefiles.  Do not generate TAGS if we have
dnl a monocase filesystem.
AC_DEFUN([CF_MAKE_TAGS],[
AC_REQUIRE([CF_MIXEDCASE_FILENAMES])
AC_CHECK_PROG(MAKE_LOWER_TAGS, ctags, yes, no)

if test "$cf_cv_mixedcase" = yes ; then
	AC_CHECK_PROG(MAKE_UPPER_TAGS, etags, yes, no)
else
	MAKE_UPPER_TAGS=no
fi

if test "$MAKE_UPPER_TAGS" = yes ; then
	MAKE_UPPER_TAGS=
else
	MAKE_UPPER_TAGS="#"
fi
AC_SUBST(MAKE_UPPER_TAGS)

if test "$MAKE_LOWER_TAGS" = yes ; then
	MAKE_LOWER_TAGS=
else
	MAKE_LOWER_TAGS="#"
fi
AC_SUBST(MAKE_LOWER_TAGS)
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_MIXEDCASE_FILENAMES version: 3 updated: 2003/09/20 17:07:55
dnl ----------------------
dnl Check if the file-system supports mixed-case filenames.  If we're able to
dnl create a lowercase name and see it as uppercase, it doesn't support that.
AC_DEFUN([CF_MIXEDCASE_FILENAMES],
[
AC_CACHE_CHECK(if filesystem supports mixed-case filenames,cf_cv_mixedcase,[
if test "$cross_compiling" = yes ; then
	case $target_alias in #(vi
	*-os2-emx*|*-msdosdjgpp*|*-cygwin*|*-mingw32*|*-uwin*) #(vi
		cf_cv_mixedcase=no
		;;
	*)
		cf_cv_mixedcase=yes
		;;
	esac
else
	rm -f conftest CONFTEST
	echo test >conftest
	if test -f CONFTEST ; then
		cf_cv_mixedcase=no
	else
		cf_cv_mixedcase=yes
	fi
	rm -f conftest CONFTEST
fi
])
test "$cf_cv_mixedcase" = yes && AC_DEFINE(MIXEDCASE_FILENAMES)
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_MSG_LOG version: 4 updated: 2007/07/29 09:55:12
dnl ----------
dnl Write a debug message to config.log, along with the line number in the
dnl configure script.
AC_DEFUN([CF_MSG_LOG],[
echo "${as_me-configure}:__oline__: testing $* ..." 1>&AC_FD_CC
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_NCURSES_CC_CHECK version: 4 updated: 2007/07/29 10:39:05
dnl -------------------
dnl Check if we can compile with ncurses' header file
dnl $1 is the cache variable to set
dnl $2 is the header-file to include
dnl $3 is the root name (ncurses or ncursesw)
AC_DEFUN([CF_NCURSES_CC_CHECK],[
	AC_TRY_COMPILE([
]ifelse($3,ncursesw,[
#define _XOPEN_SOURCE_EXTENDED
#undef  HAVE_LIBUTF8_H	/* in case we used CF_UTF8_LIB */
#define HAVE_LIBUTF8_H	/* to force ncurses' header file to use cchar_t */
])[
#include <$2>],[
#ifdef NCURSES_VERSION
]ifelse($3,ncursesw,[
#ifndef WACS_BSSB
	make an error
#endif
])[
printf("%s\n", NCURSES_VERSION);
#else
#ifdef __NCURSES_H
printf("old\n");
#else
	make an error
#endif
#endif
	]
	,[$1=$2]
	,[$1=no])
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_NCURSES_CPPFLAGS version: 19 updated: 2007/07/29 13:35:20
dnl -------------------
dnl Look for the SVr4 curses clone 'ncurses' in the standard places, adjusting
dnl the CPPFLAGS variable so we can include its header.
dnl
dnl The header files may be installed as either curses.h, or ncurses.h (would
dnl be obsolete, except that some packagers prefer this name to distinguish it
dnl from a "native" curses implementation).  If not installed for overwrite,
dnl the curses.h file would be in an ncurses subdirectory (e.g.,
dnl /usr/include/ncurses), but someone may have installed overwriting the
dnl vendor's curses.  Only very old versions (pre-1.9.2d, the first autoconf'd
dnl version) of ncurses don't define either __NCURSES_H or NCURSES_VERSION in
dnl the header.
dnl
dnl If the installer has set $CFLAGS or $CPPFLAGS so that the ncurses header
dnl is already in the include-path, don't even bother with this, since we cannot
dnl easily determine which file it is.  In this case, it has to be <curses.h>.
dnl
dnl The optional parameter gives the root name of the library, in case it is
dnl not installed as the default curses library.  That is how the
dnl wide-character version of ncurses is installed.
AC_DEFUN([CF_NCURSES_CPPFLAGS],
[AC_REQUIRE([CF_WITH_CURSES_DIR])

AC_PROVIDE([CF_CURSES_CPPFLAGS])dnl
cf_ncuhdr_root=ifelse($1,,ncurses,$1)

test -n "$cf_cv_curses_dir" && \
test "$cf_cv_curses_dir" != "no" && { \
  CF_ADD_INCDIR($cf_cv_curses_dir/include $cf_cv_curses_dir/include/$cf_ncuhdr_root)
}

AC_CACHE_CHECK(for $cf_ncuhdr_root header in include-path, cf_cv_ncurses_h,[
	cf_header_list="$cf_ncuhdr_root/curses.h $cf_ncuhdr_root/ncurses.h"
	( test "$cf_ncuhdr_root" = ncurses || test "$cf_ncuhdr_root" = ncursesw ) && cf_header_list="$cf_header_list curses.h ncurses.h"
	for cf_header in $cf_header_list
	do
		CF_NCURSES_CC_CHECK(cf_cv_ncurses_h,$cf_header,$1)
		test "$cf_cv_ncurses_h" != no && break
	done
])

CF_NCURSES_HEADER
CF_TERM_HEADER

# some applications need this, but should check for NCURSES_VERSION
AC_DEFINE(NCURSES)

CF_NCURSES_VERSION
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_NCURSES_HEADER version: 2 updated: 2008/03/23 14:48:54
dnl -----------------
dnl Find a "curses" header file, e.g,. "curses.h", or one of the more common
dnl variations of ncurses' installs.
dnl
dnl See also CF_CURSES_HEADER, which sets the same cache variable.
AC_DEFUN([CF_NCURSES_HEADER],[

if test "$cf_cv_ncurses_h" != no ; then
	cf_cv_ncurses_header=$cf_cv_ncurses_h
else

AC_CACHE_CHECK(for $cf_ncuhdr_root include-path, cf_cv_ncurses_h2,[
	test -n "$verbose" && echo
	CF_HEADER_PATH(cf_search,$cf_ncuhdr_root)
	test -n "$verbose" && echo search path $cf_search
	cf_save2_CPPFLAGS="$CPPFLAGS"
	for cf_incdir in $cf_search
	do
		CF_ADD_INCDIR($cf_incdir)
		for cf_header in \
			ncurses.h \
			curses.h
		do
			CF_NCURSES_CC_CHECK(cf_cv_ncurses_h2,$cf_header,$1)
			if test "$cf_cv_ncurses_h2" != no ; then
				cf_cv_ncurses_h2=$cf_incdir/$cf_header
				test -n "$verbose" && echo $ac_n "	... found $ac_c" 1>&AC_FD_MSG
				break
			fi
			test -n "$verbose" && echo "	... tested $cf_incdir/$cf_header" 1>&AC_FD_MSG
		done
		CPPFLAGS="$cf_save2_CPPFLAGS"
		test "$cf_cv_ncurses_h2" != no && break
	done
	test "$cf_cv_ncurses_h2" = no && AC_MSG_ERROR(not found)
	])

	CF_DIRNAME(cf_1st_incdir,$cf_cv_ncurses_h2)
	cf_cv_ncurses_header=`basename $cf_cv_ncurses_h2`
	if test `basename $cf_1st_incdir` = $cf_ncuhdr_root ; then
		cf_cv_ncurses_header=$cf_ncuhdr_root/$cf_cv_ncurses_header
	fi
	CF_ADD_INCDIR($cf_1st_incdir)

fi

# Set definitions to allow ifdef'ing for ncurses.h

case $cf_cv_ncurses_header in # (vi
*ncurses.h)
	AC_DEFINE(HAVE_NCURSES_H)
	;;
esac

case $cf_cv_ncurses_header in # (vi
ncurses/curses.h|ncurses/ncurses.h)
	AC_DEFINE(HAVE_NCURSES_NCURSES_H)
	;;
ncursesw/curses.h|ncursesw/ncurses.h)
	AC_DEFINE(HAVE_NCURSESW_NCURSES_H)
	;;
esac

])dnl
dnl ---------------------------------------------------------------------------
dnl CF_NCURSES_LIBS version: 13 updated: 2007/07/29 10:29:20
dnl ---------------
dnl Look for the ncurses library.  This is a little complicated on Linux,
dnl because it may be linked with the gpm (general purpose mouse) library.
dnl Some distributions have gpm linked with (bsd) curses, which makes it
dnl unusable with ncurses.  However, we don't want to link with gpm unless
dnl ncurses has a dependency, since gpm is normally set up as a shared library,
dnl and the linker will record a dependency.
dnl
dnl The optional parameter gives the root name of the library, in case it is
dnl not installed as the default curses library.  That is how the
dnl wide-character version of ncurses is installed.
AC_DEFUN([CF_NCURSES_LIBS],
[AC_REQUIRE([CF_NCURSES_CPPFLAGS])

cf_nculib_root=ifelse($1,,ncurses,$1)
	# This works, except for the special case where we find gpm, but
	# ncurses is in a nonstandard location via $LIBS, and we really want
	# to link gpm.
cf_ncurses_LIBS=""
cf_ncurses_SAVE="$LIBS"
AC_CHECK_LIB(gpm,Gpm_Open,
	[AC_CHECK_LIB(gpm,initscr,
		[LIBS="$cf_ncurses_SAVE"],
		[cf_ncurses_LIBS="-lgpm"])])

case $host_os in #(vi
freebsd*)
	# This is only necessary if you are linking against an obsolete
	# version of ncurses (but it should do no harm, since it's static).
	if test "$cf_nculib_root" = ncurses ; then
		AC_CHECK_LIB(mytinfo,tgoto,[cf_ncurses_LIBS="-lmytinfo $cf_ncurses_LIBS"])
	fi
	;;
esac

LIBS="$cf_ncurses_LIBS $LIBS"

if ( test -n "$cf_cv_curses_dir" && test "$cf_cv_curses_dir" != "no" )
then
	CF_ADD_LIBDIR($cf_cv_curses_dir/lib)
	LIBS="-l$cf_nculib_root $LIBS"
else
	CF_FIND_LIBRARY($cf_nculib_root,$cf_nculib_root,
		[#include <${cf_cv_ncurses_header-curses.h}>],
		[initscr()],
		initscr)
fi

if test -n "$cf_ncurses_LIBS" ; then
	AC_MSG_CHECKING(if we can link $cf_nculib_root without $cf_ncurses_LIBS)
	cf_ncurses_SAVE="$LIBS"
	for p in $cf_ncurses_LIBS ; do
		q=`echo $LIBS | sed -e "s%$p %%" -e "s%$p$%%"`
		if test "$q" != "$LIBS" ; then
			LIBS="$q"
		fi
	done
	AC_TRY_LINK([#include <${cf_cv_ncurses_header-curses.h}>],
		[initscr(); mousemask(0,0); tgoto((char *)0, 0, 0);],
		[AC_MSG_RESULT(yes)],
		[AC_MSG_RESULT(no)
		 LIBS="$cf_ncurses_SAVE"])
fi

CF_UPPER(cf_nculib_ROOT,HAVE_LIB$cf_nculib_root)
AC_DEFINE_UNQUOTED($cf_nculib_ROOT)
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_NCURSES_VERSION version: 12 updated: 2007/04/28 09:15:55
dnl ------------------
dnl Check for the version of ncurses, to aid in reporting bugs, etc.
dnl Call CF_CURSES_CPPFLAGS first, or CF_NCURSES_CPPFLAGS.  We don't use
dnl AC_REQUIRE since that does not work with the shell's if/then/else/fi.
AC_DEFUN([CF_NCURSES_VERSION],
[
AC_REQUIRE([CF_CURSES_CPPFLAGS])dnl
AC_CACHE_CHECK(for ncurses version, cf_cv_ncurses_version,[
	cf_cv_ncurses_version=no
	cf_tempfile=out$$
	rm -f $cf_tempfile
	AC_TRY_RUN([
#include <${cf_cv_ncurses_header-curses.h}>
#include <stdio.h>
int main()
{
	FILE *fp = fopen("$cf_tempfile", "w");
#ifdef NCURSES_VERSION
# ifdef NCURSES_VERSION_PATCH
	fprintf(fp, "%s.%d\n", NCURSES_VERSION, NCURSES_VERSION_PATCH);
# else
	fprintf(fp, "%s\n", NCURSES_VERSION);
# endif
#else
# ifdef __NCURSES_H
	fprintf(fp, "old\n");
# else
	make an error
# endif
#endif
	${cf_cv_main_return-return}(0);
}],[
	cf_cv_ncurses_version=`cat $cf_tempfile`],,[

	# This will not work if the preprocessor splits the line after the
	# Autoconf token.  The 'unproto' program does that.
	cat > conftest.$ac_ext <<EOF
#include <${cf_cv_ncurses_header-curses.h}>
#undef Autoconf
#ifdef NCURSES_VERSION
Autoconf NCURSES_VERSION
#else
#ifdef __NCURSES_H
Autoconf "old"
#endif
;
#endif
EOF
	cf_try="$ac_cpp conftest.$ac_ext 2>&AC_FD_CC | grep '^Autoconf ' >conftest.out"
	AC_TRY_EVAL(cf_try)
	if test -f conftest.out ; then
		cf_out=`cat conftest.out | sed -e 's%^Autoconf %%' -e 's%^[[^"]]*"%%' -e 's%".*%%'`
		test -n "$cf_out" && cf_cv_ncurses_version="$cf_out"
		rm -f conftest.out
	fi
])
	rm -f $cf_tempfile
])
test "$cf_cv_ncurses_version" = no || AC_DEFINE(NCURSES)
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_NO_LEAKS_OPTION version: 4 updated: 2006/12/16 14:24:05
dnl ------------------
dnl see CF_WITH_NO_LEAKS
AC_DEFUN([CF_NO_LEAKS_OPTION],[
AC_MSG_CHECKING(if you want to use $1 for testing)
AC_ARG_WITH($1,
	[$2],
	[AC_DEFINE($3)ifelse([$4],,[
	 $4
])
	: ${with_cflags:=-g}
	: ${with_no_leaks:=yes}
	 with_$1=yes],
	[with_$1=])
AC_MSG_RESULT(${with_$1:-no})

case .$with_cflags in #(vi
.*-g*)
	case .$CFLAGS in #(vi
	.*-g*) #(vi
		;;
	*)
		CF_ADD_CFLAGS([-g])
		;;
	esac
	;;
esac
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_PATH_SYNTAX version: 12 updated: 2008/03/23 14:45:59
dnl --------------
dnl Check the argument to see that it looks like a pathname.  Rewrite it if it
dnl begins with one of the prefix/exec_prefix variables, and then again if the
dnl result begins with 'NONE'.  This is necessary to work around autoconf's
dnl delayed evaluation of those symbols.
AC_DEFUN([CF_PATH_SYNTAX],[
if test "x$prefix" != xNONE; then
  cf_path_syntax="$prefix"
else
  cf_path_syntax="$ac_default_prefix"
fi

case ".[$]$1" in #(vi
.\[$]\(*\)*|.\'*\'*) #(vi
  ;;
..|./*|.\\*) #(vi
  ;;
.[[a-zA-Z]]:[[\\/]]*) #(vi OS/2 EMX
  ;;
.\[$]{*prefix}*) #(vi
  eval $1="[$]$1"
  case ".[$]$1" in #(vi
  .NONE/*)
    $1=`echo [$]$1 | sed -e s%NONE%$cf_path_syntax%`
    ;;
  esac
  ;; #(vi
.no|.NONE/*)
  $1=`echo [$]$1 | sed -e s%NONE%$cf_path_syntax%`
  ;;
*)
  ifelse($2,,[AC_MSG_ERROR([expected a pathname, not \"[$]$1\"])],$2)
  ;;
esac
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_PDCURSES_X11 version: 8 updated: 2008/03/23 14:48:54
dnl ---------------
dnl Configure for PDCurses' X11 library
AC_DEFUN([CF_PDCURSES_X11],[
AC_REQUIRE([CF_X_ATHENA])

AC_PATH_PROGS(XCURSES_CONFIG,xcurses-config,none)

if test "$XCURSES_CONFIG" != none ; then

CPPFLAGS="`$XCURSES_CONFIG --cflags` $CPPFLAGS"
LIBS="`$XCURSES_CONFIG --libs` $LIBS"

cf_cv_lib_XCurses=yes

else

LDFLAGS="$LDFLAGS $X_LIBS"
CF_CHECK_CFLAGS($X_CFLAGS)
AC_CHECK_LIB(X11,XOpenDisplay,
	[LIBS="-lX11 $LIBS"],,
	[$X_PRE_LIBS $LIBS $X_EXTRA_LIBS])
AC_CACHE_CHECK(for XCurses library,cf_cv_lib_XCurses,[
LIBS="-lXCurses $LIBS"
AC_TRY_LINK([
#include <xcurses.h>
char *XCursesProgramName = "test";
],[XCursesExit();],
[cf_cv_lib_XCurses=yes],
[cf_cv_lib_XCurses=no])
])

fi

if test $cf_cv_lib_XCurses = yes ; then
	AC_DEFINE(UNIX)
	AC_DEFINE(XCURSES)
	AC_DEFINE(HAVE_XCURSES)
else
	AC_MSG_ERROR(Cannot link with XCurses)
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_POSIX_C_SOURCE version: 6 updated: 2005/07/14 20:25:10
dnl -----------------
dnl Define _POSIX_C_SOURCE to the given level, and _POSIX_SOURCE if needed.
dnl
dnl	POSIX.1-1990				_POSIX_SOURCE
dnl	POSIX.1-1990 and			_POSIX_SOURCE and
dnl		POSIX.2-1992 C-Language			_POSIX_C_SOURCE=2
dnl		Bindings Option
dnl	POSIX.1b-1993				_POSIX_C_SOURCE=199309L
dnl	POSIX.1c-1996				_POSIX_C_SOURCE=199506L
dnl	X/Open 2000				_POSIX_C_SOURCE=200112L
dnl
dnl Parameters:
dnl	$1 is the nominal value for _POSIX_C_SOURCE
AC_DEFUN([CF_POSIX_C_SOURCE],
[
cf_POSIX_C_SOURCE=ifelse($1,,199506L,$1)

cf_save_CFLAGS="$CFLAGS"
cf_save_CPPFLAGS="$CPPFLAGS"

CF_REMOVE_DEFINE(cf_trim_CFLAGS,$cf_save_CFLAGS,_POSIX_C_SOURCE)
CF_REMOVE_DEFINE(cf_trim_CPPFLAGS,$cf_save_CPPFLAGS,_POSIX_C_SOURCE)

AC_CACHE_CHECK(if we should define _POSIX_C_SOURCE,cf_cv_posix_c_source,[
	CF_MSG_LOG(if the symbol is already defined go no further)
	AC_TRY_COMPILE([#include <sys/types.h>],[
#ifndef _POSIX_C_SOURCE
make an error
#endif],
	[cf_cv_posix_c_source=no],
	[cf_want_posix_source=no
	 case .$cf_POSIX_C_SOURCE in #(vi
	 .[[12]]??*) #(vi
		cf_cv_posix_c_source="-D_POSIX_C_SOURCE=$cf_POSIX_C_SOURCE"
		;;
	 .2) #(vi
		cf_cv_posix_c_source="-D_POSIX_C_SOURCE=$cf_POSIX_C_SOURCE"
		cf_want_posix_source=yes
		;;
	 .*)
		cf_want_posix_source=yes
		;;
	 esac
	 if test "$cf_want_posix_source" = yes ; then
		AC_TRY_COMPILE([#include <sys/types.h>],[
#ifdef _POSIX_SOURCE
make an error
#endif],[],
		cf_cv_posix_c_source="$cf_cv_posix_c_source -D_POSIX_SOURCE")
	 fi
	 CF_MSG_LOG(ifdef from value $cf_POSIX_C_SOURCE)
	 CFLAGS="$cf_trim_CFLAGS"
	 CPPFLAGS="$cf_trim_CPPFLAGS $cf_cv_posix_c_source"
	 CF_MSG_LOG(if the second compile does not leave our definition intact error)
	 AC_TRY_COMPILE([#include <sys/types.h>],[
#ifndef _POSIX_C_SOURCE
make an error
#endif],,
	 [cf_cv_posix_c_source=no])
	 CFLAGS="$cf_save_CFLAGS"
	 CPPFLAGS="$cf_save_CPPFLAGS"
	])
])

if test "$cf_cv_posix_c_source" != no ; then
	CFLAGS="$cf_trim_CFLAGS"
	CPPFLAGS="$cf_trim_CPPFLAGS"
	if test "$cf_cv_cc_u_d_options" = yes ; then
		cf_temp_posix_c_source=`echo "$cf_cv_posix_c_source" | \
				sed -e 's/-D/-U/g' -e 's/=[[^ 	]]*//g'`
		CPPFLAGS="$CPPFLAGS $cf_temp_posix_c_source"
	fi
	CPPFLAGS="$CPPFLAGS $cf_cv_posix_c_source"
fi

])dnl
dnl ---------------------------------------------------------------------------
dnl CF_PROG_CC_U_D version: 1 updated: 2005/07/14 16:59:30
dnl --------------
dnl Check if C (preprocessor) -U and -D options are processed in the order
dnl given rather than by type of option.  Some compilers insist on apply all
dnl of the -U options after all of the -D options.  Others allow mixing them,
dnl and may predefine symbols that conflict with those we define.
AC_DEFUN([CF_PROG_CC_U_D],
[
AC_CACHE_CHECK(if $CC -U and -D options work together,cf_cv_cc_u_d_options,[
	cf_save_CPPFLAGS="$CPPFLAGS"
	CPPFLAGS="-UU_D_OPTIONS -DU_D_OPTIONS -DD_U_OPTIONS -UD_U_OPTIONS"
	AC_TRY_COMPILE([],[
#ifndef U_D_OPTIONS
make an undefined-error
#endif
#ifdef  D_U_OPTIONS
make a defined-error
#endif
	],[
	cf_cv_cc_u_d_options=yes],[
	cf_cv_cc_u_d_options=no])
	CPPFLAGS="$cf_save_CPPFLAGS"
])
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_PROG_EXT version: 10 updated: 2004/01/03 19:28:18
dnl -----------
dnl Compute $PROG_EXT, used for non-Unix ports, such as OS/2 EMX.
AC_DEFUN([CF_PROG_EXT],
[
AC_REQUIRE([CF_CHECK_CACHE])
case $cf_cv_system_name in
os2*)
    CFLAGS="$CFLAGS -Zmt"
    CPPFLAGS="$CPPFLAGS -D__ST_MT_ERRNO__"
    CXXFLAGS="$CXXFLAGS -Zmt"
    # autoconf's macro sets -Zexe and suffix both, which conflict:w
    LDFLAGS="$LDFLAGS -Zmt -Zcrtdll"
    ac_cv_exeext=.exe
    ;;
esac

AC_EXEEXT
AC_OBJEXT

PROG_EXT="$EXEEXT"
AC_SUBST(PROG_EXT)
test -n "$PROG_EXT" && AC_DEFINE_UNQUOTED(PROG_EXT,"$PROG_EXT")
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_REMOVE_DEFINE version: 2 updated: 2005/07/09 16:12:18
dnl ----------------
dnl Remove all -U and -D options that refer to the given symbol from a list
dnl of C compiler options.  This works around the problem that not all
dnl compilers process -U and -D options from left-to-right, so a -U option
dnl cannot be used to cancel the effect of a preceding -D option.
dnl
dnl $1 = target (which could be the same as the source variable)
dnl $2 = source (including '$')
dnl $3 = symbol to remove
define([CF_REMOVE_DEFINE],
[
# remove $3 symbol from $2
$1=`echo "$2" | \
	sed	-e 's/-[[UD]]$3\(=[[^ 	]]*\)\?[[ 	]]/ /g' \
		-e 's/-[[UD]]$3\(=[[^ 	]]*\)\?[$]//g'`
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_SUBDIR_PATH version: 5 updated: 2007/07/29 09:55:12
dnl --------------
dnl Construct a search-list for a nonstandard header/lib-file
dnl	$1 = the variable to return as result
dnl	$2 = the package name
dnl	$3 = the subdirectory, e.g., bin, include or lib
AC_DEFUN([CF_SUBDIR_PATH],
[$1=""

CF_ADD_SUBDIR_PATH($1,$2,$3,/usr,$prefix)
CF_ADD_SUBDIR_PATH($1,$2,$3,$prefix,NONE)
CF_ADD_SUBDIR_PATH($1,$2,$3,/usr/local,$prefix)
CF_ADD_SUBDIR_PATH($1,$2,$3,/opt,$prefix)
CF_ADD_SUBDIR_PATH($1,$2,$3,[$]HOME,$prefix)
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_SUBST version: 4 updated: 2006/06/17 12:33:03
dnl --------
dnl	Shorthand macro for substituting things that the user may override
dnl	with an environment variable.
dnl
dnl	$1 = long/descriptive name
dnl	$2 = environment variable
dnl	$3 = default value
AC_DEFUN([CF_SUBST],
[AC_CACHE_VAL(cf_cv_subst_$2,[
AC_MSG_CHECKING(for $1 (symbol $2))
CF_SUBST_IF([-z "[$]$2"], [$2], [$3])
cf_cv_subst_$2=[$]$2
AC_MSG_RESULT([$]$2)
])
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_SYSTYPE version: 3 updated: 2001/02/03 00:14:59
dnl ----------
dnl Derive the system-type (our main clue to the method of building shared
dnl libraries).
AC_DEFUN([CF_SYSTYPE],
[
AC_MSG_CHECKING(for system type)
AC_CACHE_VAL(cf_cv_systype,[
AC_ARG_WITH(system-type,
[  --with-system-type=XXX  test: override derived host system-type],
[cf_cv_systype=$withval],
[
cf_cv_systype="`(uname -s || hostname || echo unknown) 2>/dev/null |sed -e s'/[[:\/.-]]/_/'g  | sed 1q`"
if test -z "$cf_cv_systype"; then cf_cv_systype=unknown;fi
])])
AC_MSG_RESULT($cf_cv_systype)
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_SYS_NAME version: 3 updated: 2008/03/23 14:48:54
dnl -----------
dnl Derive the system name, as a check for reusing the autoconf cache
AC_DEFUN([CF_SYS_NAME],
[
SYS_NAME=`(uname -s -r || uname -a || hostname) 2>/dev/null | sed 1q`
test -z "$SYS_NAME" && SYS_NAME=unknown
AC_DEFINE_UNQUOTED(SYS_NAME,"$SYS_NAME")

AC_CACHE_VAL(cf_cv_system_name,[cf_cv_system_name="$SYS_NAME"])

if test ".$SYS_NAME" != ".$cf_cv_system_name" ; then
	AC_MSG_RESULT("Cached system name does not agree with actual")
	AC_MSG_ERROR("Please remove config.cache and try again.")
fi])
dnl ---------------------------------------------------------------------------
dnl CF_TERM_HEADER version: 1 updated: 2005/12/31 13:26:39
dnl --------------
dnl Look for term.h, which is part of X/Open curses.  It defines the interface
dnl to terminfo database.  Usually it is in the same include-path as curses.h,
dnl but some packagers change this, breaking various applications.
AC_DEFUN([CF_TERM_HEADER],[
AC_CACHE_CHECK(for terminfo header, cf_cv_term_header,[
case ${cf_cv_ncurses_header} in #(vi
*/ncurses.h|*/ncursesw.h) #(vi
	cf_term_header=`echo "$cf_cv_ncurses_header" | sed -e 's%ncurses[[^.]]*\.h$%term.h%'`
	;;
*)
	cf_term_header=term.h
	;;
esac

for cf_test in $cf_term_header "ncurses/term.h" "ncursesw/term.h"
do
AC_TRY_COMPILE([#include <stdio.h>
#include <${cf_cv_ncurses_header-curses.h}>
#include <$cf_test>
],[int x = auto_left_margin],[
	cf_cv_term_header="$cf_test"],[
	cf_cv_term_header=unknown
	])
	test "$cf_cv_term_header" != unknown && break
done
])

# Set definitions to allow ifdef'ing to accommodate subdirectories

case $cf_cv_term_header in # (vi
*term.h)
	AC_DEFINE(HAVE_TERM_H)
	;;
esac

case $cf_cv_term_header in # (vi
ncurses/term.h) #(vi
	AC_DEFINE(HAVE_NCURSES_TERM_H)
	;;
ncursesw/term.h)
	AC_DEFINE(HAVE_NCURSESW_TERM_H)
	;;
esac
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_UPPER version: 5 updated: 2001/01/29 23:40:59
dnl --------
dnl Make an uppercase version of a variable
dnl $1=uppercase($2)
AC_DEFUN([CF_UPPER],
[
$1=`echo "$2" | sed y%abcdefghijklmnopqrstuvwxyz./-%ABCDEFGHIJKLMNOPQRSTUVWXYZ___%`
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_UTF8_LIB version: 5 updated: 2008/10/17 19:37:52
dnl -----------
dnl Check for multibyte support, and if not found, utf8 compatibility library
AC_DEFUN([CF_UTF8_LIB],
[
AC_CACHE_CHECK(for multibyte character support,cf_cv_utf8_lib,[
	cf_save_LIBS="$LIBS"
	AC_TRY_LINK([
#include <stdlib.h>],[putwc(0,0);],
	[cf_cv_utf8_lib=yes],
	[CF_FIND_LINKAGE([
#include <libutf8.h>],[putwc(0,0);],utf8,
		[cf_cv_utf8_lib=add-on],
		[cf_cv_utf8_lib=no])
])])

# HAVE_LIBUTF8_H is used by ncurses if curses.h is shared between
# ncurses/ncursesw:
if test "$cf_cv_utf8_lib" = "add-on" ; then
	AC_DEFINE(HAVE_LIBUTF8_H)
	CF_ADD_INCDIR($cf_cv_header_path_utf8)
	CF_ADD_LIBDIR($cf_cv_library_path_utf8)
	LIBS="-lutf8 $LIBS"
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_VERBOSE version: 3 updated: 2007/07/29 09:55:12
dnl ----------
dnl Use AC_VERBOSE w/o the warnings
AC_DEFUN([CF_VERBOSE],
[test -n "$verbose" && echo "	$1" 1>&AC_FD_MSG
CF_MSG_LOG([$1])
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_VERSION_INFO version: 3 updated: 2003/11/22 12:22:45
dnl ---------------
dnl Define several useful symbols derived from the VERSION file.  A separate
dnl file is preferred to embedding the version numbers in various scripts.
dnl (automake is a textbook-example of why the latter is a bad idea, but there
dnl are others).
dnl
dnl The file contents are:
dnl	libtool-version	release-version	patch-version
dnl or
dnl	release-version
dnl where
dnl	libtool-version (see ?) consists of 3 integers separated by '.'
dnl	release-version consists of a major version and minor version
dnl		separated by '.', optionally followed by a patch-version
dnl		separated by '-'.  The minor version need not be an
dnl		integer (but it is preferred).
dnl	patch-version is an integer in the form yyyymmdd, so ifdef's and
dnl		scripts can easily compare versions.
dnl
dnl If libtool is used, the first form is required, since CF_WITH_LIBTOOL
dnl simply extracts the first field using 'cut -f1'.
AC_DEFUN([CF_VERSION_INFO],
[
if test -f $srcdir/VERSION ; then
	AC_MSG_CHECKING(for package version)

	# if there are not enough fields, cut returns the last one...
	cf_field1=`sed -e '2,$d' $srcdir/VERSION|cut -f1`
	cf_field2=`sed -e '2,$d' $srcdir/VERSION|cut -f2`
	cf_field3=`sed -e '2,$d' $srcdir/VERSION|cut -f3`

	# this is how CF_BUNDLED_INTL uses $VERSION:
	VERSION="$cf_field1"

	VERSION_MAJOR=`echo "$cf_field2" | sed -e 's/\..*//'`
	test -z "$VERSION_MAJOR" && AC_MSG_ERROR(missing major-version)

	VERSION_MINOR=`echo "$cf_field2" | sed -e 's/^[[^.]]*\.//' -e 's/-.*//'`
	test -z "$VERSION_MINOR" && AC_MSG_ERROR(missing minor-version)

	AC_MSG_RESULT(${VERSION_MAJOR}.${VERSION_MINOR})

	AC_MSG_CHECKING(for package patch date)
	VERSION_PATCH=`echo "$cf_field3" | sed -e 's/^[[^-]]*-//'`
	case .$VERSION_PATCH in
	.)
		AC_MSG_ERROR(missing patch-date $VERSION_PATCH)
		;;
	.[[0-9]][[0-9]][[0-9]][[0-9]][[0-9]][[0-9]][[0-9]][[0-9]])
		;;
	*)
		AC_MSG_ERROR(illegal patch-date $VERSION_PATCH)
		;;
	esac
	AC_MSG_RESULT($VERSION_PATCH)
else
	AC_MSG_ERROR(did not find $srcdir/VERSION)
fi

# show the actual data that we have for versions:
CF_VERBOSE(VERSION $VERSION)
CF_VERBOSE(VERSION_MAJOR $VERSION_MAJOR)
CF_VERBOSE(VERSION_MINOR $VERSION_MINOR)
CF_VERBOSE(VERSION_PATCH $VERSION_PATCH)

AC_SUBST(VERSION)
AC_SUBST(VERSION_MAJOR)
AC_SUBST(VERSION_MINOR)
AC_SUBST(VERSION_PATCH)

dnl if a package name is given, define its corresponding version info.  We
dnl need the package name to ensure that the defined symbols are unique.
ifelse($1,,,[
	PACKAGE=$1
	AC_DEFINE_UNQUOTED(PACKAGE, "$PACKAGE")
	AC_SUBST(PACKAGE)
	CF_UPPER(cf_PACKAGE,$PACKAGE)
	AC_DEFINE_UNQUOTED(${cf_PACKAGE}_VERSION,"${VERSION_MAJOR}.${VERSION_MINOR}")
	AC_DEFINE_UNQUOTED(${cf_PACKAGE}_PATCHDATE,${VERSION_PATCH})
])
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_WITH_CURSES_DIR version: 2 updated: 2002/11/10 14:46:59
dnl ------------------
dnl Wrapper for AC_ARG_WITH to specify directory under which to look for curses
dnl libraries.
AC_DEFUN([CF_WITH_CURSES_DIR],[
AC_ARG_WITH(curses-dir,
	[  --with-curses-dir=DIR   directory in which (n)curses is installed],
	[CF_PATH_SYNTAX(withval)
	 cf_cv_curses_dir=$withval],
	[cf_cv_curses_dir=no])
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_WITH_DBMALLOC version: 6 updated: 2006/12/16 14:24:05
dnl ----------------
dnl Configure-option for dbmalloc.  The optional parameter is used to override
dnl the updating of $LIBS, e.g., to avoid conflict with subsequent tests.
AC_DEFUN([CF_WITH_DBMALLOC],[
CF_NO_LEAKS_OPTION(dbmalloc,
	[  --with-dbmalloc         test: use Conor Cahill's dbmalloc library],
	[USE_DBMALLOC])

if test "$with_dbmalloc" = yes ; then
	AC_CHECK_HEADER(dbmalloc.h,
		[AC_CHECK_LIB(dbmalloc,[debug_malloc]ifelse($1,,[],[,$1]))])
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_WITH_DMALLOC version: 6 updated: 2006/12/16 14:24:05
dnl ---------------
dnl Configure-option for dmalloc.  The optional parameter is used to override
dnl the updating of $LIBS, e.g., to avoid conflict with subsequent tests.
AC_DEFUN([CF_WITH_DMALLOC],[
CF_NO_LEAKS_OPTION(dmalloc,
	[  --with-dmalloc          test: use Gray Watson's dmalloc library],
	[USE_DMALLOC])

if test "$with_dmalloc" = yes ; then
	AC_CHECK_HEADER(dmalloc.h,
		[AC_CHECK_LIB(dmalloc,[dmalloc_debug]ifelse($1,,[],[,$1]))])
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_WITH_LIBTOOL version: 19 updated: 2008/03/29 15:46:43
dnl ---------------
dnl Provide a configure option to incorporate libtool.  Define several useful
dnl symbols for the makefile rules.
dnl
dnl The reference to AC_PROG_LIBTOOL does not normally work, since it uses
dnl macros from libtool.m4 which is in the aclocal directory of automake.
dnl Following is a simple script which turns on the AC_PROG_LIBTOOL macro.
dnl But that still does not work properly since the macro is expanded outside
dnl the CF_WITH_LIBTOOL macro:
dnl
dnl	#!/bin/sh
dnl	ACLOCAL=`aclocal --print-ac-dir`
dnl	if test -z "$ACLOCAL" ; then
dnl		echo cannot find aclocal directory
dnl		exit 1
dnl	elif test ! -f $ACLOCAL/libtool.m4 ; then
dnl		echo cannot find libtool.m4 file
dnl		exit 1
dnl	fi
dnl	
dnl	LOCAL=aclocal.m4
dnl	ORIG=aclocal.m4.orig
dnl	
dnl	trap "mv $ORIG $LOCAL" 0 1 2 5 15
dnl	rm -f $ORIG
dnl	mv $LOCAL $ORIG
dnl	
dnl	# sed the LIBTOOL= assignment to omit the current directory?
dnl	sed -e 's/^LIBTOOL=.*/LIBTOOL=${LIBTOOL-libtool}/' $ACLOCAL/libtool.m4 >>$LOCAL
dnl	cat $ORIG >>$LOCAL
dnl	
dnl	autoconf-257 $*
dnl
AC_DEFUN([CF_WITH_LIBTOOL],
[
ifdef([AC_PROG_LIBTOOL],,[
LIBTOOL=
])
# common library maintenance symbols that are convenient for libtool scripts:
LIB_CREATE='${AR} -cr'
LIB_OBJECT='${OBJECTS}'
LIB_SUFFIX=.a
LIB_PREP="$RANLIB"

# symbols used to prop libtool up to enable it to determine what it should be
# doing:
LIB_CLEAN=
LIB_COMPILE=
LIB_LINK='${CC}'
LIB_INSTALL=
LIB_UNINSTALL=

AC_MSG_CHECKING(if you want to build libraries with libtool)
AC_ARG_WITH(libtool,
	[  --with-libtool          generate libraries with libtool],
	[with_libtool=$withval],
	[with_libtool=no])
AC_MSG_RESULT($with_libtool)
if test "$with_libtool" != "no"; then
ifdef([AC_PROG_LIBTOOL],[
	# missing_content_AC_PROG_LIBTOOL{{
	AC_PROG_LIBTOOL
	# missing_content_AC_PROG_LIBTOOL}}
],[
 	if test "$with_libtool" != "yes" ; then
		CF_PATH_SYNTAX(with_libtool)
		LIBTOOL=$with_libtool
	else
 		AC_PATH_PROG(LIBTOOL,libtool)
 	fi
 	if test -z "$LIBTOOL" ; then
 		AC_MSG_ERROR(Cannot find libtool)
 	fi
])dnl
	LIB_CREATE='${LIBTOOL} --mode=link ${CC} -rpath ${DESTDIR}${libdir} -version-info `cut -f1 ${srcdir}/VERSION` ${LIBTOOL_OPTS} -o'
	LIB_OBJECT='${OBJECTS:.o=.lo}'
	LIB_SUFFIX=.la
	LIB_CLEAN='${LIBTOOL} --mode=clean'
	LIB_COMPILE='${LIBTOOL} --mode=compile'
	LIB_LINK='${LIBTOOL} --mode=link ${CC} ${LIBTOOL_OPTS}'
	LIB_INSTALL='${LIBTOOL} --mode=install'
	LIB_UNINSTALL='${LIBTOOL} --mode=uninstall'
	LIB_PREP=:

	# Show the version of libtool
	AC_MSG_CHECKING(version of libtool)

	# Save the version in a cache variable - this is not entirely a good
	# thing, but the version string from libtool is very ugly, and for
	# bug reports it might be useful to have the original string.
	cf_cv_libtool_version=`$LIBTOOL --version 2>&1 | sed -e '/^$/d' |sed -e '2,$d' -e 's/([[^)]]*)//g' -e 's/^[[^1-9]]*//' -e 's/[[^0-9.]].*//'`
	AC_MSG_RESULT($cf_cv_libtool_version)
	if test -z "$cf_cv_libtool_version" ; then
		AC_MSG_ERROR(This is not GNU libtool)
	fi

	# special hack to add --tag option for C++ compiler
	case $cf_cv_libtool_version in
	1.[[5-9]]*|[[2-9]]*)
		LIBTOOL_CXX="$LIBTOOL --tag=CXX"
		LIBTOOL="$LIBTOOL --tag=CC"
		;;
	*)
		LIBTOOL_CXX="$LIBTOOL"
		;;
	esac
else
	LIBTOOL=""
	LIBTOOL_CXX=""
fi

test -z "$LIBTOOL" && ECHO_LT=

AC_SUBST(LIBTOOL)
AC_SUBST(LIBTOOL_CXX)
AC_SUBST(LIBTOOL_OPTS)

AC_SUBST(LIB_CREATE)
AC_SUBST(LIB_OBJECT)
AC_SUBST(LIB_SUFFIX)
AC_SUBST(LIB_PREP)

AC_SUBST(LIB_CLEAN)
AC_SUBST(LIB_COMPILE)
AC_SUBST(LIB_LINK)
AC_SUBST(LIB_INSTALL)
AC_SUBST(LIB_UNINSTALL)

])dnl
dnl ---------------------------------------------------------------------------
dnl CF_WITH_NC_ALLOC_H version: 4 updated: 2008/11/01 12:44:02
dnl ------------------
dnl Applications that use ncurses can provide better leak-checking if they
dnl call ncurses' functions to free its memory on exit.  That is normally not
dnl configured (--disable-leaks), but is useful in a debugging library.
dnl
dnl Use this after checking for ncurses/ncursesw libraries.
AC_DEFUN([CF_WITH_NC_ALLOC_H],
[
AC_REQUIRE([CF_DISABLE_LEAKS])

case $LIBS in #(vi
*ncurses*)

if test "$with_no_leaks" = yes ; then
	AC_CHECK_HEADER(nc_alloc.h)
	AC_CHECK_FUNCS(_nc_free_and_exit)
fi
;;
esac
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_WITH_PURIFY version: 2 updated: 2006/12/14 18:43:43
dnl --------------
AC_DEFUN([CF_WITH_PURIFY],[
CF_NO_LEAKS_OPTION(purify,
	[  --with-purify           test: use Purify],
	[USE_PURIFY],
	[LINK_PREFIX="$LINK_PREFIX purify"])
AC_SUBST(LINK_PREFIX)
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_WITH_VALGRIND version: 1 updated: 2006/12/14 18:00:21
dnl ----------------
AC_DEFUN([CF_WITH_VALGRIND],[
CF_NO_LEAKS_OPTION(valgrind,
	[  --with-valgrind         test: use valgrind],
	[USE_VALGRIND])
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_WITH_WARNINGS version: 5 updated: 2004/07/23 14:40:34
dnl ----------------
dnl Combine the checks for gcc features into a configure-script option
dnl
dnl Parameters:
dnl	$1 - see CF_GCC_WARNINGS
AC_DEFUN([CF_WITH_WARNINGS],
[
if ( test "$GCC" = yes || test "$GXX" = yes )
then
AC_MSG_CHECKING(if you want to check for gcc warnings)
AC_ARG_WITH(warnings,
	[  --with-warnings         test: turn on gcc warnings],
	[cf_opt_with_warnings=$withval],
	[cf_opt_with_warnings=no])
AC_MSG_RESULT($cf_opt_with_warnings)
if test "$cf_opt_with_warnings" != no ; then
	CF_GCC_ATTRIBUTES
	CF_GCC_WARNINGS([$1])
fi
fi
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_XOPEN_CURSES version: 8 updated: 2003/11/07 19:47:46
dnl ---------------
dnl Test if we should define X/Open source for curses, needed on Digital Unix
dnl 4.x, to see the extended functions, but breaks on IRIX 6.x.
dnl
dnl The getbegyx() check is needed for HPUX, which omits legacy macros such
dnl as getbegy().  The latter is better design, but the former is standard.
AC_DEFUN([CF_XOPEN_CURSES],
[
AC_REQUIRE([CF_CURSES_CPPFLAGS])dnl
AC_CACHE_CHECK(if we must define _XOPEN_SOURCE_EXTENDED,cf_cv_need_xopen_extension,[
AC_TRY_LINK([
#include <stdlib.h>
#include <${cf_cv_ncurses_header-curses.h}>],[
	long x = winnstr(stdscr, "", 0);
	int x1, y1;
	getbegyx(stdscr, y1, x1)],
	[cf_cv_need_xopen_extension=no],
	[AC_TRY_LINK([
#define _XOPEN_SOURCE_EXTENDED
#include <stdlib.h>
#include <${cf_cv_ncurses_header-curses.h}>],[
	long x = winnstr(stdscr, "", 0);
	int x1, y1;
	getbegyx(stdscr, y1, x1)],
	[cf_cv_need_xopen_extension=yes],
	[cf_cv_need_xopen_extension=unknown])])])
test $cf_cv_need_xopen_extension = yes && CPPFLAGS="$CPPFLAGS -D_XOPEN_SOURCE_EXTENDED"
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_XOPEN_SOURCE version: 26 updated: 2008/07/27 11:26:57
dnl ---------------
dnl Try to get _XOPEN_SOURCE defined properly that we can use POSIX functions,
dnl or adapt to the vendor's definitions to get equivalent functionality,
dnl without losing the common non-POSIX features.
dnl
dnl Parameters:
dnl	$1 is the nominal value for _XOPEN_SOURCE
dnl	$2 is the nominal value for _POSIX_C_SOURCE
AC_DEFUN([CF_XOPEN_SOURCE],[

AC_REQUIRE([CF_PROG_CC_U_D])

cf_XOPEN_SOURCE=ifelse($1,,500,$1)
cf_POSIX_C_SOURCE=ifelse($2,,199506L,$2)

case $host_os in #(vi
aix[[45]]*) #(vi
	CPPFLAGS="$CPPFLAGS -D_ALL_SOURCE"
	;;
freebsd*|dragonfly*) #(vi
	# 5.x headers associate
	#	_XOPEN_SOURCE=600 with _POSIX_C_SOURCE=200112L
	#	_XOPEN_SOURCE=500 with _POSIX_C_SOURCE=199506L
	cf_POSIX_C_SOURCE=200112L
	cf_XOPEN_SOURCE=600
	CPPFLAGS="$CPPFLAGS -D_BSD_TYPES -D__BSD_VISIBLE -D_POSIX_C_SOURCE=$cf_POSIX_C_SOURCE -D_XOPEN_SOURCE=$cf_XOPEN_SOURCE"
	;;
hpux*) #(vi
	CPPFLAGS="$CPPFLAGS -D_HPUX_SOURCE"
	;;
irix[[56]].*) #(vi
	CPPFLAGS="$CPPFLAGS -D_SGI_SOURCE"
	;;
linux*|gnu*|k*bsd*-gnu) #(vi
	CF_GNU_SOURCE
	;;
mirbsd*) #(vi
	# setting _XOPEN_SOURCE or _POSIX_SOURCE breaks <arpa/inet.h>
	;;
netbsd*) #(vi
	# setting _XOPEN_SOURCE breaks IPv6 for lynx on NetBSD 1.6, breaks xterm, is not needed for ncursesw
	;;
openbsd*) #(vi
	# setting _XOPEN_SOURCE breaks xterm on OpenBSD 2.8, is not needed for ncursesw
	;;
osf[[45]]*) #(vi
	CPPFLAGS="$CPPFLAGS -D_OSF_SOURCE"
	;;
nto-qnx*) #(vi
	CPPFLAGS="$CPPFLAGS -D_QNX_SOURCE"
	;;
sco*) #(vi
	# setting _XOPEN_SOURCE breaks Lynx on SCO Unix / OpenServer
	;;
solaris*) #(vi
	CPPFLAGS="$CPPFLAGS -D__EXTENSIONS__"
	;;
*)
	AC_CACHE_CHECK(if we should define _XOPEN_SOURCE,cf_cv_xopen_source,[
	AC_TRY_COMPILE([#include <sys/types.h>],[
#ifndef _XOPEN_SOURCE
make an error
#endif],
	[cf_cv_xopen_source=no],
	[cf_save="$CPPFLAGS"
	 CPPFLAGS="$CPPFLAGS -D_XOPEN_SOURCE=$cf_XOPEN_SOURCE"
	 AC_TRY_COMPILE([#include <sys/types.h>],[
#ifdef _XOPEN_SOURCE
make an error
#endif],
	[cf_cv_xopen_source=no],
	[cf_cv_xopen_source=$cf_XOPEN_SOURCE])
	CPPFLAGS="$cf_save"
	])
])
	if test "$cf_cv_xopen_source" != no ; then
		CF_REMOVE_DEFINE(CFLAGS,$CFLAGS,_XOPEN_SOURCE)
		CF_REMOVE_DEFINE(CPPFLAGS,$CPPFLAGS,_XOPEN_SOURCE)
		test "$cf_cv_cc_u_d_options" = yes && \
			CPPFLAGS="$CPPFLAGS -U_XOPEN_SOURCE"
		CPPFLAGS="$CPPFLAGS -D_XOPEN_SOURCE=$cf_cv_xopen_source"
	fi
	CF_POSIX_C_SOURCE($cf_POSIX_C_SOURCE)
	;;
esac
])
dnl ---------------------------------------------------------------------------
dnl CF_X_ATHENA version: 12 updated: 2004/06/15 21:14:41
dnl -----------
dnl Check for Xaw (Athena) libraries
dnl
dnl Sets $cf_x_athena according to the flavor of Xaw which is used.
AC_DEFUN([CF_X_ATHENA],
[AC_REQUIRE([CF_X_TOOLKIT])
cf_x_athena=${cf_x_athena-Xaw}

AC_MSG_CHECKING(if you want to link with Xaw 3d library)
withval=
AC_ARG_WITH(Xaw3d,
	[  --with-Xaw3d            link with Xaw 3d library])
if test "$withval" = yes ; then
	cf_x_athena=Xaw3d
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(if you want to link with neXT Athena library)
withval=
AC_ARG_WITH(neXtaw,
	[  --with-neXtaw           link with neXT Athena library])
if test "$withval" = yes ; then
	cf_x_athena=neXtaw
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_MSG_CHECKING(if you want to link with Athena-Plus library)
withval=
AC_ARG_WITH(XawPlus,
	[  --with-XawPlus          link with Athena-Plus library])
if test "$withval" = yes ; then
	cf_x_athena=XawPlus
	AC_MSG_RESULT(yes)
else
	AC_MSG_RESULT(no)
fi

AC_CHECK_LIB(Xext,XextCreateExtension,
	[LIBS="-lXext $LIBS"])

cf_x_athena_lib=""

CF_X_ATHENA_CPPFLAGS($cf_x_athena)
CF_X_ATHENA_LIBS($cf_x_athena)
])dnl
dnl ---------------------------------------------------------------------------
dnl CF_X_ATHENA_CPPFLAGS version: 2 updated: 2002/10/09 20:00:37
dnl --------------------
dnl Normally invoked by CF_X_ATHENA, with $1 set to the appropriate flavor of
dnl the Athena widgets, e.g., Xaw, Xaw3d, neXtaw.
AC_DEFUN([CF_X_ATHENA_CPPFLAGS],
[
cf_x_athena_root=ifelse($1,,Xaw,$1)
cf_x_athena_include=""

for cf_path in default \
	/usr/contrib/X11R6 \
	/usr/contrib/X11R5 \
	/usr/lib/X11R5 \
	/usr/local
do
	if test -z "$cf_x_athena_include" ; then
		cf_save="$CPPFLAGS"
		cf_test=X11/$cf_x_athena_root/SimpleMenu.h
		if test $cf_path != default ; then
			CPPFLAGS="-I$cf_path/include $cf_save"
			AC_MSG_CHECKING(for $cf_test in $cf_path)
		else
			AC_MSG_CHECKING(for $cf_test)
		fi
		AC_TRY_COMPILE([
#include <X11/Intrinsic.h>
#include <$cf_test>],[],
			[cf_result=yes],
			[cf_result=no])
		AC_MSG_RESULT($cf_result)
		if test "$cf_result" = yes ; then
			cf_x_athena_include=$cf_path
			break
		else
			CPPFLAGS="$cf_save"
		fi
	fi
done

if test -z "$cf_x_athena_include" ; then
	AC_MSG_WARN(
[Unable to successfully find Athena header files with test program])
elif test "$cf_x_athena_include" != default ; then
	CPPFLAGS="$CPPFLAGS -I$cf_x_athena_include"
fi
])
dnl ---------------------------------------------------------------------------
dnl CF_X_ATHENA_LIBS version: 7 updated: 2008/03/23 14:46:03
dnl ----------------
dnl Normally invoked by CF_X_ATHENA, with $1 set to the appropriate flavor of
dnl the Athena widgets, e.g., Xaw, Xaw3d, neXtaw.
AC_DEFUN([CF_X_ATHENA_LIBS],
[AC_REQUIRE([CF_X_TOOLKIT])
cf_x_athena_root=ifelse($1,,Xaw,$1)
cf_x_athena_lib=""

for cf_path in default \
	/usr/contrib/X11R6 \
	/usr/contrib/X11R5 \
	/usr/lib/X11R5 \
	/usr/local
do
	for cf_lib in \
		"-l$cf_x_athena_root -lXmu" \
		"-l$cf_x_athena_root -lXpm -lXmu" \
		"-l${cf_x_athena_root}_s -lXmu_s"
	do
		if test -z "$cf_x_athena_lib" ; then
			cf_save="$LIBS"
			cf_test=XawSimpleMenuAddGlobalActions
			if test $cf_path != default ; then
				LIBS="-L$cf_path/lib $cf_lib $LIBS"
				AC_MSG_CHECKING(for $cf_lib in $cf_path)
			else
				LIBS="$cf_lib $LIBS"
				AC_MSG_CHECKING(for $cf_test in $cf_lib)
			fi
			AC_TRY_LINK([],[$cf_test()],
				[cf_result=yes],
				[cf_result=no])
			AC_MSG_RESULT($cf_result)
			if test "$cf_result" = yes ; then
				cf_x_athena_lib="$cf_lib"
				break
			fi
			LIBS="$cf_save"
		fi
	done
done

if test -z "$cf_x_athena_lib" ; then
	AC_MSG_ERROR(
[Unable to successfully link Athena library (-l$cf_x_athena_root) with test program])
fi

CF_UPPER(cf_x_athena_LIBS,HAVE_LIB_$cf_x_athena)
AC_DEFINE_UNQUOTED($cf_x_athena_LIBS)
])
dnl ---------------------------------------------------------------------------
dnl CF_X_TOOLKIT version: 12 updated: 2008/03/23 15:04:54
dnl ------------
dnl Check for X Toolkit libraries
dnl
AC_DEFUN([CF_X_TOOLKIT],
[
AC_REQUIRE([AC_PATH_XTRA])
AC_REQUIRE([CF_CHECK_CACHE])

# SYSTEM_NAME=`echo "$cf_cv_system_name"|tr ' ' -`

cf_have_X_LIBS=no

LDFLAGS="$X_LIBS $LDFLAGS"
CF_CHECK_CFLAGS($X_CFLAGS)

AC_CHECK_FUNC(XOpenDisplay,,[
AC_CHECK_LIB(X11,XOpenDisplay,
	[LIBS="-lX11 $LIBS"],,
	[$X_PRE_LIBS $LIBS $X_EXTRA_LIBS])])

AC_CHECK_FUNC(XtAppInitialize,,[
AC_CHECK_LIB(Xt, XtAppInitialize,
	[AC_DEFINE(HAVE_LIBXT)
	 cf_have_X_LIBS=Xt
	 LIBS="-lXt $X_PRE_LIBS $LIBS $X_EXTRA_LIBS"],,
	[$X_PRE_LIBS $LIBS $X_EXTRA_LIBS])])

if test $cf_have_X_LIBS = no ; then
	AC_MSG_WARN(
[Unable to successfully link X Toolkit library (-lXt) with
test program.  You will have to check and add the proper libraries by hand
to makefile.])
fi
])dnl
