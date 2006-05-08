#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2006/05/05 00:27:44 $
 * $Revision: 1.156 $
 */

/*
 * Declare some local definitions.
 */
#define		DOWN		0
#define		UP		1

/*
 * Declare file local prototypes.
 */
static int createList (CDKVIEWER *swindow, int listSize);
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
static int	SearchDirection = DOWN;

DeclareCDKObjects (VIEWER, Viewer, setCdk, Unknown);

/*
 * This function creates a new viewer object.
 */
CDKVIEWER *newCDKViewer (CDKSCREEN *cdkscreen,
			 int xplace,
			 int yplace,
			 int height,
			 int width,
			 char **buttons,
			 int buttonCount,
			 chtype buttonHighlight,
			 boolean Box,
			 boolean shadow)
{
   CDKVIEWER *viewer	= 0;
   int parentWidth	= getmaxx (cdkscreen->window);
   int parentHeight	= getmaxy (cdkscreen->window);
   int boxWidth		= width;
   int boxHeight	= height;
   int xpos		= xplace;
   int ypos		= yplace;
   int buttonWidth	= 0;
   int buttonAdj	= 0;
   int buttonPos	= 1;
   int x		= 0;

   /* *INDENT-OFF* */
   static const struct { int from; int to; } bindings[] = {
	    { CDK_BACKCHAR,	KEY_PPAGE },
	    { 'b',		KEY_PPAGE },
	    { 'B',		KEY_PPAGE },
	    { CDK_FORCHAR,	KEY_NPAGE },
	    { SPACE,		KEY_NPAGE },
	    { 'f',		KEY_NPAGE },
	    { 'F',		KEY_NPAGE },
	    { '|',		KEY_HOME },
	    { '$',		KEY_END },
   };
   /* *INDENT-ON* */

   if ((viewer = newCDKObject (CDKVIEWER, &my_funcs)) == 0)
      return (0);

   setCDKViewerBox (viewer, Box);

   boxHeight = setWidgetDimension (parentHeight, height, 0);
   boxWidth = setWidgetDimension (parentWidth, width, 0);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the viewer window. */
   viewer->win = newwin (boxHeight, boxWidth, ypos, xpos);
   if (viewer->win == 0)
   {
      destroyCDKObject (viewer);
      return (0);
   }

   /* Turn the keypad on for the viewer. */
   keypad (viewer->win, TRUE);

   /* Create the buttons. */
   viewer->buttonCount = buttonCount;
   if (buttonCount > 0)
   {
      if ((viewer->button    = typeCallocN (chtype *, buttonCount + 1)) == 0
       || (viewer->buttonLen = typeCallocN (int,      buttonCount + 1)) == 0
       || (viewer->buttonPos = typeCallocN (int,      buttonCount + 1)) == 0)
      {
	 destroyCDKObject (viewer);
	 return (0);
      }
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
   ScreenOf (viewer)		= cdkscreen;
   viewer->parent		= cdkscreen->window;
   viewer->shadowWin		= 0;
   viewer->buttonHighlight	= buttonHighlight;
   viewer->boxHeight		= boxHeight;
   viewer->boxWidth		= boxWidth - 2;
   viewer->viewSize		= height - 2;
   ObjOf (viewer)->inputWindow	= viewer->win;
   initExitType (viewer);
   viewer->shadow		= shadow;
   viewer->currentButton	= 0;
   viewer->currentTop		= 0;
   viewer->length		= 0;
   viewer->leftChar		= 0;
   viewer->maxLeftChar		= 0;
   viewer->maxTopLine		= 0;
   viewer->characters		= 0;
   viewer->listSize		= -1;
   viewer->showLineInfo		= 1;
   viewer->exitType		= vEARLY_EXIT;

   /* Do we need to create a shadow??? */
   if (shadow)
   {
      viewer->shadowWin = newwin (boxHeight, boxWidth + 1, ypos + 1, xpos + 1);
      if (viewer->shadowWin == 0)
      {
	 destroyCDKObject (viewer);
	 return (0);
      }
   }

   /* Setup the key bindings. */
   for (x = 0; x < (int)SIZEOF (bindings); ++x)
      bindCDKObject (vVIEWER, viewer, bindings[x].from, getcCDKBind, (void *)(long)bindings[x].to);

   registerCDKObject (cdkscreen, vVIEWER, viewer);

   return (viewer);
}

/*
 * This function sets various attributes of the widget.
 */
int setCDKViewer (CDKVIEWER *viewer,
		  char *title,
		  char **list,
		  int listSize,
		  chtype buttonHighlight,
		  boolean attrInterp,
		  boolean showLineInfo,
		  boolean Box)
{
   setCDKViewerTitle (viewer, title);
   setCDKViewerHighlight (viewer, buttonHighlight);
   setCDKViewerInfoLine (viewer, showLineInfo);
   setCDKViewerBox (viewer, Box);
   return setCDKViewerInfo (viewer, list, listSize, attrInterp);
}

/*
 * This sets the title of the viewer. (A null title is allowed.
 * It just means that the viewer will not have a title when drawn.)
 */
void setCDKViewerTitle (CDKVIEWER *viewer, char *title)
{
   (void) setCdkTitle (ObjOf (viewer), title, - (viewer->boxWidth + 1));
   viewer->titleAdj = TitleLinesOf (viewer);

   /* Need to set viewer->viewSize. */
   viewer->viewSize = viewer->boxHeight - (TitleLinesOf (viewer) + 1) - 2;
}
chtype **getCDKViewerTitle (CDKVIEWER *viewer)
{
   return TitleOf (viewer);
}

static void setupLine (CDKVIEWER *viewer, boolean interpret, char *list, int x)
{
   /* Did they ask for attribute interpretation? */
   if (interpret)
   {
      viewer->list[x]    = char2Chtype (list, &viewer->listLen[x], &viewer->listPos[x]);
      viewer->listPos[x] = justifyString (viewer->boxWidth,
					  viewer->listLen[x],
					  viewer->listPos[x]);
   }
   else
   {
      int len = (int)strlen (list);
      int pass;
      int y;
      chtype *t = 0;

      /*
       * We must convert tabs and other nonprinting characters.  The curses
       * library normally does this, but we are bypassing it by writing
       * chtype's directly.
       */
      for (pass = 0; pass < 2; ++pass)
      {
	 len = 0;
	 for (y = 0; list[y] != '\0'; ++y)
	 {
	    if (list[y] == '\t')
	    {
	       do
	       {
		  if (pass)
		     t[len] = ' ';
		  ++len;
	       }
	       while (len & 7);
	    }
	    else if (isprint (CharOf (list[y])))
	    {
	       if (pass)
		  t[len] = CharOf (list[y]);
	       ++len;
	    }
	    else
	    {
	       const char *s = unctrl (list[y]);
	       while (*s != 0)
	       {
		  if (pass)
		     t[len] = CharOf (*s);
		  ++len;
		  ++s;
	       }
	    }
	 }
	 if (!pass)
	 {
	    viewer->list[x] = t = typeCallocN (chtype, len + 3);
	    if (t == 0)
	    {
	       len = 0;
	       break;
	    }
	 }
      }
      viewer->listLen[x] = len;
      viewer->listPos[x] = 0;
   }
   viewer->widestLine = MAXIMUM (viewer->widestLine, viewer->listLen[x]);
}

static void freeLine (CDKVIEWER *viewer, int x)
{
   if (x < viewer->listSize)
   {
      freeChtype (viewer->list[x]);
      viewer->list[x] = 0;
   }
}

/*
 * This function sets the contents of the viewer.
 */
int setCDKViewerInfo (CDKVIEWER *viewer, char **list, int listSize, boolean interpret)
{
   char filename[CDK_PATHMAX + 2];
   int currentLine	= 0;
   int x		= 0;
   int viewerSize	= listSize;

   /*
    * If the list-size is negative, count the length of the null-terminated
    * list of strings.
    */
   if (listSize < 0)
   {
      listSize = CDKcountStrings (list);
   }

   /* compute the size of the resulting display */
   viewerSize = listSize;
   if (list != 0 && interpret)
   {
      for (x = 0; x < listSize; ++x)
      {
	 if (list[x] == 0)
	 {
	    viewerSize = x;	/* oops - caller gave the wrong length */
	    break;
	 }
	 if (checkForLink (list[x], filename) == 1)
	 {
	    char **fileContents = 0;
	    int fileLen = CDKreadFile (filename, &fileContents);

	    if (fileLen >= 0)
	       viewerSize += (fileLen - 1);
	    CDKfreeStrings (fileContents);
	 }
      }
   }

   /* Clean out the old viewer info. (if there is any) */
   viewer->inProgress = TRUE;
   cleanCDKViewer (viewer);
   createList (viewer, viewerSize);

   /* Keep some semi-permanent info. */
   viewer->interpret = interpret;

   /* Copy the information given. */
   for (x = currentLine = 0; x < listSize && currentLine < viewerSize; x++)
   {
      if (list[x] == 0)
      {
	 viewer->list[currentLine]	= 0;
	 viewer->listLen[currentLine]	= 0;
	 viewer->listPos[currentLine]	= 0;
	 currentLine++;
      }
      else
      {
	 /* Check if we have a file link in this line. */
	 if (checkForLink (list[x], filename) == 1)
	 {
	    /* We have a link, open the file. */
	    char **fileContents = 0;
	    int fileLen		= 0;
	    int fileLine	= 0;

	    /* Open the file and put it into the viewer. */
	    fileLen = CDKreadFile (filename, &fileContents);
	    if (fileLen == -1)
	    {
#ifdef HAVE_START_COLOR
#define FOPEN_FMT "<C></16>Link Failed: Could not open the file %s"
#else
#define FOPEN_FMT "<C></K>Link Failed: Could not open the file %s"
#endif
	       char *temp = (char *)malloc (80 + strlen (filename));
	       sprintf (temp, FOPEN_FMT, filename);
	       setupLine (viewer, TRUE, temp, currentLine++);
	       free (temp);
	    }
	    else
	    {
	       /* For each line read, copy it into the viewer. */
	       fileLen = MINIMUM (fileLen, (viewerSize - currentLine));
	       for (fileLine = 0; fileLine < fileLen; fileLine++)
	       {
		  if (currentLine >= viewerSize)
		     break;
		  setupLine (viewer, FALSE, fileContents[fileLine], currentLine);
		  viewer->characters += viewer->listLen[currentLine];
		  currentLine++;
	       }
	       CDKfreeStrings (fileContents);
	    }
	 }
	 else if (currentLine < viewerSize)
	 {
	    setupLine (viewer, viewer->interpret, list[x], currentLine);
	    viewer->characters += viewer->listLen[currentLine];
	    currentLine++;
	 }
      }
   }

   /*
    * Determine how many characters we can shift to the right
    * before all the items have been viewer off the screen.
    */
   if (viewer->widestLine > viewer->boxWidth)
   {
      viewer->maxLeftChar = (viewer->widestLine - viewer->boxWidth) + 1;
   }
   else
   {
      viewer->maxLeftChar = 0;
   }

   /* Set up the needed vars for the viewer list. */
   viewer->inProgress = FALSE;
   viewer->listSize = viewerSize;
   if (viewer->listSize <= viewer->viewSize)
   {
      viewer->maxTopLine = 0;
   }
   else
   {
      viewer->maxTopLine = viewer->listSize - 1;
   }
   return viewer->listSize;
}
chtype **getCDKViewerInfo (CDKVIEWER *viewer, int *size)
{
   (*size) = viewer->listSize;
   return viewer->list;
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
   ObjOf (viewer)->box = Box;
   ObjOf (viewer)->borderSize = Box ? 1 : 0;
}
boolean getCDKViewerBox (CDKVIEWER *viewer)
{
   return ObjOf (viewer)->box;
}

/*
 * This removes all the lines inside the scrolling window.
 */
void cleanCDKViewer (CDKVIEWER *viewer)
{
   int x;

   /* Clean up the memory used ... */
   for (x = 0; x < viewer->listSize; x++)
   {
      freeLine (viewer, x);
   }

   /* Reset some variables. */
   viewer->listSize	= 0;
   viewer->maxLeftChar  = 0;
   viewer->widestLine	= 0;
   viewer->currentTop	= 0;
   viewer->maxTopLine	= 0;

   /* Redraw the window. */
   drawCDKViewer (viewer, ObjOf (viewer)->box);
}

static void PatternNotFound (CDKVIEWER *viewer, char *pattern)
{
   char *tempInfo[2];
   char *temp = (char *)malloc (80 + strlen (pattern));
   tempInfo[0] = temp;
   tempInfo[1] = 0;
   sprintf (temp, "</U/5>Pattern '%s' not found.<!U!5>", pattern);
   popUpLabel (viewer, tempInfo);
   free (temp);
}

/*
 * This function actually controls the viewer...
 */
int activateCDKViewer (CDKVIEWER *viewer, chtype *actions GCC_UNUSED)
{
   char *fileInfo[10];
   char *tempInfo[2], temp[500];
   chtype input;
   boolean functionKey;
   int x, REFRESH;

   /* Create the information about the file stats. */
   sprintf (temp, "</5>      </U>File Statistics<!U>     <!5>");
   fileInfo[0] = copyChar (temp);
   sprintf (temp, "</5>                          <!5>");
   fileInfo[1] = copyChar (temp);
   sprintf (temp, "</5/R>Character Count:<!R> %-4ld     <!5>", viewer->characters);
   fileInfo[2] = copyChar (temp);
   sprintf (temp, "</5/R>Line Count     :<!R> %-4d     <!5>", viewer->listSize);
   fileInfo[3] = copyChar (temp);
   sprintf (temp, "</5>                          <!5>");
   fileInfo[4] = copyChar (temp);
   sprintf (temp, "<C></5>Press Any Key To Continue.<!5>");
   fileInfo[5] = copyChar (temp);
   fileInfo[6] = 0;

   tempInfo[0] = temp;
   tempInfo[1] = 0;

   /* Set the current button. */
   viewer->currentButton = 0;

   /* Draw the viewer list. */
   drawCDKViewer (viewer, ObjOf (viewer)->box);

   /* Do this until KEY_ENTER is hit. */
   for (;;)
   {
      /* Reset the refresh flag. */
      REFRESH = FALSE;

      input = getchCDKObject (ObjOf (viewer), &functionKey);
      if (!checkCDKObjectBind (vVIEWER, viewer, input))
      {
	 switch (input)
	 {
	 case KEY_TAB:
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

	 case CDK_PREV:
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

	 case KEY_UP:
	    if (viewer->currentTop > 0)
	    {
	       viewer->currentTop--;
	       REFRESH = TRUE;
	    }
	    else
	    {
	       Beep ();
	    }
	    break;

	 case KEY_DOWN:
	    if (viewer->currentTop < viewer->maxTopLine)
	    {
	       viewer->currentTop++;
	       REFRESH = TRUE;
	    }
	    else
	    {
	       Beep ();
	    }
	    break;

	 case KEY_RIGHT:
	    if (viewer->leftChar < viewer->maxLeftChar)
	    {
	       viewer->leftChar++;
	       REFRESH = TRUE;
	    }
	    else
	    {
	       Beep ();
	    }
	    break;

	 case KEY_LEFT:
	    if (viewer->leftChar > 0)
	    {
	       viewer->leftChar--;
	       REFRESH = TRUE;
	    }
	    else
	    {
	       Beep ();
	    }
	    break;

	 case KEY_PPAGE:
	    if (viewer->currentTop > 0)
	    {
	       if ((viewer->currentTop - (viewer->viewSize-1)) > 0)
	       {
		  viewer->currentTop = (viewer->currentTop
					- (viewer->viewSize - 1));
	       }
	       else
	       {
		  viewer->currentTop = 0;
	       }
	       REFRESH = TRUE;
	    }
	    else
	    {
	       Beep ();
	    }
	    break;

	 case KEY_NPAGE:
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
	       Beep ();
	    }
	    break;

	 case KEY_HOME:
	    viewer->leftChar = 0;
	    REFRESH = TRUE;
	    break;

	 case KEY_END:
	    viewer->leftChar = viewer->maxLeftChar;
	    REFRESH = TRUE;
	    break;

	 case 'g':
	 case '1':
	 case '<':
	    viewer->currentTop = 0;
	    REFRESH = TRUE;
	    break;

	 case 'G':
	 case '>':
	    viewer->currentTop = viewer->maxTopLine;
	    REFRESH = TRUE;
	    break;

	 case 'L':
	    x = (int)((viewer->listSize + viewer->currentTop) / 2);
	    if (x < viewer->maxTopLine)
	    {
	       viewer->currentTop = x;
	       REFRESH = TRUE;
	    }
	    else
	    {
	       Beep ();
	    }
	    break;

	 case 'l':
	    x = (int)(viewer->currentTop / 2);
	    if (x >= 0)
	    {
	       viewer->currentTop = x;
	       REFRESH = TRUE;
	    }
	    else
	    {
	       Beep ();
	    }
	    break;

	 case '?':
	    SearchDirection = UP;
	    getAndStorePattern (ScreenOf (viewer));
	    if (!searchForWord (viewer, SearchPattern, SearchDirection))
	    {
	       PatternNotFound (viewer, SearchPattern);
	    }
	    REFRESH = TRUE;
	    break;

	 case '/':
	    SearchDirection = DOWN;
	    getAndStorePattern (ScreenOf (viewer));
	    if (! searchForWord (viewer, SearchPattern, SearchDirection))
	    {
	       PatternNotFound (viewer, SearchPattern);
	    }
	    REFRESH = TRUE;
	    break;

	 case 'N':
	 case 'n':
	    if (SearchPattern == 0)
	    {
	       sprintf (temp, "</5>There is no pattern in the buffer.<!5>");
	       popUpLabel (viewer, tempInfo);
	    }
	    else if (!searchForWord (viewer,
				     SearchPattern,
				     ((input == 'n')
				      ? SearchDirection
				      : !SearchDirection)))
	    {
	       PatternNotFound (viewer, SearchPattern);
	    }
	    REFRESH = TRUE;
	    break;

	 case ':':
	    viewer->currentTop = jumpToLine (viewer);
	    REFRESH = TRUE;
	    break;

	 case 'i':
	 case 's':
	 case 'S':
	    popUpLabel (viewer, fileInfo);
	    REFRESH = TRUE;
	    break;

	 case KEY_ESC:
	    freeCharList (fileInfo, 6);
	    setExitType (viewer, input);
	    return -1;

	 case KEY_ENTER:
	    freeCharList (fileInfo, 6);
	    setExitType (viewer, input);
	    return viewer->currentButton;

	 case CDK_REFRESH:
	    eraseCDKScreen (ScreenOf (viewer));
	    refreshCDKScreen (ScreenOf (viewer));
	    break;

	 default:
	    Beep ();
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
   CDKENTRY *getPattern = 0;
   char *temp		= 0;
   char *list		= 0;

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
				COLOR_PAIR (5)|A_BOLD,
				'.'|COLOR_PAIR (5)|A_BOLD,
				vMIXED, 10, 0, 256, TRUE, FALSE);

   /* Is there an old search pattern? */
   if (SearchPattern != 0)
   {
      setCDKEntry (getPattern, SearchPattern, getPattern->min, getPattern->max, ObjOf (getPattern)->box);
   }
   freeChar (SearchPattern);

   /* Activate this baby. */
   list = activateCDKEntry (getPattern, 0);

   /* Save the list. */
   if ((list != 0) || (strlen (list) != 0))
   {
      SearchPattern = copyChar (list);
   }

   /* Clean up. */
   destroyCDKEntry (getPattern);
}

/*
 * This searches for a line containing the word and realigns the value on the
 * screen.
 */
static int searchForWord (CDKVIEWER *viewer, char *pattern, int direction)
{
   int x, y, pos, len, plen;
   int found = 0;

   /* If the pattern is empty then return. */
   if (pattern != 0 && (plen = strlen (pattern)) != 0)
   {
      if (direction == DOWN)
      {
	 /* Start looking from 'here' down. */
	 for (x = viewer->currentTop + 1; !found && (x < viewer->listSize); x++)
	 {
	    len = chlen (viewer->list[x]);
	    for (y = pos = 0; y < len; y++)
	    {
	       int plainChar = CharOf (viewer->list[x][y]);

	       if (CharOf (pattern[pos]) != plainChar)
	       {
		  y -= pos;
		  pos = 0;
	       }
	       else if (++pos == plen)
	       {
		  viewer->currentTop = (x < viewer->maxTopLine ? x : viewer->maxTopLine);
		  viewer->leftChar = (y < viewer->boxWidth ? 0 : viewer->maxLeftChar);
		  found = 1;
		  break;
	       }

	    }
	 }
      }
      else
      {
	 /* Start looking from 'here' up. */
	 for (x = viewer->currentTop - 1; !found && (x >= 0); x--)
	 {
	    len = chlen (viewer->list[x]);
	    for (y = pos = 0; y < len; y++)
	    {
	       int plainChar = CharOf (viewer->list[x][y]);

	       if (CharOf (pattern[pos]) != plainChar)
	       {
		  y -= pos;
		  pos = 0;
	       }
	       else if (++pos == plen)
	       {
		  viewer->currentTop = x;
		  viewer->leftChar = (y < viewer->boxWidth ? 0 : viewer->maxLeftChar);
		  found = 1;
		  break;
	       }

	    }
	 }
      }
   }
   return (found);
}

/*
 * This allows us to 'jump' to a given line in the file.
 */
static int jumpToLine (CDKVIEWER *viewer)
{
   int line		= 0;
   CDKSCALE * newline	= newCDKScale (ScreenOf (viewer), CENTER, CENTER,
				"<C>Jump To Line", "</5>Line :", A_BOLD,
				intlen (viewer->listSize) + 1,
				viewer->currentTop + 1,
				0, viewer->maxTopLine + 1,
				1, 10, TRUE, TRUE);
   line = activateCDKScale (newline, 0);
   destroyCDKScale (newline);
   return ((line-1));
}

/*
 * This pops a little message up on the screen.
 */
static void popUpLabel (CDKVIEWER *viewer, char **mesg)
{
   CDKLABEL *label;
   boolean functionKey;

   /* Set up variables. */
   label = newCDKLabel (ScreenOf (viewer), CENTER, CENTER, mesg, CDKcountStrings (mesg), TRUE, FALSE);

   /* Draw the label and wait. */
   drawCDKLabel (label, TRUE);
   getchCDKObject (ObjOf (label), &functionKey);

   /* Clean up. */
   destroyCDKLabel (label);
}

/*
 * This moves the viewer field to the given location.
 */
static void _moveCDKViewer (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKVIEWER *viewer = (CDKVIEWER *)object;
   int currentX = getbegx (viewer->win);
   int currentY = getbegy (viewer->win);
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
      xpos = getbegx (viewer->win) + xplace;
      ypos = getbegy (viewer->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf (viewer), &xpos, &ypos, viewer->boxWidth, viewer->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow (viewer->win, -xdiff, -ydiff);
   moveCursesWindow (viewer->shadowWin, -xdiff, -ydiff);

   /* Touch the windows so they 'move'. */
   refreshCDKWindow (WindowOf (viewer));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKViewer (viewer, ObjOf (viewer)->box);
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
      drawObjBox (viewer->win, ObjOf (viewer));
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
   chtype character;
   int x;

   /* No buttons, no drawing. */
   if (viewer->buttonCount == 0)
   {
      return;
   }

   /* Redraw the buttons. */
   for (x = 0; x < viewer->buttonCount; x++)
   {
      writeChtype (viewer->win,
		   viewer->buttonPos[x],
		   viewer->boxHeight - 2,
		   viewer->button[x],
		   HORIZONTAL,
		   0,
		   viewer->buttonLen[x]);
   }

   /* Highlight the current button. */
   for (x=0; x < viewer->buttonLen[viewer->currentButton]; x++)
   {
      /* Strip the character of any extra attributes. */
      character = CharOf (viewer->button[viewer->currentButton][x]);

      /* Add the character into the window. */
      mvwaddch (viewer->win,
		viewer->boxHeight - 2,
		viewer->buttonPos[viewer->currentButton] + x,
		character | viewer->buttonHighlight);
   }

   /* Refresh the window. */
   wrefresh (viewer->win);
}

/*
 * This sets the background attribute of the widget.
 */
static void _setBKattrViewer (CDKOBJS *object, chtype attrib)
{
   if (object != 0)
   {
      CDKVIEWER *widget = (CDKVIEWER *)object;

      wbkgd (widget->win, attrib);
   }
}

/*
 * Free any storage associated with the info-list.
 */
static void destroyInfo (CDKVIEWER *viewer)
{
   CDKfreeChtypes (viewer->list);
   freeChecked (viewer->listPos);
   freeChecked (viewer->listLen);

   viewer->list = 0;
   viewer->listPos = 0;
   viewer->listLen = 0;
}

/*
 * This function destroys the viewer widget.
 */
static void _destroyCDKViewer (CDKOBJS *object)
{
   if (object != 0)
   {
      CDKVIEWER *viewer = (CDKVIEWER *)object;

      destroyInfo (viewer);

      cleanCdkTitle (object);
      CDKfreeChtypes (viewer->button);
      freeChecked (viewer->buttonLen);
      freeChecked (viewer->buttonPos);

      /* Clean up the windows. */
      deleteCursesWindow (viewer->shadowWin);
      deleteCursesWindow (viewer->win);

      /* Clean the key bindings. */
      cleanCDKObjectBindings (vVIEWER, viewer);

      /* Unregister this object. */
      unregisterCDKObject (vVIEWER, viewer);
   }
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
   int listAdjust	= 0;
   int lastLine		= 0;
   char temp[256];
   int x;

   /* Clear the window. */
   werase (viewer->win);

   drawCdkTitle (viewer->win, ObjOf (viewer));

   /* Draw in the current line at the top. */
   if (viewer->showLineInfo == TRUE)
   {
      /* Set up the info line and draw it. */
      if (viewer->inProgress)
      {
	 strcpy (temp, "processing...");
      }
      else if (viewer->listSize != 0)
      {
	 sprintf (temp, "%d/%d %2.0f%%",
		  (viewer->currentTop + 1),
		  viewer->listSize,
		  ((float)(viewer->currentTop + 1)
		   / (float)viewer->listSize) * 100);
      }
      else
      {
	 sprintf (temp, "%d/%d %2.0f%%", 0, 0, 0.0);
      }

      /*
       * The listAdjust variable tells us if we have to shift down one line
       * because the person asked for the line X of Y line at the top of the
       * screen.  We only want to set this to 1 if they asked for the info line
       * and there is no title, or if the two items overlap.
       */
      if (TitleLinesOf (viewer) == 0
	  || TitlePosOf (viewer)[0] < ((int)strlen (temp) + 2))
      {
	 listAdjust = 1;
      }
      writeChar (viewer->win, 1, (listAdjust ? TitleLinesOf (viewer) : 0) + 1,
		 temp, HORIZONTAL, 0, (int)strlen (temp));
   }

   /* Determine the last line to draw. */
   lastLine = ((viewer->listSize <= viewer->viewSize)
	       ? viewer->listSize
	       : viewer->viewSize);
   lastLine -= listAdjust;

   /* Redraw the list. */
   for (x = 0; x < lastLine; x++)
   {
      if (viewer->currentTop + x < viewer->listSize)
      {
	 int screenPos = viewer->listPos[viewer->currentTop + x] + 1 - viewer->leftChar;

	 writeChtype (viewer->win,
		      ((screenPos >= 0)
		       ? screenPos
		       : 1),
		      x + TitleLinesOf (viewer) + listAdjust + 1,
		      viewer->list[x + viewer->currentTop],
		      HORIZONTAL,
		      ((screenPos >= 0)
		       ? 0
		       : (viewer->leftChar
			   - viewer->listPos[viewer->currentTop + x])),
		      viewer->listLen[x + viewer->currentTop]);
      }
   }

   /* Box it if we have to. */
   if (ObjOf (viewer)->box)
   {
      drawObjBox (viewer->win, ObjOf (viewer));
      wrefresh (viewer->win);
   }

   /* Draw the separation line. */
   if (viewer->buttonCount > 0)
   {
      chtype boxattr = BXAttrOf (viewer);

      for (x=1; x <= viewer->boxWidth; x++)
      {
	 mvwaddch (viewer->win, viewer->boxHeight-3, x,
			HZCharOf (viewer) | boxattr);
      }
      mvwaddch (viewer->win, viewer->boxHeight-3, 0,
			ACS_LTEE | boxattr);
      mvwaddch (viewer->win, viewer->boxHeight-3,
			getmaxx (viewer->win) - 1,
			ACS_RTEE | boxattr);
   }

   /* Draw the buttons. This will call refresh on the viewer win. */
   drawCDKViewerButtons (viewer);
}

/*
 * The listSize may be negative, to assign no definite limit.
 */
static int createList (CDKVIEWER *swindow, int listSize)
{
   int status = 0;
   if (listSize <= 0)
   {
      destroyInfo (swindow);
   }
   else
   {
      chtype **newList = typeCallocN (chtype *, listSize + 1);
      int *newPos = typeCallocN (int, listSize + 1);
      int *newLen = typeCallocN (int, listSize + 1);

      if (newList != 0
	  && newPos != 0
	  && newLen != 0)
      {
	 status = 1;
	 destroyInfo (swindow);

	 swindow->list    = newList;
	 swindow->listPos = newPos;
	 swindow->listLen = newLen;
      }
      if (!status)
      {
	 CDKfreeChtypes (newList);
	 freeChecked (newPos);
	 freeChecked (newLen);
      }
   }
   return status;
}

dummyInject (Viewer)

dummyFocus (Viewer)

dummyUnfocus (Viewer)

dummyRefreshData (Viewer)

dummySaveData (Viewer)
