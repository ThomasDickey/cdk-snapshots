/* $Id: cdkscale.c,v 1.15 2025/01/09 00:20:21 tom Exp $ */

#include <cdk_test.h>

#ifdef XCURSES
char *XCursesProgramName = "cdkscale";
#endif

/*
 * Declare file local prototypes.
 */
static int widgetCB (EObjectType cdktype, void *object, void *clientData, chtype key);

/*
 * Define file local variables.
 */
static const char *FPUsage = "-f Field Width -l Low Value -h High Value [-s Initial Value]] [-i Increment Value] [-a Accelerated Increment Value] [-T Title] [-L Label] [-B Buttons] [-O Output File] [-X X Position] [-Y Y Position] [-N] [-S]";

/*
 *
 */
int main (int argc, char **argv)
{
   /* *INDENT-EQLS* */
   CDKSCREEN *cdkScreen         = NULL;
   CDKSCALE *widget             = NULL;
   CDKBUTTONBOX *buttonWidget   = NULL;
   char *CDK_WIDGET_COLOR       = NULL;
   char *temp                   = NULL;
   chtype *holder               = NULL;
   int answer                   = 0;
   int buttonCount              = 0;
   int selection                = 0;
   int shadowHeight             = 0;
   FILE *fp                     = stderr;
   char **buttonList            = NULL;
   int tmp, j1, j2;

   CDK_PARAMS params;
   boolean boxWidget;
   boolean shadowWidget;
   char *buttons;
   char *label;
   char *outputFile;
   char *title;
   int fieldWidth;
   int incrementStep;
   int acceleratedStep;
   int initValue;
   int lowValue;
   int highValue;
   int xpos;
   int ypos;

   CDKparseParams (argc, argv, &params, "a:f:h:i:l:s:B:L:O:T:" CDK_MIN_PARAMS);

   /* *INDENT-EQLS* */
   xpos            = CDKparamValue (&params, 'X', CENTER);
   ypos            = CDKparamValue (&params, 'Y', CENTER);
   boxWidget       = CDKparamValue (&params, 'N', TRUE);
   shadowWidget    = CDKparamValue (&params, 'S', FALSE);
   acceleratedStep = CDKparamValue (&params, 'a', -1);
   fieldWidth      = CDKparamValue (&params, 'f', 0);
   highValue       = CDKparamValue (&params, 'h', INT_MIN);
   incrementStep   = CDKparamValue (&params, 'i', 1);
   lowValue        = CDKparamValue (&params, 'l', INT_MAX);
   initValue       = CDKparamValue (&params, 's', INT_MIN);
   buttons         = CDKparamString (&params, 'B');
   label           = CDKparamString (&params, 'L');
   outputFile      = CDKparamString (&params, 'O');
   title           = CDKparamString (&params, 'T');

   incrementStep = abs (incrementStep);

   /* Make sure all the command line parameters were provided. */
   if (fieldWidth <= 0)
   {
      fprintf (stderr, "Usage: %s %s\n", argv[0], FPUsage);
      ExitProgram (CLI_ERROR);
   }

   /* Make sure the user supplied the low/high values. */
   if ((lowValue == INT_MAX) || (highValue == INT_MIN))
   {
      fprintf (stderr, "Usage: %s %s\n", argv[0], FPUsage);
      ExitProgram (CLI_ERROR);
   }

   /* If the user asked for an output file, try to open it. */
   if (outputFile != NULL)
   {
      if ((fp = fopen (outputFile, "w")) == NULL)
      {
	 fprintf (stderr, "%s: Can not open output file %s\n", argv[0], outputFile);
	 ExitProgram (CLI_ERROR);
      }
   }

   /* Make sure the low is lower than the high (and vice versa). */
   if (lowValue > highValue)
   {
      /* *INDENT-EQLS* */
      tmp       = lowValue;
      lowValue  = highValue;
      highValue = tmp;
   }

   /* Make sure the starting value is in range. */
   if (initValue < lowValue)
   {
      initValue = lowValue;
   }
   else if (initValue > highValue)
   {
      initValue = highValue;
   }

   /* Check if the accelerated incremnt value was set. */
   if (acceleratedStep <= 0)
   {
      acceleratedStep = (int)((highValue - lowValue) / 10);
      acceleratedStep = MAXIMUM (1, acceleratedStep);
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

   /* Get the widget color background color. */
   if ((CDK_WIDGET_COLOR = getenv ("CDK_WIDGET_COLOR")) == NULL)
   {
      CDK_WIDGET_COLOR = NULL;
   }

   /* Create the entry widget. */
   widget = newCDKScale (cdkScreen, xpos, ypos,
			 title, label,
			 A_NORMAL, fieldWidth,
			 initValue, lowValue, highValue,
			 incrementStep, acceleratedStep,
			 boxWidget, shadowWidget);

   /* Check to make sure we created the dialog box. */
   if (widget == NULL)
   {
      /* Shut down curses and CDK. */
      destroyCDKScreen (cdkScreen);
      endCDK ();

      fprintf (stderr,
	       "Error: Could not create the numeric scale field. "
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
      CDKfreeStrings (buttonList);

      setCDKButtonboxULChar (buttonWidget, ACS_LTEE);
      setCDKButtonboxURChar (buttonWidget, ACS_RTEE);

      /*
       * We need to set the lower left and right
       * characters of the widget.
       */
      setCDKScaleLLChar (widget, ACS_LTEE);
      setCDKScaleLRChar (widget, ACS_RTEE);

      /*
       * Bind the Tab key in the widget to send a
       * Tab key to the button box widget.
       */
      bindCDKObject (vSCALE, widget, KEY_TAB, widgetCB, buttonWidget);
      bindCDKObject (vSCALE, widget, CDK_NEXT, widgetCB, buttonWidget);
      bindCDKObject (vSCALE, widget, CDK_PREV, widgetCB, buttonWidget);

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
	 drawCDKScale (widget, ObjOf (widget)->box);
	 eraseCDKButtonbox (buttonWidget);
	 drawCDKButtonbox (buttonWidget, ObjOf (buttonWidget)->box);
      }
   }

   /* Check if the user wants to set the background of the widget. */
   setCDKScaleBackgroundColor (widget, CDK_WIDGET_COLOR);

   /* Activate the widget. */
   answer = activateCDKScale (widget, NULL);

   /* If there were buttons, get the button selected. */
   if (buttonWidget != NULL)
   {
      selection = buttonWidget->currentButton;
      destroyCDKButtonbox (buttonWidget);
   }

   /* End CDK. */
   destroyCDKScale (widget);
   destroyCDKScreen (cdkScreen);
   endCDK ();

   /* Print the value from the widget. */
   fprintf (fp, "%d\n", answer);
   fclose (fp);

   /* Exit with the answer. */
   ExitProgram (selection);
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
