#include "cdk.h"
#include <limits.h>

/*
 * $Author: tom $
 * $Date: 1999/05/23 02:53:30 $
 * $Revision: 1.18 $
 */

static CDKFUNCS my_funcs = {
    _drawCDKButtonbox,
    _eraseCDKButtonbox,
};

/*
 * This returns a CDK buttonbox widget pointer.
 */
CDKBUTTONBOX *newCDKButtonbox (CDKSCREEN *cdkscreen, int xPos, int yPos, int height, int width, char *title, int rows, int cols, char **buttons, int buttonCount, chtype highlight, boolean Box, boolean shadow)
{
   /* Declare local variables. */
   CDKBUTTONBOX *buttonbox	= newCDKObject(CDKBUTTONBOX, &my_funcs);
   int parentWidth		= getmaxx(cdkscreen->window) - 1;
   int parentHeight		= getmaxy(cdkscreen->window) - 1;
   int boxWidth			= 0;
   int boxHeight		= 0;
   int maxColWidth		= INT_MIN;
   int maxWidth			= INT_MIN;
   int colWidth			= 0;
   int xpos			= xPos;
   int ypos			= yPos;
   int currentButton		= 0;
   chtype *holder		= (chtype *)NULL;
   char *temp[256];
   int x, y, len, junk;

   /* Set some default values for the widget. */
   buttonbox->rowAdjust = 0;
   buttonbox->colAdjust = 0;

  /*
   * If the height is a negative value, the height will
   * be ROWS-height, otherwise, the height will be the
   * given height.
   */
   boxHeight = setWidgetDimension (parentHeight, height, rows + 1);

  /*
   * If the width is a negative value, the width will
   * be COLS-width, otherwise, the width will be the
   * given width.
   */
   boxWidth = setWidgetDimension (parentWidth, width, 0);

   /* Translate the char * items to chtype * */
   if (title != (char *)NULL)
   {
      /* We need to split the title on \n. */
      buttonbox->titleLines = splitString (title, temp, '\n');

      /* We need to determine the widest title line. */
      for (x=0; x < buttonbox->titleLines; x++)
      {
         holder = char2Chtype (temp[x], &len, &junk);
         maxWidth = MAXIMUM (maxWidth, len);
         freeChtype (holder);
      }
      boxWidth = MAXIMUM (boxWidth, maxWidth + 2);

      /* For each line in the title, convert from char * to chtype * */
      for (x=0; x < buttonbox->titleLines; x++)
      {
         buttonbox->title[x]	= char2Chtype (temp[x], &buttonbox->titleLen[x], &buttonbox->titlePos[x]);
         buttonbox->titlePos[x]	= justifyString (boxWidth, buttonbox->titleLen[x], buttonbox->titlePos[x]);
         freeChar (temp[x]);
      }
   }
   else
   {
      /* No title? Set the required variables. */
      buttonbox->titleLines = 0;
   }

   /* Translate the buttons char * to a chtype * */
   for (x = 0; x < buttonCount; x++)
   {
      buttonbox->button[x] = char2Chtype (buttons[x], &buttonbox->buttonLen[x], &junk);
   }

   /* Set the button positions. */
   for (x=0; x < cols; x++)
   {
      maxColWidth = INT_MIN;

      /* Look for the widest item in this column. */
      for (y=0; y < rows; y++)
      {
         if (currentButton < buttonCount)
         {
             maxColWidth = MAXIMUM (buttonbox->buttonLen[currentButton], maxColWidth);
             currentButton++;
         }
      }

      /* Keep the maximum column width for this column. */
      buttonbox->columnWidths[x] = maxColWidth;
      colWidth += maxColWidth;
   }
   boxWidth++;

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);

   /* Now we have to readjust the x and y positions. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Set up the buttonbox box attributes. */
   ScreenOf(buttonbox)			= cdkscreen;
   buttonbox->parent			= cdkscreen->window;
   buttonbox->win			= newwin (boxHeight, boxWidth, ypos, xpos);
   buttonbox->shadowWin			= (WINDOW *)NULL;
   buttonbox->buttonCount		= buttonCount;
   buttonbox->currentButton		= 0;
   buttonbox->rows			= rows;
   buttonbox->cols			= (buttonCount < cols ? buttonCount : cols);
   buttonbox->boxHeight			= boxHeight;
   buttonbox->boxWidth			= boxWidth;
   buttonbox->highlight			= highlight;
   buttonbox->exitType			= vNEVER_ACTIVATED;
   ObjOf(buttonbox)->box		= Box;
   buttonbox->shadow			= shadow;
   buttonbox->ULChar			= ACS_ULCORNER;
   buttonbox->URChar			= ACS_URCORNER;
   buttonbox->LLChar			= ACS_LLCORNER;
   buttonbox->LRChar			= ACS_LRCORNER;
   buttonbox->HChar			= ACS_HLINE;
   buttonbox->VChar			= ACS_VLINE;
   buttonbox->BoxAttrib			= A_NORMAL;
   buttonbox->preProcessFunction	= (PROCESSFN)NULL;
   buttonbox->preProcessData		= (void *)NULL;
   buttonbox->postProcessFunction	= (PROCESSFN)NULL;
   buttonbox->postProcessData		= (void *)NULL;

   /* Set up the row adjustment. */
   if (boxHeight - rows - buttonbox->titleLines > 0)
   {
      buttonbox->rowAdjust = (int)((boxHeight-rows-buttonbox->titleLines) / buttonbox->rows);
   }

   /* Set the col adjustment. */
   if (boxWidth - colWidth > 0)
   {
      buttonbox->colAdjust = (int)((boxWidth-colWidth) / buttonbox->cols)-1;
   }

   /* If we couldn't create the window, we should return a NULL value. */
   if (buttonbox->win == (WINDOW *)NULL)
   {
      /* Couldn't create the window. Clean up used memory. */
      for (x=0; x < buttonbox->buttonCount; x++)
      {
         freeChtype (buttonbox->button[x]);
      }

      /* Remove the memory used by the buttonbox pointer. */
      free (buttonbox);

      /* Return a NULL buttonbox box. */
      return ((CDKBUTTONBOX *)NULL);
   }
   keypad (buttonbox->win, TRUE);

   /* Was there a shadow? */
   if (shadow)
   {
      buttonbox->shadowWin = newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
   }

   /* Empty the key bindings. */
   cleanCDKObjectBindings (vBUTTONBOX, buttonbox);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vBUTTONBOX, buttonbox);

   /* Return the buttonbox box pointer. */
   return (buttonbox);
}

/*
 * This activates the widget.
 */
int activateCDKButtonbox (CDKBUTTONBOX *buttonbox, chtype *actions)
{
   /* Declare local variables. */
   chtype input = (chtype)NULL;
   int ret;

   /* Draw the buttonbox box. */
   drawCDKButtonbox (buttonbox, ObjOf(buttonbox)->box);

   /* Check if actions is NULL. */
   if (actions == (chtype *)NULL)
   {
      for (;;)
      {
         /* Get the input. */
         input = wgetch (buttonbox->win);

         /* Inject the character into the widget. */
         ret = injectCDKButtonbox (buttonbox, input);
         if (buttonbox->exitType != vEARLY_EXIT)
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
         ret = injectCDKButtonbox (buttonbox, actions[x]);
         if (buttonbox->exitType != vEARLY_EXIT)
         {
            return ret;
         }
      }
   }

   /* Set the exit type and exit. */
   buttonbox->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This injects a single character into the widget.
 */
int injectCDKButtonbox (CDKBUTTONBOX *buttonbox, chtype input)
{
   int firstButton	= 0;
   int lastButton	= buttonbox->buttonCount - 1;
   int ppReturn		= 1;

   /* Set the exit type. */
   buttonbox->exitType = vEARLY_EXIT;

   /* Check if there is a pre-process function to be called. */
   if (buttonbox->preProcessFunction != (PROCESSFN)NULL)
   {
      ppReturn = ((PROCESSFN)(buttonbox->preProcessFunction)) (vBUTTONBOX, buttonbox, buttonbox->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a key binding. */
      if (checkCDKObjectBind (vBUTTONBOX, buttonbox, input) != 0)
      {
         buttonbox->exitType = vESCAPE_HIT;
         return -1;
      }
      else
      {
         switch (input)
         {
            case KEY_LEFT : case CDK_PREV :
                 if ((buttonbox->currentButton-buttonbox->rows) < firstButton)
                 {
                    buttonbox->currentButton = lastButton;
                 }
                 else
                 {
                    buttonbox->currentButton -= buttonbox->rows;
                 }
                 break;

            case KEY_RIGHT : case CDK_NEXT : case KEY_TAB : case ' ' :
                 if ((buttonbox->currentButton + buttonbox->rows) > lastButton)
                 {
                    buttonbox->currentButton = firstButton;
                 }
                 else
                 {
                    buttonbox->currentButton += buttonbox->rows;
                 }
                 break;

            case KEY_UP :
                 if ((buttonbox->currentButton-1) < firstButton)
                 {
                    buttonbox->currentButton = lastButton;
                 }
                 else
                 {
                    buttonbox->currentButton--;
                 }
                 break;

            case KEY_DOWN :
                 if ((buttonbox->currentButton + 1) > lastButton)
                 {
                    buttonbox->currentButton = firstButton;
                 }
                 else
                 {
                    buttonbox->currentButton++;
                 }
                 break;

            case CDK_REFRESH :
                 eraseCDKScreen (ScreenOf(buttonbox));
                 refreshCDKScreen (ScreenOf(buttonbox));
                 break;

            case KEY_ESC :
                 buttonbox->exitType = vESCAPE_HIT;
                 return -1;

            case KEY_RETURN : case KEY_ENTER :
                 buttonbox->exitType = vNORMAL;
                 return buttonbox->currentButton;

            default :
                 break;
         }
      }

      /* Should we call a post-process? */
      if (buttonbox->postProcessFunction != (PROCESSFN)NULL)
      {
         ((PROCESSFN)(buttonbox->postProcessFunction)) (vBUTTONBOX, buttonbox, buttonbox->postProcessData, input);
      }
   }

   /* Redraw the buttons. */
   drawCDKButtonboxButtons (buttonbox);
   wrefresh (buttonbox->win);

   /* Exit the buttonbox box. */
   buttonbox->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This sets multiple attributes of the widget.
 */
void setCDKButtonbox (CDKBUTTONBOX *buttonbox, chtype highlight, boolean Box)
{
   setCDKButtonboxHighlight (buttonbox, highlight);
   setCDKButtonboxBox (buttonbox, Box);
}

/*
 * This sets the highlight attribute for the buttonboxs.
 */
void setCDKButtonboxHighlight (CDKBUTTONBOX *buttonbox, chtype highlight)
{
   buttonbox->highlight = highlight;
}
chtype getCDKButtonboxHighlight (CDKBUTTONBOX *buttonbox)
{
   return (chtype)buttonbox->highlight;
}

/*
 * This sets the box attribute of the widget.
 */
void setCDKButtonboxBox (CDKBUTTONBOX *buttonbox, boolean Box)
{
   ObjOf(buttonbox)->box = Box;
}
boolean getCDKButtonboxBox (CDKBUTTONBOX *buttonbox)
{
   return ObjOf(buttonbox)->box;
}

/*
 * This sets the background color of the widget.
 */
void setCDKButtonboxBackgroundColor (CDKBUTTONBOX *buttonbox, char *color)
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
   wbkgd (buttonbox->win, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This draws the buttonbox box widget.
 */
void _drawCDKButtonbox (CDKOBJS *object, boolean Box)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)object;
   int x = 0;

   /* Is there a shadow? */
   if (buttonbox->shadowWin != (WINDOW *)NULL)
   {
      drawShadow (buttonbox->shadowWin);
   }

   /* Box the widget if they asked. */
   if (Box)
   {
      attrbox (buttonbox->win,
		buttonbox->ULChar, buttonbox->URChar,
		buttonbox->LLChar, buttonbox->LRChar,
		buttonbox->HChar,  buttonbox->VChar,
		buttonbox->BoxAttrib);
      wrefresh (buttonbox->win);
   }

   /* Draw in the title if there is one. */
   if (buttonbox->titleLines != 0)
   {
      for (x=0; x < buttonbox->titleLines; x++)
      {
         writeChtype (buttonbox->win,
			buttonbox->titlePos[x],
			x + 1,
			buttonbox->title[x],
			HORIZONTAL, 0,
			buttonbox->titleLen[x]);
      }
   }

   /* Draw in the buttons. */
   drawCDKButtonboxButtons (buttonbox);
}

/*
 * This draws the buttons on the button box widget.
 */
void drawCDKButtonboxButtons (CDKBUTTONBOX *buttonbox)
{
   int row		= buttonbox->titleLines + 1;
   int col		= (int)(buttonbox->colAdjust / 2);
   int currentButton	= 0;
   int x, y;

   /* Draw the buttons. */
   while (currentButton < buttonbox->buttonCount)
   {
      for (x=0; x < buttonbox->cols; x++)
      {
         row = buttonbox->titleLines + 1;

         for (y=0; y < buttonbox->rows; y++)
         {
            if (currentButton == buttonbox->currentButton)
            {
               writeChtypeAttrib (buttonbox->win,
                       			col, row,
                       			buttonbox->button[currentButton],
                       			buttonbox->highlight,
                       			HORIZONTAL, 0,
                       			buttonbox->buttonLen[currentButton]);
            }
            else
            {
               writeChtype (buttonbox->win,
				col, row,
				buttonbox->button[currentButton],
				HORIZONTAL, 0,
				buttonbox->buttonLen[currentButton]);
            }
            row += (1 + buttonbox->rowAdjust);
            currentButton++;
         }
         col += buttonbox->columnWidths[x] + buttonbox->colAdjust + 1;
      }
   }
   touchwin (buttonbox->win);
   wrefresh (buttonbox->win);
}

/*
 * This erases the buttonbox box from the screen.
 */
void _eraseCDKButtonbox (CDKOBJS *object)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)object;
   eraseCursesWindow (buttonbox->win);
   eraseCursesWindow (buttonbox->shadowWin);
}

/*
 * This moves the buttonbox box to a new screen location.
 */
void moveCDKButtonbox (CDKBUTTONBOX *buttonbox, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   /* Declare local variables. */
   int currentX = getbegx(buttonbox->win);
   int currentY = getbegy(buttonbox->win);
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
      xpos = getbegx(buttonbox->win) + xplace;
      ypos = getbegy(buttonbox->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(buttonbox), &xpos, &ypos, buttonbox->boxWidth, buttonbox->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(buttonbox->win, -xdiff, -ydiff);

   /* If there is a shadow box we have to move it too. */
   if (buttonbox->shadowWin != (WINDOW *)NULL)
   {
      moveCursesWindow(buttonbox->shadowWin, -xdiff, -ydiff);
   }

   /* Touch the windows so they 'move'. */
   touchwin (WindowOf(buttonbox));
   wrefresh (WindowOf(buttonbox));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKButtonbox (buttonbox, ObjOf(buttonbox)->box);
   }
}

/*
 * This allows the user to position the widget on the screen interactively.
 */
void positionCDKButtonbox (CDKBUTTONBOX *buttonbox)
{
   /* Declare some variables. */
   int origX	= getbegx(buttonbox->win);
   int origY	= getbegy(buttonbox->win);
   chtype key	= (chtype)NULL;

   /* Let them move the widget around until they hit return. */
   while ((key != KEY_RETURN) && (key != KEY_ENTER))
   {
      key = wgetch (buttonbox->win);
      if (key == KEY_UP || key == '8')
      {
         if (getbegy(buttonbox->win) > 0)
         {
            moveCDKButtonbox (buttonbox, 0, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_DOWN || key == '2')
      {
         if (getendy(buttonbox->win) < getmaxy(WindowOf(buttonbox))-1)
         {
            moveCDKButtonbox (buttonbox, 0, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_LEFT || key == '4')
      {
         if (getbegx(buttonbox->win) > 0)
         {
            moveCDKButtonbox (buttonbox, -1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_RIGHT || key == '6')
      {
         if (getendx(buttonbox->win) < getmaxx(WindowOf(buttonbox))-1)
         {
            moveCDKButtonbox (buttonbox, 1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '7')
      {
         if (getbegy(buttonbox->win) > 0 && getbegx(buttonbox->win) > 0)
         {
            moveCDKButtonbox (buttonbox, -1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '9')
      {
         if (getendx(buttonbox->win) < getmaxx(WindowOf(buttonbox))-1
	  && getbegy(buttonbox->win) > 0)
         {
            moveCDKButtonbox (buttonbox, 1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '1')
      {
         if (getbegx(buttonbox->win) > 0 && getendx(buttonbox->win) < getmaxx(WindowOf(buttonbox))-1)
         {
            moveCDKButtonbox (buttonbox, -1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '3')
      {
         if (getendx(buttonbox->win) < getmaxx(WindowOf(buttonbox))-1
	  && getendy(buttonbox->win) < getmaxy(WindowOf(buttonbox))-1)
         {
            moveCDKButtonbox (buttonbox, 1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '5')
      {
         moveCDKButtonbox (buttonbox, CENTER, CENTER, FALSE, TRUE);
      }
      else if (key == 't')
      {
         moveCDKButtonbox (buttonbox, getbegx(buttonbox->win), TOP, FALSE, TRUE);
      }
      else if (key == 'b')
      {
         moveCDKButtonbox (buttonbox, getbegx(buttonbox->win), BOTTOM, FALSE, TRUE);
      }
      else if (key == 'l')
      {
         moveCDKButtonbox (buttonbox, LEFT, getbegy(buttonbox->win), FALSE, TRUE);
      }
      else if (key == 'r')
      {
         moveCDKButtonbox (buttonbox, RIGHT, getbegy(buttonbox->win), FALSE, TRUE);
      }
      else if (key == 'c')
      {
         moveCDKButtonbox (buttonbox, CENTER, getbegy(buttonbox->win), FALSE, TRUE);
      }
      else if (key == 'C')
      {
         moveCDKButtonbox (buttonbox, getbegx(buttonbox->win), CENTER, FALSE, TRUE);
      }
      else if (key == CDK_REFRESH)
      {
         eraseCDKScreen (ScreenOf(buttonbox));
         refreshCDKScreen (ScreenOf(buttonbox));
      }
      else if (key == KEY_ESC)
      {
         moveCDKButtonbox (buttonbox, origX, origY, FALSE, TRUE);
      }
      else if ((key != KEY_RETURN) && (key != KEY_ENTER))
      {
         Beep();
      }
   }
}

/*
 * This destroys the widget and all the memory associated with it.
 */
void destroyCDKButtonbox (CDKBUTTONBOX *buttonbox)
{
   int x;

   /* Erase the widget. */
   eraseCDKButtonbox (buttonbox);

   /* Free up some memory. */
   for (x=0; x < buttonbox->titleLines; x++)
   {
      freeChtype (buttonbox->title[x]);
   }
   for (x=0; x < buttonbox->buttonCount; x++)
   {
      freeChtype (buttonbox->button[x]);
   }

   /* Delete the windows. */
   deleteCursesWindow (buttonbox->shadowWin);
   deleteCursesWindow (buttonbox->win);

   /* Unregister this object. */
   unregisterCDKObject (vBUTTONBOX, buttonbox);

   /* Finish cleaning up. */
   free (buttonbox);
}

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKButtonboxULChar (CDKBUTTONBOX *buttonbox, chtype character)
{
   buttonbox->ULChar = character;
}
void setCDKButtonboxURChar (CDKBUTTONBOX *buttonbox, chtype character)
{
   buttonbox->URChar = character;
}
void setCDKButtonboxLLChar (CDKBUTTONBOX *buttonbox, chtype character)
{
   buttonbox->LLChar = character;
}
void setCDKButtonboxLRChar (CDKBUTTONBOX *buttonbox, chtype character)
{
   buttonbox->LRChar = character;
}
void setCDKButtonboxVerticalChar (CDKBUTTONBOX *buttonbox, chtype character)
{
   buttonbox->VChar = character;
}
void setCDKButtonboxHorizontalChar (CDKBUTTONBOX *buttonbox, chtype character)
{
   buttonbox->HChar = character;
}
void setCDKButtonboxBoxAttribute (CDKBUTTONBOX *buttonbox, chtype character)
{
   buttonbox->BoxAttrib = character;
}

/*
 * These set the pre/post process functions of the buttonbox widget.
 */
void setCDKButtonboxPreProcess (CDKBUTTONBOX *buttonbox, PROCESSFN callback, void *data)
{
   buttonbox->preProcessFunction = callback;
   buttonbox->preProcessData = data;
}
void setCDKButtonboxPostProcess (CDKBUTTONBOX *buttonbox, PROCESSFN callback, void *data)
{
   buttonbox->postProcessFunction = callback;
   buttonbox->postProcessData = data;
}

/*
 *
 */
void setCDKButtonboxCurrentButton (CDKBUTTONBOX *buttonbox, int button)
{
   if ((button >= 0) && (button < buttonbox->buttonCount))
   {
      buttonbox->currentButton = button;
   }
}
int getCDKButtonboxCurrentButton (CDKBUTTONBOX *buttonbox)
{
   return buttonbox->currentButton;
}
int getCDKButtonboxButtonCount (CDKBUTTONBOX *buttonbox)
{
   return buttonbox->buttonCount;
}
