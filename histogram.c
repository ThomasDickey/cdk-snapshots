#include "cdk.h"

/*
 * $Author: tom $
 * $Date: 1999/05/23 02:53:30 $
 * $Revision: 1.54 $
 */

static CDKFUNCS my_funcs = {
    _drawCDKHistogram,
    _eraseCDKHistogram,
};

/*
 * This creates a histogram widget.
 */
CDKHISTOGRAM *newCDKHistogram (CDKSCREEN *cdkscreen, int xplace, int yplace, int height, int width, int orient, char *title, boolean Box, boolean shadow)
{
   /* Declare local variables. */
   CDKHISTOGRAM *histogram	= newCDKObject(CDKHISTOGRAM, &my_funcs);
   int parentWidth		= getmaxx(cdkscreen->window) - 1;
   int parentHeight		= getmaxy(cdkscreen->window) - 1;
   int boxWidth			= width;
   int boxHeight		= height;
   int xpos			= xplace;
   int ypos			= yplace;
   int oldWidth			= 0;
   int oldHeight		= 0;
   char *temp[256];
   int x;

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

   /* Translate the char * items to chtype * */
   if (title != (char *)NULL)
   {
      /* We need to split the title on \n. */
      histogram->titleLines = splitString (title, temp, '\n');

      /* For each line in the title, convert from char * to chtype * */
      for (x=0; x < histogram->titleLines; x++)
      {
         histogram->title[x]	= char2Chtype (temp[x], &histogram->titleLen[x], &histogram->titlePos[x]);
         histogram->titlePos[x]	= justifyString (boxWidth, histogram->titleLen[x], histogram->titlePos[x]);
         freeChar (temp[x]);
      }
   }
   else
   {
      /* No title? Set the required variables. */
      histogram->titleLines = 0;
   }

   /* Increment the height by the number of lines in the title. */
   boxHeight += histogram->titleLines;

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
   histogram->shadowWin		= (WINDOW *)NULL;
   histogram->boxWidth		= boxWidth;
   histogram->boxHeight		= boxHeight;
   histogram->fieldWidth	= boxWidth-2;
   histogram->fieldHeight	= boxHeight-histogram->titleLines-2;
   histogram->orient		= orient;
   histogram->shadow		= shadow;
   histogram->ULChar		= ACS_ULCORNER;
   histogram->URChar		= ACS_URCORNER;
   histogram->LLChar		= ACS_LLCORNER;
   histogram->LRChar		= ACS_LRCORNER;
   histogram->HChar		= ACS_HLINE;
   histogram->VChar		= ACS_VLINE;
   histogram->BoxAttrib		= A_NORMAL;

   /* Is the window NULL. */
   if (histogram->win == (WINDOW *)NULL)
   {
      /* Clean up any memory used. */
      free (histogram);

      /* Return a NULL pointer. */
      return ((CDKHISTOGRAM *)NULL);
   }
   keypad (histogram->win, TRUE);

   /* Set up some default values. */
   histogram->filler	= '#' | A_REVERSE;
   histogram->statsAttr	= A_NORMAL;
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
   histogram->lowString	= (char *)NULL;
   histogram->highString	= (char *)NULL;
   histogram->curString	= (char *)NULL;
   ObjOf(histogram)->box	= Box;

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
   /* Declare local variables. */
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
            histogram->highy		= histogram->titleLines + 1;

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
            histogram->cury		= ((histogram->fieldHeight - len) / 2) + histogram->titleLines + 1;
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
            histogram->highy		= histogram->titleLines + 1;

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
            histogram->cury		= ((histogram->fieldHeight - len)/2) + histogram->titleLines + 1;
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
            histogram->highy		= histogram->titleLines + 1;

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
            histogram->cury		= ((histogram->fieldHeight - len)/2) + histogram->titleLines + 1;
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
            histogram->lowy		= histogram->titleLines + 1;

            /* Set the high label attributes. */
            sprintf (string, "%d", histogram->high);
            len				= (int)strlen(string);
            histogram->highString	= copyChar (string);
            histogram->highx		= histogram->boxWidth - len - 1;
            histogram->highy		= histogram->titleLines + 1;

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
            histogram->cury		= histogram->titleLines + 1;
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
            histogram->lowy		= (histogram->fieldHeight/2) + histogram->titleLines + 1;

            /* Set the high label attributes. */
            sprintf (string, "%d", histogram->high);
            len				= (int)strlen (string);
            histogram->highString	= copyChar (string);
            histogram->highx		= histogram->boxWidth - len - 1;
            histogram->highy		= (histogram->fieldHeight/2) + histogram->titleLines + 1;

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
            histogram->cury		= (histogram->fieldHeight/2) + histogram->titleLines + 1;
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
            histogram->lowy		= histogram->boxHeight - 2;

            /* Set the high label attributes. */
            sprintf (string, "%d", histogram->high);
            len				= (int)strlen (string);
            histogram->highString	= copyChar (string);
            histogram->highx		= histogram->boxWidth - len - 1;
            histogram->highy		= histogram->boxHeight - 2;

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
            histogram->cury		= histogram->boxHeight - 2;
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
}
boolean getCDKHistogramBox (CDKHISTOGRAM *histogram)
{
   return ObjOf(histogram)->box;
}

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKHistogramULChar (CDKHISTOGRAM *histogram, chtype character)
{
   histogram->ULChar = character;
}
void setCDKHistogramURChar (CDKHISTOGRAM *histogram, chtype character)
{
   histogram->URChar = character;
}
void setCDKHistogramLLChar (CDKHISTOGRAM *histogram, chtype character)
{
   histogram->LLChar = character;
}
void setCDKHistogramLRChar (CDKHISTOGRAM *histogram, chtype character)
{
   histogram->LRChar = character;
}
void setCDKHistogramVerticalChar (CDKHISTOGRAM *histogram, chtype character)
{
   histogram->VChar = character;
}
void setCDKHistogramHorizontalChar (CDKHISTOGRAM *histogram, chtype character)
{
   histogram->HChar = character;
}
void setCDKHistogramBoxAttribute (CDKHISTOGRAM *histogram, chtype character)
{
   histogram->BoxAttrib = character;
}

/*
 * This sets the background color of the widget.
 */
void setCDKHistogramBackgroundColor (CDKHISTOGRAM *histogram, char *color)
{
   chtype *holder = (chtype *)NULL;
   int junk1, junk2;

   /* Make sure the color isn't NULL. */
   if (color == (char *)NULL)
   {
      return;
   }

   /* Convert the value of the environment variable to a chtype. */
   holder = char2Chtype (color, &junk1, &junk2);

   /* Set the widgets background color. */
   wbkgd (histogram->win, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This moves the histogram field to the given location.
 */
void moveCDKHistogram (CDKHISTOGRAM *histogram, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   /* Declare local variables. */
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

   /* If there is a shadow box we have to move it too. */
   if (histogram->shadowWin != (WINDOW *)NULL)
   {
      moveCursesWindow(histogram->shadowWin, -xdiff, -ydiff);
   }

   /* Touch the windows so they 'move'. */
   touchwin (WindowOf(histogram));
   wrefresh (WindowOf(histogram));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKHistogram (histogram, ObjOf(histogram)->box);
   }
}

/*
 * This allows the user to use the cursor keys to adjust the
 * position of the widget.
 */
void positionCDKHistogram (CDKHISTOGRAM *histogram)
{
   /* Declare some variables. */
   int origX	= getbegx(histogram->win);
   int origY	= getbegy(histogram->win);
   chtype key	= (chtype)NULL;

   /* Let them move the widget around until they hit return. */
   while ((key != KEY_RETURN) && (key != KEY_ENTER))
   {
      key = wgetch (histogram->win);
      if (key == KEY_UP || key == '8')
      {
         if (getbegy(histogram->win) > 0)
         {
            moveCDKHistogram (histogram, 0, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_DOWN || key == '2')
      {
         if (getendy(histogram->win) < getmaxy(WindowOf(histogram))-1)
         {
            moveCDKHistogram (histogram, 0, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_LEFT || key == '4')
      {
         if (getbegx(histogram->win) > 0)
         {
            moveCDKHistogram (histogram, -1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_RIGHT || key == '6')
      {
         if (getendx(histogram->win) < getmaxx(WindowOf(histogram))-1)
         {
            moveCDKHistogram (histogram, 1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '7')
      {
         if (getbegy(histogram->win) > 0 && getbegx(histogram->win) > 0)
         {
            moveCDKHistogram (histogram, -1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '9')
      {
         if (getendx(histogram->win) < getmaxx(WindowOf(histogram))-1
	  && getbegy(histogram->win) > 0)
         {
            moveCDKHistogram (histogram, 1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '1')
      {
         if (getbegx(histogram->win) > 0 && getendx(histogram->win) < getmaxx(WindowOf(histogram))-1)
         {
            moveCDKHistogram (histogram, -1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '3')
      {
         if (getendx(histogram->win) < getmaxx(WindowOf(histogram))-1
	  && getendy(histogram->win) < getmaxy(WindowOf(histogram))-1)
         {
            moveCDKHistogram (histogram, 1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '5')
      {
         moveCDKHistogram (histogram, CENTER, CENTER, FALSE, TRUE);
      }
      else if (key == 't')
      {
         moveCDKHistogram (histogram, getbegx(histogram->win), TOP, FALSE, TRUE);
      }
      else if (key == 'b')
      {
         moveCDKHistogram (histogram, getbegx(histogram->win), BOTTOM, FALSE, TRUE);
      }
      else if (key == 'l')
      {
         moveCDKHistogram (histogram, LEFT, getbegy(histogram->win), FALSE, TRUE);
      }
      else if (key == 'r')
      {
         moveCDKHistogram (histogram, RIGHT, getbegy(histogram->win), FALSE, TRUE);
      }
      else if (key == 'c')
      {
         moveCDKHistogram (histogram, CENTER, getbegy(histogram->win), FALSE, TRUE);
      }
      else if (key == 'C')
      {
         moveCDKHistogram (histogram, getbegx(histogram->win), CENTER, FALSE, TRUE);
      }
      else if (key == CDK_REFRESH)
      {
         eraseCDKScreen (ScreenOf(histogram));
         refreshCDKScreen (ScreenOf(histogram));
      }
      else if (key == KEY_ESC)
      {
         moveCDKHistogram (histogram, origX, origY, FALSE, TRUE);
      }
      else if ((key != KEY_RETURN) && (key != KEY_ENTER))
      {
         Beep();
      }
   }
}

/*
 * This function draws the histogram.
 */
void _drawCDKHistogram (CDKOBJS *object, boolean Box)
{
   CDKHISTOGRAM *histogram = (CDKHISTOGRAM *)object;
   chtype battr	= (chtype)NULL;
   chtype bchar	= (chtype)NULL;
   chtype fattr	= histogram->filler & A_ATTRIBUTES;
   chtype fchar	= histogram->filler & A_CHARTEXT;
   int histX	= histogram->titleLines + 1;
   int histY	= histogram->barSize;
   int len, x, y;

   /* Erase the window. */
   werase (histogram->win);

   /* Box the widget if asked. */
   if (Box)
   {
      attrbox (histogram->win,
		histogram->ULChar, histogram->URChar,
		histogram->LLChar, histogram->LRChar,
		histogram->HChar,  histogram->VChar,
		histogram->BoxAttrib);
   }

   /* Do we have a shadow to draw? */
   if (histogram->shadowWin != (WINDOW *)NULL)
   {
      drawShadow (histogram->shadowWin);
   }

   /* Draw in the title if there is one. */
   if (histogram->titleLines != 0)
   {
      for (x=0; x < histogram->titleLines; x++)
      {
         writeChtype (histogram->win,
			histogram->titlePos[x],
			x + 1,
			histogram->title[x],
			HORIZONTAL, 0,
			histogram->titleLen[x]);
      }
   }

   /* If the user asked for labels, draw them in. */
   if (histogram->viewType != vNONE)
   {
      /* Draw in the low label. */
      if (histogram->lowString != (char *)NULL)
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
      if (histogram->curString != (char *)NULL)
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
      if (histogram->highString != (char *)NULL)
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
         bchar	= battr & A_CHARTEXT;

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
   touchwin (histogram->win);
   wrefresh (histogram->win);
}

/*
 * This function destroys the histogram.
 */
void destroyCDKHistogram (CDKHISTOGRAM *histogram)
{
   int x;

   /* Erase the object. */
   eraseCDKHistogram (histogram);

   /* Clean up the char pointers. */
   freeChar (histogram->curString);
   freeChar (histogram->lowString);
   freeChar (histogram->highString);
   for (x=0; x < histogram->titleLines; x++)
   {
      freeChtype (histogram->title[x]);
   }

   /* Clean up the windows. */
   deleteCursesWindow (histogram->shadowWin);
   deleteCursesWindow (histogram->win);

   /* Unregister this object. */
   unregisterCDKObject (vHISTOGRAM, histogram);

   /* Finish cleaning up. */
   free (histogram);
}

/*
 * This function erases the histogram from the screen.
 */
void _eraseCDKHistogram (CDKOBJS *object)
{
   CDKHISTOGRAM *histogram = (CDKHISTOGRAM *)object;

   eraseCursesWindow (histogram->win);
   eraseCursesWindow (histogram->shadowWin);
}
