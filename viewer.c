#include <cdk.h>

/*
 * $Author: tom $
 * $Date: 2002/07/27 16:06:08 $
 * $Revision: 1.116 $
 */

/*
 * Declare some local definitions.
 */
#define		DOWN		0
#define		UP		1

/*
 * Declare file local prototypes.
 */
static int searchForWord (CDKVIEWER *viewer, char *pattern, int direction);
static int jumpToLine (CDKVIEWER *viewer);
static void popUpLabel (CDKVIEWER *viewer, char **mesg);
static void getAndStorePattern (CDKSCREEN *screen);
static void drawCDKViewerButtons (CDKVIEWER *viewer);
static void drawCDKViewerInfo (CDKVIEWER *viewer);

/*
 * Declare file local variables.
 */
static char *	SearchPattern	= 0;
int		SearchDirection = DOWN;

DeclareCDKObjects(VIEWER, Viewer, Unknown);

/*
 * This function creates a new viewer object.
 */
CDKVIEWER *newCDKViewer (CDKSCREEN *cdkscreen, int xplace, int yplace, int height, int width, char **buttons, int buttonCount, chtype buttonHighlight, boolean Box, boolean shadow)
{
   /* Declare local variables. */
   CDKVIEWER *viewer	= newCDKObject(CDKVIEWER, &my_funcs);
   int parentWidth	= getmaxx(cdkscreen->window) - 1;
   int parentHeight	= getmaxy(cdkscreen->window) - 1;
   int boxWidth		= width;
   int boxHeight	= height;
   int xpos		= xplace;
   int ypos		= yplace;
   int borderSize       = 1;
   int buttonWidth	= 0;
   int buttonAdj	= 0;
   int buttonPos	= 1;
   int x		= 0;

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

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight, borderSize);

   /* Make the viewer window. */
   viewer->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the window null? */
   if (viewer->win == 0)
   {
      /* Clean up any used memory. */
      free (viewer);

      /* Return a null pointer. */
      return (0);
   }

   /* Turn the keypad on for the viewer. */
   keypad (viewer->win, TRUE);

   /* Create the buttons. */
   viewer->buttonCount = buttonCount;
   if (buttonCount > 0)
   {
      for (x=0; x < buttonCount; x++)
      {
	 viewer->button[x]	= char2Chtype (buttons[x], &viewer->buttonLen[x], &buttonAdj);
	 buttonWidth		+= viewer->buttonLen[x] + 1;
      }
      buttonAdj = (int)((boxWidth-buttonWidth)/ (buttonCount +1));
      buttonPos = 1 + buttonAdj;
      for (x=0; x < buttonCount; x++)
      {
	 viewer->buttonPos[x]	= buttonPos;
	 buttonPos		+= buttonAdj + viewer->buttonLen[x];
      }
   }

   /* Set the rest of the variables */
   ScreenOf(viewer)		= cdkscreen;
   viewer->parent		= cdkscreen->window;
   viewer->shadowWin		= 0;
   viewer->buttonHighlight	= buttonHighlight;
   viewer->boxHeight		= boxHeight;
   viewer->boxWidth		= boxWidth - 2;
   viewer->viewSize		= height - 2;
   ObjOf(viewer)->box		= Box;
   ObjOf(viewer)->borderSize	= borderSize;
   ObjOf(viewer)->inputWindow	= viewer->win;
   viewer->exitType		= vNEVER_ACTIVATED;
   viewer->shadow		= shadow;
   viewer->currentButton	= 0;
   viewer->currentTop		= 0;
   viewer->length		= 0;
   viewer->leftChar		= 0;
   viewer->maxLeftChar		= 0;
   viewer->maxTopLine		= 0;
   viewer->characters		= 0;
   viewer->infoSize		= -1;
   viewer->showLineInfo		= 1;
   viewer->exitType		= vEARLY_EXIT;
   viewer->titleLines		= 0;
   viewer->ULChar		= ACS_ULCORNER;
   viewer->URChar		= ACS_URCORNER;
   viewer->LLChar		= ACS_LLCORNER;
   viewer->LRChar		= ACS_LRCORNER;
   viewer->HChar		= ACS_HLINE;
   viewer->VChar		= ACS_VLINE;
   viewer->BoxAttrib		= A_NORMAL;

   /* Do we need to create a shadow??? */
   if (shadow)
   {
      viewer->shadowWin = newwin (boxHeight, boxWidth + 1, ypos + 1, xpos + 1);
   }

   /* Clean the key bindings. */
   cleanCDKObjectBindings (vVIEWER, viewer);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vVIEWER, viewer);

   /* Return the viewer object. */
   return (viewer);
}

/*
 * This function sets various attributes of the widget.
 */
int setCDKViewer (CDKVIEWER *viewer, char *title, char **info, int infoSize, chtype buttonHighlight, boolean attrInterp, boolean showLineInfo, boolean Box)
{
   setCDKViewerTitle (viewer, title);
   setCDKViewerHighlight (viewer, buttonHighlight);
   setCDKViewerInfoLine (viewer, showLineInfo);
   setCDKViewerBox (viewer, Box);
   return setCDKViewerInfo (viewer, info, infoSize, attrInterp);
}

/*
 * This sets the title of the viewer. (A null title is allowed.
 * It just means that the viewer will not have a title when drawn.)
 */
void setCDKViewerTitle (CDKVIEWER *viewer, char *title)
{
   char **temp;
   int x;

   /* Clean out the old title. */
   for (x=0; x < viewer->titleLines; x++)
   {
      freeChtype (viewer->title[x]);
      viewer->title[x] = 0;
      viewer->titlePos[x] = 0;
      viewer->titleLen[x] = 0;
   }
   viewer->titleLines = 0;
   viewer->titleAdj = 0;

   /* Create the new title if there is one. */
   if (title != 0)
   {
      temp = CDKsplitString (title, '\n');
      viewer->titleLines = CDKcountStrings (temp);
      viewer->titleLines = MINIMUM(viewer->titleLines, MAX_LINES - 1);

      /* For each line in the title, convert from char * to chtype * */
      for (x=0; x < viewer->titleLines; x++)
      {
	 viewer->title[x]	= char2Chtype (temp[x], &viewer->titleLen[x], &viewer->titlePos[x]);
	 viewer->titlePos[x]	= justifyString (viewer->boxWidth, viewer->titleLen[x], viewer->titlePos[x]);
      }
      viewer->titleAdj = viewer->titleLines;
      CDKfreeStrings(temp);
   }

   /* Need to set viewer->viewSize. */
   viewer->viewSize = viewer->boxHeight - (viewer->titleLines + 1) - 2;
}
chtype **getCDKViewerTitle (CDKVIEWER *viewer)
{
   return viewer->title;
}

/*
 * This function sets the contents of the viewer.
 */
int setCDKViewerInfo (CDKVIEWER *viewer, char **info, int infoSize, boolean interpret)
{
   /* Declare local variables. */
   char filename[512];
   int widestLine	= -1;
   int currentLine	= 0;
   int x		= 0;

   infoSize = MINIMUM(infoSize, MAX_LINES - 4);

   /* Clean out the old viewer info. (if there is any) */
   for (x=0; x < viewer->infoSize; x++)
   {
      freeChtype (viewer->info[x]);
      viewer->info[x] = 0;
   }
   memset (filename, '\0', 512);

   /* Keep some semi-perinant info. */
   viewer->interpret = interpret;

   /* Copy the information given. */
   currentLine = 0;
   for (x=0; x <= infoSize; x++)
   {
      if (info[x] == 0)
      {
	 viewer->info[currentLine]	= 0;
	 viewer->infoLen[currentLine]	= 0;
	 viewer->infoPos[currentLine]	= 0;
	 currentLine++;
      }
      else
      {
	 /* Check if we have a file link in this line. */
	 if (checkForLink (info[x], filename) == 1)
	 {
	    /* We have a link, open the file. */
	    char **fileContents = 0, temp[256];
	    int fileLen		= 0;
	    int fileLine	= 0;

	    /* Open the file and put it into the viewer. */
	    fileLen = CDKreadFile (filename, &fileContents);
	    if (fileLen == -1)
	    {
		int adj		= 0;
		int len		= 0;

	       /* Could not open the file. */
#ifdef HAVE_START_COLOR
	       sprintf (temp, "<C></16>Link Failed: Could not open the file %s", filename);
#else
	       sprintf (temp, "<C></K>Link Failed: Could not open the file %s", filename);
#endif
	       viewer->info[currentLine]	= char2Chtype (temp, &len, &adj);
	       viewer->infoPos[currentLine]	= justifyString (viewer->boxWidth, len, adj);
	       viewer->infoLen[currentLine]	= len;
	       widestLine = MAXIMUM(widestLine, viewer->infoLen[currentLine]);
	       currentLine++;
	    }
	    else
	    {
	       /* For each line read, copy it into the viewer. */
	       fileLen = MINIMUM(fileLen, MAX_LINES - 1);
	       for (fileLine=0; fileLine < fileLen ; fileLine++)
	       {
		  int len = (int)strlen (fileContents[fileLine]);
		  int y;

		  /* Init memory and clean it. */
		  viewer->info[currentLine] = (chtype *)malloc (sizeof(chtype) * (len + 3));
		  cleanChtype (viewer->info[currentLine], len + 3, '\0');

		  /* Copy from one to the other. */
		  for (y=0; y < len; y++)
		  {
		     viewer->info[currentLine][y] = fileContents[fileLine][y];
		  }
		  viewer->infoLen[currentLine]	= len;
		  viewer->infoPos[currentLine]	= 0;
		  widestLine			= MAXIMUM(widestLine, len);
		  viewer->characters		+= len;
		  currentLine++;
	       }
	       CDKfreeStrings (fileContents);
	    }
	 }
	 else
	 {
	    /* Did they ask for attribute interpretation? */
	    if (!viewer->interpret)
	    {
	       int len = (int)strlen (info[x]);
	       int y;

	       /* Init memory and clean it. */
	       viewer->info[currentLine] = (chtype *)malloc (sizeof(chtype) * (len + 3));
	       cleanChtype (viewer->info[currentLine], len + 3, '\0');

	       /* Copy from one to the other. */
	       for (y=0; y < len; y++)
	       {
		  viewer->info[currentLine][y] = info[x][y];
	       }
	       viewer->infoLen[currentLine]	= len;
	       viewer->infoPos[currentLine]	= 0;
	       widestLine			= MAXIMUM(widestLine, len);
	       viewer->characters		+= len;
	       currentLine++;
	    }
	    else
	    {
	       viewer->info[currentLine] = char2Chtype (info[x],
						&viewer->infoLen[currentLine],
						&viewer->infoPos[currentLine]);
	       viewer->infoPos[currentLine] = justifyString (viewer->boxWidth,
						viewer->infoLen[currentLine],
						viewer->infoPos[currentLine]);
	       widestLine = MAXIMUM(widestLine, viewer->infoLen[currentLine]);
	       viewer->characters += viewer->infoLen[currentLine];
	       currentLine++;
	    }
	 }
      }
   }

  /*
   * Determine how many characters we can shift to the right
   * before all the items have been viewer off the screen.
   */
   if (widestLine > viewer->boxWidth)
   {
      viewer->maxLeftChar = (widestLine - viewer->boxWidth) + 1;
   }
   else
   {
      viewer->maxLeftChar = 0;
   }

   /* Set up the needed vars for the viewer list. */
   viewer->infoSize = currentLine-1;
   if (viewer->infoSize <= viewer->viewSize)
   {
      viewer->maxTopLine = -1;
   }
   else
   {
      viewer->maxTopLine = viewer->infoSize-1;
   }
   return viewer->infoSize;
}
chtype **getCDKViewerInfo (CDKVIEWER *viewer, int *size)
{
   (*size) = viewer->infoSize;
   return viewer->info;
}

/*
 * This function sets the highlight type of the buttons.
 */
void setCDKViewerHighlight (CDKVIEWER *viewer, chtype buttonHighlight)
{
   viewer->buttonHighlight = buttonHighlight;
}
chtype getCDKViewerHighlight (CDKVIEWER *viewer)
{
   return viewer->buttonHighlight;
}

/*
 * This sets whether or not you want to set the viewer info line.
 */
void setCDKViewerInfoLine (CDKVIEWER *viewer, boolean showLineInfo)
{
   viewer->showLineInfo = showLineInfo;
}
boolean getCDKViewerInfoLine (CDKVIEWER *viewer)
{
   return viewer->showLineInfo;
}

/*
 * This sets the widgets box attribute.
 */
void setCDKViewerBox (CDKVIEWER *viewer, boolean Box)
{
   ObjOf(viewer)->box = Box;
}
boolean getCDKViewerBox (CDKVIEWER *viewer)
{
   return ObjOf(viewer)->box;
}

/*
 * This function actually controls the viewer...
 */
int activateCDKViewer (CDKVIEWER *viewer, chtype *actions GCC_UNUSED)
{
   /* Declare local variables. */
   char *fileInfo[10];
   char *tempInfo[2], temp[500];
   chtype input;
   int x, REFRESH;

   /* Create the information about the file stats. */
   sprintf (temp, "</5>      </U>File Statistics<!U>     <!5>");
   fileInfo[0] = copyChar(temp);
   sprintf (temp, "</5>                          <!5>");
   fileInfo[1] = copyChar(temp);
   sprintf (temp, "</5/R>Character Count:<!R> %-4d     <!5>", viewer->characters);
   fileInfo[2] = copyChar(temp);
   sprintf (temp, "</5/R>Line Count     :<!R> %-4d     <!5>", viewer->infoSize);
   fileInfo[3] = copyChar(temp);
   sprintf (temp, "</5>                          <!5>");
   fileInfo[4] = copyChar(temp);
   sprintf (temp, "<C></5>Press Any Key To Continue.<!5>");
   fileInfo[5] = copyChar(temp);
   fileInfo[6] = 0;

   tempInfo[0] = temp;
   tempInfo[1] = 0;

   /* Set the current button. */
   viewer->currentButton = 0;

   /* Draw the viewer list. */
   drawCDKViewer (viewer, ObjOf(viewer)->box);

   /* Do this until KEY_RETURN is hit. */
   for (;;)
   {
      /* Reset the refresh flag. */
      REFRESH = FALSE;

      /* Get the user input. */
      input = getcCDKObject (ObjOf(viewer));
      if (! checkCDKObjectBind (vVIEWER, viewer, input))
      {
	 switch (input)
	 {
	    case KEY_TAB : case CDK_NEXT :
		 if (viewer->buttonCount > 1)
		 {
		    if (viewer->currentButton == (viewer->buttonCount - 1))
		    {
		       viewer->currentButton = 0;
		    }
		    else
		    {
		       viewer->currentButton++;
		    }

		    /* Redraw the buttons. */
		    drawCDKViewerButtons (viewer);
		 }
		 break;

	    case CDK_PREV :
		 if (viewer->buttonCount > 1)
		 {
		    if (viewer->currentButton == 0)
		    {
		       viewer->currentButton = viewer->buttonCount - 1;
		    }
		    else
		    {
		       viewer->currentButton--;
		    }

		    /* Redraw the buttons. */
		    drawCDKViewerButtons (viewer);
		 }
		 break;

	    case KEY_UP :
		 if (viewer->currentTop > 0)
		 {
		    viewer->currentTop--;
		    REFRESH = TRUE;
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_DOWN :
		 if (viewer->currentTop < viewer->maxTopLine)
		 {
		    viewer->currentTop++;
		    REFRESH = TRUE;
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_RIGHT :
		 if (viewer->leftChar < viewer->maxLeftChar)
		 {
		    viewer->leftChar++;
		    REFRESH = TRUE;
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_LEFT :
		 if (viewer->leftChar > 0)
		 {
		    viewer->leftChar--;
		    REFRESH = TRUE;
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_PPAGE : case CONTROL('B') : case 'b' : case 'B' :
		 if (viewer->currentTop > 0)
		 {
		    if ((viewer->currentTop - (viewer->viewSize-1)) > 0)
		    {
		       viewer->currentTop = viewer->currentTop - (viewer->viewSize - 1);
		    }
		    else
		    {
		       viewer->currentTop = 0;
		    }
		    REFRESH = TRUE;
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_NPAGE : case CONTROL('F') : case ' ' : case 'f' : case 'F' :
		 if (viewer->currentTop < viewer->maxTopLine)
		 {
		    if ((viewer->currentTop + viewer->viewSize) < viewer->maxTopLine)
		    {
		       viewer->currentTop = viewer->currentTop + (viewer->viewSize - 1);
		    }
		    else
		    {
		       viewer->currentTop = viewer->maxTopLine;
		    }
		    REFRESH = TRUE;
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_HOME : case '|' :
		 viewer->leftChar = 0;
		 REFRESH = TRUE;
		 break;

	    case KEY_END : case '$' :
		 viewer->leftChar = viewer->maxLeftChar;
		 REFRESH = TRUE;
		 break;

	    case 'g' : case '1' :
		 viewer->currentTop = 0;
		 REFRESH = TRUE;
		 break;

	    case 'G' :
		 viewer->currentTop = viewer->maxTopLine;
		 REFRESH = TRUE;
		 break;

	    case 'L' :
		 x = (int) ((viewer->infoSize + viewer->currentTop) / 2);
		 if (x < viewer->maxTopLine)
		 {
		    viewer->currentTop = x;
		    REFRESH = TRUE;
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case 'l' :
		 x = (int) (viewer->currentTop / 2);
		 if (x >= 0)
		 {
		    viewer->currentTop = x;
		    REFRESH = TRUE;
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case '?' :
		 SearchDirection = UP;
		 getAndStorePattern (ScreenOf(viewer));
		 if (! searchForWord(viewer, SearchPattern, SearchDirection))
		 {
		    sprintf (temp, "</U/5>Pattern '%s' not found.<!U!5>", SearchPattern);
		    popUpLabel (viewer, tempInfo);
		 }
		 REFRESH = TRUE;
		 break;

	    case '/' :
		 SearchDirection = DOWN;
		 getAndStorePattern (ScreenOf(viewer));
		 if (! searchForWord(viewer, SearchPattern, SearchDirection))
		 {
		    sprintf (temp, "</U/5>Pattern '%s' not found.<!U!5>", SearchPattern);
		    popUpLabel (viewer, tempInfo);
		 }
		 REFRESH = TRUE;
		 break;

	    case 'n' :
		 if (SearchPattern == 0)
		 {
		    sprintf (temp, "</5>There is no pattern in the buffer.<!5>");
		    popUpLabel (viewer, tempInfo);
		 }
		 else
		 {
		    if (! searchForWord(viewer, SearchPattern, SearchDirection))
		    {
		       sprintf (temp, "</5>Pattern '%s' not found.<!5>", SearchPattern);
		       popUpLabel (viewer, tempInfo);
		    }
		 }
		 REFRESH = TRUE;
		 break;

	    case ':' :
		 viewer->currentTop	= jumpToLine (viewer);
		 REFRESH		= TRUE;
		 break;

	    case 'i' : case 's' : case 'S' :
		 popUpLabel(viewer, fileInfo);
		 REFRESH = TRUE;
		 break;

	    case KEY_ESC :
		 freeCharList (fileInfo, 6);
		 viewer->exitType = vESCAPE_HIT;
		 return -1;

	    case KEY_RETURN : case KEY_ENTER :
		 freeCharList (fileInfo, 6);
		 viewer->exitType = vNORMAL;
		 return viewer->currentButton;

	    case CDK_REFRESH :
		 eraseCDKScreen (ScreenOf(viewer));
		 refreshCDKScreen (ScreenOf(viewer));
		 break;

	    default :
		 break;
	 }
      }

      /* Do we need to redraw the screen??? */
      if (REFRESH)
      {
	 drawCDKViewerInfo (viewer);
      }
   }
}

/*
 * This searches the document looking for the given word.
 */
static void getAndStorePattern (CDKSCREEN *screen)
{
   /* Declare local variables. */
   CDKENTRY *getPattern = 0;
   char *temp		= 0;
   char *info		= 0;

   /* Check the direction. */
   if (SearchDirection == UP)
   {
      temp = "</5>Search Up  : <!5>";
   }
   else
   {
      temp = "</5>Search Down: <!5>";
   }

   /* Pop up the entry field. */
   getPattern	= newCDKEntry (screen, CENTER, CENTER,
				0, temp,
				COLOR_PAIR(5)|A_BOLD,
				'.'|COLOR_PAIR(5)|A_BOLD,
				vMIXED, 10, 0, 256, TRUE, FALSE);

   /* Is there an old search pattern? */
   if (SearchPattern != 0)
   {
      setCDKEntry (getPattern, SearchPattern, getPattern->min, getPattern->max, ObjOf(getPattern)->box);
   }
   freeChar (SearchPattern);

   /* Activate this baby. */
   info = activateCDKEntry (getPattern, 0);

   /* Save the info. */
   if ((info != 0) || (strlen (info) != 0))
   {
      SearchPattern = copyChar (info);
   }

   /* Clean up. */
   destroyCDKEntry (getPattern);
}

/*
 * This searches for the word and realigns the value on the screen.
 */
static int searchForWord (CDKVIEWER *viewer, char *pattern, int direction)
{
   /* Declare local variables. */
   int x, y, pos, len, plen;

   /* If the pattern is null then return. */
   if (pattern == 0)
   {
      return(0);
   }
   plen = (int)strlen(pattern);

   /* Given the direction, start looking.... */
   if (direction == DOWN)
   {
      /* Start looking from 'here' down. */
      for (x = viewer->currentTop + 1; x < viewer->infoSize; x++)
      {
	/*
	 * Start looking. If we find it, then set the value of
	 * viewer->currentTop and possibly even leftChar...
	 */
	 len	= chlen (viewer->info[x]);
	 pos	= 0;
	 for (y=0; y < len; y++)
	 {
	    /* We have to tear the attributes from the chtype. */
	    char plainChar	= viewer->info[x][y] & A_CHARTEXT;

	    /* We have found the word at this point. */
	    if (pos == plen)
	    {
	       viewer->currentTop	= (x < viewer->maxTopLine  ? x : viewer->maxTopLine);
	       viewer->leftChar = (y < viewer->boxWidth ? 0 : viewer->maxLeftChar);
	       return (1);
	    }

	    /* Keep trudging along. */
	    if (pattern[pos++] != plainChar)
	    {
	       pos	= 0;
	    }
	 }
      }
   }
   else
   {
      /* Start looking from 'here' up. */
      for (x = viewer->currentTop - 1; x >= 0; x--)
      {
	/*
	 * Start looking. If we find it, then set the value of
	 * viewer->currentTop and possibly even leftChar...
	 */
	 len	= chlen (viewer->info[x]);
	 pos	= 0;
	 for (y=0; y < len; y++)
	 {
	    /* We have to tear the attributes from the chtype. */
	    char plainChar	= viewer->info[x][y] & A_CHARTEXT;

	    /* We have found the word at this point. */
	    if (pos == plen)
	    {
	       viewer->currentTop	= x;
	       viewer->leftChar = (y < viewer->boxWidth ? 0 : viewer->maxLeftChar);
	       return (1);
	    }

	    /* Keep trudging along. */
	    if (pattern[pos++] != plainChar)
	    {
	       pos	= 0;
	    }
	 }
      }
   }
   return(0);
}

/*
 * This allows us to 'jump' to a given line in the file.
 */
static int jumpToLine (CDKVIEWER *viewer)
{
   /* Declare local variables. */
   int line		= 0;
   CDKSCALE * newline	= newCDKScale (ScreenOf(viewer), CENTER, CENTER,
				"<C>Jump To Line", "</5>Line :", A_BOLD, 5,
				0, 0, viewer->maxTopLine + 1, 1, 10, TRUE, TRUE);
   line = activateCDKScale (newline, 0);
   destroyCDKScale (newline);
   return ((line-1));
}

/*
 * This pops a little message up on the screen.
 */
static void popUpLabel (CDKVIEWER *viewer, char **mesg)
{
   /* Declare local variables. */
   CDKLABEL *label;

   /* Set up variables. */
   label	= newCDKLabel (ScreenOf(viewer), CENTER, CENTER, mesg, CDKcountStrings(mesg), TRUE, FALSE);

   /* Draw the label and wait. */
   drawCDKLabel (label, TRUE);
   getcCDKObject (ObjOf(label));

   /* Clean up. */
   destroyCDKLabel (label);
}

/*
 * This moves the viewer field to the given location.
 */
static void _moveCDKViewer (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKVIEWER *viewer = (CDKVIEWER *)object;
   /* Declare local variables. */
   int currentX = getbegx(viewer->win);
   int currentY = getbegy(viewer->win);
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
      xpos = getbegx(viewer->win) + xplace;
      ypos = getbegy(viewer->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(viewer), &xpos, &ypos, viewer->boxWidth, viewer->boxHeight, BorderOf(viewer));

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(viewer->win, -xdiff, -ydiff);
   moveCursesWindow(viewer->shadowWin, -xdiff, -ydiff);

   /* Touch the windows so they 'move'. */
   touchwin (WindowOf(viewer));
   wrefresh (WindowOf(viewer));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKViewer (viewer, ObjOf(viewer)->box);
   }
}

/*
 * This function draws the viewer widget.
 */
static void _drawCDKViewer (CDKOBJS *object, boolean Box)
{
   CDKVIEWER *viewer = (CDKVIEWER *)object;

   /* Do we need to draw in the shadow??? */
   if (viewer->shadowWin != 0)
   {
      drawShadow (viewer->shadowWin);
   }

   /* Box it if it was asked for. */
   if (Box)
   {
      attrbox (viewer->win,
		viewer->ULChar, viewer->URChar,
		viewer->LLChar, viewer->LRChar,
		viewer->HChar,	viewer->VChar,
		viewer->BoxAttrib);
      wrefresh (viewer->win);
   }

   /* Draw the info in the viewer. */
   drawCDKViewerInfo (viewer);
}

/*
 * This redraws the viewer buttons.
 */
static void drawCDKViewerButtons (CDKVIEWER *viewer)
{
   /* Declare local variables. */
   char character;
   int x;

   /* No buttons, no drawing. */
   if (viewer->buttonCount == 0)
   {
      return;
   }

   /* Redraw the buttons. */
   for (x=0; x < viewer->buttonCount; x++)
   {
      writeChtype (viewer->win, viewer->buttonPos[x],
			viewer->boxHeight-2, viewer->button[x], HORIZONTAL,
			0, viewer->buttonLen[x]);
   }

   /* Highlight the current button. */
   for (x=0; x < viewer->buttonLen[viewer->currentButton]; x++)
   {
      /* Strip the character of any extra attributes. */
      character = viewer->button[viewer->currentButton][x] & A_CHARTEXT;

      /* Add the character into the window. */
      mvwaddch (viewer->win, viewer->boxHeight-2,
		viewer->buttonPos[viewer->currentButton] + x,
		character|viewer->buttonHighlight);
   }

   /* Refresh the window. */
   touchwin (viewer->win);
   wrefresh (viewer->win);
}

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKViewerULChar (CDKVIEWER *viewer, chtype character)
{
   viewer->ULChar = character;
}
void setCDKViewerURChar (CDKVIEWER *viewer, chtype character)
{
   viewer->URChar = character;
}
void setCDKViewerLLChar (CDKVIEWER *viewer, chtype character)
{
   viewer->LLChar = character;
}
void setCDKViewerLRChar (CDKVIEWER *viewer, chtype character)
{
   viewer->LRChar = character;
}
void setCDKViewerVerticalChar (CDKVIEWER *viewer, chtype character)
{
   viewer->VChar = character;
}
void setCDKViewerHorizontalChar (CDKVIEWER *viewer, chtype character)
{
   viewer->HChar = character;
}
void setCDKViewerBoxAttribute (CDKVIEWER *viewer, chtype character)
{
   viewer->BoxAttrib = character;
}

/*
 * This sets the background color of the widget.
 */
void setCDKViewerBackgroundColor (CDKVIEWER *viewer, char *color)
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
   setCDKViewerBackgroundAttrib (viewer, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This sets the background attribute of the widget.
 */
void setCDKViewerBackgroundAttrib (CDKVIEWER *viewer, chtype attrib)
{
   /* Set the widgets background attribute. */
   wbkgd (viewer->win, attrib);
}

/*
 * This function destroys the viewer widget.
 */
static void _destroyCDKViewer (CDKOBJS *object)
{
   CDKVIEWER *viewer = (CDKVIEWER *)object;
   int x;

   /* Clear up the char pointers. */
   for (x=0; x < viewer->titleLines; x++)
   {
      freeChtype (viewer->title[x]);
   }
   for (x=0; x <= viewer->infoSize; x++)
   {
      freeChtype (viewer->info[x]);
   }
   for (x=0; x < viewer->buttonCount; x++)
   {
      freeChtype (viewer->button[x]);
   }

   /* Clean up the windows. */
   deleteCursesWindow (viewer->shadowWin);
   deleteCursesWindow (viewer->win);

   /* Unregister this object. */
   unregisterCDKObject (vVIEWER, viewer);
}

/*
 * This function erases the viewer widget from the screen.
 */
static void _eraseCDKViewer (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKVIEWER *viewer = (CDKVIEWER *)object;

      eraseCursesWindow (viewer->win);
      eraseCursesWindow (viewer->shadowWin);
   }
}

/*
 * This draws the viewer info lines.
 */
static void drawCDKViewerInfo (CDKVIEWER *viewer)
{
   int infoAdjust	= 0;
   int lastLine		= 0;
   char temp[256];
   int x;

   /* Clear the window. */
   werase (viewer->win);

   /* Redraw the title. */
   if (viewer->titleLines != 0)
   {
      for (x=0; x < viewer->titleLines; x++)
      {
	 writeChtype (viewer->win,
			viewer->titlePos[x],
			x + 1,
			viewer->title[x],
			HORIZONTAL, 0,
			viewer->titleLen[x]);
      }
   }

   /* Draw in the current line at the top. */
   if (viewer->showLineInfo == TRUE)
   {
      /* Set up the info line and draw it. */
      if (viewer->infoSize != 0)
      {
	 sprintf (temp, "%d/%d %2.0f%%",
			(viewer->currentTop + 1),
			viewer->infoSize,
			((float)(viewer->currentTop + 1)/(float)viewer->infoSize) * 100);
      }
      else
      {
	 sprintf (temp, "%d/%d %2.0f%%", 0, 0, 0.0);
      }

     /*
      * The infoAdjust variable tells us if we have to shift down
      * one line because the person asked for the line X of Y line
      * at the top of the screen. We only want to set this to 1 if
      * they asked for the info line and there is no title, or if the
      * two items overlap.
      */
      if (viewer->titleLines == 0
       || viewer->titlePos[0] < ((int)strlen(temp) + 2))
      {
	 infoAdjust = 1;
      }
      writeChar (viewer->win, 1, (infoAdjust ? viewer->titleLines : 0) + 1,
		 temp, HORIZONTAL, 0, (int)strlen(temp));
   }

   /* Determine the last line to draw. */
   lastLine = (viewer->infoSize <= viewer->viewSize ? viewer->infoSize : viewer->viewSize);
   lastLine -= infoAdjust;

   /* Redraw the list. */
   for (x=0; x < lastLine; x++)
   {
      if (viewer->currentTop + x < viewer->infoSize)
      {
	 int screenPos = viewer->infoPos[viewer->currentTop + x] + 1 - viewer->leftChar;
	 if (screenPos >= 0)
	 {
	    writeChtype (viewer->win, screenPos,
				x + viewer->titleLines + infoAdjust + 1,
				viewer->info[x + viewer->currentTop],
				HORIZONTAL, 0,
				viewer->infoLen[x + viewer->currentTop]);
	 }
	 else
	 {
	    writeChtype (viewer->win, 1,
				x + viewer->titleLines + infoAdjust + 1,
				viewer->info[x + viewer->currentTop],
				HORIZONTAL,
				viewer->leftChar - viewer->infoPos[viewer->currentTop + x],
				viewer->infoLen[x + viewer->currentTop]);
	 }
      }
   }

   /* Box it if we have to. */
   if (ObjOf(viewer)->box)
   {
      attrbox (viewer->win,
		viewer->ULChar, viewer->URChar,
		viewer->LLChar, viewer->LRChar,
		viewer->HChar,	viewer->VChar,
		viewer->BoxAttrib);
      wrefresh (viewer->win);
   }

   /* Draw the separation line. */
   if (viewer->buttonCount > 0)
   {
      for (x=1; x <= viewer->boxWidth; x++)
      {
	 mvwaddch (viewer->win, viewer->boxHeight-3, x,
			viewer->HChar | viewer->BoxAttrib);
      }
      mvwaddch (viewer->win, viewer->boxHeight-3, 0,
			ACS_LTEE | viewer->BoxAttrib);
      mvwaddch (viewer->win, viewer->boxHeight-3,
			getmaxx(viewer->win) - 1,
			ACS_RTEE | viewer->BoxAttrib);
   }

   /* Draw the buttons. This will call refresh on the viewer win. */
   drawCDKViewerButtons (viewer);
}

static int _injectCDKViewer(struct CDKOBJS *object GCC_UNUSED, chtype key GCC_UNUSED)
{
   return 0;
}

static void _focusCDKViewer(CDKOBJS *object GCC_UNUSED)
{
   /* FIXME */
}

static void _unfocusCDKViewer(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _refreshDataCDKViewer(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _saveDataCDKViewer(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}
