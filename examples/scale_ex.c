/* $Id: scale_ex.c,v 1.4 2003/11/30 19:50:16 tom Exp $ */

#include <cdk.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "scale_ex";
#endif

/*
 * This program demonstrates the Cdk scale widget.
 */
int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkscreen = 0;
   CDKSCALE *scale	= 0;
   WINDOW *cursesWin	= 0;
   char *title		= "<C>Select a value";
   char *label		= "</5>Current value";
   char temp[256], *mesg[5];
   int selection;

   CDK_PARAMS params;
   int high;
   int inc;
   int low;

   CDKparseParams(argc, argv, &params, "h:i:l:w:" CDK_MIN_PARAMS);
   high   = CDKparamNumber2(&params, 'h', 100);
   inc    = CDKparamNumber2(&params, 'i', 1);
   low    = CDKparamNumber2(&params, 'l', 0);

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start CDK Colors. */
   initCDKColor();

   /* Create the scale. */
   scale = newCDKScale (cdkscreen,
			CDKparamValue(&params, 'X', CENTER),
			CDKparamValue(&params, 'Y', CENTER),
			title, label, A_NORMAL,
			CDKparamNumber2(&params, 'w', 5),
			low, low, high,
			inc, (inc*2),
			CDKparamValue(&params, 'N', TRUE),
			CDKparamValue(&params, 'S', FALSE));

   /* Is the scale null? */
   if (scale == 0)
   {
      /* Exit CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a message. */
      printf ("Oops. Can't make the scale widget. Is the window too small?\n");
      exit (1);
   }

   /* Activate the scale. */
   selection = activateCDKScale (scale, 0);

   /* Check the exit value of the scale widget. */
   if (scale->exitType == vESCAPE_HIT)
   {
      mesg[0] = "<C>You hit escape. No value selected.";
      mesg[1] = "",
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
   }
   else if (scale->exitType == vNORMAL)
   {
      sprintf (temp, "<C>You selected %d", selection);
      mesg[0] = copyChar (temp);
      mesg[1] = "",
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
      freeChar (mesg[0]);
   }

   /* Clean up. */
   destroyCDKScale (scale);
   destroyCDKScreen (cdkscreen);
   delwin (cursesWin);
   endCDK();
   exit (0);
}
