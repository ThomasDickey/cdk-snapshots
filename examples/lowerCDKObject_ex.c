/* $Id: lowerCDKObject_ex.c,v 1.7 2005/12/26 22:04:35 tom Exp $ */

#include <cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "lowerCDKObject_ex";
#endif

int main (int argc, char **argv)
{
   char ch;

   char *mesg[1];
   char *mesg1[10];
   char *mesg2[10];

   WINDOW *cursesWin;
   CDKSCREEN *cdkscreen;
   CDKLABEL *label1, *label2, *instruct;

   CDK_PARAMS params;

   CDKparseParams (argc, argv, &params, CDK_MIN_PARAMS);

   /* Set up CDK. */
   cursesWin = initscr ();
   cdkscreen = initCDKScreen (cursesWin);

   mesg1[0] = "label1 label1 label1 label1 label1 label1 label1";
   mesg1[1] = "label1 label1 label1 label1 label1 label1 label1";
   mesg1[2] = "label1 label1 label1 label1 label1 label1 label1";
   mesg1[3] = "label1 label1 label1 label1 label1 label1 label1";
   mesg1[4] = "label1 label1 label1 label1 label1 label1 label1";
   mesg1[5] = "label1 label1 label1 label1 label1 label1 label1";
   mesg1[6] = "label1 label1 label1 label1 label1 label1 label1";
   mesg1[7] = "label1 label1 label1 label1 label1 label1 label1";
   mesg1[8] = "label1 label1 label1 label1 label1 label1 label1";
   mesg1[9] = "label1 label1 label1 label1 label1 label1 label1";
   label1 = newCDKLabel (cdkscreen, 8, 5, mesg1, 10, TRUE, FALSE);

   mesg2[0] = "label2 label2 label2 label2 label2 label2 label2";
   mesg2[1] = "label2 label2 label2 label2 label2 label2 label2";
   mesg2[2] = "label2 label2 label2 label2 label2 label2 label2";
   mesg2[3] = "label2 label2 label2 label2 label2 label2 label2";
   mesg2[4] = "label2 label2 label2 label2 label2 label2 label2";
   mesg2[5] = "label2 label2 label2 label2 label2 label2 label2";
   mesg2[6] = "label2 label2 label2 label2 label2 label2 label2";
   mesg2[7] = "label2 label2 label2 label2 label2 label2 label2";
   mesg2[8] = "label2 label2 label2 label2 label2 label2 label2";
   mesg2[9] = "label2 label2 label2 label2 label2 label2 label2";
   label2 = newCDKLabel (cdkscreen, 14, 9, mesg2, 10, TRUE, FALSE);

   mesg[0] = "</B>1<!B> - lower </U>label1<!U>, </B>2<!B> - lower </U>label2<!U>, </B>q<!B> - </U>quit<!U>";
   instruct = newCDKLabel (cdkscreen,
			   CDKparamValue (&params, 'X', CENTER),
			   CDKparamValue (&params, 'Y', BOTTOM),
			   mesg, 1,
			   CDKparamValue (&params, 'N', FALSE),
			   CDKparamValue (&params, 'S', FALSE));

   refreshCDKScreen (cdkscreen);

   while ((ch = getch ()) != 'q')
   {
      switch (ch)
      {
      case '1':
	 lowerCDKObject (vLABEL, label1);
	 break;
      case '2':
	 lowerCDKObject (vLABEL, label2);
	 break;
      default:
	 continue;
      }
      refreshCDKScreen (cdkscreen);
   }

   /* Clean up. */
   destroyCDKLabel (label1);
   destroyCDKLabel (label2);
   destroyCDKLabel (instruct);
   destroyCDKScreen (cdkscreen);
   endCDK ();
   ExitProgram (EXIT_SUCCESS);
}
