#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2003/12/06 16:47:20 $
 * $Revision: 1.104 $
 */

/*
 * Declare file local prototypes.
 */
static void drawCDKScrollList (CDKSCROLL *scrollp, boolean Box);
static int createCDKScrollItemList (CDKSCROLL *scrollp, boolean numbers,
				    char **list, int listSize);

#define NUMBER_FMT      "%4d. %s"
#define NUMBER_LEN(s)   (8 + strlen(s))

DeclareCDKObjects(SCROLL, Scroll, setCdk, Int);

/*
 * This function creates a new scrolling list widget.
 */
CDKSCROLL *newCDKScroll (CDKSCREEN *cdkscreen, int xplace, int yplace, int splace, int height, int width, char *title, char **list, int listSize, boolean numbers, chtype highlight, boolean Box, boolean shadow)
{
   CDKSCROLL *scrollp		= 0;
   int parentWidth		= getmaxx(cdkscreen->window);
   int parentHeight		= getmaxy(cdkscreen->window);
   int boxWidth			= width;
   int boxHeight		= height;
   int xpos			= xplace;
   int ypos			= yplace;
   int scrollAdjust		= 0;
   int listWinHeight;

   if (listSize <= 0
    || (scrollp = newCDKObject(CDKSCROLL, &my_funcs)) == 0)
   {
      destroyCDKObject(scrollp);
      return (0);
   }

   setCDKScrollBox (scrollp, Box);

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

   boxWidth = setCdkTitle(ObjOf(scrollp), title, boxWidth);

   /* Set the box height. */
   if (TitleLinesOf(scrollp) > boxHeight)
   {
      if (listSize > 8)
      {
	 boxHeight = TitleLinesOf(scrollp) + 8 + 2*BorderOf(scrollp);
      }
      else
      {
	 boxHeight = TitleLinesOf(scrollp) + listSize + 2*BorderOf(scrollp);
      }
   }

   /* Set the rest of the variables. */
   scrollp->titleAdj	= TitleLinesOf(scrollp) + BorderOf(scrollp);
   scrollp->listSize	= listSize;
   listWinHeight        = boxHeight - (2*BorderOf(scrollp) + TitleLinesOf(scrollp));
   scrollp->viewSize	= listWinHeight;
   scrollp->lastItem	= listSize - 1;
   scrollp->maxTopItem	= listSize - scrollp->viewSize;

   /* Is the view size larger than the list? */
   if (listSize < scrollp->viewSize)
   {
      scrollp->viewSize		= listSize;
      scrollp->listSize		= listSize;
      scrollp->lastItem		= listSize;
      scrollp->maxTopItem	= -1;
   }

   /* Adjust the box width if there is a scrollp bar. */
   if ((splace == LEFT) || (splace == RIGHT))
   {
      scrollp->scrollbar = TRUE;
      boxWidth += 1;
   }
   else
   {
      scrollp->scrollbar = FALSE;
   }

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth		= (boxWidth > parentWidth ? (parentWidth - scrollAdjust) : boxWidth);
   boxHeight		= (boxHeight > parentHeight ? parentHeight : boxHeight);

   /* Determine the size of the scrollbar toggle and the step. */
   scrollp->step	= (float)(listWinHeight) / (float)scrollp->listSize;
   scrollp->toggleSize	= (scrollp->listSize > listWinHeight ? 1 : ceilCDK(scrollp->step));

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the scrolling window */
   scrollp->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the scrolling window null?? */
   if (scrollp->win == 0)
   {
      destroyCDKObject(scrollp);
      return (0);
   }

   /* Turn the keypad on for the window. */
   keypad (scrollp->win, TRUE);

   /* Create the scrollbar window. */
   if (splace == RIGHT)
   {
      scrollp->scrollbarWin = subwin (scrollp->win,
					listWinHeight, 1,
					ypos + scrollp->titleAdj,
					xpos + boxWidth - BorderOf(scrollp) - 1);
   }
   else if (splace == LEFT)
   {
      scrollp->scrollbarWin = subwin (scrollp->win,
					listWinHeight, 1,
					ypos + scrollp->titleAdj,
					xpos + BorderOf(scrollp));
   }
   else
   {
      scrollp->scrollbarWin = 0;
   }

   /* create the list window */

   scrollp->listWin = subwin(scrollp->win,
			     listWinHeight,
			     boxWidth - 2*BorderOf(scrollp) - scrollAdjust,
			     ypos + scrollp->titleAdj,
			     xpos + BorderOf(scrollp) + (splace == LEFT ? 1 : 0));

   /* Set the rest of the variables */
   ScreenOf(scrollp)		= cdkscreen;
   scrollp->parent		= cdkscreen->window;
   scrollp->shadowWin		= 0;
   scrollp->boxHeight		= boxHeight;
   scrollp->boxWidth		= boxWidth;
   scrollp->scrollbarPlacement	= splace;
   scrollp->maxLeftChar		= 0;
   scrollp->currentTop		= 0;
   scrollp->currentItem		= 0;
   scrollp->currentHigh		= 0;
   scrollp->leftChar		= 0;
   scrollp->highlight		= highlight;
   scrollp->exitType		= vNEVER_ACTIVATED;
   ObjOf(scrollp)->acceptsFocus = TRUE;
   ObjOf(scrollp)->inputWindow  = scrollp->win;
   scrollp->shadow		= shadow;
   scrollp->preProcessFunction	= 0;
   scrollp->preProcessData	= 0;
   scrollp->postProcessFunction = 0;
   scrollp->postProcessData	= 0;

   /* Create the scrolling list item list and needed variables. */
   if (createCDKScrollItemList (scrollp, numbers, list, listSize) <= 0)
   {
      destroyCDKObject(scrollp);
      return (0);
   }

   /* Do we need to create a shadow? */
   if (shadow)
   {
      scrollp->shadowWin = newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
   }

   /* Clean the key bindings. */
   cleanCDKObjectBindings (vSCROLL, scrollp);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vSCROLL, scrollp);

   /* Return the scrolling list */
   return scrollp;
}

/*
 * This actually does all the 'real' work of managing the scrolling list.
 */
int activateCDKScroll (CDKSCROLL *scrollp, chtype *actions)
{
   /* Draw the scrolling list */
   drawCDKScroll (scrollp, ObjOf(scrollp)->box);

   /* Check if actions is null. */
   if (actions == 0)
   {
      chtype input;
      int ret;

      for (;;)
      {
	 /* Get the input. */
	 input = getcCDKObject (ObjOf(scrollp));

	 /* Inject the character into the widget. */
	 ret = injectCDKScroll (scrollp, input);
	 if (scrollp->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }
   else
   {
      int length = chlen (actions);
      int i = 0;
      int ret;

      /* Inject each character one at a time. */
      for (i=0; i < length; i++)
      {
	 ret = injectCDKScroll (scrollp, actions[i]);
	 if (scrollp->exitType != vEARLY_EXIT)
	    return ret;
      }
   }

   /* Set the exit type for the widget and return. */
   scrollp->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This injects a single character into the widget.
 */
static int _injectCDKScroll (CDKOBJS *object, chtype input)
{
   CDKSCROLL *scrollp = (CDKSCROLL *)object;
   int ppReturn = 1;
   int ret = unknownInt;
   bool complete = FALSE;

   /* Set the exit type for the widget. */
   scrollp->exitType = vEARLY_EXIT;

   /* Draw the scrolling list */
   drawCDKScrollList (scrollp, ObjOf(scrollp)->box);

   /* Check if there is a pre-process function to be called. */
   if (scrollp->preProcessFunction != 0)
   {
      /* Call the pre-process function. */
      ppReturn = scrollp->preProcessFunction (vSCROLL, scrollp, scrollp->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a predefined key binding. */
      if (checkCDKObjectBind (vSCROLL, scrollp, input) != 0)
      {
	 scrollp->exitType = vESCAPE_HIT;
	 complete = TRUE;
      }
      else
      {
	 switch (input)
	 {
	    case KEY_UP :
		 if (scrollp->listSize > 0)
		 {
		    if (scrollp->currentItem > 0)
		    {
		       if (scrollp->currentHigh == 0)
		       {
			  if (scrollp->currentTop != 0)
			  {
			     scrollp->currentTop--;
			     scrollp->currentItem--;
			  }
			  else
			  {
			     Beep();
			  }
		       }
		       else
		       {
			  scrollp->currentItem--;
			  scrollp->currentHigh--;
		       }
		    }
		    else
		    {
		       Beep();
		    }
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_DOWN :
		 if (scrollp->listSize > 0)
		 {
		    if (scrollp->currentItem < scrollp->listSize-1)
		    {
		       if (scrollp->currentHigh == scrollp->viewSize - 1)
		       {
			  /* We need to scroll down one line. */
			  if (scrollp->currentTop < scrollp->maxTopItem)
			  {
			     scrollp->currentTop++;
			     scrollp->currentItem++;
			  }
			  else
			  {
			     Beep();
			  }
		       }
		       else
		       {
			  scrollp->currentItem++;
			  scrollp->currentHigh++;
		       }
		    }
		    else
		    {
		       Beep();
		    }
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_RIGHT :
		 if (scrollp->listSize > 0)
		 {
		    if (scrollp->leftChar >= scrollp->maxLeftChar)
		    {
		       Beep();
		    }
		    else
		    {
		       scrollp->leftChar ++;
		    }
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_LEFT :
		 if (scrollp->listSize > 0)
		 {
		    if (scrollp->leftChar == 0)
		    {
		       Beep();
		    }
		    else
		    {
		       scrollp->leftChar --;
		    }
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_PPAGE : case CONTROL('B') :
		 if (scrollp->listSize > 0)
		 {
		    if (scrollp->currentTop > 0)
		    {
		       if (scrollp->currentTop >= (scrollp->viewSize -1))
		       {
			  scrollp->currentTop	-= (scrollp->viewSize - 1);
			  scrollp->currentItem	-= (scrollp->viewSize - 1);
		       }
		       else
		       {
			  scrollp->currentTop	= 0;
			  scrollp->currentItem	= 0;
			  scrollp->currentHigh	= 0;
		       }
		    }
		    else
		    {
		       Beep();
		    }
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_NPAGE : case CONTROL('F') :
		 if (scrollp->listSize > 0)
		 {
		    if (scrollp->currentTop < scrollp->maxTopItem)
		    {
		       if ((scrollp->currentTop + scrollp->viewSize - 1) <= scrollp->maxTopItem)
		       {
			  scrollp->currentTop	+= (scrollp->viewSize - 1);
			  scrollp->currentItem	+= (scrollp->viewSize - 1);
		       }
		       else
		       {
			  scrollp->currentTop	= scrollp->maxTopItem;
			  scrollp->currentItem	= scrollp->lastItem;
			  scrollp->currentHigh	= scrollp->viewSize-1;
		       }
		    }
		    else
		    {
		       Beep();
		    }
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case 'g' : case '1' : case KEY_HOME :
		 scrollp->currentTop	= 0;
		 scrollp->currentItem	= 0;
		 scrollp->currentHigh	= 0;
		 break;

	    case 'G' : case KEY_END :
		 scrollp->currentTop	= scrollp->maxTopItem;
		 scrollp->currentItem	= scrollp->lastItem;
		 scrollp->currentHigh	= scrollp->viewSize-1;
		 break;

	    case '$' :
		 scrollp->leftChar	= scrollp->maxLeftChar;
		 break;

	    case '|' :
		 scrollp->leftChar	= 0;
		 break;

	    case KEY_ESC :
		 scrollp->exitType = vESCAPE_HIT;
		 complete = TRUE;
		 break;

	    case CDK_REFRESH :
		 eraseCDKScreen (ScreenOf(scrollp));
		 refreshCDKScreen (ScreenOf(scrollp));
		 break;

	    case KEY_RETURN : case KEY_TAB : case KEY_ENTER :
		 scrollp->exitType = vNORMAL;
		 ret = scrollp->currentItem;
		 complete = TRUE;
		 break;

	    default :
		 break;
	 }
      }

      /* Should we call a post-process? */
      if (!complete && (scrollp->postProcessFunction != 0))
      {
	 scrollp->postProcessFunction (vSCROLL, scrollp, scrollp->postProcessData, input);
      }
   }

   if (!complete) {
      drawCDKScrollList (scrollp, ObjOf(scrollp)->box);
      scrollp->exitType = vEARLY_EXIT;
   }

   ResultOf(scrollp).valueInt = ret;
   return (ret != unknownInt);
}

/*
 * This allows the user to accelerate to a position in the scrolling list.
 */
void setCDKScrollPosition (CDKSCROLL *scrollp, int item)
{
   if (item < 0)
   {
      scrollp->currentTop	= 0;
      scrollp->currentItem	= 0;
      scrollp->currentHigh	= 1;
   }
   else if (item > scrollp->listSize-1)
   {
      scrollp->currentTop	= scrollp->maxTopItem;
      scrollp->currentItem	= scrollp->lastItem;
      scrollp->currentHigh	= scrollp->viewSize-1;
   }
   else
   {
      scrollp->currentTop	= item;
      scrollp->currentItem	= item;
      scrollp->currentHigh	= 0;
   }
}

int getCDKScrollCurrent (CDKSCROLL *scrollp)
{
  return scrollp->currentItem;
}


void setCDKScrollCurrent (CDKSCROLL *s, int i)
{
  s->currentItem = i;
  if (i < s->currentTop)
    s->currentTop = i;
  else if (i >= s->currentTop + s->viewSize)
    s->currentTop = s->maxTopItem;

  s->currentHigh = i - s->currentTop;
}

/*
 * This moves the scroll field to the given location.
 */
static void _moveCDKScroll (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKSCROLL *scrollp = (CDKSCROLL *)object;
   int currentX = getbegx(scrollp->win);
   int currentY = getbegy(scrollp->win);
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
      xpos = getbegx(scrollp->win) + xplace;
      ypos = getbegy(scrollp->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(scrollp), &xpos, &ypos, scrollp->boxWidth, scrollp->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(scrollp->win, -xdiff, -ydiff);
   moveCursesWindow(scrollp->listWin, -xdiff, -ydiff);
   moveCursesWindow(scrollp->shadowWin, -xdiff, -ydiff);
   moveCursesWindow(scrollp->scrollbarWin, -xdiff, -ydiff);

   /* Touch the windows so they 'move'. */
   touchwin (WindowOf(scrollp));
   wrefresh (WindowOf(scrollp));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKScroll (scrollp, ObjOf(scrollp)->box);
   }
}

/*
 * This function draws the scrolling list widget.
 */
static void _drawCDKScroll (CDKOBJS *object, boolean Box)
{
   CDKSCROLL *scrollp = (CDKSCROLL *)object;

   /* Draw in the shadow if we need to. */
   if (scrollp->shadowWin != 0)
      drawShadow (scrollp->shadowWin);

   drawCdkTitle (scrollp->win, object);

   /* Draw in the scolling list items. */
   drawCDKScrollList (scrollp, Box);
}

static void drawCDKScrollCurrent(CDKSCROLL *s)
{
   /* Rehighlight the current menu item. */
   int screenPos = s->itemPos[s->currentItem] - s->leftChar;
   chtype highlight = HasFocusObj(s) ? s->highlight : A_NORMAL;

   if (screenPos >= 0)
   {
      writeChtypeAttrib (s->listWin,
			 screenPos,
			 s->currentHigh,
			 s->item[s->currentItem],
			 highlight,
			 HORIZONTAL, 0,
			 s->itemLen[s->currentItem]);
   }
   else
   {
      writeChtypeAttrib (s->listWin,
			 0,
			 s->currentHigh,
			 s->item[s->currentItem],
			 highlight,
			 HORIZONTAL,
			 s->leftChar - s->itemPos[s->currentTop] + 1,
			 s->itemLen[s->currentItem]);
   }
}

/*
 * This redraws the scrolling list.
 */
static void drawCDKScrollList (CDKSCROLL *scrollp, boolean Box)
{
   int screenPos = 0;
   int i;

   /* If the list is empty, don't draw anything. */
   if (scrollp->listSize < 1)
   {
      /* Box it if needed. */
      if (Box) {
	  drawObjBox (scrollp->win, ObjOf(scrollp));
      }

      /* Refresh the window. */
      touchwin (scrollp->win);
      wrefresh (scrollp->win);
      return;
   }
  
   /* Redraw the list */
   for (i=0; i < scrollp->viewSize; i++)
   {
      int j = i + scrollp->currentTop;

      /* Draw the elements in the scrolling list. */
      if (j < scrollp->listSize)
      {
	 screenPos = scrollp->itemPos[j] - scrollp->leftChar;

	 /* Draw in an empty line. */
	 writeBlanks (scrollp->listWin, 0, i, HORIZONTAL, 0, scrollp->boxWidth-2);

	 /* Write in the correct line. */
	 if (screenPos >= 0)
	 {
	      writeChtype (scrollp->listWin,
			   screenPos, i,
			   scrollp->item[j],
			   HORIZONTAL, 0,
			   scrollp->itemLen[j]);
	 }
	 else
	 {
	      writeChtype (scrollp->listWin,
			   0, i,
			   scrollp->item[j],
			   HORIZONTAL,
			   scrollp->leftChar - scrollp->itemPos[j]+1,
			   scrollp->itemLen[j]);
	 }
      }
      else
      {
	 /* Draw in an empty line. */
	  writeBlanks (scrollp->listWin, 0, i, HORIZONTAL, 0, scrollp->boxWidth-2);
      }
   }

   drawCDKScrollCurrent(scrollp);

   /* Determine where the toggle is supposed to be. */
   if (scrollp->scrollbarWin != (WINDOW *)NULL)
   {
      if (scrollp->listSize > getmaxy(scrollp->scrollbarWin))
	 scrollp->togglePos = floorCDK(scrollp->currentItem * scrollp->step);
      else
	 scrollp->togglePos = ceilCDK(scrollp->currentItem * scrollp->step);

      /* Make sure the toggle button doesn't go out of bounds. */
      
      if (scrollp->togglePos >= getmaxy(scrollp->scrollbarWin))
	 scrollp->togglePos = getmaxy(scrollp->scrollbarWin) - 1;

      /* Draw the scrollbar. */
      mvwvline (scrollp->scrollbarWin, 0, 0, ACS_CKBOARD, getmaxy(scrollp->scrollbarWin)); 
      mvwvline (scrollp->scrollbarWin, scrollp->togglePos, 0, ' ' | A_REVERSE, scrollp->toggleSize); 
   }

   /* Box it if needed. */
   if (Box) {
      drawObjBox (scrollp->win, ObjOf(scrollp));
   }

   /* Refresh the window. */
   touchwin (scrollp->win);
   wrefresh (scrollp->win);
}

/*
 * This sets the background color of the widget.
 */
void setCDKScrollBackgroundColor (CDKSCROLL *scrollp, char *color)
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
   setCDKScrollBackgroundAttrib (scrollp, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This sets the background attribute of the widget.
 */
void setCDKScrollBackgroundAttrib (CDKSCROLL *scrollp, chtype attrib)
{
   /* Set the widgets background attribute. */
   wbkgd (scrollp->win, attrib);
   wbkgd (scrollp->listWin, attrib);
   if (scrollp->scrollbarWin != 0)
   {
      wbkgd (scrollp->scrollbarWin, attrib);
   }
}

/*
 * This function destroys
 */
static void _destroyCDKScroll (CDKOBJS *object)
{
   if (object != 0)
   {
      CDKSCROLL *scrollp = (CDKSCROLL *)object;

      cleanCdkTitle (object);
      CDKfreeChtypes (scrollp->item);
      freeChecked (scrollp->itemPos);
      freeChecked (scrollp->itemLen);

      /* Clean up the windows. */
      deleteCursesWindow (scrollp->scrollbarWin);
      deleteCursesWindow (scrollp->shadowWin);
      deleteCursesWindow (scrollp->listWin);
      deleteCursesWindow (scrollp->win);

      /* Unregister this object. */
      unregisterCDKObject (vSCROLL, scrollp);
   }
}

/*
 * This function erases the scrolling list from the screen.
 */
static void _eraseCDKScroll (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKSCROLL *scrollp = (CDKSCROLL *)object;

      eraseCursesWindow (scrollp->win);
      eraseCursesWindow (scrollp->shadowWin);
   }
}

/*
 * This function creates the scrolling list information and sets up the needed
 * variables for the scrolling list to work correctly.
 */
static int createCDKScrollItemList (CDKSCROLL *scrollp, boolean numbers, char **list, int listSize)
{
   int status = 0;

   if (listSize > 0)
   {
      int widestItem		= 0;
      int x			= 0;
      unsigned have		= 256;
      char *temp		= malloc(have);
      chtype ** newList		= typeCallocN(chtype *, listSize + 1);
      int * newLen		= typeCallocN(int, listSize + 1);
      int * newPos		= typeCallocN(int, listSize + 1);

      if (temp != 0
       && newList != 0
       && newLen != 0
       && newPos != 0)
      {
	 /* Create the items in the scrolling list. */
	 status = 1;
	 for (x=0 ; x < listSize; x++)
	 {
	    char *value = list[x];
	    if (numbers)
	    {
	       unsigned need = NUMBER_LEN(value);
	       if (need > have)
	       {
		  temp = realloc(temp, have = (need * 2) / 3); 
		  if (temp == 0)
		  {
		     status = 0;
		     break;
		  }
	       }
	       sprintf (temp, NUMBER_FMT, x + 1, value);
	       value = temp;
	    }
	    
	    newList[x] = char2Chtype (value, &newLen[x], &newPos[x]);
	    if (newList[x] == 0)
	    {
	       status = 0;
	       break;
	    }
	    newPos[x]  = justifyString (scrollp->boxWidth, newLen[x], newPos[x]);
	    widestItem = MAXIMUM (newLen[x], widestItem);
	 }
	 free (temp);

	 if (status)
	 {
	    CDKfreeChtypes (scrollp->item);
	    freeChecked (scrollp->itemPos);
	    freeChecked (scrollp->itemLen);

	    scrollp->item = newList;
	    scrollp->itemPos = newPos;
	    scrollp->itemLen = newLen;

	    /* Determine how many characters we can shift to the right */
	    /* before all the items have been scrolled off the screen. */
	    if (scrollp->boxWidth > widestItem)
	    {
	       scrollp->maxLeftChar = 0;
	    }
	    else
	    {
	       scrollp->maxLeftChar = widestItem - (scrollp->boxWidth - 2);
	    }

	    /* Keep the boolean flag 'numbers' */
	    scrollp->numbers = numbers;
	 }
	 else
	 {
	    CDKfreeChtypes (newList);
	    freeChecked (newPos);
	    freeChecked (newLen);
	 }
      }
   }
   return status;
}

/*
 * This sets certain attributes of the scrolling list.
 */
void setCDKScroll (CDKSCROLL *scrollp, char **list, int listSize, boolean numbers, chtype highlight, boolean Box)
{
   setCDKScrollItems (scrollp, list, listSize, numbers);
   setCDKScrollHighlight (scrollp, highlight);
   setCDKScrollBox (scrollp, Box);
}

/*
 * This sets the scrolling list items.
 */
void setCDKScrollItems (CDKSCROLL *scrollp, char **list, int listSize, boolean numbers)
{
   int x = 0;

   if (createCDKScrollItemList (scrollp, numbers, list, listSize) <= 0)
      return;

   /* Clean up the display. */
   for (x=0; x < scrollp->viewSize ; x++)
   {
      writeBlanks (scrollp->win, 1, scrollp->titleAdj + x,
		   HORIZONTAL, 0, scrollp->boxWidth-2);
   }

   /* Readjust all of the variables ... */
   scrollp->listSize	= listSize;
   scrollp->viewSize	= scrollp->boxHeight - (2 + TitleLinesOf(scrollp));
   scrollp->lastItem	= listSize - 1;
   scrollp->maxTopItem	= listSize - scrollp->viewSize;

   /* Is the view size smaller then the window? */
   if (listSize < (scrollp->boxHeight-1-scrollp->titleAdj))
   {
      scrollp->viewSize		= listSize;
      scrollp->listSize		= listSize;
      scrollp->lastItem		= listSize;
      scrollp->maxTopItem	= -1;
   }

   /* Set some vars. */
   scrollp->currentTop	= 0;
   scrollp->currentItem = 0;
   scrollp->currentHigh = 0;
   scrollp->leftChar	= 0;

   /* Set the information for the scroll bar. */
   scrollp->step	= (float)(scrollp->boxHeight-2) / (float)scrollp->listSize;
   scrollp->toggleSize	= (scrollp->listSize > (scrollp->boxHeight-2) ? 1 : ceilCDK(scrollp->step));
}
int getCDKScrollItems (CDKSCROLL *scrollp, char *list[])
{
   int x;

   for (x=0; x < scrollp->listSize; x++)
   {
      list[x] = chtype2Char (scrollp->item[x]);
   }
   return scrollp->listSize;
}

/*
 * This sets the highlight of the scrolling list.
 */
void setCDKScrollHighlight (CDKSCROLL *scrollp, chtype highlight)
{
   scrollp->highlight = highlight;
}
chtype getCDKScrollHighlight (CDKSCROLL *scrollp, chtype highlight GCC_UNUSED)
{
   return scrollp->highlight;
}

/*
 * This sets the box attribute of the scrolling list.
 */
void setCDKScrollBox (CDKSCROLL *scrollp, boolean Box)
{
   ObjOf(scrollp)->box = Box;
   ObjOf(scrollp)->borderSize = Box ? 1 : 0;
}
boolean getCDKScrollBox (CDKSCROLL *scrollp)
{
   return ObjOf(scrollp)->box;
}

/*
 * This adds a single item to a scrolling list.
 */
void addCDKScrollItem (CDKSCROLL *scrollp, char *item)
{
   int itemNumber = scrollp->listSize;
   int widestItem = scrollp->maxLeftChar + (scrollp->boxWidth-2);
   char *temp = 0;
   char *value = item;

   /*
    * If the scrolling list has numbers, then add the new item
    * with a numeric value attached.
    */
   if (scrollp->numbers)
   {
      temp = malloc(NUMBER_LEN(value));
      sprintf (temp, NUMBER_FMT, itemNumber + 1, value);
      value = temp;
   }
   scrollp->item[itemNumber] = char2Chtype (value, &scrollp->itemLen[itemNumber], &scrollp->itemPos[itemNumber]);
   scrollp->itemPos[itemNumber] = justifyString (scrollp->boxWidth, scrollp->itemLen[itemNumber], scrollp->itemPos[itemNumber]);
   freeChar(temp);

   /* Determine the size of the widest item. */
   widestItem = MAXIMUM (scrollp->itemLen[itemNumber], widestItem);
   if (scrollp->boxWidth > widestItem)
   {
      scrollp->maxLeftChar = 0;
   }
   else
   {
      scrollp->maxLeftChar = widestItem - (scrollp->boxWidth-2);
   }

   /* Increment the list size. */
   scrollp->listSize++;
   if (scrollp->listSize < scrollp->viewSize)
   {
      scrollp->lastItem = scrollp->listSize;
      scrollp->maxTopItem = -1;
   }
   else
   {
      scrollp->lastItem = scrollp->listSize - 1;
      scrollp->maxTopItem = scrollp->listSize - scrollp->viewSize;
   }

   /* Reset some variables. */
   scrollp->currentHigh = 0;
   scrollp->currentTop	= 0;
   scrollp->currentItem = 0;
   scrollp->leftChar	= 0;
}

/*
 * This adds a single item to a scrolling list.
 */
void deleteCDKScrollItem (CDKSCROLL *scrollp, int position)
{
   int x;

   /* Nuke the current item. */
   freeChtype (scrollp->item[position]);

   /* Adjust the list. */
   for (x=position; x < scrollp->listSize-1; x++)
   {
      scrollp->item[x]		= scrollp->item[x + 1];
      scrollp->itemLen[x]	= scrollp->itemLen[x + 1];
      scrollp->itemPos[x]	= scrollp->itemPos[x + 1];
   }
   scrollp->listSize--;

   /* Did we just delete the last element? */
   if (scrollp->listSize == 0)
   {
      scrollp->lastItem		= -1;
      scrollp->maxTopItem	= -1;
      scrollp->currentHigh	= -1;
      scrollp->currentItem	= -1;
      scrollp->currentTop	= -1;
   }
   else
   {
      scrollp->lastItem--;
      scrollp->maxTopItem--;
      scrollp->currentHigh = 0;
      scrollp->currentItem = 0;
      scrollp->currentTop = 0;
   }
}

/*
 * This function sets the pre-process function.
 */
void setCDKScrollPreProcess (CDKSCROLL *scrollp, PROCESSFN callback, void *data)
{
   scrollp->preProcessFunction = callback;
   scrollp->preProcessData = data;
}

/*
 * This function sets the post-process function.
 */
void setCDKScrollPostProcess (CDKSCROLL *scrollp, PROCESSFN callback, void *data)
{
   scrollp->postProcessFunction = callback;
   scrollp->postProcessData = data;
}

static void _focusCDKScroll(CDKOBJS *object)
{
   CDKSCROLL *scrollp = (CDKSCROLL *)object;

   curs_set(0);
   HasFocusObj(scrollp) = TRUE;
   drawCDKScrollCurrent(scrollp);
   wrefresh (scrollp->listWin);
}

static void _unfocusCDKScroll(CDKOBJS *object)
{
   CDKSCROLL *scrollp = (CDKSCROLL *)object;

   curs_set(1);
   HasFocusObj(scrollp) = FALSE;
   drawCDKScrollCurrent(scrollp);
   wrefresh (scrollp->listWin);
}

static void _refreshDataCDKScroll(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _saveDataCDKScroll(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}
