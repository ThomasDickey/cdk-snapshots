#ifndef CDKBUTTONBOX_H
#define CDKBUTTONBOX_H	1

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
 * Define the CDK buttonbox structure.
 */
struct SButtonBox {
   CDKSCREEN	*screen;
   WINDOW	*parent;
   WINDOW	*win;
   WINDOW	*shadowWin;
   chtype	*title[MAX_LINES];
   int		titlePos[MAX_LINES];
   int		titleLen[MAX_LINES];
   int		titleAdj;
   int		titleLines;
   chtype	*button[MAX_BUTTONS];
   int		buttonLen[MAX_BUTTONS];
   int		buttonPos[MAX_BUTTONS];
   int		columnWidths[MAX_BUTTONS];
   int		buttonCount;
   int		buttonWidth;
   int		currentButton;
   int		rows;
   int		cols;
   int		colAdjust;
   int		rowAdjust;
   int		boxWidth;
   int		boxHeight;
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
   chtype	highlight;
   int		screenIndex;
   BINDFN	bindFunction[MAX_BINDINGS];
   void *	bindData[MAX_BINDINGS];
   PROCESSFN	preProcessFunction;
   void *	preProcessData;
   PROCESSFN	postProcessFunction;
   void *	postProcessData;
};
typedef struct SButtonBox CDKBUTTONBOX;

/*
 * This returns a CDK buttonbox widget pointer.
 */
CDKBUTTONBOX *newCDKButtonbox (CDKSCREEN *cdkscreen,
				int xPos, int yPos,
				int height, int width, char *title,
				int rows, int cols,
				char **buttons, int buttonCount,
				chtype highlight,
				boolean Box, boolean shadow);

/*
 * This activates the widget.
 */
int activateCDKButtonbox (CDKBUTTONBOX *buttonbox, chtype *actions);

/*
 * This injects a single character into the widget.
 */
int injectCDKButtonbox (CDKBUTTONBOX *buttonbox, chtype input);

/*
 * This sets multiple attributes of the widget.
 */
void setCDKButtonbox (CDKBUTTONBOX *buttonbox, chtype highlight, boolean Box);
void setCDKButtonboxCurrentButton (CDKBUTTONBOX *buttonbox, int button);
int getCDKButtonboxCurrentButton (CDKBUTTONBOX *buttonbox);
int getCDKButtonboxButtonCount (CDKBUTTONBOX *buttonbox);

/*
 * This sets the highlight attribute for the buttonbox.
 */
void setCDKButtonboxHighlight (CDKBUTTONBOX *buttonbox, chtype highlight);
chtype getCDKButtonboxHighlight (CDKBUTTONBOX *buttonbox);

/*
 * This sets the box attribute of the widget.
 */
void setCDKButtonboxBox (CDKBUTTONBOX *buttonbox, boolean Box);
boolean getCDKButtonboxBox (CDKBUTTONBOX *buttonbox);

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKButtonboxULChar (CDKBUTTONBOX *buttonbox, chtype character);
void setCDKButtonboxURChar (CDKBUTTONBOX *buttonbox, chtype character);
void setCDKButtonboxLLChar (CDKBUTTONBOX *buttonbox, chtype character);
void setCDKButtonboxLRChar (CDKBUTTONBOX *buttonbox, chtype character);
void setCDKButtonboxVerticalChar (CDKBUTTONBOX *buttonbox, chtype character);
void setCDKButtonboxHorizontalChar (CDKBUTTONBOX *buttonbox, chtype character);
void setCDKButtonboxBoxAttribute (CDKBUTTONBOX *buttonbox, chtype character);

/*
 * This sets the background color of the widget.
 */ 
void setCDKButtonboxBackgroundColor (CDKBUTTONBOX *buttonbox, char *color);

/*
 * This draws a button in the buttonbox widget.
 */
void drawCDKButtonboxButton (CDKBUTTONBOX *buttonbox, int button,
				chtype active, chtype highlight);

/*
 * This draws the buttonbox box widget.
 */
void drawCDKButtonbox (CDKBUTTONBOX *buttonbox, boolean Box);
void drawCDKButtonboxButtons (CDKBUTTONBOX *buttonbox);

/*
 * This erases the buttonbox box from the screen.
 */
void eraseCDKButtonbox (CDKBUTTONBOX *buttonbox);

/*
 * This moves the buttonbox box to a new screen location.
 */
void moveCDKButtonbox (CDKBUTTONBOX *buttonbox, int xpos, int ypos, boolean relative, boolean refresh);

/*
 * This allows the user to position the widget on the screen interactively.
 */
void positionCDKButtonbox (CDKBUTTONBOX *buttonbox);

/*
 * This destroys the widget and all the memory associated with it.
 */
void destroyCDKButtonbox (CDKBUTTONBOX *buttonbox);

/*
 * This redraws the buttonbox box buttonboxs.
 */
void redrawCDKButtonboxButtonboxs (CDKBUTTONBOX *buttonbox);

/*
 * These set the pre/post process functions of the buttonbox widget.
 */
void setCDKButtonboxPreProcess (CDKBUTTONBOX *buttonbox, PROCESSFN callback, void *data);
void setCDKButtonboxPostProcess (CDKBUTTONBOX *buttonbox, PROCESSFN callback, void *data);
#endif
