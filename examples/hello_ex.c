/* $Id: hello_ex.c,v 1.5 2003/11/30 17:57:18 tom Exp $ */

#include <cdk.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "hello_ex";
#endif

int main(int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN	*cdkscreen;
   CDKLABEL	*demo;
   WINDOW	*cursesWin;
   char		*mesg[4];

   CDK_PARAMS params;

   CDKparseParams(argc, argv, &params, CDK_MIN_PARAMS);

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start CDK Colors. */
   initCDKColor();

   /* Set the labels up. */
   mesg[0] = "                               ";
   mesg[1] = "<C>Hello World!";
   mesg[2] = "                               ";

   /* Declare the labels. */
   demo = newCDKLabel (cdkscreen,
		       CDKparamValue(&params, 'X', CENTER),
		       CDKparamValue(&params, 'Y', CENTER),
		       mesg, 3,
		       CDKparamValue(&params, 'N', TRUE),
		       CDKparamValue(&params, 'S', TRUE));

   /* Is the label null? */
   if (demo == 0)
   {
      /* Clean up the memory. */
      destroyCDKScreen (cdkscreen);

      /* End curses... */
      endCDK();

      /* Spit out a message. */
      printf ("Oops. Can't seem to create the label. Is the window too small?\n");
      exit (1);
   }

   /* Draw the CDK screen. */
   refreshCDKScreen (cdkscreen);
   waitCDKLabel (demo, ' ');

   /* Clean up. */
   destroyCDKLabel (demo);
   destroyCDKScreen (cdkscreen);
   delwin (cursesWin);
   endCDK();
   exit (0);
}
