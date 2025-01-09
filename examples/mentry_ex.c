/* $Id: mentry_ex.c,v 1.12 2025/01/09 00:20:21 tom Exp $ */

#include <cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "mentry_ex";
#endif

int main (int argc, char **argv)
{
   /* *INDENT-EQLS* */
   CDKSCREEN *cdkscreen = NULL;
   CDKMENTRY *widget    = NULL;
   char *info           = NULL;
   const char *label    = "</R>Message";
   const char *title    = "<C></5>Enter a message.<!5>";

   CDK_PARAMS params;

   CDKparseParams (argc, argv, &params, "w:h:l:" CDK_MIN_PARAMS);

   cdkscreen = initCDKScreen (NULL);

   /* Start CDK Colors. */
   initCDKColor ();

   /* Set up the multi-line entry field. */
   widget = newCDKMentry (cdkscreen,
			  CDKparamValue (&params, 'X', CENTER),
			  CDKparamValue (&params, 'Y', CENTER),
			  title, label, A_BOLD, '.', vMIXED,
			  CDKparamValue (&params, 'w', 20),
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

   /* Draw the CDK screen. */
   refreshCDKScreen (cdkscreen);

   /* Set what ever was given from the command line. */
   setCDKMentry (widget, argv[optind], 0, TRUE);

   /* Activate this thing. */
   activateCDKMentry (widget, NULL);
   info = strdup (widget->info);

   /* Clean up. */
   destroyCDKMentry (widget);
   destroyCDKScreen (cdkscreen);
   endCDK ();

   printf ("\n\n\n");
   printf ("Your message was : <%s>\n", info);
   free (info);
   ExitProgram (EXIT_SUCCESS);
}
