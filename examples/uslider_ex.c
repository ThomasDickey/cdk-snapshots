/* $Id: uslider_ex.c,v 1.6 2025/01/09 00:20:21 tom Exp $ */

#include <cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "uslider_ex";
#endif

/*
 * This program demonstrates the Cdk unsigned-slider widget.
 */
int main (int argc, char **argv)
{
   /* *INDENT-EQLS* */
   CDKSCREEN *cdkscreen = NULL;
   CDKUSLIDER *widget   = NULL;
   char title[256];
   const char *label    = "</B>Current Value:";
   const char *mesg[5];
   char temp[256];
   unsigned selection;

   CDK_PARAMS params;
   unsigned high;
   unsigned inc;
   unsigned low;

   /* *INDENT-EQLS* */
   CDKparseParams (argc, argv, &params, "h:i:l:w:" CDK_MIN_PARAMS);
   high   = (unsigned)CDKparamNumber2 (&params, 'h', 100);
   inc    = (unsigned)CDKparamNumber2 (&params, 'i', 1);
   low    = (unsigned)CDKparamNumber2 (&params, 'l', 1);

   sprintf (title, "<C></U>Enter a value:\nLow  %#x\nHigh %#x", low, high);

   cdkscreen = initCDKScreen (NULL);

   /* Start CDK Colors. */
   initCDKColor ();

   /* Create the widget. */
   widget = newCDKUSlider (cdkscreen,
			   CDKparamValue (&params, 'X', CENTER),
			   CDKparamValue (&params, 'Y', CENTER),
			   title, label,
			   A_REVERSE | COLOR_PAIR (29) | ' ',
			   CDKparamNumber2 (&params, 'w', 50),
			   low, low, high,
			   inc, (inc * 2),
			   CDKparamValue (&params, 'N', TRUE),
			   CDKparamValue (&params, 'S', FALSE));

   /* Is the widget null? */
   if (widget == NULL)
   {
      /* Exit CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK ();

      printf ("Cannot make the widget. Is the window too small?\n");
      ExitProgram (EXIT_FAILURE);
   }

   /* Activate the widget. */
   selection = activateCDKUSlider (widget, NULL);

   /* Check the exit value of the widget. */
   if (widget->exitType == vESCAPE_HIT)
   {
      mesg[0] = "<C>You hit escape. No value selected.";
      mesg[1] = "";
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 3);
   }
   else if (widget->exitType == vNORMAL)
   {
      sprintf (temp, "<C>You selected %u", selection);
      mesg[0] = temp;
      mesg[1] = "";
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, (CDK_CSTRING2) mesg, 3);
   }

   /* Clean up. */
   destroyCDKUSlider (widget);
   destroyCDKScreen (cdkscreen);
   endCDK ();
   ExitProgram (EXIT_SUCCESS);
}
