#ifndef CDK_H
#define CDK_H

/*
 * Copyright (c) 1990 The Regents of the University of California.
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
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef	CDK_PERL_EXT
#undef	instr
#endif

#include <curses.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

/*
 * This enumerated typedef lists all the CDK widget types.
 */
typedef enum {vNULL,
		vALPHALIST, vBUTTONBOX, vCALENDAR, vDIALOG,
		vENTRY, vFSELECT, vGRAPH, vHISTOGRAM,
		vITEMLIST, vLABEL, vMARQUEE, vMATRIX,
		vMENTRY, vMENU, vRADIO, vSCALE, vSCROLL,
		vSELECTION, vSLIDER, vSWINDOW, vTEMPLATE,
		vTAB, vVIEWER
		} EObjectType;

/*
 * This enumerated typedef lists all the valid display types for
 * the entry, mentry, and template widgets.
 */
typedef enum {vINVALID,
		vCHAR, vHCHAR, vINT, vHINT, vMIXED, vHMIXED,
		vUCHAR, vLCHAR, vUHCHAR, vLHCHAR, vUMIXED,
		vLMIXED, vUHMIXED, vLHMIXED, vVIEWONLY
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
 * recognise.
 */
typedef enum {vEARLY_EXIT, vESCAPE_HIT, vNORMAL, vNEVER_ACTIVATED} EExitType;

/*
 * This defines a boolean type.
 */
typedef int boolean;

/*
 * Declare miscellaneos defines.
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

#define	BOX		1
#define	NOBOX		0

#define MAX_BINDINGS	300
#define MAX_ITEMS	2000
#define MAX_LINES	5000
#define MAX_BUTTONS	200

#define WIN_WIDTH(a)	(a->_maxx)
#define WIN_HEIGHT(a)	(a->_maxy)
#define WIN_XPOS(a)	(a->_begx)
#define WIN_YPOS(a)	(a->_begy)
#define	DIFFERENCE(a,b)	(abs(a-b))
#define	MAXIMUM(a,b)	(a > b ? a : b)
#define	MINIMUM(a,b)	(a < b ? a : b)
#define	HALF(a)		(a>>1)

#ifndef COLOR_PAIR
#define	COLOR_PAIR(a)	A_NORMAL
#endif

/* Define the 'GLOBAL DEBUG FILEHANDLE'	*/
extern  FILE	*CDKDEBUG;

/*
 * =========================================================
 * 	Delcare Debugging Routines.
 * =========================================================
 */
#define START_DEBUG(a)		(CDKDEBUG=startCDKDebug(a))
#define WRITE_DEBUGMESG(a,b)	(writeCDKDebugMessage (CDKDEBUG,__FILE__,a,__LINE__,b))
#define	END_DEBUG		(stopCDKDebug(CDKDEBUG)
FILE *startCDKDebug(char *filename);
void writeCDKDebugMessage (FILE *fd, char *filename, char *function, int line, char *message);
void stopCDKDebug (FILE *fd);

/*
 * These header files define misc values and prototypes.
 */
#include "cdkscreen.h"
#include "curdefs.h"
#include "binding.h"
#include "cdk_util.h"

/*
 * Include the CDK widget header files.
 */
#include "alphalist.h"
#include "buttonbox.h"
#include "calendar.h"
#include "dialog.h"
#include "entry.h"
#include "fselect.h"
#include "graph.h"
#include "histogram.h"
#include "itemlist.h"
#include "label.h"
#include "marquee.h"
#include "matrix.h"
#include "mentry.h"
#include "menu.h"
#include "radio.h"
#include "scale.h"
#include "scroll.h"
#include "selection.h"
#include "slider.h"
#include "swindow.h"
#include "template.h"
#include "viewer.h"
#include "draw.h"

#endif
