/* $Id: cdkmatrix.c,v 1.5 2001/04/20 22:51:24 tom Exp $ */

#include <cdk.h>

#ifdef XCURSES
char *XCursesProgramName="cdkmatrix";
#endif

/*
 * Declare file local prototypes.
 */
int widgetCB (EObjectType cdktype, void *object, void *clientData, chtype key);

/*
 * Define file local variables.
 */
char *FPUsage = "-r Row Titles -c Column Titles -v Visible Rows -w Column Widths [-t Column Types] [-d Default Values] [-F Field Character] [-T Title] [-B Buttons] [-O Output File] [-X X Position] [-Y Y Position] [-N] [-S]";

/*
 *
 */
int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkScreen		= 0;
   CDKMATRIX *widget		= 0;
   CDKBUTTONBOX *buttonWidget	= 0;
   WINDOW *cursesWindow		= 0;
   chtype *holder		= 0;
   char *title			= 0;
   char *outputFile		= 0;
   char *tempRowTitles		= 0;
   char *tempColTitles		= 0;
   char *tempColWidths		= 0;
   char *tempColTypes		= 0;
   char *defaultValueFile	= 0;
   char *tempFiller		= 0;
   char *buttons		= 0;
   char *CDK_WIDGET_COLOR	= 0;
   char *temp			= 0;
   chtype filler		= A_NORMAL | '.';
   int xpos			= CENTER;
   int ypos			= CENTER;
   int rows			= -1;
   int cols			= -1;
   int vrows			= -1;
   int buttonCount		= 0;
   int selection		= 0;
   int shadowHeight		= 0;
   boolean boxWidget		= TRUE;
   boolean shadowWidget		= FALSE;
   FILE *fp			= stderr;
   char *info[MAX_MATRIX_ROWS][MAX_MATRIX_COLS];
   char *rowTitles[MAX_MATRIX_ROWS];
   char *colTitles[MAX_MATRIX_COLS];
   char **rowTemp		= 0;
   char **colTemp		= 0;
   char **buttonList		= 0;
   int subSize[MAX_MATRIX_COLS];
   int colWidths[MAX_MATRIX_COLS];
   int colTypes[MAX_MATRIX_COLS];
   int count, infoLines, ret, x, y, j1, j2;

   /* Parse up the command line. */
   while (1)
   {
      /* If there aren't any more options, then break. */
      if ((ret = getopt (argc, argv, "d:r:c:w:v:t:F:T:B:O:X:Y:NS")) == -1)
      {
         break;
      }

      /* Determine which command line option we just received. */
      switch (ret)
      {
         case 'd':
              defaultValueFile = copyChar (optarg);
              break;

         case 'r':
              tempRowTitles = copyChar (optarg);
              break;

         case 'c':
              tempColTitles = copyChar (optarg);
              break;

         case 'w':
              tempColWidths = copyChar (optarg);
              break;

         case 'v':
              vrows = atoi (optarg);
              break;

         case 't':
              tempColTypes = copyChar (optarg);
              break;

         case 'F':
              tempFiller = copyChar (optarg);
              break;

         case 'T':
              title = copyChar (optarg);
              break;

         case 'B':
              buttons = copyChar (optarg);
              break;

         case 'O':
              outputFile = copyChar (optarg);
              break;

         case 'X':
              if (strcmp (optarg, "TOP") == 0)
              {
                 xpos = TOP;
              }
              else if (strcmp (optarg, "BOTTOM") == 0)
              {
                 xpos = BOTTOM;
              }
              else if (strcmp (optarg, "LEFT") == 0)
              {
                 xpos = LEFT;
              }
              else if (strcmp (optarg, "RIGHT") == 0)
              {
                 xpos = RIGHT;
              }
              else if (strcmp (optarg, "CENTER") == 0)
              {
                 xpos = CENTER;
              }
              else
              {
                 xpos = atoi (optarg);
              }
              break;

         case 'Y':
              if (strcmp (optarg, "TOP") == 0)
              {
                 ypos = TOP;
              }
              else if (strcmp (optarg, "BOTTOM") == 0)
              {
                 ypos = BOTTOM;
              }
              else if (strcmp (optarg, "LEFT") == 0)
              {
                 ypos = LEFT;
              }
              else if (strcmp (optarg, "RIGHT") == 0)
              {
                 ypos = RIGHT;
              }
              else if (strcmp (optarg, "CENTER") == 0)
              {
                 ypos = CENTER;
              }
              else
              {
                 ypos = atoi (optarg);
              }
              break;

         case 'N':
              boxWidget = FALSE;
              break;

         case 'S':
              shadowWidget = TRUE;
              break;

         default:
              break;
      }
   }

   /* If the user asked for an output file, try to open it. */
   if (outputFile != 0)
   {
      if ((fp = fopen (outputFile, "w")) == 0)
      {
         fprintf (stderr, "%s: Can not open output file %s\n", argv[0], outputFile);
         exit (-1);
      }
   }

   /* Make sure all the needed command line parameters were provided. */
   if ((tempRowTitles == 0) ||
	(tempColTitles == 0) ||
	(tempColWidths == 0) ||
	(vrows == -1))
   {
      fprintf (stderr, "Usage: %s %s\n", argv[0], FPUsage);
      exit (-1);
   }

   /* Convert the char * titles to a char ** */
   rowTemp = CDKsplitString (tempRowTitles, '\n');
   rows = CDKcountStrings (rowTemp);
   for (x=0; x < rows; x++)
   {
      rowTitles[x+1] = rowTemp[x];
   }
   colTemp = CDKsplitString (tempColTitles, '\n');
   cols = CDKcountStrings (colTemp);
   for (x=0; x < cols; x++)
   {
      colTitles[x+1] = colTemp[x];
   }

   /* Convert the column widths. */
   colTemp = CDKsplitString (tempColWidths, '\n');
   count = CDKcountStrings (colTemp);
   for (x=0; x < count; x++)
   {
      colWidths[x+1] = atoi (colTemp[x]);
   }
   CDKfreeStrings(colTemp);

   /* If they passed in the column types, convert them. */
   if (tempColTypes != 0)
   {
      colTemp = CDKsplitString (tempColTypes, '\n');
      count = CDKcountStrings (colTemp);
      for (x=0; x < count; x++)
      {
         colTypes[x+1] = char2DisplayType (colTemp[x]);
      }
      CDKfreeStrings(colTemp);
   }
   else
   {
      /* If they didn't set default values. */
      for (x=0; x < cols; x++)
      {
         colTypes[x+1] = vMIXED;
      }
   }

   /* Start curses. */
   cursesWindow = initscr();

   /* Create the CDK screen. */
   cdkScreen = initCDKScreen (cursesWindow);

   /* Start color. */
   initCDKColor();

   /* Check if the user wants to set the background of the main screen. */
   if ((temp = getenv ("CDK_SCREEN_COLOR")) != 0)
   {
      holder = char2Chtype (temp, &j1, &j2);
      wbkgd (cdkScreen->window, holder[0]);
      wrefresh (cdkScreen->window);
      freeChtype (holder);
   }

   /* Get the widget color background color. */
   if ((CDK_WIDGET_COLOR = getenv ("CDK_WIDGET_COLOR")) == 0)
   {
      CDK_WIDGET_COLOR = 0;
   }

   /* If the set the filler character, set it now. */
   if (tempFiller != 0)
   {
      holder = char2Chtype (tempFiller, &j1, &j2);
      filler = holder[0];
      freeChtype (holder);
   }

   /* Create the matrix widget. */
   widget = newCDKMatrix (cdkScreen, xpos, ypos,
				rows, cols, vrows, cols,
				title, rowTitles, colTitles,
				colWidths, colTypes, 1, 1,
				filler, COL,
				boxWidget, TRUE, shadowWidget);

   /* Make sure we could create the widget. */
   if (widget == 0)
   {
      /* Shut down curses and CDK. */
      destroyCDKScreen (cdkScreen);
      delwin (cursesWindow);
      endCDK();

      /* Spit out the message. */
      fprintf (stderr, "Error: Could not create the matrix. Is the window too small?\n");

      /* Exit with an error. */
      exit (-1);
   }

  /*
   * If the user sent in a file of default values, read it and
   * stick the values read in from the file into the matrix.
   */
   if (defaultValueFile != 0)
   {
      /* Read the file. */
      infoLines = CDKreadFile (defaultValueFile, &rowTemp);
      if (infoLines > 0)
      {
         /* For each line, split on a CTRL-V. */
         for (x=0; x < infoLines; x++)
         {
            colTemp = CDKsplitString (rowTemp[x], CTRL('V'));
            subSize[x+1] = CDKcountStrings (colTemp);
            for (y=0; y < subSize[x+1]; y++)
            {
               info[x+1][y+1] = colTemp[y];
            }
         }

         /* Call setCDKMatrix. */
         setCDKMatrix (widget, info, rows, subSize);

         /* Clean up. */
         for (x=0; x < infoLines; x++)
         {
            for (y=0; y < subSize[x+1]; y++)
            {
               freeChar (info[x+1][y+1]);
            }
         }
      }
   }

   /* Split the buttons if they supplied some. */
   if (buttons != 0)
   {
      /* Split the button list up. */
      buttonList = CDKsplitString (buttons, '\n');
      buttonCount = CDKcountStrings (buttonList);
      freeChar (buttons);

      /* We need to create a buttonbox widget. */
      buttonWidget = newCDKButtonbox (cdkScreen,
					getbegx (widget->win),
					getbegy (widget->win) + widget->boxHeight - 1,
					1, widget->boxWidth - 1,
					NULL, 1, buttonCount,
					buttonList, buttonCount,
					A_REVERSE, boxWidget, FALSE);
      setCDKButtonboxULChar (buttonWidget, ACS_LTEE);
      setCDKButtonboxURChar (buttonWidget, ACS_RTEE);

     /*
      * We need to set the lower left and right
      * characters of the widget.
      */
      setCDKMatrixLLChar (widget, ACS_LTEE);
      setCDKMatrixLRChar (widget, ACS_RTEE);

     /*
      * Bind the Tab key in the widget to send a
      * Tab key to the button box widget.
      */
      bindCDKObject (vMATRIX, widget, KEY_TAB, widgetCB, buttonWidget);
      bindCDKObject (vMATRIX, widget, CDK_NEXT, widgetCB, buttonWidget);
      bindCDKObject (vMATRIX, widget, CDK_PREV, widgetCB, buttonWidget);

      /* Check if the user wants to set the background of the widget. */
      setCDKButtonboxBackgroundColor (buttonWidget, CDK_WIDGET_COLOR);

      /* Draw the button widget. */
      drawCDKButtonbox (buttonWidget, boxWidget);
   }

  /*
   * If the user asked for a shadow, we need to create one.
   * I do this instead of using the shadow parameter because
   * the button widget sin't part of the main widget and if
   * the user asks for both buttons and a shadow, we need to
   * create a shadow big enough for both widgets. We'll create
   * the shadow window using the widgets shadowWin element, so
   * screen refreshes will draw them as well.
   */
   if (shadowWidget == TRUE)
   {
      /* Determine the height of the shadow window. */
      shadowHeight = (buttonWidget == (CDKBUTTONBOX *)NULL ?
			widget->boxHeight :
			widget->boxHeight + buttonWidget->boxHeight - 1);

      /* Create the shadow window. */
      widget->shadowWin = newwin (shadowHeight,
					widget->boxWidth,
					getbegy (widget->win) + 1,
					getbegx (widget->win) + 1);

      /* Make sure we could have created the shadow window. */
      if (widget->shadowWin != 0)
      {
         widget->shadow = TRUE;

        /*
         * We force the widget and buttonWidget to be drawn so the
         * buttonbox widget will be drawn when the widget is activated.
         * Otherwise the shadow window will draw over the button widget.
         */
         drawCDKMatrix (widget, ObjOf(widget)->box);
         eraseCDKButtonbox (buttonWidget);
         drawCDKButtonbox (buttonWidget, ObjOf(buttonWidget)->box);
      }
   }

   /* Check if the user wants to set the background of the widget. */
   setCDKMatrixBackgroundColor (widget, CDK_WIDGET_COLOR);

   /* Let them play. */
   activateCDKMatrix (widget, 0);

   /* Print out the matrix cells. */
   if (widget->exitType == vNORMAL)
   {
      for (x=0; x < widget->rows; x++)
      {
         for (y=0; y < widget->cols; y++)
         {
            if (widget->info[x][y] != 0)
            {
               fprintf (fp, "%s%c", widget->info[x][y], CTRL('V'));
            }
            else
            {
               fprintf (fp, "%c", CTRL('V'));
            }
         }
         fprintf (fp, "\n");
      }
   }

   /* If there were buttons, get the button selected. */
   if (buttonWidget != 0)
   {
      selection = buttonWidget->currentButton;
      destroyCDKButtonbox (buttonWidget);
   }

   /* Shut down curses. */
   destroyCDKMatrix (widget);
   destroyCDKScreen (cdkScreen);
   delwin (cursesWindow);
   endCDK();
   exit (selection);
}

int widgetCB (EObjectType cdktype GCC_UNUSED, void *object GCC_UNUSED, void *clientData, chtype key)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)clientData;
   injectCDKButtonbox (buttonbox, key);
   return (TRUE);
}
