/* $Id: slider_ex.c,v 1.4 2003/11/30 20:01:11 tom Exp $ */

#include <cdk.h>

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
   CDKSLIDER *slider	= 0;
   WINDOW *cursesWin	= 0;
   char *title		= "<C></U>Enter a value:";
   char *label		= "</B>Current Value:";
   char temp[256], *mesg[5];
   int selection;

   CDK_PARAMS params;
   int high;
   int inc;
   int low;

   CDKparseParams(argc, argv, &params, "h:i:l:w:" CDK_MIN_PARAMS);
   high   = CDKparamNumber2(&params, 'h', 100);
   inc    = CDKparamNumber2(&params, 'i', 1);
   low    = CDKparamNumber2(&params, 'l', 1);

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start CDK Colors. */
   initCDKColor();

   /* Create the slider. */
   slider = newCDKSlider (cdkscreen,
			  CDKparamValue(&params, 'X', CENTER),
			  CDKparamValue(&params, 'Y', CENTER),
			  title, label,
			  A_REVERSE | COLOR_PAIR (29) | ' ',
			  CDKparamNumber2(&params, 'w', 50),
			  low, low, high, inc, (inc*2),
			  CDKparamValue(&params, 'N', TRUE),
			  CDKparamValue(&params, 'S', FALSE));

   /* Is the slider null? */
   if (slider == 0)
   {
      /* Exit CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a message. */
      printf ("Oops. Can't make the slider widget. Is the window too small?\n");
      exit (1);
   }

   /* Activate the slider. */
   selection = activateCDKSlider (slider, 0);

   /* Check the exit value of the slider widget. */
   if (slider->exitType == vESCAPE_HIT)
   {
      mesg[0] = "<C>You hit escape. No value selected.";
      mesg[1] = "",
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
   }
   else if (slider->exitType == vNORMAL)
   {
      sprintf (temp, "<C>You selected %d", selection);
      mesg[0] = copyChar (temp);
      mesg[1] = "",
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
      freeChar (mesg[0]);
   }

   /* Clean up.*/
   destroyCDKSlider (slider);
   destroyCDKScreen (cdkscreen);
   delwin (cursesWin);
   endCDK();
   exit (0);
}
