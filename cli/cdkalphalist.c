/* $Id: cdkalphalist.c,v 1.7 2003/11/28 19:13:05 tom Exp $ */

#include <cdk.h>

#ifdef XCURSES
char *XCursesProgramName="cdkalphalist";
#endif

/*
 * Declare file local prototypes.
 */
int widgetCB (EObjectType cdktype, void *object, void *clientData, chtype key);

/*
 * Define file local variables.
 */
char *FPUsage = "-l List | -f filename [-F Field Character] [-T Title] [-L Label] [-B Buttons] [-O Output File] [-X X Position] [-Y Y Position] [-H Height] [-W Width] [-N] [-S]";

/*
 *
 */
int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkScreen		= 0;
   CDKALPHALIST *widget		= 0;
   CDKBUTTONBOX *buttonWidget	= 0;
   WINDOW *cursesWindow		= 0;
   char *CDK_WIDGET_COLOR	= 0;
   char *answer			= 0;
   char *buttons;
   char *temp			= 0;
   chtype *holder		= 0;
   chtype filler		= A_NORMAL | '.';
   int scrollLines		= -1;
   int buttonCount		= 0;
   int selection		= 0;
   int shadowHeight		= 0;
   FILE *fp			= stderr;
   char **scrollList		= 0;
   char **buttonList		= 0;
   int j1, j2;

   CDK_PARAMS params;
   boolean boxWidget;
   boolean shadowWidget;
   char *filename;
   char *label;
   char *list;
   char *outputFile;
   char *tempFiller;
   char *title;
   int height;
   int width;
   int xpos;
   int ypos;

   CDKparseParams(argc, argv, &params, "f:l:B:F:L:O:T:" CDK_CLI_PARAMS);

   xpos         = CDKparamValue(&params, 'X', CENTER);
   ypos         = CDKparamValue(&params, 'Y', CENTER);
   height       = CDKparamValue(&params, 'H', -1);
   width        = CDKparamValue(&params, 'W', -1);
   boxWidget    = CDKparamValue(&params, 'N', TRUE);
   shadowWidget = CDKparamValue(&params, 'S', FALSE);

   filename     = CDKparamString(&params, 'f');
   list         = CDKparamString(&params, 'l');
   buttons      = CDKparamString(&params, 'B');
   tempFiller   = CDKparamString(&params, 'F');
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

   /* If they set the filler character, set it now. */
   if (tempFiller != 0)
   {
      holder = char2Chtype (tempFiller, &j1, &j2);
      filler = holder[0];
      freeChtype (holder);
   }

   /* Create the widget. */
   widget = newCDKAlphalist (cdkScreen, xpos, ypos,
				height, width,
				title, label,
				scrollList, scrollLines,
				filler, A_REVERSE,
				boxWidget, shadowWidget);

   /* Make sure we could create the widget. */
   if (widget == 0)
   {
      /* Clean up some memory. */
      CDKfreeStrings(scrollList);

      /* Shut down curses and CDK. */
      destroyCDKScreen (cdkScreen);
      delwin (cursesWindow);
      endCDK();

      /* Spit out the message. */
      fprintf (stderr, "Error: Could not create the alphalist. Is the window too small?\n");

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
					getbegx (widget->win) + 1,
					getbegy (widget->win) + widget->boxHeight - 1,
					1, widget->boxWidth - 3,
					0, 1, buttonCount,
					buttonList, buttonCount,
					A_REVERSE, boxWidget, FALSE);
      setCDKButtonboxULChar (buttonWidget, ACS_LTEE);
      setCDKButtonboxURChar (buttonWidget, ACS_RTEE);

     /*
      * We need to set the lower left and right
      * characters of the entry field.
      */
      setCDKAlphalistLLChar (widget, ACS_LTEE);
      setCDKAlphalistLRChar (widget, ACS_RTEE);

     /*
      * Bind the Tab key in the entry field to send a
      * Tab key to the button box widget.
      */
      bindCDKObject (vENTRY, widget->entryField, KEY_RIGHT, widgetCB, buttonWidget);
      bindCDKObject (vENTRY, widget->entryField, KEY_LEFT, widgetCB, buttonWidget);
      bindCDKObject (vENTRY, widget->entryField, CDK_NEXT, widgetCB, buttonWidget);
      bindCDKObject (vENTRY, widget->entryField, CDK_PREV, widgetCB, buttonWidget);

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
         drawCDKAlphalist (widget, ObjOf(widget)->box);
         eraseCDKButtonbox (buttonWidget);
         drawCDKButtonbox (buttonWidget, ObjOf(buttonWidget)->box);
      }
   }

   /* Check if the user wants to set the background of the widget. */
   setCDKAlphalistBackgroundColor (widget, CDK_WIDGET_COLOR);

   /* Activate the widget. */
   answer = copyChar (activateCDKAlphalist (widget, 0));

   /* If there were buttons, get the button selected. */
   if (buttonWidget != 0)
   {
      selection = buttonWidget->currentButton;
      destroyCDKButtonbox (buttonWidget);
   }

   /* Shut down curses. */
   destroyCDKAlphalist (widget);
   destroyCDKScreen (cdkScreen);
   delwin (cursesWindow);
   endCDK();

   /* Print out the answer. */
   if (answer != 0)
   {
      fprintf (fp, "%s\n", answer);
   }

   /* Exit with the selected button. */
   exit (0);
}

int widgetCB (EObjectType cdktype GCC_UNUSED, void *object GCC_UNUSED, void *clientData, chtype key)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)clientData;
   injectCDKButtonbox (buttonbox, key);
   return (TRUE);
}
