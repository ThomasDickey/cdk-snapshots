#include "cdk.h"
#include <limits.h>
   
/*
 * $Author: glovem $
 * $Date: 1999/01/04 19:06:15 $
 * $Revision: 1.60 $
 */

/*
 * Declare file local prototypes.
 */
void drawCDKScrollList (CDKSCROLL *scroll, boolean box);
void createCDKScrollItemList (CDKSCROLL *scroll, boolean numbers,
				char **list, int listSize);

/*
 * This function creates a new scrolling list widget.
 */
CDKSCROLL *newCDKScroll (CDKSCREEN *cdkscreen, int xplace, int yplace, int splace, int height, int width, char *title, char **list, int listSize, boolean numbers, chtype highlight, boolean box, boolean shadow)
{
   /* Declare local variables. */
   CDKSCROLL *scroll		= (CDKSCROLL *)malloc (sizeof (CDKSCROLL));
   chtype *holder		= (chtype *)NULL;
   int parentWidth		= WIN_WIDTH (cdkscreen->window);
   int parentHeight		= WIN_HEIGHT (cdkscreen->window);
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
      scroll->titleLines = splitString (title, temp, '\n');

      /* We need to determine the widest title line. */
      for (x=0; x < scroll->titleLines; x++)
      {
         holder = char2Chtype (temp[x], &len, &junk2);
         maxWidth = MAXIMUM (maxWidth, len);
         freeChtype (holder);
      }
      boxWidth = MAXIMUM (boxWidth, maxWidth+2);

      /* For each line in the title, convert from char * to chtype * */
      for (x=0; x < scroll->titleLines; x++)
      {
         scroll->title[x]	= char2Chtype (temp[x], &scroll->titleLen[x], &scroll->titlePos[x]);
         scroll->titlePos[x]	= justifyString (boxWidth, scroll->titleLen[x], scroll->titlePos[x]);
         freeChar (temp[x]);
      }
   }
   else
   {
      /* No title? Set the required variables. */
      scroll->titleLines = 0;
   }

   /* Set the box height. */
   if (scroll->titleLines > boxHeight)
   {
      if (listSize > 8)
      {
         boxHeight = scroll->titleLines + 10;
      }
      else
      {
         boxHeight = scroll->titleLines + listSize + 2;
      }
   }

   /* Set the rest of the variables. */
   scroll->titleAdj	= scroll->titleLines + 1;
   scroll->listSize	= listSize;
   scroll->viewSize	= boxHeight - (2 + scroll->titleLines); 
   scroll->lastItem	= listSize - 1;
   scroll->maxTopItem	= listSize - scroll->viewSize;

   /* Is the view size larger than the list? */
   if (listSize < scroll->viewSize)
   {
      scroll->viewSize		= listSize;
      scroll->listSize		= listSize;
      scroll->lastItem		= listSize;
      scroll->maxTopItem	= -1;
   }

   /* Adjust the box width if there is a scroll bar. */
   if ((splace == LEFT) || (splace == RIGHT))
   {
      scroll->scrollbar = TRUE;
      scrollAdjust = 1;
      boxWidth += 1;
   }
   else
   {
      scroll->scrollbar = FALSE;
   }

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth		= (boxWidth > parentWidth ? (parentWidth-scrollAdjust) : boxWidth);
   boxHeight		= (boxHeight > parentHeight ? parentHeight : boxHeight);

   /* Determine the size of the scrollbar toggle and the step. */
   scroll->step		= (float)(boxHeight-2) / (float)scroll->listSize;
   scroll->toggleSize	= (scroll->listSize > (boxHeight-2) ? 1 : (int)ceil(scroll->step));

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the scrolling window */
   scroll->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the scrolling window NULL?? */
   if (scroll->win == (WINDOW *)NULL)
   {
      /* Clean up any memory. */
      for (x=0; x < scroll->titleLines; x++)
      {
         freeChtype (scroll->title[x]);
      }
      free(scroll);

      /* Return a NULL pointer. */
      return ((CDKSCROLL *) NULL);
   }

   /* Turn the keypad on for the window. */
   keypad (scroll->win, TRUE);

   /* Create the scrollbar window. */
   if (splace == RIGHT)
   {
      scroll->scrollbarWin = subwin (scroll->win,
					boxHeight-scroll->titleAdj-1, 1,
					ypos+scroll->titleAdj,
					xpos+boxWidth-2);
   }
   else if (splace == LEFT)
   {
      scroll->scrollbarWin = subwin (scroll->win,
					boxHeight, 1,
					scroll->win->_begy+scroll->titleAdj,
					scroll->win->_begx+1);
   }
   else
   {
      scroll->scrollbarWin = (WINDOW *)NULL;
   }

   /* Set the rest of the variables */
   scroll->parent		= cdkscreen->window;
   scroll->shadowWin		= (WINDOW *)NULL;
   scroll->boxHeight		= boxHeight;
   scroll->boxWidth		= boxWidth;
   scroll->scrollbarPlacement	= splace;
   scroll->maxLeftChar		= 0;
   scroll->currentTop		= 0;
   scroll->currentItem		= 0;
   scroll->currentHigh		= 0;
   scroll->leftChar		= 0;
   scroll->highlight		= highlight;
   scroll->exitType		= vNEVER_ACTIVATED;
   scroll->box			= box;
   scroll->shadow		= shadow;
   scroll->preProcessFunction	= (PROCESSFN)NULL;
   scroll->preProcessData	= (void *)NULL;
   scroll->postProcessFunction	= (PROCESSFN)NULL;
   scroll->postProcessData	= (void *)NULL;
   scroll->ULChar		= ACS_ULCORNER;
   scroll->URChar		= ACS_URCORNER;
   scroll->LLChar		= ACS_LLCORNER;
   scroll->LRChar		= ACS_LRCORNER;
   scroll->HChar		= ACS_HLINE;
   scroll->VChar		= ACS_VLINE;
   scroll->BoxAttrib		= A_NORMAL;

   /* Create the scrolling list item list and needed variables. */
   createCDKScrollItemList (scroll, numbers, list, listSize);

   /* Do we need to create a shadow? */
   if (shadow)
   {
      scroll->shadowWin = newwin (boxHeight, boxWidth, ypos+1, xpos+1);
   }

   /* Clean the key bindings. */
   cleanCDKObjectBindings (vSCROLL, scroll);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vSCROLL, scroll);

   /* Return the scrolling list */
   return scroll;
}

/*
 * This actually does all the 'real' work of managing the scrolling list.
 */
int activateCDKScroll (CDKSCROLL *scroll, chtype *actions)
{
   /* Draw the scrolling list */
   drawCDKScroll (scroll, scroll->box);

   /* Check if actions is NULL. */
   if (actions == (chtype *)NULL)
   {
      /* Declare some local variables. */
      chtype input;
      int ret;

      for (;;)
      {
         /* Get the input. */
         input = wgetch (scroll->win);

         /* Inject the character into the widget. */
         ret = injectCDKScroll (scroll, input);
         if (scroll->exitType != vEARLY_EXIT)
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
         ret = injectCDKScroll (scroll, actions[x]);
         if (scroll->exitType != vEARLY_EXIT)
         {
            return ret;
         }
      }
   }

   /* Set the exit type for the widget and return. */
   scroll->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This injects a single character into the widget.
 */
int injectCDKScroll (CDKSCROLL *scroll, chtype input)
{
   /* Declare local variables. */
   int ppReturn = 1;

   /* Set the exit type for the widget. */
   scroll->exitType = vEARLY_EXIT;

   /* Draw the scrolling list */
   drawCDKScrollList (scroll, scroll->box);

   /* Check if there is a pre-process function to be called. */
   if (scroll->preProcessFunction != (PROCESSFN)NULL)
   {
      /* Call the pre-process function. */
      ppReturn = ((PROCESSFN)(scroll->preProcessFunction)) (vSCROLL, scroll, scroll->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a predefined key binding. */
      if (checkCDKObjectBind (vSCROLL, scroll, input) != 0)
      {
         scroll->exitType = vESCAPE_HIT;
         return -1;
      }
      else
      {
         switch (input)
         {
            case KEY_UP :
                 if (scroll->listSize > 0)
                 {
                    if (scroll->currentItem > 0)
                    {
                       if (scroll->currentHigh == 0)
                       {
                          if (scroll->currentTop != 0)
                          {
                             scroll->currentTop--;
                             scroll->currentItem--;
                          }
                          else
                          {
                             Beep();
                          }
                       }
                       else
                       {
                          scroll->currentItem--;
                          scroll->currentHigh--;
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
                 if (scroll->listSize > 0)
                 {
                    if (scroll->currentItem < scroll->listSize-1)
                    {
                       if (scroll->currentHigh == scroll->viewSize - 1)
                       {
                          /* We need to scroll down one line. */
                          if (scroll->currentTop < scroll->maxTopItem)
                          {
                             scroll->currentTop++;
                             scroll->currentItem++;
                          }
                          else
                          {
                             Beep();
                          }
                       }
                       else
                       {
                          scroll->currentItem++;
                          scroll->currentHigh++;
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
                 if (scroll->listSize > 0)
                 {
                    if (scroll->leftChar >= scroll->maxLeftChar)
                    {
                       Beep();
                    }
                    else
                    {
                       scroll->leftChar ++;
                    }
                 }
                 else
                 {
                    Beep();
                 }
                 break;
   
            case KEY_LEFT :
                 if (scroll->listSize > 0)
                 {
                    if (scroll->leftChar == 0)
                    {
                       Beep();
                    }
                    else
                    {
                       scroll->leftChar --;
                    }
                 }
                 else
                 {
                    Beep();
                 }
                 break;
   
            case KEY_PPAGE : case '' :
                 if (scroll->listSize > 0)
                 {
                    if (scroll->currentTop > 0)
                    {
                       if (scroll->currentTop >= (scroll->viewSize -1))
                       {
                          scroll->currentTop	-= (scroll->viewSize - 1);
                          scroll->currentItem	-= (scroll->viewSize - 1);
                       }
                       else
                       {
                          scroll->currentTop	= 0;
                          scroll->currentItem	= 0;
                          scroll->currentHigh	= 0;
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
                 if (scroll->listSize > 0)
                 {
                    if (scroll->currentTop < scroll->maxTopItem)
                    {
                       if ((scroll->currentTop + scroll->viewSize - 1) <= scroll->maxTopItem)
                       {
                          scroll->currentTop	+= (scroll->viewSize - 1);
                          scroll->currentItem	+= (scroll->viewSize - 1);
                       }
                       else
                       {
                          scroll->currentTop	= scroll->maxTopItem;
                          scroll->currentItem	= scroll->lastItem;
                          scroll->currentHigh	= scroll->viewSize-1;
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
                 scroll->currentTop	= 0;
                 scroll->currentItem	= 0;
                 scroll->currentHigh	= 0;
                 break;
   
            case 'G' : case KEY_END :
                 scroll->currentTop	= scroll->maxTopItem;
                 scroll->currentItem	= scroll->lastItem;
                 scroll->currentHigh	= scroll->viewSize-1;
                 break;
   
            case '$' :
                 scroll->leftChar	= scroll->maxLeftChar;
                 break;
   
            case '|' :
                 scroll->leftChar	= 0;
                 break;
   
            case KEY_ESC :
                 scroll->exitType = vESCAPE_HIT;
                 return -1;
                 break;
      
            case CDK_REFRESH :
                 eraseCDKScreen (scroll->screen);
                 refreshCDKScreen (scroll->screen);
                 break;
      
            case KEY_RETURN : case KEY_TAB : case KEY_ENTER :
                 scroll->exitType = vNORMAL;
                 return scroll->currentItem;
                 break;
      
            default :
                 break;
         }
      }

      /* Should we call a post-process? */
      if (scroll->postProcessFunction != (PROCESSFN)NULL)
      {
         ((PROCESSFN)(scroll->postProcessFunction)) (vSCROLL, scroll, scroll->postProcessData, input);
      }
   }
      
   /* Redraw the list */
   drawCDKScrollList (scroll, scroll->box);

   /* Set the exit type and return. */
   scroll->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This allows the user to accelerate to a position in the scrolling list.
 */
void setCDKScrollPosition (CDKSCROLL *scroll, int item)
{
   if (item < 0)
   {
      scroll->currentTop	= 0;
      scroll->currentItem	= 0;
      scroll->currentHigh	= 1;
   }
   else if (item > scroll->listSize-1)
   {
      scroll->currentTop	= scroll->maxTopItem;
      scroll->currentItem	= scroll->lastItem;
      scroll->currentHigh	= scroll->viewSize-1;
   }
   else
   {
      scroll->currentTop	= item;
      scroll->currentItem	= item;
      scroll->currentHigh	= 0;
   }
}

/*
 * This moves the scroll field to the given location.
 */
void moveCDKScroll (CDKSCROLL *scroll, int xplace, int yplace, boolean relative, boolean refresh)
{
   /* Declare local variables. */
   int currentX = scroll->win->_begx;
   int currentY = scroll->win->_begy;
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
      xpos = scroll->win->_begx + xplace;
      ypos = scroll->win->_begy + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (scroll->screen->window, &xpos, &ypos, scroll->boxWidth, scroll->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   scroll->win->_begx = xpos;
   scroll->win->_begy = ypos;

   /* If there is a shadow box we have to move it too. */
   if (scroll->shadowWin != (WINDOW *)NULL)
   {
      scroll->shadowWin->_begx -= xdiff;
      scroll->shadowWin->_begy -= ydiff;
   }

   /* If there is a scrollbar we have to move it too. */
   if (scroll->scrollbar)
   {
      scroll->scrollbarWin->_begx -= xdiff;
      scroll->scrollbarWin->_begy -= ydiff;
   }

   /* Touch the windows so they 'move'. */
   touchwin (scroll->screen->window);
   wrefresh (scroll->screen->window);

   /* Redraw the window, if they asked for it. */
   if (refresh)
   {
      drawCDKScroll (scroll, scroll->box);
   }
}


/*
 * This allows the user to use the cursor keys to adjust the
 * position of the widget.
 */
void positionCDKScroll (CDKSCROLL *scroll)
{
   /* Declare some variables. */
   int origX	= scroll->win->_begx;
   int origY	= scroll->win->_begy;
   chtype key	= (chtype)NULL;

   /* Let them move the widget around until they hit return. */
   while ((key != KEY_RETURN) && (key != KEY_ENTER))
   {
      key = wgetch (scroll->win);
      if (key == KEY_UP || key == '8')
      {
         if (scroll->win->_begy > 0)
         {
            moveCDKScroll (scroll, 0, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_DOWN || key == '2')
      {
         if (scroll->win->_begy+scroll->win->_maxy < scroll->screen->window->_maxy-1)
         {
            moveCDKScroll (scroll, 0, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_LEFT || key == '4')
      {
         if (scroll->win->_begx > 0)
         {
            moveCDKScroll (scroll, -1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_RIGHT || key == '6')
      {
         if (scroll->win->_begx+scroll->win->_maxx < scroll->screen->window->_maxx-1)
         {
            moveCDKScroll (scroll, 1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '7')
      {
         if (scroll->win->_begy > 0 && scroll->win->_begx > 0)
         {
            moveCDKScroll (scroll, -1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '9')
      {
         if (scroll->win->_begx+scroll->win->_maxx < scroll->screen->window->_maxx-1 &&
		scroll->win->_begy > 0)
         {
            moveCDKScroll (scroll, 1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '1')
      {
         if (scroll->win->_begx > 0 && scroll->win->_begx+scroll->win->_maxx < scroll->screen->window->_maxx-1)
         {
            moveCDKScroll (scroll, -1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '3')
      {
         if (scroll->win->_begx+scroll->win->_maxx < scroll->screen->window->_maxx-1 &&
		scroll->win->_begy+scroll->win->_maxy < scroll->screen->window->_maxy-1)
         {
            moveCDKScroll (scroll, 1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '5')
      {
         moveCDKScroll (scroll, CENTER, CENTER, FALSE, TRUE);
      }
      else if (key == 't')
      {
         moveCDKScroll (scroll, scroll->win->_begx, TOP, FALSE, TRUE);
      }
      else if (key == 'b')
      {
         moveCDKScroll (scroll, scroll->win->_begx, BOTTOM, FALSE, TRUE);
      }
      else if (key == 'l')
      {
         moveCDKScroll (scroll, LEFT, scroll->win->_begy, FALSE, TRUE);
      }
      else if (key == 'r')
      {
         moveCDKScroll (scroll, RIGHT, scroll->win->_begy, FALSE, TRUE);
      }
      else if (key == 'c')
      {
         moveCDKScroll (scroll, CENTER, scroll->win->_begy, FALSE, TRUE);
      }
      else if (key == 'C')
      {
         moveCDKScroll (scroll, scroll->win->_begx, CENTER, FALSE, TRUE);
      }
      else if (key == CDK_REFRESH)
      {
         eraseCDKScreen (scroll->screen);
         refreshCDKScreen (scroll->screen);
      }
      else if (key == KEY_ESC)
      {
         moveCDKScroll (scroll, origX, origY, FALSE, TRUE);
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
void drawCDKScroll (CDKSCROLL *scroll, boolean Box)
{
   int x;

   /* Draw in the shadow if we need to. */
   if (scroll->shadowWin != (WINDOW *)NULL)
   {
      drawShadow (scroll->shadowWin);
   }

   /* Draw in the title if there is one. */
   if (scroll->titleLines != 0)
   {
      for (x=0; x < scroll->titleLines; x++)
      {
         writeChtype (scroll->win,
			scroll->titlePos[x],
			x+1,
			scroll->title[x], 
			HORIZONTAL, 0,
			scroll->titleLen[x]);
      }
   }

   /* Draw in the scolling list items. */
   drawCDKScrollList (scroll, Box);
}

/*
 * This redraws the scrolling list.
 */
void drawCDKScrollList (CDKSCROLL *scroll, boolean Box)
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
   if (scroll->numbers == FALSE)
   {
      scrollbarAdj = (scroll->scrollbarPlacement == LEFT ? 2 : 0);
   }
   else
   {
      scrollbarAdj = 1;
   }

   /* If the list is empty, don't draw anything. */
   if (scroll->listSize < 1)
   {
      /* Box it if needed. */
      if (Box)
      {
         attrbox (scroll->win,
			scroll->ULChar, scroll->URChar,
			scroll->LLChar, scroll->LRChar,
			scroll->HChar,  scroll->VChar,
			scroll->BoxAttrib);
      }
   
      /* Refresh the window. */
      touchwin (scroll->win);
      wrefresh (scroll->win);
      return;
   }

   /* Create the empty string. */
   cleanChar (emptyString, scroll->boxWidth-1, ' ');

   /* Redraw the list */
   for (x=0; x < scroll->viewSize; x++)
   {
      /* Draw the elements in the scrolling list. */
      if (x < scroll->listSize)
      {
         screenPos = scroll->itemPos[x+scroll->currentTop]-scroll->leftChar+scrollbarAdj;

         /* Draw in an empty line. */
         writeChar (scroll->win, 1, scroll->titleAdj+x,
			emptyString, HORIZONTAL, 0, (int)strlen(emptyString));

         /* Write in the correct line. */
         if (screenPos >= 0)
         {
            writeChtype (scroll->win,
			screenPos, x+scroll->titleAdj,
			scroll->item[x+scroll->currentTop],
			HORIZONTAL, 0,
			scroll->itemLen[x+scroll->currentTop]);
         }
         else
         {
            writeChtype (scroll->win,
			1, x+scroll->titleAdj,
			scroll->item[x+scroll->currentTop],
			HORIZONTAL,
			scroll->leftChar - scroll->itemPos[scroll->currentTop]+1,
			scroll->itemLen[x+scroll->currentTop]);
         }
      }
      else 
      {
         /* Draw in an empty line. */
         writeChar (scroll->win, 1, scroll->titleAdj+x,
			emptyString, HORIZONTAL, 0, (int)strlen(emptyString));
      }
   }
   screenPos = scroll->itemPos[scroll->currentItem]-scroll->leftChar+scrollbarAdj;

   /* Rehighlight the current menu item. */
   if (screenPos >= 0)
   {
      writeChtypeAttrib (scroll->win,
			screenPos,
			scroll->currentHigh+scroll->titleAdj,
			scroll->item[scroll->currentItem],
			scroll->highlight,
			HORIZONTAL, 0,
			scroll->itemLen[scroll->currentItem]);
   }
   else
   {
      writeChtypeAttrib (scroll->win,
			1+scrollbarAdj,
			scroll->currentHigh+scroll->titleAdj,
			scroll->item[scroll->currentItem],
			scroll->highlight,
			HORIZONTAL,
			scroll->leftChar - scroll->itemPos[scroll->currentTop]+1,
			scroll->itemLen[scroll->currentItem]);
   }

   /* Determine where the toggle is supposed to be. */
   if (scroll->scrollbarWin != (WINDOW *)NULL)
   {
      if (scroll->listSize > scroll->boxHeight-2)
      {
         scroll->togglePos = (int)(floor((float)((float)scroll->currentItem * (float)scroll->step)));
      }
      else
      {
         scroll->togglePos = (int)(ceil((float)((float)scroll->currentItem * (float)scroll->step)));
      }

      /* Make sure the toggle button doesn't go out of bounds. */
      scrollbarAdj = (scroll->togglePos+scroll->toggleSize)-(scroll->boxHeight-scroll->titleAdj-1);
      if (scrollbarAdj > 0)
      {
         scroll->togglePos -= scrollbarAdj;
      }

      /* Draw the scrollbar. */
      for (x=0; x < scroll->boxHeight-1; x++)
      {
         mvwaddch (scroll->scrollbarWin, x, 0, ACS_CKBOARD);
      }
      for (x=scroll->togglePos; x < scroll->togglePos+scroll->toggleSize; x++)
      {
         mvwaddch (scroll->scrollbarWin, x, 0, ' ' | A_REVERSE);
      }
      touchwin (scroll->scrollbarWin);
      wrefresh (scroll->scrollbarWin);
   }

   /* Box it if needed. */
   if (Box)
   {
      attrbox (scroll->win,
		scroll->ULChar, scroll->URChar,
		scroll->LLChar, scroll->LRChar,
		scroll->HChar,  scroll->VChar,
		scroll->BoxAttrib);
      wrefresh (scroll->win);
   }
   
   /* Refresh the window. */
   touchwin (scroll->win);
   wrefresh (scroll->win);
}

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKScrollULChar (CDKSCROLL *scroll, chtype character)
{
   scroll->ULChar = character;
}
void setCDKScrollURChar (CDKSCROLL *scroll, chtype character)
{
   scroll->URChar = character;
}
void setCDKScrollLLChar (CDKSCROLL *scroll, chtype character)
{
   scroll->LLChar = character;
}
void setCDKScrollLRChar (CDKSCROLL *scroll, chtype character)
{
   scroll->LRChar = character;
}
void setCDKScrollVerticalChar (CDKSCROLL *scroll, chtype character)
{
   scroll->VChar = character;
}
void setCDKScrollHorizontalChar (CDKSCROLL *scroll, chtype character)
{
   scroll->HChar = character;
}
void setCDKScrollBoxAttribute (CDKSCROLL *scroll, chtype character)
{
   scroll->BoxAttrib = character;
}

/*
 * This sets the background color of the widget.
 */ 
void setCDKScrollBackgroundColor (CDKSCROLL *scroll, char *color)
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
   wbkgd (scroll->win, holder[0]);
   if (scroll->scrollbarWin != (WINDOW *)NULL)
   {
      wbkgd (scroll->scrollbarWin, holder[0]);
   }

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This function destroys
 */
void destroyCDKScroll (CDKSCROLL *scroll)
{
   /* Declare local variables. */
   int x;

   /* Erase the object. */
   eraseCDKScroll (scroll);

   /* Clean up the char pointers. */
   for (x=0; x < scroll->titleLines; x++)
   {
      freeChtype (scroll->title[x]);
   }
   for (x=0; x < scroll->listSize; x++)
   {
      freeChtype (scroll->item[x]);
   }

   /* Clean up the windows. */
   deleteCursesWindow (scroll->scrollbarWin);
   deleteCursesWindow (scroll->shadowWin);
   deleteCursesWindow (scroll->win);

   /* Unregister this object. */
   unregisterCDKObject (vSCROLL, scroll);

   /* Finish cleaning up. */
   free (scroll);
}

/*
 * This function erases the scrolling list from the screen.
 */
void eraseCDKScroll (CDKSCROLL *scroll)
{
   eraseCursesWindow (scroll->win);
   eraseCursesWindow (scroll->shadowWin);
}

/*
 * This function creates massages the scrolling list information and 
 * sets up the needed variables fïr the scrolling list to work correctly.
 */
void createCDKScrollItemList (CDKSCROLL *scroll, boolean numbers, char **list, int listSize)
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
         sprintf (temp, "%4d. %s", x+1, list[x]);
         scroll->item[x]	= char2Chtype (temp, &scroll->itemLen[x], &scroll->itemPos[x]);
         scroll->itemPos[x]	= justifyString (scroll->boxWidth, scroll->itemLen[x], scroll->itemPos[x]);
         widestItem		= MAXIMUM (scroll->itemLen[x], widestItem);
      }
   }
   else
   {
      for (x=0 ; x < listSize; x++)
      {
         scroll->item[x]	= char2Chtype (list[x], &scroll->itemLen[x], &scroll->itemPos[x]);
         scroll->itemPos[x]	= justifyString (scroll->boxWidth, scroll->itemLen[x], scroll->itemPos[x]);
         widestItem		= MAXIMUM (scroll->itemLen[x], widestItem);
      }
   }

   /* Determine how many characters we can shift to the right */
   /* before all the items have been scrolled off the screen. */
   if (scroll->boxWidth > widestItem)
   {
      scroll->maxLeftChar = 0;
   }
   else
   {
      scroll->maxLeftChar = widestItem-(scroll->boxWidth-2);
   }

   /* Keep the boolean flag 'numbers' */
   scroll->numbers = numbers;
}

/*
 * This sets certain attributes of the scrolling list.
 */
void setCDKScroll (CDKSCROLL *scroll, char **list, int listSize, boolean numbers, chtype highlight, boolean Box)
{
   setCDKScrollItems (scroll, list, listSize, numbers);
   setCDKScrollHighlight (scroll, highlight);
   setCDKScrollBox (scroll, Box);
}

/*
 * This sets the scrolling list items.
 */
void setCDKScrollItems (CDKSCROLL *scroll, char **list, int listSize, boolean numbers)
{
   /* Declare some wars. */
   char emptyString[2000];
   int x = 0;

   /* Clean out the old list. */
   for (x=0; x < scroll->listSize; x++)
   {
      freeChtype (scroll->item[x]);
      scroll->itemLen[x] = 0;
      scroll->itemPos[x] = 0;
   }

   /* Clean up the display. */
   cleanChar (emptyString, scroll->boxWidth-1, ' ');
   for (x=0; x < scroll->viewSize ; x++)
   {
      writeChar (scroll->win, 1, scroll->titleAdj+x, emptyString, 
			HORIZONTAL, 0, (int)strlen(emptyString));
   }

   /* Readjust all of the variables ... */
   scroll->listSize	= listSize;
   scroll->viewSize	= scroll->boxHeight - (2 + scroll->titleLines);
   scroll->lastItem	= listSize - 1;
   scroll->maxTopItem	= listSize - scroll->viewSize;

   /* Is the view size smaller then the window? */
   if (listSize < (scroll->boxHeight-1-scroll->titleAdj))
   {
      scroll->viewSize		= listSize;
      scroll->listSize		= listSize;
      scroll->lastItem		= listSize;
      scroll->maxTopItem	= -1;
   }

   /* Set some vars. */
   scroll->currentTop	= 0;
   scroll->currentItem	= 0;
   scroll->currentHigh	= 0;
   scroll->leftChar	= 0;

   /* Set the information for the scroll bar. */
   scroll->step		= (float)(scroll->boxHeight-2) / (float)scroll->listSize;
   scroll->toggleSize	= (scroll->listSize > (scroll->boxHeight-2) ? 1 : (int)ceil(scroll->step));

   /* Set up the new list. */
   createCDKScrollItemList (scroll, numbers, list, listSize);
}
int getCDKScrollItems (CDKSCROLL *scroll, char *list[])
{
   int x;

   for (x=0; x < scroll->listSize; x++)
   {
      list[x] = chtype2Char (scroll->item[x]);
   }
   return scroll->listSize;
}

/*
 * This sets the highlight of the scrolling list.
 */
void setCDKScrollHighlight (CDKSCROLL *scroll, chtype highlight)
{
   scroll->highlight = highlight;
}
chtype getCDKScrollHighlight (CDKSCROLL *scroll, chtype highlight)
{
   return scroll->highlight;
}

/*
 * This sets the box attribute of the scrolling list.
 */
void setCDKScrollBox (CDKSCROLL *scroll, boolean Box)
{
   scroll->box = Box;
}
boolean getCDKScrollBox (CDKSCROLL *scroll)
{
   return scroll->box;
}

/*
 * This adds a single item to a scrolling list.
 */
void addCDKScrollItem (CDKSCROLL *scroll, char *item)
{
   /* Declare some local variables. */
   int itemNumber = scroll->listSize;
   int widestItem = scroll->maxLeftChar + (scroll->boxWidth-2);
   char temp[256];

   /*
    * If the scrolling list has numbers, then add the new item 
    * with a numeric value attached.
     */
   if (scroll->numbers == NUMBERS)
   {
      sprintf (temp, "%4d. %s", itemNumber+1, item);
      scroll->item[itemNumber] = char2Chtype (temp, &scroll->itemLen[itemNumber], &scroll->itemPos[itemNumber]);
      scroll->itemPos[itemNumber] = justifyString (scroll->boxWidth, scroll->itemLen[itemNumber], scroll->itemPos[itemNumber]);
   }
   else
   {
      scroll->item[itemNumber] = char2Chtype (item, &scroll->itemLen[itemNumber], &scroll->itemPos[itemNumber]);
      scroll->itemPos[itemNumber] = justifyString (scroll->boxWidth, scroll->itemLen[itemNumber], scroll->itemPos[itemNumber]);
   }

   /* Determine the size of the widest item. */
   widestItem = MAXIMUM (scroll->itemLen[itemNumber], widestItem);
   if (scroll->boxWidth > widestItem)
   {
      scroll->maxLeftChar = 0;
   }
   else
   {
      scroll->maxLeftChar = widestItem - (scroll->boxWidth-2);
   }

   /* Increment the list size. */
   scroll->listSize++;
   if (scroll->listSize < scroll->viewSize)
   {
      scroll->lastItem = scroll->listSize;
      scroll->maxTopItem = -1;
   }
   else
   {
      scroll->lastItem = scroll->listSize - 1;
      scroll->maxTopItem = scroll->listSize - scroll->viewSize;
   }

   /* Reset some variables. */
   scroll->currentHigh	= 0;
   scroll->currentTop	= 0;
   scroll->currentItem	= 0;
   scroll->leftChar	= 0;
}

/*
 * This adds a single item to a scrolling list.
 */
void deleteCDKScrollItem (CDKSCROLL *scroll, int position)
{
   /* Declare some local variables. */
   int x;

   /* Nuke the current item. */
   freeChtype (scroll->item[position]);

   /* Adjust the list. */
   for (x=position; x < scroll->listSize-1; x++)
   {
      scroll->item[x]		= scroll->item[x+1];
      scroll->itemLen[x]	= scroll->itemLen[x+1];
      scroll->itemPos[x]	= scroll->itemPos[x+1];
   }
   scroll->listSize--;

   /* Did we just delete the last element? */
   if (scroll->listSize == 0)
   {
      scroll->lastItem		= -1;
      scroll->maxTopItem	= -1;
      scroll->currentHigh	= -1;
      scroll->currentItem	= -1;
      scroll->currentTop	= -1;
   }
   else
   {
      scroll->lastItem--;
      scroll->maxTopItem--;
      scroll->currentHigh = 0;
      scroll->currentItem = 0;
      scroll->currentTop = 0;
   }
}

/*
 * This function sets the pre-process function.
 */
void setCDKScrollPreProcess (CDKSCROLL *scroll, PROCESSFN callback, void *data)
{
   scroll->preProcessFunction = callback;
   scroll->preProcessData = data;
}
 
/*
 * This function sets the post-process function.
 */
void setCDKScrollPostProcess (CDKSCROLL *scroll, PROCESSFN callback, void *data)
{
   scroll->postProcessFunction = callback;
   scroll->postProcessData = data;
}
