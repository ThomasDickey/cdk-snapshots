#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2003/11/30 21:15:51 $
 * $Revision: 1.72 $
 */

/*
 * Declare file local prototypes.
 */
static void cleanUpMenu (CDKMENU *menu);

DeclareCDKObjects(MENU, Menu, setCdk, Int);

/*
 * This creates a new menu widget.
 */
CDKMENU *newCDKMenu (CDKSCREEN *cdkscreen, char *menulist[MAX_MENU_ITEMS][MAX_SUB_ITEMS], int menuItems, int *subsize, int *menuloc, int menuPos, chtype titleAttr, chtype subtitleAttr)
{
   CDKMENU *menu	= 0;
   int rightcount	= menuItems - 1;
   int rightloc		= getmaxx((cdkscreen->window));
   int leftloc		= 0;
   int x, y, max, junk;

   if ((menu = newCDKObject(CDKMENU, &my_funcs)) == 0)
      return (0);

   /* Start making a copy of the information. */
   ScreenOf(menu)		= cdkscreen;
   ObjOf(menu)->box		= FALSE;
   ObjOf(menu)->acceptsFocus    = FALSE;
   rightcount			= menuItems-1;
   menu->parent			= cdkscreen->window;
   menu->menuItems		= menuItems;
   menu->titleAttr		= titleAttr;
   menu->subtitleAttr		= subtitleAttr;
   menu->currentTitle		= 0;
   menu->currentSubtitle	= 0;
   menu->lastSelection		= -1;
   menu->preProcessFunction	= 0;
   menu->preProcessData		= 0;
   menu->postProcessFunction	= 0;
   menu->postProcessData	= 0;
   menu->menuPos		= menuPos;
   menu->exitType		= vNEVER_ACTIVATED;
   ObjOf(menu)->inputWindow     = menu->titleWin[menu->currentTitle];

   /* Create the pull down menus. */
   for (x=0; x < menuItems; x++)
   {
      max = -1;
      for (y=1; y < subsize[x] ; y++)
      {
	 if (menuloc[x] == LEFT)
	 {
	    menu->sublist[x][y-1] = char2Chtype (menulist[x][y], &menu->sublistLen[x][y-1], &junk);
	    max = MAXIMUM (max, menu->sublistLen[x][y-1]);
	 }
	 else
	 {
	    menu->sublist[rightcount][y-1] = char2Chtype (menulist[x][y], &menu->sublistLen[rightcount][y-1], &junk);
	    max = MAXIMUM (max, menu->sublistLen[rightcount][y-1]);
	 }
      }

      if (menuloc[x] == LEFT)
      {
	/*
	 * Its a menu item on the left, add it to the left side
	 * of the menu.
	 */
	 menu->title[x]		= char2Chtype (menulist[x][0], &menu->titleLen[x], &junk);
	 menu->subsize[x]	= subsize[x] - 1;
	 if (menu->menuPos == BOTTOM)
	 {
	    menu->titleWin[x]	= subwin (cdkscreen->window, 1, menu->titleLen[x]+2, LINES-1, leftloc);
	    menu->pullWin[x]	= subwin (cdkscreen->window, subsize[x]+1, max+2, LINES-subsize[x]-2, leftloc);
	 }
	 else
	 {
	    menu->titleWin[x]	= subwin (cdkscreen->window, 1, menu->titleLen[x]+2, 0, leftloc);
	    menu->pullWin[x]	= subwin (cdkscreen->window, subsize[x]+1, max+2, 1, leftloc);
	 }
	 leftloc += menu->titleLen[x] + 1;

	 /* Allow the windows to use the keypad. */
	 keypad (menu->titleWin[x], TRUE);
	 keypad (menu->pullWin[x], TRUE);
      }
      else
      {
	/*
	 * Its a menu item on the right, add it to the right side
	 * of the menu.
	 */
	 rightloc			-= max + 3;
	 menu->title[rightcount]	= char2Chtype (menulist[x][0], &menu->titleLen[rightcount], &junk);
	 menu->subsize[rightcount]	= subsize[x] - 1;
	 if (menu->menuPos == BOTTOM)
	 {
	    menu->titleWin[rightcount]	= subwin (cdkscreen->window, 1, menu->titleLen[x]+2, LINES-1, rightloc);
	    menu->pullWin[rightcount]	= subwin (cdkscreen->window, subsize[x]+1, max+2, LINES-subsize[x]-2, rightloc);
	 }
	 else
	 {
	    menu->titleWin[rightcount]	= subwin (cdkscreen->window, 1, menu->titleLen[x]+2, 0, rightloc);
	    menu->pullWin[rightcount]	= subwin (cdkscreen->window, subsize[x]+1, max+2, 1, rightloc);
	 }

	 /* Allow the windows to use the keypad. */
	 keypad (menu->titleWin[rightcount], TRUE);
	 keypad (menu->pullWin[rightcount], TRUE);
	 rightcount--;
      }
   }

   /* Clean the key bindings. */
   cleanCDKObjectBindings (vMENU, menu);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vMENU, menu);

   /* Return the menu object. */
   return (menu);
}

/*
 * This activates the CDK Menu.
 */
int activateCDKMenu (CDKMENU *menu, chtype *actions)
{
   chtype input;
   int ret;

   /* Draw in the screen. */
   refreshCDKScreen (ScreenOf(menu));

   /* Display the menu titles. */
   drawCDKMenu(menu, ObjOf(menu)->box);

   /* Highlight the current title and window. */
   drawCDKMenuSubwin (menu);

   /* If the input string is null, this is an interactive activate. */
   if (actions == 0)
   {
      /* Start taking input from the keyboard. */
      for (;;)
      {
	 input = wgetch (menu->titleWin[menu->currentTitle]);

	 /* Inject the character into the widget. */
	 ret = injectCDKMenu (menu, input);
	 if (menu->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }
   else
   {
      int count = chlen (actions);
      int x = 0;

      for (x=0; x < count; x++)
      {
	 ret = injectCDKMenu (menu, actions[x]);
	 if (menu->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }

   /* Set the exit type and return. */
   menu->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This injects a character into the menu widget.
 */
static int _injectCDKMenu (CDKOBJS *object, chtype input)
{
   CDKMENU *menu = (CDKMENU *)object;
   int ppReturn = 1;
   int ret = unknownInt;
   bool complete = FALSE;

   /* Set the exit type. */
   menu->exitType = vEARLY_EXIT;

   /* Check if there is a pre-process function to be called. */
   if (menu->preProcessFunction != 0)
   {
      /* Call the pre-process function. */
      ppReturn = menu->preProcessFunction (vMENU, menu, menu->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for key bindings. */
      if (checkCDKObjectBind (vMENU, menu, input) != 0)
      {
	 menu->exitType = vESCAPE_HIT;
	 complete = TRUE;
      }
      else
      {
	 switch (input)
	 {
	    case KEY_LEFT :
		 /* Erase the menu sub-window */
		 eraseCDKMenuSubwin (menu);
		 refreshCDKScreen (ScreenOf(menu));

		 /* Set the values. */
		 menu->currentSubtitle = 0;
		 if (menu->currentTitle == 0)
		 {
		    menu->currentTitle = menu->menuItems - 1;
		 }
		 else
		 {
		    menu->currentTitle--;
		 }

		 /* Draw the new menu sub-window. */
		 drawCDKMenuSubwin (menu);
		 ObjOf(menu)->inputWindow = menu->titleWin[menu->currentTitle];
		 break;

	    case KEY_RIGHT : case KEY_TAB :
		 /* Erase the menu sub-window. */
		 eraseCDKMenuSubwin (menu);
		 refreshCDKScreen (ScreenOf(menu));

		 /* Set the values. */
		 menu->currentSubtitle = 0;
		 if (menu->currentTitle == menu->menuItems-1)
		 {
		       menu->currentTitle = 0;
		 }
		 else
		 {
		    menu->currentTitle++;
		 }

		 /* Draw the new menu sub-window. */
		 drawCDKMenuSubwin (menu);
		 ObjOf(menu)->inputWindow = menu->titleWin[menu->currentTitle];
		 break;

	    case KEY_UP :
		 /* Erase the old subtitle. */
		 writeChtype (menu->pullWin[menu->currentTitle],
				1, menu->currentSubtitle+1,
				menu->sublist[menu->currentTitle][menu->currentSubtitle],
				HORIZONTAL,
				0, menu->sublistLen[menu->currentTitle][menu->currentSubtitle]);

		 /* Set the values. */
		 if (menu->currentSubtitle == 0)
		 {
		    menu->currentSubtitle = menu->subsize[menu->currentTitle] - 1;
		 }
		 else
		 {
		    menu->currentSubtitle--;
		 }

		 /* Draw the new sub-title. */
		 writeChtypeAttrib (menu->pullWin[menu->currentTitle],
					1, menu->currentSubtitle+1,
					menu->sublist[menu->currentTitle][menu->currentSubtitle],
					menu->subtitleAttr,
					HORIZONTAL,
					0, menu->sublistLen[menu->currentTitle][menu->currentSubtitle]);
		 wrefresh (menu->pullWin[menu->currentTitle]);
		 ObjOf(menu)->inputWindow = menu->titleWin[menu->currentTitle];
		 break;

	    case KEY_DOWN : case SPACE :
		 /* Erase the old subtitle. */
		 writeChtype (menu->pullWin[menu->currentTitle],
				1, menu->currentSubtitle+1,
				menu->sublist[menu->currentTitle][menu->currentSubtitle],
				HORIZONTAL,
				0, menu->sublistLen[menu->currentTitle][menu->currentSubtitle]);

		 /* Set the values. */
		 if (menu->currentSubtitle == menu->subsize[menu->currentTitle]-1)
		 {
		    menu->currentSubtitle = 0;
		 }
		 else
		 {
		    menu->currentSubtitle++;
		 }

		 /* Draw the new sub-title. */
		 writeChtypeAttrib (
				menu->pullWin[menu->currentTitle],
				1, menu->currentSubtitle+1,
				menu->sublist[menu->currentTitle][menu->currentSubtitle],
				menu->subtitleAttr,
				HORIZONTAL,
				0, menu->sublistLen[menu->currentTitle][menu->currentSubtitle]);
		 wrefresh (menu->pullWin[menu->currentTitle]);
		 ObjOf(menu)->inputWindow = menu->titleWin[menu->currentTitle];
		 break;

	    case KEY_RETURN : case KEY_ENTER :
		 cleanUpMenu (menu);
		 menu->exitType = vNORMAL;
		 menu->lastSelection = ((menu->currentTitle * 100) + menu->currentSubtitle);
		 ret = menu->lastSelection;
		 complete = TRUE;
		 break;

	    case KEY_ESC :
		 cleanUpMenu (menu);
		 menu->exitType = vESCAPE_HIT;
		 menu->lastSelection = -1;
		 ret = menu->lastSelection;
		 complete = TRUE;
		 break;

	    case CDK_REFRESH :
		 eraseCDKScreen (ScreenOf(menu));
		 refreshCDKScreen (ScreenOf(menu));
		 break;
	 }
      }

      /* Should we call a post-process? */
      if (!complete && (menu->postProcessFunction != 0))
      {
	 menu->postProcessFunction (vMENU, menu, menu->postProcessData, input);
      }
   }

   if (!complete) {
      menu->exitType = vEARLY_EXIT;
   }

   ResultOf(menu).valueInt = ret;
   return (ret != unknownInt);
}

/*
 * This draws a menu item subwindow.
 */
void drawCDKMenuSubwin (CDKMENU *menu)
{
   int x;

   /* Box the window. */
   box (menu->pullWin[menu->currentTitle], ACS_VLINE, ACS_HLINE);
   if (menu->menuPos == BOTTOM)
   {
      mvwaddch (menu->pullWin[menu->currentTitle],
		menu->subsize[menu->currentTitle]+1, 0, ACS_LTEE);
   }
   else
   {
      mvwaddch (menu->pullWin[menu->currentTitle], 0, 0, ACS_LTEE);
   }

   /* Draw in the items. */
   for (x=0; x < menu->subsize[menu->currentTitle] ; x++)
   {
      writeChtype (menu->pullWin[menu->currentTitle],
			1, x+1,
			menu->sublist[menu->currentTitle][x],
			HORIZONTAL,
			0, menu->sublistLen[menu->currentTitle][x]);
   }

   /* Highlight the current sub-menu item. */
   writeChtypeAttrib (menu->pullWin[menu->currentTitle],
			1, menu->currentSubtitle+1,
			menu->sublist[menu->currentTitle][menu->currentSubtitle],
			menu->subtitleAttr, HORIZONTAL, 0,
			menu->sublistLen[menu->currentTitle][menu->currentSubtitle]);
   touchwin (menu->pullWin[menu->currentTitle]);
   wrefresh (menu->pullWin[menu->currentTitle]);

   /* Highlight the title. */
   writeChtypeAttrib (menu->titleWin[menu->currentTitle],
			0, 0, menu->title[menu->currentTitle],
			menu->titleAttr, HORIZONTAL, 0,
			menu->titleLen[menu->currentTitle]);
   touchwin (menu->titleWin[menu->currentTitle]);
   wrefresh (menu->titleWin[menu->currentTitle]);
}

/*
 * This erases a menu item subwindow.
 */
void eraseCDKMenuSubwin (CDKMENU *menu)
{
   eraseCursesWindow (menu->pullWin[menu->currentTitle]);

   /* Redraw the sub-menu title. */
   writeChtype (menu->titleWin[menu->currentTitle],
		0, 0, menu->title[menu->currentTitle],
		HORIZONTAL,
		0, menu->titleLen[menu->currentTitle]);
   touchwin (menu->titleWin[menu->currentTitle]);
   wrefresh (menu->titleWin[menu->currentTitle]);
}

/*
 * This function draws the menu.
 */
static void _drawCDKMenu (CDKOBJS *object, boolean Box GCC_UNUSED)
{
   CDKMENU *menu = (CDKMENU *)object;
   int x;

   /* Draw in the menu titles. */
   for (x=0; x < menu->menuItems; x++)
   {
      writeChtype (menu->titleWin[x], 0, 0, menu->title[x], HORIZONTAL, 0, menu->titleLen[x]);
      touchwin (menu->titleWin[x]);
      wrefresh (menu->titleWin[x]);
   }
}

/*
 * This moves the menu to the given location.
 */
static void _moveCDKMenu (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKMENU *menu = (CDKMENU *)object;
   int currentX = getbegx(WindowOf(menu));
   int currentY = getbegy(WindowOf(menu));
   int xpos	= xplace;
   int ypos	= yplace;
   int xdiff	= 0;
   int ydiff	= 0;
   int x;

   /*
    * If this is a relative move, then we will adjust where we want
    * to move to.
    */
   if (relative)
   {
      xpos = getbegx(WindowOf(menu)) + xplace;
      ypos = getbegy(WindowOf(menu)) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(menu), &xpos, &ypos, getmaxx(WindowOf(menu)), getmaxy(WindowOf(menu)));

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the windows to the new location. */
   moveCursesWindow(WindowOf(menu), -xdiff, -ydiff);
   for (x=0; x < menu->menuItems; x++)
   {
      moveCursesWindow(menu->titleWin[x], -xdiff, -ydiff);
   }

   /* Touch the windows so they 'move'. */
   touchwin (WindowOf(menu));
   wrefresh (WindowOf(menu));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKMenu (menu, ObjOf(menu)->box);
   }
}

/*
 * This sets the background color of the widget.
 */
void setCDKMenuBackgroundColor (CDKMENU *menu, char *color)
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
   setCDKMenuBackgroundAttrib (menu, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This sets the background attribute of the widget.
 */
void setCDKMenuBackgroundAttrib (CDKMENU *menu, chtype attrib)
{
   int x;
   /* Set the widgets background attribute. */
   for (x=0; x < menu->menuItems; x++)
   {
      wbkgd (menu->titleWin[x], attrib);
      wbkgd (menu->pullWin[x], attrib);
   }
}

/*
 * This function destroys a menu widget.
 */
static void _destroyCDKMenu (CDKOBJS *object)
{
   CDKMENU *menu = (CDKMENU *)object;
   int x, y;

   /* Clean up both the winodws and the char pointers. */
   for (x=0; x < menu->menuItems; x++)
   {
      /* Clean the windows. */
      deleteCursesWindow (menu->titleWin[x]);
      deleteCursesWindow (menu->pullWin[x]);

      /* Delete the character pointers. */
      freeChtype (menu->title[x]);
      for (y=0; y < menu->subsize[x] ; y++)
      {
	 freeChtype (menu->sublist[x][y]);
      }
   }

   /* Unregister this object. */
   unregisterCDKObject (vMENU, menu);
}

/*
 * This function erases the menu widget from the screen.
 */
static void _eraseCDKMenu (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKMENU *menu = (CDKMENU *)object;
      int x = 0;

      /* Erase the menu items. */
      for (x=0 ; x < menu->menuItems; x++)
      {
	 werase (menu->titleWin[x]);
	 wrefresh (menu->titleWin[x]);
	 werase (menu->pullWin[x]);
	 wrefresh (menu->pullWin[x]);
      }
   }
}

/*
 * This sets multiple features of the menu.
 */
void setCDKMenu (CDKMENU *menu, int menuItem, int subMenuItem, chtype titleHighlight, chtype subTitleHighlight)
{
   setCDKMenuCurrentItem (menu, menuItem, subMenuItem);
   setCDKMenuTitleHighlight (menu, titleHighlight);
   setCDKMenuSubTitleHighlight (menu, subTitleHighlight);
}

/*
 * This sets the current menu itme to highlight.
 */
void setCDKMenuCurrentItem (CDKMENU *menu, int menuitem, int submenuitem)
{
   /* Set the menu item position values. */
   if (menuitem < 0)
   {
      menu->currentTitle = 0;
   }
   else if (menuitem >= menu->menuItems)
   {
      menu->currentTitle = menu->menuItems - 1;
   }
   else
   {
      menu->currentTitle = menuitem;
   }
   if (submenuitem < 0)
   {
      menu->currentSubtitle = 0;
   }
   else if (submenuitem >= menu->subsize[menu->currentTitle])
   {
      menu->currentSubtitle = menu->subsize[menu->currentTitle]-1;
   }
   else
   {
      menu->currentSubtitle = submenuitem;
   }
}
void getCDKMenuCurrentItem (CDKMENU *menu, int *menuItem, int *subMenuItem)
{
   (*menuItem)		= menu->currentTitle;
   (*subMenuItem)	= menu->currentSubtitle;
}

/*
 * This sets the attribute of the menu titles.
 */
void setCDKMenuTitleHighlight (CDKMENU *menu, chtype highlight)
{
   menu->titleAttr = highlight;
}
chtype getCDKMenuTitleHighlight (CDKMENU *menu)
{
   return menu->titleAttr;
}

/*
 * This sets the attribute of the sub-title.
 */
void setCDKMenuSubTitleHighlight (CDKMENU *menu, chtype highlight)
{
   menu->subtitleAttr = highlight;
}
chtype getCDKMenuSubTitleHighlight (CDKMENU *menu)
{
   return menu->subtitleAttr;
}

/*
 * This exits the menu.
 */
static void cleanUpMenu (CDKMENU *menu)
{
   /* Erase the sub-menu. */
   eraseCDKMenuSubwin (menu);
   wrefresh (menu->pullWin[menu->currentTitle]);

   /* Refresh the screen. */
   refreshCDKScreen (ScreenOf(menu));
}

/*
 * This function sets the pre-process function.
 */
void setCDKMenuPreProcess (CDKMENU *menu, PROCESSFN callback, void *data)
{
   menu->preProcessFunction = callback;
   menu->preProcessData = data;
}

/*
 * This function sets the post-process function.
 */
void setCDKMenuPostProcess (CDKMENU *menu, PROCESSFN callback, void *data)
{
   menu->postProcessFunction = callback;
   menu->postProcessData = data;
}


static void _focusCDKMenu(CDKOBJS *object)
{
   CDKMENU *menu = (CDKMENU *)object;

   curs_set(0);
   drawCDKMenuSubwin(menu);
   ObjOf(menu)->inputWindow = menu->titleWin[menu->currentTitle];
}

static void _unfocusCDKMenu(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _refreshDataCDKMenu(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _saveDataCDKMenu(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}
