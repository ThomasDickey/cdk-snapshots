/* $Id: cdklabel.c,v 1.4 2002/07/16 19:37:37 tom Exp $ */

#include <cdk.h>

#ifdef XCURSES
char *XCursesProgramName="cdklabel";
#endif

/*
 * Declare file local variables.
 */
char *FPUsage = "-m Message String | -f filename [-c Command] [-p Pause Character] [-s Sleep] [-X X Position] [-Y Y Position] [-N] [-S]";

/*
 *
 */
int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkScreen		= 0;
   CDKLABEL *widget		= 0;
   WINDOW *cursesWindow		= 0;
   char *message		= 0;
   char *filename		= 0;
   char *command		= 0;
   char *CDK_WIDGET_COLOR	= 0;
   char *temp			= 0;
   chtype *holder		= 0;
   char waitChar		= 0;
   boolean shadowWidget		= FALSE;
   boolean boxWidget		= TRUE;
   int xpos			= CENTER;
   int ypos			= CENTER;
   int messageLines		= -1;
   int sleepLength		= -1;
   char **messageList		= 0;
   char tempCommand[1000];
   int ret, x, j1, j2;

   /* Parse up the command line. */
   while (1)
   {
      /* If there aren't any more options, then break. */
      if ((ret = getopt (argc, argv, "m:f:p:s:c:X:Y:NS")) == -1)
      {
         break;
      }

      /* Determine which command line option we just received. */
      switch (ret)
      {
         case 'm':
              message = copyChar (optarg);
              break;

         case 'f':
              filename = copyChar (optarg);
              break;

         case 'c':
              command = copyChar (optarg);
              break;

         case 'p':
              waitChar = optarg[0];
              break;

         case 's':
              sleepLength = atoi (optarg);
              break;

         case 'X':
              if (strcmp (optarg, "TOP") == 0)
              {
                 xpos = TOP;
              }
              else if (strcmp (optarg, "BOTTOM") == 0)
              {
                 xpos = BOTTOM;
              }
              else if (strcmp (optarg, "LEFT") == 0)
              {
                 xpos = LEFT;
              }
              else if (strcmp (optarg, "RIGHT") == 0)
              {
                 xpos = RIGHT;
              }
              else if (strcmp (optarg, "CENTER") == 0)
              {
                 xpos = CENTER;
              }
              else
              {
                 xpos = atoi (optarg);
              }
              break;

         case 'Y':
              if (strcmp (optarg, "TOP") == 0)
              {
                 ypos = TOP;
              }
              else if (strcmp (optarg, "BOTTOM") == 0)
              {
                 ypos = BOTTOM;
              }
              else if (strcmp (optarg, "LEFT") == 0)
              {
                 ypos = LEFT;
              }
              else if (strcmp (optarg, "RIGHT") == 0)
              {
                 ypos = RIGHT;
              }
              else if (strcmp (optarg, "CENTER") == 0)
              {
                 ypos = CENTER;
              }
              else
              {
                 ypos = atoi (optarg);
              }
              break;

         case 'N':
              boxWidget = FALSE;
              break;

         case 'S':
              shadowWidget = TRUE;
              break;

         default:
              break;
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
         freeChar (filename);

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
      freeChar (message);
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

   /* Create the label widget. */
   widget = newCDKLabel (cdkScreen, xpos, ypos,
			messageList, messageLines,
			boxWidget, shadowWidget);

   /* Make sure we could create the widget. */
   if (widget == 0)
   {
      /* Clean up some memory. */
      for (x=0; x < messageLines; x++)
      {
         freeChar (messageList[x]);
      }

      /* Shut down curses and CDK. */
      destroyCDKScreen (cdkScreen);
      delwin (cursesWindow);
      endCDK();

      /* Spit out the message. */
      fprintf (stderr, "Error: Could not create the label. Is the window too small?\n");

      /* Exit with an error. */
      exit (-1);
   }

   /* Check if the user wants to set the background of the widget. */
   setCDKLabelBackgroundColor (widget, CDK_WIDGET_COLOR);

   /* Draw the widget. */
   drawCDKLabel (widget, boxWidget);

   /* If they supplied a command, run it. */
   if (command != 0)
   {
      char *fmt = "(sh -c %.*s) >/dev/null 2>&1";
      sprintf (tempCommand, fmt, (int)(sizeof(tempCommand) - strlen(fmt)), command);
      system (tempCommand);
   }

   /* If they supplied a wait character, wait for the user to hit it. */
   if (waitChar != 0)
   {
      waitCDKLabel (widget, waitChar);
   }

   /* If they supplied a sleep time, sleep for the given length. */
   if (sleepLength != -1)
   {
      sleep (sleepLength);
   }

   /* Clean up. */
   for (x=0; x < messageLines; x++)
   {
      freeChar (messageList[x]);
   }
   destroyCDKLabel (widget);
   destroyCDKScreen (cdkScreen);
   delwin (cursesWindow);
   endCDK();

   /* Exit cleanly. */
   exit (0);
}
