#ifndef CDKDIALOG_H
#define CDKDIALOG_H	1

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
 * Declare definitions the dialog box may need.
 */
#define	MAX_DIALOG_ROWS		50
#define	MAX_DIALOG_BUTTONS	25
#define	MIN_DIALOG_WIDTH	10

/*
 * Define the CDK dialog structure.
 */
struct SDialogBox {
   CDKSCREEN	*screen;
   WINDOW	*parent;
   WINDOW	*win;
   WINDOW	*shadowWin;
   chtype	*info[MAX_DIALOG_ROWS];
   int		infoLen[MAX_DIALOG_ROWS];
   int		infoPos[MAX_DIALOG_ROWS];
   chtype	*buttonLabel[MAX_DIALOG_BUTTONS];
   int		buttonLen[MAX_DIALOG_ROWS];
   int		buttonPos[MAX_DIALOG_ROWS];
   int		messageRows;
   int		buttonCount;
   int		currentButton;
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
   boolean	separator;
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
typedef struct SDialogBox CDKDIALOG;

/*
 * This returns a CDK dialog widget pointer.
 */
CDKDIALOG *newCDKDialog (CDKSCREEN *cdkscreen,
				int xPos, int yPos,
				char **message, int Rows,
				char **buttons, int buttonCount,
				chtype highlight,
				boolean separator, boolean Box, boolean shadow);

/*
 * This activates the widget.
 */
int activateCDKDialog (CDKDIALOG *dialog, chtype *actions);

/*
 * This injects a single character into the widget.
 */
int injectCDKDialog (CDKDIALOG *dialog, chtype input);

/*
 * This sets multiple attributes of the widget.
 */
void setCDKDialog (CDKDIALOG *dialog, chtype highlight, boolean separator, boolean Box);

/*
 * This sets the highlight attribute for the buttons.
 */
void setCDKDialogHighlight (CDKDIALOG *dialog, chtype highlight);
chtype getCDKDialogHighlight (CDKDIALOG *dialog);

/*
 * This sets whether or not the dialog box will have a separator line.
 */
void setCDKDialogSeparator (CDKDIALOG *dialog, boolean separator);
boolean getCDKDialogSeparator (CDKDIALOG *dialog);

/*
 * This sets the box attribute of the widget.
 */
void setCDKDialogBox (CDKDIALOG *dialog, boolean Box);
boolean getCDKDialogBox (CDKDIALOG *dialog);

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKDialogULChar (CDKDIALOG *dialog, chtype character);
void setCDKDialogURChar (CDKDIALOG *dialog, chtype character);
void setCDKDialogLLChar (CDKDIALOG *dialog, chtype character);
void setCDKDialogLRChar (CDKDIALOG *dialog, chtype character);
void setCDKDialogVerticalChar (CDKDIALOG *dialog, chtype character);
void setCDKDialogHorizontalChar (CDKDIALOG *dialog, chtype character);
void setCDKDialogBoxAttribute (CDKDIALOG *dialog, chtype character);

/*
 * This sets the background color of the widget.
 */ 
void setCDKDialogBackgroundColor (CDKDIALOG *dialog, char *color);

/*
 * This draws a dialog box button.
 */
void drawCDKDialogButton (CDKDIALOG *dialog, int button,
				boolean Box, chtype active, chtype highlight);

/*
 * This draws the dialog box widget.
 */
void drawCDKDialog (CDKDIALOG *dialog, boolean Box);

/*
 * This erases the dialog box from the screen.
 */
void eraseCDKDialog (CDKDIALOG *dialog);

/*
 * This moves the dialog box to a new screen location.
 */
void moveCDKDialog (CDKDIALOG *dialog, int xpos, int ypos, boolean relative, boolean refresh);

/*
 * This allows the user to position the widget on the screen interactively.
 */
void positionCDKDialog (CDKDIALOG *dialog);

/*
 * This destroys the widget and all the memory associated with it.
 */
void destroyCDKDialog (CDKDIALOG *dialog);

/*
 * This draws the dialog box buttons.
 */
void drawCDKDialogButtons (CDKDIALOG *dialog);

/*
 * These set the pre/post process functions of the dialog widget.
 */
void setCDKDialogPreProcess (CDKDIALOG *dialog, PROCESSFN callback, void *data);
void setCDKDialogPostProcess (CDKDIALOG *dialog, PROCESSFN callback, void *data);
#endif
