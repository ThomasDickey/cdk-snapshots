/* $Id: position_ex.c,v 1.13 2005/12/26 22:04:35 tom Exp $ */

#include <cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "position_ex";
#endif

/*
 * This demonstrates the positioning of a Cdk entry field widget.
 */
int main(int argc, char **argv)
{
   /* Declare local variables.*/
   CDKSCREEN *cdkscreen = 0;
   CDKENTRY *directory	= 0;
   WINDOW *cursesWin	= 0;
   char *label		= "</U/5>Directory:<!U!5> ";
   char *info		= 0;
   char *mesg[10];
   char temp[256];

   CDK_PARAMS params;

   CDKparseParams(argc, argv, &params, "w:" CDK_MIN_PARAMS);

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start CDK colors. */
   initCDKColor();

   /* Create the entry field widget. */
   directory = newCDKEntry (cdkscreen,
			    CDKparamValue(&params, 'X', CENTER),
			    CDKparamValue(&params, 'Y', CENTER),
			    0, label, A_NORMAL, '.', vMIXED,
			    CDKparamValue(&params, 'w', 40),
			    0, 256,
			    CDKparamValue(&params, 'N', TRUE),
			    CDKparamValue(&params, 'S', FALSE));

   /* Is the widget null? */
   if (directory == 0)
   {
      /* Clean up. */
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a little message. */
      printf ("Oops. Can't seem to create the entry box. Is the window too small?\n");
      ExitProgram (EXIT_FAILURE);
   }

   /* Let the user move the widget around the window. */
   drawCDKEntry (directory, ObjOf(directory)->box);
   positionCDKEntry (directory);

   /* Activate the entry field. */
   info = activateCDKEntry (directory, 0);

   /* Tell them what they typed. */
   if (directory->exitType == vESCAPE_HIT)
   {
      mesg[0] = "<C>You hit escape. No information passed back.";
      mesg[1] = "",
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
   }
   else if (directory->exitType == vNORMAL)
   {
      mesg[0] = "<C>You typed in the following";
      sprintf (temp, "<C>(%.*s)", (int)(sizeof(temp) - 20), info);
      mesg[1] = copyChar (temp);
      mesg[2] = "";
      mesg[3] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 4);
      freeChar (mesg[1]);
   }

   /* Clean up and exit. */
   destroyCDKEntry (directory);
   destroyCDKScreen (cdkscreen);
   endCDK();
   ExitProgram (EXIT_SUCCESS);
}
