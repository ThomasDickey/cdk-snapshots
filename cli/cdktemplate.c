/* $Id: cdktemplate.c,v 1.15 2025/01/09 00:20:21 tom Exp $ */

#include <cdk_test.h>

#ifdef XCURSES
char *XCursesProgramName = "cdktemplate";
#endif

/*
 * Declare file local prototypes.
 */
static int widgetCB (EObjectType cdktype, void *object, void *clientData, chtype key);

/*
 * Define file local variables.
 */
static const char *FPUsage = "-p Plate [-o Overlay] [-P Mix Plate] [-d Default Answer] [-m Minimum Length] [-T Title] [-L Label] [-B Buttons] [-O Output file] [-X X Position] [-Y Y Position] [-N] [-S]";

/*
 *
 */
int main (int argc, char **argv)
{
   /* *INDENT-EQLS* */
   CDKSCREEN *cdkScreen         = NULL;
   CDKTEMPLATE *widget          = NULL;
   CDKBUTTONBOX *buttonWidget   = NULL;
   char *answer                 = NULL;
   char *tmp                    = NULL;
   char *CDK_WIDGET_COLOR       = NULL;
   char *temp                   = NULL;
   chtype *holder               = NULL;
   int buttonCount              = 0;
   int selection                = 0;
   int shadowHeight             = 0;
   FILE *fp                     = stderr;
   char **buttonList            = NULL;
   int j1, j2;

   CDK_PARAMS params;
   boolean boxWidget;
   boolean mixPlate;
   boolean shadowWidget;
   char *buttons;
   char *defaultAnswer;
   char *label;
   char *my_overlay;
   char *outputFile;
   char *plate;
   char *title;
   int minimum;
   int xpos;
   int ypos;

   CDKparseParams (argc, argv, &params, "d:m:o:p:B:L:O:P:T:" CDK_MIN_PARAMS);

   /* *INDENT-EQLS* */
   xpos            = CDKparamValue (&params, 'X', CENTER);
   ypos            = CDKparamValue (&params, 'Y', CENTER);
   boxWidget       = CDKparamValue (&params, 'N', TRUE);
   shadowWidget    = CDKparamValue (&params, 'S', FALSE);
   minimum         = CDKparamValue (&params, 'm', 0);
   mixPlate        = CDKparamValue (&params, 'P', FALSE);
   defaultAnswer   = CDKparamString (&params, 'd');
   my_overlay      = CDKparamString (&params, 'o');
   plate           = CDKparamString (&params, 'p');
   buttons         = CDKparamString (&params, 'B');
   label           = CDKparamString (&params, 'L');
   outputFile      = CDKparamString (&params, 'O');
   title           = CDKparamString (&params, 'T');

   /* Make sure all the command line parameters were provided. */
   if (plate == NULL)
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

   /* Create the template widget. */
   widget = newCDKTemplate (cdkScreen, xpos, ypos,
			    title, label,
			    plate, my_overlay,
			    boxWidget, shadowWidget);

   /* Check to make sure we created the widget. */
   if (widget == NULL)
   {
      /* Shut down curses and CDK. */
      destroyCDKScreen (cdkScreen);
      endCDK ();

      fprintf (stderr,
	       "Error: Could not create the template field. "
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
      setCDKTemplateLLChar (widget, ACS_LTEE);
      setCDKTemplateLRChar (widget, ACS_RTEE);

      /*
       * Bind the Tab key in the widget to send a
       * Tab key to the button box widget.
       */
      bindCDKObject (vTEMPLATE, widget, KEY_TAB, widgetCB, buttonWidget);
      bindCDKObject (vTEMPLATE, widget, CDK_NEXT, widgetCB, buttonWidget);
      bindCDKObject (vTEMPLATE, widget, CDK_PREV, widgetCB, buttonWidget);

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
	 drawCDKTemplate (widget, ObjOf (widget)->box);
	 eraseCDKButtonbox (buttonWidget);
	 drawCDKButtonbox (buttonWidget, ObjOf (buttonWidget)->box);
      }
   }

   /* Check if the user wants to set the background of the widget. */
   setCDKTemplateBackgroundColor (widget, CDK_WIDGET_COLOR);

   /* If a default answer were proivded, set it in the widget. */
   if (defaultAnswer != NULL)
   {
      setCDKTemplateValue (widget, defaultAnswer);
   }

   /* If the user asked for a minimum value, set it. */
   setCDKTemplateMin (widget, minimum);

   /* Activate the widget. */
   tmp = activateCDKTemplate (widget, NULL);

   /* If the user asked for plate mixing, give it to them. */
   if (mixPlate == TRUE)
   {
      answer = mixCDKTemplate (widget);
   }
   else
   {
      answer = copyChar (tmp);
   }

   /* If there were buttons, get the button selected. */
   if (buttonWidget != NULL)
   {
      selection = buttonWidget->currentButton;
      destroyCDKButtonbox (buttonWidget);
   }

   /* End CDK. */
   destroyCDKTemplate (widget);
   destroyCDKScreen (cdkScreen);
   endCDK ();

   /* Print the value from the widget. */
   if (answer != NULL)
   {
      fprintf (fp, "%s\n", answer);
      freeChar (answer);
   }
   fclose (fp);

   /* Exit with the button number picked. */
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
