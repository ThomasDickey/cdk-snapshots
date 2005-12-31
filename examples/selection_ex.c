/* $Id: selection_ex.c,v 1.15 2005/12/28 01:46:34 tom Exp $ */

#include <cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "selection_ex";
#endif

/*
 * This program demonstrates the Cdk selection widget.
 *
 * Options (in addition to normal CLI parameters):
 *	-c	create the data after the widget
 *	-s SPOS	location for the scrollbar
 *	-t TEXT	title for the widget
 *
 */
int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkscreen		= 0;
   CDKSELECTION *selection	= 0;
   WINDOW *cursesWin		= 0;
   char *title			= "<C></5>Pick one or more accounts.";
   char *choices[]		= {"   ", "-->"};
   char **item = 0;
   char temp[256], *mesg[200];
   struct passwd *ent;
   unsigned x, y;
   unsigned used = 0;
   unsigned count = 0;

   CDK_PARAMS params;

   CDKparseParams(argc, argv, &params, "cs:t:" CDK_CLI_PARAMS);

   /* Use the account names to create a list. */
   count = 0;
   while ((ent = getpwent ()) != 0)
   {
      used = CDKallocStrings(&item, ent->pw_name, count++, used);
   }
   endpwent();
   count--;

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Set up CDK Colors. */
   initCDKColor();

   /* Create the selection list. */
   selection = newCDKSelection (cdkscreen,
				CDKparamValue(&params, 'X', CENTER),
				CDKparamValue(&params, 'Y', CENTER),
				CDKparsePosition(CDKparamString2(&params, 's', "RIGHT")),
				CDKparamValue(&params, 'H', 10),
				CDKparamValue(&params, 'W', 50),
				CDKparamString2(&params, 't', title),
				CDKparamNumber(&params, 'c') ? 0 : item,
				CDKparamNumber(&params, 'c') ? 0 : count,
				choices, 2,
				A_REVERSE,
				CDKparamValue(&params, 'N', TRUE),
				CDKparamValue(&params, 'S', FALSE));

   /* Is the selection list null? */
   if (selection == 0)
   {
      /* Exit CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a message and exit. */
      printf ("Oops. Can;t seem to create the selection list. Is the window too small?\n");
      ExitProgram (EXIT_FAILURE);
   }

   if (CDKparamNumber(&params, 'c'))
   {
      setCDKSelectionItems (selection, item, count);
   }

   /* Activate the selection list. */
   activateCDKSelection (selection, 0);

   /* Check the exit status of the widget. */
   if (selection->exitType == vESCAPE_HIT)
   {
      mesg[0] = "<C>You hit escape. No items selected.";
      mesg[1] = "",
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
   }
   else if (selection->exitType == vNORMAL)
   {
      mesg[0] = "<C>Here are the accounts you selected.";
      y = 1;
      for (x=0; x < count; x++)
      {
	 if (selection->selections[x] == 1)
	 {
	    sprintf (temp, "<C></5>%.*s", (int)(sizeof(temp) - 20), item[x]);
	    mesg[y++] = copyChar (temp);
	 }
      }
      popupLabel (cdkscreen, mesg, y);

      /* Clean up. */
      for (x=1; x < y; x++)
      {
	  freeChar (mesg[x]);
      }
   }

   /* Clean up. */
   CDKfreeStrings (item);
   destroyCDKSelection (selection);
   destroyCDKScreen (cdkscreen);
   endCDK();
   ExitProgram (EXIT_SUCCESS);
}
