#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2004/08/30 00:19:18 $
 * $Revision: 1.83 $
 */

DeclareCDKObjects(HISTOGRAM, Histogram, setCdk, Unknown);

/*
 * This creates a histogram widget.
 */
CDKHISTOGRAM *newCDKHistogram (CDKSCREEN *cdkscreen, int xplace, int yplace, int height, int width, int orient, char *title, boolean Box, boolean shadow)
{
   CDKHISTOGRAM *histogram	= 0;
   int parentWidth		= getmaxx(cdkscreen->window);
   int parentHeight		= getmaxy(cdkscreen->window);
   int boxWidth			= width;
   int boxHeight		= height;
   int xpos			= xplace;
   int ypos			= yplace;
   int oldWidth			= 0;
   int oldHeight		= 0;

   if ((histogram = newCDKObject(CDKHISTOGRAM, &my_funcs)) == 0)
      return (0);

   setCDKHistogramBox (histogram, Box);

  /*
   * If the height is a negative value, the height will
   * be ROWS-height, otherwise, the height will be the
   * given height.
   */
   boxHeight = setWidgetDimension (parentHeight, height, 2);
   oldHeight = boxHeight;

  /*
   * If the width is a negative value, the width will
   * be COLS-width, otherwise, the width will be the
   * given width.
   */
   boxWidth = setWidgetDimension (parentWidth, width, 0);
   oldWidth = boxWidth;

   boxWidth = setCdkTitle(ObjOf(histogram), title, - (boxWidth + 1));

   /* Increment the height by the number of lines in the title. */
   boxHeight += TitleLinesOf(histogram);

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth = (boxWidth > parentWidth ? oldWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? oldHeight : boxHeight);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Create the histogram pointer. */
   ScreenOf(histogram)		= cdkscreen;
   histogram->parent		= cdkscreen->window;
   histogram->win		= newwin (boxHeight, boxWidth, ypos, xpos);
   histogram->shadowWin		= 0;
   histogram->boxWidth		= boxWidth;
   histogram->boxHeight		= boxHeight;
   histogram->fieldWidth	= boxWidth - 2 * BorderOf(histogram);
   histogram->fieldHeight	= boxHeight - TitleLinesOf(histogram) - 2 * BorderOf(histogram);
   histogram->orient		= orient;
   histogram->shadow		= shadow;

   /* Is the window null. */
   if (histogram->win == 0)
   {
      destroyCDKObject(histogram);
      return (0);
   }
   keypad (histogram->win, TRUE);

   /* Set up some default values. */
   histogram->filler	= '#' | A_REVERSE;
   histogram->statsAttr = A_NORMAL;
   histogram->statsPos	= TOP;
   histogram->viewType	= vREAL;
   histogram->high	= 0;
   histogram->low	= 0;
   histogram->value	= 0;
   histogram->lowx	= 0;
   histogram->lowy	= 0;
   histogram->highx	= 0;
   histogram->highy	= 0;
   histogram->curx	= 0;
   histogram->cury	= 0;
   histogram->lowString = 0;
   histogram->highString = 0;
   histogram->curString = 0;

   /* Do we want a shadow? */
   if (shadow)
   {
      histogram->shadowWin = newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
   }

   /* Register this baby. */
   registerCDKObject (cdkscreen, vHISTOGRAM, histogram);

   /* Return this thing. */
   return (histogram);
}

/*
 * This was added for the builder.
 */
void activateCDKHistogram (CDKHISTOGRAM *histogram, chtype *actions GCC_UNUSED)
{
   drawCDKHistogram (histogram, ObjOf(histogram)->box);
}

/*
 * This sets various histogram attributes.
 */
void setCDKHistogram (CDKHISTOGRAM *histogram, EHistogramDisplayType viewType, int statsPos, chtype statsAttr, int low, int high, int value, chtype filler, boolean Box)
{
   setCDKHistogramDisplayType (histogram, viewType);
   setCDKHistogramStatsPos (histogram, statsPos);
   setCDKHistogramValue (histogram, low, high, value);
   setCDKHistogramFillerChar (histogram, filler);
   setCDKHistogramStatsAttr (histogram, statsAttr);
   setCDKHistogramBox (histogram, Box);
}

/*
 * This sets the values for the histogram.
 */
void setCDKHistogramValue (CDKHISTOGRAM *histogram, int low, int high, int value)
{
   char string[100];
   int len;

   /* We should error check the information we have. */
   histogram->low	= (low <= high ? low : 0);
   histogram->high	= (low <= high ? high : 0);
   histogram->value	= (low <= value && value <= high ? value : 0);

   /* Determine the percentage of the given value. */
   histogram->percent	= (histogram->high == 0 ? 0 : ((float)histogram->value / (float)histogram->high));

   /* Determine the size of the histogram bar. */
   if (histogram->orient == VERTICAL)
   {
      histogram->barSize = (int)(histogram->percent * (float)histogram->fieldHeight);
   }
   else
   {
      histogram->barSize = (int)(histogram->percent * (float)histogram->fieldWidth);
   }

  /*
   * We have a number of variables which determine the personality of
   * the histogram. We have to go through each one methodically, and
   * set them correctly. This section does this.
   */
   if (histogram->viewType != vNONE)
   {
      if (histogram->orient == VERTICAL)
      {
	 if (histogram->statsPos == LEFT || histogram->statsPos == BOTTOM)
	 {
	    /* Free the space used by the character strings. */
	    freeChar (histogram->lowString);
	    freeChar (histogram->highString);
	    freeChar (histogram->curString);

	    /* Set the low label attributes. */
	    sprintf (string, "%d", histogram->low);
	    len				= (int)strlen (string);
	    histogram->lowString	= copyChar (string);
	    histogram->lowx		= 1;
	    histogram->lowy		= histogram->boxHeight - len - 1;

	    /* Set the high label attributes. */
	    sprintf (string, "%d", histogram->high);
	    histogram->highString	= copyChar (string);
	    histogram->highx		= 1;
	    histogram->highy		= TitleLinesOf(histogram) + 1;

	    /* Set the current value attributes. */
	    if (histogram->viewType == vPERCENT)
	    {
	       sprintf (string, "%3.1f%%", (float) (histogram->percent * 100));
	    }
	    else if (histogram->viewType == vFRACTION)
	    {
	       sprintf (string, "%d/%d", histogram->value, histogram->high);
	    }
	    else
	    {
	       sprintf (string, "%d", histogram->value);
	    }
	    len				= (int)strlen (string);
	    histogram->curString	= copyChar (string);
	    histogram->curx		= 1;
	    histogram->cury		= ((histogram->fieldHeight - len) / 2) + TitleLinesOf(histogram) + 1;
	 }
	 else if (histogram->statsPos == CENTER)
	 {
	    /* Set the character strings correctly. */
	    freeChar (histogram->lowString);
	    freeChar (histogram->highString);
	    freeChar (histogram->curString);

	    /* Set the low label attributes. */
	    sprintf (string, "%d", histogram->low);
	    len				= (int)strlen (string);
	    histogram->lowString	= copyChar (string);
	    histogram->lowx		= (histogram->fieldWidth/2) + 1;
	    histogram->lowy		= histogram->boxHeight - len - 1;

	    /* Set the high label attributes. */
	    sprintf (string, "%d", histogram->high);
	    histogram->highString	= copyChar (string);
	    histogram->highx		= (histogram->fieldWidth/2) + 1;
	    histogram->highy		= TitleLinesOf(histogram) + 1;

	    /* Set the stats label attributes. */
	    if (histogram->viewType == vPERCENT)
	    {
	       sprintf (string, "%3.2f%%", (float) (histogram->percent * 100));
	    }
	    else if (histogram->viewType == vFRACTION)
	    {
	       sprintf (string, "%d/%d", histogram->value, histogram->high);
	    }
	    else
	    {
	       sprintf (string, "%d", histogram->value);
	    }
	    len				= (int)strlen (string);
	    histogram->curString	= copyChar (string);
	    histogram->curx		= (histogram->fieldWidth/2) + 1;
	    histogram->cury		= ((histogram->fieldHeight - len)/2) + TitleLinesOf(histogram) + 1;
	 }
	 else if (histogram->statsPos == RIGHT || histogram->statsPos == TOP)
	 {
	    /* Set the character strings correctly. */
	    freeChar (histogram->lowString);
	    freeChar (histogram->highString);
	    freeChar (histogram->curString);

	    /* Set the low label attributes. */
	    sprintf (string, "%d", histogram->low);
	    len				= (int)strlen (string);
	    histogram->lowString	= copyChar (string);
	    histogram->lowx		= histogram->fieldWidth;
	    histogram->lowy		= histogram->boxHeight - len - 1;

	    /* Set the high label attributes. */
	    sprintf (string, "%d", histogram->high);
	    histogram->highString	= copyChar (string);
	    histogram->highx		= histogram->fieldWidth;
	    histogram->highy		= TitleLinesOf(histogram) + 1;

	    /* Set the stats label attributes. */
	    if (histogram->viewType == vPERCENT)
	    {
	       sprintf (string, "%3.2f%%", (float) (histogram->percent * 100));
	    }
	    else if (histogram->viewType == vFRACTION)
	    {
	       sprintf (string, "%d/%d", histogram->value, histogram->high);
	    }
	    else
	    {
	       sprintf (string, "%d", histogram->value);
	    }
	    len				= (int)strlen (string);
	    histogram->curString	= copyChar (string);
	    histogram->curx		= histogram->fieldWidth;
	    histogram->cury		= ((histogram->fieldHeight - len)/2) + TitleLinesOf(histogram) + 1;
	 }
      }
      else
      {
	 /* Alignment is HORIZONTAL. */
	 if (histogram->statsPos == TOP || histogram->statsPos == RIGHT)
	 {
	    /* Set the character strings correctly. */
	    freeChar (histogram->lowString);
	    freeChar (histogram->highString);
	    freeChar (histogram->curString);

	    /* Set the low label attributes. */
	    sprintf (string, "%d", histogram->low);
	    histogram->lowString	= copyChar (string);
	    histogram->lowx		= 1;
	    histogram->lowy		= TitleLinesOf(histogram) + 1;

	    /* Set the high label attributes. */
	    sprintf (string, "%d", histogram->high);
	    len				= (int)strlen(string);
	    histogram->highString	= copyChar (string);
	    histogram->highx		= histogram->boxWidth - len - 1;
	    histogram->highy		= TitleLinesOf(histogram) + 1;

	    /* Set the stats label attributes. */
	    if (histogram->viewType == vPERCENT)
	    {
	       sprintf (string, "%3.1f%%", (float) (histogram->percent * 100));
	    }
	    else if (histogram->viewType == vFRACTION)
	    {
	       sprintf (string, "%d/%d", histogram->value, histogram->high);
	    }
	    else
	    {
	       sprintf (string, "%d", histogram->value);
	    }
	    len				= (int)strlen(string);
	    histogram->curString	= copyChar (string);
	    histogram->curx		= (histogram->fieldWidth - len)/2 + 1;
	    histogram->cury		= TitleLinesOf(histogram) + 1;
	 }
	 else if (histogram->statsPos == CENTER)
	 {
	    /* Set the character strings correctly. */
	    freeChar (histogram->lowString);
	    freeChar (histogram->highString);
	    freeChar (histogram->curString);

	    /* Set the low label attributes. */
	    sprintf (string, "%d", histogram->low);
	    histogram->lowString	= copyChar (string);
	    histogram->lowx		= 1;
	    histogram->lowy		= (histogram->fieldHeight/2) + TitleLinesOf(histogram) + 1;

	    /* Set the high label attributes. */
	    sprintf (string, "%d", histogram->high);
	    len				= (int)strlen (string);
	    histogram->highString	= copyChar (string);
	    histogram->highx		= histogram->boxWidth - len - 1;
	    histogram->highy		= (histogram->fieldHeight/2) + TitleLinesOf(histogram) + 1;

	    /* Set the stats label attributes. */
	    if (histogram->viewType == vPERCENT)
	    {
	       sprintf (string, "%3.1f%%", (float) (histogram->percent * 100));
	    }
	    else if (histogram->viewType == vFRACTION)
	    {
	       sprintf (string, "%d/%d", histogram->value, histogram->high);
	    }
	    else
	    {
	       sprintf (string, "%d", histogram->value);
	    }
	    len				= (int)strlen (string);
	    histogram->curString	= copyChar (string);
	    histogram->curx		= (histogram->fieldWidth - len)/2 + 1;
	    histogram->cury		= (histogram->fieldHeight/2) + TitleLinesOf(histogram) + 1;
	 }
	 else if (histogram->statsPos == BOTTOM || histogram->statsPos == LEFT)
	 {
	    /* Set the character strings correctly. */
	    freeChar (histogram->lowString);
	    freeChar (histogram->highString);
	    freeChar (histogram->curString);

	    /* Set the low label attributes. */
	    sprintf (string, "%d", histogram->low);
	    histogram->lowString	= copyChar (string);
	    histogram->lowx		= 1;
	    histogram->lowy		= histogram->boxHeight - 2 * BorderOf(histogram);

	    /* Set the high label attributes. */
	    sprintf (string, "%d", histogram->high);
	    len				= (int)strlen (string);
	    histogram->highString	= copyChar (string);
	    histogram->highx		= histogram->boxWidth - len - 1;
	    histogram->highy		= histogram->boxHeight - 2 * BorderOf(histogram);

	    /* Set the stats label attributes. */
	    if (histogram->viewType == vPERCENT)
	    {
	       sprintf (string, "%3.1f%%", (float) (histogram->percent * 100));
	    }
	    else if (histogram->viewType == vFRACTION)
	    {
	       sprintf (string, "%d/%d", histogram->value, histogram->high);
	    }
	    else
	    {
	       sprintf (string, "%d", histogram->value);
	    }
	    histogram->curString	= copyChar (string);
	    histogram->curx		= (histogram->fieldWidth - len)/2 + 1;
	    histogram->cury		= histogram->boxHeight - 2 * BorderOf(histogram);
	 }
      }
   }
}
int getCDKHistogramValue (CDKHISTOGRAM *histogram)
{
   return histogram->value;
}
int getCDKHistogramLowValue (CDKHISTOGRAM *histogram)
{
   return histogram->low;
}
int getCDKHistogramHighValue (CDKHISTOGRAM *histogram)
{
   return histogram->high;
}

/*
 * This sets the histogram display type.
 */
void setCDKHistogramDisplayType (CDKHISTOGRAM *histogram, EHistogramDisplayType viewType)
{
   histogram->viewType = viewType;
}
EHistogramDisplayType getCDKHistogramViewType (CDKHISTOGRAM *histogram)
{
   return histogram->viewType;
}

/*
 * This sets the position of the statistics information.
 */
void setCDKHistogramStatsPos (CDKHISTOGRAM *histogram, int statsPos)
{
   histogram->statsPos = statsPos;
}
int getCDKHistogramStatsPos (CDKHISTOGRAM *histogram)
{
   return histogram->statsPos;
}

/*
 * This sets the attribute of the statistics.
 */
void setCDKHistogramStatsAttr (CDKHISTOGRAM *histogram, chtype statsAttr)
{
   histogram->statsAttr = statsAttr;
}
chtype getCDKHistogramStatsAttr (CDKHISTOGRAM *histogram)
{
   return histogram->statsAttr;
}

/*
 * This sets the character to use when drawing the histogram.
 */
void setCDKHistogramFillerChar (CDKHISTOGRAM *histogram, chtype character)
{
   histogram->filler = character;
}
chtype getCDKHistogramFillerChar (CDKHISTOGRAM *histogram)
{
   return histogram->filler;
}

/*
 * This sets the histogram box attribute.
 */
void setCDKHistogramBox (CDKHISTOGRAM *histogram, boolean Box)
{
   ObjOf(histogram)->box = Box;
   ObjOf(histogram)->borderSize = Box ? 1 : 0;
}
boolean getCDKHistogramBox (CDKHISTOGRAM *histogram)
{
   return ObjOf(histogram)->box;
}

/*
 * This sets the background attribute of the widget.
 */
static void _setBKattrHistogram (CDKOBJS *object, chtype attrib)
{
   if (object != 0)
   {
      CDKHISTOGRAM *widget = (CDKHISTOGRAM *) object;
      wbkgd (widget->win, attrib);
   }
}

/*
 * This moves the histogram field to the given location.
 */
static void _moveCDKHistogram (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKHISTOGRAM *histogram = (CDKHISTOGRAM *)object;
   int currentX = getbegx(histogram->win);
   int currentY = getbegy(histogram->win);
   int xpos	= xplace;
   int ypos	= yplace;
   int xdiff	= 0;
   int ydiff	= 0;

   /*
    * If this is a relative move, then we will adjust where we want
    * to move to.
    */
   if (relative)
   {
      xpos = getbegx(histogram->win) + xplace;
      ypos = getbegy(histogram->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(histogram), &xpos, &ypos, histogram->boxWidth, histogram->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(histogram->win, -xdiff, -ydiff);
   moveCursesWindow(histogram->shadowWin, -xdiff, -ydiff);

   /* Touch the windows so they 'move'. */
   refreshCDKWindow (WindowOf(histogram));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKHistogram (histogram, ObjOf(histogram)->box);
   }
}

/*
 * This function draws the histogram.
 */
static void _drawCDKHistogram (CDKOBJS *object, boolean Box)
{
   CDKHISTOGRAM *histogram = (CDKHISTOGRAM *)object;
   chtype battr = 0;
   chtype bchar = 0;
   chtype fattr = histogram->filler & A_ATTRIBUTES;
   chtype fchar = CharOf(histogram->filler);
   int histX	= TitleLinesOf(histogram) + 1;
   int histY	= histogram->barSize;
   int len, x, y;

   /* Erase the window. */
   werase (histogram->win);

   /* Box the widget if asked. */
   if (Box)
   {
      drawObjBox (histogram->win, ObjOf(histogram));
   }

   /* Do we have a shadow to draw? */
   if (histogram->shadowWin != 0)
   {
      drawShadow (histogram->shadowWin);
   }

   drawCdkTitle (histogram->win, object);

   /* If the user asked for labels, draw them in. */
   if (histogram->viewType != vNONE)
   {
      /* Draw in the low label. */
      if (histogram->lowString != 0)
      {
	 len = (int)strlen (histogram->lowString);
	 writeCharAttrib (histogram->win,
				histogram->lowx,
				histogram->lowy,
				histogram->lowString,
				histogram->statsAttr,
				histogram->orient,
				0, len);
      }

      /* Draw in the current value label. */
      if (histogram->curString != 0)
      {
	 len = (int)strlen (histogram->curString);
	 writeCharAttrib (histogram->win,
				histogram->curx,
				histogram->cury,
				histogram->curString,
				histogram->statsAttr,
				histogram->orient,
				0, len);
      }

      /* Draw in the high label. */
      if (histogram->highString != 0)
      {
	 len = (int)strlen (histogram->highString);
	 writeCharAttrib (histogram->win,
				histogram->highx,
				histogram->highy,
				histogram->highString,
				histogram->statsAttr,
				histogram->orient,
				0, len);
      }
   }

  /*
   * If the orientation is vertical, set
   * where the bar will start drawing from.
   */
   if (histogram->orient == VERTICAL)
   {
      histX = histogram->boxHeight - histogram->barSize - 1;
      histY = histogram->fieldWidth;
   }

   /* Draw the histogram bar. */
   for (x=histX; x < histogram->boxHeight-1; x++)
   {
      for (y=1; y <= histY; y++)
      {
#ifdef HAVE_WINCHBUG
	 battr	= ' ' | A_REVERSE;
#else
	 battr	= mvwinch (histogram->win, x, y);
#endif
	 fchar	= battr & A_ATTRIBUTES;
	 bchar	= CharOf(battr);

	 if (bchar == ' ')
	 {
	    mvwaddch (histogram->win, x, y, histogram->filler);
	 }
	 else
	 {
	    mvwaddch (histogram->win, x, y, battr | fattr);
	 }
      }
   }

   /* Refresh the window. */
   refreshCDKWindow (histogram->win);
}

/*
 * This function destroys the histogram.
 */
static void _destroyCDKHistogram (CDKOBJS *object)
{
   if (object != 0)
   {
      CDKHISTOGRAM *histogram = (CDKHISTOGRAM *)object;

      freeChar (histogram->curString);
      freeChar (histogram->lowString);
      freeChar (histogram->highString);
      cleanCdkTitle (object);

      /* Clean up the windows. */
      deleteCursesWindow (histogram->shadowWin);
      deleteCursesWindow (histogram->win);

      /* Unregister this object. */
      unregisterCDKObject (vHISTOGRAM, histogram);
   }
}

/*
 * This function erases the histogram from the screen.
 */
static void _eraseCDKHistogram (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKHISTOGRAM *histogram = (CDKHISTOGRAM *)object;

      eraseCursesWindow (histogram->win);
      eraseCursesWindow (histogram->shadowWin);
   }
}

dummyInject(Histogram)

dummyFocus(Histogram)

dummyUnfocus(Histogram)

dummyRefreshData(Histogram)

dummySaveData(Histogram)
