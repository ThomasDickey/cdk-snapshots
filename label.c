#include "cdk.h"
#include <limits.h>

/*
 * $Author: tom $
 * $Date: 2000/01/16 23:07:27 $
 * $Revision: 1.59 $
 */

DeclareCDKObjects(my_funcs,Label);

/*
 * This creates a label widget.
 */
CDKLABEL *newCDKLabel(CDKSCREEN *cdkscreen, int xplace, int yplace, char **mesg, int rows, boolean Box, boolean shadow)
{
   /* Maintain the label information. */
   CDKLABEL *label	= newCDKObject(CDKLABEL, &my_funcs);
   int parentWidth	= getmaxx(cdkscreen->window) - 1;
   int parentHeight	= getmaxy(cdkscreen->window) - 1;
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
      label->infoPos[x] = justifyString (boxWidth, label->infoLen[x], label->infoPos[x]);
   }

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Create the label. */
   ScreenOf(label)	= cdkscreen;
   label->parent	= cdkscreen->window;
   label->win		= newwin (boxHeight, boxWidth, ypos, xpos);
   label->shadowWin	= 0;
   label->xpos		= xpos;
   label->ypos		= ypos;
   label->rows		= rows;
   label->boxWidth	= boxWidth;
   label->boxHeight	= boxHeight;
   ObjOf(label)->box	= Box;
   label->shadow	= shadow;
   label->ULChar	= ACS_ULCORNER;
   label->URChar	= ACS_URCORNER;
   label->LLChar	= ACS_LLCORNER;
   label->LRChar	= ACS_LRCORNER;
   label->HChar		= ACS_HLINE;
   label->VChar		= ACS_VLINE;
   label->BoxAttrib	= A_NORMAL;

   /* Is the window null? */
   if (label->win == 0)
   {
      /* Free up any memory used. */
      for (x=0; x < rows; x++)
      {
	 freeChtype (label->info[x]);
      }
      free(label);

      /* Return a null pointer. */
      return ( 0 );
   }
   keypad (label->win, TRUE);

   /* If a shadow was requested, then create the shadow window. */
   if (shadow)
   {
      label->shadowWin	= newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
   }

   /* Register this baby. */
   registerCDKObject (cdkscreen, vLABEL, label);

   /* Return the label pointer. */
   return (label);
}

/*
 * This was added for the builder.
 */
void activateCDKLabel (CDKLABEL *label, chtype *actions GCC_UNUSED)
{
   drawCDKLabel (label, ObjOf(label)->box);
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
      label->infoPos[x] = justifyString (label->boxWidth, label->infoLen[x], label->infoPos[x]);
   }

   /* Redraw the label widget. */
   eraseCDKLabel (label);
   drawCDKLabel (label, ObjOf(label)->box);
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
   ObjOf(label)->box = Box;
}
boolean getCDKLabelBox (CDKLABEL *label)
{
   return ObjOf(label)->box;
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
   wbkgd (label->win, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This draws the label widget.
 */
static void _drawCDKLabel (CDKOBJS *object, boolean Box GCC_UNUSED)
{
   CDKLABEL *label = (CDKLABEL *)object;
   int x = 0;

   /* Is there a shadow? */
   if (label->shadowWin != 0)
   {
      drawShadow (label->shadowWin);
   }

   /* Box the widget if asked. */
   if (ObjOf(label)->box)
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
      writeChtype (label->win, label->infoPos[x], x + 1, label->info[x], HORIZONTAL, 0, label->infoLen[x]);
   }

   /* Refresh the window. */
   touchwin (label->win);
   wrefresh (label->win);
}

/*
 * This erases the label widget.
 */
static void _eraseCDKLabel (CDKOBJS *object)
{
   CDKLABEL *label = (CDKLABEL *)object;

   eraseCursesWindow (label->win);
   eraseCursesWindow (label->shadowWin);
}

/*
 * This moves the label field to the given location.
 */
static void _moveCDKLabel (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKLABEL *label = (CDKLABEL *)object;
   int currentX = getbegx(label->win);
   int currentY = getbegy(label->win);
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
      xpos = getbegx(label->win) + xplace;
      ypos = getbegy(label->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(label), &xpos, &ypos, label->boxWidth, label->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(label->win, -xdiff, -ydiff);

   /* If there is a shadow box we have to move it too. */
   if (label->shadowWin != 0)
   {
      moveCursesWindow(label->shadowWin, -xdiff, -ydiff);
   }

   /* Touch the windows so they 'move'. */
   touchwin (WindowOf(label));
   wrefresh (WindowOf(label));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKLabel (label, ObjOf(label)->box);
   }
}

/*
 * This destroys the label object pointer.
 */
void destroyCDKLabel (CDKLABEL *label)
{
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
   if ( key == 0 )
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
