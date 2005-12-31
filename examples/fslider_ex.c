/* $Id: fslider_ex.c,v 1.1 2005/12/27 19:16:29 tom Exp $ */

#include <cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "slider_ex";
#endif

/*
 * This program demonstrates the Cdk slider widget.
 */
int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkscreen = 0;
   CDKFSLIDER *widget	= 0;
   WINDOW *cursesWin	= 0;
   char *title		= "<C></U>Enter a value:";
   char *label		= "</B>Current Value:";
   char temp[256], *mesg[5];
   float selection;

   CDK_PARAMS params;
   float high;
   float inc;
   float low;

   float scale;
   int n, digits;

   CDKparseParams(argc, argv, &params, "h:i:l:w:p:" CDK_MIN_PARAMS);
   digits = CDKparamNumber2(&params, 'p', 0);

   scale = 1.0;
   for (n = 0; n < digits; ++n) {
      scale = scale * 10.0;
   }

   high   = CDKparamNumber2(&params, 'h', 100) / scale;
   inc    = CDKparamNumber2(&params, 'i', 1) / scale;
   low    = CDKparamNumber2(&params, 'l', 1) / scale;

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start CDK Colors. */
   initCDKColor();

   /* Create the widget. */
   widget = newCDKFSlider (cdkscreen,
			  CDKparamValue(&params, 'X', CENTER),
			  CDKparamValue(&params, 'Y', CENTER),
			  title, label,
			  A_REVERSE | COLOR_PAIR (29) | ' ',
			  CDKparamNumber2(&params, 'w', 50),
			  low, low, high,
			  inc, (inc*2),
			  digits,
			  CDKparamValue(&params, 'N', TRUE),
			  CDKparamValue(&params, 'S', FALSE));

   /* Is the widget null? */
   if (widget == 0)
   {
      /* Exit CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a message. */
      printf ("Oops. Can't make the widget. Is the window too small?\n");
      ExitProgram (EXIT_FAILURE);
   }

   /* Activate the widget. */
   selection = activateCDKFSlider (widget, 0);

   /* Check the exit value of the widget. */
   if (widget->exitType == vESCAPE_HIT)
   {
      mesg[0] = "<C>You hit escape. No value selected.";
      mesg[1] = "",
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
   }
   else if (widget->exitType == vNORMAL)
   {
      sprintf (temp, "<C>You selected %.*f", digits, selection);
      mesg[0] = copyChar (temp);
      mesg[1] = "",
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
      freeChar (mesg[0]);
   }

   /* Clean up.*/
   destroyCDKFSlider (widget);
   destroyCDKScreen (cdkscreen);
   endCDK();
   ExitProgram (EXIT_SUCCESS);
}
