/* $Id: cdkviewer.c,v 1.17 2025/01/09 00:20:21 tom Exp $ */

#include <cdk_test.h>

#ifdef XCURSES
char *XCursesProgramName = "cdkviewer";
#endif

/*
 * Declare file local prototypes.
 */
static void saveInformation (CDKVIEWER *widget);
static int dumpViewer (CDKVIEWER *widget, char *filename);
static int widgetCB (EObjectType cdktype, void *object, void *clientData, chtype key);

/*
 * Define file local variables.
 */
static const char *FPUsage = "-f filename [-i Interpret] [-l Show Line Stats] [-T Title] [-B Buttons] [-X X Position] [-Y Y Position] [-H Height] [-W Width] [-N] [-S]";

/*
 *
 */
int main (int argc, char **argv)
{
   /* *INDENT-EQLS* */
   CDKSCREEN *cdkScreen         = NULL;
   CDKVIEWER *widget            = NULL;
   char *filename               = NULL;
   char *CDK_WIDGET_COLOR       = NULL;
   char *temp                   = NULL;
   chtype *holder               = NULL;
   int answer                   = 0;
   int messageLines             = -1;
   int buttonCount              = 0;
   char **messageList           = NULL;
   char **buttonList            = NULL;
   char tempTitle[256];
   int j1, j2;

   CDK_PARAMS params;
   boolean boxWidget;
   boolean interpret;
   boolean shadowWidget;
   boolean showInfoLine;
   char *buttons;
   char *title;
   int height;
   int width;
   int xpos;
   int ypos;

   CDKparseParams (argc, argv, &params, "f:il:B:T:" CDK_CLI_PARAMS);

   /* *INDENT-EQLS* */
   xpos            = CDKparamValue (&params, 'X', CENTER);
   ypos            = CDKparamValue (&params, 'Y', CENTER);
   height          = CDKparamValue (&params, 'H', 20);
   width           = CDKparamValue (&params, 'W', 60);
   boxWidget       = CDKparamValue (&params, 'N', TRUE);
   shadowWidget    = CDKparamValue (&params, 'S', FALSE);
   interpret       = CDKparamValue (&params, 'i', FALSE);
   showInfoLine    = CDKparamValue (&params, 'l', FALSE);
   filename        = CDKparamString (&params, 'f');
   buttons         = CDKparamString (&params, 'B');
   title           = CDKparamString (&params, 'T');

   /* Make sure they gave us a file to read. */
   if (filename == NULL)
   {
      fprintf (stderr, "Usage: %s %s\n", argv[0], FPUsage);
      ExitProgram (CLI_ERROR);
   }

   /* Read the file in. */
   messageLines = CDKreadFile (filename, &messageList);

   /* Check if there was an error. */
   if (messageLines == -1)
   {
      fprintf (stderr, "Error: Could not open the file %s\n", filename);
      ExitProgram (CLI_ERROR);
   }

   /* Set up the buttons of the viewer. */
   if (buttons == NULL)
   {
      /* *INDENT-EQLS* */
      buttonList        = calloc(3, sizeof(char *));
      buttonList[0]     = copyChar ("OK");
      buttonList[1]     = copyChar ("Cancel");
      buttonCount       = 2;
   }
   else
   {
      /* Split the button list up. */
      buttonList        = CDKsplitString (buttons, '\n');
      buttonCount       = (int)CDKcountStrings ((CDK_CSTRING2)buttonList);
   }

   /* Set up the title of the viewer. */
   if (title == NULL)
   {
      sprintf (tempTitle, "<C>Filename: </U>%s<!U>", filename);
      title = copyChar (tempTitle);
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

   /* Create the viewer widget. */
   widget = newCDKViewer (cdkScreen, xpos, ypos, height, width,
			  (CDK_CSTRING2)buttonList, buttonCount, A_REVERSE,
			  boxWidget, shadowWidget);

   /* Check to make sure we created the file viewer. */
   if (widget == NULL)
   {
      CDKfreeStrings (messageList);
      CDKfreeStrings (buttonList);

      /* Shut down curses and CDK. */
      destroyCDKScreen (cdkScreen);
      endCDK ();

      fprintf (stderr,
	       "Error: Could not create the file viewer. "
	       "Is the window too small?\n");

      ExitProgram (CLI_ERROR);
   }

   /* Check if the user wants to set the background of the widget. */
   setCDKViewerBackgroundColor (widget, CDK_WIDGET_COLOR);

   /* Set a binding for saving the info. */
   bindCDKObject (vVIEWER, widget, 'S', widgetCB, NULL);

   /* Set the information needed for the viewer. */
   setCDKViewer (widget, title, (CDK_CSTRING2)messageList, messageLines,
		 A_REVERSE, interpret, showInfoLine, TRUE);

   /* Activate the viewer. */
   answer = activateCDKViewer (widget, NULL);

   CDKfreeStrings (messageList);
   CDKfreeStrings (buttonList);

   destroyCDKViewer (widget);
   destroyCDKScreen (cdkScreen);
   endCDK ();

   /* Exit with the button number picked. */
   ExitProgram (answer);
}

/*
 * This function allows the user to dump the
 * information from the viewer into a file.
 */
static void saveInformation (CDKVIEWER *widget)
{
   /* *INDENT-EQLS* */
   CDKENTRY *entry      = NULL;
   char *filename       = NULL;
   char temp[256];
   const char *mesg[10];
   int linesSaved;

   /* Create the entry field to get the filename. */
   entry = newCDKEntry (ScreenOf (widget), CENTER, CENTER,
			"<C></B/5>Enter the filename of the save file.",
			"Filename: ",
			A_NORMAL, '_', vMIXED,
			20, 1, 256,
			TRUE, FALSE);

   /* Get the filename. */
   filename = activateCDKEntry (entry, NULL);

   /* Did they hit escape? */
   if (entry->exitType == vESCAPE_HIT)
   {
      /* Popup a message. */
      mesg[0] = "<C></B/5>Save Canceled.";
      mesg[1] = "<C>Escape hit. Scrolling window information not saved.";
      mesg[2] = " ";
      mesg[3] = "<C>Press any key to continue.";
      popupLabel (ScreenOf (widget), (CDK_CSTRING2)mesg, 4);

      destroyCDKEntry (entry);
      return;
   }

   /* Write the contents of the viewer to the file. */
   linesSaved = dumpViewer (widget, filename);

   /* Was the save successful? */
   if (linesSaved == -1)
   {
      /* Nope, tell 'em. */
      mesg[0] = "<C></B/16>Error";
      mesg[1] = "<C>Could not save to the file.";
      sprintf (temp, "<C>(%s)", filename);
      mesg[2] = temp;
      mesg[3] = " ";
      mesg[4] = "<C>Press any key to continue.";
      popupLabel (ScreenOf (widget), (CDK_CSTRING2)mesg, 5);
   }
   else
   {
      char *msg_1;

      mesg[0] = "<C></B/5>Save Successful";
      sprintf (temp, "<C>There were %d lines saved to the file", linesSaved);
      mesg[1] = msg_1 = copyChar (temp);
      sprintf (temp, "<C>(%s)", filename);
      mesg[2] = temp;
      mesg[3] = " ";
      mesg[4] = "<C>Press any key to continue.";
      popupLabel (ScreenOf (widget), (CDK_CSTRING2)mesg, 5);
      freeChar (msg_1);
   }

   destroyCDKEntry (entry);
   eraseCDKScreen (ScreenOf (widget));
   drawCDKScreen (ScreenOf (widget));
}

/*
 * This actually dumps the information from the viewer to a file.
 */
static int dumpViewer (CDKVIEWER *widget, char *filename)
{
   /* *INDENT-EQLS* */
   FILE *outputFile     = NULL;
   char *rawLine        = NULL;
   int listSize;
   chtype **list        = getCDKViewerInfo (widget, &listSize);
   int x;

   /* Try to open the file. */
   if ((outputFile = fopen (filename, "w")) == NULL)
   {
      return -1;
   }

   /* Start writing out the file. */
   for (x = 0; x < listSize; x++)
   {
      rawLine = chtype2Char (list[x]);
      fprintf (outputFile, "%s\n", rawLine);
      freeChar (rawLine);
   }

   /* Close the file and return the number of lines written. */
   fclose (outputFile);
   return listSize;
}

static int widgetCB (EObjectType cdktype GCC_UNUSED, void *object,
		     void *clientData GCC_UNUSED,
		     chtype key GCC_UNUSED)
{
   CDKVIEWER *widget = (CDKVIEWER *)object;
   saveInformation (widget);
   return (TRUE);
}
