#include "cdk.h"
#include <limits.h>

/*
 * $Author: tom $
 * $Date: 1999/05/23 02:53:29 $
 * $Revision: 1.61 $
 */

/*
 * Declare file local prototypes.
 */
static void drawCDKSwindowList (CDKSWINDOW *swindow, boolean Box);

static CDKFUNCS my_funcs = {
    _drawCDKSwindow,
    _eraseCDKSwindow,
};

/*
 * This function creates a scrolling window widget.
 */
CDKSWINDOW *newCDKSwindow (CDKSCREEN *cdkscreen, int xplace, int yplace, int height, int width, char *title, int saveLines, boolean Box, boolean shadow)
{
   /* Declare local variables. */
   CDKSWINDOW	*swindow	= newCDKObject(CDKSWINDOW, &my_funcs);
   int parentWidth		= getmaxx(cdkscreen->window) - 1;
   int parentHeight		= getmaxy(cdkscreen->window) - 1;
   int boxWidth			= width;
   int boxHeight		= height;
   int xpos			= xplace;
   int ypos			= yplace;
   char *temp[MAX_LINES];
   int x;

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
      swindow->titleLines = splitString (title, temp, '\n');

      /* For each line in the title, convert from char * to chtype * */
      for (x=0; x < swindow->titleLines; x++)
      {
         swindow->title[x]	= char2Chtype (temp[x], &swindow->titleLen[x], &swindow->titlePos[x]);
         swindow->titlePos[x]	= justifyString (boxWidth, swindow->titleLen[x], swindow->titlePos[x]);
         freeChar (temp[x]);
      }
   }
   else
   {
      /* No title? Set the required variables. */
      swindow->titleLines = 0;
   }

   /* Set the box height. */
   boxHeight += swindow->titleLines + 1;

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);

   /* Set the rest of the variables. */
   swindow->titleAdj = swindow->titleLines + 1;

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the scrolling window */
   swindow->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the window NULL?? */
   if (swindow->win == (WINDOW *)NULL)
   {
      /* Clean up. */
      for (x=0; x < swindow->titleLines; x++)
      {
         freeChtype (swindow->title[x]);
      }
      free(swindow);

      /* Return a NULL pointer. */
      return ((CDKSWINDOW *)NULL);
   }
   keypad (swindow->win, TRUE);

  /* Make the field window. */
   swindow->fieldWin = subwin (swindow->win,
				(boxHeight-swindow->titleLines-2),
				boxWidth - 2,
                                ypos + swindow->titleLines + 1,
                                xpos + 1);
   keypad (swindow->fieldWin, TRUE);

   /* Set the rest of the variables */
   ScreenOf(swindow)		= cdkscreen;
   swindow->parent		= cdkscreen->window;
   swindow->shadowWin		= (WINDOW *)NULL;
   swindow->boxHeight		= boxHeight;
   swindow->boxWidth		= boxWidth;
   swindow->viewSize		= boxHeight-swindow->titleLines-2;
   swindow->currentTop		= 0;
   swindow->maxTopLine		= 0;
   swindow->leftChar		= 0;
   swindow->maxLeftChar		= 0;
   swindow->itemCount		= 0;
   swindow->widestLine		= -1;
   swindow->saveLines		= saveLines;
   swindow->exitType		= vNEVER_ACTIVATED;
   ObjOf(swindow)->box		= Box;
   swindow->shadow		= shadow;
   swindow->preProcessFunction	= (PROCESSFN)NULL;
   swindow->preProcessData	= (void *)NULL;
   swindow->postProcessFunction	= (PROCESSFN)NULL;
   swindow->postProcessData	= (void *)NULL;
   swindow->ULChar		= ACS_ULCORNER;
   swindow->URChar		= ACS_URCORNER;
   swindow->LLChar		= ACS_LLCORNER;
   swindow->LRChar		= ACS_LRCORNER;
   swindow->HChar		= ACS_HLINE;
   swindow->VChar		= ACS_VLINE;
   swindow->BoxAttrib		= A_NORMAL;

   /* For each line in the window, set the value to NULL. */
   for (x=0; x < MAX_LINES; x++)
   {
      swindow->info[x] = (chtype *)NULL;
   }

   /* Do we need to create a shadow??? */
   if (shadow)
   {
      swindow->shadowWin = newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
   }

   /* Clean the key bindings. */
   cleanCDKObjectBindings (vSWINDOW, swindow);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vSWINDOW, swindow);

   /* Return the scrolling window */
   return (swindow);
}

/*
 * This sets the lines and the box attribute of the scrolling window.
 */
void setCDKSwindow (CDKSWINDOW *swindow, char **info, int lines, boolean Box)
{
   setCDKSwindowContents (swindow, info, lines);
   setCDKSwindowBox (swindow, Box);
}

/*
 * This sets all the lines inside the scrolling window.
 */
void setCDKSwindowContents (CDKSWINDOW *swindow, char **info, int lines)
{
   /* Declare local variables. */
   int x = 0;

   /* First lets clean all the lines in the window. */
   cleanCDKSwindow(swindow);

   /* Now lets set all the lines inside the window. */
   for (x=0; x < lines; x++)
   {
      swindow->info[x]		= char2Chtype (info[x], &swindow->infoLen[x], &swindow->infoPos[x]);
      swindow->infoPos[x]	= justifyString (swindow->boxWidth, swindow->infoLen[x], swindow->infoPos[x]);
      swindow->widestLine	= MAXIMUM (swindow->widestLine, swindow->infoLen[x]);
   }

   /* Set some of the more important members of the scrolling window. */
   swindow->itemCount	= lines;
   swindow->maxTopLine	= swindow->itemCount - swindow->viewSize;
   swindow->maxTopLine	= (swindow->maxTopLine < 0 ? 0 : swindow->maxTopLine);
   swindow->maxLeftChar	= swindow->widestLine - (swindow->boxWidth - 2);
   swindow->currentTop	= 0;
   swindow->leftChar	= 0;
}
chtype **getCDKSwindowContents (CDKSWINDOW *swindow, int *size)
{
   (*size) = swindow->itemCount;
   return swindow->info;
}

/*
 * This sets the box attribute for the widget.
 */
void setCDKSwindowBox (CDKSWINDOW *swindow, boolean Box)
{
   ObjOf(swindow)->box = Box;
}
boolean getCDKSwindowBox (CDKSWINDOW *swindow)
{
   return ObjOf(swindow)->box;
}

/*
 * This adds a line to the scrolling window.
 */
void addCDKSwindow  (CDKSWINDOW *swindow, char *info, int insertPos)
{
   /* Declare variables. */
   int x = 0;

  /*
   * If we are at the maximum number of save lines. Erase
   * the first position and bump everything up one spot.
   */
   if (swindow->itemCount == swindow->saveLines)
   {
      /* Free up the memory. */
      freeChtype (swindow->info[0]);

      /* Bump everything up one spot. */
      for (x=0; x < swindow->itemCount; x++)
      {
         swindow->info[x] = swindow->info[x + 1];
         swindow->infoPos[x] = swindow->infoPos[x + 1];
         swindow->infoLen[x] = swindow->infoLen[x + 1];
      }

      /* Clean out the last position. */
      swindow->info[swindow->itemCount] = '\0';
      swindow->infoLen[swindow->itemCount] = 0;
      swindow->infoPos[swindow->itemCount] = 0;
      swindow->itemCount--;
   }

   /* Determine where the line is being added. */
   if (insertPos == TOP)
   {
      /* We need to 'bump' everything down one line... */
      for (x=swindow->itemCount; x > 0; x--)
      {
         /* Copy in the new row. */
         swindow->info[x] = swindow->info[x-1];
         swindow->infoPos[x] = swindow->infoPos[x-1];
         swindow->infoLen[x] = swindow->infoLen[x-1];
      }

      /* Add it into the scrolling window. */
      swindow->info[0] = char2Chtype (info, &swindow->infoLen[0], &swindow->infoPos[0]);
      swindow->infoPos[0] = justifyString (swindow->boxWidth, swindow->infoLen[0], swindow->infoPos[0]);

      /* Set some variables. */
      swindow->currentTop = 0;
      if (swindow->itemCount < swindow->saveLines)
      {
         swindow->itemCount++;
      }

      /* Set the maximum top line. */
      swindow->maxTopLine = swindow->itemCount - swindow->viewSize;
      swindow->maxTopLine = (swindow->maxTopLine < 0 ? 0 : swindow->maxTopLine);

      /* Lets determine the widest line and the maximum leftmost character.  */
      swindow->widestLine = MAXIMUM (swindow->widestLine, swindow->infoLen[0]);
      swindow->maxLeftChar = swindow->widestLine - (swindow->boxWidth - 2);
   }
   else
   {
      /* Add to the bottom. */
      swindow->info[swindow->itemCount]	 = char2Chtype (info, &swindow->infoLen[swindow->itemCount], &swindow->infoPos[swindow->itemCount]);
      swindow->infoPos[swindow->itemCount] = justifyString (swindow->boxWidth, swindow->infoLen[swindow->itemCount], swindow->infoPos[swindow->itemCount]);

      /* Lets determine the widest line and the maximum leftmost character.  */
      swindow->widestLine = MAXIMUM (swindow->widestLine, swindow->infoLen[swindow->itemCount]);
      swindow->maxLeftChar = swindow->widestLine - (swindow->boxWidth - 2);

      /* Increment the item count and zero out the next row. */
      if (swindow->itemCount < swindow->saveLines)
      {
         swindow->itemCount++;
         freeChtype (swindow->info[swindow->itemCount]);
      }

      /* Set the maximum top line. */
      if (swindow->itemCount <= swindow->viewSize)
      {
         swindow->maxTopLine = 0;
         swindow->currentTop = 0;
      }
      else
      {
         swindow->maxTopLine = (swindow->itemCount - swindow->viewSize);
         swindow->currentTop = swindow->maxTopLine;
      }
   }

   /* Draw in the list. */
   drawCDKSwindowList (swindow, ObjOf(swindow)->box);
}

/*
 * This jumps to a given line.
 */
void jumpToLineCDKSwindow (CDKSWINDOW *swindow, int line)
{
  /*
   * Make sure the line is in bounds.
   */
   if (line == BOTTOM || line >= swindow->itemCount)
   {
      /* We are moving to the last page. */
      swindow->currentTop = swindow->itemCount - swindow->viewSize;
   }
   else if (line == TOP || line <= 0)
   {
      /* We are moving to the top of the page. */
      swindow->currentTop = 0;
   }
   else
   {
      /* We are moving in the middle somewhere. */
      if ((swindow->viewSize + line) < swindow->itemCount)
      {
         swindow->currentTop = line;
      }
      else
      {
         swindow->currentTop = swindow->itemCount - swindow->viewSize;
      }
   }

   /* A little sanity check to make we don't something silly. */
   if (swindow->currentTop < 0)
   {
      swindow->currentTop = 0;
   }

   /* Redraw the window. */
   drawCDKSwindow (swindow, ObjOf(swindow)->box);
}

/*
 * This removes all the lines inside the scrolling window.
 */
void cleanCDKSwindow (CDKSWINDOW *swindow)
{
   /* Declare local variables. */
   int x;

   /* Clean up the memory used ... */
   for (x=0; x < swindow->itemCount; x++)
   {
      freeChtype (swindow->info[x]);
   }

   /* Reset some variables. */
   swindow->itemCount	= 0;
   swindow->maxLeftChar	= 0;
   swindow->widestLine	= 0;
   swindow->currentTop	= 0;
   swindow->maxTopLine	= 0;

   /* Redraw the window. */
   drawCDKSwindow (swindow, ObjOf(swindow)->box);
}

/*
 * This trims lines from the scrolling window.
 */
void trimCDKSwindow (CDKSWINDOW *swindow, int begin, int end)
{
   int start, finish, x;

   /* Check the value of begin. */
   if (begin < 0)
   {
      start = 0;
   }
   else if (begin >= swindow->itemCount)
   {
      start = swindow->itemCount-1;
   }
   else
   {
      start = begin;
   }

   /* Check the value of end. */
   if (end < 0)
   {
      finish = 0;
   }
   else if (end >= swindow->itemCount)
   {
      finish = swindow->itemCount-1;
   }
   else
   {
      finish = end;
   }

   /* Make sure the start is lower than the end. */
   if (start > finish)
   {
      return;
   }

   /* Start nuking elements from the window. */
   for (x=start; x <=finish; x++)
   {
      freeChtype (swindow->info[x]);

      if (x < swindow->itemCount-1)
      {
         swindow->info[x] = copyChtype (swindow->info[x + 1]);
         swindow->infoPos[x] = swindow->infoPos[x + 1];
         swindow->infoLen[x] = swindow->infoLen[x + 1];
      }
   }

   /* Adjust the item count correctly. */
   swindow->itemCount = swindow->itemCount - (end - begin) - 1;

   /* Redraw the window. */
   drawCDKSwindow (swindow, ObjOf(swindow)->box);
}

/*
 * This allows the user to play inside the scolling window.
 */
void activateCDKSwindow (CDKSWINDOW *swindow, chtype *actions)
{
   /* Draw the scrolling list */
   drawCDKSwindow (swindow, ObjOf(swindow)->box);

   /* Check if actions is NULL. */
   if (actions == (chtype *)NULL)
   {
      /* Declare some local variables. */
      chtype input;
      int ret;

      for (;;)
      {
         /* Get the input. */
         input = wgetch (swindow->win);

         /* Inject the character into the widget. */
         ret = injectCDKSwindow (swindow, input);
         if (swindow->exitType != vEARLY_EXIT)
         {
            return;
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
         ret = injectCDKSwindow (swindow, actions[x]);
         if (swindow->exitType != vEARLY_EXIT)
         {
            return;
         }
      }
   }

   /* Set the exit type and return. */
   swindow->exitType = vEARLY_EXIT;
   return;
}

/*
 * This injects a single character into the widget.
 */
int injectCDKSwindow (CDKSWINDOW *swindow, chtype input)
{
   /* Declare local variables. */
   int ppReturn = 1;

   /* Draw the window.... */
   drawCDKSwindow (swindow, ObjOf(swindow)->box);

   /* Check if there is a pre-process function to be called. */
   if (swindow->preProcessFunction != (PROCESSFN)NULL)
   {
      /* Call the pre-process function. */
      ppReturn = ((PROCESSFN)(swindow->preProcessFunction)) (vSWINDOW, swindow, swindow->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a key binding. */
      if (checkCDKObjectBind (vSWINDOW, swindow, input) != 0)
      {
         swindow->exitType = vESCAPE_HIT;
         return -1;
      }
      else
      {
         switch (input)
         {
            case KEY_UP :
                 if (swindow->currentTop > 0)
                 {
                    swindow->currentTop--;
                 }
                 else
                 {
                    Beep();
                 }
                 break;

            case KEY_DOWN :
                 if (swindow->currentTop >= 0 && swindow->currentTop < swindow->maxTopLine)
                 {
                    swindow->currentTop++;
                 }
                 else
                 {
                    Beep();
                 }
                 break;

            case KEY_RIGHT :
                 if (swindow->leftChar < swindow->maxLeftChar)
                 {
                    swindow->leftChar ++;
                 }
                 else
                 {
                    Beep();
                 }
                 break;

            case KEY_LEFT :
                 if (swindow->leftChar > 0)
                 {
                    swindow->leftChar--;
                 }
                 else
                 {
                    Beep();
                 }
                 break;

            case KEY_PPAGE : case '' : case 'b' : case 'B' :
                 if (swindow->currentTop != 0)
                 {
                    if (swindow->currentTop >= swindow->viewSize)
                    {
                       swindow->currentTop	= swindow->currentTop - (swindow->viewSize - 1);
                    }
                    else
                    {
                       swindow->currentTop 	= 0;
                    }
                 }
                 else
                 {
                    Beep();
                 }
                 break;

            case KEY_NPAGE : case '' : case ' ' : case 'f' : case 'F' :
                 if (swindow->currentTop != swindow->maxTopLine)
                 {
                    if ((swindow->currentTop + swindow->viewSize) < swindow->maxTopLine)
                    {
                       swindow->currentTop	= swindow->currentTop + (swindow->viewSize - 1);
                    }
                    else
                    {
                       swindow->currentTop	= swindow->maxTopLine;
                    }
                 }
                 else
                 {
                    Beep();
                 }
                 break;

            case KEY_HOME : case '|' :
                 swindow->leftChar = 0;
                 break;

            case KEY_END : case '$' :
                 swindow->leftChar = swindow->maxLeftChar + 1;
                 break;

            case 'g' : case '1' :
                 swindow->currentTop = 0;
                 break;

            case 'G' :
                 swindow->currentTop = swindow->maxTopLine;
                 break;

            case 'l' : case 'L' :
	         loadCDKSwindowInformation (swindow);
                 break;

            case 's' : case 'S' :
 	         saveCDKSwindowInformation (swindow);
	         break;

            case KEY_RETURN : case KEY_TAB : case KEY_ENTER :
                 swindow->exitType = vNORMAL;
                 return 1;

            case KEY_ESC :
                 swindow->exitType = vESCAPE_HIT;
                 return -1;

            case CDK_REFRESH :
                 eraseCDKScreen (ScreenOf(swindow));
                 refreshCDKScreen (ScreenOf(swindow));
                 break;

            default :
                 break;
         }
      }

      /* Should we call a post-process? */
      if (swindow->postProcessFunction != (PROCESSFN)NULL)
      {
         ((PROCESSFN)(swindow->postProcessFunction)) (vSWINDOW, swindow, swindow->postProcessData, input);
      }
   }

   /* Redraw the list */
   drawCDKSwindowList (swindow, ObjOf(swindow)->box);

   /* Set the exit type and return. */
   swindow->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This moves the swindow field to the given location.
 */
void moveCDKSwindow (CDKSWINDOW *swindow, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   /* Declare local variables. */
   int currentX = getbegx(swindow->win);
   int currentY = getbegy(swindow->win);
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
      xpos = getbegx(swindow->win) + xplace;
      ypos = getbegy(swindow->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(swindow), &xpos, &ypos, swindow->boxWidth, swindow->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(swindow->win, -xdiff, -ydiff);

   /* If there is a shadow box we have to move it too. */
   if (swindow->shadowWin != (WINDOW *)NULL)
   {
      moveCursesWindow(swindow->shadowWin, -xdiff, -ydiff);
   }

   /* Touch the windows so they 'move'. */
   touchwin (WindowOf(swindow));
   wrefresh (WindowOf(swindow));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKSwindow (swindow, ObjOf(swindow)->box);
   }
}

/*
 * This allows the user to use the cursor keys to adjust the
 * position of the widget.
 */
void positionCDKSwindow (CDKSWINDOW *swindow)
{
   /* Declare some variables. */
   int origX	= getbegx(swindow->win);
   int origY	= getbegy(swindow->win);
   chtype key	= (chtype)NULL;

   /* Let them move the widget around until they hit return. */
   while ((key != KEY_RETURN) && (key != KEY_ENTER))
   {
      key = wgetch (swindow->win);
      if (key == KEY_UP || key == '8')
      {
         if (getbegy(swindow->win) > 0)
         {
            moveCDKSwindow (swindow, 0, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_DOWN || key == '2')
      {
         if (getendy(swindow->win) < getmaxy(WindowOf(swindow))-1)
         {
            moveCDKSwindow (swindow, 0, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_LEFT || key == '4')
      {
         if (getbegx(swindow->win) > 0)
         {
            moveCDKSwindow (swindow, -1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_RIGHT || key == '6')
      {
         if (getendx(swindow->win) < getmaxx(WindowOf(swindow))-1)
         {
            moveCDKSwindow (swindow, 1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '7')
      {
         if (getbegy(swindow->win) > 0 && getbegx(swindow->win) > 0)
         {
            moveCDKSwindow (swindow, -1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '9')
      {
         if (getendx(swindow->win) < getmaxx(WindowOf(swindow))-1
	  && getbegy(swindow->win) > 0)
         {
            moveCDKSwindow (swindow, 1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '1')
      {
         if (getbegx(swindow->win) > 0 && getendx(swindow->win) < getmaxx(WindowOf(swindow))-1)
         {
            moveCDKSwindow (swindow, -1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '3')
      {
         if (getendx(swindow->win) < getmaxx(WindowOf(swindow))-1
	  && getendy(swindow->win) < getmaxy(WindowOf(swindow))-1)
         {
            moveCDKSwindow (swindow, 1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '5')
      {
         moveCDKSwindow (swindow, CENTER, CENTER, FALSE, TRUE);
      }
      else if (key == 't')
      {
         moveCDKSwindow (swindow, getbegx(swindow->win), TOP, FALSE, TRUE);
      }
      else if (key == 'b')
      {
         moveCDKSwindow (swindow, getbegx(swindow->win), BOTTOM, FALSE, TRUE);
      }
      else if (key == 'l')
      {
         moveCDKSwindow (swindow, LEFT, getbegy(swindow->win), FALSE, TRUE);
      }
      else if (key == 'r')
      {
         moveCDKSwindow (swindow, RIGHT, getbegy(swindow->win), FALSE, TRUE);
      }
      else if (key == 'c')
      {
         moveCDKSwindow (swindow, CENTER, getbegy(swindow->win), FALSE, TRUE);
      }
      else if (key == 'C')
      {
         moveCDKSwindow (swindow, getbegx(swindow->win), CENTER, FALSE, TRUE);
      }
      else if (key == CDK_REFRESH)
      {
         eraseCDKScreen (ScreenOf(swindow));
         refreshCDKScreen (ScreenOf(swindow));
      }
      else if (key == KEY_ESC)
      {
         moveCDKSwindow (swindow, origX, origY, FALSE, TRUE);
      }
      else if ((key != KEY_RETURN) && (key != KEY_ENTER))
      {
         Beep();
      }
   }
}

/*
 * This function draws the swindow window widget.
 */
void _drawCDKSwindow (CDKOBJS *object, boolean Box)
{
   CDKSWINDOW *swindow = (CDKSWINDOW *)object;
   int x;

   /* Do we need to draw in the shadow. */
   if (swindow->shadowWin != (WINDOW *)NULL)
   {
      drawShadow (swindow->shadowWin);
   }

   /* Box the widget if needed */
   if (Box)
   {
      attrbox (swindow->win,
		swindow->ULChar, swindow->URChar,
		swindow->LLChar, swindow->LRChar,
		swindow->HChar,  swindow->VChar,
		swindow->BoxAttrib);
      wrefresh (swindow->win);
   }

   /* Draw in the title if there is one */
   if (swindow->titleLines != 0)
   {
      for (x=0; x < swindow->titleLines; x++)
      {
         writeChtype (swindow->win,
			swindow->titlePos[x],
			x + 1,
			swindow->title[x],
			HORIZONTAL, 0,
			swindow->titleLen[x]);
      }
   }
   touchwin (swindow->win);
   wrefresh (swindow->win);

   /* Draw in the list. */
   drawCDKSwindowList (swindow, Box);
}

/*
 * This draws in the contents of the scrolling window.
 */
static void drawCDKSwindowList (CDKSWINDOW *swindow, boolean Box GCC_UNUSED)
{
   /* Declare local variables. */
   int lastLine, screenPos, x;

   /* Determine the last line to draw. */
   if (swindow->itemCount < swindow->viewSize)
   {
      lastLine = swindow->itemCount;
   }
   else
   {
      lastLine = swindow->viewSize;
   }

   /* Erase the scrolling window. */
   werase (swindow->fieldWin);

   /* Start drawing in each line. */
   for (x=0; x < lastLine; x++)
   {
      screenPos = swindow->infoPos[x + swindow->currentTop]-swindow->leftChar;

      /* Write in the correct line. */
      if (screenPos >= 0)
      {
         writeChtype (swindow->fieldWin, screenPos, x,
			swindow->info[x + swindow->currentTop],
			HORIZONTAL, 0,
			swindow->infoLen[x + swindow->currentTop]);
      }
      else
      {
         writeChtype (swindow->fieldWin, 0, x,
			swindow->info[x + swindow->currentTop],
			HORIZONTAL,
			swindow->leftChar - swindow->infoPos[x + swindow->currentTop],
			swindow->infoLen[x + swindow->currentTop]);
      }
   }

   /* Reddraw the window. */
   touchwin (swindow->fieldWin);
   wrefresh (swindow->fieldWin);
}

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKSwindowULChar (CDKSWINDOW *swindow, chtype character)
{
   swindow->ULChar = character;
}
void setCDKSwindowURChar (CDKSWINDOW *swindow, chtype character)
{
   swindow->URChar = character;
}
void setCDKSwindowLLChar (CDKSWINDOW *swindow, chtype character)
{
   swindow->LLChar = character;
}
void setCDKSwindowLRChar (CDKSWINDOW *swindow, chtype character)
{
   swindow->LRChar = character;
}
void setCDKSwindowVerticalChar (CDKSWINDOW *swindow, chtype character)
{
   swindow->VChar = character;
}
void setCDKSwindowHorizontalChar (CDKSWINDOW *swindow, chtype character)
{
   swindow->HChar = character;
}
void setCDKSwindowBoxAttribute (CDKSWINDOW *swindow, chtype character)
{
   swindow->BoxAttrib = character;
}

/*
 * This sets the background color of the widget.
 */
void setCDKSwindowBackgroundColor (CDKSWINDOW *swindow, char *color)
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
   wbkgd (swindow->win, holder[0]);
   wbkgd (swindow->fieldWin, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This function destroys the scrolling window widget.
 */
void destroyCDKSwindow (CDKSWINDOW *swindow)
{
   /* Declare local variables. */
   int x;

   /* Erase the object. */
   eraseCDKSwindow (swindow);

   /* Clear out the character pointers. */
   for (x=0; x <= swindow->itemCount; x++)
   {
      freeChtype (swindow->info[x]);
   }
   for (x=0; x < swindow->titleLines; x++)
   {
      freeChtype (swindow->title[x]);
   }

   /* Delete the windows. */
   deleteCursesWindow (swindow->shadowWin);
   deleteCursesWindow (swindow->fieldWin);
   deleteCursesWindow (swindow->win);

   /* Unregister this object. */
   unregisterCDKObject (vSWINDOW, swindow);

   /* Finish cleaning up. */
   free (swindow);
}

/*
 * This function erases the scrolling window widget.
 */
void _eraseCDKSwindow (CDKOBJS *object)
{
   CDKSWINDOW *swindow = (CDKSWINDOW *)object;

   eraseCursesWindow (swindow->win);
   eraseCursesWindow (swindow->shadowWin);
}

/*
 * This exec's a command and redirects the output to the scrolling window.
 */
int execCDKSwindow (CDKSWINDOW *swindow, char *command, int insertPos)
{
   /* Declare local variables. */
   FILE *ps;
   char temp[513];
   int count = 0;

   /* Try to open the command. */
   if ((ps = popen (command, "r")) == NULL)
   {
      return -1;
   }

   /* Start reading. */
   while ((fgets (temp, 512, ps) != (char *)NULL))
   {
      int len = (int)strlen (temp);
      temp[len-1] = '\0';

      /* Add the line to the scrolling window. */
      addCDKSwindow  (swindow, temp, insertPos);
      count++;
   }

   /* Close the pipe. */
   fclose (ps);
   return count;
}

/*
 * This function allows the user to dump the information from the
 * scrollong window to a file.
 */
void saveCDKSwindowInformation (CDKSWINDOW *swindow)
{
   /* Declare local variables. */
   CDKENTRY *entry	= (CDKENTRY *)NULL;
   char *filename	= (char *)NULL;
   char temp[256], *mesg[10];
   int linesSaved;

   /* Create the entry field to get the filename. */
   entry = newCDKEntry (ScreenOf(swindow), CENTER, CENTER,
				"<C></B/5>Enter the filename of the save file.",
				"Filename: ",
				A_NORMAL, '_', vMIXED,
				20, 1, 256,
				TRUE, FALSE);

   /* Get the filename. */
   filename = activateCDKEntry (entry, (chtype *)NULL);

   /* Did they hit escape? */
   if (entry->exitType == vESCAPE_HIT)
   {
      /* Popup a message. */
      mesg[0] = "<C></B/5>Save Canceled.";
      mesg[1] = "<C>Escape hit. Scrolling window information not saved.";
      mesg[2] = " ";
      mesg[3] = "<C>Press any key to continue.";
      popupLabel (ScreenOf(swindow), mesg, 4);

      /* Clean up and exit. */
      destroyCDKEntry (entry);
      return;
   }

   /* Write the contents of the scrolling window to the file. */
   linesSaved = dumpCDKSwindow (swindow, filename);

   /* Was the save successful? */
   if (linesSaved == -1)
   {
      /* Nope, tell 'em. */
      mesg[0] = "<C></B/16>Error";
      mesg[1] = "<C>Could not save to the file.";
      sprintf (temp, "<C>(%s)", filename);
      mesg[2] = copyChar (temp);
      mesg[3] = " ";
      mesg[4] = "<C>Press any key to continue.";
      popupLabel (ScreenOf(swindow), mesg, 5);
      freeChar (mesg[2]);
   }
   else
   {
      /* Yep, let them know how many lines were saved. */
      mesg[0] = "<C></B/5>Save Successful";
      sprintf (temp, "<C>There were %d lines saved to the file", linesSaved);
      mesg[1] = copyChar (temp);
      sprintf (temp, "<C>(%s)", filename);
      mesg[2] = copyChar (temp);
      mesg[3] = " ";
      mesg[4] = "<C>Press any key to continue.";
      popupLabel (ScreenOf(swindow), mesg, 5);
      freeChar (mesg[1]); freeChar (mesg[2]);
   }

   /* Clean up and exit. */
   destroyCDKEntry (entry);
   eraseCDKScreen (ScreenOf(swindow));
   drawCDKScreen (ScreenOf(swindow));
}

/*
 * This function allows the user to load new informatrion into the scrolling
 * window.
 */
void loadCDKSwindowInformation (CDKSWINDOW *swindow)
{
   /* Declare local variables. */
   CDKFSELECT *fselect	= (CDKFSELECT *)NULL;
   CDKDIALOG *dialog	= (CDKDIALOG *)NULL;
   char *filename	= (char *)NULL;
   char temp[256], *mesg[15], *button[5], *fileInfo[MAX_LINES];
   int lines, answer, x;

   /* Create the file selector to choose the file. */
   fselect = newCDKFselect (ScreenOf(swindow), CENTER, CENTER, 20, 55,
					"<C>Load Which File",
					"Filename",
					A_NORMAL, '.',
					A_REVERSE,
					"</5>", "</48>", "</N>", "</N>",
					TRUE, FALSE);

   /* Get the filename to load. */
   filename = activateCDKFselect (fselect, (chtype *)NULL);

   /* Make sure they selected a file. */
   if (fselect->exitType == vESCAPE_HIT)
   {
      /* Popup a message. */
      mesg[0] = "<C></B/5>Load Canceled.";
      mesg[1] = " ";
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (ScreenOf(swindow), mesg, 3);

      /* Clean up and exit. */
      destroyCDKFselect (fselect);
      return;
   }

   /* Copy the filename and destroy the file selector. */
   filename = copyChar (fselect->pathname);
   destroyCDKFselect (fselect);

   /*
    * Maye we should check before nuking all the information
    * in the scrolling window...
     */
   if (swindow->itemCount > 0)
   {
      /* Create the dialog message. */
      mesg[0] = "<C></B/5>Save Information First";
      mesg[1] = "<C>There is information in the scrolling window.";
      mesg[2] = "<C>Do you want to save it to a file first?";
      button[0] = "(Yes)";
      button[1] = "(No)";

      /* Create the dialog widget. */
      dialog = newCDKDialog (ScreenOf(swindow), CENTER, CENTER,
				mesg, 2, button, 2,
				COLOR_PAIR(2)|A_REVERSE,
				TRUE, TRUE, FALSE);

      /* Activate the widget. */
      answer = activateCDKDialog (dialog, (chtype *)NULL);
      destroyCDKDialog (dialog);

      /* Check the answer. */
      if (answer == -1 || answer == 0)
      {
         /* Save the information. */
         saveCDKSwindowInformation (swindow);
      }
   }

   /* Open the file and read it in. */
   lines = readFile (filename, fileInfo, MAX_LINES);
   if (lines == -1)
   {
      /* The file read didn't work. */
      mesg[0] = "<C></B/16>Error";
      mesg[1] = "<C>Could not read the file";
      sprintf (temp, "<C>(%s)", filename);
      mesg[2] = copyChar (temp);
      mesg[3] = " ";
      mesg[4] = "<C>Press any key to continue.";
      popupLabel (ScreenOf(swindow), mesg, 5);
      freeChar (mesg[2]);
      freeChar (filename);
      return;
   }

   /* Clean out the scrolling window. */
   cleanCDKSwindow (swindow);

   /* Set the new information in the scrolling window. */
   setCDKSwindow (swindow, fileInfo, lines, ObjOf(swindow)->box);

   /* Clean up. */
   for (x=0; x < lines; x++)
   {
      freeChar (fileInfo[x]);
   }
   freeChar (filename);
}

/*
 * This actually dumps the information from the scrolling window to a
 * file.
 */
int dumpCDKSwindow (CDKSWINDOW *swindow, char *filename)
{
   /* Declare local variables. */
   FILE *outputFile	= (FILE *)NULL;
   char *rawLine	= (char *)NULL;
   int x;

   /* Try to open the file. */
   if ((outputFile = fopen (filename, "w")) == NULL)
   {
      return -1;
   }

   /* Start writing out the file. */
   for (x=0; x < swindow->itemCount; x++)
   {
      rawLine = chtype2Char (swindow->info[x]);
      fprintf (outputFile, "%s\n", rawLine);
      freeChar (rawLine);
   }

   /* Close the file and return the number of lines written. */
   fclose (outputFile);
   return swindow->itemCount;
}

/*
 * This function sets the pre-process function.
 */
void setCDKSwindowPreProcess (CDKSWINDOW *swindow, PROCESSFN callback, void *data)
{
   swindow->preProcessFunction = callback;
   swindow->preProcessData = data;
}

/*
 * This function sets the post-process function.
 */
void setCDKSwindowPostProcess (CDKSWINDOW *swindow, PROCESSFN callback, void *data)
{
   swindow->postProcessFunction = callback;
   swindow->postProcessData = data;
}
