#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2004/08/30 00:22:24 $
 * $Revision: 1.73 $
 */

DeclareCDKObjects(MARQUEE, Marquee, setCdk, Unknown);

/*
 * This creates a marquee widget.
 */
CDKMARQUEE *newCDKMarquee (CDKSCREEN *cdkscreen, int xplace, int yplace, int width, boolean Box, boolean shadow)
{
   CDKMARQUEE *marquee	= 0;
   int parentWidth	= getmaxx(cdkscreen->window);
   int xpos		= xplace;
   int ypos		= yplace;
   int boxHeight	= 3;
   int boxWidth		= width;

   if ((marquee = newCDKObject(CDKMARQUEE, &my_funcs)) == 0)
      return (0);

   setCDKMarqueeBox (marquee, Box);

  /*
   * If the width is a negative value, the width will
   * be COLS-width, otherwise, the width will be the
   * given width.
   */
   boxWidth = setWidgetDimension (parentWidth, width, 0);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Create the marquee pointer. */
   ScreenOf(marquee)	= cdkscreen;
   marquee->parent	= cdkscreen->window;
   marquee->win		= newwin (boxHeight, boxWidth, ypos, xpos);
   marquee->boxHeight	= boxHeight;
   marquee->boxWidth	= boxWidth;
   marquee->shadowWin	= 0;
   marquee->active	= TRUE;
   marquee->width	= width;
   marquee->shadow	= shadow;

   /* Is the window null??? */
   if (marquee->win == 0)
   {
      destroyCDKObject(marquee);
      return (0);
   }

   /* Do we want a shadow? */
   if (shadow)
   {
      marquee->shadowWin = subwin (cdkscreen->window, boxHeight, boxWidth, ypos+1, xpos+1);
   }

   keypad (marquee->win, TRUE);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vMARQUEE, marquee);

   /* Return the marquee pointer. */
   return(marquee);
}

/*
 * This activates the marquee.
 */
int activateCDKMarquee (CDKMARQUEE *marquee, char *mesg, int delay, int repeat, boolean Box)
{
   chtype *message;
   int mesgLength	= 0;
   int startPos		= 0;
   int firstChar	= 0;
   int lastChar		= 1;
   int repeatCount	= 0;
   int viewSize		= 0;
   int viewLimit;
   int padding;
   int x, y, junk, oldcurs;
   bool firstTime	= TRUE;

   /* Make sure the message has some content. */
   if (mesg == 0 || *mesg == '\0')
   {
      return (-1);
   }

   /* Keep the box info, setting BorderOf() */
   setCDKMarqueeBox(marquee, Box);

   padding = (mesg[strlen(mesg) - 1] == ' ') ? 0 : 1;

   /* Translate the char * to a chtype * */
   message = char2Chtype (mesg, &mesgLength, &junk);

   /* Draw in the marquee. */
   drawCDKMarquee (marquee, ObjOf(marquee)->box);
   viewLimit = marquee->width - (2 * BorderOf(marquee));

   /* Start doing the marquee thing... */
   oldcurs = curs_set(0);
   while (marquee->active)
   {
      if (firstTime)
      {
	 firstChar = 0;
	 lastChar = 1;
	 viewSize = lastChar - firstChar;
	 startPos = marquee->width - viewSize - BorderOf(marquee);

	 firstTime = FALSE;
      }

      /* Draw in the characters. */
      y = firstChar;
      for (x=startPos ; x < (startPos + viewSize) ; x++)
      {
	 chtype ch = (y < mesgLength) ? message[y] : ' ';
	 mvwaddch (marquee->win, BorderOf(marquee), x, ch);
	 y++;
      }
      wrefresh (marquee->win);

      /* Set my variables. */
      if (mesgLength < viewLimit)
      {
	 if (lastChar < (mesgLength + padding))
	 {
	    lastChar ++;
	    viewSize ++;
	    startPos = marquee->width - viewSize - BorderOf(marquee);
	 }
	 else if (startPos > BorderOf(marquee))
	 {
	    /* This means the whole string is visible. */
	    startPos --;
	    viewSize = mesgLength + padding;
	 }
	 else
	 {
	   /* We have to start chopping the viewSize */
	   startPos = BorderOf(marquee);
	   firstChar++;
	   viewSize--;
	 }
      }
      else
      {
	 if (startPos > BorderOf(marquee))
	 {
	    lastChar ++;
	    viewSize ++;
	    startPos --;
	 }
	 else if (lastChar < (mesgLength + padding))
	 {
	    firstChar ++;
	    lastChar  ++;
	    startPos = BorderOf(marquee);
	    viewSize = viewLimit;
	 }
	 else
	 {
	    startPos = BorderOf(marquee);
	    firstChar++;
	    viewSize--;
	 }
      }

      /* OK, lets check if we have to start over. */
      if (viewSize <= 0 && firstChar == mesgLength + padding)
      {
	 /* Check if we repeat a specified number, or loop indefinitely. */
	 if ((repeat > 0) && (++repeatCount >= repeat))
	 {
	    break;
	 }

	 /* Time to start over.  */
	 mvwaddch (marquee->win, BorderOf(marquee), BorderOf(marquee), ' ');
	 wrefresh (marquee->win);
	 firstTime = TRUE;
      }

      /* Now sleep */
      napms (delay * 10);
   }
   if (oldcurs < 0)
      oldcurs = 1;
   curs_set(oldcurs);
   freeChtype (message);
   return (0);
}

/*
 * This de-activates a marquee widget.
 */
void deactivateCDKMarquee (CDKMARQUEE *marquee)
{
   marquee->active = FALSE;
}

/*
 * This moves the marquee field to the given location.
 */
static void _moveCDKMarquee (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKMARQUEE *marquee = (CDKMARQUEE *)object;
   int currentX = getbegx(marquee->win);
   int currentY = getbegy(marquee->win);
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
      xpos = getbegx(marquee->win) + xplace;
      ypos = getbegy(marquee->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(marquee), &xpos, &ypos, marquee->boxWidth, marquee->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(marquee->win, -xdiff, -ydiff);
   moveCursesWindow(marquee->shadowWin, -xdiff, -ydiff);

   /* Touch the windows so they 'move'. */
   refreshCDKWindow (WindowOf(marquee));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKMarquee (marquee, ObjOf(marquee)->box);
   }
}

/*
 * This draws the marquee widget on the screen.
 */
static void _drawCDKMarquee (CDKOBJS *object, boolean Box)
{
   CDKMARQUEE *marquee = (CDKMARQUEE *)object;

   /* Keep the box information. */
   ObjOf(marquee)->box	= Box;

   /* Do we need to draw a shadow??? */
   if (marquee->shadowWin != 0)
   {
      drawShadow (marquee->shadowWin);
   }

   /* Box it if needed. */
   if (Box)
   {
      drawObjBox (marquee->win, ObjOf(marquee));
   }

   /* Refresh the window. */
   refreshCDKWindow (marquee->win);
}

/*
 * This destroys the marquee.
 */
static void _destroyCDKMarquee (CDKOBJS *object)
{
   if (object != 0)
   {
      CDKMARQUEE *marquee = (CDKMARQUEE *)object;

      /* Clean up the windows. */
      deleteCursesWindow (marquee->shadowWin);
      deleteCursesWindow (marquee->win);

      /* Unregister this object. */
      unregisterCDKObject (vMARQUEE, marquee);
   }
}

/*
 * This erases the marquee.
 */
static void _eraseCDKMarquee (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKMARQUEE *marquee = (CDKMARQUEE *)object;

      eraseCursesWindow (marquee->win);
      eraseCursesWindow (marquee->shadowWin);
   }
}

/*
 * This sets the widgets box attribute.
 */
void setCDKMarqueeBox (CDKMARQUEE *marquee, boolean Box)
{
   ObjOf(marquee)->box = Box;
   ObjOf(marquee)->borderSize = Box ? 1 : 0;
}
boolean getCDKMarqueeBox (CDKMARQUEE *marquee)
{
   return ObjOf(marquee)->box;
}

/*
 * This sets the background attribute of the widget.
 */
static void _setBKattrMarquee (CDKOBJS *object, chtype attrib)
{
   if (object != 0)
   {
      CDKMARQUEE *marquee = (CDKMARQUEE *) object;

      wbkgd (marquee->win, attrib);
   }
}

dummyInject(Marquee)

dummyFocus(Marquee)

dummyUnfocus(Marquee)

dummyRefreshData(Marquee)

dummySaveData(Marquee)
