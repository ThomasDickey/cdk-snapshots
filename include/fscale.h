/*
 * $Id: fscale.h,v 1.8 2003/11/27 14:55:27 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKFSCALE_H
#define CDKFSCALE_H	1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CDK_H
#define CDKINCLUDES
#include <cdk.h>
#undef CDKINCLUDES
#include <binding.h>
#include <cdkscreen.h>
#include <cdk_objs.h>
#endif

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
   int		titleAdj;
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
#define injectCDKFScale(obj,input) injectCDKObject(obj,input,Float)

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
 * These set the drawing characters of the widget.
 */
#define setCDKFScaleULChar(w,c)            setULCharOf(w,c)
#define setCDKFScaleURChar(w,c)            setURCharOf(w,c)
#define setCDKFScaleLLChar(w,c)            setLLCharOf(w,c)
#define setCDKFScaleLRChar(w,c)            setLRCharOf(w,c)
#define setCDKFScaleVerticalChar(w,c)      setVTCharOf(w,c)
#define setCDKFScaleHorizontalChar(w,c)    setHZCharOf(w,c)
#define setCDKFScaleBoxAttribute(w,c)      setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
void setCDKFScaleBackgroundColor (
		CDKFSCALE *	/* scale */,
		char *		/* color */);

/*
 * This sets the background attribute of the widget.
 */ 
void setCDKFScaleBackgroundAttrib (
		CDKFSCALE *	/* fscale */,
		chtype		/* attribute */);

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
#define destroyCDKFScale(obj) destroyCDKObject(obj)

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
#endif /* CDKINCLUDES */
