#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2003/11/30 21:15:51 $
 * $Revision: 1.186 $
 */

/*
 * Declare file local prototypes.
 */
static void CDKEntryCallBack (CDKENTRY *entry, chtype character);
static void drawCDKEntryField (CDKENTRY *entry);

DeclareCDKObjects(ENTRY, Entry, setCdk, String);

/*
 * This creates a pointer to an entry widget.
 */
CDKENTRY *newCDKEntry (CDKSCREEN *cdkscreen, int xplace, int yplace, char *title, char *label, chtype fieldAttr, chtype filler, EDisplayType dispType, int fWidth, int min, int max, boolean Box, boolean shadow)
{
   CDKENTRY *entry	= 0;
   int parentWidth	= getmaxx(cdkscreen->window);
   int parentHeight	= getmaxy(cdkscreen->window);
   int fieldWidth	= fWidth;
   int boxWidth		= 0;
   int boxHeight;
   int xpos		= xplace;
   int ypos		= yplace;
   int junk		= 0;
   int horizontalAdjust, oldWidth;

   if ((entry = newCDKObject(CDKENTRY, &my_funcs)) == 0)
      return (0);

   setCDKEntryBox (entry, Box);
   boxHeight		= (BorderOf(entry) * 2) + 1;

  /*
   * If the fieldWidth is a negative value, the fieldWidth will
   * be COLS-fieldWidth, otherwise, the fieldWidth will be the
   * given width.
   */
   fieldWidth = setWidgetDimension (parentWidth, fieldWidth, 0);
   boxWidth = fieldWidth + 2 * BorderOf(entry);

   /* Set some basic values of the entry field. */
   entry->label		= 0;
   entry->labelLen	= 0;
   entry->labelWin	= 0;

   /* Translate the label char *pointer to a chtype pointer. */
   if (label != 0)
   {
      entry->label = char2Chtype (label, &entry->labelLen, &junk);
      boxWidth += entry->labelLen;
   }

   oldWidth = boxWidth;
   boxWidth = setCdkTitle(ObjOf(entry), title, boxWidth);
   horizontalAdjust = (boxWidth - oldWidth) / 2;

   boxHeight += TitleLinesOf(entry);

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);
   fieldWidth = MINIMUM(fieldWidth, boxWidth - entry->labelLen - 2 * BorderOf(entry));

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the label window. */
   entry->win = subwin (cdkscreen->window, boxHeight, boxWidth, ypos, xpos);

   /* Is the window null? */
   if (entry->win == 0)
   {
      _destroyCDKEntry (ObjOf(entry));
      return (0);
   }
   keypad (entry->win, TRUE);

   /* Make the field window. */
   entry->fieldWin = subwin (entry->win, 1, fieldWidth,
				ypos + TitleLinesOf(entry) + BorderOf(entry),
				xpos + entry->labelLen + horizontalAdjust + BorderOf(entry));
   keypad (entry->fieldWin, TRUE);

   /* Make the label win, if we need to. */
   if (label != 0)
   {
      entry->labelWin = subwin (entry->win, 1, entry->labelLen,
					ypos + TitleLinesOf(entry) + BorderOf(entry),
					xpos + horizontalAdjust + BorderOf(entry));
   }

   /* Make room for the info char * pointer. */
   entry->info		= (char *)malloc (sizeof(char) * (max + 3));
   cleanChar (entry->info, max + 3, '\0');
   entry->infoWidth	= max + 3;

   /* Set up the rest of the structure. */
   ScreenOf(entry)		= cdkscreen;
   entry->parent		= cdkscreen->window;
   entry->shadowWin		= 0;
   entry->fieldAttr		= fieldAttr;
   entry->fieldWidth		= fieldWidth;
   entry->filler		= filler;
   entry->hidden		= filler;
   ObjOf(entry)->inputWindow    = entry->fieldWin;
   ObjOf(entry)->acceptsFocus   = 1;
   ReturnOf(entry)              = NULL;
   entry->shadow		= shadow;
   entry->screenCol		= 0;
   entry->leftChar		= 0;
   entry->min			= min;
   entry->max			= max;
   entry->boxWidth		= boxWidth;
   entry->boxHeight		= boxHeight;
   entry->exitType		= vNEVER_ACTIVATED;
   entry->dispType		= dispType;
   entry->callbackfn		= CDKEntryCallBack;
   entry->preProcessFunction	= 0;
   entry->preProcessData	= 0;
   entry->postProcessFunction	= 0;
   entry->postProcessData	= 0;

   /* Do we want a shadow? */
   if (shadow)
   {
      entry->shadowWin = subwin (cdkscreen->window, boxHeight, boxWidth, ypos+1, xpos+1);
   }

   /* Clean the key bindings. */
   cleanCDKObjectBindings (vENTRY, entry);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vENTRY, entry);

   /* Return the pointer to the structure. */
   return (entry);
}

/*
 * This means you want to use the given entry field. It takes input
 * from the keyboard, and when its done, it fills the entry info
 * element of the structure with what was typed.
 */
char *activateCDKEntry (CDKENTRY *entry, chtype *actions)
{
   chtype input = 0;
   char *ret	= 0;

   /* Draw the widget. */
   drawCDKEntry (entry, ObjOf(entry)->box);

   /* Check if 'actions' is null. */
   if (actions == 0)
   {
      for (;;)
      {
	 /* Get the input. */
	 input = getcCDKObject (ObjOf(entry));

	 /* Inject the character into the widget. */
	 ret = injectCDKEntry (entry, input);
	 if (entry->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }
   else
   {
      int length = chlen (actions);
      int x =0;

      /* Inject each character one at a time. */
      for (x=0; x < length; x++)
      {
	 ret = injectCDKEntry (entry, actions[x]);
	 if (entry->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }

   /* Make sure we return the correct info. */
   if (entry->exitType == vNORMAL)
   {
      return entry->info;
   }
   else
   {
      return 0;
   }
}

/*
 * This injects a single character into the widget.
 */
static int _injectCDKEntry (CDKOBJS *object, chtype input)
{
   CDKENTRY *entry = (CDKENTRY *)object;
   int ppReturn = 1;
   int temp, x, charCount, stringLen;
   char holder;
   char *ret = unknownString;
   bool complete = FALSE;

   /* Set the exit type. */
   entry->exitType = vEARLY_EXIT;

   /* Refresh the entry field. */
   drawCDKEntryField (entry);

   /* Check if there is a pre-process function to be called. */
   if (entry->preProcessFunction != 0)
   {
      /* Call the pre-process function. */
      ppReturn = entry->preProcessFunction (vENTRY, entry, entry->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check a predefined binding... */
      if (checkCDKObjectBind (vENTRY, entry, input) != 0)
      {
	 entry->exitType = vEARLY_EXIT;
	 complete = TRUE;
      }
      else
      {
	 switch (input)
	 {
	    case KEY_UP : case KEY_DOWN :
		 Beep();
		 break;

	    case KEY_HOME : case CDK_BEGOFLINE :
		 entry->leftChar = 0;
		 entry->screenCol = 0;
		 drawCDKEntryField (entry);
		 break;

	    case CDK_TRANSPOSE :
		 stringLen = (int)strlen (entry->info);
		 temp = entry->leftChar + entry->screenCol;
		 if (temp >= stringLen-1)
		 {
		    Beep();
		 }
		 else
		 {
		    holder = entry->info[temp];
		    entry->info[temp] = entry->info[temp + 1];
		    entry->info[temp + 1] = holder;
		    drawCDKEntryField (entry);
		 }
		 break;

	    case KEY_END : case CDK_ENDOFLINE :
		 stringLen = (int)strlen (entry->info);
		 if (stringLen >= entry->fieldWidth)
		 {
		    charCount = (int)(entry->fieldWidth * .8);
		    entry->leftChar = stringLen - charCount;
		    entry->screenCol = charCount;
		 }
		 else
		 {
		    entry->leftChar = 0;
		    entry->screenCol = stringLen;
		 }
		 drawCDKEntryField (entry);
		 break;

	    case KEY_LEFT : case CDK_BACKCHAR :
		 if (entry->screenCol == 0)
		 {
		    if (entry->leftChar == 0)
		    {
		       Beep();
		    }
		    else
		    {
		       /* Scroll left.	*/
		       entry->leftChar--;
		       drawCDKEntryField (entry);
		    }
		 }
		 else
		 {
		    wmove (entry->fieldWin, 0, --entry->screenCol);
		    wrefresh (entry->fieldWin);
		 }
		 break;

	    case KEY_RIGHT : case CDK_FORCHAR :
		 if (entry->screenCol == entry->fieldWidth-1)
		 {
		    temp = (int)strlen (entry->info);
		    if ((entry->leftChar + entry->screenCol + 1) == temp)
		    {
		       Beep();
		    }
		    else
		    {
		       /* Scroll to the right. */
		       entry->leftChar++;
		       drawCDKEntryField (entry);
		    }
		 }
		 else
		 {
		    /* Move right. */
		    temp = (int)strlen (entry->info);
		    if ((entry->leftChar + entry->screenCol) == temp)
		    {
		       Beep();
		    }
		    else
		    {
		       wmove (entry->fieldWin, 0, ++entry->screenCol);
		    }
		    wrefresh (entry->fieldWin);
		 }
		 break;

	    case DELETE : case CONTROL('H') : case KEY_BACKSPACE : case KEY_DC :
		 if (entry->dispType == vVIEWONLY)
		 {
		    Beep();
		 }
		 else
		 {
		    /* Get the length of the widget information. */
		    int infoLength = (int)strlen (entry->info);
		    if ((entry->leftChar + entry->screenCol) < infoLength)
		    {
		       /* We are deleting from inside the string. */
		       int currPos = entry->screenCol + entry->leftChar;
		       for (x=currPos; x < infoLength; x++)
		       {
			  entry->info[x] = entry->info[x + 1];
		       }
		       entry->info[infoLength] = '\0';

		       /* Update the widget. */
		       drawCDKEntryField (entry);
		    }
		    else
		    {
		       /* We are deleting from the end of the string. */
		       if (infoLength > 0)
		       {
			  /* Update the character pointer. */
			  entry->info[infoLength-1] = '\0';
			  infoLength--;

			  /* Adjust the cursor. */
			  if (entry->screenCol > 0)
			  {
			     entry->screenCol--;
			  }

			  /*
			   * If we deleted the last character on the
			   * screen and the information has scrolled,
			   * adjust the entry field to show the info.
			   */
			   if (entry->leftChar > 0 && entry->screenCol == 1)
			   {
			      if (infoLength < entry->fieldWidth-1)
			      {
				 entry->leftChar = 0;
				 entry->screenCol = infoLength;
			      }
			      else
			      {
				 entry->leftChar -= (entry->fieldWidth-3);
				 entry->screenCol = entry->fieldWidth-2;
			      }
			   }

			  /* Update the widget. */
			  drawCDKEntryField (entry);
		       }
		       else
		       {
			  Beep();
		       }
		    }
		 }
		 break;

	    case KEY_ESC :
		 entry->exitType = vESCAPE_HIT;
		 complete = TRUE;
		 break;

	    case CDK_ERASE :
		 if ((int)strlen(entry->info) != 0)
		 {
		    cleanCDKEntry (entry);
		    drawCDKEntryField (entry);
		 }
		 break;

	    case CDK_CUT:
		 if ((int)strlen(entry->info) != 0)
		 {
		    freeChar (GPasteBuffer);
		    GPasteBuffer = copyChar (entry->info);
		    cleanCDKEntry (entry);
		    drawCDKEntryField (entry);
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case CDK_COPY:
		 if ((int)strlen(entry->info) != 0)
		 {
		    freeChar (GPasteBuffer);
		    GPasteBuffer = copyChar (entry->info);
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case CDK_PASTE:
		 if (GPasteBuffer != 0)
		 {
		    setCDKEntryValue (entry, GPasteBuffer);
		    drawCDKEntryField (entry);
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_RETURN : case KEY_TAB : case KEY_ENTER :
		 if ((int)strlen (entry->info) >= entry->min)
		 {
		    entry->exitType = vNORMAL;
		    ret = (entry->info);
		    complete = TRUE;
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case CDK_REFRESH :
		 eraseCDKScreen (ScreenOf(entry));
		 refreshCDKScreen (ScreenOf(entry));
		 break;

	    default :
		 (entry->callbackfn)(entry, input);
		 break;
	 }
      }

      /* Should we do a post-process? */
      if (!complete && (entry->postProcessFunction != 0))
      {
	 entry->postProcessFunction (vENTRY, entry, entry->postProcessData, input);
      }
   }

   if (!complete) {
      entry->exitType = vEARLY_EXIT;
   }

   ResultOf(entry).valueString = ret;
   return (ret != unknownString);
}

/*
 * This moves the entry field to the given location.
 */
static void _moveCDKEntry (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKENTRY *entry = (CDKENTRY *)object;
   int currentX = getbegx(entry->win);
   int currentY = getbegy(entry->win);
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
      xpos = getbegx(entry->win) + xplace;
      ypos = getbegy(entry->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(entry), &xpos, &ypos, entry->boxWidth, entry->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(entry->win, -xdiff, -ydiff);
   moveCursesWindow(entry->fieldWin, -xdiff, -ydiff);
   moveCursesWindow(entry->labelWin, -xdiff, -ydiff);
   moveCursesWindow(entry->shadowWin, -xdiff, -ydiff);

   /* Touch the windows so they 'move'. */
   touchwin (WindowOf(entry));
   wrefresh (WindowOf(entry));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      touchwin (entry->win);
      touchwin (entry->labelWin);
      touchwin (entry->fieldWin);
      drawCDKEntry (entry, ObjOf(entry)->box);
   }
}

/*
 * This is a generic character parser for the entry field. It is used as a
 * callback function, so any personal modifications can be made by creating
 * a new function and calling the activation with its name.
 */
static void CDKEntryCallBack (CDKENTRY *entry, chtype character)
{
   int plainchar = (character & A_CHARTEXT);
   unsigned temp, x;

   /* Start checking the input. */
   if (character <= 0 || character >= KEY_MIN)
   {
      Beep();
   }
   else if ((entry->dispType == vINT ||
	entry->dispType ==vHINT) &&
	!isdigit((int)plainchar))
   {
      Beep();
   }
   else if ((entry->dispType == vCHAR ||
		entry->dispType == vUCHAR ||
		entry->dispType == vLCHAR ||
		entry->dispType == vUHCHAR ||
		entry->dispType == vLHCHAR) &&
		isdigit((int)plainchar))
   {
      Beep();
   }
   else if (entry->dispType == vVIEWONLY)
   {
      Beep();
   }
   else if ((int)strlen (entry->info) == entry->max)
   {
      Beep();
   }
   else
   {
      /* We will make any adjustments to the case of the character. */
      if ((entry->dispType == vUCHAR ||
	     entry->dispType == vUHCHAR ||
	     entry->dispType == vUMIXED ||
	     entry->dispType == vUHMIXED)
	     && !isdigit(plainchar))
      {
	 plainchar = toupper (plainchar);
      }
      else if ((entry->dispType == vLCHAR ||
		     entry->dispType == vLHCHAR ||
		     entry->dispType == vLMIXED ||
		     entry->dispType == vLHMIXED) &&
		     !isdigit(plainchar))
      {
	 plainchar = tolower (plainchar);
      }

      /* Update the screen and pointer. */
      if (entry->screenCol != entry->fieldWidth-1)
      {
	 /* Update the character pointer. */
	 temp = strlen (entry->info);
	 for (x=temp; (int) x > entry->screenCol + entry->leftChar; x--)
	 {
	    entry->info[x] = entry->info[x-1];
	 }
	 entry->info[entry->screenCol + entry->leftChar] = plainchar;
	 entry->screenCol++;
      }
      else
      {
	 /* Update the character pointer. */
	 temp = strlen (entry->info);
	 entry->info[temp]     = plainchar;
	 entry->info[temp + 1] = '\0';
	 entry->leftChar++;
      }

      /* Update the entry field. */
      drawCDKEntryField (entry);
   }
}

/*
 * This erases the information in the entry field
 * and redraws a clean and empty entry field.
 */
void cleanCDKEntry (CDKENTRY *entry)
{
   int width = entry->fieldWidth;

   /* Erase the information in the character pointer. */
   cleanChar (entry->info, entry->infoWidth, '\0');

   /* Clean the entry screen field. */
   mvwhline(entry->fieldWin, 0, 0, entry->filler, width);

   /* Reset some variables. */
   entry->screenCol	= 0;
   entry->leftChar	= 0;

   /* Refresh the entry field. */
   wrefresh (entry->fieldWin);
}

/*
 * This draws the entry field.
 */
static void _drawCDKEntry (CDKOBJS *object, boolean Box)
{
   CDKENTRY *entry = (CDKENTRY *)object;

   /* Did we ask for a shadow? */
   if (entry->shadowWin != 0)
   {
      drawShadow (entry->shadowWin);
   }

   /* Box the widget if asked. */
   if (Box)
   {
      drawObjBox (entry->win, ObjOf(entry));
   }

   drawCdkTitle (entry->win, object);

   /* Refresh the window. */
   touchwin (entry->win);
   wrefresh (entry->win);

   /* Draw in the label to the widget. */
   if (entry->labelWin != 0)
   {
      writeChtype (entry->labelWin, 0, 0, entry->label, HORIZONTAL, 0, entry->labelLen);
      touchwin (entry->labelWin);
      wrefresh (entry->labelWin);
   }

   /* Redraw the entry field. */
   drawCDKEntryField (entry);
}

/*
 * This redraws the entry field.
 */
static void drawCDKEntryField (CDKENTRY *entry)
{
   int infoLength	= 0;
   int x		= 0;

   /* Draw in the filler characters.*/
   mvwhline(entry->fieldWin, 0, x, entry->filler, entry->fieldWidth);

   /* If there is information in the field. Then draw it in. */
   if (entry->info != 0)
   {
      infoLength = (int)strlen (entry->info);

      /* Redraw the field. */
      if (entry->dispType == vHINT ||
		entry->dispType == vHCHAR ||
		entry->dispType == vHMIXED ||
		entry->dispType == vUHCHAR ||
		entry->dispType == vLHCHAR ||
		entry->dispType == vUHMIXED ||
		entry->dispType == vLHMIXED)
      {
	 for (x=entry->leftChar; x < infoLength; x++)
	 {
	    mvwaddch (entry->fieldWin, 0, x - entry->leftChar, entry->hidden);
	 }
      }
      else
      {
	 for (x=entry->leftChar; x < infoLength; x++)
	 {
	    mvwaddch (entry->fieldWin, 0, x - entry->leftChar, (A_CHARTEXT & entry->info[x]) | entry->fieldAttr);
	 }
      }
      wmove (entry->fieldWin, 0, entry->screenCol);
   }

   /* Refresh the field. */
   touchwin (entry->fieldWin);
   wrefresh (entry->fieldWin);
}

/*
 * This erases an entry widget from the screen.
 */
static void _eraseCDKEntry (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKENTRY *entry = (CDKENTRY *)object;

      eraseCursesWindow (entry->fieldWin);
      eraseCursesWindow (entry->labelWin);
      eraseCursesWindow (entry->win);
      eraseCursesWindow (entry->shadowWin);
   }
}

/*
 * This destroys an entry widget.
 */
static void _destroyCDKEntry (CDKOBJS *object)
{
   CDKENTRY *entry = (CDKENTRY *)object;

   cleanCdkTitle (object);
   freeChtype (entry->label);
   freeChar (entry->info);

   /* Delete the windows. */
   deleteCursesWindow (entry->fieldWin);
   deleteCursesWindow (entry->labelWin);
   deleteCursesWindow (entry->shadowWin);
   deleteCursesWindow (entry->win);

   /* Unregister this object. */
   unregisterCDKObject (vENTRY, entry);
}

/*
 * This sets specific attributes of the entry field.
 */
void setCDKEntry (CDKENTRY *entry, char *value, int min, int max, boolean Box GCC_UNUSED)
{
   setCDKEntryValue (entry, value);
   setCDKEntryMin (entry, min);
   setCDKEntryMax (entry, max);
}

/*
 * This removes the old information in the entry field and keeps the
 * new information given.
 */
void setCDKEntryValue (CDKENTRY *entry, char *newValue)
{
   int copychars	= 0;
   int stringLen	= 0;
   int charCount	= 0;

   /* If the pointer sent in is the same pointer as before, do nothing. */
   if (entry->info == newValue)
   {
      return;
   }

   /* Just to be sure, if lets make sure the new value isn't null. */
   if (newValue == 0)
   {
      /* Then we want to just erase the old value. */
      cleanChar (entry->info, entry->infoWidth, '\0');

      /* Set the pointers back to zero. */
      entry->leftChar = 0;
      entry->screenCol = 0;
      return;
   }

   /* Determine how many characters we need to copy. */
   copychars = MINIMUM ((int)strlen(newValue), entry->max);

   /* OK, erase the old value, and copy in the new value. */
   cleanChar (entry->info, entry->max, '\0');
   strncpy (entry->info, newValue, (unsigned) copychars);
   stringLen = (int)strlen (entry->info);

   /* Now determine the values of leftChar and screenCol. */
   if (stringLen >= entry->fieldWidth)
   {
      charCount		= (int)(entry->fieldWidth * .8);
      entry->leftChar	= stringLen - charCount;
      entry->screenCol	= charCount;
   }
   else
   {
      entry->leftChar = 0;
      entry->screenCol = stringLen;
   }
}
char *getCDKEntryValue (CDKENTRY *entry)
{
   return entry->info;
}

/*
 * This sets the maximum length of the string that will be accepted.
 */
void setCDKEntryMax (CDKENTRY *entry, int max)
{
   entry->max = max;
}
int getCDKEntryMax (CDKENTRY *entry)
{
   return entry->max;
}

/*
 * This sets the minimum length of the string that will
 * be accepted.
 */
void setCDKEntryMin (CDKENTRY *entry, int min)
{
   entry->min = min;
}
int getCDKEntryMin (CDKENTRY *entry)
{
   return entry->min;
}

/*
 * This sets the filler character to be used in the entry field.
 */
void setCDKEntryFillerChar (CDKENTRY *entry, chtype fillerCharacter)
{
   entry->filler = fillerCharacter;
}
chtype getCDKEntryFillerChar (CDKENTRY *entry)
{
   return entry->filler;
}

/*
 * This sets the character to use when a hidden type is used.
 */
void setCDKEntryHiddenChar (CDKENTRY *entry, chtype hiddenCharacter)
{
   entry->hidden = hiddenCharacter;
}
chtype getCDKEntryHiddenChar (CDKENTRY *entry)
{
   return entry->hidden;
}

/*
 * This sets the widgets box attribute.
 */
void setCDKEntryBox (CDKENTRY *entry, boolean Box)
{
   ObjOf(entry)->box = Box;
   ObjOf(entry)->borderSize = Box ? 1 : 0;
}
boolean getCDKEntryBox (CDKENTRY *entry)
{
   return ObjOf(entry)->box;
}

/*
 * This sets the background color of the widget.
 */
void setCDKEntryBackgroundColor (CDKENTRY *entry, char *color)
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
   setCDKEntryBackgroundAttrib (entry, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This sets the background attribute of the widget.
 */
void setCDKEntryBackgroundAttrib (CDKENTRY *entry, chtype attrib)
{
   /* Set the widgets background attribute. */
   wbkgd (entry->win, attrib);
   wbkgd (entry->fieldWin, attrib);
   if (entry->labelWin != 0)
   {
      wbkgd (entry->labelWin, attrib);
   }
}

/*
 * This sets the attribute of the entry field.
 */
void setCDKEntryHighlight (CDKENTRY *entry, chtype highlight, boolean cursor)
{
   wbkgd (entry->fieldWin, highlight);
   entry->fieldAttr = highlight;
   curs_set (cursor);
   /*
   **  FIXME -  if (cursor) { move the cursor to this widget }
   */
}

/*
 * This sets the entry field callback function.
 */
void setCDKEntryCB (CDKENTRY *entry, ENTRYCB callback)
{
   entry->callbackfn = callback;
}

/*
 * This function sets the pre-process function.
 */
void setCDKEntryPreProcess (CDKENTRY *entry, PROCESSFN callback, void *data)
{
   entry->preProcessFunction = callback;
   entry->preProcessData = data;
}

/*
 * This function sets the post-process function.
 */
void setCDKEntryPostProcess (CDKENTRY *entry, PROCESSFN callback, void *data)
{
   entry->postProcessFunction = callback;
   entry->postProcessData = data;
}

static void _focusCDKEntry(CDKOBJS *object)
{
   CDKENTRY *entry = (CDKENTRY *)object;

   wmove (entry->fieldWin, 0, entry->screenCol);
   curs_set(1);
   wrefresh (entry->fieldWin);
}

static void _unfocusCDKEntry(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _refreshDataCDKEntry(CDKOBJS *object)
{
   CDKENTRY *entry = (CDKENTRY *)object;

   if (ReturnOf(entry))
   {
      switch (DataTypeOf(entry))
      {
      default:
      case DataTypeString:
	 strcpy(entry->info, (char *)ReturnOf(entry));
	 break;
      case DataTypeInt:
	 sprintf(entry->info, "%d", *((int*)ReturnOf(entry)));
	 break;
      case DataTypeFloat:
	 sprintf(entry->info, "%g", *((float*)ReturnOf(entry)));
	 break;
      case DataTypeDouble:
	 sprintf(entry->info, "%g", *((double*)ReturnOf(entry)));
	 break;
      }
      entry->screenCol = strlen(entry->info);
      drawCDKEntryField(entry);
   }
}

static void _saveDataCDKEntry(CDKOBJS *object)
{
   CDKENTRY *entry = (CDKENTRY *)object;

   if (ReturnOf(entry))
   {
      switch (DataTypeOf(entry))
      {
      default:
      case DataTypeString:
	 strcpy(ReturnOf(entry), entry->info);
	 break;
      case DataTypeInt:
	 *((int*)ReturnOf(entry)) = atoi(entry->info);
	 break;
      case DataTypeFloat:
	 *((float*)ReturnOf(entry)) = atof(entry->info);
	 break;
      case DataTypeDouble:
	 *((double*)ReturnOf(entry)) = atof(entry->info);
	 break;
      }
   }
}
