#include "cdk.h"
#include <limits.h>

/*
 * $Author: glovem $
 * $Date: 1997/07/24 13:30:21 $
 * $Revision: 1.42 $
 */

/*
 * This creates a label widget.
 */
CDKLABEL *newCDKLabel(CDKSCREEN *cdkscreen, int xplace, int yplace, char **mesg, int rows, boolean box, boolean shadow)
{
   /* Maintain the label information. */
   CDKLABEL *label	= (CDKLABEL *)malloc (sizeof (CDKLABEL));
   int parentWidth	= WIN_WIDTH (cdkscreen->window);
   int parentHeight	= WIN_HEIGHT (cdkscreen->window);
   int boxWidth		= INT_MIN;
   int boxHeight	= rows + 2;
   int xpos		= xplace;
   int ypos		= yplace;
   int x		= 0;

   /* Determine the box width. */
   for (x=0; x < rows; x++)
   {
      /* Translate the char * to a chtype. */
      label->info[x] = char2Chtype (mesg[x], &label->infoLen[x], &label->infoPos[x]);
      boxWidth = MAXIMUM (boxWidth, label->infoLen[x]);
   }
   boxWidth += 2;

   /* Create the string alignments. */
   for (x=0; x < rows; x++)
   {
      label->infoPos[x]	= justifyString (boxWidth, label->infoLen[x], label->infoPos[x]);
   }

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Create the label. */
   label->parent	= cdkscreen->window;
   label->win		= newwin (boxHeight, boxWidth, ypos, xpos);
   label->shadowWin	= (WINDOW *)NULL;
   label->xpos		= xpos;
   label->ypos		= ypos;
   label->rows		= rows;
   label->boxWidth	= boxWidth;
   label->boxHeight	= boxHeight;
   label->box		= box;
   label->shadow	= shadow;
   label->ULChar	= ACS_ULCORNER;
   label->URChar	= ACS_URCORNER;
   label->LLChar	= ACS_LLCORNER;
   label->LRChar	= ACS_LRCORNER;
   label->HChar		= ACS_HLINE;
   label->VChar		= ACS_VLINE;
   label->BoxAttrib	= A_NORMAL;

   /* Is the window NULL? */
   if (label->win == (WINDOW *)NULL) 
   {
      /* Free up any memory used. */
      for (x=0; x < rows; x++)
      {
         freeChtype (label->info[x]);
      }
      free(label);

      /* Return a NULL pointer. */
      return ( (CDKLABEL *)NULL );
   }
   keypad (label->win, TRUE);

   /* If a shadow was requested, then create the shadow window. */
   if (shadow)
   {
      label->shadowWin	= newwin (boxHeight, boxWidth, ypos+1, xpos+1);
   }

   /* Register this baby. */
   registerCDKObject (cdkscreen, vLABEL, label);

   /* Return the label pointer. */
   return (label);
}

/*
 * This was added for the builder.
 */
void activateCDKLabel (CDKLABEL *label, chtype *actions)
{
   drawCDKLabel (label, label->box);
}

/*
 * This sets multiple attributes of the widget.
 */
void setCDKLabel (CDKLABEL *label, char **mesg, int lines, boolean Box)
{
   setCDKLabelMessage (label, mesg, lines);
   setCDKLabelBox (label, Box);
}

/*
 * This sets the information within the label.
 */
void setCDKLabelMessage (CDKLABEL *label, char **info, int infoSize)
{ 
   /* Declare local vairables. */
   int x;

   /* Clean out the old message. */
   for (x=0; x < label->rows; x++)
   {
      freeChtype (label->info[x]);   
      label->infoPos[x] = 0;
      label->infoLen[x] = 0;
   }
   label->rows = (infoSize < label->rows ? infoSize : label->rows);

   /* Copy in the new message. */
   for (x=0; x < label->rows; x++)
   {
      label->info[x]	= char2Chtype (info[x], &label->infoLen[x], &label->infoPos[x]);
      label->infoPos[x]	= justifyString (label->boxWidth, label->infoLen[x], label->infoPos[x]);
   }

   /* Redraw the label widget. */
   eraseCDKLabel (label);
   drawCDKLabel (label, label->box);
}
chtype **getCDKLabelMessage (CDKLABEL *label, int *size)
{
   (*size) = label->rows;
   return label->info;
}

/*
 * This sets the box flag for the label widget.
 */
void setCDKLabelBox (CDKLABEL *label, boolean Box)
{ 
   label->box = Box;
}
boolean getCDKLabelBox (CDKLABEL *label)
{
   return label->box;
}

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKLabelULChar (CDKLABEL *label, chtype character)
{
   label->ULChar = character;
}
void setCDKLabelURChar (CDKLABEL *label, chtype character)
{
   label->URChar = character;
}
void setCDKLabelLLChar (CDKLABEL *label, chtype character)
{
   label->LLChar = character;
}
void setCDKLabelLRChar (CDKLABEL *label, chtype character)
{
   label->LRChar = character;
}
void setCDKLabelVerticalChar (CDKLABEL *label, chtype character)
{
   label->VChar = character;
}
void setCDKLabelHorizontalChar (CDKLABEL *label, chtype character)
{
   label->HChar = character;
}
void setCDKLabelBoxAttribute (CDKLABEL *label, chtype character)
{
   label->BoxAttrib = character;
}

/*
 * This sets the background color of the widget.
 */ 
void setCDKLabelBackgroundColor (CDKLABEL *label, char *color)
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
   wbkgd (label->win, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This draws the label widget.
 */
void drawCDKLabel (CDKLABEL *label, boolean Box)
{
   /* Declare local variables. */
   int x = 0;
   
   /* Is there a shadow? */
   if (label->shadowWin != (WINDOW *)NULL)
   {
      drawShadow (label->shadowWin);
   }

   /* Box the widget if asked. */
   if (label->box)
   {
      attrbox (label->win,
		label->ULChar, label->URChar,
		label->LLChar, label->LRChar,
		label->HChar,  label->VChar,
		label->BoxAttrib);
   }

   /* Draw in the message. */
   for (x=0; x < label->rows; x++)
   {
      writeChtype (label->win, label->infoPos[x], x+1, label->info[x], HORIZONTAL, 0, label->infoLen[x]);
   }

   /* Refresh the window. */
   touchwin (label->win);
   wrefresh (label->win);
}

/*
 * This erases the label widget.
 */
void eraseCDKLabel (CDKLABEL *label)
{
   eraseCursesWindow (label->win);
   eraseCursesWindow (label->shadowWin);
}

/*
 * This moves the label field to the given location.
 */
void moveCDKLabel (CDKLABEL *label, int xplace, int yplace, boolean relative, boolean refresh)
{
   /* Declare local variables. */
   int currentX = label->win->_begx;
   int currentY = label->win->_begy;
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
      xpos = label->win->_begx + xplace;
      ypos = label->win->_begy + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (label->screen->window, &xpos, &ypos, label->boxWidth, label->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   label->win->_begx = xpos;
   label->win->_begy = ypos;

   /* If there is a shadow box we have to move it too. */
   if (label->shadowWin != (WINDOW *)NULL)
   {
      label->shadowWin->_begx -= xdiff;
      label->shadowWin->_begy -= ydiff;
   }

   /* Touch the windows so they 'move'. */
   touchwin (label->screen->window);
   wrefresh (label->screen->window);

   /* Redraw the window, if they asked for it. */
   if (refresh)
   {
      drawCDKLabel (label, label->box);
   }
}

/*
 * This allows the user to use the cursor keys to adjust the
 * position of the widget.
 */
void positionCDKLabel (CDKLABEL *label)
{
   /* Declare some variables. */
   int origX	= label->win->_begx;
   int origY	= label->win->_begy;
   chtype key	= (chtype)NULL;

   /* Let them move the widget around until they hit return. */
   while ((key != KEY_RETURN) && (key != KEY_ENTER))
   {
      key = wgetch (label->win);
      if (key == KEY_UP || key == '8')
      {
         if (label->win->_begy > 0)
         {
            moveCDKLabel (label, 0, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_DOWN || key == '2')
      {
         if (label->win->_begy+label->win->_maxy < label->screen->window->_maxy-1)
         {
            moveCDKLabel (label, 0, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_LEFT || key == '4')
      {
         if (label->win->_begx > 0)
         {
            moveCDKLabel (label, -1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_RIGHT || key == '6')
      {
         if (label->win->_begx+label->win->_maxx < label->screen->window->_maxx-1)
         {
            moveCDKLabel (label, 1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '7')
      {
         if (label->win->_begy > 0 && label->win->_begx > 0)
         {
            moveCDKLabel (label, -1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '9')
      {
         if (label->win->_begx+label->win->_maxx < label->screen->window->_maxx-1 &&
		label->win->_begy > 0)
         {
            moveCDKLabel (label, 1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '1')
      {
         if (label->win->_begx > 0 && label->win->_begx+label->win->_maxx < label->screen->window->_maxx-1)
         {
            moveCDKLabel (label, -1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '3')
      {
         if (label->win->_begx+label->win->_maxx < label->screen->window->_maxx-1 &&
		label->win->_begy+label->win->_maxy < label->screen->window->_maxy-1)
         {
            moveCDKLabel (label, 1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '5')
      {
         moveCDKLabel (label, CENTER, CENTER, FALSE, TRUE);
      }
      else if (key == 't')
      {
         moveCDKLabel (label, label->win->_begx, TOP, FALSE, TRUE);
      }
      else if (key == 'b')
      {
         moveCDKLabel (label, label->win->_begx, BOTTOM, FALSE, TRUE);
      }
      else if (key == 'l')
      {
         moveCDKLabel (label, LEFT, label->win->_begy, FALSE, TRUE);
      }
      else if (key == 'r')
      {
         moveCDKLabel (label, RIGHT, label->win->_begy, FALSE, TRUE);
      }
      else if (key == 'c')
      {
         moveCDKLabel (label, CENTER, label->win->_begy, FALSE, TRUE);
      }
      else if (key == 'C')
      {
         moveCDKLabel (label, label->win->_begx, CENTER, FALSE, TRUE);
      }
      else if (key == CDK_REFRESH)
      {
         eraseCDKScreen (label->screen);
         refreshCDKScreen (label->screen);
      }
      else if (key == KEY_ESC)
      {
         moveCDKLabel (label, origX, origY, FALSE, TRUE);
      }
      else if ((key != KEY_RETURN) && (key != KEY_ENTER))
      {
         Beep();
      }
   }
}


/*
 * This destroys the label object pointer.
 */
void destroyCDKLabel (CDKLABEL *label)
{
   /* Declare local variables. */
   int x;

   /* Erase the old label. */
   eraseCDKLabel (label);

   /* Free up the character pointers. */
   for (x=0; x < label->rows ; x++)
   {
      freeChtype (label->info[x]);
   }

   /* Free up the window pointers. */
   deleteCursesWindow (label->shadowWin);
   deleteCursesWindow (label->win);
 
   /* Unregister the object. */
   unregisterCDKObject (vLABEL, label);

   /* Free the object pointer. */
   free (label);
}

/*
 * This pauses until a user hits a key...
 */
char waitCDKLabel (CDKLABEL *label, char key)
{
   /* If the key is null, we'll accept anything. */
   if ( key == (char)NULL )
   {
      return (wgetch (label->win));
   }
   else
   {
      /* Only exit when a specific key is hit. */
      char newkey;
      for (;;)
      {
         newkey = wgetch(label->win);
         if (newkey == key)
         {
            return ( newkey );
         }
      }
   }
}
