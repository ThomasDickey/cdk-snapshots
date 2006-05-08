#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2006/05/05 00:27:44 $
 * $Revision: 1.79 $
 */

static int createList (CDKITEMLIST *itemlist, char **item, int count);
static int createFieldWin (CDKITEMLIST *itemlist, int ypos, int xpos);
static int maximumFieldWidth (CDKITEMLIST *itemlist);
static void updateFieldWidth (CDKITEMLIST *itemlist);

DeclareCDKObjects(ITEMLIST, Itemlist, setCdk, Int);

/*
 * This creates a pointer to an itemlist widget.
 */
CDKITEMLIST *newCDKItemlist (CDKSCREEN *cdkscreen, int xplace, int yplace, char *title, char *label, char **item, int count, int defaultItem, boolean Box, boolean shadow)
{
   /* Set up some variables.  */
   CDKITEMLIST *itemlist = 0;
   int parentWidth	= getmaxx(cdkscreen->window);
   int parentHeight	= getmaxy(cdkscreen->window);
   int boxWidth		= 0;
   int boxHeight;
   int fieldWidth	= 0;
   int xpos		= xplace;
   int ypos		= yplace;
   int junk;

   if ((itemlist = newCDKObject(CDKITEMLIST, &my_funcs)) == 0
    || !createList(itemlist, item, count))
   {
      destroyCDKObject (itemlist);
      return (0);
   }

   setCDKItemlistBox (itemlist, Box);
   boxHeight		= (BorderOf(itemlist) * 2) + 1;

   /* Set some basic values of the itemlist. */
   itemlist->label	= 0;
   itemlist->labelLen	= 0;
   itemlist->labelWin	= 0;

   /* Translate the label char *pointer to a chtype pointer. */
   if (label != 0)
   {
      itemlist->label = char2Chtype (label, &itemlist->labelLen, &junk);
   }

   /*
    * Set the box width.  Allow an extra char in field width for cursor
    */
   fieldWidth = maximumFieldWidth (itemlist) + 1;
   boxWidth = fieldWidth + itemlist->labelLen + 2 * BorderOf(itemlist);

   boxWidth = setCdkTitle(ObjOf(itemlist), title, boxWidth);

   boxHeight += TitleLinesOf(itemlist);

   /*
    * Make sure we didn't extend beyond the dimensions of the window.
    */
   itemlist->boxWidth = MINIMUM(boxWidth, parentWidth);
   itemlist->boxHeight = MINIMUM(boxHeight, parentHeight);
   updateFieldWidth (itemlist);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the window. */
   itemlist->win = newwin(boxHeight, boxWidth, ypos, xpos);
   if (itemlist->win == 0)
   {
      destroyCDKObject (itemlist);
      return (0);
   }

   /* Make the label window if there was a label. */
   if (itemlist->label != 0)
   {
      itemlist->labelWin = subwin (itemlist->win,
				   1,
				   itemlist->labelLen,
				   ypos + BorderOf(itemlist) + TitleLinesOf(itemlist),
				   xpos + BorderOf(itemlist));
      if (itemlist->labelWin == 0)
      {
	 destroyCDKObject (itemlist);
	 return (0);
      }
   }

   keypad (itemlist->win, TRUE);

   /* Make the field window */
   if (!createFieldWin (itemlist, 
			ypos + BorderOf(itemlist) + TitleLinesOf(itemlist),
			xpos + itemlist->labelLen + BorderOf(itemlist)))
   {
      destroyCDKObject (itemlist);
      return (0);
   }

   /* Set up the rest of the structure. */
   ScreenOf(itemlist)			= cdkscreen;
   itemlist->parent			= cdkscreen->window;
   itemlist->shadowWin			= 0;
   initExitType(itemlist);
   ObjOf(itemlist)->acceptsFocus	= TRUE;
   itemlist->shadow			= shadow;

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
	 destroyCDKObject (itemlist);
	 return (0);
      }
   }

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

   if (actions == 0)
   {
      chtype input = 0;
      boolean functionKey;

      for (;;)
      {
	 input = getchCDKObject (ObjOf(itemlist), &functionKey);

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
   setExitType(itemlist, 0);
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
   setExitType(itemlist, 0);

   /* Draw the itemlist field. */
   drawCDKItemlistField (itemlist,TRUE);

   /* Check if there is a pre-process function to be called. */
   if (PreProcessFuncOf(itemlist) != 0)
   {
      /* Call the pre-process function. */
      ppReturn = PreProcessFuncOf(itemlist) (vITEMLIST, itemlist, PreProcessDataOf(itemlist), input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check a predefined binding. */
      if (checkCDKObjectBind (vITEMLIST, itemlist, input) != 0)
      {
	 checkEarlyExit(itemlist);
	 complete = TRUE;
      }
      else
      {
	 switch (input)
	 {
	    case KEY_UP : case KEY_RIGHT : case SPACE : case '+' : case 'n' :
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
		 setExitType(itemlist, input);
		 complete = TRUE;
		 break;

	    case KEY_TAB : case KEY_ENTER :
		 setExitType(itemlist, input);
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
      if (!complete && (PostProcessFuncOf(itemlist) != 0))
      {
	 PostProcessFuncOf(itemlist) (vITEMLIST, itemlist, PostProcessDataOf(itemlist), input);
      }
   }

   if (!complete)
   {
      drawCDKItemlistField (itemlist,TRUE);
      setExitType(itemlist, 0);
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
   alignxy (WindowOf(itemlist), &xpos, &ypos, itemlist->boxWidth, itemlist->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(itemlist->win, -xdiff, -ydiff);
   moveCursesWindow(itemlist->fieldWin, -xdiff, -ydiff);
   moveCursesWindow(itemlist->labelWin, -xdiff, -ydiff);
   moveCursesWindow(itemlist->shadowWin, -xdiff, -ydiff);

   /* Touch the windows so they 'move'. */
   refreshCDKWindow (WindowOf(itemlist));

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

   drawCdkTitle (itemlist->win, object);

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

   wrefresh (itemlist->win);

   /* Draw in the field. */
   drawCDKItemlistField(itemlist, FALSE);
}

/*
 * This sets the background attribute of the widget.
 */
static void _setBKattrItemlist (CDKOBJS *object, chtype attrib)
{
   if (object != 0)
   {
      CDKITEMLIST *widget = (CDKITEMLIST *) object;

      wbkgd (widget->win, attrib);
      wbkgd (widget->fieldWin, attrib);
      if (widget->labelWin != 0)
      {
	 wbkgd (widget->labelWin, attrib);
      }
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
	c = CharOf(c) | A_REVERSE;
      }

      mvwaddch (itemlist->fieldWin, 0,
		x + itemlist->itemPos[currentItem],
		c);
   }

   /* Redraw the field window. */
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

static void destroyInfo (CDKITEMLIST *widget)
{
   widget->listSize = 0;

   CDKfreeChtypes (widget->item);
   widget->item = 0;

   freeAndNull (widget->itemPos);
   freeAndNull (widget->itemLen);
}

/*
 * This function destroys the widget and all the memory it used.
 */
static void _destroyCDKItemlist (CDKOBJS *object)
{
   if (object != 0)
   {
      CDKITEMLIST *itemlist = (CDKITEMLIST *)object;

      cleanCdkTitle (object);
      freeChtype (itemlist->label);
      destroyInfo (itemlist);

      /* Delete the windows. */
      deleteCursesWindow (itemlist->fieldWin);
      deleteCursesWindow (itemlist->labelWin);
      deleteCursesWindow (itemlist->shadowWin);
      deleteCursesWindow (itemlist->win);

      /* Clean the key bindings. */
      cleanCDKObjectBindings (vITEMLIST, itemlist);

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
      int oldWidth = itemlist->fieldWidth;

      /* Set the default item. */
      if ((defaultItem >= 0) && (defaultItem < itemlist->listSize))
      {
	 itemlist->currentItem = defaultItem;
	 itemlist->defaultItem = defaultItem;
      }

      /*
       * This will not resize the outer windows but can still make a usable
       * field width if the title made the outer window wide enough.
       */
      updateFieldWidth (itemlist);
      if (itemlist->fieldWidth > oldWidth)
      {
	 createFieldWin (itemlist,
			 getbegy(itemlist->fieldWin),
			 getbegx(itemlist->fieldWin));
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

static void _focusCDKItemlist(CDKOBJS *object)
{
   CDKITEMLIST *itemlist = (CDKITEMLIST *)object;

   drawCDKItemlistField (itemlist,TRUE);
}

static void _unfocusCDKItemlist(CDKOBJS *object)
{
   CDKITEMLIST *itemlist = (CDKITEMLIST *)object;

   drawCDKItemlistField (itemlist,FALSE);
}

#if 0
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
#else
dummyRefreshData(Itemlist)
dummySaveData(Itemlist)
#endif

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
	 destroyInfo (itemlist);

	 /* Copy in the new information. */
	 itemlist->listSize = count;
	 itemlist->item = newItems;
	 itemlist->itemPos = newPos;
	 itemlist->itemLen = newLen;
      }
      else
      {
	 CDKfreeChtypes (newItems);
	 freeChecked (newPos);
	 freeChecked (newLen);
      }
   }
   else
   {
      destroyInfo (itemlist);
      status = TRUE;
   }

   return status;
}

/* Go through the list and determine the widest item. */
static int maximumFieldWidth (CDKITEMLIST *itemlist)
{
   int x;
   int maxWidth	= INT_MIN;

   for (x=0; x < itemlist->listSize; x++)
   {
      maxWidth = MAXIMUM (maxWidth, itemlist->itemLen[x]);
   }
   maxWidth = MAXIMUM (maxWidth, 0);

   return maxWidth;
}

static void updateFieldWidth (CDKITEMLIST *itemlist)
{
   int want = maximumFieldWidth (itemlist) + 1;
   int have = itemlist->boxWidth - itemlist->labelLen - 2 * BorderOf(itemlist);
   itemlist->fieldWidth = MINIMUM(want, have);
}

/* Make the field window */
static int createFieldWin (CDKITEMLIST *itemlist, int ypos, int xpos)
{
   itemlist->fieldWin = subwin (itemlist->win,
				1,
				itemlist->fieldWidth,
				ypos,
				xpos);
   if (itemlist->fieldWin != 0)
   {
      keypad (itemlist->fieldWin, TRUE);
      ObjOf(itemlist)->inputWindow = itemlist->fieldWin;
      return 1;
   }
   return 0;
}
