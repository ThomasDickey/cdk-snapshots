#include "cdk.h"
#include <limits.h>

/*
 * $Author: glovem $
 * $Date: 1998/03/02 16:31:18 $
 * $Revision: 1.57 $
 */

/*
 * Declare file local prototypes.
 */
void drawCDKRadioList (CDKRADIO *radio, boolean box);

/*
 * This function creates the radio widget.
 */
CDKRADIO *newCDKRadio (CDKSCREEN *cdkscreen, int xplace, int yplace, int splace, int height, int width, char *title, char **list, int listSize, chtype choiceChar, int defItem, chtype highlight, boolean box, boolean shadow)
{
   /* Declare local variables. */
   CDKRADIO *radio	= (CDKRADIO *)malloc (sizeof (CDKRADIO));
   chtype *holder	= (chtype *)NULL;
   int parentWidth	= WIN_WIDTH (cdkscreen->window);
   int parentHeight	= WIN_HEIGHT (cdkscreen->window);
   int boxWidth		= width;
   int boxHeight	= height;
   int maxWidth		= INT_MIN;
   int xpos		= xplace;
   int ypos		= yplace;
   int widestItem	= 0;
   int x		= 0;
   char *temp[MAX_LINES];
   int len, junk2;

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

   /* Translate the char * title to a chtype * */
   if (title != (char *)NULL)
   {
      /* We need to split the title on '\n' */
      radio->titleLines = splitString (title, temp, '\n');

      /* We need to determine the widest title line. */
      for (x=0; x < radio->titleLines; x++)
      {
         holder = char2Chtype (temp[x], &len, &junk2);
         maxWidth = MAXIMUM (maxWidth, len);
         freeChtype (holder);
      }
      boxWidth = MAXIMUM (boxWidth, maxWidth+2);

      /* For each line in the title, convert from a char * to a chtype * */
      for (x=0; x < radio->titleLines; x++)
      {
         radio->title[x]	= char2Chtype (temp[x], &radio->titleLen[x], &radio->titlePos[x]);
         radio->titlePos[x]	= justifyString (boxWidth, radio->titleLen[x], radio->titlePos[x]);
         freeChar (temp[x]);
      }
   }
   else
   {
      radio->titleLines = 0;
   }

   /* Set the box height. */
   if (radio->titleLines > boxHeight)
   {
      if (listSize > 8)
      {
         boxHeight = radio->titleLines + 10;
      }
      else
      {
         boxHeight = radio->titleLines + listSize + 2;
      }
   }

   /* Set the rest of the variables. */
   radio->titleAdj	= radio->titleLines + 1;
   radio->listSize	= listSize;
   radio->viewSize	= boxHeight - (2 + radio->titleLines);
   radio->lastItem	= listSize - 1;
   radio->maxTopItem	= listSize - radio->viewSize;

   /* Is the view size smaller then the window??? */
   if (listSize < (boxHeight-1-radio->titleAdj))
   {
      radio->viewSize	= listSize;
      radio->listSize	= listSize;
      radio->lastItem	= listSize;
      radio->maxTopItem	= -1;
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
   radio->step		= (float)(boxHeight-2) / (float)radio->listSize;
   radio->toggleSize	= (radio->listSize > (boxHeight-2) ? 1 : (int)ceil(radio->step));

   /* Each item in the needs to be converted to chtype * */
   for (x=0; x < listSize; x++)
   {
      radio->item[x]	= char2Chtype (list[x], &radio->itemLen[x], &radio->itemPos[x]);
      radio->itemPos[x]	= justifyString (boxWidth, radio->itemLen[x], radio->itemPos[x]) + 3;
      widestItem	= MAXIMUM(widestItem, radio->itemLen[x]);
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
      radio->maxLeftChar = widestItem-(boxWidth-2) + 3;
   }

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the radio window */
   radio->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the window NULL??? */
   if (radio->win == (WINDOW *)NULL)
   {
      /* Clean up memory. */
      for (x=0; x < listSize; x++)
      {
         freeChtype (radio->item[x]);
      }
      for (x=0; x < radio->titleLines; x++)
      {
         freeChtype (radio->title[x]);
      }
      free (radio);

      /* Return a NULL pointer. */
      return ((CDKRADIO *)NULL);
   }

   /* Turn on the keypad. */
   keypad (radio->win, TRUE);

   /* Create the scrollbar window. */
   if (splace == RIGHT)
   {
      radio->scrollbarWin = subwin (radio->win,
					boxHeight-radio->titleAdj-1, 1,
					ypos+radio->titleAdj,
					xpos+boxWidth-2);
   }
   else if (splace == LEFT)
   {
      radio->scrollbarWin = subwin (radio->win,
					boxHeight-radio->titleAdj-1, 1,
					ypos+radio->titleAdj,
					xpos+1);
   }
   else
   {
      radio->scrollbarWin = (WINDOW *)NULL;
   }

   /* Set the rest of the variables */
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
   radio->exitType		= vNEVER_ACTIVATED;
   radio->box			= box;
   radio->shadow		= shadow;
   radio->preProcessFunction	= (PROCESSFN)NULL;
   radio->preProcessData	= (void *)NULL;
   radio->postProcessFunction	= (PROCESSFN)NULL;
   radio->postProcessData	= (void *)NULL;
   radio->shadowWin		= (WINDOW *)NULL;
   radio->ULChar		= ACS_ULCORNER;
   radio->URChar		= ACS_URCORNER;
   radio->LLChar		= ACS_LLCORNER;
   radio->LRChar		= ACS_LRCORNER;
   radio->HChar			= ACS_HLINE;
   radio->VChar			= ACS_VLINE;
   radio->BoxAttrib		= A_NORMAL;

   /* Do we need to create the shadow??? */
   if (shadow)
   {
      radio->shadowWin	= newwin (boxHeight, boxWidth+1, ypos+1, xpos+1);
   }

   /* Clean the key bindings. */
   cleanCDKObjectBindings (vRADIO, radio);

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
   drawCDKRadio (radio, radio->box);

   /* Check if actions is NULL. */
   if (actions == (chtype *)NULL)
   {
      /* Declare some local variables. */
      chtype input;
      int ret;

      for (;;)
      {
         /* Get the input. */
         input = wgetch (radio->win);

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
      /* Declare some local variables. */
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
   radio->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This injects a single character into the widget.
 */
int injectCDKRadio (CDKRADIO *radio, chtype input)
{
   /* Declare local variables. */
   int ppReturn = 1;

   /* Set the exit type. */
   radio->exitType = vEARLY_EXIT;

   /* Draw the radio list */
   drawCDKRadioList (radio, radio->box);

   /* Check if there is a pre-process function to be called. */
   if (radio->preProcessFunction != (PROCESSFN)NULL)
   {
      /* Call the pre-process function. */
      ppReturn = ((PROCESSFN)(radio->preProcessFunction)) (vRADIO, radio, radio->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a predefined key binding. */
      if (checkCDKObjectBind (vRADIO, radio, input) != 0)
      {
         radio->exitType = vESCAPE_HIT;
         return -1;
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
   
            case KEY_PPAGE : case '' :
                 if (radio->currentTop > 0)
                 {
                    if (radio->currentTop >= (radio->viewSize -1))
                    {
                       radio->currentTop	-= (radio->viewSize - 1);
                       radio->currentItem	-= (radio->viewSize - 1);
                    }
                    else
                    {
                       radio->currentTop 	= 0;
                       radio->currentItem	= 0;
                       radio->currentHigh	= 0;
                    }
                 }
                 else
                 {
                    Beep();
                 }
                 break;
   
            case KEY_NPAGE : case '' :
                 if (radio->currentTop < radio->maxTopItem)
                 {
                    if ((radio->currentTop + radio->viewSize - 1) <= radio->maxTopItem)
                    {
                       radio->currentTop	+= (radio->viewSize - 1);
                       radio->currentItem	+= (radio->viewSize - 1);
                    }
                    else
                    {
                       radio->currentTop 	= radio->maxTopItem;
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
                 radio->currentTop 	= radio->maxTopItem;
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
   
            case KEY_NEXT : case KEY_ESC :
                 radio->exitType = vESCAPE_HIT;
                 return -1;
                 break;
   
            case KEY_RETURN : case KEY_TAB : case KEY_ENTER :
                 radio->exitType = vNORMAL;
                 return radio->selectedItem;
                 break;
   
            case CDK_REFRESH :
                 eraseCDKScreen (radio->screen);
                 refreshCDKScreen (radio->screen);
                 break;
   
            default :
                 break;
         }
      }
   
      /* Should we call a post-process? */
      if (radio->postProcessFunction != (PROCESSFN)NULL)
      {
         ((PROCESSFN)(radio->postProcessFunction)) (vRADIO, radio, radio->postProcessData, input);
      }
   }

   /* Draw the radio list */
   drawCDKRadioList (radio, radio->box);

   /* Set the exit type and return. */
   radio->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This moves the radio field to the given location.
 */
void moveCDKRadio (CDKRADIO *radio, int xplace, int yplace, boolean relative, boolean refresh)
{
   /* Declare local variables. */
   int currentX = radio->win->_begx;
   int currentY = radio->win->_begy;
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
      xpos = radio->win->_begx + xplace;
      ypos = radio->win->_begy + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (radio->screen->window, &xpos, &ypos, radio->boxWidth, radio->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   radio->win->_begx = xpos;
   radio->win->_begy = ypos;

   /* If there is a scrollbar we have to move it too. */
   if (radio->scrollbar)
   {
      radio->scrollbarWin->_begx -= xdiff;
      radio->scrollbarWin->_begy -= ydiff;
   }

   /* If there is a shadow box we have to move it too. */
   if (radio->shadowWin != (WINDOW *)NULL)
   {
      radio->shadowWin->_begx -= xdiff;
      radio->shadowWin->_begy -= ydiff;
   }

   /* Touch the windows so they 'move'. */
   touchwin (radio->screen->window);
   wrefresh (radio->screen->window);

   /* Redraw the window, if they asked for it. */
   if (refresh)
   {
      drawCDKRadio (radio, radio->box);
   }
}


/*
 * This allows the user to use the cursor keys to adjust the
 * position of the widget.
 */
void positionCDKRadio (CDKRADIO *radio)
{
   /* Declare some variables. */
   int origX	= radio->win->_begx;
   int origY	= radio->win->_begy;
   chtype key	= (chtype)NULL;

   /* Let them move the widget around until they hit return. */
   while ((key != KEY_RETURN) && (key != KEY_ENTER))
   {
      key = wgetch (radio->win);
      if (key == KEY_UP || key == '8')
      {
         if (radio->win->_begy > 0)
         {
            moveCDKRadio (radio, 0, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_DOWN || key == '2')
      {
         if (radio->win->_begy+radio->win->_maxy < radio->screen->window->_maxy-1)
         {
            moveCDKRadio (radio, 0, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_LEFT || key == '4')
      {
         if (radio->win->_begx > 0)
         {
            moveCDKRadio (radio, -1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_RIGHT || key == '6')
      {
         if (radio->win->_begx+radio->win->_maxx < radio->screen->window->_maxx-1)
         {
            moveCDKRadio (radio, 1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '7')
      {
         if (radio->win->_begy > 0 && radio->win->_begx > 0)
         {
            moveCDKRadio (radio, -1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '9')
      {
         if (radio->win->_begx+radio->win->_maxx < radio->screen->window->_maxx-1 &&
		radio->win->_begy > 0)
         {
            moveCDKRadio (radio, 1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '1')
      {
         if (radio->win->_begx > 0 && radio->win->_begx+radio->win->_maxx < radio->screen->window->_maxx-1)
         {
            moveCDKRadio (radio, -1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '3')
      {
         if (radio->win->_begx+radio->win->_maxx < radio->screen->window->_maxx-1 &&
		radio->win->_begy+radio->win->_maxy < radio->screen->window->_maxy-1)
         {
            moveCDKRadio (radio, 1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '5')
      {
         moveCDKRadio (radio, CENTER, CENTER, FALSE, TRUE);
      }
      else if (key == 't')
      {
         moveCDKRadio (radio, radio->win->_begx, TOP, FALSE, TRUE);
      }
      else if (key == 'b')
      {
         moveCDKRadio (radio, radio->win->_begx, BOTTOM, FALSE, TRUE);
      }
      else if (key == 'l')
      {
         moveCDKRadio (radio, LEFT, radio->win->_begy, FALSE, TRUE);
      }
      else if (key == 'r')
      {
         moveCDKRadio (radio, RIGHT, radio->win->_begy, FALSE, TRUE);
      }
      else if (key == 'c')
      {
         moveCDKRadio (radio, CENTER, radio->win->_begy, FALSE, TRUE);
      }
      else if (key == 'C')
      {
         moveCDKRadio (radio, radio->win->_begx, CENTER, FALSE, TRUE);
      }
      else if (key == CDK_REFRESH)
      {
         eraseCDKScreen (radio->screen);
         refreshCDKScreen (radio->screen);
      }
      else if (key == KEY_ESC)
      {
         moveCDKRadio (radio, origX, origY, FALSE, TRUE);
      }
      else if ((key != KEY_RETURN) && (key != KEY_ENTER))
      {
         Beep();
      }
   }
}

/*
 * This function draws the radio widget.
 */
void drawCDKRadio (CDKRADIO *radio, boolean Box)
{
   int x;

   /* Do we need to draw in the shadow??? */
   if (radio->shadowWin != (WINDOW *)NULL)
   {
      drawShadow (radio->shadowWin);
   }

   /* Draw in the title if there is one */
   if (radio->titleLines != 0)
   {
      for (x=0; x < radio->titleLines; x++)
      {
         writeChtype (radio->win,
			radio->titlePos[x],
			x+1,
			radio->title[x],
			HORIZONTAL, 0,
			radio->titleLen[x]);
      }
   }

   /* Draw in the radio list. */
   drawCDKRadioList (radio, radio->box);
}

/*
 * This redraws the radio list.
 */
void drawCDKRadioList (CDKRADIO *radio, boolean Box)
{
   /* Declare local variables. */
   int scrollbarAdj	= 0;
   int screenPos	= 0;
   char emptyString[1000];
   int x;

  /*
   * If the scroll bar is on the left hand side, then adjust
   * everything over to the right one character.
   */
   if (radio->scrollbarPlacement == LEFT)
   {
      scrollbarAdj = 1;
   }

   /* Create a string full of spaces. */
   cleanChar (emptyString, radio->boxWidth-1, ' ');

   /* Redraw the list */
   for (x=0; x < radio->viewSize; x++)
   {
      screenPos = radio->itemPos[x+radio->currentTop]-radio->leftChar+scrollbarAdj;

      /* Draw in the empty string. */
      writeChar (radio->win, 1, radio->titleAdj+x,
			emptyString, HORIZONTAL, 0, (int)strlen(emptyString));

      /* Draw in the line. */
      if (screenPos >= 0)
      {
         writeChtype (radio->win,
			screenPos, x+radio->titleAdj,
			radio->item[x+radio->currentTop],
			HORIZONTAL, 0,
			radio->itemLen[x+radio->currentTop]);
      }
      else
      {
         writeChtype (radio->win,
			1, x+radio->titleAdj,
			radio->item[x+radio->currentTop],
			HORIZONTAL,
			radio->leftChar - radio->itemPos[x+radio->currentTop]+1,
			radio->itemLen[x+radio->currentTop]);
      }

      /* Draw in the selected choice... */
      if (x+radio->currentTop == radio->selectedItem)
      {
         mvwaddch (radio->win, x+radio->titleAdj, 1+scrollbarAdj, radio->leftBoxChar);
         mvwaddch (radio->win, x+radio->titleAdj, 2+scrollbarAdj, radio->choiceChar);
         mvwaddch (radio->win, x+radio->titleAdj, 3+scrollbarAdj, radio->rightBoxChar);
      }
      else
      {
         mvwaddch (radio->win, x+radio->titleAdj, 1+scrollbarAdj, radio->leftBoxChar);
         mvwaddch (radio->win, x+radio->titleAdj, 2+scrollbarAdj, ' ');
         mvwaddch (radio->win, x+radio->titleAdj, 3+scrollbarAdj, radio->rightBoxChar);
      }
   }
   screenPos = radio->itemPos[radio->currentItem]-radio->leftChar+scrollbarAdj;

   /* Draw in the filler character for the scroll bar. */
   if (radio->scrollbarWin != (WINDOW *)NULL)
   {
      for (x=0; x < radio->boxHeight-1; x++)
      {
         mvwaddch (radio->scrollbarWin, x, 0, ACS_CKBOARD);
      }
   }

   /* Highlight the current item. */
   if (screenPos >= 0)
   {
      writeChtypeAttrib (radio->win,
			screenPos, radio->currentHigh+radio->titleAdj,
			radio->item[radio->currentItem],
			radio->highlight,
			HORIZONTAL, 0,
			radio->itemLen[radio->currentItem]);
   }
   else
   {
      writeChtypeAttrib (radio->win,
			1+scrollbarAdj,
			radio->currentHigh+radio->titleAdj,
			radio->item[radio->currentItem],
			radio->highlight,
			HORIZONTAL,
			radio->leftChar - radio->itemPos[radio->currentTop]+1,
			radio->itemLen[radio->currentItem]);
   }

   /* Draw in the selected choice... */
   if (radio->currentItem == radio->selectedItem)
   {
      mvwaddch (radio->win, radio->currentHigh+radio->titleAdj, 1+scrollbarAdj, radio->leftBoxChar);
      mvwaddch (radio->win, radio->currentHigh+radio->titleAdj, 2+scrollbarAdj, radio->choiceChar);
      mvwaddch (radio->win, radio->currentHigh+radio->titleAdj, 3+scrollbarAdj, radio->rightBoxChar);
   }
   else
   {
      mvwaddch (radio->win, radio->currentHigh+radio->titleAdj, 1+scrollbarAdj, radio->leftBoxChar);
      mvwaddch (radio->win, radio->currentHigh+radio->titleAdj, 2+scrollbarAdj, ' ');
      mvwaddch (radio->win, radio->currentHigh+radio->titleAdj, 3+scrollbarAdj, radio->rightBoxChar);
   }

   /* Determine where the toggle is supposed to be. */
   if (radio->scrollbar)
   {
      if (radio->listSize > radio->boxHeight-2)
      {
         radio->togglePos = (int)(floor((float)((float)radio->currentItem * (float)radio->step)));
      }
      else
      {
         radio->togglePos = (int)(ceil((float)((float)radio->currentItem * (float)radio->step)));
      }

      /* Make sure the toggle button doesn't go out of bounds. */
      scrollbarAdj = (radio->togglePos+radio->toggleSize)-(radio->boxHeight-radio->titleAdj-1);
      if (scrollbarAdj > 0)
      {
         radio->togglePos -= scrollbarAdj;
      }

      /* Draw the scrollbar. */
      for (x=radio->togglePos; x < radio->togglePos+radio->toggleSize; x++)
      {
         mvwaddch (radio->scrollbarWin, x, 0, ' ' | A_REVERSE);
      }
      touchwin (radio->scrollbarWin);
      wrefresh (radio->scrollbarWin);
   }

   /* Box it if needed. */
   if (Box)
   {
      attrbox (radio->win,
		radio->ULChar, radio->URChar,
		radio->LLChar, radio->LRChar,
		radio->HChar,  radio->VChar,
		radio->BoxAttrib);
   }

   /* Refresh the window. */
   touchwin (radio->win);
   wrefresh (radio->win);
}

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKRadioULChar (CDKRADIO *radio, chtype character)
{
   radio->ULChar = character;
}
void setCDKRadioURChar (CDKRADIO *radio, chtype character)
{
   radio->URChar = character;
}
void setCDKRadioLLChar (CDKRADIO *radio, chtype character)
{
   radio->LLChar = character;
}
void setCDKRadioLRChar (CDKRADIO *radio, chtype character)
{
   radio->LRChar = character;
}
void setCDKRadioVerticalChar (CDKRADIO *radio, chtype character)
{
   radio->VChar = character;
}
void setCDKRadioHorizontalChar (CDKRADIO *radio, chtype character)
{
   radio->HChar = character;
}
void setCDKRadioBoxAttribute (CDKRADIO *radio, chtype character)
{
   radio->BoxAttrib = character;
}

/*
 * This sets the background color of the widget.
 */ 
void setCDKRadioBackgroundColor (CDKRADIO *radio, char *color)
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
   wbkgd (radio->win, holder[0]);
   if (radio->scrollbarWin != (WINDOW *)NULL)
   {
      wbkgd (radio->scrollbarWin, holder[0]);
   }

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This function destroys the radio widget.
 */
void destroyCDKRadio (CDKRADIO *radio)
{
   /* Declare local variables. */
   int x	= 0;

   /* Erase the object. */
   eraseCDKRadio (radio);

   /* Clear up the char pointers. */
   for (x=0; x < radio->titleLines; x++)
   {
      freeChtype (radio->title[x]);
   }
   for (x=0; x < radio->listSize; x++)
   {
      freeChtype (radio->item[x]);
   }

   /* Clean up the windows. */
   deleteCursesWindow (radio->scrollbarWin);
   deleteCursesWindow (radio->shadowWin);
   deleteCursesWindow (radio->win);

   /* Unregister this object. */
   unregisterCDKObject (vRADIO, radio);

   /* Finish cleaning up. */
   free (radio);
}

/*
 * This function erases the radio widget.
 */
void eraseCDKRadio (CDKRADIO *radio)
{
   eraseCursesWindow (radio->win);
   eraseCursesWindow (radio->shadowWin);
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
   /* Declare some wars. */
   char emptyString[2000];
   int widestItem	= -1;
   int x		= 0;

   /* Clean out the old list. */
   for (x=0; x < radio->listSize; x++)
   {
      freeChtype (radio->item[x]);
      radio->itemLen[x] = 0;
      radio->itemPos[x] = 0;
   }

   /* Clean up the display. */
   cleanChar (emptyString, radio->boxWidth-1, ' ');
   for (x=0; x < radio->viewSize ; x++)
   {
      writeChar (radio->win, 1, radio->titleAdj+x, emptyString, 
			HORIZONTAL, 0, (int)strlen(emptyString));
   }

   /* Readjust all of the variables ... */
   radio->listSize		= listSize;
   radio->viewSize		= radio->boxHeight - (2 + radio->titleLines);
   radio->lastItem		= listSize - 1;
   radio->maxTopItem	= listSize - radio->viewSize;

   /* Is the view size smaller then the window? */
   if (listSize < (radio->boxHeight-1-radio->titleAdj))
   {
      radio->viewSize	= listSize;
      radio->listSize	= listSize;
      radio->lastItem	= listSize;
      radio->maxTopItem	= -1;
   }

   /* Set some vars. */
   radio->currentTop	= 0;
   radio->currentItem	= 0;
   radio->currentHigh	= 0;
   radio->leftChar	= 0;
   radio->selectedItem	= 0;

   /* Set the information for the radio bar. */
   radio->step		= (float)(radio->boxHeight-2) / (float)radio->listSize;
   radio->toggleSize	= (radio->listSize > (radio->boxHeight-2) ? 1 : (int)ceil(radio->step));

   /* Each item in the needs to be converted to chtype * */
   for (x=0; x < listSize; x++)
   {
      radio->item[x]	= char2Chtype (list[x], &radio->itemLen[x], &radio->itemPos[x]);
      radio->itemPos[x]	= justifyString (radio->boxWidth, radio->itemLen[x], radio->itemPos[x]) + 3;
      widestItem	= MAXIMUM(widestItem, radio->itemLen[x]);
   }
 
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
   radio->box = Box;
}
boolean getCDKRadioBox (CDKRADIO *radio)
{
   return radio->box;
}

/*
 * This function sets the pre-process function.
 */
void setCDKRadioPreProcess (CDKRADIO *radio, PROCESSFN callback, void *data)
{
   radio->preProcessFunction = callback;
   radio->preProcessData = data;
}
 
/*
 * This function sets the post-process function.
 */
void setCDKRadioPostProcess (CDKRADIO *radio, PROCESSFN callback, void *data)
{
   radio->postProcessFunction = callback;
   radio->postProcessData = data;
}
