#ifndef CDKGRAPH_H
#define CDKGRAPH_H	1

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
 * Define the CDK graph structure.
 */
struct SGraph {
   CDKSCREEN		*screen;
   WINDOW		*parent;
   WINDOW		*win;
   WINDOW		*shadowWin;
   chtype		*title[MAX_LINES];
   int			titlePos[MAX_LINES];
   int			titleLen[MAX_LINES];
   int			titleAdj;
   int			titleLines;
   chtype		*graphChar;
   boolean		box;
   boolean		shadow;
   chtype		ULChar;
   chtype		URChar;
   chtype		LLChar;
   chtype		LRChar;
   chtype		VChar;
   chtype		HChar;
   chtype		BoxAttrib;
   int			boxHeight;
   int			boxWidth;
   chtype		*xtitle;
   int			xtitlePos;
   int			xtitleLen;
   int			values[MAX_LINES];
   int			count;
   int			minx;
   int			maxx;
   int			xscale;
   int			txpos;
   chtype		*ytitle;
   int			ytitlePos;
   int			ytitleLen;
   int			yscale;
   int			typos;
   EGraphDisplayType	displayType;
   int			screenIndex;
};
typedef struct SGraph CDKGRAPH;

/*
 * This creates a new CDK graph pointer.
 */
CDKGRAPH *newCDKGraph (CDKSCREEN *cdkscreen,
			int xpos, int ypos, int height, int width,
			char *title, char *xtitle, char *ytitle);

/*
 * This was added to make the builder simpler. All this will
 * do is call drawCDKGraph.
 */
void activateCDKGraph (CDKGRAPH *graph, chtype *actions);

/*
 * This sets multiple attributes of the widget.
 */
int setCDKGraph (CDKGRAPH *graph, int *values, int count, char *graphChar,
			boolean startAtZero,
			EGraphDisplayType displayType);

/*
 * This sets the values of the graph widget.
 */
int setCDKGraphValues (CDKGRAPH *graph, int *values, int count, boolean startAtZero);
int setCDKGraphValue (CDKGRAPH *graph, int index, int value, boolean startAtZero);
int *getCDKGraphValues (CDKGRAPH *graph, int *size);
int getCDKGraphValue (CDKGRAPH *graph, int index);

/*
 * This sets the characters of the graph widget.
 */
int setCDKGraphCharacters (CDKGRAPH *graph, char *characters);
int setCDKGraphCharacter (CDKGRAPH *graph, int index, char *character);
chtype *getCDKGraphCharacters (CDKGRAPH *graph);
chtype getCDKGraphCharacter (CDKGRAPH *graph, int index);

/*
 * This sets the display type of the graph.
 */
void setCDKGraphDisplayType (CDKGRAPH *graph, EGraphDisplayType type);
EGraphDisplayType getCDKGraphDisplayType (CDKGRAPH *graph);

/*
 * This sets the box attibute of the widget.
 */
void setCDKGraphBox (CDKGRAPH *graph, boolean Box);
boolean getCDKGraphBox (CDKGRAPH *graph);

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKGraphULChar (CDKGRAPH *graph, chtype character);
void setCDKGraphURChar (CDKGRAPH *graph, chtype character);
void setCDKGraphLLChar (CDKGRAPH *graph, chtype character);
void setCDKGraphLRChar (CDKGRAPH *graph, chtype character);
void setCDKGraphVerticalChar (CDKGRAPH *graph, chtype character);
void setCDKGraphHorizontalChar (CDKGRAPH *graph, chtype character);
void setCDKGraphBoxAttribute (CDKGRAPH *graph, chtype character);

/*
 * This sets the background color of the widget.
 */ 
void setCDKGraphBackgroundColor (CDKGRAPH *graph, char *color);

/*
 * This draws the graph on the screeen.
 */
void drawCDKGraph (CDKGRAPH *graph, boolean Box);

/*
 * This removes the graph from the screen.
 */
void eraseCDKGraph (CDKGRAPH *graph);

/*
 * This moves the widget to the given location.
 */
void moveCDKGraph (CDKGRAPH *graph, int xpos, int ypos, boolean relative, boolean refresh);

/*
 * This allows the user to interactively move the widget.
 */
void positionCDKGraph (CDKGRAPH *graph);

/*
 * This destroys the graph and all the associated memory.
 */
void destroyCDKGraph (CDKGRAPH *graph);
#endif
