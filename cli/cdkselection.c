/* $Id: cdkselection.c,v 1.18 2025/01/09 00:20:21 tom Exp $ */

#include <cdk_test.h>

#ifdef XCURSES
char *XCursesProgramName = "cdkselection";
#endif

/*
 * Declare file local prototypes.
 */
static int widgetCB (EObjectType cdktype, void *object, void *clientData, chtype key);

/*
 * Define file local variables.
 */
static const char *FPUsage = "-l List | -f filename [-c Choices ] [-s Selection Bar Position] [-T Title] [-B Buttons] [-O Output File] [-X X Position] [-Y Y Position] [-H Height] [-W Width] [-N] [-S]";

/*
 *
 */
int main (int argc, char **argv)
{
   /* *INDENT-EQLS* */
   CDKSCREEN *cdkScreen         = NULL;
   CDKSELECTION *widget         = NULL;
   CDKBUTTONBOX *buttonWidget   = NULL;
   chtype *holder               = NULL;
   char *item                   = NULL;
   char *CDK_WIDGET_COLOR       = NULL;
   char *temp                   = NULL;
   int scrollLines              = -1;
   int choiceSize               = -1;
   int buttonCount              = 0;
   int selection                = 0;
   int shadowHeight             = 0;
   FILE *fp                     = stderr;
   char **scrollList            = NULL;
   char **choiceList            = NULL;
   char **buttonList            = NULL;
   char **items                 = NULL;
   int choiceValues[MAX_ITEMS];
   int editModes[MAX_ITEMS];
   int x, fields, j1, j2;

   CDK_PARAMS params;
   boolean boxWidget;
   boolean shadowWidget;
   char *buttons;
   char *choices;
   char *filename;
   char *list;
   char *outputFile;
   char *title;
   int height;
   int numbers;
   int spos;
   int width;
   int xpos;
   int ypos;

   CDKparseParams (argc, argv, &params, "c:f:ln:s:B:O:T:" CDK_CLI_PARAMS);

   /* *INDENT-EQLS* */
   xpos         = CDKparamValue (&params, 'X', CENTER);
   ypos         = CDKparamValue (&params, 'Y', CENTER);
   height       = CDKparamValue (&params, 'H', 10);
   width        = CDKparamValue (&params, 'W', 10);
   boxWidget    = CDKparamValue (&params, 'N', TRUE);
   shadowWidget = CDKparamValue (&params, 'S', FALSE);
   choices      = CDKparamString (&params, 'c');
   filename     = CDKparamString (&params, 'f');
   list         = CDKparamString (&params, 'l');
   buttons      = CDKparamString (&params, 'B');
   outputFile   = CDKparamString (&params, 'O');
   title        = CDKparamString (&params, 'T');
   numbers      = CDKparamValue (&params, 'n', FALSE);
   spos         = CDKparsePosition (CDKparamString (&params, 's'));
   (void)numbers;

   /* If the user asked for an output file, try to open it. */
   if (outputFile != NULL)
   {
      if ((fp = fopen (outputFile, "w")) == NULL)
      {
	 fprintf (stderr, "%s: Can not open output file %s\n", argv[0], outputFile);
	 ExitProgram (CLI_ERROR);
      }
   }

   /* Did they provide a list of items. */
   if (list == NULL)
   {
      /* Maybe they gave a filename to use to read. */
      if (filename != NULL)
      {
	 /* Read the file in. */
	 scrollLines = CDKreadFile (filename, &scrollList);

	 /* Check if there was an error. */
	 if (scrollLines == -1)
	 {
	    fprintf (stderr, "Error: Could not open the file '%s'.\n", filename);
	    ExitProgram (CLI_ERROR);
	 }

	 /*
	  * For each line, we will split on a CTRL-V and look for a selection
	  * value/edit mode. The format of the input file can be the following:
	  * Index value [choice value] [edit flag]
	  */
	 for (x = 0; x < scrollLines; x++)
	 {
	    /* Split the line on CTRL-V. */
	    items = CDKsplitString (scrollList[x], CTRL ('V'));
	    fields = (int)CDKcountStrings ((CDK_CSTRING2)items);

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

	    CDKfreeStrings (items);
	 }
      }
      else
      {
	 /* They didn't provide anything. */
	 fprintf (stderr, "Usage: %s %s\n", argv[0], FPUsage);
	 ExitProgram (CLI_ERROR);
      }
   }
   else
   {
      /* Split the scroll lines up. */
      scrollList = CDKsplitString (list, '\n');
      scrollLines = (int)CDKcountStrings ((CDK_CSTRING2)scrollList);
   }

   /* Did they supply a choice list. */
   if (choices == NULL)
   {
      choiceList = calloc(3, sizeof (char *));
      choiceList[0] = copyChar ("Yes ");
      choiceList[1] = copyChar ("No ");
      choiceSize = 2;
   }
   else
   {
      /* Split the choices up. */
      choiceList = CDKsplitString (choices, '\n');
      choiceSize = (int)CDKcountStrings ((CDK_CSTRING2)choiceList);
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

   /* Create the scrolling list. */
   widget = newCDKSelection (cdkScreen, xpos, ypos, spos,
			     height, width, title,
			     (CDK_CSTRING2)scrollList, scrollLines,
			     (CDK_CSTRING2)choiceList, choiceSize,
			     A_REVERSE,
			     boxWidget, shadowWidget);
   CDKfreeStrings (choiceList);

   /* Make sure we could create the widget. */
   if (widget == NULL)
   {
      CDKfreeStrings (scrollList);

      destroyCDKScreen (cdkScreen);
      endCDK ();

      fprintf (stderr,
	       "Error: Could not create the selection list. "
	       "Is the window too small?\n");

      ExitProgram (CLI_ERROR);
   }

   /* Set up the default selection choices. */
   setCDKSelectionChoices (widget, choiceValues);

   /* Split the buttons if they supplied some. */
   if (buttons != NULL)
   {
      /* Split the button list up. */
      buttonList = CDKsplitString (buttons, '\n');
      buttonCount = (int)CDKcountStrings ((CDK_CSTRING2)buttonList);

      /* We need to create a buttonbox widget. */
      buttonWidget = newCDKButtonbox (cdkScreen,
				      getbegx (widget->win),
				      (getbegy (widget->win)
				       + widget->boxHeight - 1),
				      1, widget->boxWidth - 1,
				      NULL, 1, buttonCount,
				      (CDK_CSTRING2)buttonList, buttonCount,
				      A_REVERSE, boxWidget, FALSE);
      CDKfreeStrings (buttonList);

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
	 drawCDKSelection (widget, ObjOf (widget)->box);
	 eraseCDKButtonbox (buttonWidget);
	 drawCDKButtonbox (buttonWidget, ObjOf (buttonWidget)->box);
      }
   }

   /* Check if the user wants to set the background of the widget. */
   setCDKSelectionBackgroundColor (widget, CDK_WIDGET_COLOR);

   /* Set up the default selection modes. */
   setCDKSelectionModes (widget, editModes);

   /* Activate the selection list. */
   activateCDKSelection (widget, NULL);

   /* If there were buttons, get the button selected. */
   if (buttonWidget != NULL)
   {
      selection = buttonWidget->currentButton;
      destroyCDKButtonbox (buttonWidget);
   }

   /* Print out the answer. */
   for (x = 0; x < scrollLines; x++)
   {
      holder = char2Chtype (scrollList[x], &j1, &j2);
      item = chtype2Char (holder);
      fprintf (fp, "%d %s\n", widget->selections[x], item);
      freeChtype (holder);
      freeChar (item);
   }

   CDKfreeStrings (scrollList);

   /* Shut down curses. */
   destroyCDKSelection (widget);
   destroyCDKScreen (cdkScreen);
   endCDK ();
   ExitProgram (selection);
}

static int widgetCB (EObjectType cdktype GCC_UNUSED, void *object GCC_UNUSED,
		     void *clientData,
		     chtype key)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)clientData;
   (void)injectCDKButtonbox (buttonbox, key);
   return (TRUE);
}
