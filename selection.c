#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2004/08/31 01:41:53 $
 * $Revision: 1.128 $
 */

/*
 * Declare file local prototypes.
 */
static int createList(CDKSELECTION *selection, char **list, int listSize);
static void drawCDKSelectionList (CDKSELECTION *selection, boolean Box);

DeclareCDKObjects(SELECTION, Selection, setCdk, Int);

/*
 * This function creates a selection widget.
 */
CDKSELECTION *newCDKSelection (CDKSCREEN *cdkscreen, int xplace, int yplace, int splace, int height, int width, char *title, char **list, int listSize, char **choices, int choiceCount, chtype highlight, boolean Box, boolean shadow)
{
   CDKSELECTION *selection	= 0;
   int widestItem		= -1;
   int parentWidth		= getmaxx(cdkscreen->window);
   int parentHeight		= getmaxy(cdkscreen->window);
   int boxWidth			= width;
   int boxHeight		= height;
   int xpos			= xplace;
   int ypos			= yplace;
   int x			= 0;
   int junk2;

   static const struct { int from; int to; } bindings[] = {
	     { CDK_BACKCHAR,	KEY_PPAGE },
	     { CDK_FORCHAR,	KEY_NPAGE },
   };

   if (choiceCount <= 0
    || (selection = newCDKObject(CDKSELECTION, &my_funcs)) == 0
    || (selection->choice    = typeCallocN(chtype *, choiceCount + 1)) == 0
    || (selection->choicelen = typeCallocN(int,      choiceCount + 1)) == 0)
   {
      destroyCDKObject(selection);
      return (0);
   }

   setCDKSelectionBox (selection, Box);

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
   boxWidth = setWidgetDimension (parentWidth, width, 0);

   boxWidth = setCdkTitle(ObjOf(selection), title, boxWidth);

   /* Set the box height. */
   if (TitleLinesOf(selection) > boxHeight) {
      if (listSize > 8)
	 boxHeight = TitleLinesOf(selection) + 10;
      else
	 boxHeight = TitleLinesOf(selection) + listSize + 2;
   }

   /* Set the rest of the variables. */
   selection->titleAdj		= TitleLinesOf(selection) + BorderOf(selection);
   selection->listSize		= listSize;
   selection->viewSize		= boxHeight - (2 * BorderOf(selection) + TitleLinesOf(selection));
   selection->lastItem		= listSize - 1;
   selection->maxTopItem	= listSize - selection->viewSize;
   selection->maxchoicelen	= -1;

   /* Is the view size smaller than the window??? */
   if (listSize < (boxHeight - BorderOf(selection) - selection->titleAdj))
   {
      selection->viewSize	= listSize;
      selection->listSize	= listSize;
      selection->lastItem	= listSize;
      selection->maxTopItem	= -1;
   }

   /* Adjust the box width if there is a scroll bar. */
   if (splace == LEFT || splace == RIGHT)
   {
      boxWidth++;
      selection->scrollbar = TRUE;
   }
   else
   {
      selection->scrollbar = FALSE;
   }

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);

   /* Determine the size of the scrollbar toggle and the step. */
   selection->step		= (float)(boxHeight - 2 * BorderOf(selection)) / (float)selection->listSize;
   selection->toggleSize	= (selection->listSize > (boxHeight - 2 * BorderOf(selection)) ? 1 : ceilCDK(selection->step));

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the selection window */
   selection->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the window null?? */
   if (selection->win == 0)
   {
      destroyCDKObject(selection);
      return (0);
   }

   /* Turn the keypad on for this window. */
   keypad (selection->win, TRUE);

   /* Create the scrollbar window. */
   if (splace == RIGHT)
   {
      selection->scrollbarWin = subwin (selection->win,
					boxHeight - selection->titleAdj - BorderOf(selection), 1,
					ypos + selection->titleAdj,
					xpos + boxWidth - 1 - BorderOf(selection));
   }
   else if (splace == LEFT)
   {
      selection->scrollbarWin = subwin (selection->win,
					boxHeight - selection->titleAdj - BorderOf(selection), 1,
					ypos + selection->titleAdj,
					xpos + BorderOf(selection));
   }
   else
   {
      selection->scrollbarWin = 0;
   }

   /* Set the rest of the variables */
   ScreenOf(selection)			= cdkscreen;
   selection->parent			= cdkscreen->window;
   selection->boxHeight			= boxHeight;
   selection->boxWidth			= boxWidth;
   selection->scrollbarPlacement	= splace;
   selection->currentTop		= 0;
   selection->currentHigh		= 0;
   selection->currentItem		= 0;
   selection->maxLeftChar		= 0;
   selection->leftChar			= 0;
   selection->highlight			= highlight;
   selection->choiceCount		= choiceCount;
   initExitType(selection);
   ObjOf(selection)->acceptsFocus	= TRUE;
   ObjOf(selection)->inputWindow	= selection->win;
   selection->shadow			= shadow;

   /* Each choice has to be converted from char * to chtype * */
   for (x = 0; x < choiceCount; x++)
   {
      selection->choice[x]	= char2Chtype (choices[x], &selection->choicelen[x], &junk2);
      selection->maxchoicelen	= MAXIMUM (selection->maxchoicelen, selection->choicelen[x]);
   }

   /* Each item in the needs to be converted to chtype * */
   widestItem = createList(selection, list, listSize);
   if (widestItem <= 0)
   {
      destroyCDKObject(selection);
      return (0);
   }

  /*
   * Determine how many characters we can shift to the right
   * before all the items have been scrolled off the screen.
   */
   if ((selection->boxWidth - selection->maxchoicelen) > widestItem)
   {
      selection->maxLeftChar = 0;
   }
   else
   {
      selection->maxLeftChar = widestItem-(selection->boxWidth - selection->maxchoicelen - 2);
   }

   /* Do we need to create a shadow. */
   if (shadow)
   {
      selection->shadowWin = newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
   }

   /* Setup the key bindings. */
   for (x = 0; x < (int) SIZEOF(bindings); ++x)
      bindCDKObject (vSELECTION, selection, bindings[x].from, getcCDKBind, (void *)(long)bindings[x].to);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vSELECTION, selection);

   /* Return the selection list */
   return (selection);
}

/*
 * This actually manages the selection widget...
 */
int activateCDKSelection (CDKSELECTION *selection, chtype *actions)
{
   /* Draw the selection list */
   drawCDKSelection (selection, ObjOf(selection)->box);

   /* Check if actions is null. */
   if (actions == 0)
   {
      chtype input;
      int ret;

      for (;;)
      {
	 /* Get the input. */
	 input = getcCDKObject (ObjOf(selection));

	 /* Inject the character into the widget. */
	 ret = injectCDKSelection (selection, input);
	 if (selection->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }
   else
   {
      int length = chlen (actions);
      int x = 0;
      int ret;

      /* Inject each character one at a time. */
      for (x = 0; x < length; x++)
      {
	 ret = injectCDKSelection (selection, actions[x]);
	 if (selection->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }

   /* Set the exit type and return. */
   setExitType(selection, 0);
   return 0;
}

/*
 * This injects a single character into the widget.
 */
static int _injectCDKSelection (CDKOBJS *object, chtype input)
{
   CDKSELECTION *selection = (CDKSELECTION *)object;
   int ppReturn = 1;
   int ret = unknownInt;
   bool complete = FALSE;

   /* Set the exit type. */
   setExitType(selection, 0);

   /* Draw the selection list */
   drawCDKSelectionList (selection, ObjOf(selection)->box);

   /* Check if there is a pre-process function to be called. */
   if (PreProcessFuncOf(selection) != 0)
   {
      /* Call the pre-process function. */
      ppReturn = PreProcessFuncOf(selection) (vSELECTION, selection, PreProcessDataOf(selection), input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a predefined binding. */
      if (checkCDKObjectBind (vSELECTION, selection, input) != 0)
      {
	 checkEarlyExit(selection);
	 complete = TRUE;
      }
      else
      {
	 switch (input)
	 {
	    case KEY_UP :
		 if (selection->currentHigh == 0)
		 {
		    if (selection->currentTop != 0)
		    {
		       selection->currentTop--;
		       selection->currentItem--;
		    }
		    else
		    {
		       Beep();
		    }
		 }
		 else
		 {
		    selection->currentItem--;
		    selection->currentHigh--;
		 }
		 break;

	    case KEY_DOWN :
		 if (selection->currentHigh == selection->viewSize - 1)
		    {
		       /* We need to scroll down one line. */
		       if (selection->currentTop < selection->maxTopItem)
		       {
			  selection->currentTop++;
			  selection->currentItem++;
		       }
		       else
		       {
			  Beep();
		       }
		    }
		    else
		    {
		       selection->currentItem++;
		       selection->currentHigh++;
		    }
		    break;

	       case KEY_RIGHT :
		    if (selection->leftChar >= selection->maxLeftChar)
		    {
		       Beep();
		    }
		    else
		    {
		       selection->leftChar ++;
		    }
		    break;

	       case KEY_LEFT :
		    if (selection->leftChar == 0)
		    {
		       Beep();
		    }
		    else
		    {
		       selection->leftChar --;
		    }
		    break;

	       case KEY_PPAGE :
		    if (selection->currentTop > 0)
		    {
		       if (selection->currentTop >= (selection->viewSize -1))
		       {
			  selection->currentTop -= (selection->viewSize - 1);
			  selection->currentItem	-= (selection->viewSize - 1);
		       }
		       else
		       {
			  selection->currentTop		= 0;
			  selection->currentItem	= 0;
			  selection->currentHigh	= 0;
		       }
		    }
		    else
		    {
		       Beep();
		    }
		    break;

	       case KEY_NPAGE :
		    if (selection->currentTop < selection->maxTopItem)
		    {
		       if ((selection->currentTop + selection->viewSize - 1) <= selection->maxTopItem)
		       {
			  selection->currentTop += (selection->viewSize - 1);
			  selection->currentItem	+= (selection->viewSize - 1);
		       }
		       else
		       {
			  selection->currentTop		= selection->maxTopItem;
			  selection->currentItem	= selection->lastItem;
			  selection->currentHigh	= selection->viewSize - 1;
		       }
		    }
		    else
		    {
		       Beep();
		    }
		    break;

	       case 'g' : case '1' :
		    selection->currentTop	= 0;
		    selection->currentItem	= 0;
		    selection->currentHigh	= 0;
		    break;

	       case 'G' :
		    selection->currentTop	= selection->maxTopItem;
		    selection->currentItem	= selection->lastItem;
		    selection->currentHigh	= selection->viewSize - 1;
		    break;

	       case '$' :
		    selection->leftChar = selection->maxLeftChar;
		    break;

	       case '|' :
		    selection->leftChar = 0;
		    break;

	       case SPACE :
		    if (selection->mode[selection->currentItem] == 0)
		    {
		       if (selection->selections[selection->currentItem] == selection->choiceCount - 1)
		       {
			  selection->selections[selection->currentItem] = 0;
		       }
		       else
		       {
			  selection->selections[selection->currentItem]++;
		       }
		    }
		    else
		    {
		       Beep();
		    }
		    break;

	       case KEY_ESC :
		    setExitType(selection, input);
		    complete = TRUE;
		    break;

	       case KEY_TAB : case KEY_ENTER :
		    setExitType(selection, input);
		    ret = 1;
		    complete = TRUE;
		    break;

	       case CDK_REFRESH :
		    eraseCDKScreen (ScreenOf(selection));
		    refreshCDKScreen (ScreenOf(selection));
		    break;

	       default :
		    break;
	 }
      }

      /* Should we call a post-process? */
      if (!complete && (PostProcessFuncOf(selection) != 0))
      {
	 PostProcessFuncOf(selection) (vSELECTION, selection, PostProcessDataOf(selection), input);
      }
   }

   if (!complete) {
      drawCDKSelectionList (selection, ObjOf(selection)->box);
      setExitType(selection, 0);
   }

   ResultOf(selection).valueInt = ret;
   return (ret != unknownInt);
}

/*
 * This moves the selection field to the given location.
 */
static void _moveCDKSelection (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKSELECTION *selection = (CDKSELECTION *)object;
   int currentX = getbegx(selection->win);
   int currentY = getbegy(selection->win);
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
      xpos = getbegx(selection->win) + xplace;
      ypos = getbegy(selection->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(selection), &xpos, &ypos, selection->boxWidth, selection->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(selection->win, -xdiff, -ydiff);
   moveCursesWindow(selection->scrollbarWin, -xdiff, -ydiff);
   moveCursesWindow(selection->shadowWin, -xdiff, -ydiff);

   /* Touch the windows so they 'move'. */
   refreshCDKWindow (WindowOf(selection));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKSelection (selection, ObjOf(selection)->box);
   }
}

/*
 * This function draws the selection list.
 */
static void _drawCDKSelection (CDKOBJS *object, boolean Box)
{
   CDKSELECTION *selection = (CDKSELECTION *)object;

   /* Draw in the shadow if we need to. */
   if (selection->shadowWin != 0)
   {
      drawShadow (selection->shadowWin);
   }

   drawCdkTitle (selection->win, object);

   /* Redraw the list */
   drawCDKSelectionList (selection, Box);
}

/*
 * This function draws the selection list window.
 */
static void drawCDKSelectionList (CDKSELECTION *selection, boolean Box GCC_UNUSED)
{
   int scrollbarAdj	= 0;
   int screenPos	= 0;
   int x;

   /*
    * If the scroll bar is on the left hand side, then adjust
    * everything over to the right one character.
     */
   if (selection->scrollbarPlacement == LEFT)
   {
      scrollbarAdj = 1;
   }

   /* Redraw the list... */
   for (x = 0; x < selection->viewSize; x++)
   {
      screenPos = selection->itemPos[x + selection->currentTop] - selection->leftChar + scrollbarAdj;

      /* Draw in the empty line. */
      writeBlanks (selection->win, 1,
			selection->titleAdj + x,
			HORIZONTAL, 0,
			getmaxx(selection->win));

      /* Draw in the selection item. */
      if (screenPos >= 0)
      {
	 writeChtype (selection->win,
		screenPos, x + selection->titleAdj,
		selection->item[x + selection->currentTop],
		HORIZONTAL, 0,
		selection->itemLen[x + selection->currentTop]);
      }
      else
      {
	 writeChtype (selection->win,
		1, x + selection->titleAdj,
		selection->item[x + selection->currentTop],
		HORIZONTAL,
		selection->leftChar - selection->itemPos[x + selection->currentTop] + 1,
		selection->itemLen[x + selection->currentTop]);
      }


      /* Draw in the choice value. */
      writeChtype (selection->win, 1 + scrollbarAdj,
		x + selection->titleAdj,
		selection->choice[selection->selections[x + selection->currentTop]],
		HORIZONTAL,
		0,
		selection->choicelen[selection->selections[x + selection->currentTop]]);
   }

   /* Draw in the filler character for the scroll bar. */
   if (selection->scrollbarWin != 0)
   {
      for (x = 0; x < selection->boxHeight - 1; x++)
      {
	 mvwaddch (selection->scrollbarWin, x, 0, ACS_CKBOARD);
      }
   }

   /* Draw in the current highlight. */
   if (ObjOf(selection)->hasFocus)
      writeChtypeAttrib (selection->win,
			selection->itemPos[selection->currentItem] + scrollbarAdj,
			selection->currentHigh + selection->titleAdj,
			selection->item[selection->currentItem],
			selection->highlight,
			HORIZONTAL, selection->leftChar,
			selection->itemLen[selection->currentItem]);

   /* Determine where the toggle is supposed to be. */
   if (selection->scrollbar)
   {
      if (selection->listSize > selection->boxHeight - 2)
      {
	 selection->togglePos = floorCDK((float)selection->currentItem * (float)selection->step);
      }
      else
      {
	 selection->togglePos = ceilCDK((float)selection->currentItem * (float)selection->step);
      }

      /* Make sure the toggle button doesn't go out of bounds. */
      scrollbarAdj = (selection->togglePos + selection->toggleSize)-(selection->boxHeight - selection->titleAdj - 1);
      if (scrollbarAdj > 0)
      {
	 selection->togglePos -= scrollbarAdj;
      }

      /* Draw the scrollbar. */
      for (x = selection->togglePos; x < selection->togglePos + selection->toggleSize; x++)
      {
	 mvwaddch (selection->scrollbarWin, x, 0, ' ' | A_REVERSE);
      }
      refreshCDKWindow (selection->scrollbarWin);
   }

   /* Box it if needed */
   if (ObjOf(selection)->box)
   {
      drawObjBox (selection->win, ObjOf(selection));
   }

   /* Refresh the window. */
   refreshCDKWindow (selection->win);
}

/*
 * This sets the background attribute of the widget.
 */
static void _setBKattrSelection (CDKOBJS *object, chtype attrib)
{
   if (object != 0)
   {
      CDKSELECTION *widget = (CDKSELECTION *) object;

      wbkgd (widget->win, attrib);
      if (widget->scrollbarWin != 0)
      {
	 wbkgd (widget->scrollbarWin, attrib);
      }
   }
}

/*
 * This function destroys the selection list.
 */
static void _destroyCDKSelection (CDKOBJS *object)
{
   if (object != 0)
   {
      CDKSELECTION *selection = (CDKSELECTION *)object;

      cleanCdkTitle (object);
      CDKfreeChtypes (selection->choice);
      freeChecked (selection->choicelen);
      CDKfreeChtypes (selection->item);
      freeChecked (selection->itemPos);
      freeChecked (selection->itemLen);
      freeChecked (selection->selections);
      freeChecked (selection->mode);

      /* Clean up the windows. */
      deleteCursesWindow (selection->scrollbarWin);
      deleteCursesWindow (selection->shadowWin);
      deleteCursesWindow (selection->win);

      /* Unregister this object. */
      unregisterCDKObject (vSELECTION, selection);
   }
}

/*
 * This function erases the selection list from the screen.
 */
static void _eraseCDKSelection (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKSELECTION *selection = (CDKSELECTION *)object;

      eraseCursesWindow (selection->win);
      eraseCursesWindow (selection->shadowWin);
   }
}

/*
 * This function sets a couple of the selection list attributes.
 */
void setCDKSelection (CDKSELECTION *selection, chtype highlight, int choices[], boolean Box)
{
   setCDKSelectionChoices (selection, choices);
   setCDKSelectionHighlight(selection, highlight);
   setCDKSelectionBox (selection, Box);
}

/*
 * This sets the selection list items.
 */
void setCDKSelectionItems (CDKSELECTION *selection, char **list, int listSize)
{
   int widestItem	= -1;
   int x		= 0;

   widestItem = createList(selection, list, listSize);
   if (widestItem <= 0)
      return;

   /* Clean up the display. */
   for (x = 0; x < selection->viewSize ; x++)
   {
      writeBlanks (selection->win, 1, selection->titleAdj + x,
		   HORIZONTAL, 0, getmaxx(selection->win));
   }

   /* Readjust all of the variables ... */
   selection->listSize		= listSize;
   selection->viewSize		= selection->boxHeight - (2 + TitleLinesOf(selection));
   selection->lastItem		= listSize - 1;
   selection->maxTopItem	= listSize - selection->viewSize;

   /* Is the view size smaller than the window? */
   if (listSize < (selection->boxHeight - 1 - selection->titleAdj))
   {
      selection->viewSize	= listSize;
      selection->listSize	= listSize;
      selection->lastItem	= listSize;
      selection->maxTopItem	= -1;
   }

   /* Set some vars. */
   selection->currentTop	= 0;
   selection->currentItem	= 0;
   selection->currentHigh	= 0;
   selection->leftChar		= 0;

   /* Set the information for the selection bar. */
   selection->step		= (float)(selection->boxHeight - 2) / (float)selection->listSize;
   selection->toggleSize	= (selection->listSize > (selection->boxHeight - 2) ? 1 : ceilCDK(selection->step));

  /*
   * Determine how many characters we can shift to the right
   * before all the items have been scrolled off the screen.
   */
   if ((selection->boxWidth - selection->maxchoicelen) > widestItem)
   {
      selection->maxLeftChar = 0;
   }
   else
   {
      selection->maxLeftChar = widestItem-(selection->boxWidth - selection->maxchoicelen - 2);
   }
}
int getCDKSelectionItems (CDKSELECTION *selection, char *list[])
{
   int x;

   for (x = 0; x < selection->listSize; x++)
   {
      list[x] = chtype2Char (selection->item[x]);
   }
   return selection->listSize;
}

/*
 *
 */
void setCDKSelectionTitle (CDKSELECTION *selection, char *title)
{
   /* Make sure the title isn't null. */
   if (title == 0)
   {
      return;
   }

   (void) setCdkTitle(ObjOf(selection), title, - (selection->boxWidth + 1));

   /* Set the rest of the variables. */
   selection->titleAdj		= TitleLinesOf(selection) + 1;
   selection->viewSize		= selection->boxHeight - (2 + TitleLinesOf(selection));
   selection->maxTopItem	= selection->listSize - selection->viewSize;
   selection->maxchoicelen	= -1;

   /* Is the view size smaller than the window??? */
   if (selection->listSize < (selection->boxHeight - 1 - selection->titleAdj))
   {
      selection->viewSize	= selection->listSize;
      selection->lastItem	= selection->listSize;
      selection->maxTopItem	= -1;
   }

   /* Determine the size of the scrollbar toggle and the step. */
   selection->step		= (float)(selection->boxHeight - 2) / (float)selection->listSize;
   selection->toggleSize	= (selection->listSize > (selection->boxHeight - 2) ? 1 : ceilCDK(selection->step));
}
char *getCDKSelectionTitle (CDKSELECTION *selection GCC_UNUSED)
{
   /* FIXME: this is not implemented */
   return ("HELLO");
}

/*
 * This sets the highlight bar.
 */
void setCDKSelectionHighlight (CDKSELECTION *selection, chtype highlight)
{
   selection->highlight = highlight;
}
chtype getCDKSelectionHighlight (CDKSELECTION *selection)
{
   return selection->highlight;
}

/*
 * This sets the default choices for the selection list.
 */
void setCDKSelectionChoices (CDKSELECTION *selection, int choices[])
{
   int x;

   /* Set the choice values in the selection list. */
   for (x = 0; x < selection->listSize; x++)
   {
      if (choices[x] < 0)
      {
	 selection->selections[x] = 0;
      }
      else if (choices[x] > selection->choiceCount)
      {
	 selection->selections[x] = selection->choiceCount - 1;
      }
      else
      {
	 selection->selections[x] = choices[x];
      }
   }
}
int *getCDKSelectionChoices (CDKSELECTION *selection)
{
   return selection->selections;
}

/*
 * This sets a single item's choice value.
 */
void setCDKSelectionChoice (CDKSELECTION *selection, int Index, int choice)
{
   int correctChoice = choice;
   int correctIndex = Index;

   /* Verify that the choice value is in range. */
   if (choice < 0)
   {
      correctChoice = 0;
   }
   else if (choice > selection->choiceCount)
   {
      correctChoice = selection->choiceCount - 1;
   }

   /* Make sure the index isn't out of range. */
   if (Index < 0)
   {
      Index = 0;
   }
   else if (Index > selection->listSize)
   {
      Index = selection->listSize - 1;
   }

   /* Set the choice value. */
   selection->selections[correctIndex] = correctChoice;
}
int getCDKSelectionChoice (CDKSELECTION *selection, int Index)
{
   /* Make sure the index isn't out of range. */
   if (Index < 0)
   {
      return selection->selections[0];
   }
   else if (Index > selection->listSize)
   {
      return selection->selections[selection->listSize - 1];
   }
   else
   {
      return selection->selections[Index];
   }
}

/*
 * This sets the modes of the items in the selection list. Currently
 * there are only two: editable=0 and read-only=1
 */
void setCDKSelectionModes (CDKSELECTION *selection, int modes[])
{
   int x;

   /* Make sure the widget pointer is not null. */
   if (selection == 0)
   {
      return;
   }

   /* Set the modes. */
   for (x = 0; x < selection->listSize; x++)
   {
      selection->mode[x] = modes[x];
   }
}
int *getCDKSelectionModes (CDKSELECTION *selection)
{
   return selection->mode;
}

/*
 * This sets a single mode of an item in the selection list.
 */
void setCDKSelectionMode (CDKSELECTION *selection, int Index, int mode)
{
   /* Make sure the widget pointer is not null. */
   if (selection == 0)
   {
      return;
   }

   /* Make sure the index isn't out of range. */
   if (Index < 0)
   {
      selection->mode[0] = mode;
   }
   else if (Index > selection->listSize)
   {
      selection->mode[selection->listSize - 1] = mode;
   }
   else
   {
      selection->mode[Index] = mode;
   }
}
int getCDKSelectionMode (CDKSELECTION *selection, int Index)
{
   /* Make sure the index isn't out of range. */
   if (Index < 0)
   {
      return selection->mode[0];
   }
   else if (Index > selection->listSize)
   {
      return selection->mode[selection->listSize - 1];
   }
   else
   {
      return selection->mode[Index];
   }
}

/*
 * This sets the box attribute of the widget.
 */
void setCDKSelectionBox (CDKSELECTION *selection, boolean Box)
{
   ObjOf(selection)->box = Box;
   ObjOf(selection)->borderSize = Box ? 1 : 0;
}
boolean getCDKSelectionBox (CDKSELECTION *selection)
{
   return ObjOf(selection)->box;
}

/*
 * set/get the current item index
 */
void setCDKSelectionCurrent (CDKSELECTION *selection, int inx)
{
  if (inx < 0)
    selection->currentItem = 0;
  else if (inx >= selection->listSize)
    selection->currentItem = selection->listSize - 1;
  else
    selection->currentItem = inx;
}

int getCDKSelectionCurrent (CDKSELECTION *selection)
{
  return selection->currentItem;
}

/*
 * methods for generic type methods
 */
static void _focusCDKSelection(CDKOBJS *object)
{
   CDKSELECTION *selection = (CDKSELECTION *)object;

   drawCDKSelectionList (selection, ObjOf(selection)->box);
}

static void _unfocusCDKSelection(CDKOBJS *object)
{
   CDKSELECTION *selection = (CDKSELECTION *)object;

   drawCDKSelectionList (selection, ObjOf(selection)->box);
}

dummyRefreshData(Selection)

dummySaveData(Selection)

static int createList(CDKSELECTION *selection, char **list, int listSize)
{
   int status = 0;
   int widestItem = 0;
   int x;

   if (listSize > 0)
   {
      chtype **	newList = typeCallocN(chtype *, listSize + 1);
      int *	newLen  = typeCallocN(int, listSize + 1);
      int *	newPos  = typeCallocN(int, listSize + 1);
      int *	newSel  = typeCallocN(int, listSize + 1);
      int *	newMode = typeCallocN(int, listSize + 1);

      if (newList != 0
       && newLen != 0
       && newPos != 0
       && newSel != 0
       && newMode != 0)
      {
	 int boxWidth = (selection->boxWidth - selection->maxchoicelen - 2 * BorderOf(selection));
	 int adjust = selection->maxchoicelen + BorderOf(selection);

	 status = 1;
	 for (x = 0; x < listSize; x++)
	 {
	    newList[x] = char2Chtype (list[x], &newLen[x], &newPos[x]);
	    if (newList[x] == 0)
	    {
	       status = 0;
	       break;
	    }
	    newPos[x]  = justifyString (boxWidth, newLen[x], newPos[x]) + adjust;
	    widestItem = MAXIMUM (widestItem, newLen[x]);
	 }

	 if (status)
	 {
	    CDKfreeChtypes (selection->item);
	    freeChecked (selection->itemPos);
	    freeChecked (selection->itemLen);
	    freeChecked (selection->selections);
	    freeChecked (selection->mode);

	    selection->item	  = newList;
	    selection->itemPos	  = newPos;
	    selection->itemLen	  = newLen;
	    selection->selections = newSel;
	    selection->mode	  = newMode;
	 }
	 else
	 {
	    CDKfreeChtypes (newList);
	    freeChecked (newPos);
	    freeChecked (newLen);
	    freeChecked (newSel);
	    freeChecked (newMode);
	 }
      }
   }

   return status ? widestItem : 0;
}
