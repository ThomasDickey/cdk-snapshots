/* $Id: hello_ex.c,v 1.9 2005/12/26 22:04:35 tom Exp $ */

#include <cdk_test.h>

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
   mesg[0] = "</5><#UL><#HL(30)><#UR>";
   mesg[1] = "</5><#VL(10)>Hello World!<#VL(10)>";
   mesg[2] = "</5><#LL><#HL(30)><#LR>";

   /* Declare the labels. */
   demo = newCDKLabel (cdkscreen,
		       CDKparamValue(&params, 'X', CENTER),
		       CDKparamValue(&params, 'Y', CENTER),
		       mesg, 3,
		       CDKparamValue(&params, 'N', TRUE),
		       CDKparamValue(&params, 'S', TRUE));

   setCDKLabelBackgroundAttrib (demo, COLOR_PAIR(2));

   /* Is the label null? */
   if (demo == 0)
   {
      /* Clean up the memory. */
      destroyCDKScreen (cdkscreen);

      /* End curses... */
      endCDK();

      /* Spit out a message. */
      printf ("Oops. Can't seem to create the label. Is the window too small?\n");
      ExitProgram (EXIT_FAILURE);
   }

   /* Draw the CDK screen. */
   refreshCDKScreen (cdkscreen);
   waitCDKLabel (demo, ' ');

   /* Clean up. */
   destroyCDKLabel (demo);
   destroyCDKScreen (cdkscreen);
   endCDK();
   ExitProgram (EXIT_SUCCESS);
}
