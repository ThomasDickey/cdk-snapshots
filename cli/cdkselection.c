/* $Id: cdkselection.c,v 1.3 2000/09/23 15:09:12 tom Exp $ */

#include <cdk.h>

#ifdef XCURSES
char *XCursesProgramName="cdkselection";
#endif

/*
 * Declare file local prototypes.
 */
void widgetCB (EObjectType cdktype, void *object, void *clientData, chtype key);

/*
 * Define file local variables.
 */
char *FPUsage = "-l List | -f filename [-c Choices ] [-s Selection Bar Position] [-T Title] [-B Buttons] [-O Output File] [-X X Position] [-Y Y Position] [-H Height] [-W Width] [-N] [-S]";

/*
 *
 */
int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkScreen		= 0;
   CDKSELECTION *widget		= 0;
   CDKBUTTONBOX *buttonWidget	= 0;
   WINDOW *cursesWindow		= 0;
   chtype *holder		= 0;
   char *filename		= 0;
   char *choices		= 0;
   char *title			= 0;
   char *item			= 0;
   char *list			= 0;
   char *outputFile		= 0;
   char *buttons		= 0;
   char *CDK_WIDGET_COLOR	= 0;
   char *temp			= 0;
   int xpos			= CENTER;
   int ypos			= CENTER;
   int spos			= NONE;
   int numbers			= FALSE;
   int height			= 10;
   int width			= 10;
   int ret			= 0;
   int scrollLines		= -1;
   int choiceSize		= -1;
   int buttonCount		= 0;
   int selection		= 0;
   int shadowHeight		= 0;
   boolean boxWidget		= TRUE;
   boolean shadowWidget		= FALSE;
   FILE *fp			= stderr;
   char **scrollList		= 0;
   char **choiceList		= 0;
   char **buttonList		= 0;
   char **items		= 0;
   int choiceValues[MAX_ITEMS];
   int editModes[MAX_ITEMS];
   int x, y, fields, j1, j2;

   /* Parse up the command line. */
   while (1)
   {
      /* If there aren't any more options, then break. */
      if ((ret = getopt (argc, argv, "l:T:B:f:s:c:O:H:W:X:Y:NS")) == -1)
      {
         break;
      }

      /* Determine which command line option we just received. */
      switch (ret)
      {
         case 'f':
              filename = copyChar (optarg);
              break;

         case 'l':
              list = copyChar (optarg);
              break;

         case 'T':
              title = copyChar (optarg);
              break;

         case 'B':
              buttons = copyChar (optarg);
              break;

         case 'c':
              choices = copyChar (optarg);
              break;

         case 'O':
              outputFile = copyChar (optarg);
              break;

         case 'W':
              if (strcmp (optarg, "FULL") == 0)
              {
                 width = COLS;
              }
              else
              {
                 width = (int)atoi (optarg);
              }
              break;

         case 'H':
              if (strcmp (optarg, "FULL") == 0)
              {
                 height = LINES;
              }
              else
              {
                 height = (int)atoi (optarg);
              }
              break;

         case 'n':
              numbers = TRUE;
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
                 xpos = (int)atoi (optarg);
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
                 ypos = (int)atoi (optarg);
              }
              break;

         case 's':
              if (strcmp (optarg, "LEFT") == 0)
              {
                 spos = LEFT;
              }
              else if (strcmp (optarg, "RIGHT") == 0)
              {
                 spos = RIGHT;
              }
              else
              {
                 spos = NONE;
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

   /* Did they provide a list of items. */
   if (list == 0)
   {
      /* Maybe they gave a filename to use to read. */
      if (filename != 0)
      {
         /* Read the file in. */
         scrollLines = CDKreadFile (filename, &scrollList);

         /* Check if there was an error. */
         if (scrollLines == -1)
         {
            fprintf (stderr, "Error: Could not open the file '%s'.\n", filename);
            exit (-1);
         }

        /*
         * For each line, we will split on a CTRL-V and look for a selection
         * value/edit mode. The format of the input file can be the following:
         * Index value [choice value] [edit flag]
         */
         for (x=0; x < scrollLines; x++)
         {
            /* Split the line on CTRL-V. */
            items = CDKsplitString (scrollList[x], CTRL('V'));
            fields = CDKcountStrings (items);

            /* Check the field count. */
            if (fields == 1)
            {
               choiceValues[x] = 0;
               editModes[x] = 0;
            }
            else if (fields == 2)
            {
               freeChar (scrollList[x]);
               scrollList[x] = copyChar (items[0]);

               choiceValues[x] = (int)atoi (items[1]);
               editModes[x] = 0;
            }
            else if (fields == 3)
            {
               freeChar (scrollList[x]);
               scrollList[x] = copyChar (items[0]);

               choiceValues[x] = (int)atoi (items[1]);
               editModes[x] = (int)atoi (items[2]);
            }

            /* Clean up. */
            for (y=0; y < fields; y++)
            {
               freeChar (items[y]);
            }
         }
      }
      else
      {
         /* They didn't provide anything. */
         fprintf (stderr, "Usage: %s %s\n", argv[0], FPUsage);
         exit (-1);
      }
   }
   else
   {
      /* Split the scroll lines up. */
      scrollList = CDKsplitString (list, '\n');
      scrollLines = CDKcountStrings (scrollList);
      freeChar (title);
   }

   /* Did they supply a chopice list. */
   if (choices == 0)
   {
      choiceList[0] = copyChar ("Yes ");
      choiceList[1] = copyChar ("No ");
      choiceSize = 2;
   }
   else
   {
      /* Split the choices up. */
      choiceList = CDKsplitString (choices, '\n');
      choiceSize = CDKcountStrings (choiceList);
      freeChar (choices);
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

   /* Create the scrolling list. */
   widget = newCDKSelection (cdkScreen, xpos, ypos, spos,
				height, width, title,
				scrollList, scrollLines,
				choiceList, choiceSize,
				A_REVERSE,
				boxWidget, shadowWidget);

   /* Make sure we could create the widget. */
   if (widget == 0)
   {
      /* Clean up some memory. */
      for (x=0; x < scrollLines; x++)
      {
         freeChar (scrollList[x]);
      }
      freeChar (title);

      /* Shut down curses and CDK. */
      destroyCDKScreen (cdkScreen);
      delwin (cursesWindow);
      endCDK();

      /* Spit out the message. */
      fprintf (stderr, "Error: Could not create the selection list. Is the window too small?\n");

      /* Exit with an error. */
      exit (-1);
   }

   /* Set up the default selection choices. */
   setCDKSelectionChoices (widget, choiceValues);

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
					0, 1, buttonCount,
					buttonList, buttonCount,
					A_REVERSE, boxWidget, FALSE);
      setCDKButtonboxULChar (buttonWidget, ACS_LTEE);
      setCDKButtonboxURChar (buttonWidget, ACS_RTEE);

     /*
      * We need to set the lower left and right
      * characters of the widget.
      */
      setCDKSelectionLLChar (widget, ACS_LTEE);
      setCDKSelectionLRChar (widget, ACS_RTEE);

     /*
      * Bind the Tab key in the widget to send a
      * Tab key to the button box widget.
      */
      bindCDKObject (vSELECTION, widget, KEY_TAB, widgetCB, buttonWidget);
      bindCDKObject (vSELECTION, widget, CDK_NEXT, widgetCB, buttonWidget);
      bindCDKObject (vSELECTION, widget, CDK_PREV, widgetCB, buttonWidget);

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
      shadowHeight = (buttonWidget == 0 ?
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
         drawCDKSelection (widget, ObjOf(widget)->box);
         eraseCDKButtonbox (buttonWidget);
         drawCDKButtonbox (buttonWidget, ObjOf(buttonWidget)->box);
      }
   }

   /* Check if the user wants to set the background of the widget. */
   setCDKSelectionBackgroundColor (widget, CDK_WIDGET_COLOR);

   /* Set up the default selection modes. */
   setCDKSelectionModes (widget, editModes);

   /* Activate the selection list. */
   activateCDKSelection (widget, 0);

   /* If there were buttons, get the button selected. */
   if (buttonWidget != 0)
   {
      selection = buttonWidget->currentButton;
      destroyCDKButtonbox (buttonWidget);
   }

   /* Print out the answer. */
   for (x=0; x < scrollLines; x++)
   {
      holder = char2Chtype (scrollList[x], &j1, &j2);
      item = chtype2Char (holder);
      fprintf (fp, "%d %s\n", widget->selections[x], item);
      freeChtype (holder);
      freeChar (item);
   }

   /* Shut down curses. */
   destroyCDKSelection (widget);
   destroyCDKScreen (cdkScreen);
   delwin (cursesWindow);
   endCDK();
   exit (selection);
}

void widgetCB (EObjectType cdktype GCC_UNUSED, void *object GCC_UNUSED, void *clientData, chtype key)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)clientData;
   injectCDKButtonbox (buttonbox, key);
}
