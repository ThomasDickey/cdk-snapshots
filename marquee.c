#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2003/11/30 21:15:51 $
 * $Revision: 1.65 $
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
   int x, y, junk;

   /* Keep the box info. */
   ObjOf(marquee)->box = Box;

   /* Make sure the message has some content. */
   if (mesg == 0)
   {
      return (-1);
   }

   /* Translate the char * to a chtype * */
   message = char2Chtype (mesg, &mesgLength, &junk);

   /* Draw in the marquee. */
   drawCDKMarquee (marquee, ObjOf(marquee)->box);

   /* Set up the variables. */
   viewSize = lastChar - firstChar;
   startPos = marquee->width - viewSize;
   if (ObjOf(marquee)->box == TRUE)
   {
      startPos--;
   }

   /* Start doing the marquee thing... */
   for (;;)
   {
      if (marquee->active)
      {
	 /* Draw in the characters. */
	 y = firstChar;
	 for (x=startPos ; x < (startPos + viewSize) ; x++)
	 {
	    mvwaddch (marquee->win, 1, x, message[y]);
	    y++;
	 }
	 wrefresh (marquee->win);

	 /* Set my variables. */
	 if (mesgLength < (marquee->width-2))
	 {
	    if (lastChar < mesgLength)
	    {
	       lastChar ++;
	       viewSize ++;
	       startPos = marquee->width - viewSize + 1;
	    }
	    else if (lastChar == mesgLength)
	    {
	       if (startPos > 1)
	       {
		  /* This means the whole string is visible. */
		  startPos --;
		  viewSize = mesgLength - 1;
	       }
	       else
	       {
		 /* We have to start chopping the viewSize */
		 startPos = 1;
		 firstChar++;
		 viewSize--;
	       }
	    }
	 }
	 else
	 {
	    if (startPos > 1)
	    {
	       lastChar ++;
	       viewSize ++;
	       startPos --;
	    }
	    else
	    {
	       if (lastChar < mesgLength)
	       {
		  firstChar ++;
		  lastChar  ++;
		  startPos = 1;
		  viewSize = marquee->width - 2;
	       }
	       else
	       {
		  firstChar ++;
		  viewSize --;
		  startPos = 1;
	       }
	    }
	 }

	 /* OK, lets check if we have to start over. */
	 if ( viewSize == 0 && firstChar == mesgLength)
	 {
	    /* Check if we need to repeat or not. */
	    repeatCount ++;
	    if (repeat > 0 && repeatCount == repeat)
	    {
	       freeChtype (message);
	       return (0);
	    }

	    /* Time to start over.  */
	    mvwaddch (marquee->win, 1, 1, ' '|A_NORMAL);
	    wrefresh (marquee->win);
	    firstChar = 0;
	    lastChar = 1;
	    viewSize = lastChar - firstChar;
	    startPos = marquee->width - viewSize;
	    if (ObjOf(marquee)->box)
	    {
	       startPos--;
	    }
	 }

	 /* Now sleep */
	 napms (delay * 10);
      }
   }
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
   touchwin (WindowOf(marquee));
   wrefresh (WindowOf(marquee));

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
   touchwin (marquee->win);
   wrefresh (marquee->win);
}

/*
 * This destroys the marquee.
 */
static void _destroyCDKMarquee (CDKOBJS *object)
{
   CDKMARQUEE *marquee = (CDKMARQUEE *)object;

   /* Clean up the windows. */
   deleteCursesWindow (marquee->shadowWin);
   deleteCursesWindow (marquee->win);

   /* Unregister this object. */
   unregisterCDKObject (vMARQUEE, marquee);
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
 * This sets the background color of the widget.
 */
void setCDKMarqueeBackgroundColor (CDKMARQUEE *marquee, char *color)
{
   chtype *holder = 0;
   int junk1, junk2;

   /* Make sure the color isn't null. */
   if (color == 0)
   {
      return;
   }

   /* Convert the value of the environment variable to a chtype. */
   holder = char2Chtype (color, &junk1, &junk2);

   /* Set the widgets background color. */
   setCDKMarqueeBackgroundAttrib (marquee, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This sets the background attribute of the widget.
 */
void setCDKMarqueeBackgroundAttrib (CDKMARQUEE *marquee, chtype attrib)
{
   /* Set the widgets background attribute. */
   wbkgd (marquee->win, attrib);
}

static int _injectCDKMarquee(CDKOBJS *object GCC_UNUSED, chtype input GCC_UNUSED)
{
   return 0;
}

static void _focusCDKMarquee(CDKOBJS *object GCC_UNUSED)
{
   /* FIXME */
}

static void _unfocusCDKMarquee(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _refreshDataCDKMarquee(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _saveDataCDKMarquee(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}
