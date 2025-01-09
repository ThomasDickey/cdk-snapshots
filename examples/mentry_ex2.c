/* $Id: mentry_ex2.c,v 1.3 2025/01/09 00:20:21 tom Exp $ */

#include <cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "mentry_ex2";
#endif

int main (int argc, char **argv)
{
   /* *INDENT-EQLS* */
   CDKSCREEN *cdkscreen = NULL;
   CDKMENTRY *widget    = NULL;
   char *info           = NULL;
   const char *label    = "</R>Message";
   const char *title    =
   "<C></5>Enter a message (\".\" to exit).<!5>\n"
   "<C>It can be </3>multi<!3>-line!";
   int boxWidth;

   CDK_PARAMS params;

   CDKparseParams (argc, argv, &params, "w:h:l:" CDK_MIN_PARAMS);

   cdkscreen = initCDKScreen (NULL);

   /* Start CDK Colors. */
   initCDKColor ();

   /* Set up the multi-line entry field. */
   boxWidth = CDKparamValue (&params, 'w', 40);
   widget = newCDKMentry (cdkscreen,
			  CDKparamValue (&params, 'X', CENTER),
			  CDKparamValue (&params, 'Y', CENTER),
			  title, label, A_BOLD, '.', vMIXED,
			  boxWidth,
			  CDKparamValue (&params, 'h', 5),
			  CDKparamValue (&params, 'l', 20),
			  0,
			  CDKparamValue (&params, 'N', TRUE),
			  CDKparamValue (&params, 'S', FALSE));

   /* Is the object null? */
   if (widget == NULL)
   {
      /* Shut down CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK ();

      printf ("Cannot create CDK object. Is the window too small?\n");
      ExitProgram (EXIT_FAILURE);
   }

   refreshCDKScreen (cdkscreen);

   for (;;)
   {
      info = getCdkTitle (ObjOf (widget));
      setCDKMentry (widget, info, 0, TRUE);
      free (info);

      activateCDKMentry (widget, NULL);
      if (strlen (widget->info) > 1)
      {
	 setCdkTitle (ObjOf (widget), widget->info, getmaxx (widget->win));
	 eraseCDKScreen (ScreenOf (widget));
	 drawCDKScreen (ScreenOf (widget));
      }
      else
      {
	 break;
      }
   }

   destroyCDKMentry (widget);
   destroyCDKScreen (cdkscreen);
   endCDK ();

   ExitProgram (EXIT_SUCCESS);
}
