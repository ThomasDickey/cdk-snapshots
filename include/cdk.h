/*
 * $Id: cdk.h,v 1.34 2004/08/29 21:49:32 tom Exp $
 */

#ifndef CDK_H
#define CDK_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Changes 2000-2003,2004 copyright Thomas E. Dickey
 *
 * Copyright 1999, Mike Glover
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgment:
 * 	This product includes software developed by Mike Glover
 * 	and contributors.
 * 4. Neither the name of Mike Glover, nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY MIKE GLOVER AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL MIKE GLOVER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <cdk_config.h>
#include <cdk_version.h>

#ifdef	CDK_PERL_EXT
#undef	instr
#endif

#ifdef HAVE_XCURSES
#include <xcurses.h>
#ifndef mvwhline
#define mvwhline(win,y,x,c,n)     	(wmove(win,y,x) == ERR ? ERR : whline(win,c,n))
#endif
#ifndef mvwvline
#define mvwvline(win,y,x,c,n)     	(wmove(win,y,x) == ERR ? ERR : wvline(win,c,n))
#endif
#elif defined(HAVE_NCURSESW_NCURSES_H)
#include <ncursesw/ncurses.h>
#elif defined(HAVE_NCURSES_NCURSES_H)
#include <ncurses/ncurses.h>
#elif defined(HAVE_NCURSES_H)
#include <ncurses.h>
#else
#include <curses.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

#ifdef HAVE_GETOPT_HEADER
#ifdef HAVE_GETOPT_H
#include <getopt.h>
#endif
#else
extern int optind;
extern char * optarg;
#endif

/*
 * Definitions that we do not want if term.h does
 */
#ifdef buttons
#undef buttons
#endif

#ifdef lines
#undef lines
#endif

#ifdef newline
#undef newline
#endif

/*
 * Values we normally get from limits.h (assume 32-bits)
 */
#ifndef INT_MIN
#define INT_MIN (-INT_MAX - 1)
#endif

#ifndef INT_MAX
#define INT_MAX 2147483647
#endif

#ifndef GCC_UNUSED
#define GCC_UNUSED /*nothing*/
#endif

#ifdef HAVE_LIBDMALLOC
#include <dmalloc.h>    /* Gray Watson's library */
#else
#undef  HAVE_LIBDMALLOC
#define HAVE_LIBDMALLOC 0
#endif

#ifdef HAVE_LIBDBMALLOC
#include <dbmalloc.h>   /* Conor Cahill's library */
#else
#undef  HAVE_LIBDBMALLOC
#define HAVE_LIBDBMALLOC 0
#endif

/*
 * This enumerated typedef lists all of the CDK widget types.
 */
typedef enum {	vNULL = 0
		,vALPHALIST
		,vBUTTON
		,vBUTTONBOX
		,vCALENDAR
		,vDIALOG
		,vDSCALE
		,vENTRY
		,vFSCALE
		,vFSELECT
		,vFSLIDER
		,vGRAPH
		,vHISTOGRAM
		,vITEMLIST
		,vLABEL
		,vMARQUEE
		,vMATRIX
		,vMENTRY
		,vMENU
		,vRADIO
		,vSCALE
		,vSCROLL
		,vSELECTION
		,vSLIDER
		,vSWINDOW
		,vTEMPLATE
		,vTRAVERSE
		,vUSCALE
		,vUSLIDER
		,vVIEWER
		} EObjectType;

/*
 * This enumerated typedef lists all the valid display types for
 * the entry, mentry, and template widgets.
 */
typedef enum {	vINVALID = 0
		,vCHAR
		,vHCHAR
		,vINT
		,vHINT
		,vMIXED
		,vHMIXED
		,vUCHAR
		,vLCHAR
		,vUHCHAR
		,vLHCHAR
		,vUMIXED
		,vLMIXED
		,vUHMIXED
		,vLHMIXED
		,vVIEWONLY
		} EDisplayType;

/*
 * This enumerated typedef lists all the display types for
 * the histogram widget.
 */
typedef enum {vNONE, vPERCENT, vFRACTION, vREAL} EHistogramDisplayType;

/*
 * This enumerated typedef defines the display types for the graph.
 */
typedef enum {vPLOT, vLINE} EGraphDisplayType;

/*
 * This enumerated typedef defines where white space is to be
 * stripped from in the function stripWhiteSpace.
 */
typedef enum {vFRONT, vBACK, vBOTH} EStripType;

/*
 * This enumerated typedef defines the type of exits the widgets
 * recognize.
 */
typedef enum {vEARLY_EXIT, vESCAPE_HIT, vNORMAL, vNEVER_ACTIVATED} EExitType;

/*
 * This defines a boolean type.
 */
typedef int boolean;

/*
 * Declare miscellaneous defines.
 */
#define	LEFT		9000
#define	RIGHT		9001
#define	CENTER		9002
#define	TOP		9003
#define	BOTTOM		9004
#define	HORIZONTAL	9005
#define	VERTICAL	9006
#define	FULL		9007

#define NONE		0
#define ROW		1
#define COL		2

#define MAX_BINDINGS	300	/* unused by widgets */
#define MAX_ITEMS	2000	/* unused by widgets */
#define MAX_BUTTONS	200	/* unused by widgets */

#define	MAXIMUM(a,b)	((a) > (b) ? (a) : (b))
#define	MINIMUM(a,b)	((a) < (b) ? (a) : (b))
#define	HALF(a)		((a) >> 1)

#ifndef COLOR_PAIR
#define	COLOR_PAIR(a)	A_NORMAL
#endif

#define CONTROL(c)	((c) & 0x1f)	/* obsolete: use CTRL() */

/* Define the 'GLOBAL DEBUG FILEHANDLE'	*/
extern  FILE	*CDKDEBUG;

/*
 * =========================================================
 * 	Declare Debugging Routines.
 * =========================================================
 */
#define START_DEBUG(a)		(CDKDEBUG=startCDKDebug(a))
#define WRITE_DEBUGMESG(a,b)	(writeCDKDebugMessage (CDKDEBUG,__FILE__,a,__LINE__,b))
#define	END_DEBUG		(stopCDKDebug(CDKDEBUG)
FILE *startCDKDebug(char *filename);
void writeCDKDebugMessage (FILE *fd, char *filename, char *function, int line, char *message);
void stopCDKDebug (FILE *fd);

/*
 * These header files define miscellaneous values and prototypes.
 */
#include <cdkscreen.h>
#include <curdefs.h>
#include <binding.h>
#include <cdk_util.h>
#include <cdk_objs.h>
#include <cdk_params.h>

/*
 * Include the CDK widget header files.
 */
#include <alphalist.h>
#include <buttonbox.h>
#include <calendar.h>
#include <dialog.h>
#include <entry.h>
#include <fselect.h>
#include <graph.h>
#include <histogram.h>
#include <itemlist.h>
#include <label.h>
#include <marquee.h>
#include <matrix.h>
#include <mentry.h>
#include <menu.h>
#include <radio.h>
#include <scroll.h>
#include <selection.h>
#include <swindow.h>
#include <template.h>
#include <viewer.h>
#include <traverse.h>
#include <button.h>

/*
 * Generated headers:
 */
#include <dscale.h>
#include <fscale.h>
#include <scale.h>
#include <uscale.h>

#include <fslider.h>
#include <slider.h>
#include <uslider.h>

/*
 * Low-level object drawing
 */
#include <draw.h>

#ifdef __cplusplus
}
#endif

#endif /* CDK_H */
