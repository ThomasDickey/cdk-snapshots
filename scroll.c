#include "cdk.h"
#include <limits.h>
   
/*
 * $Author: tom $
 * $Date: 1999/05/23 02:53:30 $
 * $Revision: 1.74 $
 */

/*
 * Declare file local prototypes.
 */
static void drawCDKScrollList (CDKSCROLL *scrollp, boolean Box);
static void createCDKScrollItemList (CDKSCROLL *scrollp, boolean numbers,
				char **list, int listSize);

static CDKFUNCS my_funcs = {
    _drawCDKScroll,
    _eraseCDKScroll,
};

/*
 * This function creates a new scrolling list widget.
 */
CDKSCROLL *newCDKScroll (CDKSCREEN *cdkscreen, int xplace, int yplace, int splace, int height, int width, char *title, char **list, int listSize, boolean numbers, chtype highlight, boolean Box, boolean shadow)
{
   /* Declare local variables. */
   CDKSCROLL *scrollp		= newCDKObject(CDKSCROLL, &my_funcs);
   chtype *holder		= (chtype *)NULL;
   int parentWidth		= getmaxx(cdkscreen->window) - 1;
   int parentHeight		= getmaxy(cdkscreen->window) - 1;
   int boxWidth			= width;
   int boxHeight		= height;
   int maxWidth			= INT_MIN;
   int xpos			= xplace;
   int ypos			= yplace;
   int scrollAdjust		= 0;
   char *temp[MAX_LINES];
   int x, len, junk2;

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

   /* Translate the char * items to chtype * */
   if (title != (char *)NULL)
   {
      /* We need to split the title on \n. */
      scrollp->titleLines = splitString (title, temp, '\n');

      /* We need to determine the widest title line. */
      for (x=0; x < scrollp->titleLines; x++)
      {
         holder = char2Chtype (temp[x], &len, &junk2);
         maxWidth = MAXIMUM (maxWidth, len);
         freeChtype (holder);
      }
      boxWidth = MAXIMUM (boxWidth, maxWidth + 2);

      /* For each line in the title, convert from char * to chtype * */
      for (x=0; x < scrollp->titleLines; x++)
      {
         scrollp->title[x]	= char2Chtype (temp[x], &scrollp->titleLen[x], &scrollp->titlePos[x]);
         scrollp->titlePos[x]	= justifyString (boxWidth, scrollp->titleLen[x], scrollp->titlePos[x]);
         freeChar (temp[x]);
      }
   }
   else
   {
      /* No title? Set the required variables. */
      scrollp->titleLines = 0;
   }

   /* Set the box height. */
   if (scrollp->titleLines > boxHeight)
   {
      if (listSize > 8)
      {
         boxHeight = scrollp->titleLines + 10;
      }
      else
      {
         boxHeight = scrollp->titleLines + listSize + 2;
      }
   }

   /* Set the rest of the variables. */
   scrollp->titleAdj	= scrollp->titleLines + 1;
   scrollp->listSize	= listSize;
   scrollp->viewSize	= boxHeight - (2 + scrollp->titleLines); 
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
      scrollAdjust = 1;
      boxWidth += 1;
   }
   else
   {
      scrollp->scrollbar = FALSE;
   }

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth		= (boxWidth > parentWidth ? (parentWidth-scrollAdjust) : boxWidth);
   boxHeight		= (boxHeight > parentHeight ? parentHeight : boxHeight);

   /* Determine the size of the scrollbar toggle and the step. */
   scrollp->step	= (float)(boxHeight-2) / (float)scrollp->listSize;
   scrollp->toggleSize	= (scrollp->listSize > (boxHeight-2) ? 1 : ceilCDK(scrollp->step));

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the scrolling window */
   scrollp->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the scrolling window NULL?? */
   if (scrollp->win == (WINDOW *)NULL)
   {
      /* Clean up any memory. */
      for (x=0; x < scrollp->titleLines; x++)
      {
         freeChtype (scrollp->title[x]);
      }
      free(scrollp);

      /* Return a NULL pointer. */
      return ((CDKSCROLL *) NULL);
   }

   /* Turn the keypad on for the window. */
   keypad (scrollp->win, TRUE);

   /* Create the scrollbar window. */
   if (splace == RIGHT)
   {
      scrollp->scrollbarWin = subwin (scrollp->win,
					boxHeight-scrollp->titleAdj-1, 1,
					ypos + scrollp->titleAdj,
					xpos + boxWidth-2);
   }
   else if (splace == LEFT)
   {
      scrollp->scrollbarWin = subwin (scrollp->win,
					boxHeight, 1,
					getbegy(scrollp->win) + scrollp->titleAdj,
					getbegx(scrollp->win) + 1);
   }
   else
   {
      scrollp->scrollbarWin = (WINDOW *)NULL;
   }

   /* Set the rest of the variables */
   ScreenOf(scrollp)		= cdkscreen;
   scrollp->parent		= cdkscreen->window;
   scrollp->shadowWin		= (WINDOW *)NULL;
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
   ObjOf(scrollp)->box		= Box;
   scrollp->shadow		= shadow;
   scrollp->preProcessFunction	= (PROCESSFN)NULL;
   scrollp->preProcessData	= (void *)NULL;
   scrollp->postProcessFunction	= (PROCESSFN)NULL;
   scrollp->postProcessData	= (void *)NULL;
   scrollp->ULChar		= ACS_ULCORNER;
   scrollp->URChar		= ACS_URCORNER;
   scrollp->LLChar		= ACS_LLCORNER;
   scrollp->LRChar		= ACS_LRCORNER;
   scrollp->HChar		= ACS_HLINE;
   scrollp->VChar		= ACS_VLINE;
   scrollp->BoxAttrib		= A_NORMAL;

   /* Create the scrolling list item list and needed variables. */
   createCDKScrollItemList (scrollp, numbers, list, listSize);

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

   /* Check if actions is NULL. */
   if (actions == (chtype *)NULL)
   {
      /* Declare some local variables. */
      chtype input;
      int ret;

      for (;;)
      {
         /* Get the input. */
         input = wgetch (scrollp->win);

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
      /* Declare some local variables. */
      int length = chlen (actions);
      int x = 0;
      int ret;

      /* Inject each character one at a time. */
      for (x=0; x < length; x++)
      {
         ret = injectCDKScroll (scrollp, actions[x]);
         if (scrollp->exitType != vEARLY_EXIT)
         {
            return ret;
         }
      }
   }

   /* Set the exit type for the widget and return. */
   scrollp->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This injects a single character into the widget.
 */
int injectCDKScroll (CDKSCROLL *scrollp, chtype input)
{
   /* Declare local variables. */
   int ppReturn = 1;

   /* Set the exit type for the widget. */
   scrollp->exitType = vEARLY_EXIT;

   /* Draw the scrolling list */
   drawCDKScrollList (scrollp, ObjOf(scrollp)->box);

   /* Check if there is a pre-process function to be called. */
   if (scrollp->preProcessFunction != (PROCESSFN)NULL)
   {
      /* Call the pre-process function. */
      ppReturn = ((PROCESSFN)(scrollp->preProcessFunction)) (vSCROLL, scrollp, scrollp->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a predefined key binding. */
      if (checkCDKObjectBind (vSCROLL, scrollp, input) != 0)
      {
         scrollp->exitType = vESCAPE_HIT;
         return -1;
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
   
            case KEY_PPAGE : case '' :
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
   
            case KEY_NPAGE : case '' :
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
                 return -1;
      
            case CDK_REFRESH :
                 eraseCDKScreen (ScreenOf(scrollp));
                 refreshCDKScreen (ScreenOf(scrollp));
                 break;
      
            case KEY_RETURN : case KEY_TAB : case KEY_ENTER :
                 scrollp->exitType = vNORMAL;
                 return scrollp->currentItem;
      
            default :
                 break;
         }
      }

      /* Should we call a post-process? */
      if (scrollp->postProcessFunction != (PROCESSFN)NULL)
      {
         ((PROCESSFN)(scrollp->postProcessFunction)) (vSCROLL, scrollp, scrollp->postProcessData, input);
      }
   }
      
   /* Redraw the list */
   drawCDKScrollList (scrollp, ObjOf(scrollp)->box);

   /* Set the exit type and return. */
   scrollp->exitType = vEARLY_EXIT;
   return -1;
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

/*
 * This moves the scroll field to the given location.
 */
void moveCDKScroll (CDKSCROLL *scrollp, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   /* Declare local variables. */
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

   /* If there is a shadow box we have to move it too. */
   if (scrollp->shadowWin != (WINDOW *)NULL)
   {
      moveCursesWindow(scrollp->shadowWin, -xdiff, -ydiff);
   }

   /* If there is a scrollbar we have to move it too. */
   if (scrollp->scrollbar)
   {
      moveCursesWindow(scrollp->scrollbarWin, -xdiff, -ydiff);
   }

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
 * This allows the user to use the cursor keys to adjust the
 * position of the widget.
 */
void positionCDKScroll (CDKSCROLL *scrollp)
{
   /* Declare some variables. */
   int origX	= getbegx(scrollp->win);
   int origY	= getbegy(scrollp->win);
   chtype key	= (chtype)NULL;

   /* Let them move the widget around until they hit return. */
   while ((key != KEY_RETURN) && (key != KEY_ENTER))
   {
      key = wgetch (scrollp->win);
      if (key == KEY_UP || key == '8')
      {
         if (getbegy(scrollp->win) > 0)
         {
            moveCDKScroll (scrollp, 0, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_DOWN || key == '2')
      {
         if (getendy(scrollp->win) < getmaxy(WindowOf(scrollp))-1)
         {
            moveCDKScroll (scrollp, 0, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_LEFT || key == '4')
      {
         if (getbegx(scrollp->win) > 0)
         {
            moveCDKScroll (scrollp, -1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_RIGHT || key == '6')
      {
         if (getendx(scrollp->win) < getmaxx(WindowOf(scrollp))-1)
         {
            moveCDKScroll (scrollp, 1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '7')
      {
         if (getbegy(scrollp->win) > 0 && getbegx(scrollp->win) > 0)
         {
            moveCDKScroll (scrollp, -1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '9')
      {
         if (getendx(scrollp->win) < getmaxx(WindowOf(scrollp))-1
	  && getbegy(scrollp->win) > 0)
         {
            moveCDKScroll (scrollp, 1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '1')
      {
         if (getbegx(scrollp->win) > 0 && getendx(scrollp->win) < getmaxx(WindowOf(scrollp))-1)
         {
            moveCDKScroll (scrollp, -1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '3')
      {
         if (getendx(scrollp->win) < getmaxx(WindowOf(scrollp))-1
	  && getendy(scrollp->win) < getmaxy(WindowOf(scrollp))-1)
         {
            moveCDKScroll (scrollp, 1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '5')
      {
         moveCDKScroll (scrollp, CENTER, CENTER, FALSE, TRUE);
      }
      else if (key == 't')
      {
         moveCDKScroll (scrollp, getbegx(scrollp->win), TOP, FALSE, TRUE);
      }
      else if (key == 'b')
      {
         moveCDKScroll (scrollp, getbegx(scrollp->win), BOTTOM, FALSE, TRUE);
      }
      else if (key == 'l')
      {
         moveCDKScroll (scrollp, LEFT, getbegy(scrollp->win), FALSE, TRUE);
      }
      else if (key == 'r')
      {
         moveCDKScroll (scrollp, RIGHT, getbegy(scrollp->win), FALSE, TRUE);
      }
      else if (key == 'c')
      {
         moveCDKScroll (scrollp, CENTER, getbegy(scrollp->win), FALSE, TRUE);
      }
      else if (key == 'C')
      {
         moveCDKScroll (scrollp, getbegx(scrollp->win), CENTER, FALSE, TRUE);
      }
      else if (key == CDK_REFRESH)
      {
         eraseCDKScreen (ScreenOf(scrollp));
         refreshCDKScreen (ScreenOf(scrollp));
      }
      else if (key == KEY_ESC)
      {
         moveCDKScroll (scrollp, origX, origY, FALSE, TRUE);
      }
      else if ((key != KEY_RETURN) && (key != KEY_ENTER))
      {
         Beep();
      }
   }
}

/*
 * This function draws the scrolling list widget.
 */
void _drawCDKScroll (CDKOBJS *object, boolean Box)
{
   CDKSCROLL *scrollp = (CDKSCROLL *)object;
   int x;

   /* Draw in the shadow if we need to. */
   if (scrollp->shadowWin != (WINDOW *)NULL)
   {
      drawShadow (scrollp->shadowWin);
   }

   /* Draw in the title if there is one. */
   if (scrollp->titleLines != 0)
   {
      for (x=0; x < scrollp->titleLines; x++)
      {
         writeChtype (scrollp->win,
			scrollp->titlePos[x],
			x + 1,
			scrollp->title[x], 
			HORIZONTAL, 0,
			scrollp->titleLen[x]);
      }
   }

   /* Draw in the scolling list items. */
   drawCDKScrollList (scrollp, Box);
}

/*
 * This redraws the scrolling list.
 */
static void drawCDKScrollList (CDKSCROLL *scrollp, boolean Box)
{
   /* Declare some local vars */
   int scrollbarAdj	= 0;
   int screenPos	= 0;
   char emptyString[1000];
   int x;

   /*
    * If the scroll bar is on the left hand side, then adjust
    * everything over to the right one character.
     */
   if (scrollp->numbers == FALSE)
   {
      scrollbarAdj = (scrollp->scrollbarPlacement == LEFT ? 2 : 0);
   }
   else
   {
      scrollbarAdj = 1;
   }

   /* If the list is empty, don't draw anything. */
   if (scrollp->listSize < 1)
   {
      /* Box it if needed. */
      if (Box)
      {
         attrbox (scrollp->win,
			scrollp->ULChar, scrollp->URChar,
			scrollp->LLChar, scrollp->LRChar,
			scrollp->HChar,  scrollp->VChar,
			scrollp->BoxAttrib);
      }
   
      /* Refresh the window. */
      touchwin (scrollp->win);
      wrefresh (scrollp->win);
      return;
   }

   /* Create the empty string. */
   cleanChar (emptyString, scrollp->boxWidth-1, ' ');

   /* Redraw the list */
   for (x=0; x < scrollp->viewSize; x++)
   {
      /* Draw the elements in the scrolling list. */
      if (x < scrollp->listSize)
      {
         screenPos = scrollp->itemPos[x + scrollp->currentTop]-scrollp->leftChar + scrollbarAdj;

         /* Draw in an empty line. */
         writeChar (scrollp->win, 1, scrollp->titleAdj + x,
			emptyString, HORIZONTAL, 0, (int)strlen(emptyString));

         /* Write in the correct line. */
         if (screenPos >= 0)
         {
            writeChtype (scrollp->win,
			screenPos, x + scrollp->titleAdj,
			scrollp->item[x + scrollp->currentTop],
			HORIZONTAL, 0,
			scrollp->itemLen[x + scrollp->currentTop]);
         }
         else
         {
            writeChtype (scrollp->win,
			1, x + scrollp->titleAdj,
			scrollp->item[x + scrollp->currentTop],
			HORIZONTAL,
			scrollp->leftChar - scrollp->itemPos[scrollp->currentTop] + 1,
			scrollp->itemLen[x + scrollp->currentTop]);
         }
      }
      else 
      {
         /* Draw in an empty line. */
         writeChar (scrollp->win, 1, scrollp->titleAdj + x,
			emptyString, HORIZONTAL, 0, (int)strlen(emptyString));
      }
   }
   screenPos = scrollp->itemPos[scrollp->currentItem]-scrollp->leftChar + scrollbarAdj;

   /* Rehighlight the current menu item. */
   if (screenPos >= 0)
   {
      writeChtypeAttrib (scrollp->win,
			screenPos,
			scrollp->currentHigh + scrollp->titleAdj,
			scrollp->item[scrollp->currentItem],
			scrollp->highlight,
			HORIZONTAL, 0,
			scrollp->itemLen[scrollp->currentItem]);
   }
   else
   {
      writeChtypeAttrib (scrollp->win,
			1 + scrollbarAdj,
			scrollp->currentHigh + scrollp->titleAdj,
			scrollp->item[scrollp->currentItem],
			scrollp->highlight,
			HORIZONTAL,
			scrollp->leftChar - scrollp->itemPos[scrollp->currentTop] + 1,
			scrollp->itemLen[scrollp->currentItem]);
   }

   /* Determine where the toggle is supposed to be. */
   if (scrollp->scrollbarWin != (WINDOW *)NULL)
   {
      if (scrollp->listSize > scrollp->boxHeight-2)
      {
         scrollp->togglePos = floorCDK((float)scrollp->currentItem * (float)scrollp->step);
      }
      else
      {
         scrollp->togglePos = ceilCDK((float)scrollp->currentItem * (float)scrollp->step);
      }

      /* Make sure the toggle button doesn't go out of bounds. */
      scrollbarAdj = (scrollp->togglePos + scrollp->toggleSize)-(scrollp->boxHeight-scrollp->titleAdj-1);
      if (scrollbarAdj > 0)
      {
         scrollp->togglePos -= scrollbarAdj;
      }

      /* Draw the scrollbar. */
      for (x=0; x < scrollp->boxHeight-1; x++)
      {
         mvwaddch (scrollp->scrollbarWin, x, 0, ACS_CKBOARD);
      }
      for (x=scrollp->togglePos; x < scrollp->togglePos + scrollp->toggleSize; x++)
      {
         mvwaddch (scrollp->scrollbarWin, x, 0, ' ' | A_REVERSE);
      }
      touchwin (scrollp->scrollbarWin);
      wrefresh (scrollp->scrollbarWin);
   }

   /* Box it if needed. */
   if (Box)
   {
      attrbox (scrollp->win,
		scrollp->ULChar, scrollp->URChar,
		scrollp->LLChar, scrollp->LRChar,
		scrollp->HChar,  scrollp->VChar,
		scrollp->BoxAttrib);
      wrefresh (scrollp->win);
   }
   
   /* Refresh the window. */
   touchwin (scrollp->win);
   wrefresh (scrollp->win);
}

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKScrollULChar (CDKSCROLL *scrollp, chtype character)
{
   scrollp->ULChar = character;
}
void setCDKScrollURChar (CDKSCROLL *scrollp, chtype character)
{
   scrollp->URChar = character;
}
void setCDKScrollLLChar (CDKSCROLL *scrollp, chtype character)
{
   scrollp->LLChar = character;
}
void setCDKScrollLRChar (CDKSCROLL *scrollp, chtype character)
{
   scrollp->LRChar = character;
}
void setCDKScrollVerticalChar (CDKSCROLL *scrollp, chtype character)
{
   scrollp->VChar = character;
}
void setCDKScrollHorizontalChar (CDKSCROLL *scrollp, chtype character)
{
   scrollp->HChar = character;
}
void setCDKScrollBoxAttribute (CDKSCROLL *scrollp, chtype character)
{
   scrollp->BoxAttrib = character;
}

/*
 * This sets the background color of the widget.
 */ 
void setCDKScrollBackgroundColor (CDKSCROLL *scrollp, char *color)
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
   wbkgd (scrollp->win, holder[0]);
   if (scrollp->scrollbarWin != (WINDOW *)NULL)
   {
      wbkgd (scrollp->scrollbarWin, holder[0]);
   }

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This function destroys
 */
void destroyCDKScroll (CDKSCROLL *scrollp)
{
   /* Declare local variables. */
   int x;

   /* Erase the object. */
   eraseCDKScroll (scrollp);

   /* Clean up the char pointers. */
   for (x=0; x < scrollp->titleLines; x++)
   {
      freeChtype (scrollp->title[x]);
   }
   for (x=0; x < scrollp->listSize; x++)
   {
      freeChtype (scrollp->item[x]);
   }

   /* Clean up the windows. */
   deleteCursesWindow (scrollp->scrollbarWin);
   deleteCursesWindow (scrollp->shadowWin);
   deleteCursesWindow (scrollp->win);

   /* Unregister this object. */
   unregisterCDKObject (vSCROLL, scrollp);

   /* Finish cleaning up. */
   free (scrollp);
}

/*
 * This function erases the scrolling list from the screen.
 */
void _eraseCDKScroll (CDKOBJS *object)
{
   CDKSCROLL *scrollp = (CDKSCROLL *)object;

   eraseCursesWindow (scrollp->win);
   eraseCursesWindow (scrollp->shadowWin);
}

/*
 * This function creates massages the scrolling list information and 
 * sets up the needed variables fïr the scrolling list to work correctly.
 */
static void createCDKScrollItemList (CDKSCROLL *scrollp, boolean numbers, char **list, int listSize)
{
   /* Declare local variables. */
   int widestItem	= 0;
   int x		= 0;
   char temp[100];

   /* Create the items in the scrolling list. */
   if (numbers == NUMBERS)
   {
      for (x=0 ; x < listSize; x++)
      {
         sprintf (temp, "%4d. %s", x + 1, list[x]);
         scrollp->item[x]	= char2Chtype (temp, &scrollp->itemLen[x], &scrollp->itemPos[x]);
         scrollp->itemPos[x]	= justifyString (scrollp->boxWidth, scrollp->itemLen[x], scrollp->itemPos[x]);
         widestItem		= MAXIMUM (scrollp->itemLen[x], widestItem);
      }
   }
   else
   {
      for (x=0 ; x < listSize; x++)
      {
         scrollp->item[x]	= char2Chtype (list[x], &scrollp->itemLen[x], &scrollp->itemPos[x]);
         scrollp->itemPos[x]	= justifyString (scrollp->boxWidth, scrollp->itemLen[x], scrollp->itemPos[x]);
         widestItem		= MAXIMUM (scrollp->itemLen[x], widestItem);
      }
   }

   /* Determine how many characters we can shift to the right */
   /* before all the items have been scrolled off the screen. */
   if (scrollp->boxWidth > widestItem)
   {
      scrollp->maxLeftChar = 0;
   }
   else
   {
      scrollp->maxLeftChar = widestItem-(scrollp->boxWidth-2);
   }

   /* Keep the boolean flag 'numbers' */
   scrollp->numbers = numbers;
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
   /* Declare some wars. */
   char emptyString[2000];
   int x = 0;

   /* Clean out the old list. */
   for (x=0; x < scrollp->listSize; x++)
   {
      freeChtype (scrollp->item[x]);
      scrollp->itemLen[x] = 0;
      scrollp->itemPos[x] = 0;
   }

   /* Clean up the display. */
   cleanChar (emptyString, scrollp->boxWidth-1, ' ');
   for (x=0; x < scrollp->viewSize ; x++)
   {
      writeChar (scrollp->win, 1, scrollp->titleAdj + x, emptyString, 
			HORIZONTAL, 0, (int)strlen(emptyString));
   }

   /* Readjust all of the variables ... */
   scrollp->listSize	= listSize;
   scrollp->viewSize	= scrollp->boxHeight - (2 + scrollp->titleLines);
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
   scrollp->currentItem	= 0;
   scrollp->currentHigh	= 0;
   scrollp->leftChar	= 0;

   /* Set the information for the scroll bar. */
   scrollp->step	= (float)(scrollp->boxHeight-2) / (float)scrollp->listSize;
   scrollp->toggleSize	= (scrollp->listSize > (scrollp->boxHeight-2) ? 1 : ceilCDK(scrollp->step));

   /* Set up the new list. */
   createCDKScrollItemList (scrollp, numbers, list, listSize);
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
   /* Declare some local variables. */
   int itemNumber = scrollp->listSize;
   int widestItem = scrollp->maxLeftChar + (scrollp->boxWidth-2);
   char temp[256];

   /*
    * If the scrolling list has numbers, then add the new item 
    * with a numeric value attached.
     */
   if (scrollp->numbers == NUMBERS)
   {
      sprintf (temp, "%4d. %s", itemNumber + 1, item);
      scrollp->item[itemNumber] = char2Chtype (temp, &scrollp->itemLen[itemNumber], &scrollp->itemPos[itemNumber]);
      scrollp->itemPos[itemNumber] = justifyString (scrollp->boxWidth, scrollp->itemLen[itemNumber], scrollp->itemPos[itemNumber]);
   }
   else
   {
      scrollp->item[itemNumber] = char2Chtype (item, &scrollp->itemLen[itemNumber], &scrollp->itemPos[itemNumber]);
      scrollp->itemPos[itemNumber] = justifyString (scrollp->boxWidth, scrollp->itemLen[itemNumber], scrollp->itemPos[itemNumber]);
   }

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
   scrollp->currentHigh	= 0;
   scrollp->currentTop	= 0;
   scrollp->currentItem	= 0;
   scrollp->leftChar	= 0;
}

/*
 * This adds a single item to a scrolling list.
 */
void deleteCDKScrollItem (CDKSCROLL *scrollp, int position)
{
   /* Declare some local variables. */
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
