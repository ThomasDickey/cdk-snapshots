/* $Id: cdkdialog.c,v 1.17 2025/01/09 00:20:21 tom Exp $ */

#include <cdk_test.h>

#ifdef XCURSES
char *XCursesProgramName = "cdkdialog";
#endif

/*
 * Define file local variables.
 */
static const char *FPUsage = "-m Message String | -f filename [-B Buttons] [-O Output file] [-X X Position] [-Y Y Position] [-N] [-S]";

/*
 *
 */
int main (int argc, char **argv)
{
   /* *INDENT-EQLS* */
   CDKSCREEN *cdkScreen         = NULL;
   CDKDIALOG *widget            = NULL;
   char *CDK_WIDGET_COLOR       = NULL;
   char *button                 = NULL;
   char *temp                   = NULL;
   chtype *holder               = NULL;
   int answer                   = 0;
   int messageLines             = -1;
   int buttonCount              = 0;
   FILE *fp                     = stderr;
   char **messageList           = NULL;
   char **buttonList            = NULL;
   int j1, j2;

   CDK_PARAMS params;
   boolean boxWidget;
   boolean shadowWidget;
   char *buttons;
   char *filename;
   char *outputFile;
   char *message;
   int xpos;
   int ypos;

   CDKparseParams (argc, argv, &params, "f:m:B:O:" CDK_MIN_PARAMS);

   /* *INDENT-EQLS* */
   xpos         = CDKparamValue (&params, 'X', CENTER);
   ypos         = CDKparamValue (&params, 'Y', CENTER);
   boxWidget    = CDKparamValue (&params, 'N', TRUE);
   shadowWidget = CDKparamValue (&params, 'S', FALSE);
   filename     = CDKparamString (&params, 'f');
   message      = CDKparamString (&params, 'm');
   buttons      = CDKparamString (&params, 'B');
   outputFile   = CDKparamString (&params, 'O');

   /* If the user asked for an output file, try to open it. */
   if (outputFile != NULL)
   {
      if ((fp = fopen (outputFile, "w")) == NULL)
      {
	 fprintf (stderr, "%s: Can not open output file %s\n", argv[0], outputFile);
	 ExitProgram (CLI_ERROR);
      }
   }

   /* Make sure we have a message to display. */
   if (message == NULL)
   {
      /* No message, maybe they provided a file to read. */
      if (filename != NULL)
      {
	 /* Read the file in. */
	 messageLines = CDKreadFile (filename, &messageList);

	 /* Check if there was an error. */
	 if (messageLines == -1)
	 {
	    fprintf (stderr, "Error: Could not open the file %s\n", filename);
	    ExitProgram (CLI_ERROR);
	 }
      }
      else
      {
	 /* No message, no file, it's an error. */
	 fprintf (stderr, "Usage: %s %s\n", argv[0], FPUsage);
	 ExitProgram (CLI_ERROR);
      }
   }
   else
   {
      /* Split the message up. */
      messageList = CDKsplitString (message, '\n');
      messageLines = (int)CDKcountStrings ((CDK_CSTRING2)messageList);
   }

   /* Set up the buttons for the dialog box. */
   if (buttons == NULL)
   {
      buttonList = calloc(3, sizeof(char *));
      buttonList[0] = copyChar ("OK");
      buttonList[1] = copyChar ("Cancel");
      buttonCount = 2;
   }
   else
   {
      /* Split the button list up. */
      buttonList = CDKsplitString (buttons, '\n');
      buttonCount = (int)CDKcountStrings ((CDK_CSTRING2)buttonList);
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

   /* Create the dialog box. */
   widget = newCDKDialog (cdkScreen, xpos, ypos,
			  (CDK_CSTRING2)messageList, messageLines,
			  (CDK_CSTRING2)buttonList, buttonCount,
			  A_REVERSE,
			  boxWidget, boxWidget, shadowWidget);

   /* Check to make sure we created the dialog box. */
   if (widget == NULL)
   {
      CDKfreeStrings (messageList);
      CDKfreeStrings (buttonList);

      destroyCDKScreen (cdkScreen);
      endCDK ();

      fprintf (stderr,
	       "Error: Could not create the dialog box. "
	       "Is the window too small?\n");

      ExitProgram (CLI_ERROR);
   }

   /* Check if the user wants to set the background of the widget. */
   setCDKDialogBackgroundColor (widget, CDK_WIDGET_COLOR);

   /* Activate the dialog box. */
   answer = activateCDKDialog (widget, NULL);

   /* End CDK. */
   destroyCDKDialog (widget);
   destroyCDKScreen (cdkScreen);
   endCDK ();

   /* Print the name of the button selected. */
   if (answer >= 0)
   {
      button = copyChar (buttonList[answer]);
      fprintf (fp, "%s\n", button);
      freeChar (button);
   }

   CDKfreeStrings (messageList);
   CDKfreeStrings (buttonList);

   /* Exit with the button number picked. */
   ExitProgram (answer);
}
