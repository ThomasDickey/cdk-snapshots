#ifndef CDKSLIDER_H
#define CDKSLIDER_H	1

#include "cdk.h"

/*
 * Description of the widget:
 *
 */

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

/*
 * Define the CDK entry widget structure.
 */
struct SSlider {
   CDKSCREEN	*screen;
   WINDOW	*parent;
   WINDOW	*win;
   WINDOW	*shadowWin;
   WINDOW	*fieldWin;
   WINDOW	*labelWin;
   chtype	*title[MAX_LINES];
   int		titlePos[MAX_LINES];
   int		titleLen[MAX_LINES];
   int		titleAdj;
   int		titleLines;
   chtype	*label;
   int		labelLen;
   int		fieldWidth;
   int		barFieldWidth;
   chtype	filler;
   int 		low;
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
   chtype	ULChar;
   chtype	URChar;
   chtype	LLChar;
   chtype	LRChar;
   chtype	VChar;
   chtype	HChar;
   chtype	BoxAttrib;
   EExitType	exitType;
   boolean	box;
   boolean	shadow;
   int		screenIndex;
   BINDFN	bindFunction[MAX_BINDINGS];
   void *	bindData[MAX_BINDINGS];
   PROCESSFN	preProcessFunction;
   void *	preProcessData;
   PROCESSFN	postProcessFunction;
   void *	postProcessData;
};
typedef struct SSlider CDKSLIDER;

/*
 * This creates a new pointer to a CDK slider widget.
 */
CDKSLIDER *newCDKSlider (CDKSCREEN *cdkscreen,
				int xpos, int ypos,
				char *title, char *label,
				chtype fieldAttr, int fieldWidth,
				int start, int low, int high,
				int inc, int fastInc,
				boolean Box, boolean shadow);

/*
 * This activates the widget.
 */
int activateCDKSlider (CDKSLIDER *slider, chtype *actions);

/*
 * This injects a single character into the widget.
 */
int injectCDKSlider (CDKSLIDER *slider, chtype input);

/*
 * This sets various attributes of the widget.
 */
void setCDKSlider (CDKSLIDER *slider,
			int low, int high, int value, 
			boolean Box);

/*
 * This sets the low/high/current value of the slider widget.
 */
void setCDKSliderLowHigh (CDKSLIDER *slider, int low, int high);
void setCDKSliderValue (CDKSLIDER *slider, int value);
int getCDKSliderLowValue (CDKSLIDER *slider);
int getCDKSliderHighValue (CDKSLIDER *slider);
int getCDKSliderValue (CDKSLIDER *slider);

/*
 * This sets the box attribute of the widget.
 */
void setCDKSliderBox (CDKSLIDER *slider, boolean Box);
boolean getCDKSliderBox (CDKSLIDER *slider);

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKSliderULChar (CDKSLIDER *slider, chtype character);
void setCDKSliderURChar (CDKSLIDER *slider, chtype character);
void setCDKSliderLLChar (CDKSLIDER *slider, chtype character);
void setCDKSliderLRChar (CDKSLIDER *slider, chtype character);
void setCDKSliderVerticalChar (CDKSLIDER *slider, chtype character);
void setCDKSliderHorizontalChar (CDKSLIDER *slider, chtype character);
void setCDKSliderBoxAttribute (CDKSLIDER *slider, chtype character);

/*
 * This sets the background color of the widget.
 */ 
void setCDKSliderBackgroundColor (CDKSLIDER *slider, char *color);

/*
 * This draws the slider widget on the screen.
 */
void drawCDKSlider (CDKSLIDER *slider, boolean Box);

/*
 * This erases the slider widget from the screen.
 */
void eraseCDKSlider (CDKSLIDER *slider);

/*
 * This moves the widge to the location dictated by the given location.
 */
void moveCDKSlider (CDKSLIDER *slider, int xpos, int ypos, boolean relative, boolean refresh);

/*
 * This allows the user to interactively position the widget on the screen.
 */
void positionCDKSlider (CDKSLIDER *slider);

/*
 * This destroys the slider widget and associated memory.
 */
void destroyCDKSlider (CDKSLIDER *slider);

/*
 * These functions set the pre/post process functions.
 */
void setCDKSliderPreProcess (CDKSLIDER *slider, PROCESSFN callback, void *data);
void setCDKSliderPostProcess (CDKSLIDER *slider, PROCESSFN callback, void *data);
#endif
