#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2003/11/19 00:18:24 $
 * $Revision: 1.56 $
 */

static int createList (CDKITEMLIST *itemlist, char **item, int count);

DeclareCDKObjects(ITEMLIST, Itemlist, setCdk, Int);

/*
 * This creates a pointer to an itemlist widget.
 */
CDKITEMLIST *newCDKItemlist (CDKSCREEN *cdkscreen, int xplace, int yplace, char *title, char *label, char **item, int count, int defaultItem, boolean Box, boolean shadow)
{
   /* Set up some variables.  */
   CDKITEMLIST *itemlist = 0;
   chtype *holder	= 0;
   int parentWidth	= getmaxx(cdkscreen->window) - 1;
   int parentHeight	= getmaxy(cdkscreen->window) - 1;
   int boxWidth		= 0;
   int boxHeight;
   int maxWidth		= INT_MIN;
   int fieldWidth	= 0;
   int xpos		= xplace;
   int ypos		= yplace;
   int horizontalAdjust = 0;
   char **temp		= 0;
   int x, len, junk, junk2;

   if ((itemlist = newCDKObject(CDKITEMLIST, &my_funcs)) == 0
    || !createList(itemlist, item, count))
   {
      _destroyCDKItemlist (ObjOf(itemlist));
      return (0);
   }

   setCDKItemlistBox (itemlist, Box);
   boxHeight		= (BorderOf(itemlist) * 2) + 1;

   /* Set some basic values of the itemlist. */
   itemlist->label	= 0;
   itemlist->titleLines = 0;
   itemlist->labelLen	= 0;
   itemlist->labelWin	= 0;

   /* Translate the label char *pointer to a chtype pointer. */
   if (label != 0)
   {
      itemlist->label = char2Chtype (label, &itemlist->labelLen, &junk);
   }

   /* Go through the list and determine the widest item. */
   for (x=0; x < count; x++)
   {
      maxWidth = MAXIMUM (maxWidth, itemlist->itemLen[x]);
   }

   /*
    * Set the box width.  Allow an extra char in field width for cursor
    */
   fieldWidth = maxWidth + 1;
   boxWidth = fieldWidth + itemlist->labelLen + 2 * BorderOf(itemlist);

   /* Translate the char * items to chtype * */
   if (title != 0)
   {
      int titleWidth;

      temp = CDKsplitString (title, '\n');
      itemlist->titleLines = CDKcountStrings (temp);

      /* We need to determine the widest title line. */
      for (x=0; x < itemlist->titleLines; x++)
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
	  boxWidth = maxWidth + 2 * BorderOf(itemlist);
       }

      /* For each line in the title, convert from char * to chtype * */
      titleWidth = boxWidth - (2 * BorderOf(itemlist));
      for (x=0; x < itemlist->titleLines; x++)
      {
	 itemlist->title[x]	= char2Chtype (temp[x], &itemlist->titleLen[x], &itemlist->titlePos[x]);
	 itemlist->titlePos[x]	= justifyString (titleWidth, itemlist->titleLen[x], itemlist->titlePos[x]);
      }
      CDKfreeStrings(temp);
   }
   else
   {
      /* No title? Set the required variables. */
      itemlist->titleLines = 0;
   }
   boxHeight += itemlist->titleLines;

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);
   fieldWidth = MINIMUM(fieldWidth, boxWidth - itemlist->labelLen - 2 * BorderOf(itemlist));

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight, BorderOf(itemlist));

   /* Make the window. */
   itemlist->win = newwin(boxHeight, boxWidth, ypos, xpos);
   if (itemlist->win == 0)
   {
      _destroyCDKItemlist (ObjOf(itemlist));
      return (0);
   }

   /* Make the label window if there was a label. */
   if (itemlist->label != 0)
   {
      itemlist->labelWin = subwin (itemlist->win,
				   1,
				   itemlist->labelLen,
				   ypos + BorderOf(itemlist) + itemlist->titleLines,
				   xpos + BorderOf(itemlist));
      if (itemlist->labelWin == 0)
      {
	 _destroyCDKItemlist (ObjOf(itemlist));
	 return (0);
      }
   }

   keypad (itemlist->win, TRUE);

   /* Make the field window */
   itemlist->fieldWin = subwin (itemlist->win,
				1,
				fieldWidth,
				ypos + BorderOf(itemlist) + itemlist->titleLines,
				xpos + itemlist->labelLen + BorderOf(itemlist));
   if (itemlist->fieldWin == 0)
   {
      _destroyCDKItemlist (ObjOf(itemlist));
      return (0);
   }

   keypad (itemlist->fieldWin, TRUE);

   /* Set up the rest of the structure. */
   ScreenOf(itemlist)			= cdkscreen;
   itemlist->parent			= cdkscreen->window;
   itemlist->shadowWin			= 0;
   itemlist->boxHeight			= boxHeight;
   itemlist->boxWidth			= boxWidth;
   itemlist->fieldWidth			= fieldWidth;
   itemlist->listSize			= count;
   itemlist->exitType			= vNEVER_ACTIVATED;
   ObjOf(itemlist)->acceptsFocus	= 1;
   ObjOf(itemlist)->inputWindow		= itemlist->fieldWin;
   itemlist->shadow			= shadow;
   itemlist->preProcessFunction		= 0;
   itemlist->preProcessData		= 0;
   itemlist->postProcessFunction	= 0;
   itemlist->postProcessData		= 0;

   setCDKItemlistBox (itemlist, Box);

   /* Set then default item. */
   if (defaultItem >= 0 && defaultItem < itemlist->listSize)
   {
      itemlist->currentItem	= defaultItem;
      itemlist->defaultItem	= defaultItem;
   }
   else
   {
      itemlist->currentItem	= 0;
      itemlist->defaultItem	= 0;
   }

   /* Do we want a shadow??? */
   if (shadow)
   {
      itemlist->shadowWin = newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
      if (itemlist->shadowWin == 0)
      {
	 _destroyCDKItemlist (ObjOf(itemlist));
	 return (0);
      }
   }

   /* Clean the key bindings. */
   cleanCDKObjectBindings (vITEMLIST, itemlist);

   /* Register this baby.		 */
   registerCDKObject (cdkscreen, vITEMLIST, itemlist);

   /* Return the pointer to the structure */
   return (itemlist);
}

/*
 * This allows the user to play with the widget.
 */
int activateCDKItemlist (CDKITEMLIST *itemlist, chtype *actions)
{
   /* Declare local variables. */
   int ret = -1;

   /* Draw the widget. */
   drawCDKItemlist (itemlist, ObjOf(itemlist)->box);
   drawCDKItemlistField(itemlist, TRUE);

   /* Check if actions is null. */
   if (actions == 0)
   {
      chtype input = 0;
      for (;;)
      {
	 /* Get the input. */
	 input = getcCDKObject (ObjOf(itemlist));

	 /* Inject the character into the widget. */
	 ret = injectCDKItemlist (itemlist, input);
	 if (itemlist->exitType != vEARLY_EXIT)
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
	 ret = injectCDKItemlist (itemlist, actions[x]);
	 if (itemlist->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }

   /* Set the exit type and exit. */
   itemlist->exitType = vEARLY_EXIT;
   return ret;
}

/*
 * This injects a single character into the widget.
 */
static int _injectCDKItemlist (CDKOBJS *object, chtype input)
{
   CDKITEMLIST *itemlist = (CDKITEMLIST *)object;
   /* Declare local variables. */
   int ppReturn = 1;
   int ret = unknownInt;
   bool complete = FALSE;

   /* Set the exit type. */
   itemlist->exitType = vEARLY_EXIT;

   /* Draw the itemlist field. */
   drawCDKItemlistField (itemlist,TRUE);

   /* Check if there is a pre-process function to be called. */
   if (itemlist->preProcessFunction != 0)
   {
      /* Call the pre-process function. */
      ppReturn = itemlist->preProcessFunction (vITEMLIST, itemlist, itemlist->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check a predefined binding. */
      if (checkCDKObjectBind (vITEMLIST, itemlist, input) != 0)
      {
	 itemlist->exitType = vESCAPE_HIT;
	 complete = TRUE;
      }
      else
      {
	 switch (input)
	 {
	    case KEY_UP : case KEY_RIGHT : case ' ' : case '+' : case 'n' :
		 if (itemlist->currentItem < itemlist->listSize - 1)
		 {
		    itemlist->currentItem++;
		 }
		 else
		 {
		    itemlist->currentItem = 0;
		 }
		 break;

	    case KEY_DOWN : case KEY_LEFT : case '-' : case 'p' :
		 if (itemlist->currentItem > 0)
		 {
		    itemlist->currentItem--;
		 }
		 else
		 {
		    itemlist->currentItem = itemlist->listSize - 1;
		 }
		 break;

	    case 'd' : case 'D' :
		 itemlist->currentItem = itemlist->defaultItem;
		 break;

	    case '0' :
		 itemlist->currentItem = 0;
		 break;

	    case '$' :
		 itemlist->currentItem = itemlist->listSize - 1;
		 break;

	    case KEY_ESC :
		 itemlist->exitType = vESCAPE_HIT;
		 complete = TRUE;
		 break;

	    case KEY_RETURN : case KEY_TAB : case KEY_ENTER :
		 itemlist->exitType = vNORMAL;
		 ret = itemlist->currentItem;
		 complete = TRUE;
		 break;

	    case CDK_REFRESH :
		 eraseCDKScreen (ScreenOf(itemlist));
		 refreshCDKScreen (ScreenOf(itemlist));
		 break;

	    default :
		 Beep();
		 break;
	 }
      }

      /* Should we call a post-process? */
      if (!complete && (itemlist->postProcessFunction != 0))
      {
	 itemlist->postProcessFunction (vITEMLIST, itemlist, itemlist->postProcessData, input);
      }
   }

   if (!complete) {
      drawCDKItemlistField (itemlist,TRUE);
      itemlist->exitType = vEARLY_EXIT;
   }

   ResultOf(itemlist).valueInt = ret;
   return (ret != unknownInt);
}

/*
 * This moves the itemlist field to the given location.
 */
static void _moveCDKItemlist (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKITEMLIST *itemlist = (CDKITEMLIST *)object;
   /* Declare local variables. */
   int currentX = getbegx(itemlist->win);
   int currentY = getbegy(itemlist->win);
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
      xpos = getbegx(itemlist->win) + xplace;
      ypos = getbegy(itemlist->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(itemlist), &xpos, &ypos, itemlist->boxWidth, itemlist->boxHeight, BorderOf(itemlist));

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(itemlist->win, -xdiff, -ydiff);
   moveCursesWindow(itemlist->fieldWin, -xdiff, -ydiff);
   moveCursesWindow(itemlist->labelWin, -xdiff, -ydiff);
   moveCursesWindow(itemlist->shadowWin, -xdiff, -ydiff);

   /* Touch the windows so they 'move'. */
   touchwin (WindowOf(itemlist));
   wrefresh (WindowOf(itemlist));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKItemlist (itemlist, ObjOf(itemlist)->box);
   }
}

/*
 * This draws the widget on the screen.
 */
static void _drawCDKItemlist (CDKOBJS *object, int Box)
{
   CDKITEMLIST *itemlist = (CDKITEMLIST *)object;
   int x;

   /* Did we ask for a shadow? */
   if (itemlist->shadowWin != 0)
   {
      drawShadow (itemlist->shadowWin);
   }

   /* Box the widget if asked. */
   if (Box)
   {
      drawObjBox (itemlist->win, ObjOf(itemlist));
   }

   /* Draw in the title if there is one. */
   for (x=0; x < itemlist->titleLines; x++)
   {
      writeChtype (itemlist->win,
		itemlist->titlePos[x] + BorderOf(itemlist),
		x + BorderOf(itemlist),
		itemlist->title[x],
		HORIZONTAL,
		0,
		chlen(itemlist->title[x]));
   }

   /* Draw in the label to the widget. */
   if (itemlist->labelWin != 0)
   {
      writeChtype (itemlist->labelWin,
		0,
		0,
		itemlist->label,
		HORIZONTAL,
		0,
		chlen(itemlist->label));
   }

   touchwin (itemlist->win);
   wrefresh (itemlist->win);

   /* Draw in the field. */
   drawCDKItemlistField(itemlist, FALSE);
}

/*
 * This sets the background color of the widget.
 */
void setCDKItemlistBackgroundColor (CDKITEMLIST *itemlist, char *color)
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
   setCDKItemlistBackgroundAttrib (itemlist, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This sets the background attribute of the widget.
 */
void setCDKItemlistBackgroundAttrib (CDKITEMLIST *itemlist, chtype attrib)
{
   /* Set the widgets background attribute. */
   wbkgd (itemlist->win, attrib);
   wbkgd (itemlist->fieldWin, attrib);
   if (itemlist->labelWin != 0)
   {
      wbkgd (itemlist->labelWin, attrib);
   }
}

/*
 * This function draws the contents of the field.
 */
void drawCDKItemlistField (CDKITEMLIST *itemlist, boolean highlight)
{
   /* Declare local vars. */
   int currentItem = itemlist->currentItem;
   int len;
   int x;

   /* Determine how much we have to draw. */
   len = MINIMUM (itemlist->itemLen[currentItem], itemlist->fieldWidth);

   /* Erase the field window. */
   werase (itemlist->fieldWin);

   /* Draw in the current item in the field. */
   for (x=0; x < len; x++)
   {
      chtype c = itemlist->item[currentItem][x];

      if (highlight)
      {
	c &= A_CHARTEXT;
	c |= A_REVERSE;
      }

      mvwaddch (itemlist->fieldWin, 0,
		x + itemlist->itemPos[currentItem],
		c);
   }

   /* Redraw the field window. */
   touchwin (itemlist->fieldWin);
   wrefresh (itemlist->fieldWin);
}

/*
 * This function removes the widget from the screen.
 */
static void _eraseCDKItemlist (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKITEMLIST *itemlist = (CDKITEMLIST *)object;

      eraseCursesWindow (itemlist->fieldWin);
      eraseCursesWindow (itemlist->labelWin);
      eraseCursesWindow (itemlist->win);
      eraseCursesWindow (itemlist->shadowWin);
   }
}

/*
 * This function destroys the widget and all the memory it used.
 */
static void _destroyCDKItemlist (CDKOBJS *object)
{
   if (object != 0)
   {
      CDKITEMLIST *itemlist = (CDKITEMLIST *)object;
      /* Declare local variables. */
      int x;

      /* Clear out the character pointers. */
      freeChtype (itemlist->label);
      for (x=0; x < itemlist->titleLines; x++)
      {
	 freeChtype (itemlist->title[x]);
      }
      CDKfreeChtypes (itemlist->item);

      /* Delete the windows. */
      deleteCursesWindow (itemlist->fieldWin);
      deleteCursesWindow (itemlist->labelWin);
      deleteCursesWindow (itemlist->shadowWin);
      deleteCursesWindow (itemlist->win);

      /* Unregister this object. */
      unregisterCDKObject (vITEMLIST, itemlist);
   }
}

/*
 * This sets multiple attributes of the widget.
 */
void setCDKItemlist (CDKITEMLIST *itemlist, char **list, int count, int current, boolean Box)
{
   setCDKItemlistValues (itemlist, list, count, current);
   setCDKItemlistBox (itemlist, Box);
}

/*
 * This function sets the contents of the list.
 */
void setCDKItemlistValues (CDKITEMLIST *itemlist, char **item, int count, int defaultItem)
{
   if (createList(itemlist, item, count))
   {
      /* Set the default item. */
      if ((defaultItem >= 0) && (defaultItem < itemlist->listSize))
      {
	 itemlist->currentItem = defaultItem;
	 itemlist->defaultItem = defaultItem;
      }

      /* Draw the field. */
      eraseCDKItemlist (itemlist);
      drawCDKItemlist (itemlist, ObjOf(itemlist)->box);
   }
}
chtype **getCDKItemlistValues (CDKITEMLIST *itemlist, int *size)
{
   (*size) = itemlist->listSize;
   return itemlist->item;
}

/*
 * This sets the default/current item of the itemlist.
 */
void setCDKItemlistCurrentItem (CDKITEMLIST *itemlist, int currentItem)
{
   /* Set the default item. */
   if ((currentItem >= 0) && (currentItem < itemlist->listSize))
   {
      itemlist->currentItem = currentItem;
   }
}
int getCDKItemlistCurrentItem (CDKITEMLIST *itemlist)
{
   return itemlist->currentItem;
}

/*
 * This sets the default item in the list.
 */
void setCDKItemlistDefaultItem (CDKITEMLIST *itemlist, int defaultItem)
{
   /* Make sure the item is in the correct range. */
   if (defaultItem < 0)
   {
      itemlist->defaultItem = 0;
   }
   else if (defaultItem >= itemlist->listSize)
   {
      itemlist->defaultItem = itemlist->listSize - 1;
   }
   else
   {
      itemlist->defaultItem = defaultItem;
   }
}
int getCDKItemlistDefaultItem (CDKITEMLIST *itemlist)
{
   return itemlist->defaultItem;
}

/*
 * This sets the box attribute of the itemlist widget.
 */
void setCDKItemlistBox (CDKITEMLIST *itemlist, boolean Box)
{
   ObjOf(itemlist)->box = Box;
   ObjOf(itemlist)->borderSize = Box ? 1 : 0;
}
boolean getCDKItemlistBox (CDKITEMLIST *itemlist)
{
   return ObjOf(itemlist)->box;
}

/*
 * This function sets the pre-process function.
 */
void setCDKItemlistPreProcess (CDKITEMLIST *itemlist, PROCESSFN callback, void *data)
{
   itemlist->preProcessFunction = callback;
   itemlist->preProcessData = data;
}

/*
 * This function sets the post-process function.
 */
void setCDKItemlistPostProcess (CDKITEMLIST *itemlist, PROCESSFN callback, void *data)
{
   itemlist->postProcessFunction = callback;
   itemlist->postProcessData = data;
}

static void _focusCDKItemlist(CDKOBJS *object)
{
   CDKITEMLIST *itemlist = (CDKITEMLIST *)object;

   curs_set(0);
   drawCDKItemlistField (itemlist,TRUE);
}

static void _unfocusCDKItemlist(CDKOBJS *object)
{
   CDKITEMLIST *itemlist = (CDKITEMLIST *)object;

   drawCDKItemlistField (itemlist,FALSE);
}

static void _refreshDataCDKItemlist(CDKOBJS *object)
{
   CDKITEMLIST *itemlist = (CDKITEMLIST *)object;

   if (ReturnOf(itemlist))
   {
      switch (DataTypeOf(itemlist))
      {
      default:
      case DataTypeInt:
	 itemlist->currentItem = *((int*)ReturnOf(itemlist));
	 break;
      case DataTypeDouble:
	 itemlist->currentItem = *((double*)ReturnOf(itemlist));
	 break;
      case DataTypeFloat:
	 itemlist->currentItem = *((float*)ReturnOf(itemlist));
	 break;
      case DataTypeString:
	 {
	    int i;

	    for (i=0; i < itemlist->listSize; ++i)
	       if (!cmpStrChstr((char*)ReturnOf(itemlist),itemlist->item[i]))
	       {
		  itemlist->currentItem = i;
		  break;
	       }
	    itemlist->currentItem = itemlist->defaultItem;
	    break;
	 }
      }
      drawCDKItemlistField(itemlist, FALSE);
   }
}

static void _saveDataCDKItemlist(CDKOBJS *object)
{
   CDKITEMLIST *itemlist = (CDKITEMLIST *)object;

   if (ReturnOf(itemlist))
   {
      switch (DataTypeOf(itemlist))
      {
      default:
      case DataTypeInt:
	 *((int*)ReturnOf(itemlist)) = itemlist->currentItem;
	 break;
      case DataTypeFloat:
	 *((float*)ReturnOf(itemlist)) = itemlist->currentItem;
	 break;
      case DataTypeDouble:
	 *((double*)ReturnOf(itemlist)) = itemlist->currentItem;
	 break;
      case DataTypeString:
	 chstrncpy((char*)ReturnOf(itemlist), itemlist->item[itemlist->currentItem], 9999);
	 break;
      }
   }
}

static int createList (CDKITEMLIST *itemlist, char **item, int count)
{
   int status = 0;

   if (count > 0)
   {
      chtype **newItems = typeCallocN(chtype*, count + 1);
      int *newPos = typeCallocN(int, count + 1);
      int *newLen = typeCallocN(int, count + 1);
      int x;
      int fieldWidth = 0;

      if (newItems != 0
	 && newPos != 0
	 && newLen != 0)
      {
	 /* Go through the list and determine the widest item. */
	 status = 1;
	 for (x=0; x < count; x++)
	 {
	    /* Copy the item to the list. */
	    newItems[x] = char2Chtype (item[x], &newLen[x], &newPos[x]);
	    if (newItems[x] == 0)
	    {
	       status = 0;
	       break;
	    }
	    fieldWidth = MAXIMUM (fieldWidth, newLen[x]);
	 }

	 /* Now we need to justify the strings. */
	 for (x=0; x < count; x++)
	 {
	    newPos[x] = justifyString (fieldWidth + 1, newLen[x], newPos[x]);
	 }
      }

      if (status)
      {
	 /* Free up the old memory. */
	 CDKfreeChtypes (itemlist->item);
	 free(itemlist->itemPos);
	 free(itemlist->itemLen);

	 /* Copy in the new information. */
	 itemlist->listSize = count;
	 itemlist->item = newItems;
	 itemlist->itemPos = newPos;
	 itemlist->itemLen = newLen;
      }
      else
      {
	 CDKfreeChtypes(newItems);
	 if (newPos != 0) free(newPos);
	 if (newLen != 0) free(newLen);
      }
   }
   return status;
}
