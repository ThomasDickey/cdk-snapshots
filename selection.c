#include "cdk.h"
#include <limits.h>

/*
 * $Author: tom $
 * $Date: 1999/05/23 02:53:29 $
 * $Revision: 1.82 $
 */

/*
 * Declare file local prototypes.
 */
static void drawCDKSelectionList (CDKSELECTION *selection, boolean Box);

static CDKFUNCS my_funcs = {
    _drawCDKSelection,
    _eraseCDKSelection,
};

/*
 * This function creates a selection widget.
 */
CDKSELECTION *newCDKSelection (CDKSCREEN *cdkscreen, int xplace, int yplace, int splace, int height, int width, char *title, char **list, int listSize, char **choices, int choiceCount, chtype highlight, boolean Box, boolean shadow)
{
   /* Declare local variables. */
   CDKSELECTION *selection	= newCDKObject(CDKSELECTION, &my_funcs);
   chtype *holder		= (chtype *)NULL;
   int maxWidth			= INT_MIN;
   int widestitem		= -1;
   int parentWidth		= getmaxx(cdkscreen->window) - 1;
   int parentHeight		= getmaxy(cdkscreen->window) - 1;
   int boxWidth			= width;
   int boxHeight		= height;
   int xpos			= xplace;
   int ypos			= yplace;
   int x			= 0;
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
   boxWidth = setWidgetDimension (parentWidth, width, 0);

   /* Translate the char * title to a chtype * */
   if (title != (char *)NULL)
   {
      /* We need to split the title on \n. */
      selection->titleLines = splitString (title, temp, '\n');

      /* We need to determine the widest title line. */
      for (x=0; x < selection->titleLines; x++)
      {
         holder = char2Chtype (temp[x], &len, &junk2);
         maxWidth = MAXIMUM (maxWidth, len);
         freeChtype (holder);
      }
      boxWidth = MAXIMUM (boxWidth, maxWidth + 2);

      /* For each line in the title, convert from char * to chtype * */
      for (x=0; x < selection->titleLines; x++)
      {
         selection->title[x] = char2Chtype (temp[x], 
						&selection->titleLen[x], 
						&selection->titlePos[x]);
         selection->titlePos[x] = justifyString (boxWidth, 
							selection->titleLen[x], 
							selection->titlePos[x]);
         freeChar (temp[x]);
      }
   }
   else
   {
      /* No title? Set the required variables. */
      selection->titleLines = 0;
   }

   /* Set the box height. */
   if (selection->titleLines > boxHeight)
   {
      if (listSize > 8)
      {
         boxHeight = selection->titleLines + 10;
      }
      else
      {
         boxHeight = selection->titleLines + listSize + 2;
      }
   }

   /* Set the rest of the variables. */
   selection->titleAdj		= selection->titleLines + 1;
   selection->listSize		= listSize;
   selection->viewSize		= boxHeight - (2 + selection->titleLines);
   selection->lastItem		= listSize - 1;
   selection->maxTopItem	= listSize - selection->viewSize;
   selection->maxchoicelen	= -1;
   
   /* Is the view size smaller then the window??? */
   if (listSize < (boxHeight-1-selection->titleAdj))
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
   selection->step		= (float)(boxHeight-2) / (float)selection->listSize;
   selection->toggleSize	= (selection->listSize > (boxHeight-2) ? 1 : ceilCDK(selection->step));

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);
   
   /* Make the selection window */
   selection->win = newwin (boxHeight, boxWidth, ypos, xpos);
   
   /* Is the window NULL?? */
   if (selection->win == (WINDOW *)NULL)
   {
      /* Clean up any memory used. */
      for (x=0; x < selection->titleLines; x++)
      {
         freeChtype (selection->title[x]);
      }
      free (selection);
   
      /* Return a NULL pointer. */
      return ((CDKSELECTION *)NULL);
   }
   
   /* Turn the keypad on for this window. */
   keypad (selection->win, TRUE);

   /* Create the scrollbar window. */
   if (splace == RIGHT)
   {
      selection->scrollbarWin = subwin (selection->win,
					boxHeight-selection->titleAdj-1, 1,
					ypos + selection->titleAdj,
					xpos + boxWidth-2);
   }
   else if (splace == LEFT)
   {
      selection->scrollbarWin = subwin (selection->win,
					boxHeight-selection->titleAdj-1, 1,
					ypos + selection->titleAdj,
					xpos + 1);
   }
   else
   {
      selection->scrollbarWin = (WINDOW *)NULL;
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
   selection->exitType			= vNEVER_ACTIVATED;
   ObjOf(selection)->box		= Box;
   selection->shadow			= shadow;
   selection->preProcessFunction	= (PROCESSFN)NULL;
   selection->preProcessData		= (void *)NULL;
   selection->postProcessFunction	= (PROCESSFN)NULL;
   selection->postProcessData		= (void *)NULL;
   selection->shadowWin			= (WINDOW *)NULL;
   selection->ULChar			= ACS_ULCORNER;
   selection->URChar			= ACS_URCORNER;
   selection->LLChar			= ACS_LLCORNER;
   selection->LRChar			= ACS_LRCORNER;
   selection->HChar			= ACS_HLINE;
   selection->VChar			= ACS_VLINE;
   selection->BoxAttrib			= A_NORMAL;
   
   /* Each choice has to be converted from char * to chtype * */
   for (x=0; x < choiceCount; x++)
   {
      selection->choice[x]	= char2Chtype (choices[x], &selection->choicelen[x], &junk2);
      selection->maxchoicelen	= MAXIMUM (selection->maxchoicelen, selection->choicelen[x]);
   }
   
   /* Each item in the needs to be converted to chtype * */
   for (x=0; x < listSize; x++)
   {
      selection->item[x] = char2Chtype (list[x], 
					&selection->itemLen[x],
					&selection->itemPos[x]);
      selection->itemPos[x] = justifyString ((selection->boxWidth-selection->maxchoicelen), 
						selection->itemLen[x], 
						selection->itemPos[x]) + selection->maxchoicelen;
      selection->selections[x] = 0;
      widestitem = MAXIMUM (widestitem, selection->itemLen[x]);

      /* Set the mode of the selection lists. */
      selection->mode[x] = 0;
   }
   
  /*
   * Determine how many characters we can shift to the right
   * before all the items have been scrolled off the screen.
   */
   if ((selection->boxWidth-selection->maxchoicelen) > widestitem)
   {
      selection->maxLeftChar = 0;
   }
   else
   {
      selection->maxLeftChar = widestitem-(selection->boxWidth-selection->maxchoicelen-2);
   }
   
   /* Do we need to create a shadow. */
   if (shadow)
   {
      selection->shadowWin = newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
   }
   
   /* Clean the key bindings. */
   cleanCDKObjectBindings (vSELECTION, selection);
   
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
   
   /* Check if actions is NULL. */
   if (actions == (chtype *)NULL)
   {
      /* Declare some local variables. */
      chtype input;
      int ret;
   
      for (;;)
      {
         /* Get the input. */
         input = wgetch (selection->win);
   
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
      /* Declare some local variables. */
      int length = chlen (actions);
      int x = 0;
      int ret;
   
      /* Inject each character one at a time. */
      for (x=0; x < length; x++)
      {
         ret = injectCDKSelection (selection, actions[x]);
         if (selection->exitType != vEARLY_EXIT)
         {
            return ret;
         }
      }
   }

   /* Set the exit type and return. */
   selection->exitType = vEARLY_EXIT;
   return 0;
}
   
/*
 * This injects a single character into the widget.
 */
int injectCDKSelection (CDKSELECTION *selection, chtype input)
{
   /* Declare local variables. */
   int ppReturn = 1;
   
   /* Set the exit type. */
   selection->exitType = vEARLY_EXIT;

   /* Draw the selection list */
   drawCDKSelectionList (selection, ObjOf(selection)->box);
   
   /* Check if there is a pre-process function to be called. */
   if (selection->preProcessFunction != (PROCESSFN)NULL)
   {
      /* Call the pre-process function. */
      ppReturn = ((PROCESSFN)(selection->preProcessFunction)) (vSELECTION, selection, selection->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a predefined binding. */
      if (checkCDKObjectBind (vSELECTION, selection, input) != 0)
      {
         selection->exitType = vESCAPE_HIT;
         return -1;
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
         
               case KEY_PPAGE : case '' :
                    if (selection->currentTop > 0)
                    {
                       if (selection->currentTop >= (selection->viewSize -1))
                       {
                          selection->currentTop	-= (selection->viewSize - 1);
                          selection->currentItem	-= (selection->viewSize - 1);
                       }
                       else
                       {
                          selection->currentTop 	= 0;
                          selection->currentItem	= 0;
                          selection->currentHigh	= 0;
                       }
                    }
                    else
                    {
                       Beep();
                    }
                    break;
         
               case KEY_NPAGE : case '' :
                    if (selection->currentTop < selection->maxTopItem)
                    {
                       if ((selection->currentTop + selection->viewSize - 1) <= selection->maxTopItem)
                       {
                          selection->currentTop	+= (selection->viewSize - 1);
                          selection->currentItem	+= (selection->viewSize - 1);
                       }
                       else
                       {
                          selection->currentTop 	= selection->maxTopItem;
                          selection->currentItem	= selection->lastItem;
                          selection->currentHigh	= selection->viewSize-1;
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
                    selection->currentTop 	= selection->maxTopItem;
                    selection->currentItem	= selection->lastItem;
                    selection->currentHigh	= selection->viewSize-1;
                    break;
         
               case '$' :
                    selection->leftChar	= selection->maxLeftChar;
                    break;
         
               case '|' :
                    selection->leftChar	= 0;
                    break;
         
               case SPACE :
                    if (selection->mode[selection->currentItem] == 0)
                    {
                       if (selection->selections[selection->currentItem] == selection->choiceCount-1)
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
                    selection->exitType = vESCAPE_HIT;
                    return -1;
         
               case KEY_RETURN : case KEY_TAB : case KEY_ENTER :
                    selection->exitType = vNORMAL;
                    return 1;
         
               case CDK_REFRESH :
                    eraseCDKScreen (ScreenOf(selection));
                    refreshCDKScreen (ScreenOf(selection));
                    break;
         
               default :
                    break;
         }
      }

      /* Should we call a post-process? */
      if (selection->postProcessFunction != (PROCESSFN)NULL)
      {
         ((PROCESSFN)(selection->postProcessFunction)) (vSELECTION, selection, selection->postProcessData, input);
      }
   }
      
   /* Redraw the list */
   drawCDKSelectionList (selection, ObjOf(selection)->box);

   /* Set the exit type and return. */
   selection->exitType = vEARLY_EXIT;
   return -1;
}
   
/*
 * This moves the selection field to the given location.
 */
void moveCDKSelection (CDKSELECTION *selection, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   /* Declare local variables. */
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

   /* If there is a scroll bar we have to move it too. */
   if (selection->scrollbar)
   {
      moveCursesWindow(selection->scrollbarWin, -xdiff, -ydiff);
   }

   /* If there is a shadow box we have to move it too. */
   if (selection->shadowWin != (WINDOW *)NULL)
   {
      moveCursesWindow(selection->shadowWin, -xdiff, -ydiff);
   }

   /* Touch the windows so they 'move'. */
   touchwin (WindowOf(selection));
   wrefresh (WindowOf(selection));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKSelection (selection, ObjOf(selection)->box);
   }
}

/*
 * This allows the user to use the cursor keys to adjust the
 * position of the widget.
 */
void positionCDKSelection (CDKSELECTION *selection)
{
   /* Declare some variables. */
   int origX	= getbegx(selection->win);
   int origY	= getbegy(selection->win);
   chtype key	= (chtype)NULL;

   /* Let them move the widget around until they hit return. */
   while ((key != KEY_RETURN) && (key != KEY_ENTER))
   {
      key = wgetch (selection->win);
      if (key == KEY_UP || key == '8')
      {
         if (getbegy(selection->win) > 0)
         {
            moveCDKSelection (selection, 0, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_DOWN || key == '2')
      {
         if (getendy(selection->win) < getmaxy(WindowOf(selection))-1)
         {
            moveCDKSelection (selection, 0, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_LEFT || key == '4')
      {
         if (getbegx(selection->win) > 0)
         {
            moveCDKSelection (selection, -1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_RIGHT || key == '6')
      {
         if (getendx(selection->win) < getmaxx(WindowOf(selection))-1)
         {
            moveCDKSelection (selection, 1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '7')
      {
         if (getbegy(selection->win) > 0 && getbegx(selection->win) > 0)
         {
            moveCDKSelection (selection, -1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '9')
      {
         if (getendx(selection->win) < getmaxx(WindowOf(selection))-1
	  && getbegy(selection->win) > 0)
         {
            moveCDKSelection (selection, 1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '1')
      {
         if (getbegx(selection->win) > 0 && getendx(selection->win) < getmaxx(WindowOf(selection))-1)
         {
            moveCDKSelection (selection, -1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '3')
      {
         if (getendx(selection->win) < getmaxx(WindowOf(selection))-1
	  && getendy(selection->win) < getmaxy(WindowOf(selection))-1)
         {
            moveCDKSelection (selection, 1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '5')
      {
         moveCDKSelection (selection, CENTER, CENTER, FALSE, TRUE);
      }
      else if (key == 't')
      {
         moveCDKSelection (selection, getbegx(selection->win), TOP, FALSE, TRUE);
      }
      else if (key == 'b')
      {
         moveCDKSelection (selection, getbegx(selection->win), BOTTOM, FALSE, TRUE);
      }
      else if (key == 'l')
      {
         moveCDKSelection (selection, LEFT, getbegy(selection->win), FALSE, TRUE);
      }
      else if (key == 'r')
      {
         moveCDKSelection (selection, RIGHT, getbegy(selection->win), FALSE, TRUE);
      }
      else if (key == 'c')
      {
         moveCDKSelection (selection, CENTER, getbegy(selection->win), FALSE, TRUE);
      }
      else if (key == 'C')
      {
         moveCDKSelection (selection, getbegx(selection->win), CENTER, FALSE, TRUE);
      }
      else if (key == CDK_REFRESH)
      {
         eraseCDKScreen (ScreenOf(selection));
         refreshCDKScreen (ScreenOf(selection));
      }
      else if (key == KEY_ESC)
      {
         moveCDKSelection (selection, origX, origY, FALSE, TRUE);
      }
      else if ((key != KEY_RETURN) && (key != KEY_ENTER))
      {
         Beep();
      }
   }
}

/*
 * This function draws the selection list.
 */
void _drawCDKSelection (CDKOBJS *object, boolean Box)
{
   CDKSELECTION *selection = (CDKSELECTION *)object;
   int x;

   /* Draw in the shadow if we need to. */
   if (selection->shadowWin != (WINDOW *)NULL)
   {
      drawShadow (selection->shadowWin);
   }
   
   /* Draw in the title if there is one. */
   if (selection->titleLines != 0)
   {
      for (x=0; x < selection->titleLines; x++)
      {
         writeChtype (selection->win, 
   			selection->titlePos[x],
			x + 1,
   			selection->title[x],
   			HORIZONTAL, 0,
   			selection->titleLen[x]);
      }
   }
   
   /* Redraw the list */
   drawCDKSelectionList (selection, Box);
}
  
/*
 * This function draws the selection list window.
 */
static void drawCDKSelectionList (CDKSELECTION *selection, boolean Box GCC_UNUSED)
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
   if (selection->scrollbarPlacement == LEFT)
   {
      scrollbarAdj = 1;
   }

   /* Empty out the string. */
   cleanChar (emptyString, selection->boxWidth, ' ');
   
   /* Redraw the list... */
   for (x=0; x < selection->viewSize; x++)
   {
      screenPos = selection->itemPos[x + selection->currentTop] - selection->leftChar + scrollbarAdj;

      /* Draw in the empty line. */
      writeChar (selection->win, 1,
   			selection->titleAdj + x,
   			emptyString,
   			HORIZONTAL, 0,
   			(int)strlen(emptyString));
   
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
   if (selection->scrollbarWin != (WINDOW *)NULL)
   {
      for (x=0; x < selection->boxHeight-1; x++)
      {
         mvwaddch (selection->scrollbarWin, x, 0, ACS_CKBOARD);
      }
   }

   /* Draw in the current highlight. */
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
      if (selection->listSize > selection->boxHeight-2)
      {
         selection->togglePos = floorCDK((float)selection->currentItem * (float)selection->step);
      }
      else
      {
         selection->togglePos = ceilCDK((float)selection->currentItem * (float)selection->step);
      }

      /* Make sure the toggle button doesn't go out of bounds. */
      scrollbarAdj = (selection->togglePos + selection->toggleSize)-(selection->boxHeight-selection->titleAdj-1);
      if (scrollbarAdj > 0)
      {
         selection->togglePos -= scrollbarAdj;
      }

      /* Draw the scrollbar. */
      for (x=selection->togglePos; x < selection->togglePos + selection->toggleSize; x++)
      {
         mvwaddch (selection->scrollbarWin, x, 0, ' ' | A_REVERSE);
      }
      touchwin (selection->scrollbarWin);
      wrefresh (selection->scrollbarWin);
   }

   /* Box it if needed */
   if (ObjOf(selection)->box)
   {
      attrbox (selection->win,
		selection->ULChar, selection->URChar,
		selection->LLChar, selection->LRChar,
		selection->HChar,  selection->VChar,
		selection->BoxAttrib);
   }
   
   /* Refresh the window. */
   touchwin (selection->win);
   wrefresh (selection->win);
}
   
/*
 * These functions set the drawing characters of the widget.
 */
void setCDKSelectionULChar (CDKSELECTION *selection, chtype character)
{
   selection->ULChar = character;
}
void setCDKSelectionURChar (CDKSELECTION *selection, chtype character)
{
   selection->URChar = character;
}
void setCDKSelectionLLChar (CDKSELECTION *selection, chtype character)
{
   selection->LLChar = character;
}
void setCDKSelectionLRChar (CDKSELECTION *selection, chtype character)
{
   selection->LRChar = character;
}
void setCDKSelectionVerticalChar (CDKSELECTION *selection, chtype character)
{
   selection->VChar = character;
}
void setCDKSelectionHorizontalChar (CDKSELECTION *selection, chtype character)
{
   selection->HChar = character;
}
void setCDKSelectionBoxAttribute (CDKSELECTION *selection, chtype character)
{
   selection->BoxAttrib = character;
}

/*
 * This sets the background color of the widget.
 */ 
void setCDKSelectionBackgroundColor (CDKSELECTION *selection, char *color)
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
   wbkgd (selection->win, holder[0]);
   if (selection->scrollbarWin != (WINDOW *)NULL)
   {
      wbkgd (selection->scrollbarWin, holder[0]);
   }

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This function destroys the selection list.
 */
void destroyCDKSelection (CDKSELECTION *selection)
{
   /* Declare local variables. */
   int x;
   
   /* Erase the object. */
   eraseCDKSelection (selection);
   
   /* Clean up the char pointers. */
   for (x=0; x < selection->titleLines; x++)
   {
      freeChtype (selection->title[x]);
   }
   for (x=0; x < selection->listSize; x++)
   {
      freeChtype (selection->item[x]);
   }
   for (x=0; x < selection->choiceCount; x++)
   {
      freeChtype (selection->choice[x]);
   }
   
   /* Clean up the windows. */
   deleteCursesWindow (selection->scrollbarWin);
   deleteCursesWindow (selection->shadowWin);
   deleteCursesWindow (selection->win);
   
   /* Unregister this object. */
   unregisterCDKObject (vSELECTION, selection);
   
   /* Finish cleaning up. */
   free (selection);
}
   
/*
 * This function erases the selection list from the screen.
 */
void _eraseCDKSelection (CDKOBJS *object)
{
   CDKSELECTION *selection = (CDKSELECTION *)object;

   eraseCursesWindow (selection->win);
   eraseCursesWindow (selection->shadowWin);
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
   /* Declare some wars. */
   char emptyString[2000];
   int widestitem	= -1;
   int x		= 0;

   /* Clean out the old list. */
   for (x=0; x < selection->listSize; x++)
   {
      freeChtype (selection->item[x]);
      selection->itemLen[x] = 0;
      selection->itemPos[x] = 0;
   }

   /* Clean up the display. */
   cleanChar (emptyString, selection->boxWidth-1, ' ');
   for (x=0; x < selection->viewSize ; x++)
   {
      writeChar (selection->win, 1, selection->titleAdj + x, emptyString, 
			HORIZONTAL, 0, (int)strlen(emptyString));
   }

   /* Readjust all of the variables ... */
   selection->listSize		= listSize;
   selection->viewSize		= selection->boxHeight - (2 + selection->titleLines);
   selection->lastItem		= listSize - 1;
   selection->maxTopItem	= listSize - selection->viewSize;

   /* Is the view size smaller then the window? */
   if (listSize < (selection->boxHeight-1-selection->titleAdj))
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
   selection->step		= (float)(selection->boxHeight-2) / (float)selection->listSize;
   selection->toggleSize	= (selection->listSize > (selection->boxHeight-2) ? 1 : ceilCDK(selection->step));

   /* Each item in the needs to be converted to chtype * */
   for (x=0; x < listSize; x++)
   {
      selection->item[x] = char2Chtype (list[x], 
					&selection->itemLen[x],
					&selection->itemPos[x]);
      selection->itemPos[x] = justifyString ((selection->boxWidth-selection->maxchoicelen), 
						selection->itemLen[x], 
						selection->itemPos[x]) + selection->maxchoicelen;
      selection->selections[x] = 0;
      widestitem = MAXIMUM (widestitem, selection->itemLen[x]);

      /* Set the mode of the selection lists. */
      selection->mode[x] = 0;
   }
   
  /*
   * Determine how many characters we can shift to the right
   * before all the items have been scrolled off the screen.
   */
   if ((selection->boxWidth-selection->maxchoicelen) > widestitem)
   {
      selection->maxLeftChar = 0;
   }
   else
   {
      selection->maxLeftChar = widestitem-(selection->boxWidth-selection->maxchoicelen-2);
   }
}
int getCDKSelectionItems (CDKSELECTION *selection, char *list[])
{
   int x;

   for (x=0; x < selection->listSize; x++)
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
   char *temp[MAX_LINES];
   int x;

   /* Make sure the title isn't NULL. */
   if (title == (char *)NULL)
   {
      return;
   }

   /* Clear out the old title. */
   for (x=0; x < selection->titleLines; x++)
   {
      freeChtype (selection->title[x]);
   }

   /* We need to split the title on \n. */
   selection->titleLines = splitString (title, temp, '\n');

   /* For each line in the title, convert from char * to chtype * */
   for (x=0; x < selection->titleLines; x++)
   {
      selection->title[x] = char2Chtype (temp[x], 
					&selection->titleLen[x], 
					&selection->titlePos[x]);
      selection->titlePos[x] = justifyString (selection->boxWidth, 
						selection->titleLen[x], 
						selection->titlePos[x]);
      freeChar (temp[x]);
   }

   /* Set the rest of the variables. */
   selection->titleAdj		= selection->titleLines + 1;
   selection->viewSize		= selection->boxHeight - (2 + selection->titleLines);
   selection->maxTopItem	= selection->listSize - selection->viewSize;
   selection->maxchoicelen	= -1;
 
   /* Is the view size smaller then the window??? */
   if (selection->listSize < (selection->boxHeight-1-selection->titleAdj))
   {
      selection->viewSize	= selection->listSize;
      selection->lastItem	= selection->listSize;
      selection->maxTopItem	= -1;
   }

   /* Determine the size of the scrollbar toggle and the step. */
   selection->step		= (float)(selection->boxHeight-2) / (float)selection->listSize;
   selection->toggleSize	= (selection->listSize > (selection->boxHeight-2) ? 1 : ceilCDK(selection->step));
}
char *getCDKSelectionTitle (CDKSELECTION *selection GCC_UNUSED)
{
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
   /* Declare local variables. */
   int x;
   
   /* Set the choice values in the selection list. */
   for (x=0; x < selection->listSize; x++)
   {
      if (choices[x] < 0)
      {
         selection->selections[x] = 0;
      }
      else if (choices[x] > selection->choiceCount)
      {
         selection->selections[x] = selection->choiceCount-1;
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
      correctChoice = selection->choiceCount-1;
   }

   /* Make sure the index isn't out of range. */
   if (Index < 0)
   {
      Index = 0;
   }
   else if (Index > selection->listSize)
   {
      Index = selection->listSize-1;
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
      return selection->selections[selection->listSize-1];
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

   /* Make sure the widget pointer is not NULL. */
   if (selection == (CDKSELECTION *)NULL)
   {
      return;
   }

   /* Set the modes. */
   for (x=0; x < selection->listSize; x++)
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
   /* Make sure the widget pointer is not NULL. */
   if (selection == (CDKSELECTION *)NULL)
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
      selection->mode[selection->listSize-1] = mode;
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
      return selection->mode[selection->listSize-1];
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
}
boolean getCDKSelectionBox (CDKSELECTION *selection)
{
   return ObjOf(selection)->box;
}

/*
 * This function sets the pre-process function.
 */
void setCDKSelectionPreProcess (CDKSELECTION *selection, PROCESSFN callback, void *data)
{
   selection->preProcessFunction = callback;
   selection->preProcessData = data;
}
 
/*
 * This function sets the post-process function.
 */
void setCDKSelectionPostProcess (CDKSELECTION *selection, PROCESSFN callback, void *data)
{
   selection->postProcessFunction = callback;
   selection->postProcessData = data;
}
