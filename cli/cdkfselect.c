/* $Id: cdkfselect.c,v 1.6 2004/08/28 00:52:41 tom Exp $ */

#include <cdk.h>

#ifdef XCURSES
char *XCursesProgramName="cdkfselect";
#endif

/*
 * Declare file local prototypes.
 */
int widgetCB (EObjectType cdktype, void *object, void *clientData, chtype key);

/*
 * Define file local variables.
 */
char *FPUsage = "[-d Directory] [-T title ] [-L Label] [-B Buttons] [-O Output File] [-X X Position] [-Y Y Position] [-H Height] [-W Width] [-N] [-S]";

/*
 *
 */
int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkScreen		= 0;
   CDKFSELECT *widget		= 0;
   CDKBUTTONBOX *buttonWidget	= 0;
   WINDOW *cursesWindow		= 0;
   char *filename		= 0;
   char *CDK_WIDGET_COLOR	= 0;
   char *temp			= 0;
   chtype *holder		= 0;
   int selection		= 0;
   int shadowHeight		= 0;
   int buttonCount		= 0;
   FILE *fp			= stderr;
   char **buttonList		= 0;
   int x, j1, j2;

   CDK_PARAMS params;
   boolean boxWidget;
   boolean shadowWidget;
   char *buttons;
   char *directory;
   char *label;
   char *outputFile;
   char *title;
   int height;
   int width;
   int xpos;
   int ypos;

   CDKparseParams(argc, argv, &params, "d:B:L:O:T:" CDK_CLI_PARAMS);

   xpos         = CDKparamValue(&params, 'X', CENTER);
   ypos         = CDKparamValue(&params, 'Y', CENTER);
   height       = CDKparamValue(&params, 'H', 0);
   width        = CDKparamValue(&params, 'W', 0);
   boxWidget    = CDKparamValue(&params, 'N', TRUE);
   shadowWidget = CDKparamValue(&params, 'S', FALSE);

   directory    = CDKparamString(&params, 'd');
   buttons      = CDKparamString(&params, 'B');
   label        = CDKparamString(&params, 'L');
   outputFile   = CDKparamString(&params, 'O');
   title        = CDKparamString(&params, 'T');

   /* If the user asked for an output file, try to open it. */
   if (outputFile != 0)
   {
      if ((fp = fopen (outputFile, "w")) == 0)
      {
	 fprintf (stderr, "%s: Can not open output file %s\n", argv[0], outputFile);
	 exit (-1);
      }
   }

   /* If they didn't provide a directory, use . */
   if (directory == 0)
   {
      directory = copyChar (".");
   }

   /* Set the label of the file selector if it hasn't been yet. */
   if (label == 0)
   {
      label = copyChar ("Directory: ");
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

   /* Create the file selector widget. */
   widget = newCDKFselect (cdkScreen, xpos, ypos, height, width,
				title, label, A_NORMAL, '.', A_REVERSE,
				"", "", "", "",
				boxWidget, shadowWidget);

   /* Check to make sure we created the file selector. */
   if (widget == 0)
   {
      /* Clean up used memory. */
      for (x=0; x < buttonCount; x++)
      {
	 freeChar (buttonList[x]);
      }

      /* Shut down curses and CDK. */
      destroyCDKScreen (cdkScreen);
      endCDK();

      /* Spit out the message. */
      fprintf (stderr, "Error: Could not create the file selector. Is the window too small?\n");

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
					getbegy (widget->win) + widget->boxHeight - 2,
					1, widget->boxWidth - 2,
					0, 1, buttonCount,
					buttonList, buttonCount,
					A_REVERSE, boxWidget, FALSE);
      setCDKButtonboxULChar (buttonWidget, ACS_LTEE);
      setCDKButtonboxURChar (buttonWidget, ACS_RTEE);

     /*
      * We need to set the lower left and right
      * characters of the widget.
      */
      setCDKFselectLLChar (widget, ACS_LTEE);
      setCDKFselectLRChar (widget, ACS_RTEE);

     /*
      * Bind the Tab key in the widget to send a
      * Tab key to the button box widget.
      */
      bindCDKObject (vFSELECT, widget, KEY_TAB, widgetCB, buttonWidget);
      bindCDKObject (vFSELECT, widget, CDK_NEXT, widgetCB, buttonWidget);
      bindCDKObject (vFSELECT, widget, CDK_PREV, widgetCB, buttonWidget);

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
	 drawCDKFselect (widget, ObjOf(widget)->box);
	 eraseCDKButtonbox (buttonWidget);
	 drawCDKButtonbox (buttonWidget, ObjOf(buttonWidget)->box);
      }
   }

   /* Check if the user wants to set the background of the widget. */
   setCDKFselectBackgroundColor (widget, CDK_WIDGET_COLOR);

   /* Set the information for the file selector. */
   setCDKFselectDirectory (widget, directory);

   /* Activate the file selector. */
   filename = copyChar(activateCDKFselect (widget, 0));

   /* If there were buttons, get the button selected. */
   if (buttonWidget != 0)
   {
      selection = buttonWidget->currentButton;
      destroyCDKButtonbox (buttonWidget);
   }

   /* Clean up. */
   destroyCDKFselect (widget);
   destroyCDKScreen (cdkScreen);
   endCDK();

   /* Print out the filename selected. */
   fprintf (fp, "%s\n", filename);
   freeChar (filename);
   exit (selection);
}

int widgetCB (EObjectType cdktype GCC_UNUSED, void *object GCC_UNUSED, void *clientData, chtype key)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)clientData;
   injectCDKButtonbox (buttonbox, key);
   return (TRUE);
}
