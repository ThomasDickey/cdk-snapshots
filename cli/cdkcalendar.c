/* $Id: cdkcalendar.c,v 1.18 2025/01/09 00:20:21 tom Exp $ */

#include <cdk_test.h>

#ifdef XCURSES
char *XCursesProgramName = "cdkcalendar";
#endif

/*
 * Declare file local prototypes.
 */
static int widgetCB (EObjectType cdktype, void *object, void *clientData, chtype key);

/*
 * Define file local variables.
 */
static void getTodaysDate (int *day, int *month, int *year);

int main (int argc, char **argv)
{
   /* *INDENT-EQLS* */
   CDKSCREEN *cdkScreen         = NULL;
   CDKCALENDAR *widget          = NULL;
   CDKBUTTONBOX *buttonWidget   = NULL;
   struct tm *dateInfo          = NULL;
   time_t selected              = 0;
   chtype dayAttrib             = A_NORMAL;
   chtype monthAttrib           = A_NORMAL;
   chtype yearAttrib            = A_NORMAL;
   chtype highlight             = A_REVERSE;
   chtype *holder               = NULL;
   char *CDK_WIDGET_COLOR       = NULL;
   char *temp                   = NULL;
   int buttonCount              = 0;
   int selection                = 0;
   int shadowHeight             = 0;
   FILE *fp                     = stderr;
   char **buttonList            = NULL;
   int j1, j2;

   CDK_PARAMS params;
   boolean boxWidget;
   boolean shadowWidget;
   char *buttons;
   char *outputFile;
   char *title;
   int day;
   int month;
   int xpos;
   int year;
   int ypos;

   getTodaysDate (&day, &month, &year);

   CDKparseParams (argc, argv, &params, "d:m:y:B:O:T:" CDK_MIN_PARAMS);

   /* *INDENT-EQLS* */
   xpos         = CDKparamValue (&params, 'X', CENTER);
   ypos         = CDKparamValue (&params, 'Y', CENTER);
   boxWidget    = CDKparamValue (&params, 'N', TRUE);
   shadowWidget = CDKparamValue (&params, 'S', FALSE);
   day          = CDKparamValue (&params, 'd', day);
   month        = CDKparamValue (&params, 'm', month);
   year         = CDKparamValue (&params, 'y', year);
   buttons      = CDKparamString (&params, 'B');
   outputFile   = CDKparamString (&params, 'O');
   title        = CDKparamString (&params, 'T');

   /* If the user asked for an output file, try to open it. */
   if (outputFile != NULL)
   {
      if ((fp = fopen (outputFile, "w")) == NULL)
      {
	 fprintf (stderr, "%s: Can not open output file %s\n", argv[0], outputFile);
	 ExitProgram (CLI_ERROR);
      }
   }

   cdkScreen = initCDKScreen (NULL);

   /* Start color. */
   initCDKColor ();

   /* Check if the user wants to set the background of the main screen. */
   if ((temp = getenv ("CDK_SCREEN_COLOR")) != NULL)
   {
      holder = char2Chtype (temp, &j1, &j2);
      wbkgd (cdkScreen->window, holder[0]);
      wrefresh (cdkScreen->window);
      freeChtype (holder);
   }

   /* Check if the user wants to set the background of the widget. */
   if ((CDK_WIDGET_COLOR = getenv ("CDK_WIDGET_COLOR")) == NULL)
   {
      CDK_WIDGET_COLOR = NULL;
   }

   /* Create the calendar width. */
   widget = newCDKCalendar (cdkScreen, xpos, ypos, title,
			    day, month, year,
			    dayAttrib, monthAttrib,
			    yearAttrib, highlight,
			    boxWidget, shadowWidget);
   /* Check to make sure we created the dialog box. */
   if (widget == NULL)
   {
      destroyCDKScreen (cdkScreen);
      endCDK ();

      fprintf (stderr,
	       "Error: Could not create the calendar. "
	       "Is the window too small?\n");

      ExitProgram (CLI_ERROR);
   }

   /* Split the buttons if they supplied some. */
   if (buttons != NULL)
   {
      /* Split the button list up. */
      buttonList = CDKsplitString (buttons, '\n');
      buttonCount = (int)CDKcountStrings ((CDK_CSTRING2) buttonList);

      /* We need to create a buttonbox widget. */
      buttonWidget = newCDKButtonbox (cdkScreen,
				      getbegx (widget->win),
				      (getbegy (widget->win)
				       + widget->boxHeight - 1),
				      1, widget->boxWidth - 1,
				      NULL, 1, buttonCount,
				      (CDK_CSTRING2) buttonList, buttonCount,
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
    * If the user asked for a shadow, we need to create one.  Do this instead
    * of using the shadow parameter because the button widget is not part of
    * the main widget and if the user asks for both buttons and a shadow, we
    * need to create a shadow big enough for both widgets.  Create the shadow
    * window using the widgets shadowWin element, so screen refreshes will draw
    * them as well.
    */
   if (shadowWidget == TRUE)
   {
      /* Determine the height of the shadow window. */
      shadowHeight = (buttonWidget == NULL ?
		      widget->boxHeight :
		      widget->boxHeight + buttonWidget->boxHeight - 1);

      /* Create the shadow window. */
      widget->shadowWin = newwin (shadowHeight,
				  widget->boxWidth,
				  getbegy (widget->win) + 1,
				  getbegx (widget->win) + 1);

      /* Make sure we could have created the shadow window. */
      if (widget->shadowWin != NULL)
      {
	 widget->shadow = TRUE;

	 /*
	  * We force the widget and buttonWidget to be drawn so the
	  * buttonbox widget will be drawn when the widget is activated.
	  * Otherwise the shadow window will draw over the button widget.
	  */
	 drawCDKCalendar (widget, ObjOf (widget)->box);
	 eraseCDKButtonbox (buttonWidget);
	 drawCDKButtonbox (buttonWidget, ObjOf (buttonWidget)->box);
      }
   }

   /* Check if the user wants to set the background of the widget. */
   setCDKCalendarBackgroundColor (widget, CDK_WIDGET_COLOR);

   /* Activate the calendar widget. */
   selected = activateCDKCalendar (widget, NULL);

   /* If there were buttons, get the button selected. */
   if (buttonWidget != NULL)
   {
      selection = buttonWidget->currentButton;
      destroyCDKButtonbox (buttonWidget);
   }

   CDKfreeStrings (buttonList);

   destroyCDKCalendar (widget);
   destroyCDKScreen (cdkScreen);
   endCDK ();

   /* Print out the date selected. D/M/Y format. */
   dateInfo = gmtime (&selected);
   fprintf (fp, "%02d/%02d/%d\n",
	    dateInfo->tm_mday,
	    (dateInfo->tm_mon + 1),
	    (dateInfo->tm_year + 1900));
   fclose (fp);

   ExitProgram (selection);
}

/*
 * This returns what day of the week the month starts on.
 */
static void getTodaysDate (int *day, int *month, int *year)
{
   struct tm *dateInfo;
   time_t clck;

   /* Determine the current time and determine if we are in DST. */
   time (&clck);
   dateInfo = gmtime (&clck);

   /* Set the pointers accordingly. */
   (*day) = dateInfo->tm_mday;
   (*month) = dateInfo->tm_mon + 1;
   (*year) = dateInfo->tm_year + 1900;
}

static int widgetCB (EObjectType cdktype GCC_UNUSED,
		     void *object GCC_UNUSED,
		     void *clientData,
		     chtype key)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)clientData;
   (void) injectCDKButtonbox (buttonbox, key);
   return (TRUE);
}
