/* $Id: traverse_ex.c,v 1.2 2002/07/09 19:03:44 tom Exp $ */

#include "cdk.h"

#ifdef HAVE_XCURSES
char *XCursesProgramName = "entry_ex";
#endif

#define NumElements(a) ((sizeof a)/(sizeof a[0]))

/*
 * This demonstrates the Cdk widget-traversal.
 */
int main (int argc GCC_UNUSED, char **argv GCC_UNUSED)
{
   /* Declare local variables. */
   CDKSCREEN *cdkscreen = NULL;
   CDKENTRY *entry1 = NULL;
   CDKMENU *menu = NULL;
   CDKENTRY *entry2 = NULL;
   CDKENTRY *entry3 = NULL;
   CDKITEMLIST *itemlist1 = NULL;
   CDKSELECTION *selection = NULL;
   WINDOW *cursesWin = NULL;
   char *mesg[3];
   char *months[] =
   {"Jan", "Feb", "Mar", "Apr"};
   char *choices[] =
   {"[ ]", "[*]"};
   char *menulist[MAX_MENU_ITEMS][MAX_SUB_ITEMS] =
   {
      {"a1", "a2", "a3"},
      {"b1", "b2", "b3"},
      {"c1", "c2", "c3"}
   };
   int submenusize[] =
   {3, 3, 3};
   int menuloc[] =
   {LEFT, LEFT, RIGHT};

   /* Set up CDK. */
   cursesWin = initscr ();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start CDK colors. */
   initCDKColor ();


   menu = newCDKMenu (cdkscreen, menulist, 3, submenusize, menuloc,
		      TOP, A_UNDERLINE, A_REVERSE);

   entry1 = newCDKEntry (cdkscreen, CENTER, 2,
			 NULL, "Field 1: ", A_NORMAL, '.', vMIXED,
			 40, 0, 256, FALSE, FALSE);

   entry2 = newCDKEntry (cdkscreen, CENTER, 4,
			 NULL, "Field 2: ", A_NORMAL, '.', vMIXED,
			 40, 0, 256, FALSE, FALSE);

   entry3 = newCDKEntry (cdkscreen, CENTER, 6,
			 NULL, "Field 3: ", A_NORMAL, '.', vMIXED,
			 40, 0, 256, FALSE, FALSE);

   itemlist1 = newCDKItemlist (cdkscreen, CENTER, 8,
			       NULL, "Month ", months, NumElements (months),
			       1, FALSE, FALSE);

   selection = newCDKSelection (cdkscreen, RIGHT, CENTER, NONE,
				8, 20, NULL,
				months, NumElements (months),
				choices, NumElements (choices),
				A_REVERSE, TRUE, FALSE);

   /* Draw the screen. */
   refreshCDKScreen (cdkscreen);

   /* Traverse the screen */
   traverseCDKScreen (cdkscreen);

   mesg[0] = "Done";
   mesg[1] = "";
   mesg[2] = "<C>Press any key to continue.";
   popupLabel (cdkscreen, mesg, 3);

   /* Clean up and exit. */
   destroyCDKEntry (entry1);
   destroyCDKEntry (entry2);
   destroyCDKEntry (entry3);
   destroyCDKScreen (cdkscreen);
   delwin (cursesWin);
   endCDK ();
   exit (0);
}
