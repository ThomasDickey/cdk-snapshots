/* $Id: mentry_ex.c,v 1.5 2003/11/30 19:21:19 tom Exp $ */

#include <cdk.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "mentry_ex";
#endif

int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkscreen = 0;
   CDKMENTRY *filename	= 0;
   WINDOW *cursesWin	= 0;
   char *info		= 0;
   char *label		= "</R>Message";
   char *title		= "<C></5>Enter a message.<!5>";

   CDK_PARAMS params;

   CDKparseParams (argc, argv, &params, "w:h:l:" CDK_MIN_PARAMS);

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start CDK Colors.*/
   initCDKColor();

   /* Set up the multi-line entry field. */
   filename = newCDKMentry (cdkscreen,
			    CDKparamValue(&params, 'X', CENTER),
			    CDKparamValue(&params, 'Y', CENTER),
			    title, label, A_BOLD, '.', vMIXED,
			    CDKparamValue(&params, 'w', 20),
			    CDKparamValue(&params, 'h', 5),
			    CDKparamValue(&params, 'l', 20),
			    0,
			    CDKparamValue(&params, 'N', TRUE),
			    CDKparamValue(&params, 'S', FALSE));

   /* Is the object null? */
   if (filename == 0)
   {
      /* Shut down CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a message. */
      printf ("Oops. Can not create CDK object. Is the window too small?\n");
      exit (1);
   }

   /* Draw the CDK screen. */
   refreshCDKScreen (cdkscreen);

   /* Set what ever was given from the command line. */
   setCDKMentry (filename, argv[optind], 0, TRUE);

   /* Activate this thing. */
   activateCDKMentry (filename, 0);
   info = strdup (filename->info);

   /* Clean up. */
   destroyCDKMentry (filename);
   destroyCDKScreen (cdkscreen);
   delwin (cursesWin);
   endCDK();

   /* Spit out the results. */
   printf ("\n\n\n");
   printf ("Your message was : <%s>\n", info);
   free(info);
   exit (0);
}
