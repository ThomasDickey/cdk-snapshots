/* $Id: cdkmentry.c,v 1.6 2004/08/28 00:52:41 tom Exp $ */

#include <cdk.h>

#ifdef XCURSES
char *XCursesProgramName="cdkmentry";
#endif

/*
 * Declare file local prototypes.
 */
int widgetCB (EObjectType cdktype, void *object, void *clientData, chtype key);

/*
 * Define file local variables.
 */
char *FPUsage = "-f Field Width -s Screen Rows -v Virtual Rows [-d Display Type] [-F Field Character] [-i Initial Value] [-m Minimum Length] [-T Title] [-L Label] [-B Buttons] [-O Output file] [-X X Position] [-Y Y Position] [-N] [-S]";

/*
 *
 */
int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkScreen		= 0;
   CDKMENTRY *widget		= 0;
   CDKBUTTONBOX *buttonWidget	= 0;
   WINDOW *cursesWindow		= 0;
   chtype *holder		= 0;
   chtype fieldAttr		= 0;
   char *answer			= 0;
   char *CDK_WIDGET_COLOR	= 0;
   char *temp			= 0;
   char filler			= '.';
   EDisplayType dType		= vMIXED;
   int buttonCount		= 0;
   int selection		= 0;
   int shadowHeight		= 0;
   FILE *fp			= stderr;
   char **buttonList		= 0;
   int j1, j2;

   CDK_PARAMS params;
   boolean boxWidget;
   boolean shadowWidget;
   char *buttons;
   char *initValue;
   char *label;
   char *outputFile;
   char *tempFiller;
   char *title;
   int fieldWidth;
   int min;
   int screenRows;
   int virtualRows;
   int xpos;
   int ypos;

   CDKparseParams(argc, argv, &params, "d:f:i:m:s:v:B:F:L:O:T:" "X:Y:NS");

   xpos         = CDKparamValue(&params, 'X', CENTER);
   ypos         = CDKparamValue(&params, 'Y', CENTER);
   boxWidget    = CDKparamValue(&params, 'N', TRUE);
   shadowWidget = CDKparamValue(&params, 'S', FALSE);

   fieldWidth   = CDKparamValue(&params, 'f', 0);
   min          = CDKparamValue(&params, 'm', 0);
   screenRows   = CDKparamValue(&params, 's', 0);
   virtualRows  = CDKparamValue(&params, 'v', 0);
   initValue    = CDKparamString(&params, 'i');
   buttons      = CDKparamString(&params, 'B');
   tempFiller   = CDKparamString(&params, 'F');
   label        = CDKparamString(&params, 'L');
   outputFile   = CDKparamString(&params, 'O');
   title        = CDKparamString(&params, 'T');

   if ((temp = CDKparamString(&params, 'd')) != 0)
      dType = char2DisplayType (temp);

   /* Make sure all the command line parameters were provided. */
   if ((fieldWidth <= 0) || (screenRows <= 0) || (virtualRows <= 0))
   {
      fprintf (stderr, "Usage: %s %s\n", argv[0], FPUsage);
      exit (-1);
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

   /* Get the widget color background color. */
   if ((CDK_WIDGET_COLOR = getenv ("CDK_WIDGET_COLOR")) == 0)
   {
      CDK_WIDGET_COLOR = 0;
   }

   /* If the set the filler character, set it now. */
   if (tempFiller != 0)
   {
      holder	= char2Chtype (tempFiller, &j1, &j2);
      fieldAttr	= A_ATTRIBUTES & holder[0];
      filler	= (chtype)holder[0];
      freeChtype (holder);
   }
   /* Create the mentry widget. */
   widget = newCDKMentry (cdkScreen, xpos, ypos,
				title, label,
				fieldAttr, filler | fieldAttr,
				dType, fieldWidth,
				screenRows, virtualRows,
				min, boxWidget, shadowWidget);

   /* Check to make sure we created the dialog box. */
   if (widget == 0)
   {
      /* Shut down curses and CDK. */
      destroyCDKScreen (cdkScreen);
      endCDK();

      /* Spit out the message. */
      fprintf (stderr, "Error: Could not create the multiple line entry field. Is the window too small?\n");

      /* Exit with an error. */
      exit (-1);
   }

   /* Split the buttons if they supplied some. */
   if (buttons != 0)
   {
      /* Split the button list up. */
      buttonList = CDKsplitString (buttons, '\n');
      buttonCount = CDKcountStrings (buttonList);

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
      setCDKMentryLLChar (widget, ACS_LTEE);
      setCDKMentryLRChar (widget, ACS_RTEE);

     /*
      * Bind the Tab key in the widget to send a
      * Tab key to the button box widget.
      */
      bindCDKObject (vMENTRY, widget, KEY_TAB, widgetCB, buttonWidget);
      bindCDKObject (vMENTRY, widget, CDK_NEXT, widgetCB, buttonWidget);
      bindCDKObject (vMENTRY, widget, CDK_PREV, widgetCB, buttonWidget);

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
	 drawCDKMentry (widget, ObjOf(widget)->box);
	 eraseCDKButtonbox (buttonWidget);
	 drawCDKButtonbox (buttonWidget, ObjOf(buttonWidget)->box);
      }
   }

   /* Check if the user wants to set the background of the widget. */
   setCDKMentryBackgroundColor (widget, CDK_WIDGET_COLOR);

   /* If there was an initial value, set it. */
   if (initValue != 0)
   {
      setCDKMentryValue (widget, initValue);
   }

   /* Activate the widget. */
   answer = copyChar (activateCDKMentry (widget, 0));

   /* If there were buttons, get the button selected. */
   if (buttonWidget != 0)
   {
      selection = buttonWidget->currentButton;
      destroyCDKButtonbox (buttonWidget);
   }

   /* End CDK. */
   destroyCDKMentry (widget);
   destroyCDKScreen (cdkScreen);
   endCDK();

   /* Print the value from the widget. */
   if (answer != 0)
   {
      fprintf (fp, "%s\n", answer);
      freeChar (answer);
   }

   /* Exit with the button number picked. */
   exit (selection);
}

int widgetCB (EObjectType cdktype GCC_UNUSED, void *object GCC_UNUSED, void *clientData, chtype key)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)clientData;
   injectCDKButtonbox (buttonbox, key);
   return (TRUE);
}
