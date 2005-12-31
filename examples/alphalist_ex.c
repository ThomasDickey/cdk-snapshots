/* $Id: alphalist_ex.c,v 1.17 2005/12/28 00:27:35 tom Exp $ */

#include <cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "alphalist_ex";
#endif

/*
 * This program demonstrates the Cdk alphalist widget.
 *
 * Options (in addition to normal CLI parameters):
 *	-c	create the data after the widget
 */

/*
 * This reads the passwd file and retrieves user information.
 */
static int getUserList (char ***list)
{
   struct passwd *ent;
   int x = 0;
   unsigned used = 0;

   while ( (ent = getpwent ()) != 0)
   {
      used = CDKallocStrings(list, ent->pw_name, x++, used);
   }
   endpwent();
   return x;
}

int main(int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkscreen		= 0;
   CDKALPHALIST *alphaList	= 0;
   WINDOW *cursesWin		= 0;
   char *title			= "<C></B/24>Alpha List\n<C>Title";
   char *label			= "</B>Account: ";
   char *word			= 0;
   char **info			= 0;
   char *mesg[5], temp[256];
   int count;

   CDK_PARAMS params;

   CDKparseParams(argc, argv, &params, "c" CDK_CLI_PARAMS);

   /* Get the user list. */
   count = getUserList (&info);
   if (count <= 0)
   {
      fprintf(stderr, "Cannot get user list\n");
      ExitProgram (EXIT_FAILURE);
   }

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start color. */
   initCDKColor();

   /* Create the alpha list widget. */
   alphaList = newCDKAlphalist (cdkscreen,
				CDKparamValue(&params, 'X', CENTER),
				CDKparamValue(&params, 'Y', CENTER),
				CDKparamValue(&params, 'H', 0),
				CDKparamValue(&params, 'W', 0),
				title, label,
				CDKparamNumber(&params, 'c') ? 0 : info,
				CDKparamNumber(&params, 'c') ? 0 : count,
				'_', A_REVERSE,
				CDKparamValue(&params, 'N', TRUE),
				CDKparamValue(&params, 'S', FALSE));
   if (alphaList == 0)
   {
      destroyCDKScreen (cdkscreen);
      endCDK();

      fprintf(stderr, "Cannot create widget\n");
      ExitProgram (EXIT_FAILURE);
   }

   if (CDKparamNumber(&params, 'c'))
   {
      setCDKAlphalistContents (alphaList, info, count);
   }
   CDKfreeStrings(info);

   /* Let them play with the alpha list. */
   word = activateCDKAlphalist (alphaList, 0);

   /* Determine what the user did. */
   if (alphaList->exitType == vESCAPE_HIT)
   {
      mesg[0] = "<C>You hit escape. No word was selected.";
      mesg[1] = "",
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
   }
   else if (alphaList->exitType == vNORMAL)
   {
      mesg[0] = "<C>You selected the following";
      sprintf (temp, "<C>(%.*s)", (int)(sizeof(temp) - 10), word);
      mesg[1] = temp;
      mesg[2] = "";
      mesg[3] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 4);
   }

   /* Clean up. */
   destroyCDKAlphalist (alphaList);
   destroyCDKScreen (cdkscreen);
   endCDK();

   ExitProgram (EXIT_SUCCESS);
}
