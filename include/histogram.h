#ifndef CDKHISTOGRAM_H
#define CDKHISTOGRAM_H	1

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
 * Delcare the histogram structure.
 */
struct SHistogram {
   CDKSCREEN			*screen;
   WINDOW			*parent;
   WINDOW			*win;
   WINDOW			*shadowWin;
   chtype			*title[MAX_LINES];
   int				titlePos[MAX_LINES];
   int				titleLen[MAX_LINES];
   int				titleAdj;
   int				titleLines;
   char				*curString;
   char				*lowString;
   char				*highString;
   chtype			filler;
   float			percent;
   int				fieldHeight;
   int				fieldWidth;
   int				barSize;
   int 				orient;
   int				statsPos;
   chtype			statsAttr;
   EHistogramDisplayType	viewType;
   int				high;
   int				low;
   int				value;
   int				lowx;
   int				lowy;
   int				curx;
   int				cury;
   int				highx;
   int				highy;
   int				boxWidth;
   int				boxHeight;
   chtype			ULChar;
   chtype			URChar;
   chtype			LLChar;
   chtype			LRChar;
   chtype			VChar;
   chtype			HChar;
   chtype			BoxAttrib;
   boolean			box;
   boolean			shadow;
   int				screenIndex;
};
typedef struct SHistogram CDKHISTOGRAM;

/*
 * This returns a new pointer to a histogram pointer.
 */
CDKHISTOGRAM *newCDKHistogram (CDKSCREEN *cdkscreen,
				int xpos, int ypos,
				int height, int width,
				int orient, char *title,
				boolean Box, boolean shadow);

/*
 * This was added to make the builder simpler. All this will
 * do is call drawCDKHistogram.
 */
void activateCDKHistogram (CDKHISTOGRAM *histogram, chtype *actions);

/*
 * These set specific attributes of the histogram.
 */
void setCDKHistogram (CDKHISTOGRAM *histogram,
			EHistogramDisplayType viewtype,
			int statsPos, chtype statsAttr,
			int low, int high, int value,
			chtype filler, boolean Box);

/*
 * This sets the low/high/current value of the histogram.
 */
void setCDKHistogramValue (CDKHISTOGRAM *histogram, int low, int high, int value);
int getCDKHistogramValue (CDKHISTOGRAM *histogram);
int getCDKHistogramLowValue (CDKHISTOGRAM *histogram);
int getCDKHistogramHighValue (CDKHISTOGRAM *histogram);

/*
 * This sets the view type of the histogram.
 */
void setCDKHistogramDisplayType (CDKHISTOGRAM *histogram, EHistogramDisplayType viewtype);
EHistogramDisplayType getCDKHistogramViewType (CDKHISTOGRAM *histogram);

/*
 * This returns the filler character used to draw the histogram.
 */
void setCDKHistogramFillerChar (CDKHISTOGRAM *histogram, chtype character);
chtype getCDKHistogramFillerChar (CDKHISTOGRAM *histogram);

/*
 * This states where the statsitics value is to be located on the histogram.
 */
void setCDKHistogramStatsPos (CDKHISTOGRAM *histogram, int statsPos);
int getCDKHistogramStatsPos (CDKHISTOGRAM *histogram);

/*
 * This sets the attribute of the statistics.
 */
void setCDKHistogramStatsAttr (CDKHISTOGRAM *histogram, chtype statsAttr);
chtype getCDKHistogramStatsAttr (CDKHISTOGRAM *histogram);

/*
 * This sets the box attribute of the widget.
 */
void setCDKHistogramBox (CDKHISTOGRAM *histogram, boolean Box);
boolean getCDKHistogramBox (CDKHISTOGRAM *histogram);

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKHistogramULChar (CDKHISTOGRAM *histogram, chtype character);
void setCDKHistogramURChar (CDKHISTOGRAM *histogram, chtype character);
void setCDKHistogramLLChar (CDKHISTOGRAM *histogram, chtype character);
void setCDKHistogramLRChar (CDKHISTOGRAM *histogram, chtype character);
void setCDKHistogramVerticalChar (CDKHISTOGRAM *histogram, chtype character);
void setCDKHistogramHorizontalChar (CDKHISTOGRAM *histogram, chtype character);
void setCDKHistogramBoxAttribute (CDKHISTOGRAM *histogram, chtype character);

/*
 * This sets the background color of the widget.
 */ 
void setCDKHistogramBackgroundColor (CDKHISTOGRAM *histogram, char *color);

/*
 * This draw the widget on the screen.
 */
void drawCDKHistogram (CDKHISTOGRAM *histogram, boolean Box);

/*
 * This removes the widget from the screen.
 */
void eraseCDKHistogram (CDKHISTOGRAM *histogram);

/*
 * This moves the widget to the given location.
 */
void moveCDKHistogram (CDKHISTOGRAM *histogram, int xpos, int ypos, boolean relative, boolean refresh);

/*
 * This allows the user to interactively positition the widget on the screen.
 */
void positionCDKHistogram (CDKHISTOGRAM *histogram);

/*
 * This destroys the histogram and all related pointers.
 */
void destroyCDKHistogram (CDKHISTOGRAM *histogram);
#endif
