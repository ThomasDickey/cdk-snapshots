/* $Id: viewer_ex2.c,v 1.10 2025/01/09 00:20:21 tom Exp $ */

#include <cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "viewer_ex";
#endif

/*
 * This program demonstrates the viewFile() function.
 */
int main (int argc, char **argv)
{
   /* *INDENT-EQLS* */
   CDKSCREEN *cdkscreen = NULL;
   CDKFSELECT *fSelect  = NULL;
   const char *button[5];
   char vTitle[256];
   const char *mesg[4];
   char temp[256];
   int selected;

   CDK_PARAMS params;
   char *filename;		/* specify filename, bypassing fselect */
   char *directory;		/* specify starting directory for fselect */
   int interp_it;		/* interpret embedded markup */

   CDKparseParams (argc, argv, &params, "f:d:i" CDK_CLI_PARAMS);
   /* *INDENT-EQLS* */
   filename     = CDKparamString (&params, 'f');
   directory    = CDKparamString2 (&params, 'd', ".");
   interp_it    = CDKparamNumber2 (&params, 'i', FALSE);
   (void)interp_it;

   /* Create the viewer buttons. */
   button[0] = "</5><OK><!5>";
   button[1] = "</5><Cancel><!5>";

   cdkscreen = initCDKScreen (NULL);

   /* Start color. */
   initCDKColor ();

   /* Get the filename. */
   if (filename == NULL)
   {
      const char *title = "<C>Pick\n<C>A\n<C>File";
      const char *label = "File: ";

      fSelect = newCDKFselect (cdkscreen,
			       CDKparamValue (&params, 'X', CENTER),
			       CDKparamValue (&params, 'Y', CENTER),
			       CDKparamValue (&params, 'H', 20),
			       CDKparamValue (&params, 'W', 65),
			       title, label, A_NORMAL, '_', A_REVERSE,
			       "</5>", "</48>", "</N>", "</N>",
			       CDKparamValue (&params, 'N', TRUE),
			       CDKparamValue (&params, 'S', FALSE));
      if (fSelect == NULL)
      {
	 destroyCDKScreen (cdkscreen);
	 endCDK ();

	 fprintf (stderr, "Cannot create fselect-widget\n");
	 ExitProgram (EXIT_FAILURE);
      }

      /*
       * Set the starting directory. This is not necessary because when
       * the file selector starts it uses the present directory as a default.
       */
      setCDKFselect (fSelect, directory, A_NORMAL, '.', A_REVERSE,
		     "</5>", "</48>", "</N>", "</N>", ObjOf (fSelect)->box);

      /* Activate the file selector. */
      filename = activateCDKFselect (fSelect, NULL);

      /* Check how the person exited from the widget. */
      if (fSelect->exitType == vESCAPE_HIT)
      {
	 /* Pop up a message for the user. */
	 mesg[0] = "<C>Escape hit. No file selected.";
	 mesg[1] = "";
	 mesg[2] = "<C>Press any key to continue.";
	 popupLabel (cdkscreen, (CDK_CSTRING2)mesg, 3);

	 /* Exit CDK. */
	 destroyCDKFselect (fSelect);
	 destroyCDKScreen (cdkscreen);
	 endCDK ();

	 ExitProgram (EXIT_SUCCESS);
      }
   }

   /* Set up the viewer title, and the contents to the widget. */
   sprintf (vTitle, "<C></B/21>Filename:<!21></22>%20s<!22!B>", filename);

   selected = viewFile (cdkscreen, vTitle, filename, (CDK_CSTRING2)button, 2);

   /* Destroy the file selector widget (do not need filename anymore) */
   destroyCDKFselect (fSelect);

   /* Check how the person exited from the widget. */
   sprintf (temp, "<C>You selected button %d", selected);
   mesg[0] = temp;
   mesg[1] = "";
   mesg[2] = "<C>Press any key to continue.";
   popupLabel (cdkscreen, (CDK_CSTRING2)mesg, 3);

   /* Clean up. */
   destroyCDKScreen (cdkscreen);
   endCDK ();
   ExitProgram (EXIT_SUCCESS);
}
