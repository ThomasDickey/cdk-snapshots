#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2003/11/16 21:45:45 $
 * $Revision: 1.43 $
 */

DeclareCDKObjects(BUTTONBOX, Buttonbox, setCdk, Int);

/*
 * This returns a CDK buttonbox widget pointer.
 */
CDKBUTTONBOX *newCDKButtonbox (CDKSCREEN *cdkscreen, int xPos, int yPos, int height, int width, char *title, int rows, int cols, char **buttons, int buttonCount, chtype highlight, boolean Box, boolean shadow)
{
   CDKBUTTONBOX *buttonbox	= 0;
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
   chtype *holder		= 0;
   char **temp			= 0;
   int x, y, len, junk;

   if ((buttonbox = newCDKObject(CDKBUTTONBOX, &my_funcs)) == 0)
      return (0);

   setCDKButtonboxBox (buttonbox, Box);

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
   if (title != 0)
   {
      int titleWidth;

      /* We need to split the title on \n. */
      temp = CDKsplitString (title, '\n');
      buttonbox->titleLines = CDKcountStrings (temp);

      /* We need to determine the widest title line. */
      for (x=0; x < buttonbox->titleLines; x++)
      {
	 holder = char2Chtype (temp[x], &len, &junk);
	 maxWidth = MAXIMUM (maxWidth, len);
	 freeChtype (holder);
      }
      boxWidth = MAXIMUM (boxWidth, maxWidth + 2 * BorderOf(buttonbox));

      /* For each line in the title, convert from char * to chtype * */
      titleWidth = boxWidth - (2 * BorderOf(buttonbox));
      for (x=0; x < buttonbox->titleLines; x++)
      {
	 buttonbox->title[x]	= char2Chtype (temp[x], &buttonbox->titleLen[x], &buttonbox->titlePos[x]);
	 buttonbox->titlePos[x] = justifyString (titleWidth, buttonbox->titleLen[x], buttonbox->titlePos[x]);
      }
      CDKfreeStrings(temp);
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
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight, BorderOf(buttonbox));

   /* Set up the buttonbox box attributes. */
   ScreenOf(buttonbox)			= cdkscreen;
   buttonbox->parent			= cdkscreen->window;
   buttonbox->win			= newwin (boxHeight, boxWidth, ypos, xpos);
   buttonbox->shadowWin			= 0;
   buttonbox->buttonCount		= buttonCount;
   buttonbox->currentButton		= 0;
   buttonbox->rows			= rows;
   buttonbox->cols			= (buttonCount < cols ? buttonCount : cols);
   buttonbox->boxHeight			= boxHeight;
   buttonbox->boxWidth			= boxWidth;
   buttonbox->highlight			= highlight;
   buttonbox->exitType			= vNEVER_ACTIVATED;
   ObjOf(buttonbox)->inputWindow	= buttonbox->win;
   buttonbox->shadow			= shadow;
   buttonbox->ButtonAttrib		= A_NORMAL;
   buttonbox->preProcessFunction	= 0;
   buttonbox->preProcessData		= 0;
   buttonbox->postProcessFunction	= 0;
   buttonbox->postProcessData		= 0;

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

   /* If we couldn't create the window, we should return a null value. */
   if (buttonbox->win == 0)
   {
      _destroyCDKButtonbox (ObjOf(buttonbox));
      return (0);
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
   chtype input = 0;
   int ret;

   /* Draw the buttonbox box. */
   drawCDKButtonbox (buttonbox, ObjOf(buttonbox)->box);

   /* Check if actions is null. */
   if (actions == 0)
   {
      for (;;)
      {
	 /* Get the input. */
	 input = getcCDKObject (ObjOf(buttonbox));

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
static int _injectCDKButtonbox (CDKOBJS *object, chtype input)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)object;
   int firstButton	= 0;
   int lastButton	= buttonbox->buttonCount - 1;
   int ppReturn		= 1;
   int ret              = unknownInt;
   bool complete        = FALSE;

   /* Set the exit type. */
   buttonbox->exitType = vEARLY_EXIT;

   /* Check if there is a pre-process function to be called. */
   if (buttonbox->preProcessFunction != 0)
   {
      ppReturn = buttonbox->preProcessFunction (vBUTTONBOX, buttonbox, buttonbox->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a key binding. */
      if (checkCDKObjectBind (vBUTTONBOX, buttonbox, input) != 0)
      {
	 buttonbox->exitType = vESCAPE_HIT;
	 complete = TRUE;
      }
      else
      {
	 switch (input)
	 {
	    case KEY_LEFT : case CDK_PREV : case KEY_BTAB : case '\b' :
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
		 complete = TRUE;
		 break;

	    case KEY_RETURN : case KEY_ENTER :
		 buttonbox->exitType = vNORMAL;
		 ret = buttonbox->currentButton;
		 complete = TRUE;
		 break;

	    default :
		 break;
	 }
      }

      /* Should we call a post-process? */
      if (!complete && (buttonbox->postProcessFunction != 0))
      {
	 buttonbox->postProcessFunction (vBUTTONBOX, buttonbox, buttonbox->postProcessData, input);
      }
   }

   if (!complete) {
      drawCDKButtonboxButtons (buttonbox);
      buttonbox->exitType = vEARLY_EXIT;
   }

   ResultOf(buttonbox).valueInt = ret;
   return (ret != unknownInt);
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
   ObjOf(buttonbox)->borderSize = Box ? 1 : 0;
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
   setCDKButtonboxBackgroundAttrib (buttonbox, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This sets the background attribute of the widget.
 */
void setCDKButtonboxBackgroundAttrib (CDKBUTTONBOX *buttonbox, chtype attrib)
{
   /* Set the widgets background attribute. */
   wbkgd (buttonbox->win, attrib);
}

/*
 * This draws the buttonbox box widget.
 */
static void _drawCDKButtonbox (CDKOBJS *object, boolean Box)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)object;
   int x = 0;

   /* Is there a shadow? */
   if (buttonbox->shadowWin != 0)
   {
      drawShadow (buttonbox->shadowWin);
   }

   /* Box the widget if they asked. */
   if (Box)
   {
      drawObjBox (buttonbox->win, ObjOf(buttonbox));
   }

   /* Draw in the title if there is one. */
   if (buttonbox->titleLines != 0)
   {
      for (x=0; x < buttonbox->titleLines; x++)
      {
	 writeChtype (buttonbox->win,
			buttonbox->titlePos[x] + BorderOf(buttonbox),
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
   int cur_row		= -1;
   int cur_col		= -1;

   /* Draw the buttons. */
   while (currentButton < buttonbox->buttonCount)
   {
      for (x=0; x < buttonbox->cols; x++)
      {
         row = buttonbox->titleLines + BorderOf(buttonbox);

	 for (y=0; y < buttonbox->rows; y++)
	 {
	    chtype attr = buttonbox->ButtonAttrib;
	    if (currentButton == buttonbox->currentButton)
	    {
	       attr = buttonbox->highlight,
	       cur_row = row;
	       cur_col = col;
	    }
	    writeChtypeAttrib (buttonbox->win,
			       col, row,
			       buttonbox->button[currentButton],
			       attr,
			       HORIZONTAL, 0,
			       buttonbox->buttonLen[currentButton]);
	    row += (1 + buttonbox->rowAdjust);
	    currentButton++;
	 }
         col += buttonbox->columnWidths[x] + buttonbox->colAdjust + BorderOf(buttonbox);
      }
   }
   if (cur_row >= 0 && cur_col >= 0)
      wmove(buttonbox->win, cur_row, cur_col);
   touchwin (buttonbox->win);
   wrefresh (buttonbox->win);
}

/*
 * This erases the buttonbox box from the screen.
 */
static void _eraseCDKButtonbox (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)object;

      eraseCursesWindow (buttonbox->win);
      eraseCursesWindow (buttonbox->shadowWin);
   }
}

/*
 * This moves the buttonbox box to a new screen location.
 */
static void _moveCDKButtonbox (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)object;
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
   alignxy (WindowOf(buttonbox), &xpos, &ypos, buttonbox->boxWidth, buttonbox->boxHeight, BorderOf(buttonbox));

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(buttonbox->win, -xdiff, -ydiff);
   moveCursesWindow(buttonbox->shadowWin, -xdiff, -ydiff);

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
 * This destroys the widget and all the memory associated with it.
 */
static void _destroyCDKButtonbox (CDKOBJS *object)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)object;
   int x;

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
void setCDKButtonboxCurrentButton (CDKBUTTONBOX * buttonbox, int button)
{
   if ((button >= 0) && (button < buttonbox->buttonCount))
   {
      buttonbox->currentButton = button;
   }
}
int getCDKButtonboxCurrentButton (CDKBUTTONBOX * buttonbox)
{
   return buttonbox->currentButton;
}
int getCDKButtonboxButtonCount (CDKBUTTONBOX * buttonbox)
{
   return buttonbox->buttonCount;
}

static void _focusCDKButtonbox (CDKOBJS * object GCC_UNUSED)
{
   /* FIXME */
}

static void _unfocusCDKButtonbox (CDKOBJS * entry GCC_UNUSED)
{
   /* FIXME */
}

static void _refreshDataCDKButtonbox (CDKOBJS * entry GCC_UNUSED)
{
   /* FIXME */
}

static void _saveDataCDKButtonbox (CDKOBJS * entry GCC_UNUSED)
{
   /* FIXME */
}
