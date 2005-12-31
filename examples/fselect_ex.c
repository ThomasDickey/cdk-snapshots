/* $Id: fselect_ex.c,v 1.16 2005/12/30 09:47:30 tom Exp $ */

#include <cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "fselect_ex";
#endif

/*
 * This program demonstrates the file selector and the viewer widget.
 */
int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkscreen = 0;
   CDKVIEWER *example	= 0;
   CDKFSELECT *fSelect	= 0;
   WINDOW *cursesWin	= 0;
   char *title		= "<C>Pick\n<C>A\n<C>File";
   char *label		= "File: ";
   char **info		= 0;
   char *button[5], *filename, vTitle[256], *mesg[4], temp[256];
   int selected, lines;

   CDK_PARAMS params;
   char *directory;

   CDKparseParams(argc, argv, &params, "d:" CDK_CLI_PARAMS);
   directory = CDKparamString2(&params, 'd', ".");

   /* Create the viewer buttons. */
   button[0]	= "</5><OK><!5>";
   button[1]	= "</5><Cancel><!5>";

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start color. */
   initCDKColor();

   /* Get the filename. */
   fSelect = newCDKFselect (cdkscreen,
			    CDKparamValue(&params, 'X', CENTER),
			    CDKparamValue(&params, 'Y', CENTER),
			    CDKparamValue(&params, 'H', 20),
			    CDKparamValue(&params, 'W', 65),
			    title, label, A_NORMAL, '_', A_REVERSE,
			    "</5>", "</48>", "</N>", "</N>",
			    CDKparamValue(&params, 'N', TRUE),
			    CDKparamValue(&params, 'S', FALSE));

   if (fSelect == 0)
   {
      destroyCDKScreen (cdkscreen);
      endCDK();

      fprintf(stderr, "Cannot create widget\n");
      ExitProgram (EXIT_FAILURE);
   }

   /*
    * Set the starting directory. This is not necessary because when
    * the file selector starts it uses the present directory as a default.
    */
   setCDKFselect (fSelect, directory, A_NORMAL, '.', A_REVERSE,
			"</5>", "</48>", "</N>", "</N>", ObjOf(fSelect)->box);

   /* Activate the file selector. */
   filename = activateCDKFselect (fSelect, 0);

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

      ExitProgram (EXIT_SUCCESS);
   }

   /* Create the file viewer to view the file selected.*/
   example = newCDKViewer (cdkscreen, CENTER, CENTER, 20, -2,
				button, 2, A_REVERSE, TRUE, FALSE);

   /* Could we create the viewer widget? */
   if (example == 0)
   {
      /* Exit CDK. */
      destroyCDKFselect (fSelect);
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a message and exit. */
      printf ("Oops. Can't seem to create viewer. Is the window too small?\n");
      ExitProgram (EXIT_SUCCESS);
   }

   /* Open the file and read the contents. */
   lines = CDKreadFile (filename, &info);
   if (lines == -1)
   {
      endCDK();

      destroyCDKFselect (fSelect);
      destroyCDKScreen (cdkscreen);

      printf ("Could not open \"%s\"\n", filename);
      ExitProgram (EXIT_FAILURE);
   }

   /* Set up the viewer title, and the contents to the widget. */
   sprintf (vTitle, "<C></B/21>Filename:<!21></22>%20s<!22!B>", filename);
   setCDKViewer (example, vTitle, info, lines, A_REVERSE, TRUE, TRUE, TRUE);

   CDKfreeStrings(info);

   /* Destroy the file selector widget. */
   destroyCDKFselect (fSelect);

   /* Activate the viewer widget. */
   selected = activateCDKViewer (example, 0);

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
      mesg[0] = temp;
      mesg[1] = "";
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
   }

   /* Clean up. */
   destroyCDKViewer (example);
   destroyCDKScreen (cdkscreen);
   endCDK();
   ExitProgram (EXIT_SUCCESS);
}
