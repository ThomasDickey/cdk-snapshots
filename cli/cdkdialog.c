/* $Id: cdkdialog.c,v 1.6 2004/08/28 00:52:41 tom Exp $ */

#include <cdk.h>

#ifdef XCURSES
char *XCursesProgramName="cdkdialog";
#endif

/*
 * Define file local variables.
 */
char *FPUsage = "-m Message String | -f filename [-B Buttons] [-O Output file] [-X X Position] [-Y Y Position] [-N] [-S]";

/*
 *
 */
int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkScreen		= 0;
   CDKDIALOG *widget		= 0;
   WINDOW *cursesWindow		= 0;
   char *CDK_WIDGET_COLOR	= 0;
   char *button			= 0;
   char *temp			= 0;
   chtype *holder		= 0;
   int answer			= 0;
   int messageLines		= -1;
   int buttonCount		= 0;
   FILE *fp			= stderr;
   char **messageList		= 0;
   char **buttonList		= 0;
   int x, j1, j2;

   CDK_PARAMS params;
   boolean boxWidget;
   boolean shadowWidget;
   char *buttons;
   char *filename;
   char *outputFile;
   char *message;
   int xpos;
   int ypos;

   CDKparseParams(argc, argv, &params, "f:m:B:O:" "X:Y:NS");

   xpos         = CDKparamValue(&params, 'X', CENTER);
   ypos         = CDKparamValue(&params, 'Y', CENTER);
   boxWidget    = CDKparamValue(&params, 'N', TRUE);
   shadowWidget = CDKparamValue(&params, 'S', FALSE);

   filename     = CDKparamString(&params, 'f');
   message      = CDKparamString(&params, 'm');
   buttons      = CDKparamString(&params, 'B');
   outputFile   = CDKparamString(&params, 'O');

   /* If the user asked for an output file, try to open it. */
   if (outputFile != 0)
   {
      if ((fp = fopen (outputFile, "w")) == 0)
      {
	 fprintf (stderr, "%s: Can not open output file %s\n", argv[0], outputFile);
	 exit (-1);
      }
   }

   /* Make sure we have a message to display. */
   if (message == 0)
   {
      /* No message, maybe they provided a file to read. */
      if (filename != 0)
      {
	 /* Read the file in. */
	 messageLines = CDKreadFile (filename, &messageList);

	 /* Check if there was an error. */
	 if (messageLines == -1)
	 {
	    fprintf (stderr, "Error: Could not open the file %s\n", filename);
	    exit (-1);
	 }
      }
      else
      {
	 /* No message, no file, it's an error. */
	 fprintf (stderr, "Usage: %s %s\n", argv[0], FPUsage);
	 exit (-1);
      }
   }
   else
   {
      /* Split the message up. */
      messageList = CDKsplitString (message, '\n');
      messageLines = CDKcountStrings (messageList);
   }

   /* Set up the buttons for the dialog box. */
   if (buttons == 0)
   {
      buttonList[0]	= copyChar ("OK");
      buttonList[1]	= copyChar ("Cancel");
      buttonCount	= 2;
   }
   else
   {
      /* Split the button list up. */
      buttonList = CDKsplitString (buttons, '\n');
      buttonCount = CDKcountStrings (buttonList);
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

   /* Create the dialog box. */
   widget = newCDKDialog (cdkScreen, xpos, ypos,
				messageList, messageLines,
				buttonList, buttonCount,
				A_REVERSE,
				boxWidget, boxWidget, shadowWidget);

   /* Check to make sure we created the dialog box. */
   if (widget == 0)
   {
      /* Clean up used memory. */
      for (x=0; x < messageLines; x++)
      {
	 freeChar (messageList[x]);
      }
      for (x=0; x < buttonCount; x++)
      {
	 freeChar (buttonList[x]);
      }

      /* Shut down curses and CDK. */
      destroyCDKScreen (cdkScreen);
      endCDK();

      /* Spit out the message. */
      fprintf (stderr, "Error: Could not create the dialog box. Is the window too small?\n");

      /* Exit with an error. */
      exit (-1);
   }

   /* Check if the user wants to set the background of the widget. */
   setCDKDialogBackgroundColor (widget, CDK_WIDGET_COLOR);

   /* Activate the dialog box. */
   answer = activateCDKDialog (widget, 0);

   /* End CDK. */
   destroyCDKDialog (widget);
   destroyCDKScreen (cdkScreen);
   endCDK();

   /* Print the name of the button selected. */
   if (answer >= 0)
   {
      button = copyChar (buttonList[answer]);
      fprintf (fp, "%s\n", button);
      freeChar (button);
   }

   /* Clean up. */
   for (x=0; x < messageLines; x++)
   {
      freeChar (messageList[x]);
   }
   for (x=0; x < buttonCount; x++)
   {
      freeChar (buttonList[x]);
   }

   /* Exit with the button number picked. */
   exit (answer);
}
