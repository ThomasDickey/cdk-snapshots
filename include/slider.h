/*
 * $Id: slider.h,v 1.17 2003/11/15 18:11:09 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDKSLIDER_H
#define CDKSLIDER_H	1

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
 *      This product includes software developed by Mike Glover
 *      and contributors.
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

/*
 * Define the CDK entry widget structure.
 */
struct SSlider {
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
   int		fieldWidth;
   int		barFieldWidth;
   chtype	filler;
   int		low;
   int		high;
   int		inc;
   int		fastinc;
   int		current;
   int		orient;
   float	step;
   int		xpos;
   int		ypos;
   int		height;
   int		width;
   int		boxHeight;
   int		boxWidth;
   EExitType	exitType;
   boolean	shadow;
   PROCESSFN	preProcessFunction;
   void *	preProcessData;
   PROCESSFN	postProcessFunction;
   void *	postProcessData;
};
typedef struct SSlider CDKSLIDER;

/*
 * This creates a new pointer to a CDK slider widget.
 */
CDKSLIDER *newCDKSlider (
		CDKSCREEN *	/* cdkscreen */,
		int		/* xpos */,
		int		/* ypos */,
		char *		/* title */,
		char *		/* label */,
		chtype		/* fieldAttr */,
		int		/* fieldWidth */,
		int		/* start */,
		int		/* low */,
		int		/* high */,
		int		/* inc */,
		int		/* fastInc */,
		boolean		/* Box */,
		boolean		/* shadow */);

/*
 * This activates the widget.
 */
int activateCDKSlider (
		CDKSLIDER *	/* slider */,
		chtype *	/* actions */);

/*
 * This injects a single character into the widget.
 */
#define injectCDKSlider(obj,input) injectCDKObject(obj,input,Int)

/*
 * This sets various attributes of the widget.
 */
void setCDKSlider (
		CDKSLIDER *	/* slider */,
		int		/* low */,
		int		/* high */,
		int		/* value */,
		boolean		/* Box */);

/*
 * This sets the low/high/current value of the slider widget.
 */
void setCDKSliderLowHigh (
		CDKSLIDER *	/* slider */,
		int		/* low */,
		int		/* high */);

void setCDKSliderValue (
		CDKSLIDER *	/* slider */,
		int		/* value */);

int getCDKSliderLowValue (
		CDKSLIDER *	/* slider */);

int getCDKSliderHighValue (
		CDKSLIDER *	/* slider */);

int getCDKSliderValue (
		CDKSLIDER *	/* slider */);

/*
 * This sets the box attribute of the widget.
 */
void setCDKSliderBox (
		CDKSLIDER *	/* slider */,
		boolean		/* Box */);

boolean getCDKSliderBox (
		CDKSLIDER *	/* slider */);

/*
 * These set the drawing characters of the widget.
 */
#define setCDKSliderULChar(w,c)            setULCharOf(w,c)
#define setCDKSliderURChar(w,c)            setURCharOf(w,c)
#define setCDKSliderLLChar(w,c)            setLLCharOf(w,c)
#define setCDKSliderLRChar(w,c)            setLRCharOf(w,c)
#define setCDKSliderVerticalChar(w,c)      setVTCharOf(w,c)
#define setCDKSliderHorizontalChar(w,c)    setHZCharOf(w,c)
#define setCDKSliderBoxAttribute(w,c)      setBXAttrOf(w,c)

/*
 * This sets the background color of the widget.
 */
void setCDKSliderBackgroundColor (
		CDKSLIDER *	/* slider */,
		char *		/* color */);

/*
 * This sets the background attribute of the widget.
 */ 
void setCDKSliderBackgroundAttrib (
		CDKSLIDER *	/* slider */,
		chtype		/* attribute */);

/*
 * This draws the slider widget on the screen.
 */
#define drawCDKSlider(obj,Box) drawCDKObject(obj,Box)

/*
 * This erases the slider widget from the screen.
 */
#define eraseCDKSlider(obj) eraseCDKObject(obj)

/*
 * This moves the widget to the location dictated by the given location.
 */
#define moveCDKSlider(obj,xpos,ypos,relative,refresh) moveCDKObject(obj,xpos,ypos,relative,refresh)

/*
 * This allows the user to interactively position the widget on the screen.
 */
#define positionCDKSlider(widget) positionCDKObject(ObjOf(widget),widget->win)

/*
 * This destroys the slider widget and associated memory.
 */
#define destroyCDKSlider(obj) destroyCDKObject(obj)

/*
 * These functions set the pre/post process functions.
 */
void setCDKSliderPreProcess (
		CDKSLIDER *	/* slider */,
		PROCESSFN	/* callback */,
		void *		/* data */);

void setCDKSliderPostProcess (
		CDKSLIDER *	/* slider */,
		PROCESSFN	/* callback */,
		void *		/* data */);

#ifdef __cplusplus
}
#endif

#endif /* CDKSLIDER_H */
#endif /* CDKINCLUDES */
