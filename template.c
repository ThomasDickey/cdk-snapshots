#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2003/11/16 22:17:45 $
 * $Revision: 1.104 $
 */

/*
 * Declare file local prototypes.
 */
static void CDKTemplateCallBack (CDKTEMPLATE *cdktemplate, chtype input);
static void drawCDKTemplateField (CDKTEMPLATE *cdktemplate);
static void adjustCDKTemplateCursor (CDKTEMPLATE *cdktemplate, int direction);

#define isPlateChar(c) ((c) != 0 && strchr("#ACcMXz", c) != 0)

DeclareCDKObjects(TEMPLATE, Template, setCdk, String);

/*
 * This creates a cdktemplate widget.
 */
CDKTEMPLATE *newCDKTemplate (CDKSCREEN *cdkscreen, int xplace, int yplace, char *title, char *label, char *plate, char *Overlay, boolean Box, boolean shadow)
{
   CDKTEMPLATE *cdktemplate	= 0;
   chtype *holder		= 0;
   int parentWidth		= getmaxx(cdkscreen->window) - 1;
   int parentHeight		= getmaxy(cdkscreen->window) - 1;
   int boxWidth			= 0;
   int boxHeight		= Box ? 3 : 1;
   int maxWidth			= INT_MIN;
   int xpos			= xplace;
   int ypos			= yplace;
   int horizontalAdjust		= 0;
   int fieldWidth		= 0;
   int plateLen			= 0;
   int junk			= 0;
   char **temp			= 0;
   int x, len, junk2;

   if (plate == 0
    || (cdktemplate = newCDKObject(CDKTEMPLATE, &my_funcs)) == 0)
      return (0);

   setCDKTemplateBox (cdktemplate, Box);

   fieldWidth = (int)strlen (plate) + 2 * BorderOf(cdktemplate);

   /* Set some basic values of the cdktemplate field. */
   cdktemplate->label		= 0;
   cdktemplate->labelLen	= 0;
   cdktemplate->labelWin	= 0;
   cdktemplate->titleLines	= 0;

   /* Translate the char * label to a chtype * */
   if (label != 0)
   {
      cdktemplate->label	= char2Chtype (label, &cdktemplate->labelLen, &junk);
   }

   /* Translate the char * Overlay to a chtype * */
   if (Overlay != 0)
   {
      cdktemplate->overlay = char2Chtype (Overlay, &cdktemplate->overlayLen, &junk);
      cdktemplate->fieldAttr = cdktemplate->overlay[0] & A_ATTRIBUTES;
   }
   else
   {
      cdktemplate->overlay	= 0;
      cdktemplate->overlayLen	= 0;
      cdktemplate->fieldAttr	= A_NORMAL;
   }

   /* Set the box width. */
   boxWidth = fieldWidth+ cdktemplate->labelLen + 2 * BorderOf(cdktemplate);

   /* Translate the char * items to chtype * */
   if (title != 0)
   {
      int titleWidth;

      temp = CDKsplitString (title, '\n');
      cdktemplate->titleLines = CDKcountStrings (temp);

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
          boxWidth = maxWidth + 2 * BorderOf(cdktemplate);
       }

      /* For each line in the title, convert from char * to chtype * */
      titleWidth = boxWidth - (2 * BorderOf(cdktemplate));
      for (x=0; x < cdktemplate->titleLines; x++)
      {
	 cdktemplate->title[x]	  = char2Chtype (temp[x], &cdktemplate->titleLen[x], &cdktemplate->titlePos[x]);
         cdktemplate->titlePos[x] = justifyString (titleWidth, cdktemplate->titleLen[x], cdktemplate->titlePos[x]);
      }
      CDKfreeStrings(temp);
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
   fieldWidth = MINIMUM(fieldWidth, boxWidth - cdktemplate->labelLen - 2 * BorderOf(cdktemplate));

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight, BorderOf(cdktemplate));

   /* Make the cdktemplate window */
   cdktemplate->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the cdktemplate window null?? */
   if (cdktemplate->win == 0)
   {
      destroyCDKObject(cdktemplate);
      return (0);
   }
   keypad (cdktemplate->win, TRUE);

   /* Make the label window. */
   if (cdktemplate->label != 0)
   {
      cdktemplate->labelWin = subwin (cdktemplate->win, 1,
					cdktemplate->labelLen,
					ypos + cdktemplate->titleLines + BorderOf(cdktemplate),
					xpos + horizontalAdjust + BorderOf(cdktemplate));
   }

   /* Make the field window. */
   cdktemplate->fieldWin = subwin (cdktemplate->win, 1, fieldWidth,
				ypos + cdktemplate->titleLines + BorderOf(cdktemplate),
				xpos + cdktemplate->labelLen + horizontalAdjust + BorderOf(cdktemplate));
   keypad (cdktemplate->fieldWin, TRUE);

   /* Set up the info field. */
   cdktemplate->plateLen = (int)strlen(plate);
   cdktemplate->info = (char *)malloc (sizeof(char) * (cdktemplate->plateLen + 2));
   memset (cdktemplate->info, '\0', cdktemplate->plateLen + 1);

   /* Copy the plate to the cdktemplate. */
   plateLen		= strlen (plate);
   cdktemplate->plate	= (char *)malloc (sizeof (char) * plateLen + 3);
   strncpy (cdktemplate->plate, plate, plateLen + 3);

   /* Set up the rest of the structure	*/
   ScreenOf(cdktemplate)		= cdkscreen;
   cdktemplate->parent			= cdkscreen->window;
   cdktemplate->shadowWin		= 0;
   cdktemplate->fieldWidth		= fieldWidth;
   cdktemplate->boxHeight		= boxHeight;
   cdktemplate->boxWidth		= boxWidth;
   cdktemplate->platePos		= 0;
   cdktemplate->screenPos		= 0;
   cdktemplate->infoPos			= 0;
   cdktemplate->exitType		= vNEVER_ACTIVATED;
   cdktemplate->min			= 0;
   ObjOf(cdktemplate)->inputWindow	= cdktemplate->win;
   cdktemplate->shadow			= shadow;
   cdktemplate->callbackfn		= CDKTemplateCallBack;
   cdktemplate->preProcessFunction	= 0;
   cdktemplate->preProcessData		= 0;
   cdktemplate->postProcessFunction	= 0;
   cdktemplate->postProcessData		= 0;

   /* Do we need to create a shaodw??? */
   if (shadow)
   {
      cdktemplate->shadowWin = newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
   }

   /* Clean the key bindings. */
   cleanCDKObjectBindings (vTEMPLATE, cdktemplate);

   /* Register this baby.		 */
   registerCDKObject (cdkscreen, vTEMPLATE, cdktemplate);

   /* Return the pointer to the structure */
   return cdktemplate;
}

/*
 * This actually manages the cdktemplate widget...
 */
char *activateCDKTemplate (CDKTEMPLATE *cdktemplate, chtype *actions)
{
   chtype input = 0;
   char *ret	= 0;

   /* Draw the object. */
   drawCDKTemplate (cdktemplate, ObjOf(cdktemplate)->box);

   /* Check if actions is null. */
   if (actions == 0)
   {
      for (;;)
      {
	 /* Get the input. */
	 input = getcCDKObject (ObjOf(cdktemplate));

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
static int _injectCDKTemplate (CDKOBJS *object, chtype input)
{
   CDKTEMPLATE *cdktemplate = (CDKTEMPLATE *)object;
   int ppReturn = 1;
   int length, x;
   char *ret = unknownString;
   bool complete = FALSE;

   /* Set the exit type and return. */
   cdktemplate->exitType = vEARLY_EXIT;

   /* Move the cursor. */
   drawCDKTemplateField (cdktemplate);

   /* Check if there is a pre-process function to be called. */
   if (cdktemplate->preProcessFunction != 0)
   {
      ppReturn = cdktemplate->preProcessFunction (vTEMPLATE, cdktemplate, cdktemplate->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check a predefined binding...		 */
      if (checkCDKObjectBind (vTEMPLATE, cdktemplate, input) != 0)
      {
	 cdktemplate->exitType = vESCAPE_HIT;
	 complete = TRUE;
      }
      else
      {
	 switch (input)
	 {
	    case KEY_LEFT : case KEY_RIGHT : case KEY_UP : case KEY_DOWN :
		 Beep();
		 break;

	    case CDK_ERASE :
		 if (strlen(cdktemplate->info) != 0)
		 {
		    cleanChar (cdktemplate->info, cdktemplate->plateLen + 1, '\0');
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
		    cleanChar (cdktemplate->info, cdktemplate->plateLen + 1, '\0');
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
		 if (GPasteBuffer != 0)
		 {
		    /* Clean the informatiob from the field. */
		    cleanChar (cdktemplate->info, cdktemplate->plateLen + 1, '\0');
		    cdktemplate->screenPos = 0;
		    cdktemplate->infoPos = 0;
		    cdktemplate->platePos = 0;

		    /* Start inserting each character one at a time. */
		    length = (int)strlen (GPasteBuffer);
		    for (x=0; x < length; x++)
		    {
		       (cdktemplate->callbackfn)(cdktemplate, GPasteBuffer[x]);
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
		    ret = cdktemplate->info;
		    complete = TRUE;
		 }
		 break;

	    case KEY_ESC :
		 cdktemplate->exitType = vESCAPE_HIT;
		 complete = TRUE;
		 break;

	    case CDK_REFRESH :
		 eraseCDKScreen (ScreenOf(cdktemplate));
		 refreshCDKScreen (ScreenOf(cdktemplate));
		 break;

	    default :
		 (cdktemplate->callbackfn)(cdktemplate, input);
		 break;
	 }
      }

      /* Should we call a post-process? */
      if (!complete && (cdktemplate->postProcessFunction != 0))
      {
	 cdktemplate->postProcessFunction (vTEMPLATE, cdktemplate, cdktemplate->postProcessData, input);
      }
   }

   if (!complete) {
      cdktemplate->exitType = vEARLY_EXIT;
   }

   ResultOf(cdktemplate).valueString = ret;
   return (ret != unknownString);
}

/*
 * This is the standard callback proc for the cdktemplate.
 */
static void CDKTemplateCallBack (CDKTEMPLATE *cdktemplate, chtype input)
{
   chtype fieldColor;

   /* Check the character input. */
   if (input == DELETE || input == KEY_BACKSPACE ||
	input == CONTROL('H') || input == KEY_DC)
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
   else if (input <= 0 || input >= KEY_MIN)
   {
      Beep();
   }
   else
   {
      int newchar = A_CHARTEXT & input;

      /* Make sure we don't go out of bounds. */
      if (cdktemplate->platePos >= cdktemplate->fieldWidth)
      {
	 Beep();
	 return;
      }

      /* We should check to see if what they typed in matches */
      /* what the plate states. */
      if (isdigit(newchar) && (
		cdktemplate->plate[cdktemplate->platePos] == 'A' ||
		cdktemplate->plate[cdktemplate->platePos] == 'C' ||
		cdktemplate->plate[cdktemplate->platePos] == 'c'))
      {
	 Beep();
	 return;
      }
      if (!isdigit(newchar) && cdktemplate->plate[cdktemplate->platePos] == '#')
      {
	 Beep();
	 return;
      }

      /* Do we need to convert the case??? */
      if (cdktemplate->plate[cdktemplate->platePos] == 'C' ||
		cdktemplate->plate[cdktemplate->platePos] == 'X')
      {
	 newchar = toupper (newchar);
      }
      else if (cdktemplate->plate[cdktemplate->platePos] == 'c' ||
		cdktemplate->plate[cdktemplate->platePos] == 'x')
      {
	 newchar = tolower (newchar);
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
   char *mixedString	= 0;
   int platePos		= 0;
   int infoPos		= 0;

   /* Check if the string is null. */
   if (cdktemplate->info == 0 ||
	(int)strlen(cdktemplate->info) == 0)
   {
      return 0;
   }

   /* Make some room for the mixed string. */
   mixedString = (char *) malloc ((sizeof (char) * cdktemplate->plateLen) + 3);
   cleanChar (mixedString, cdktemplate->plateLen + 3, '\0');

   /* Start copying from the plate to the destination string. */
   while (platePos < cdktemplate->plateLen)
   {
      if (isPlateChar(cdktemplate->plate[platePos]))
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
   char *unmixedString	= 0;
   int infolen		= (int)strlen (info);
   int x		= 0;
   int pos		= 0;

   /* Create a char * pointer. */
   unmixedString	= (char *)malloc (sizeof(char) * (infolen + 2));
   cleanChar (unmixedString, infolen + 2, '\0');

   /* Start copying. */
   while (pos < infolen)
   {
      if (isPlateChar(cdktemplate->plate[pos]))
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
static void _moveCDKTemplate (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKTEMPLATE *cdktemplate = (CDKTEMPLATE *)object;
   int currentX = getbegx(cdktemplate->win);
   int currentY = getbegy(cdktemplate->win);
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
      xpos = getbegx(cdktemplate->win) + xplace;
      ypos = getbegy(cdktemplate->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(cdktemplate), &xpos, &ypos, cdktemplate->boxWidth, cdktemplate->boxHeight, BorderOf(cdktemplate));

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(cdktemplate->win, -xdiff, -ydiff);
   moveCursesWindow(cdktemplate->labelWin, -xdiff, -ydiff);
   moveCursesWindow(cdktemplate->fieldWin, -xdiff, -ydiff);
   moveCursesWindow(cdktemplate->shadowWin, -xdiff, -ydiff);

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
 * This function draws the tmeplate widget.
 */
static void _drawCDKTemplate (CDKOBJS *object, boolean Box)
{
   CDKTEMPLATE *cdktemplate = (CDKTEMPLATE *)object;
   int x;

   /* Do we need to draw the shadow. */
   if (cdktemplate->shadowWin != 0)
   {
      drawShadow (cdktemplate->shadowWin);
   }

   /* Box it if needed */
   if (Box)
   {
      drawObjBox (cdktemplate->win, ObjOf(cdktemplate));
   }

   /* Draw in the title if there is one. */
   if (cdktemplate->titleLines != 0)
   {
      for (x=0; x < cdktemplate->titleLines; x++)
      {
	 writeChtype (cdktemplate->win,
			cdktemplate->titlePos[x] + BorderOf(cdktemplate),
			x + BorderOf(cdktemplate),
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
   chtype fieldColor	= 0;
   int infolen		= (int)strlen (cdktemplate->info);
   int x		= 0;

   /* Draw in the label and the cdktemplate object. */
   if (cdktemplate->labelWin != 0)
   {
      writeChtype (cdktemplate->labelWin, 0, 0,
			cdktemplate->label,
			HORIZONTAL, 0,
			cdktemplate->labelLen);
      touchwin (cdktemplate->labelWin);
      wrefresh (cdktemplate->labelWin);
   }

   /* Draw in the cdktemplate... */
   if (cdktemplate->overlay != 0)
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
	 if (isPlateChar(cdktemplate->plate[x]) && pos < infolen)
	 {
	    fieldColor = cdktemplate->overlay[x] & A_ATTRIBUTES;
	    mvwaddch (cdktemplate->fieldWin, 0, x, (A_CHARTEXT & cdktemplate->info[pos++])|fieldColor);
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
   while (!isPlateChar(cdktemplate->plate[cdktemplate->platePos])
      &&  cdktemplate->platePos < cdktemplate->fieldWidth)
   {
      cdktemplate->platePos += direction;
      cdktemplate->screenPos += direction;
   }
   wmove (cdktemplate->fieldWin, 0, cdktemplate->screenPos);
   wrefresh (cdktemplate->fieldWin);
}

/*
 * This sets the background color of the widget.
 */
void setCDKTemplateBackgroundColor (CDKTEMPLATE *cdktemplate, char *color)
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
   setCDKTemplateBackgroundAttrib( cdktemplate, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This sets the background attribute of the widget.
 */
void setCDKTemplateBackgroundAttrib (CDKTEMPLATE *cdktemplate, chtype attrib)
{
   /* Set the widgets background attribute. */
   wbkgd (cdktemplate->win, attrib);
   wbkgd (cdktemplate->fieldWin, attrib);
   if (cdktemplate->labelWin != 0)
   {
      wbkgd (cdktemplate->labelWin, attrib);
   }
}

/*
 * This function destroys this widget.
 */
static void _destroyCDKTemplate (CDKOBJS *object)
{
   CDKTEMPLATE *cdktemplate = (CDKTEMPLATE *)object;
   int x;

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
}

/*
 * This function erases the widget.
 */
static void _eraseCDKTemplate (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKTEMPLATE *cdktemplate = (CDKTEMPLATE *)object;

      eraseCursesWindow (cdktemplate->fieldWin);
      eraseCursesWindow (cdktemplate->labelWin);
      eraseCursesWindow (cdktemplate->win);
      eraseCursesWindow (cdktemplate->shadowWin);
   }
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
   int len		= 0;
   int copychars	= 0;
   int x;

   /* Just to be sure, if let's make sure the new value isn't null. */
   if (newValue == 0)
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
      (cdktemplate->callbackfn)(cdktemplate, (chtype)newValue[x]);
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
   ObjOf(cdktemplate)->borderSize = Box ? 1 : 0;
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
   cdktemplate->callbackfn = callback;
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

static void _focusCDKTemplate(CDKOBJS *object GCC_UNUSED)
{
   /* FIXME */
}

static void _unfocusCDKTemplate(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _refreshDataCDKTemplate(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _saveDataCDKTemplate(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}
