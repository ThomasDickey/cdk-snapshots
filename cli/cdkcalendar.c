/* $Id: cdkcalendar.c,v 1.5 2000/09/23 15:12:08 tom Exp $ */

#include <cdk.h>

#ifdef XCURSES
char *XCursesProgramName="cdkcalendar";
#endif

/*
 * Declare file local prototypes.
 */
void widgetCB (EObjectType cdktype, void *object, void *clientData, chtype key);

/*
 * Define file local variables.
 */
char *FPUsage = "[-d Day] [-m Month] [-y Year] [-T Title] [-B Buttons] [-O Output file] [-X X Position] [-Y Y Position] [-N] [-S]";
void getTodaysDate (int *day, int *month, int *year);

int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkScreen		= 0;
   CDKCALENDAR *widget		= 0;
   CDKBUTTONBOX *buttonWidget	= 0;
   WINDOW *cursesWindow		= 0;
   struct tm *dateInfo		= 0;
   time_t selected		= 0;
   chtype dayAttrib		= A_NORMAL;
   chtype monthAttrib		= A_NORMAL;
   chtype yearAttrib		= A_NORMAL;
   chtype highlight		= A_REVERSE;
   chtype *holder		= 0;
   char *CDK_WIDGET_COLOR	= 0;
   char *title			= 0;
   char *buttons		= 0;
   char *outputFile		= 0;
   char *temp			= 0;
   int xpos			= CENTER;
   int ypos			= CENTER;
   int day			= -1;
   int month			= -1;
   int year			= -1;
   int buttonCount		= 0;
   int selection		= 0;
   int shadowHeight		= 0;
   boolean boxWidget		= TRUE;
   boolean shadowWidget		= FALSE;
   FILE *fp			= stderr;
   char **buttonList		= 0;
   int j1, j2, ret;

   /* Set the day/month/year values to todays date. */
   getTodaysDate (&day, &month, &year);

   /* Parse up the command line. */
   while (1)
   {
      /* If there aren't any more options, then break. */
      if ((ret = getopt (argc, argv, "d:m:y:B:O:X:Y:T:NS")) == -1)
      {
         break;
      }

      /* Determine which command line option we just received. */
      switch (ret)
      {
         case 'd':
              day = atoi (optarg);
              break;

         case 'm':
              month = atoi (optarg);
              break;

         case 'y':
              year = atoi (optarg);
              break;

         case 'B':
              buttons = copyChar (optarg);
              break;

         case 'T':
              title = copyChar (optarg);
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

   /* Set up CDK. */
   cursesWindow = initscr();
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

   /* Check if the user wants to set the background of the widget. */
   if ((CDK_WIDGET_COLOR = getenv ("CDK_WIDGET_COLOR")) == 0)
   {
      CDK_WIDGET_COLOR = 0;
   }

   /* Create the calendar width. */
   widget = newCDKCalendar (cdkScreen, xpos, ypos, title,
				day, month, year,
				dayAttrib, monthAttrib,
				yearAttrib, highlight,
				boxWidget, shadowWidget);
   /* Check to make sure we created the dialog box. */
   if (widget == 0)
   {
      /* Shut down curses and CDK. */
      destroyCDKScreen (cdkScreen);
      delwin (cursesWindow);
      endCDK();

      /* Spit out the message. */
      fprintf (stderr, "Error: Could not create the calendar. Is the window too small?\n");

      /* Exit with an error. */
      exit (-1);
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
					0, 1, buttonCount,
					buttonList, buttonCount,
					A_REVERSE, boxWidget, FALSE);
      setCDKButtonboxULChar (buttonWidget, ACS_LTEE);
      setCDKButtonboxURChar (buttonWidget, ACS_RTEE);

     /*
      * We need to set the lower left and right
      * characters of the widget.
      */
      setCDKCalendarLLChar (widget, ACS_LTEE);
      setCDKCalendarLRChar (widget, ACS_RTEE);

     /*
      * Bind the Tab key in the widget to send a
      * Tab key to the button box widget.
      */
      bindCDKObject (vCALENDAR, widget, KEY_TAB, widgetCB, buttonWidget);
      bindCDKObject (vCALENDAR, widget, CDK_PREV, widgetCB, buttonWidget);
      bindCDKObject (vCALENDAR, widget, CDK_NEXT, widgetCB, buttonWidget);

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
         drawCDKCalendar (widget, ObjOf(widget)->box);
         eraseCDKButtonbox (buttonWidget);
         drawCDKButtonbox (buttonWidget, ObjOf(buttonWidget)->box);
      }
   }

   /* Check if the user wants to set the background of the widget. */
   setCDKCalendarBackgroundColor (widget, CDK_WIDGET_COLOR);

   /* Activate the calendar widget. */
   selected = activateCDKCalendar (widget, 0);

   /* If there were buttons, get the button selected. */
   if (buttonWidget != 0)
   {
      selection = buttonWidget->currentButton;
      destroyCDKButtonbox (buttonWidget);
   }

   /* End CDK. */
   destroyCDKCalendar (widget);
   destroyCDKScreen (cdkScreen);
   delwin (cursesWindow);
   endCDK();

   /* Print out the date selected. D/M/Y format. */
   dateInfo = localtime (&selected);

   /* Print out the date selected. */
   fprintf (fp, "%02d/%02d/%d\n", dateInfo->tm_mday, (dateInfo->tm_mon+1), (dateInfo->tm_year+1900));
   exit (selection);
}

/*
 * This returns what day of the week the month starts on.
 */
void getTodaysDate (int *day, int *month, int *year)
{
   struct tm *dateInfo;
   time_t clck;

   /* Determine the current time and determine if we are in DST. */
   time (&clck);
   dateInfo = localtime (&clck);

   /* Set the pointers accordingly. */
   (*day) = dateInfo->tm_mday;
   (*month) = dateInfo->tm_mon+1;
   (*year) = dateInfo->tm_year + 1900;
}

void widgetCB (EObjectType cdktype GCC_UNUSED, void *object GCC_UNUSED, void *clientData, chtype key)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)clientData;
   injectCDKButtonbox (buttonbox, key);
}
