#ifndef CDKALPHALIST_H
#define CDKALPHALIST_H	1

#include "cdk.h"
#include "entry.h"
#include "scroll.h"

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
 * Define the CDK alphalist widget structure.
 */
struct SAlphalist {
   CDKSCREEN	*screen;
   WINDOW	*parent;
   WINDOW	*win;
   WINDOW	*shadowWin;
   CDKENTRY	*entryField;
   CDKSCROLL	*scrollField;
   char		*list[MAX_ITEMS];
   int		listSize;
   int		xpos;
   int		ypos;
   int		height;
   int		width;
   int		boxHeight;
   int		boxWidth;
   chtype	highlight;
   chtype	fillerChar;
   boolean	box;
   boolean	shadow;
   EExitType	exitType;
   int		screenIndex;
   BINDFN	bindFunction[MAX_BINDINGS];
   void *	bindData[MAX_BINDINGS];
   PROCESSFN	preProcessFunction;
   void *	preProcessData;
   PROCESSFN	postProcessFunction;
   void *	postProcessData;
};
typedef struct SAlphalist CDKALPHALIST;

/*
 * This creates a pointer to a new CDK alphalist widget.
 */
CDKALPHALIST *newCDKAlphalist (CDKSCREEN *cdkscreen,
				int xpos, int ypos,
				int height, int width,
				char *title, char *label,
				char *list[], int listSize,
				chtype fillerChar, chtype highlight,
				boolean Box, boolean shadow);

/*
 * This allows the user to interact with the widget.
 */
char *activateCDKAlphalist (CDKALPHALIST *alphalist, chtype *actions);

/*
 * This injects a single character into the widget.
 */
char *injectCDKAlphalist (CDKALPHALIST *alphalist, chtype input);

/*
 * This sets multiple attributes of the alphalist widget.
 */
void setCDKAlphalist (CDKALPHALIST *alphalist,
			char *list[], int listSize,
			chtype fillerChar, chtype highlight,
			boolean Box);

/*
 * This sets the contents of the alpha list.
 */
void setCDKAlphalistContents (CDKALPHALIST *alphalist, char *list[], int listSize);
char **getCDKAlphalistContents (CDKALPHALIST *alphalist, int *size);

/*
 * This sets the filler character of the entry field of the alphalist.
 */
void setCDKAlphalistFillerChar (CDKALPHALIST *alphalist, chtype fillerCharacter);
chtype getCDKAlphalistFillerChar (CDKALPHALIST *alphalist);

/*
 * This sets the highlight bar attributes.
 */
void setCDKAlphalistHighlight (CDKALPHALIST *alphalist, chtype highlight);
chtype getCDKAlphalistHighlight (CDKALPHALIST *alphalist);

/*
 * This sets the box attibute of the widget.
 */
void setCDKAlphalistBox (CDKALPHALIST *alphalist, boolean Box);
boolean getCDKAlphalistBox (CDKALPHALIST *alphalist);

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKAlphalistULChar (CDKALPHALIST *alphalist, chtype character);
void setCDKAlphalistURChar (CDKALPHALIST *alphalist, chtype character);
void setCDKAlphalistLLChar (CDKALPHALIST *alphalist, chtype character);
void setCDKAlphalistLRChar (CDKALPHALIST *alphalist, chtype character);
void setCDKAlphalistVerticalChar (CDKALPHALIST *alphalist, chtype character);
void setCDKAlphalistHorizontalChar (CDKALPHALIST *alphalist, chtype character);
void setCDKAlphalistBoxAttribute (CDKALPHALIST *alphalist, chtype character);

/*
 * This sets the background color of the widget.
 */ 
void setCDKAlphalistBackgroundColor (CDKALPHALIST *alphalist, char *color);

/*
 * This draws the widget on the screen.
 */
void drawCDKAlphalist (CDKALPHALIST *alphalist, boolean Box);

/*
 * This removes the widget from the screen.
 */
void eraseCDKAlphalist (CDKALPHALIST *alphalist);

/*
 * This moves the widget to the location specified.
 */
void moveCDKAlphalist (CDKALPHALIST *alphalist, int xpos, int ypos, boolean relative, boolean refresh);

/*
 * This allows the user to interactively position the widget.
 */
void positionCDKAlphalist (CDKALPHALIST *alphalist);

/*
 * This destroys the widget and all the memory associated with it.
 */
void destroyCDKAlphalist (CDKALPHALIST *alphalist);

/*
 * These functions set the pre and post process functions for the widget.
 */
void setCDKAlphalistPreProcess (CDKALPHALIST *alphalist, PROCESSFN callback, void *data);
void setCDKAlphalistPostProcess (CDKALPHALIST *alphalist, PROCESSFN callback, void *data);
#endif
