#include <cdk_int.h>
#include "button.h"
#include <limits.h>

/*
 * $Author: tom $
 * $Date: 2003/11/30 21:15:51 $
 * $Revision: 1.18 $
 */

DeclareCDKObjects (BUTTON, Button, setCdk, Int);

/*
 * This creates a button widget.
 */
CDKBUTTON *newCDKButton (CDKSCREEN * cdkscreen, int xplace, int yplace, char
			 *text, tButtonCallback callback, boolean Box,
			 boolean shadow)
{
   /* Maintain the button information. */
   CDKBUTTON *button	= 0;
   int parentWidth      = getmaxx (cdkscreen->window);
   int parentHeight     = getmaxy (cdkscreen->window);
   int boxWidth         = 0;
   int boxHeight;
   int xpos = xplace;
   int ypos = yplace;

   if ((button = newCDKObject(CDKBUTTON, &my_funcs)) == 0)
      return (0);

   setCDKButtonBox (button, Box);
   boxHeight = 1 + 2 * BorderOf(button);

   /* Translate the char * to a chtype. */
   button->info = char2Chtype (text, &button->infoLen, &button->infoPos);
   boxWidth = MAXIMUM (boxWidth, button->infoLen) + 2 * BorderOf(button);

   /* Create the string alignments. */
   button->infoPos = justifyString (boxWidth - 2 * BorderOf(button),
				    button->infoLen, button->infoPos);

   /*
    * Make sure we didn't extend beyond the dimensions of the window.
    */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Create the button. */
   ScreenOf (button)            = cdkscreen;
   ObjOf (button)->fn           = &my_funcs;
   button->parent               = cdkscreen->window;
   button->win                  = newwin (boxHeight, boxWidth, ypos, xpos);
   button->shadowWin            = (WINDOW *) NULL;
   button->xpos                 = xpos;
   button->ypos                 = ypos;
   button->boxWidth             = boxWidth;
   button->boxHeight            = boxHeight;
   button->callback             = callback;
   ObjOf (button)->inputWindow  = button->win;
   ObjOf (button)->acceptsFocus = TRUE;
   button->exitType		= vNEVER_ACTIVATED;
   button->shadow               = shadow;

   /* Is the window NULL? */
   if (button->win == (WINDOW *) NULL)
   {
      _destroyCDKButton (ObjOf(button));
      return (0);
   }

   keypad (button->win, TRUE);

   /* If a shadow was requested, then create the shadow window. */
   if (shadow)
      button->shadowWin = newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vBUTTON, button);

   /* Return the button pointer. */
   return (button);
}

/*
 * This was added for the builder.
 */
int activateCDKButton (CDKBUTTON * button, chtype * actions)
{
   chtype input = 0;
   int ret;

   drawCDKButton (button, ObjOf (button)->box);

   /* Check if actions is null. */
   if (actions == 0)
   {
      for (;;)
      {
	 /* Get the input. */
	 input = getcCDKObject (ObjOf(button));

	 /* Inject the character into the widget. */
	 ret = injectCDKButton(button, input);
	 if (button->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }
   else
   {
      int length = chlen (actions);
      int x = 0;

      /* Inject each character one at a time. */
      for (x=0; x < length; x++)
      {
	 ret = injectCDKButton(button, actions[x]);
	 if (button->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }

   /* Set the exit type and exit. */
   button->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This sets multiple attributes of the widget.
 */
void setCDKButton (CDKBUTTON * button, char *mesg, boolean Box)
{
   setCDKButtonMessage (button, mesg);
   setCDKButtonBox (button, Box);
}

/*
 * This sets the information within the button.
 */
void setCDKButtonMessage (CDKBUTTON * button, char *info)
{
   /* Clean out the old message. */
   freeChtype (button->info);
   button->infoPos = 0;
   button->infoLen = 0;

   /* Copy in the new message. */

   button->info = char2Chtype (info, &button->infoLen, &button->infoPos);
   button->infoPos = justifyString (button->boxWidth - 2 * BorderOf(button),
				    button->infoLen, button->infoPos);

   /* Redraw the button widget. */
   eraseCDKButton (button);
   drawCDKButton (button, ObjOf (button)->box);
}

chtype *getCDKButtonMessage (CDKBUTTON * button)
{
   return button->info;
}

/*
 * This sets the box flag for the button widget.
 */
void setCDKButtonBox (CDKBUTTON * button, boolean Box)
{
   ObjOf (button)->box = Box;
   ObjOf (button)->borderSize = Box ? 1 : 0;
}

boolean getCDKButtonBox (CDKBUTTON * button)
{
   return ObjOf (button)->box;
}

/*
 * This sets the background color of the widget.
 */
void setCDKButtonBackgroundColor (CDKBUTTON * button, char *color)
{
   chtype *holder = (chtype *) NULL;
   int junk1, junk2;

   /* Make sure the color isn't NULL. */
   if (color == (char *)NULL)
   {
      return;
   }

   /* Convert the value of the environment variable to a chtype. */
   holder = char2Chtype (color, &junk1, &junk2);

   /* Set the widgets background color. */
   setCDKButtonBackgroundAttrib (button, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This sets the background attribute of the widget.
 */
void setCDKButtonBackgroundAttrib (CDKBUTTON * button, chtype attrib)
{
   /* Set the widgets background attribute. */
   wbkgd (button->win, attrib);
}

static void drawCDKButtonText (CDKBUTTON * button)
{
   int boxWidth = button->boxWidth;
   int i;

   /* Draw in the message. */

   for (i = 0; i < boxWidth - 2 * BorderOf(button); i++)
   {
      chtype c;
      int pos = button->infoPos;
      int len = button->infoLen;

      if (i >= pos && (i - pos) < len)
	 c = button->info[i - pos];
      else
	 c = ' ';

      if (HasFocusObj (button))
      {
	 c &= A_CHARTEXT;
	 c |= A_REVERSE;
      }

      mvwaddch (button->win, BorderOf(button), i + BorderOf(button), c);
   }
}

/*
 * This draws the button widget.
 */
static void _drawCDKButton (CDKOBJS * object, boolean Box GCC_UNUSED)
{
   CDKBUTTON *button = (CDKBUTTON *) object;

   /* Is there a shadow? */
   if (button->shadowWin != (WINDOW *) NULL)
   {
      drawShadow (button->shadowWin);
   }

   /* Box the widget if asked. */
   if (ObjOf (button)->box)
   {
      drawObjBox (button->win, ObjOf(button));
   }
   drawCDKButtonText (button);
   touchwin (button->win);
   wrefresh (button->win);
}

/*
 * This erases the button widget.
 */
static void _eraseCDKButton (CDKOBJS * object)
{
   if (validCDKObject (object))
   {
      CDKBUTTON *button = (CDKBUTTON *) object;

      eraseCursesWindow (button->win);
      eraseCursesWindow (button->shadowWin);
   }
}

/*
 * This moves the button field to the given location.
 */
static void _moveCDKButton (CDKOBJS *object,
			    int xplace,
			    int yplace,
			    boolean relative,
			    boolean refresh_flag)
{
   CDKBUTTON *button = (CDKBUTTON *) object;
   int currentX = getbegx (button->win);
   int currentY = getbegy (button->win);
   int xpos = xplace;
   int ypos = yplace;
   int xdiff = 0;
   int ydiff = 0;

   /*
    * If this is a relative move, then we will adjust where we want
    * to move to.
    */
   if (relative)
   {
      xpos = getbegx (button->win) + xplace;
      ypos = getbegy (button->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf (button), &xpos, &ypos, button->boxWidth,
	    button->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(button->win, -xdiff, -ydiff);
   moveCursesWindow(button->shadowWin, -xdiff, -ydiff);

   /* Touch the windows so they 'move'. */
   touchwin (WindowOf (button));
   wrefresh (WindowOf (button));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKButton (button, ObjOf (button)->box);
   }
}

/*
 * This allows the user to use the cursor keys to adjust the
 * position of the widget.
 */
void positionCDKButton (CDKBUTTON * button)
{
   /* Declare some variables. */
   int origX = getbegx (button->win);
   int origY = getbegy (button->win);
   chtype key = (chtype) NULL;

   /* Let them move the widget around until they hit return. */
   while ((key != KEY_RETURN) && (key != KEY_ENTER))
   {
      key = getcCDKObject (ObjOf(button));
      if (key == KEY_UP || key == '8')
      {
	 if (getbegy (button->win) > 0)
	 {
	    moveCDKButton (button, 0, -1, TRUE, TRUE);
	 }
	 else
	 {
	    Beep ();
	 }
      }
      else if (key == KEY_DOWN || key == '2')
      {
	 if (getbegy (button->win) + getmaxy (button->win) <
	     getmaxy (WindowOf (button)) - 1)
	 {
	    moveCDKButton (button, 0, 1, TRUE, TRUE);
	 }
	 else
	 {
	    Beep ();
	 }
      }
      else if (key == KEY_LEFT || key == '4')
      {
	 if (getbegx (button->win) > 0)
	 {
	    moveCDKButton (button, -1, 0, TRUE, TRUE);
	 }
	 else
	 {
	    Beep ();
	 }
      }
      else if (key == KEY_RIGHT || key == '6')
      {
	 if (getbegx (button->win) + getmaxx (button->win) < getmaxx
	     (WindowOf (button)) - 1)
	 {
	    moveCDKButton (button, 1, 0, TRUE, TRUE);
	 }
	 else
	 {
	    Beep ();
	 }
      }
      else if (key == '7')
      {
	 if (getbegy (button->win) > 0 && getbegx (button->win) > 0)
	 {
	    moveCDKButton (button, -1, -1, TRUE, TRUE);
	 }
	 else
	 {
	    Beep ();
	 }
      }
      else if (key == '9')
      {
	 if (getbegx (button->win) + getmaxx (button->win) < getmaxx
	     (WindowOf (button)) - 1 &&
	     getbegy (button->win) > 0)
	 {
	    moveCDKButton (button, 1, -1, TRUE, TRUE);
	 }
	 else
	 {
	    Beep ();
	 }
      }
      else if (key == '1')
      {
	 if (getbegx (button->win) > 0 && getbegx (button->win) +
	     getmaxx (button->win) < getmaxx (WindowOf (button)) - 1)
	 {
	    moveCDKButton (button, -1, 1, TRUE, TRUE);
	 }
	 else
	 {
	    Beep ();
	 }
      }
      else if (key == '3')
      {
	 if (getbegx (button->win) + getmaxx (button->win) <
	     getmaxx (WindowOf (button)) - 1
	     && getbegy (button->win) + getmaxy (button->win) <
	     getmaxy (WindowOf (button)) - 1)
	 {
	    moveCDKButton (button, 1, 1, TRUE, TRUE);
	 }
	 else
	 {
	    Beep ();
	 }
      }
      else if (key == '5')
      {
	 moveCDKButton (button, CENTER, CENTER, FALSE, TRUE);
      }
      else if (key == 't')
      {
	 moveCDKButton (button, getbegx (button->win), TOP, FALSE, TRUE);
      }
      else if (key == 'b')
      {
	 moveCDKButton (button, getbegx (button->win), BOTTOM, FALSE, TRUE);
      }
      else if (key == 'l')
      {
	 moveCDKButton (button, LEFT, getbegy (button->win), FALSE, TRUE);
      }
      else if (key == 'r')
      {
	 moveCDKButton (button, RIGHT, getbegy (button->win), FALSE, TRUE);
      }
      else if (key == 'c')
      {
	 moveCDKButton (button, CENTER, getbegy (button->win), FALSE, TRUE);
      }
      else if (key == 'C')
      {
	 moveCDKButton (button, getbegx (button->win), CENTER, FALSE, TRUE);
      }
      else if (key == CDK_REFRESH)
      {
	 eraseCDKScreen (ScreenOf (button));
	 refreshCDKScreen (ScreenOf (button));
      }
      else if (key == KEY_ESC)
      {
	 moveCDKButton (button, origX, origY, FALSE, TRUE);
      }
      else if ((key != KEY_RETURN) && (key != KEY_ENTER))
      {
	 Beep ();
      }
   }
}

/*
 * This destroys the button object pointer.
 */
static void _destroyCDKButton (CDKOBJS * object)
{
   CDKBUTTON *button = (CDKBUTTON *) object;

   /* Free up the character pointers. */
   freeChtype (button->info);

   /* Free up the window pointers. */
   deleteCursesWindow (button->shadowWin);
   deleteCursesWindow (button->win);

   /* Unregister the object. */
   unregisterCDKObject (vBUTTON, button);
}

/*
 * This injects a single character into the widget.
 */
static int _injectCDKButton (CDKOBJS * object, chtype input)
{
   CDKBUTTON *button = (CDKBUTTON *) object;
   int ret = unknownInt;
   bool complete = FALSE;

   button->exitType = vEARLY_EXIT;

   /* Check a predefined binding. */
   if (checkCDKObjectBind (vBUTTON, button, input) != 0)
   {
	 button->exitType = vESCAPE_HIT;
	 complete = TRUE;
   }
   else
   {
      switch (input)
      {
      case KEY_ESC :
	 button->exitType = vESCAPE_HIT;
	 complete = TRUE;
	 break;

      case KEY_RETURN:
      case ' ':
      case KEY_ENTER:
	 if (button->callback)
	    button->callback (button);
	 button->exitType = vNORMAL;
	 ret = 0;
	 complete = TRUE;
	 break;

      case CDK_REFRESH:
	 eraseCDKScreen (ScreenOf (button));
	 refreshCDKScreen (ScreenOf (button));
	 break;

      default:
	 Beep ();
	 break;
      }
   }

   if (!complete) {
      button->exitType = vEARLY_EXIT;
   }

   ResultOf(button).valueInt = ret;
   return (ret != unknownInt);
}

static void _focusCDKButton (CDKOBJS * object)
{
   CDKBUTTON *button = (CDKBUTTON *) object;

   curs_set (0);
   HasFocusObj (button) = TRUE;
   drawCDKButtonText (button);
   wrefresh (button->win);
}

static void _unfocusCDKButton (CDKOBJS * object)
{
   CDKBUTTON *button = (CDKBUTTON *) object;

   HasFocusObj (button) = FALSE;
   drawCDKButtonText (button);
   wrefresh (button->win);
}

static void _refreshDataCDKButton(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _saveDataCDKButton(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}
