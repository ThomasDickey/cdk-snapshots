#include "cdk.h"

#ifdef HAVE_XCURSES
char *XCursesProgramName="viewer_ex";
#endif

#define	MAXINFOLINES	10000

/*
 * This program demonstrates the file selector and the viewer widget.
 */
int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkscreen	= (CDKSCREEN *)NULL;
   CDKVIEWER *example	= (CDKVIEWER *)NULL;
   CDKFSELECT *fSelect	= (CDKFSELECT *)NULL;	
   WINDOW *cursesWin	= (WINDOW *)NULL;
   char *title		= "<C>Pick\n<C>A\n<C>File";
   char *label		= "File: ";
   char *directory	= ".";
   char *info[MAXINFOLINES];
   char *button[5], *filename, vTitle[256], *mesg[4], temp[256];
   int selected, lines, ret;

   /* Parse up the command line. */
   while (1)
   {
      ret = getopt (argc, argv, "d:");
      if (ret == -1)
      {
         break;
      }
      switch (ret)
      {
         case 'd' :
              directory = strdup (optarg);
      }
   }

   /* Create the viewer buttons. */
   button[0]	= "</5><OK><!5>";
   button[1]	= "</5><Cancel><!5>";

   /* Set up CDK. */ 
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start color. */
   initCDKColor();

   /* Get the filename. */
   fSelect = newCDKFselect (cdkscreen, CENTER, CENTER, 20, 65,
				title, label, A_NORMAL, '_', A_REVERSE,
				"</5>", "</48>", "</N>", "</N>", TRUE, FALSE);
 
   /*
    * Set the starting directory. This is not neccessary because when
    * the file selector starts it uses the present directory as a default.
    */
   setCDKFselect (fSelect, directory, A_NORMAL, '.', A_REVERSE, 
			"</5>", "</48>", "</N>", "</N>", ObjOf(fSelect)->box);

   /* Activate the file selector. */
   filename = activateCDKFselect (fSelect, NULL);

   /* Check how the person exited from the widget. */
   if (fSelect->exitType == vESCAPE_HIT)
   {
      /* Pop up a message for the user. */
      mesg[0] = "<C>Escape hit. No file selected.";
      mesg[1] = "";
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);

      /* Exit CDK. */
      destroyCDKFselect (fSelect);
      destroyCDKScreen (cdkscreen);
      endCDK();
   }

   /* Create the file viewer to view the file selected.*/
   example = newCDKViewer (cdkscreen, CENTER, CENTER, 20, -2,
				button, 2, A_REVERSE, TRUE, FALSE);

   /* Could we create the viewer widget? */
   if (example == (CDKVIEWER *)NULL)
   {
      /* Exit CDK. */
      destroyCDKFselect (fSelect);
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a message and exit. */
      printf ("Oops. Can't seem to create viewer. Is the window too small?\n");
      exit (0);
   }

   /* Open the file and read the contents. */
   lines = readFile (filename, info, MAXINFOLINES);
   if (lines == -1)
   {
      printf ("Could not open %s\n", filename);
      exit (1);
   }

   /* Set up the viewer title, and the contents to the widget. */
   sprintf (vTitle, "<C></B/21>Filename:<!21></22>%20s<!22!B>", filename);
   setCDKViewer (example, vTitle, info, lines, A_REVERSE, TRUE, TRUE, TRUE);

   /* Destroy the file selector widget. */
   destroyCDKFselect (fSelect);

   /* Activate the viewer widget. */
   selected = activateCDKViewer (example, (chtype *)NULL);

   /* Check how the person exited from the widget.*/
   if (example->exitType == vESCAPE_HIT)
   {
      mesg[0] = "<C>Escape hit. No Button selected.";
      mesg[1] = "";
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
   }
   else if (example->exitType == vNORMAL)
   {
      sprintf (temp, "<C>You selected button %d", selected);
      mesg[0] = copyChar (temp);
      mesg[1] = "";
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
   }

   /* Clean up. */
   destroyCDKViewer (example);
   destroyCDKScreen (cdkscreen);
   delwin (cursesWin);
   endCDK();
   exit (0);
}
