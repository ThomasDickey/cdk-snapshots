#include "cdk.h"

/*
 * $Author: glovem $
 * $Date: 1997/04/25 12:50:55 $
 * $Revision: 1.34 $
 */

/*
 * This creates a marquee widget.
 */
CDKMARQUEE *newCDKMarquee (CDKSCREEN *cdkscreen, int xplace, int yplace, int width, boolean box, boolean shadow)
{
   CDKMARQUEE *marquee	= (CDKMARQUEE *)malloc (sizeof (CDKMARQUEE));
   int parentWidth	= WIN_WIDTH (cdkscreen->window);
   int xpos		= xplace;
   int ypos		= yplace;
   int boxHeight	= 3;
   int boxWidth		= width;

  /*
   * If the width is a negative value, the width will
   * be COLS-width, otherwise, the width will be the
   * given width.
   */
   boxWidth = setWidgetDimension (parentWidth, width, 0);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Create the marquee pointer. */
   marquee->parent	= cdkscreen->window;
   marquee->win		= newwin (boxHeight, boxWidth, ypos, xpos);
   marquee->boxHeight	= boxHeight;
   marquee->boxWidth	= boxWidth;
   marquee->shadowWin	= (WINDOW *)NULL;
   marquee->active	= TRUE;
   marquee->width	= width;
   marquee->box		= box;
   marquee->shadow	= shadow;
   marquee->ULChar	= ACS_ULCORNER;
   marquee->URChar	= ACS_URCORNER;
   marquee->LLChar	= ACS_LLCORNER;
   marquee->LRChar	= ACS_LRCORNER;
   marquee->HChar	= ACS_HLINE;
   marquee->VChar	= ACS_VLINE;
   marquee->BoxAttrib	= A_NORMAL;

   /* Is the window NULL??? */
   if (marquee->win == (WINDOW *)NULL)
   {
      /* Clean up any memory. */
      free (marquee);

      /* Return a NULL pointer. */
      return ( (CDKMARQUEE *)NULL );
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
int activateCDKMarquee (CDKMARQUEE *marquee, char *mesg, int delay, int repeat, boolean box)
{
   /* Declear local variables. */
   chtype *message;
   int mesgLength 	= 0;
   int startPos		= 0;
   int firstChar	= 0;
   int lastChar		= 1;
   int repeatCount	= 0;
   int viewSize		= 0;
   int x, y, junk;

   /* Keep the box info. */
   marquee->box = box;

   /* Make sure the message has some content. */
   if (mesg == (char *)NULL)
   {
      return (-1);
   }
   
   /* Translate the char * to a chtype * */
   message = char2Chtype (mesg, &mesgLength, &junk);

   /* Draw in the marquee. */
   drawCDKMarquee (marquee, marquee->box);

   /* Set up the variables. */
   viewSize = lastChar - firstChar;
   startPos = marquee->width - viewSize;
   if (marquee->box == TRUE)
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
         for (x=startPos ; x < (startPos+viewSize) ; x++)
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
            if (marquee->box)
            {
               startPos--;
            }
         }
   
         /* Now sleep */
         usleep ((delay * 10000));
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
void moveCDKMarquee (CDKMARQUEE *marquee, int xplace, int yplace, boolean relative, boolean refresh)
{
   /* Declare local variables. */
   int currentX = marquee->win->_begx;
   int currentY = marquee->win->_begy;
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
      xpos = marquee->win->_begx + xplace;
      ypos = marquee->win->_begy + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (marquee->screen->window, &xpos, &ypos, marquee->boxWidth, marquee->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   marquee->win->_begx = xpos;
   marquee->win->_begy = ypos;

   /* If there is a shadow box we have to move it too. */
   if (marquee->shadowWin != (WINDOW *)NULL)
   {
      marquee->shadowWin->_begx -= xdiff;
      marquee->shadowWin->_begy -= ydiff;
   }

   /* Touch the windows so they 'move'. */
   touchwin (marquee->screen->window);
   wrefresh (marquee->screen->window);

   /* Redraw the window, if they asked for it. */
   if (refresh)
   {
      drawCDKMarquee (marquee, marquee->box);
   }
}

/*
 * This allows the user to use the cursor keys to adjust the
 * position of the widget.
 */
void positionCDKMarquee (CDKMARQUEE *marquee)
{
   /* Declare some variables. */
   int origX	= marquee->win->_begx;
   int origY	= marquee->win->_begy;
   chtype key	= (chtype)NULL;

   /* Let them move the widget around until they hit return. */
   while ((key != KEY_RETURN) && (key != KEY_ENTER))
   {
      key = wgetch (marquee->win);
      if (key == KEY_UP || key == '8')
      {
         if (marquee->win->_begy > 0)
         {
            moveCDKMarquee (marquee, 0, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_DOWN || key == '2')
      {
         if (marquee->win->_begy+marquee->win->_maxy < marquee->screen->window->_maxy-1)
         {
            moveCDKMarquee (marquee, 0, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_LEFT || key == '4')
      {
         if (marquee->win->_begx > 0)
         {
            moveCDKMarquee (marquee, -1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_RIGHT || key == '6')
      {
         if (marquee->win->_begx+marquee->win->_maxx < marquee->screen->window->_maxx-1)
         {
            moveCDKMarquee (marquee, 1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '7')
      {
         if (marquee->win->_begy > 0 && marquee->win->_begx > 0)
         {
            moveCDKMarquee (marquee, -1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '9')
      {
         if (marquee->win->_begx+marquee->win->_maxx < marquee->screen->window->_maxx-1 &&
		marquee->win->_begy > 0)
         {
            moveCDKMarquee (marquee, 1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '1')
      {
         if (marquee->win->_begx > 0 && marquee->win->_begx+marquee->win->_maxx < marquee->screen->window->_maxx-1)
         {
            moveCDKMarquee (marquee, -1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '3')
      {
         if (marquee->win->_begx+marquee->win->_maxx < marquee->screen->window->_maxx-1 &&
		marquee->win->_begy+marquee->win->_maxy < marquee->screen->window->_maxy-1)
         {
            moveCDKMarquee (marquee, 1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '5')
      {
         moveCDKMarquee (marquee, CENTER, CENTER, FALSE, TRUE);
      }
      else if (key == 't')
      {
         moveCDKMarquee (marquee, marquee->win->_begx, TOP, FALSE, TRUE);
      }
      else if (key == 'b')
      {
         moveCDKMarquee (marquee, marquee->win->_begx, BOTTOM, FALSE, TRUE);
      }
      else if (key == 'l')
      {
         moveCDKMarquee (marquee, LEFT, marquee->win->_begy, FALSE, TRUE);
      }
      else if (key == 'r')
      {
         moveCDKMarquee (marquee, RIGHT, marquee->win->_begy, FALSE, TRUE);
      }
      else if (key == 'c')
      {
         moveCDKMarquee (marquee, CENTER, marquee->win->_begy, FALSE, TRUE);
      }
      else if (key == 'C')
      {
         moveCDKMarquee (marquee, marquee->win->_begx, CENTER, FALSE, TRUE);
      }
      else if (key == CDK_REFRESH)
      {
         eraseCDKScreen (marquee->screen);
         refreshCDKScreen (marquee->screen);
      }
      else if (key == KEY_ESC)
      {
         moveCDKMarquee (marquee, origX, origY, FALSE, TRUE);
      }
      else if ((key != KEY_RETURN) && (key != KEY_ENTER))
      {
         Beep();
      }
   }
}

/*
 * This draws the marquee widget on the screen.
 */
void drawCDKMarquee (CDKMARQUEE *marquee, boolean Box)
{
   /* Keep the box information. */
   marquee->box	= Box;

   /* Do we need to draw a shadow??? */
   if (marquee->shadowWin != (WINDOW *)NULL)
   {
      drawShadow (marquee->shadowWin);
   }

   /* Box it if needed. */
   if (Box)
   {
      attrbox (marquee->win,
		marquee->ULChar, marquee->URChar,
		marquee->LLChar, marquee->LRChar,
		marquee->HChar,  marquee->VChar,
		marquee->BoxAttrib);
   }

   /* Refresh the window. */
   touchwin (marquee->win);
   wrefresh (marquee->win);
}

/*
 * This destroys the marquee.
 */
void destroyCDKMarquee (CDKMARQUEE *marquee)
{
   /* Erase the object. */
   eraseCDKMarquee (marquee);

   /* Clean up the windows. */
   deleteCursesWindow (marquee->shadowWin);
   deleteCursesWindow (marquee->win);

   /* Unregister this object. */
   unregisterCDKObject (vMARQUEE, marquee);
   
   /* Finish cleaning up. */
   free (marquee);
}

/*
 * This erases the marquee.
 */
void eraseCDKMarquee (CDKMARQUEE *marquee)
{
   eraseCursesWindow (marquee->win);
   eraseCursesWindow (marquee->shadowWin);
}

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKMarqueeULChar (CDKMARQUEE *marquee, chtype character)
{
   marquee->ULChar = character;
}
void setCDKMarqueeURChar (CDKMARQUEE *marquee, chtype character)
{
   marquee->URChar = character;
}
void setCDKMarqueeLLChar (CDKMARQUEE *marquee, chtype character)
{
   marquee->LLChar = character;
}
void setCDKMarqueeLRChar (CDKMARQUEE *marquee, chtype character)
{
   marquee->LRChar = character;
}
void setCDKMarqueeVerticalChar (CDKMARQUEE *marquee, chtype character)
{
   marquee->VChar = character;
}
void setCDKMarqueeHorizontalChar (CDKMARQUEE *marquee, chtype character)
{
   marquee->HChar = character;
}
void setCDKMarqueeBoxAttribute (CDKMARQUEE *marquee, chtype character)
{
   marquee->BoxAttrib = character;
}

/*
 * This sets the background color of the widget.
 */ 
void setCDKMarqueeBackgroundColor (CDKMARQUEE *marquee, char *color)
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
   wbkgd (marquee->win, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

