#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2005/03/08 20:17:33 $
 * $Revision: 1.115 $
 */

/*
 * Declare file local prototypes.
 */
static int createList (CDKRADIO *radio, char **list, int listSize, int width);
static void drawCDKRadioList (CDKRADIO *radio, boolean Box);

DeclareCDKObjects(RADIO, Radio, setCdk, Int);

/*
 * This function creates the radio widget.
 */
CDKRADIO *newCDKRadio (CDKSCREEN *cdkscreen, int xplace, int yplace, int splace, int height, int width, char *title, char **list, int listSize, chtype choiceChar, int defItem, chtype highlight, boolean Box, boolean shadow)
{
   CDKRADIO *radio	= 0;
   int parentWidth	= getmaxx(cdkscreen->window);
   int parentHeight	= getmaxy(cdkscreen->window);
   int boxWidth		= width;
   int boxHeight	= height;
   int xpos		= xplace;
   int ypos		= yplace;
   int widestItem	= 0;
   int x;

   static const struct { int from; int to; } bindings[] = {
		{ CDK_BACKCHAR,	KEY_PPAGE },
		{ CDK_FORCHAR,	KEY_NPAGE },
   };

   if ((radio = newCDKObject(CDKRADIO, &my_funcs)) == 0)
   {
      return (0);
   }

   setCDKRadioBox (radio, Box);

  /*
   * If the height is a negative value, the height will
   * be ROWS-height, otherwise, the height will be the
   * given height.
   */
   boxHeight = setWidgetDimension (parentHeight, height, 0);

  /*
   * If the width is a negative value, the width will
   * be COLS-width, otherwise, the width will be the
   * given width.
   */
   boxWidth = setWidgetDimension (parentWidth, width, 5);

   boxWidth = setCdkTitle(ObjOf(radio), title, boxWidth);

   /* Set the box height. */
   if (TitleLinesOf(radio) > boxHeight)
   {
      if (listSize > 8)
      {
         boxHeight = TitleLinesOf(radio) + 8 + 2 * BorderOf(radio);
      }
      else
      {
         boxHeight = TitleLinesOf(radio) + listSize + 2 * BorderOf(radio);
      }
   }

   /* Set the rest of the variables. */
   radio->titleAdj	= TitleLinesOf(radio) + BorderOf(radio);
   radio->listSize	= listSize;
   radio->viewSize	= boxHeight - (2 * BorderOf(radio) + TitleLinesOf(radio));
   radio->lastItem	= listSize - 1;
   radio->maxTopItem	= listSize - radio->viewSize;

   /* Is the view size smaller than the window??? */
   if (listSize < (boxHeight - BorderOf(radio) - radio->titleAdj))
   {
      radio->viewSize	= listSize;
      radio->listSize	= listSize;
      radio->lastItem	= listSize;
      radio->maxTopItem = -1;
   }

   /* Adjust the box width if there is a scroll bar. */
   if (splace == LEFT || splace == RIGHT)
   {
      boxWidth++;
      radio->scrollbar = TRUE;
   }
   else
   {
      radio->scrollbar = FALSE;
   }

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth		= (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight		= (boxHeight > parentHeight ? parentHeight : boxHeight);

   /* Determine the size of the scrollbar toggle and the step. */
   radio->step		= (float)(boxHeight - 2 * BorderOf(radio)) / (float)radio->listSize;
   radio->toggleSize	= (radio->listSize > (boxHeight - 2 * BorderOf(radio)) ? 1 : ceilCDK(radio->step));

   /* Each item in the needs to be converted to chtype * */
   widestItem = createList(radio, list, listSize, boxWidth);
   if (widestItem <= 0)
   {
      destroyCDKObject(radio);
      return (0);
   }

   /*
    * Determine how many characters we can shift to the right
    * before all the items have been scrolled off the screen.
    */
   if (boxWidth > widestItem)
   {
      radio->maxLeftChar = 0;
   }
   else
   {
      radio->maxLeftChar = widestItem - (boxWidth - 2 * BorderOf(radio)) + 3;
   }

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the radio window */
   radio->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the window null??? */
   if (radio->win == 0)
   {
      destroyCDKObject(radio);
      return (0);
   }

   /* Turn on the keypad. */
   keypad (radio->win, TRUE);

   /* Create the scrollbar window. */
   if (splace == RIGHT)
   {
      radio->scrollbarWin = subwin (radio->win,
				    boxHeight - radio->titleAdj - BorderOf(radio), 1,
				    ypos + radio->titleAdj,
				    xpos + boxWidth - BorderOf(radio) - 1);
   }
   else if (splace == LEFT)
   {
      radio->scrollbarWin = subwin (radio->win,
				    boxHeight - radio->titleAdj - BorderOf(radio), 1,
				    ypos + radio->titleAdj,
				    xpos + BorderOf(radio));
   }
   else
   {
      radio->scrollbarWin = 0;
   }

   /* Set the rest of the variables */
   ScreenOf(radio)		= cdkscreen;
   radio->parent		= cdkscreen->window;
   radio->boxHeight		= boxHeight;
   radio->boxWidth		= boxWidth;
   radio->scrollbarPlacement	= splace;
   radio->widestItem		= widestItem;
   radio->currentTop		= 0;
   radio->currentItem		= 0;
   radio->currentHigh		= 0;
   radio->leftChar		= 0;
   radio->selectedItem		= 0;
   radio->highlight		= highlight;
   radio->choiceChar		= choiceChar;
   radio->leftBoxChar		= (chtype)'[';
   radio->rightBoxChar		= (chtype)']';
   radio->defItem		= defItem;
   initExitType(radio);
   ObjOf(radio)->inputWindow	= radio->win;
   ObjOf(radio)->acceptsFocus   = TRUE;
   radio->shadow		= shadow;

   /* Do we need to create the shadow??? */
   if (shadow)
   {
      radio->shadowWin	= newwin (boxHeight, boxWidth + 1, ypos + 1, xpos + 1);
   }

   /* Setup the key bindings. */
   for (x = 0; x < (int) SIZEOF(bindings); ++x)
      bindCDKObject (vRADIO, radio, bindings[x].from, getcCDKBind, (void *)(long)bindings[x].to);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vRADIO, radio);

   /* Return the radio list */
   return (radio);
}

/*
 * This actually manages the radio widget.
 */
int activateCDKRadio (CDKRADIO *radio, chtype *actions)
{
   /* Draw the radio list. */
   drawCDKRadio (radio, ObjOf(radio)->box);

   /* Check if actions is null. */
   if (actions == 0)
   {
      chtype input;
      int ret;

      for (;;)
      {
	 /* Get the input. */
	 input = getcCDKObject (ObjOf(radio));

	 /* Inject the character into the widget. */
	 ret = injectCDKRadio (radio, input);
	 if (radio->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }
   else
   {
      int length = chlen (actions);
      int x, ret;

      /* Inject each character one at a time. */
      for (x=0; x < length; x++)
      {
	 ret = injectCDKRadio (radio, actions[x]);
	 if (radio->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }

   /* Set the exit type and return. */
   setExitType(radio, 0);
   return -1;
}

/*
 * This injects a single character into the widget.
 */
static int _injectCDKRadio (CDKOBJS *object, chtype input)
{
   CDKRADIO *radio = (CDKRADIO *)object;
   int ppReturn = 1;
   int ret = unknownInt;
   bool complete = FALSE;

   /* Set the exit type. */
   setExitType(radio, 0);

   /* Draw the radio list */
   drawCDKRadioList (radio, ObjOf(radio)->box);

   /* Check if there is a pre-process function to be called. */
   if (PreProcessFuncOf(radio) != 0)
   {
      /* Call the pre-process function. */
      ppReturn = PreProcessFuncOf(radio) (vRADIO, radio, PreProcessDataOf(radio), input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a predefined key binding. */
      if (checkCDKObjectBind (vRADIO, radio, input) != 0)
      {
	 checkEarlyExit(radio);
	 complete = TRUE;
      }
      else
      {
	 switch (input)
	 {
	    case KEY_UP :
		 if (radio->currentHigh == 0)
		 {
		    if (radio->currentTop != 0)
		    {
		       radio->currentTop--;
		       radio->currentItem--;
		    }
		    else
		    {
		       Beep();
		    }
		 }
		 else
		 {
		    radio->currentItem--;
		    radio->currentHigh--;
		 }
		 break;

	    case KEY_DOWN :
		 if (radio->currentHigh == radio->viewSize - 1)
		 {
		    /* We need to scroll down one line. */
		    if (radio->currentTop < radio->maxTopItem)
		    {
		       radio->currentTop++;
		       radio->currentItem++;
		    }
		    else
		    {
		       Beep();
		    }
		 }
		 else
		 {
		    radio->currentItem++;
		    radio->currentHigh++;
		 }
		 break;

	    case KEY_RIGHT :
		 if (radio->leftChar >= radio->maxLeftChar)
		 {
		    Beep();
		 }
		 else
		 {
		    radio->leftChar ++;
		 }
		 break;

	    case KEY_LEFT :
		 if (radio->leftChar == 0)
		 {
		    Beep();
		 }
		 else
		 {
		    radio->leftChar --;
		 }
		 break;

	    case KEY_PPAGE :
		 if (radio->currentTop > 0)
		 {
		    if (radio->currentTop >= (radio->viewSize -1))
		    {
		       radio->currentTop	-= (radio->viewSize - 1);
		       radio->currentItem	-= (radio->viewSize - 1);
		    }
		    else
		    {
		       radio->currentTop	= 0;
		       radio->currentItem	= 0;
		       radio->currentHigh	= 0;
		    }
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_NPAGE :
		 if (radio->currentTop < radio->maxTopItem)
		 {
		    if ((radio->currentTop + radio->viewSize - 1) <= radio->maxTopItem)
		    {
		       radio->currentTop	+= (radio->viewSize - 1);
		       radio->currentItem	+= (radio->viewSize - 1);
		    }
		    else
		    {
		       radio->currentTop	= radio->maxTopItem;
		       radio->currentItem	= radio->lastItem;
		       radio->currentHigh	= radio->viewSize-1;
		    }
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case 'g' : case '1' :
		 radio->currentTop	= 0;
		 radio->currentItem	= 0;
		 radio->currentHigh	= 0;
		 break;

	    case 'G' :
		 radio->currentTop	= radio->maxTopItem;
		 radio->currentItem	= radio->lastItem;
		 radio->currentHigh	= radio->viewSize-1;
		 break;

	    case '$' :
		 radio->leftChar	= radio->maxLeftChar;
		 break;

	    case '|' :
		 radio->leftChar	= 0;
		 break;

	    case SPACE :
		 radio->selectedItem	= radio->currentItem;
		 break;

	    case KEY_ESC :
		 setExitType(radio, input);
		 ret = -1;
		 complete = TRUE;
		 break;

	    case KEY_TAB : case KEY_ENTER :
		 setExitType(radio, input);
		 ret = radio->selectedItem;
		 complete = TRUE;
		 break;

	    case CDK_REFRESH :
		 eraseCDKScreen (ScreenOf(radio));
		 refreshCDKScreen (ScreenOf(radio));
		 break;

	    default :
		 break;
	 }
      }

      /* Should we call a post-process? */
      if (!complete && (PostProcessFuncOf(radio) != 0))
      {
	 PostProcessFuncOf(radio) (vRADIO, radio, PostProcessDataOf(radio), input);
      }
   }

   if (!complete) {
      drawCDKRadioList (radio, ObjOf(radio)->box);
      setExitType(radio, 0);
   }

   ResultOf(radio).valueInt = ret;
   return (ret != unknownInt);
}

/*
 * This moves the radio field to the given location.
 */
static void _moveCDKRadio (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKRADIO *radio = (CDKRADIO *)object;
   int currentX = getbegx(radio->win);
   int currentY = getbegy(radio->win);
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
      xpos = getbegx(radio->win) + xplace;
      ypos = getbegy(radio->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(radio), &xpos, &ypos, radio->boxWidth, radio->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(radio->win, -xdiff, -ydiff);
   moveCursesWindow(radio->scrollbarWin, -xdiff, -ydiff);
   moveCursesWindow(radio->shadowWin, -xdiff, -ydiff);

   /* Touch the windows so they 'move'. */
   refreshCDKWindow (WindowOf(radio));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKRadio (radio, ObjOf(radio)->box);
   }
}

/*
 * This function draws the radio widget.
 */
static void _drawCDKRadio (CDKOBJS *object, boolean Box GCC_UNUSED)
{
   CDKRADIO *radio = (CDKRADIO *)object;

   /* Do we need to draw in the shadow??? */
   if (radio->shadowWin != 0)
   {
      drawShadow (radio->shadowWin);
   }

   drawCdkTitle (radio->win, object);

   /* Draw in the radio list. */
   drawCDKRadioList (radio, ObjOf(radio)->box);
}

/*
 * This redraws the radio list.
 */
static void drawCDKRadioList (CDKRADIO *radio, boolean Box)
{
   int scrollbarAdj	= 0;
   int screenPos	= 0;
   int x;

  /*
   * If the scroll bar is on the left hand side, then adjust
   * everything over to the right one character.
   */
   if (radio->scrollbarPlacement == LEFT)
   {
      scrollbarAdj = 1;
   }

   /* Redraw the list */
   for (x=0; x < radio->viewSize; x++)
   {
      screenPos = radio->itemPos[x + radio->currentTop] - radio->leftChar + scrollbarAdj + BorderOf(radio);

      /* Draw in the empty string. */
      writeBlanks (radio->win, 1, radio->titleAdj + x,
		   HORIZONTAL, 0, radio->boxWidth-1);

      /* Draw in the line. */
      if (screenPos >= 0)
      {
	 writeChtype (radio->win,
			screenPos, x + radio->titleAdj,
			radio->item[x + radio->currentTop],
			HORIZONTAL, 0,
			radio->itemLen[x + radio->currentTop]);
      }
      else
      {
	 writeChtype (radio->win,
			1, x + radio->titleAdj,
			radio->item[x + radio->currentTop],
			HORIZONTAL,
			radio->leftChar - radio->itemPos[x + radio->currentTop] + 1,
			radio->itemLen[x + radio->currentTop]);
      }

     /* Draw in the selected choice... */

     mvwaddch (radio->win, x + radio->titleAdj, BorderOf(radio) + scrollbarAdj, radio->leftBoxChar);
     mvwaddch (radio->win, x + radio->titleAdj, BorderOf(radio) + 1 + scrollbarAdj, x + radio->currentTop == radio->selectedItem ? radio->choiceChar : ' ');
     mvwaddch (radio->win, x + radio->titleAdj, BorderOf(radio) + 2 + scrollbarAdj, radio->rightBoxChar);

   }
   screenPos = radio->itemPos[radio->currentItem] - radio->leftChar + scrollbarAdj + BorderOf(radio);

   /* Draw in the filler character for the scroll bar. */
   if (radio->scrollbarWin != 0)
   {
      for (x=0; x < radio->boxHeight-1; x++)
      {
	 mvwaddch (radio->scrollbarWin, x, 0, ACS_CKBOARD);
      }
   }

   /* Highlight the current item. */
   if (ObjPtr(radio)->hasFocus)
   {
      if (screenPos >= 0)
      {
	 writeChtypeAttrib (radio->win,
			   screenPos, radio->currentHigh + radio->titleAdj,
			   radio->item[radio->currentItem],
			   radio->highlight,
			   HORIZONTAL, 0,
			   radio->itemLen[radio->currentItem]);
      }
      else
      {
	 writeChtypeAttrib (radio->win,
			   1 + scrollbarAdj,
			   radio->currentHigh + radio->titleAdj,
			   radio->item[radio->currentItem],
			   radio->highlight,
			   HORIZONTAL,
			   radio->leftChar - radio->itemPos[radio->currentTop] + 1,
			   radio->itemLen[radio->currentItem]);
      }
   }

#if 0
     /* Draw in the selected choice... */
   mvwaddch (radio->win, radio->currentHigh + radio->titleAdj, BorderOf(radio) + scrollbarAdj, radio->leftBoxChar);
   mvwaddch (radio->win, radio->currentHigh + radio->titleAdj, BorderOf(radio) + 1 + scrollbarAdj, radio->currentItem == radio->selectedItem ? radio->choiceChar : ' ');
   mvwaddch (radio->win, radio->currentHigh + radio->titleAdj, BorderOf(radio) + 2 + scrollbarAdj, radio->rightBoxChar);
#endif

   /* Determine where the toggle is supposed to be. */
   if (radio->scrollbar)
   {
      if (radio->listSize > radio->boxHeight - 2 * BorderOf(radio))
      {
	 radio->togglePos = floorCDK((float)radio->currentItem * (float)radio->step);
      }
      else
      {
	 radio->togglePos = ceilCDK((float)radio->currentItem * (float)radio->step);
      }

      /* Make sure the toggle button doesn't go out of bounds. */
      scrollbarAdj = ((radio->togglePos + radio->toggleSize) -
		      (radio->boxHeight - radio->titleAdj - 1));
      if (scrollbarAdj > 0)
      {
	 radio->togglePos -= scrollbarAdj;
      }

      /* Draw the scrollbar. */
      for (x=radio->togglePos; x < radio->togglePos + radio->toggleSize; x++)
      {
	 mvwaddch (radio->scrollbarWin, x, 0, ' ' | A_REVERSE);
      }
      refreshCDKWindow (radio->scrollbarWin);
   }

   /* Box it if needed. */
   if (Box)
   {
      drawObjBox (radio->win, ObjOf(radio));
   }

   /* Refresh the window. */
   refreshCDKWindow (radio->win);
}

/*
 * This sets the background attribute of the widget.
 */
static void _setBKattrRadio (CDKOBJS *object, chtype attrib)
{
   if (object != 0)
   {
      CDKRADIO *widget = (CDKRADIO *) object;

      wbkgd (widget->win, attrib);
      if (widget->scrollbarWin != 0)
      {
	 wbkgd (widget->scrollbarWin, attrib);
      }
   }
}

/*
 * This function destroys the radio widget.
 */
static void _destroyCDKRadio (CDKOBJS *object)
{
   if (object != 0)
   {
      CDKRADIO *radio = (CDKRADIO *)object;

      cleanCdkTitle (object);
      CDKfreeChtypes(radio->item);

      /* Clean up the windows. */
      deleteCursesWindow (radio->scrollbarWin);
      deleteCursesWindow (radio->shadowWin);
      deleteCursesWindow (radio->win);

      /* Unregister this object. */
      unregisterCDKObject (vRADIO, radio);
   }
}

/*
 * This function erases the radio widget.
 */
static void _eraseCDKRadio (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKRADIO *radio = (CDKRADIO *)object;

      eraseCursesWindow (radio->win);
      eraseCursesWindow (radio->shadowWin);
   }
}

/*
 * This set various attributes of the radio list.
 */
void setCDKRadio (CDKRADIO *radio, chtype highlight, chtype choiceChar, int Box)
{
   setCDKRadioHighlight (radio, highlight);
   setCDKRadioChoiceCharacter (radio, choiceChar);
   setCDKRadioBox (radio, Box);
}

/*
 * This sets the radio list items.
 */
void setCDKRadioItems (CDKRADIO *radio, char **list, int listSize)
{
   int widestItem	= -1;
   int x		= 0;

   widestItem = createList(radio, list, listSize, radio->boxWidth);
   if (widestItem <= 0)
      return;

   /* Clean up the display. */
   for (x=0; x < radio->viewSize ; x++)
   {
      writeBlanks (radio->win, 1, radio->titleAdj + x,
		   HORIZONTAL, 0, radio->boxWidth-1);
   }

   /* Readjust all of the variables ... */
   radio->listSize	= listSize;
   radio->viewSize	= radio->boxHeight - (2 * BorderOf(radio) + TitleLinesOf(radio));
   radio->lastItem	= listSize - 1;
   radio->maxTopItem	= listSize - radio->viewSize;

   /* Is the view size smaller than the window? */
   if (listSize < (radio->boxHeight-1-radio->titleAdj))
   {
      radio->viewSize	= listSize;
      radio->listSize	= listSize;
      radio->lastItem	= listSize;
      radio->maxTopItem = -1;
   }

   /* Set some vars. */
   radio->currentTop	= 0;
   radio->currentItem	= 0;
   radio->currentHigh	= 0;
   radio->leftChar	= 0;
   radio->selectedItem	= 0;

   /* Set the information for the radio bar. */
   radio->step		= (float)(radio->boxHeight-2) / (float)radio->listSize;
   radio->toggleSize	= (radio->listSize > (radio->boxHeight-2) ? 1 : ceilCDK(radio->step));

   /*
    * Determine how many characters we can shift to the right
    * before all the items have been scrolled off the screen.
    */
   if (radio->boxWidth > widestItem)
   {
      radio->maxLeftChar = 0;
   }
   else
   {
      radio->maxLeftChar = widestItem-(radio->boxWidth-2) + 3;
   }
}
int getCDKRadioItems (CDKRADIO *radio, char *list[])
{
   int x;

   for (x=0; x < radio->listSize; x++)
   {
      list[x] = chtype2Char (radio->item[x]);
   }
   return radio->listSize;
}

/*
 * This sets the highlight bar of the radio list.
 */
void setCDKRadioHighlight (CDKRADIO *radio, chtype highlight)
{
   radio->highlight = highlight;
}
chtype getCDKRadioHighlight (CDKRADIO *radio)
{
   return radio->highlight;
}

/*
 * This sets the character to use when selecting an item in the list.
 */
void setCDKRadioChoiceCharacter (CDKRADIO *radio, chtype character)
{
   radio->choiceChar = character;
}
chtype getCDKRadioChoiceCharacter (CDKRADIO *radio)
{
   return radio->choiceChar;
}

/*
 * This sets the character to use to draw the left side of the
 * choice box on the list.
 */
void setCDKRadioLeftBrace (CDKRADIO *radio, chtype character)
{
   radio->leftBoxChar = character;
}
chtype getCDKRadioLeftBrace (CDKRADIO *radio)
{
   return radio->leftBoxChar;
}

/*
 * This sets the character to use to draw the right side of the
 * choice box on the list.
 */
void setCDKRadioRightBrace (CDKRADIO *radio, chtype character)
{
   radio->rightBoxChar = character;
}
chtype getCDKRadioRightBrace (CDKRADIO *radio)
{
   return radio->rightBoxChar;
}

/*
 * This sets the box attribute of the widget.
 */
void setCDKRadioBox (CDKRADIO *radio, boolean Box)
{
   ObjOf(radio)->box = Box;
   ObjOf(radio)->borderSize = Box ? 1 : 0;
}
boolean getCDKRadioBox (CDKRADIO *radio)
{
   return ObjOf(radio)->box;
}

/*
 * This sets the current selected item of the widget
 */
void setCDKRadioCurrentItem (CDKRADIO *radio, int cur)
{
   radio->selectedItem = cur;
   radio->currentItem = cur;
   radio->currentHigh = cur;
}
int getCDKRadioCurrentItem (CDKRADIO *radio)
{
   return radio->currentItem;
}

static void _focusCDKRadio(CDKOBJS *object)
{
   CDKRADIO *radio = (CDKRADIO *)object;

   drawCDKRadioList (radio, ObjOf(radio)->box);
}

static void _unfocusCDKRadio(CDKOBJS *object)
{
   CDKRADIO *radio = (CDKRADIO *)object;

   drawCDKRadioList (radio, ObjOf(radio)->box);
}

dummyRefreshData(Radio)

dummySaveData(Radio)

static int createList (CDKRADIO *radio, char **list, int listSize, int boxWidth)
{
   int status = 0;
   int widestItem = 0;

   if (listSize > 0)
   {
      chtype **newList = typeCallocN(chtype *, listSize + 1);
      int *newLen = typeCallocN(int, listSize + 1);
      int *newPos = typeCallocN(int, listSize + 1);
      int x;

      if (newList != 0
        && newLen != 0
	&& newPos != 0)
      {
	 /* Each item in the needs to be converted to chtype * */
	 status = 1;
	 boxWidth -= (2 + BorderOf(radio));
	 for (x=0; x < listSize; x++)
	 {
	    newList[x]	= char2Chtype (list[x], &newLen[x], &newPos[x]);
	    if (newList[x] == 0)
	    {
	       status = 0;
	       break;
	    }
	    newPos[x]   = justifyString (boxWidth, newLen[x], newPos[x]) + 3;
	    widestItem	= MAXIMUM(widestItem, newLen[x]);
	 }
	 if (status)
	 {
	    CDKfreeChtypes (radio->item);
	    freeChecked (radio->itemLen);
	    freeChecked (radio->itemPos);

	    radio->item = newList;
	    radio->itemLen = newLen;
	    radio->itemPos = newPos;
	 }
	 else
	 {
	    CDKfreeChtypes (newList);
	    freeChecked (newLen);
	    freeChecked (newPos);
	 }
      }
   }
   return status ? widestItem : 0;
}
