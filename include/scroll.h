#ifndef CDKSCROLL_H
#define CDKSCROLL_H	1

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
 * Delcare scrolling list definitions.
 */
#define	NUMBERS		TRUE
#define	NONUMBERS	FALSE

/*
 * Delcare scrolling list definitions.
 */
struct SScroll {
   CDKSCREEN	*screen;
   WINDOW	*parent;
   WINDOW	*win;
   WINDOW	*scrollbarWin;
   WINDOW	*shadowWin;
   chtype	*title[MAX_LINES];
   int		titlePos[MAX_LINES];
   int		titleLen[MAX_LINES];
   int		titleAdj;
   int		titleLines;
   chtype	*item[MAX_ITEMS];
   int		itemPos[MAX_ITEMS];
   int		itemLen[MAX_ITEMS];
   int		maxTopItem;
   int		maxLeftChar;
   int		leftChar;
   int		lastItem;
   int		currentTop;
   int		currentItem;
   int		currentHigh;
   int		listSize;
   int		boxWidth;
   int		boxHeight;
   int		viewSize;
   int		scrollbarPlacement;
   boolean	scrollbar;
   int		toggleSize;
   int		togglePos;
   float	step;
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
   boolean	numbers;
   chtype	titlehighlight;
   chtype	highlight;
   int		screenIndex;
   BINDFN	bindFunction[MAX_BINDINGS];
   void *	bindData[MAX_BINDINGS];
   PROCESSFN	preProcessFunction;
   void *	preProcessData;
   PROCESSFN	postProcessFunction;
   void *	postProcessData;
};
typedef struct SScroll CDKSCROLL;

/*
 * This creates a new CDK scrolling list pointer.
 */
CDKSCROLL *newCDKScroll (CDKSCREEN *cdkscreen,
				int xpos, int ypos, int spos,
				int height, int width, char *title,
				char **scrollItems, int items, boolean numbers,
				chtype highlight, boolean Box, boolean shadow);

/*
 * This activates the scrolling list.
 */
int activateCDKScroll (CDKSCROLL *scroll, chtype *actions);

/*
 * This injects a single character into the scrolling list.
 */
int injectCDKScroll (CDKSCROLL *scroll, chtype input);

/*
 * This sets various attributes of the scrolling list.
 */
void setCDKScroll (CDKSCROLL *scroll, char **scrollItems, int listSize,
			boolean numbers, chtype highlight, boolean Box);

void setCDKScrollPosition (CDKSCROLL *scroll, int item);

/*
 * This sets the contents of the scrolling list.
 */
void setCDKScrollItems (CDKSCROLL *scroll,
			char **items, int itemCount, 
			boolean numbers);
int getCDKScrollItems (CDKSCROLL *scroll, char **items);

/*
 * This sets the highlight bar of the scrolling list.
 */
void setCDKScrollHighlight (CDKSCROLL *scroll, chtype highlight);
chtype getCDKScrollHighlight (CDKSCROLL *scroll, chtype highlight);

/*
 * This sets the box attribute of the widget.
 */
void setCDKScrollBox (CDKSCROLL *scroll, boolean Box);
boolean getCDKScrollBox (CDKSCROLL *scroll);

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKScrollULChar (CDKSCROLL *scroll, chtype character);
void setCDKScrollURChar (CDKSCROLL *scroll, chtype character);
void setCDKScrollLLChar (CDKSCROLL *scroll, chtype character);
void setCDKScrollLRChar (CDKSCROLL *scroll, chtype character);
void setCDKScrollVerticalChar (CDKSCROLL *scroll, chtype character);
void setCDKScrollHorizontalChar (CDKSCROLL *scroll, chtype character);
void setCDKScrollBoxAttribute (CDKSCROLL *scroll, chtype character);

/*
 * This sets the background color of the widget.
 */ 
void setCDKScrollBackgroundColor (CDKSCROLL *scroll, char *color);

/*
 * This adds a single item into the scrolling list.
 */
void addCDKScrollItem (CDKSCROLL *scroll, char *item);

/*
 * This deletes a single item from the scrolling list.
 */
void deleteCDKScrollItem (CDKSCROLL *scroll, int position);

/*
 * This draws the scrolling list on the screen.
 */
void drawCDKScroll (CDKSCROLL *scroll, boolean Box);

/*
 * This removes the scrolling list from the screen.
 */
void eraseCDKScroll (CDKSCROLL *scroll);

/*
 * This moves the widget to the given location.
 */
void moveCDKScroll (CDKSCROLL *scroll, int xpos, int ypos, boolean relative, boolean refresh);

/*
 * This allows the user to interactively position the widget on the screen.
 */
void positionCDKScroll (CDKSCROLL *scroll);

/*
 * This destroys the widget and all associated memory.
 */
void destroyCDKScroll (CDKSCROLL *scroll);

/*
 * These set the scrolling list pre/post process functions.
 */
void setCDKScrollPreProcess (CDKSCROLL *scroll, PROCESSFN callback, void *data);
void setCDKScrollPostProcess (CDKSCROLL *scroll, PROCESSFN callback, void *data);
#endif
