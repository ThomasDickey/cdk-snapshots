/* $Id: alphalist_ex.c,v 1.10 2003/12/06 16:56:01 tom Exp $ */

#include <cdk.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName="alphalist_ex";
#endif

/*
 * This program demonstrates the Cdk alphalist widget.
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

   CDKparseParams(argc, argv, &params, CDK_CLI_PARAMS);

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start color. */
   initCDKColor();

   /* Get the user list. */
   count = getUserList (&info);

   /* Create the alpha list widget. */
   alphaList = newCDKAlphalist (cdkscreen,
				CDKparamValue(&params, 'X', CENTER),
				CDKparamValue(&params, 'Y', CENTER),
				CDKparamValue(&params, 'H', 0),
				CDKparamValue(&params, 'W', 0),
				title, label,
				info, count,
				'_', A_REVERSE,
				CDKparamValue(&params, 'N', TRUE),
				CDKparamValue(&params, 'S', FALSE));
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
      sprintf (temp, "<C>(%s)", word);
      mesg[1] = temp;
      mesg[2] = "";
      mesg[3] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 4);
   }

   /* Clean up. */
   destroyCDKAlphalist (alphaList);
   destroyCDKScreen (cdkscreen);
   delwin (cursesWin);
   endCDK();
   exit (0);
}
