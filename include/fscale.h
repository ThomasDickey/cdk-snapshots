/*
 * $Id: fscale.h,v 1.2 2002/02/25 23:38:00 tom Exp $
 */

#ifndef CDKFSCALE_H
#define CDKFSCALE_H	1

#ifdef __cplusplus
extern "C" {
#endif

#include <cdk.h>

/*
 * Copyright 2001, Lutz Mueller
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
 *      This product includes software developed by Lutz Mueller
 *      and contributors.
 * 4. Neither the name of Lutz Mueller, nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY LUTZ MUELLER AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL LUTZ MUELLER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Define the CDK widget scale structure.
 */
struct SFScale {
   CDKOBJS	obj;
   WINDOW *	parent;
   WINDOW *	win;
   WINDOW *	shadowWin;
   WINDOW *	fieldWin;
   WINDOW *	labelWin;
   chtype *	title[MAX_LINES];
   int		titlePos[MAX_LINES];
   int		titleLen[MAX_LINES];
   int		titleAdj;
   int		titleLines;
   chtype *	label;
   int		labelLen;
   int		labelPos;
   int		boxHeight;
   int		boxWidth;
   int		fieldWidth;
   chtype	fieldAttr;
   float	low;
   float	high;
   float	current;
   float	inc;
   float	fastinc;
   int		digits;
   EExitType	exitType;
   boolean	shadow;
   chtype	ULChar;
   chtype	URChar;
   chtype	LLChar;
   chtype	LRChar;
   chtype	VChar;
   chtype	HChar;
   chtype	BoxAttrib;
   PROCESSFN	preProcessFunction;
   void *	preProcessData;
   PROCESSFN	postProcessFunction;
   void *	postProcessData;
};
typedef struct SFScale CDKFSCALE;

/*
 * This creates a new pointer to a scale widget.
 */
CDKFSCALE *newCDKFScale (
		CDKSCREEN *	/* cdkscreen */,
		int		/* xpos */,
		int		/* ypos */,
		char *		/* title */,
		char *		/* label */,
		chtype		/* fieldAttr */,
		int		/* fieldWidth */,
		float		/* start */,
		float		/* low */,
		float		/* high */,
		float		/* inc */,
		float		/* fastinc */,
		int		/* digits */,
		boolean		/* Box */,
		boolean		/* shadow */);

/*
 * This activates the widget.
 */
float activateCDKFScale (
		CDKFSCALE *	/* scale */,
		chtype *	/* actions */);

/*
 * This injects a single character into the widget.
 */
float injectCDKFScale (
		CDKFSCALE *	/* scale */,
		chtype		/* input */);

/*
 * This sets various attributes of the widget.
 */
void setCDKFScale (
		CDKFSCALE *	/* scale */,
		float		/* low */,
		float		/* high */,
		float		/* value */,
		boolean		/* Box */);

/*
 * This sets the low and high values.
 */
void setCDKFScaleLowHigh (
		CDKFSCALE *	/* scale */,
		float		/* low */,
		float		/* high */);

float getCDKFScaleLowValue (
		CDKFSCALE *	/* scale */);

float getCDKFScaleHighValue (
		CDKFSCALE *	/* scale */);

/*
 * This sets the digits.
 */
void setCDKFScaleDigits (
		CDKFSCALE *     /* scale */,
		int		/* digits */);

int getCDKFScaleDigits (
		CDKFSCALE *     /* scale */);

/*
 * This returns the current value of the widget.
 */
void setCDKFScaleValue (
		CDKFSCALE *	/* scale */,
		float		/* value */);

float getCDKFScaleValue (
		CDKFSCALE *	/* scale */);

/*
 * This sets the box attribute of the widget.
 */
void setCDKFScaleBox (
		CDKFSCALE *	/* scale */,
		boolean		/* Box */);

boolean getCDKFScaleBox (
		CDKFSCALE *	/* scale */);

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKFScaleULChar (
		CDKFSCALE *	/* scale */,
		chtype		/* character */);

void setCDKFScaleURChar (
		CDKFSCALE *	/* scale */,
		chtype		/* character */);

void setCDKFScaleLLChar (
		CDKFSCALE *	/* scale */,
		chtype		/* character */);

void setCDKFScaleLRChar (
		CDKFSCALE *	/* scale */,
		chtype		/* character */);

void setCDKFScaleVerticalChar (
		CDKFSCALE *	/* scale */,
		chtype		/* character */);

void setCDKFScaleHorizontalChar (
		CDKFSCALE *	/* scale */,
		chtype		/* character */);

void setCDKFScaleBoxAttribute (
		CDKFSCALE *	/* scale */,
		chtype		/* character */);

/*
 * This sets the background color of the widget.
 */
void setCDKFScaleBackgroundColor (
		CDKFSCALE *	/* scale */,
		char *		/* color */);

/*
 * This draws the scale widget on the screen.
 */
#define drawCDKFScale(obj,Box) drawCDKObject(obj,Box)

/*
 * This erases the widget on the screen.
 */
#define eraseCDKFScale(obj) eraseCDKObject(obj)

/*
 * This moves the widget to the given location on the screen.
 */
#define moveCDKFScale(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This interactively positions the widget on the screen.
 */
#define positionCDKFScale(widget) positionCDKObject(ObjOf(widget),widget->win)

/*
 * This destroys the widget and all associated memory.
 */
void destroyCDKFScale (
		CDKFSCALE *	/* scale */);

/*
 * These set the pre/post process callback functions.
 */
void setCDKFScalePreProcess (
		CDKFSCALE *	/* scale */,
		PROCESSFN	/* callback */,
		void *		/* data */);

void setCDKFScalePostProcess (
		CDKFSCALE *	/* scale */,
		PROCESSFN	/* callback */,
		void *		/* data */);

#ifdef __cplusplus
}
#endif

#endif /* CDKFSCALE_H */
