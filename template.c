#include "cdk.h"
#include <limits.h>

/*
 * $Author: tom $
 * $Date: 1999/05/16 02:45:58 $
 * $Revision: 1.71 $
 */

/*
 * Declare file local prototypes.
 */
static void CDKTemplateCallBack (CDKTEMPLATE *cdktemplate, chtype input);
static void drawCDKTemplateField (CDKTEMPLATE *cdktemplate);
static void adjustCDKTemplateCursor (CDKTEMPLATE *cdktemplate, int direction);

/*
 * Declare file local variables.
 */
extern char *GPasteBuffer;

static CDKFUNCS my_funcs = {
    _drawCDKTemplate,
    _eraseCDKTemplate,
};

/*
 * This creates a cdktemplate widget.
 */
CDKTEMPLATE *newCDKTemplate (CDKSCREEN *cdkscreen, int xplace, int yplace, char *title, char *label, char *plate, char *Overlay, boolean Box, boolean shadow)
{
   /* Set up some variables */
   CDKTEMPLATE *cdktemplate	= (CDKTEMPLATE *)malloc (sizeof (CDKTEMPLATE));
   chtype *holder		= (chtype *)NULL;
   int parentWidth		= WIN_WIDTH (cdkscreen->window);
   int parentHeight		= WIN_HEIGHT (cdkscreen->window);
   int boxWidth			= 0;
   int boxHeight		= 3;
   int maxWidth			= INT_MIN;
   int xpos			= xplace;
   int ypos			= yplace;
   int horizontalAdjust		= 0;
   int fieldWidth		= 0;
   int plateLen			= 0;
   int junk			= 0;
   char *temp[256];
   int x, len, junk2;

   /* Make sure the plate is not NULL. */
   if (plate == (char *)NULL)
   {
      free (cdktemplate);
      return (CDKTEMPLATE *)NULL;
   }
   fieldWidth = (int)strlen (plate) + 2;

   /* Set some basic values of the cdktemplate field. */
   cdktemplate->label	= (chtype *)NULL;
   cdktemplate->labelLen	= 0;
   cdktemplate->labelWin	= (WINDOW *)NULL;
   cdktemplate->titleLines	= 0;

   /* Translate the char * label to a chtype * */
   if (label != (char *)NULL)
   {
      cdktemplate->label	= char2Chtype (label, &cdktemplate->labelLen, &junk);
   }

   /* Translate the char * Overlay to a chtype * */
   if (Overlay != (char *)NULL)
   {
      cdktemplate->overlay = char2Chtype (Overlay, &cdktemplate->overlayLen, &junk);
      cdktemplate->fieldAttr = cdktemplate->overlay[0] & A_ATTRIBUTES;
   }
   else
   {
      cdktemplate->overlay	= (chtype *)NULL;
      cdktemplate->overlayLen	= 0;
      cdktemplate->fieldAttr	= A_NORMAL;
   }

   /* Set the box width. */
   boxWidth = fieldWidth+ cdktemplate->labelLen + 2;

   /* Translate the char * items to chtype * */
   if (title != (char *)NULL)
   {
      /* We need to split the title on \n. */
      cdktemplate->titleLines = splitString (title, temp, '\n');

      /* We need to determine the widest title line. */
      for (x=0; x < cdktemplate->titleLines; x++)
      {
         holder = char2Chtype (temp[x], &len, &junk2);
         maxWidth = MAXIMUM (maxWidth, len);
         freeChtype (holder);
      }

      /*
       * If one of the title lines is wider than the field and the label,
       * the box width will expand to accomodate.
       */
       if (maxWidth > boxWidth)
       {
          horizontalAdjust = (int)((maxWidth - boxWidth) / 2) + 1;
          boxWidth = maxWidth + 2;
       }

      /* For each line in the title, convert from char * to chtype * */
      for (x=0; x < cdktemplate->titleLines; x++)
      {
         cdktemplate->title[x]	= char2Chtype (temp[x], &cdktemplate->titleLen[x], &cdktemplate->titlePos[x]);
         cdktemplate->titlePos[x]	= justifyString (boxWidth, cdktemplate->titleLen[x], cdktemplate->titlePos[x]);
         freeChar (temp[x]);
      }
   }
   else
   {
      /* No title? Set the required variables. */
      cdktemplate->titleLines = 0;
   }
   boxHeight += cdktemplate->titleLines;

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);
   fieldWidth = (fieldWidth > (boxWidth - cdktemplate->labelLen - 2) ? (boxWidth - cdktemplate->labelLen - 2) : fieldWidth);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the cdktemplate window */
   cdktemplate->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the cdktemplate window NULL?? */
   if (cdktemplate->win == (WINDOW *)NULL)
   {
      /* Clean up any used memory. */
      freeChtype (cdktemplate->label);
      freeChtype (cdktemplate->overlay);
      free (cdktemplate);

      /* Return a NULL pointer. */
      return ((CDKTEMPLATE *)NULL);
   }
   keypad (cdktemplate->win, TRUE);

   /* Make the label window. */
   if (cdktemplate->label != (chtype *)NULL)
   {
      cdktemplate->labelWin = subwin (cdktemplate->win, 1,
					cdktemplate->labelLen+2,
					ypos+cdktemplate->titleLines+1,
					xpos+horizontalAdjust+1);
   }

   /* Make the field window. */
   cdktemplate->fieldWin = subwin (cdktemplate->win, 1, fieldWidth,
				ypos+cdktemplate->titleLines+1,
				xpos+cdktemplate->labelLen+horizontalAdjust+2);
   keypad (cdktemplate->fieldWin, TRUE);

   /* Set up the info field. */
   cdktemplate->plateLen = (int)strlen(plate);
   cdktemplate->info = (char *)malloc (sizeof(char) * (cdktemplate->plateLen+2));
   memset (cdktemplate->info, '\0', cdktemplate->plateLen+1);

   /* Copy the plate to the cdktemplate. */
   plateLen		= strlen (plate);
   cdktemplate->plate	= (char *)malloc (sizeof (char) * plateLen+3);
   memset (cdktemplate->plate, '\0', plateLen+3);
   strcpy (cdktemplate->plate, plate);

   /* Set up the rest of the structure  */
   ScreenOf(cdktemplate)		= cdkscreen;
   ObjOf(cdktemplate)->fn		= &my_funcs;
   cdktemplate->parent			= cdkscreen->window;
   cdktemplate->shadowWin		= (WINDOW *)NULL;
   cdktemplate->fieldWidth		= fieldWidth;
   cdktemplate->boxHeight		= boxHeight;
   cdktemplate->boxWidth		= boxWidth;
   cdktemplate->platePos		= 0;
   cdktemplate->screenPos		= 0;
   cdktemplate->infoPos			= 0;
   cdktemplate->exitType		= vNEVER_ACTIVATED;
   cdktemplate->min			= 0;
   ObjOf(cdktemplate)->box			= Box;
   cdktemplate->shadow			= shadow;
   cdktemplate->callbackfn		= (void *)&CDKTemplateCallBack;
   cdktemplate->preProcessFunction	= (PROCESSFN)NULL;
   cdktemplate->preProcessData		= (void *)NULL;
   cdktemplate->postProcessFunction	= (PROCESSFN)NULL;
   cdktemplate->postProcessData		= (void *)NULL;
   cdktemplate->ULChar			= ACS_ULCORNER;
   cdktemplate->URChar			= ACS_URCORNER;
   cdktemplate->LLChar			= ACS_LLCORNER;
   cdktemplate->LRChar			= ACS_LRCORNER;
   cdktemplate->HChar			= ACS_HLINE;
   cdktemplate->VChar			= ACS_VLINE;
   cdktemplate->BoxAttrib		= A_NORMAL;

   /* Do we need to create a shaodw??? */
   if (shadow)
   {
      cdktemplate->shadowWin = newwin (boxHeight, boxWidth, ypos+1, xpos+1);
   }

   /* Clean the key bindings. */
   cleanCDKObjectBindings (vTEMPLATE, cdktemplate);

   /* Register this baby.                */
   registerCDKObject (cdkscreen, vTEMPLATE, cdktemplate);

   /* Return the pointer to the structure */
   return cdktemplate;
}

/*
 * This actually manages the cdktemplate widget...
 */
char *activateCDKTemplate (CDKTEMPLATE *cdktemplate, chtype *actions)
{
   /* Declare local variables. */
   chtype input	= (chtype)NULL;
   char *ret	= (char *)NULL;

   /* Draw the object. */
   drawCDKTemplate (cdktemplate, ObjOf(cdktemplate)->box);

   /* Check if actions is NULL. */
   if (actions == (chtype *)NULL)
   {
      for (;;)
      {
         /* Get the input. */
         input = wgetch (cdktemplate->win);

         /* Inject the character into the widget. */
         ret = injectCDKTemplate (cdktemplate, input);
         if (cdktemplate->exitType != vEARLY_EXIT)
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
         ret = injectCDKTemplate (cdktemplate, actions[x]);
         if (cdktemplate->exitType != vEARLY_EXIT)
         {
            return ret;
         }
      }
   }

   /* Set the exit type and return. */
   cdktemplate->exitType = vEARLY_EXIT;
   return ret;
}

/*
 * This injects a character into the widget.
 */
char *injectCDKTemplate (CDKTEMPLATE *cdktemplate, chtype input)
{
   /* Declare some local variables. */
   int ppReturn = 1;
   int length, x;

   /* Set the exit type and return. */
   cdktemplate->exitType = vEARLY_EXIT;

   /* Move the cursor. */
   drawCDKTemplateField (cdktemplate);

   /* Check if there is a pre-process function to be called. */
   if (cdktemplate->preProcessFunction != (PROCESSFN)NULL)
   {
      ppReturn = ((PROCESSFN)(cdktemplate->preProcessFunction)) (vTEMPLATE, cdktemplate, cdktemplate->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check a predefined binding...           */
      if (checkCDKObjectBind (vTEMPLATE, cdktemplate, input) != 0)
      {
         cdktemplate->exitType = vESCAPE_HIT;
         return (char *)NULL;
      }
      else
      {
         switch (input)
         {
            case KEY_LEFT : case KEY_RIGHT : case KEY_UP : case KEY_DOWN :
                 Beep();
                 break;

            case CDK_ERASE :
                 if ((int)strlen(cdktemplate->info) != 0)
                 {
                    cleanChar (cdktemplate->info, cdktemplate->plateLen+1, '\0');
                    cdktemplate->screenPos = 0;
                    cdktemplate->infoPos = 0;
                    cdktemplate->platePos = 0;
                    drawCDKTemplateField (cdktemplate);
                 }
                 break;

            case CDK_CUT:
                 if ((int)strlen(cdktemplate->info) != 0)
                 {
	            freeChar (GPasteBuffer);
                    GPasteBuffer = copyChar (cdktemplate->info);
                    cleanChar (cdktemplate->info, cdktemplate->plateLen+1, '\0');
                    cdktemplate->screenPos = 0;
                    cdktemplate->infoPos = 0;
                    cdktemplate->platePos = 0;
                    drawCDKTemplateField (cdktemplate);
                 }
                 else
                 {
                    Beep();
                 }
                 break;

            case CDK_COPY:
                 if ((int)strlen(cdktemplate->info) != 0)
                 {
	            freeChar (GPasteBuffer);
                    GPasteBuffer = copyChar (cdktemplate->info);
                 }
                 else
                 {
                    Beep();
                 }
                 break;

            case CDK_PASTE:
                 if (GPasteBuffer != (char *)NULL)
                 {
                    /* Clean the informatiob from the field. */
                    cleanChar (cdktemplate->info, cdktemplate->plateLen+1, '\0');
                    cdktemplate->screenPos = 0;
                    cdktemplate->infoPos = 0;
                    cdktemplate->platePos = 0;

                    /* Start inserting each character one at a time. */
                    length = (int)strlen (GPasteBuffer);
                    for (x=0; x < length; x++)
                    {
                       ((TEMPLATECB)cdktemplate->callbackfn)(cdktemplate, GPasteBuffer[x]);
                    }
                    drawCDKTemplateField (cdktemplate);
                 }
                 else
                 {
                    Beep();
                 }
                 break;

            case KEY_RETURN : case KEY_TAB : case KEY_ENTER :
                 if ((int)strlen(cdktemplate->info) < (int)cdktemplate->min)
                 {
                    Beep();
                 }
                 else
                 {
                    cdktemplate->exitType = vNORMAL;
                    return cdktemplate->info;
                 }
                 break;

            case KEY_ESC :
                 cdktemplate->exitType = vESCAPE_HIT;
                 return (char *)NULL;

            case CDK_REFRESH :
                 eraseCDKScreen (ScreenOf(cdktemplate));
                 refreshCDKScreen (ScreenOf(cdktemplate));
                 break;

            default :
                 ((TEMPLATECB)cdktemplate->callbackfn)(cdktemplate, input);
                 break;
         }
      }

      /* Should we call a post-process? */
      if (cdktemplate->postProcessFunction != (PROCESSFN)NULL)
      {
         ((PROCESSFN)(cdktemplate->postProcessFunction)) (vTEMPLATE, cdktemplate, cdktemplate->postProcessData, input);
      }
   }

   /* Set the exit type and return. */
   cdktemplate->exitType = vEARLY_EXIT;
   return (char *)NULL;
}

/*
 * This is the standard callback proc for the cdktemplate.
 */
static void CDKTemplateCallBack (CDKTEMPLATE *cdktemplate, chtype input)
{
   chtype fieldColor;

   /* Check the character input. */
   if (input == DELETE || input == KEY_BACKSPACE ||
	input == '' || input == KEY_DC)
   {
      /* Make sure we don't go out of bounds. */
      if (cdktemplate->platePos == 0 || cdktemplate->info[0] == '\0')
      {
         Beep();
         return;
      }

      /* Remove it and redisplay.... */
      cdktemplate->info[--cdktemplate->infoPos] = '\0';
      cdktemplate->platePos--;
      cdktemplate->screenPos--;

      /* Move the cursor */
      fieldColor = cdktemplate->overlay[cdktemplate->screenPos] & A_ATTRIBUTES;
      adjustCDKTemplateCursor (cdktemplate, -1);
      mvwaddch (cdktemplate->fieldWin, 0, cdktemplate->screenPos,
		cdktemplate->overlay[cdktemplate->platePos] | fieldColor);
      adjustCDKTemplateCursor (cdktemplate, -1);
   }
   else
   {
      /* To be safe, we should cast the chtype to a char */
      char newchar = (char)input;

      /* Make sure we don't go out of bounds. */
      if (cdktemplate->platePos >= cdktemplate->fieldWidth)
      {
         Beep();
         return;
      }

      /* We should check to see if what they typed in matches */
      /* what the plate states. */
      if (isdigit((int)newchar) && ( \
		cdktemplate->plate[cdktemplate->platePos] == 'A' || \
		cdktemplate->plate[cdktemplate->platePos] == 'C' || \
		cdktemplate->plate[cdktemplate->platePos] == 'c'))
      {
         Beep();
         return;
      }
      if (!isdigit((int)newchar) && cdktemplate->plate[cdktemplate->platePos] == '#')
      {
         Beep();
         return;
      }

      /* Do we need to convert the case??? */
      if (cdktemplate->plate[cdktemplate->platePos] == 'C' || \
		cdktemplate->plate[cdktemplate->platePos] == 'X')
      {
         newchar = toupper ((char)input);
      }
      else if (cdktemplate->plate[cdktemplate->platePos] == 'c' || \
		cdktemplate->plate[cdktemplate->platePos] == 'x')
      {
         newchar = tolower ((char)input);
      }

      /* Add it and redisplay.... */
      fieldColor = cdktemplate->overlay[cdktemplate->screenPos] & A_ATTRIBUTES;
      cdktemplate->info[cdktemplate->infoPos++] = newchar;
      cdktemplate->info[cdktemplate->infoPos] = '\0';
      mvwaddch (cdktemplate->fieldWin, 0, cdktemplate->screenPos,
			newchar | fieldColor);
      cdktemplate->platePos++;
      cdktemplate->screenPos++;

      /* Move the cursor */
      adjustCDKTemplateCursor (cdktemplate, +1);
   }
}

/*
 * This takes the overlay and the info, and mixes the two, for a
 * 'mixed' character string...
 */
char *mixCDKTemplate (CDKTEMPLATE *cdktemplate)
{
   /* Reconstruct the info. */
   char *mixedString	= (char *)NULL;
   int platePos		= 0;
   int infoPos		= 0;

   /* Check if the string is NULL. */
   if (cdktemplate->info == (char *)NULL ||
	(int)strlen(cdktemplate->info) == 0)
   {
      return (char *)NULL;
   }

   /* Make some room for the mixed string. */
   mixedString = (char *) malloc ((sizeof (char) * cdktemplate->plateLen) + 3);
   cleanChar (mixedString, cdktemplate->plateLen+3, '\0');

   /* Start copying from the plate to the destination string. */
   while (platePos < cdktemplate->plateLen)
   {
      if (cdktemplate->plate[platePos] == '#' || \
		cdktemplate->plate[platePos] == 'A' || \
		cdktemplate->plate[platePos] == 'C' || \
		cdktemplate->plate[platePos] == 'c' || \
		cdktemplate->plate[platePos] == 'M' || \
		cdktemplate->plate[platePos] == 'X' || \
		cdktemplate->plate[platePos] == 'x')
      {
         mixedString[platePos]	= cdktemplate->info[infoPos++];
      }
      else
      {
         mixedString[platePos]	= cdktemplate->plate[platePos];
      }
      platePos++;
   }

   /* Return the new string. */
   return mixedString;
}

/*
 * This takes a mixed string and returns a non-mixed string.
 */
char *unmixCDKTemplate (CDKTEMPLATE *cdktemplate, char *info)
{
   /* Reconstruct the info. */
   char *unmixedString	= (char *)NULL;
   int infolen		= (int)strlen (info);
   int x		= 0;
   int pos		= 0;

   /* Create a char * pointer. */
   unmixedString	= (char *)malloc (sizeof(char) * (infolen+2));
   cleanChar (unmixedString, infolen+2, '\0');

   /* Start copying. */
   while (pos < infolen)
   {
      if (cdktemplate->plate[pos] == '#' || \
		cdktemplate->plate[pos] == 'A' || \
		cdktemplate->plate[pos] == 'C' || \
		cdktemplate->plate[pos] == 'c' || \
		cdktemplate->plate[pos] == 'M' || \
		cdktemplate->plate[pos] == 'X' || \
		cdktemplate->plate[pos] == 'x')
      {
         unmixedString[x++] = info[pos++];
      }
      else
      {
         pos++;
      }
   }

   /* Return the new string. */
   return unmixedString;
}

/*
 * This moves the cdktemplate field to the given location.
 */
void moveCDKTemplate (CDKTEMPLATE *cdktemplate, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   /* Declare local variables. */
   int currentX = cdktemplate->win->_begx;
   int currentY = cdktemplate->win->_begy;
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
      xpos = cdktemplate->win->_begx + xplace;
      ypos = cdktemplate->win->_begy + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(cdktemplate), &xpos, &ypos, cdktemplate->boxWidth, cdktemplate->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   cdktemplate->win->_begx = xpos;
   cdktemplate->win->_begy = ypos;
   if (cdktemplate->labelWin != (WINDOW *)NULL)
   {
      cdktemplate->labelWin->_begx -= xdiff;
      cdktemplate->labelWin->_begy -= ydiff;
   }
   cdktemplate->fieldWin->_begx -= xdiff;
   cdktemplate->fieldWin->_begy -= ydiff;

   /* If there is a shadow box we have to move it too. */
   if (cdktemplate->shadowWin != (WINDOW *)NULL)
   {
      cdktemplate->shadowWin->_begx -= xdiff;
      cdktemplate->shadowWin->_begy -= ydiff;
   }

   /* Touch the windows so they 'move'. */
   touchwin (WindowOf(cdktemplate));
   wrefresh (WindowOf(cdktemplate));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKTemplate (cdktemplate, ObjOf(cdktemplate)->box);
   }
}

/*
 * This allows the user to use the cursor keys to adjust the
 * position of the widget.
 */
void positionCDKTemplate (CDKTEMPLATE *cdktemplate)
{
   /* Declare some variables. */
   int origX	= cdktemplate->win->_begx;
   int origY	= cdktemplate->win->_begy;
   chtype key	= (chtype)NULL;

   /* Let them move the widget around until they hit return. */
   while ((key != KEY_RETURN) && (key != KEY_ENTER))
   {
      key = wgetch (cdktemplate->win);
      if (key == KEY_UP || key == '8')
      {
         if (cdktemplate->win->_begy > 0)
         {
            moveCDKTemplate (cdktemplate, 0, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_DOWN || key == '2')
      {
         if (cdktemplate->win->_begy+cdktemplate->win->_maxy < WindowOf(cdktemplate)->_maxy-1)
         {
            moveCDKTemplate (cdktemplate, 0, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_LEFT || key == '4')
      {
         if (cdktemplate->win->_begx > 0)
         {
            moveCDKTemplate (cdktemplate, -1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_RIGHT || key == '6')
      {
         if (cdktemplate->win->_begx+cdktemplate->win->_maxx < WindowOf(cdktemplate)->_maxx-1)
         {
            moveCDKTemplate (cdktemplate, 1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '7')
      {
         if (cdktemplate->win->_begy > 0 && cdktemplate->win->_begx > 0)
         {
            moveCDKTemplate (cdktemplate, -1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '9')
      {
         if (cdktemplate->win->_begx+cdktemplate->win->_maxx < WindowOf(cdktemplate)->_maxx-1 &&
		cdktemplate->win->_begy > 0)
         {
            moveCDKTemplate (cdktemplate, 1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '1')
      {
         if (cdktemplate->win->_begx > 0 && cdktemplate->win->_begx+cdktemplate->win->_maxx < WindowOf(cdktemplate)->_maxx-1)
         {
            moveCDKTemplate (cdktemplate, -1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '3')
      {
         if (cdktemplate->win->_begx+cdktemplate->win->_maxx < WindowOf(cdktemplate)->_maxx-1
	  && cdktemplate->win->_begy+cdktemplate->win->_maxy < WindowOf(cdktemplate)->_maxy-1)
         {
            moveCDKTemplate (cdktemplate, 1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '5')
      {
         moveCDKTemplate (cdktemplate, CENTER, CENTER, FALSE, TRUE);
      }
      else if (key == 't')
      {
         moveCDKTemplate (cdktemplate, cdktemplate->win->_begx, TOP, FALSE, TRUE);
      }
      else if (key == 'b')
      {
         moveCDKTemplate (cdktemplate, cdktemplate->win->_begx, BOTTOM, FALSE, TRUE);
      }
      else if (key == 'l')
      {
         moveCDKTemplate (cdktemplate, LEFT, cdktemplate->win->_begy, FALSE, TRUE);
      }
      else if (key == 'r')
      {
         moveCDKTemplate (cdktemplate, RIGHT, cdktemplate->win->_begy, FALSE, TRUE);
      }
      else if (key == 'c')
      {
         moveCDKTemplate (cdktemplate, CENTER, cdktemplate->win->_begy, FALSE, TRUE);
      }
      else if (key == 'C')
      {
         moveCDKTemplate (cdktemplate, cdktemplate->win->_begx, CENTER, FALSE, TRUE);
      }
      else if (key == CDK_REFRESH)
      {
         eraseCDKScreen (ScreenOf(cdktemplate));
         refreshCDKScreen (ScreenOf(cdktemplate));
      }
      else if (key == KEY_ESC)
      {
         moveCDKTemplate (cdktemplate, origX, origY, FALSE, TRUE);
      }
      else if ((key != KEY_RETURN) && (key != KEY_ENTER))
      {
         Beep();
      }
   }
}

/*
 * This function draws the tmeplate widget.
 */
void _drawCDKTemplate (CDKOBJS *object, boolean Box)
{
   CDKTEMPLATE *cdktemplate = (CDKTEMPLATE *)object;
   int x;

   /* Do we need to draw the shadow. */
   if (cdktemplate->shadowWin != (WINDOW *)NULL)
   {
      drawShadow (cdktemplate->shadowWin);
   }

   /* Box it if needed */
   if (Box)
   {
      attrbox (cdktemplate->win,
		cdktemplate->ULChar, cdktemplate->URChar,
		cdktemplate->LLChar, cdktemplate->LRChar,
		cdktemplate->HChar,  cdktemplate->VChar,
		cdktemplate->BoxAttrib);
   }

   /* Draw in the title if there is one. */
   if (cdktemplate->titleLines != 0)
   {
      for (x=0; x < cdktemplate->titleLines; x++)
      {
         writeChtype (cdktemplate->win,
			cdktemplate->titlePos[x],
			x+1,
			cdktemplate->title[x],
			HORIZONTAL, 0,
			cdktemplate->titleLen[x]);
      }
   }
   touchwin (cdktemplate->win);
   wrefresh (cdktemplate->win);

   /* Draw the cdktemplate field. */
   drawCDKTemplateField (cdktemplate);
}

/*
 * This draws the cdktemplate field.
 */
static void drawCDKTemplateField (CDKTEMPLATE *cdktemplate)
{
   /* Declare local variables. */
   chtype fieldColor	= (chtype)NULL;
   int infolen		= (int)strlen (cdktemplate->info);
   int x		= 0;

   /* Draw in the label and the cdktemplate object. */
   if (cdktemplate->labelWin != (WINDOW *)NULL)
   {
      writeChtype (cdktemplate->labelWin, 0, 0,
			cdktemplate->label,
			HORIZONTAL, 0,
			cdktemplate->labelLen);
      touchwin (cdktemplate->labelWin);
      wrefresh (cdktemplate->labelWin);
   }

   /* Draw in the cdktemplate... */
   if (cdktemplate->overlay != (chtype *)NULL)
   {
      writeChtype (cdktemplate->fieldWin, 0, 0,
			cdktemplate->overlay,
			HORIZONTAL, 0,
			cdktemplate->overlayLen);
   }

   /* Adjust the cursor. */
   if (infolen != 0)
   {
      int pos	= 0;
      for (x=0; x < cdktemplate->fieldWidth; x++)
      {
         if ((cdktemplate->plate[x] == '#' ||
		cdktemplate->plate[x] == 'A' ||
		cdktemplate->plate[x] == 'C' ||
		cdktemplate->plate[x] == 'c' ||
		cdktemplate->plate[x] == 'M' ||
		cdktemplate->plate[x] == 'X' ||
		cdktemplate->plate[x] == 'x') &&
		pos < infolen)
         {
            fieldColor = cdktemplate->overlay[x] & A_ATTRIBUTES;
            mvwaddch (cdktemplate->fieldWin, 0, x, cdktemplate->info[pos++]|fieldColor);
         }
      }
      wmove (cdktemplate->fieldWin, 0, cdktemplate->screenPos);
   }
   else
   {
      adjustCDKTemplateCursor (cdktemplate, +1);
   }
   touchwin (cdktemplate->fieldWin);
   wrefresh (cdktemplate->fieldWin);
}

/*
 * This function adjusts the cursor for the cdktemplate.
 */
static void adjustCDKTemplateCursor (CDKTEMPLATE *cdktemplate, int direction)
{
   while (cdktemplate->plate[cdktemplate->platePos] != '#' &&
		cdktemplate->plate[cdktemplate->platePos] != 'A' &&
		cdktemplate->plate[cdktemplate->platePos] != 'C' &&
		cdktemplate->plate[cdktemplate->platePos] != 'c' &&
		cdktemplate->plate[cdktemplate->platePos] != 'M' &&
		cdktemplate->plate[cdktemplate->platePos] != 'X' &&
		cdktemplate->plate[cdktemplate->platePos] != 'x'  &&
		cdktemplate->platePos < cdktemplate->fieldWidth)
   {
      cdktemplate->platePos += direction;
      cdktemplate->screenPos += direction;
   }
   wmove (cdktemplate->fieldWin, 0, cdktemplate->screenPos);
   wrefresh (cdktemplate->fieldWin);
}

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKTemplateULChar (CDKTEMPLATE *cdktemplate, chtype character)
{
   cdktemplate->ULChar = character;
}
void setCDKTemplateURChar (CDKTEMPLATE *cdktemplate, chtype character)
{
   cdktemplate->URChar = character;
}
void setCDKTemplateLLChar (CDKTEMPLATE *cdktemplate, chtype character)
{
   cdktemplate->LLChar = character;
}
void setCDKTemplateLRChar (CDKTEMPLATE *cdktemplate, chtype character)
{
   cdktemplate->LRChar = character;
}
void setCDKTemplateVerticalChar (CDKTEMPLATE *cdktemplate, chtype character)
{
   cdktemplate->VChar = character;
}
void setCDKTemplateHorizontalChar (CDKTEMPLATE *cdktemplate, chtype character)
{
   cdktemplate->HChar = character;
}
void setCDKTemplateBoxAttribute (CDKTEMPLATE *cdktemplate, chtype character)
{
   cdktemplate->BoxAttrib = character;
}

/*
 * This sets the background color of the widget.
 */
void setCDKTemplateBackgroundColor (CDKTEMPLATE *cdktemplate, char *color)
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
   wbkgd (cdktemplate->win, holder[0]);
   wbkgd (cdktemplate->fieldWin, holder[0]);
   if (cdktemplate->labelWin != (WINDOW *)NULL)
   {
      wbkgd (cdktemplate->labelWin, holder[0]);
   }

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This function destroys this widget.
 */
void destroyCDKTemplate (CDKTEMPLATE *cdktemplate)
{
   int x;

   /* Erase the object. */
   eraseCDKTemplate (cdktemplate);

   /* Clear out the character pointers. */
   freeChtype (cdktemplate->label);
   freeChtype (cdktemplate->overlay);
   freeChar (cdktemplate->plate);
   freeChar (cdktemplate->info);
   for (x=0; x < cdktemplate->titleLines; x++)
   {
      freeChtype (cdktemplate->title[x]);
   }

   /* Delete the windows. */
   deleteCursesWindow (cdktemplate->fieldWin);
   deleteCursesWindow (cdktemplate->labelWin);
   deleteCursesWindow (cdktemplate->shadowWin);
   deleteCursesWindow (cdktemplate->win);

   /* Unregister this object. */
   unregisterCDKObject (vTEMPLATE, cdktemplate);

   /* Finish cleaning up. */
   free (cdktemplate);
}

/*
 * This function erases the widget.
 */
void _eraseCDKTemplate (CDKOBJS *object)
{
   CDKTEMPLATE *cdktemplate = (CDKTEMPLATE *)object;

   eraseCursesWindow (cdktemplate->fieldWin);
   eraseCursesWindow (cdktemplate->labelWin);
   eraseCursesWindow (cdktemplate->win);
   eraseCursesWindow (cdktemplate->shadowWin);
}

/*
 * This function sets the value given to the cdktemplate.
 */
void setCDKTemplate (CDKTEMPLATE *cdktemplate, char *newValue, boolean Box)
{
   setCDKTemplateValue (cdktemplate, newValue);
   setCDKTemplateBox (cdktemplate, Box);
}

/*
 * This function sets the value given to the cdktemplate.
 */
void setCDKTemplateValue (CDKTEMPLATE *cdktemplate, char *newValue)
{
   /* Declare local variables. */
   int len		= 0;
   int copychars	= 0;
   int x;

   /* Just to be sure, if lets make sure the new value isn't NULL. */
   if (newValue == (char *)NULL)
   {
      /* Then we want to just erase the old value. */
      cleanChar (cdktemplate->info, cdktemplate->fieldWidth, '\0');

      /* Set the cursor place values. */
      cdktemplate->screenPos = 0;
      cdktemplate->platePos = 0;
      cdktemplate->infoPos = 0;
      return;
   }

   /* Determine how many characters we need to copy. */
   len		= (int)strlen (newValue);
   copychars	= MINIMUM (len, cdktemplate->fieldWidth);

   /* OK, erase the old value, and copy in the new value. */
   cleanChar (cdktemplate->info, cdktemplate->fieldWidth, '\0');
   strncpy (cdktemplate->info, newValue, copychars);

   /* Use the function which handles the input of the characters. */
   for (x=0; x < len; x++)
   {
      ((TEMPLATECB)cdktemplate->callbackfn)(cdktemplate, (chtype)newValue[x]);
   }
}
char *getCDKTemplateValue (CDKTEMPLATE *cdktemplate)
{
   return cdktemplate->info;
}

/*
 * This sets the minimum number of characters to enter into the widget.
 */
void setCDKTemplateMin (CDKTEMPLATE *cdktemplate, int min)
{
   if (min >= 0)
   {
      cdktemplate->min = min;
   }
}
int getCDKTemplateMin (CDKTEMPLATE *cdktemplate)
{
   return cdktemplate->min;
}

/*
 * This sets the box attribute of the cdktemplate widget.
 */
void setCDKTemplateBox (CDKTEMPLATE *cdktemplate, boolean Box)
{
   ObjOf(cdktemplate)->box = Box;
}
boolean getCDKTemplateBox (CDKTEMPLATE *cdktemplate)
{
   return ObjOf(cdktemplate)->box;
}

/*
 * This erases the information in the cdktemplate widget.
 */
void cleanCDKTemplate (CDKTEMPLATE *cdktemplate)
{
   cleanChar (cdktemplate->info, cdktemplate->fieldWidth, '\0');
}

/*
 * This function sets the callback function for the widget.
 */
void setCDKTemplateCB (CDKTEMPLATE *cdktemplate, TEMPLATECB callback)
{
   cdktemplate->callbackfn = (void *)callback;
}

/*
 * This function sets the pre-process function.
 */
void setCDKTemplatePreProcess (CDKTEMPLATE *cdktemplate, PROCESSFN callback, void *data)
{
   cdktemplate->preProcessFunction = callback;
   cdktemplate->preProcessData = data;
}

/*
 * This function sets the post-process function.
 */
void setCDKTemplatePostProcess (CDKTEMPLATE *cdktemplate, PROCESSFN callback, void *data)
{
   cdktemplate->postProcessFunction = callback;
   cdktemplate->postProcessData = data;
}
