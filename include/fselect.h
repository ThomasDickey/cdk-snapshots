#ifndef CDKFSELECT_H
#define CDKFSELECT_H	1

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

#include "label.h"
#include "entry.h"

/*
 * Define the CDK file selector widget structure.
 */
struct SFileSelector {
   CDKSCREEN	*screen;
   WINDOW	*parent;
   WINDOW	*win;
   WINDOW	*shadowWin;
   CDKENTRY	*entryField;
   CDKSCROLL	*scrollField;
   char		*dirContents[MAX_ITEMS];
   int		fileCounter;
   char 	*pwd;
   char		*pathname;
   int		xpos;
   int		ypos;
   int		boxHeight;
   int		boxWidth;
   chtype	fieldAttribute;
   chtype	fillerCharacter;
   chtype	highlight;
   char		*dirAttribute;
   char		*fileAttribute;
   char		*linkAttribute;
   char		*sockAttribute;
   EExitType	exitType;
   boolean	box;
   boolean	shadow;
   int		screenIndex;
};
typedef struct SFileSelector CDKFSELECT;

/*
 * This creates a new CDK file selector widget.
 */
CDKFSELECT *newCDKFselect (CDKSCREEN *cdkscreen,
				int xpos, int ypos,
				int height, int width, 
				char *title, char *label,
				chtype fieldAttribute,
				chtype fillerChar,
				chtype highlight,
				char *dirAttributes, char *fileAttributes,
				char *linkAttribute, char *sockAttribute,
				boolean Box, boolean shadow);

/*
 * This activates the file selector.
 */
char *activateCDKFselect (CDKFSELECT *fselect, chtype *actions);

/*
 * This injects a single character into the widget.
 */
char *injectCDKFselect (CDKFSELECT *fselect, chtype input);

/*
 * This sets various attributes of the file selector.
 */
void setCDKFselect (CDKFSELECT *fselect, char *directory, 
			chtype fieldAttrib, chtype filler, chtype highlight, 
			char *dirAttribute, char *fileAttribute,
			char *linkAttribute, char *sockAttribute, 
			boolean Box);

/*
 * This sets the current directory of the file selector.
 */
int setCDKFselectDirectory (CDKFSELECT *fselect, char *directory);
char *getCDKFselectDirectory (CDKFSELECT *fselect);

/*
 * This sets the filler character of the entry field.
 */
void setCDKFselectFillerChar (CDKFSELECT *fselect, chtype fillerCharacter);
chtype getCDKFselectFillerChar (CDKFSELECT *fselect);

/*
 * This sets the highlight bar of the scrolling list.
 */
void setCDKFselectHighlight (CDKFSELECT *fselect, chtype highlight);
chtype getCDKFselectHighlight (CDKFSELECT *fselect);

/*
 * These functions set the attribute of the directories, links, 
 * files, and sockets in the scrolling list portion of the file
 * selector widget.
 */
void setCDKFselectDirAttribute (CDKFSELECT *fselect, char *attribute);
void setCDKFselectLinkAttribute (CDKFSELECT *fselect, char *attribute);
void setCDKFselectFileAttribute (CDKFSELECT *fselect, char *attribute);
void setCDKFselectSocketAttribute (CDKFSELECT *fselect, char *attribute);

/*
 * These functions return the attribute of the directories, links, 
 * files, and sockets in the scrolling list portion of the file
 * selector widget.
 */
char *getCDKFselectDirAttribute (CDKFSELECT *fselect);
char *getCDKFselectLinkAttribute (CDKFSELECT *fselect);
char *getCDKFselectFileAttribute (CDKFSELECT *fselect);
char *getCDKFselectSocketAttribute (CDKFSELECT *fselect);

/*
 * This sets the box attribute of the widget.
 */
void setCDKFselectBox (CDKFSELECT *fselect, boolean Box);
boolean getCDKFselectBox (CDKFSELECT *fselect);

/*
 * This sets the contents of the file selector.
 */
int setCDKFselectDirContents (CDKFSELECT *fselect);
char **getCDKFselectDirContents (CDKFSELECT *fselect, int *count);

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKFselectULChar (CDKFSELECT *fselect, chtype character);
void setCDKFselectURChar (CDKFSELECT *fselect, chtype character);
void setCDKFselectLLChar (CDKFSELECT *fselect, chtype character);
void setCDKFselectLRChar (CDKFSELECT *fselect, chtype character);
void setCDKFselectVerticalChar (CDKFSELECT *fselect, chtype character);
void setCDKFselectHorizontalChar (CDKFSELECT *fselect, chtype character);
void setCDKFselectBoxAttribute (CDKFSELECT *fselect, chtype character);

/*
 * This sets the background color of the widget.
 */ 
void setCDKFselectBackgroundColor (CDKFSELECT *fselect, char *color);

/*
 * This draws the widget.
 */
void drawCDKFselect (CDKFSELECT *fselect, boolean Box);

/*
 * This erases the widget.
 */
void eraseCDKFselect (CDKFSELECT *fselect);

/*
 * This moves the widget to the given location.
 */
void moveCDKFselect (CDKFSELECT *fselect, int xpos, int ypos, boolean relative, boolean refresh);

/*
 * This allows the user to interactively position the widget.
 */
void positionCDKFselect (CDKFSELECT *fselect);

/*
 * This destroys the widget and all the associated memory.
 */
void destroyCDKFselect (CDKFSELECT *fselect);

/*
 * This is a callback which allows you to delete files from within the
 * file selector. It is NOT an active default: it MUST be set by the
 * user.
 */
void deleteFileCB (EObjectType objectType, void *object, void *clientData);
#endif
