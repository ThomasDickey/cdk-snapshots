#include "cdk.h"

#ifdef HAVE_XCURSES
char *XCursesProgramName="buttonbox_ex";
#endif

int entryCB (EObjectType cdktype, void *object, void *clientData, chtype key);

/*
 * This program demonstrates the Cdk buttonbox widget.
 */
int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkscreen		= (CDKSCREEN *)NULL;
   CDKBUTTONBOX *buttonWidget	= (CDKBUTTONBOX *)NULL;
   CDKENTRY *entry		= (CDKENTRY *)NULL;
   WINDOW *cursesWin		= (WINDOW *)NULL;
   char *buttons[]		= {" OK ", " Cancel "};
   char *info			= (char *)NULL;
   int selection;

   /* Set up CDK. */ 
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start color. */
   initCDKColor();

   /* Create the entry widget. */
   entry = newCDKEntry (cdkscreen, CENTER, CENTER,
			"<C>Enter a name", "Name ", A_NORMAL, '.', vMIXED,
			40, 0, 256, TRUE, FALSE);

   /* Create the button box widget. */
   buttonWidget = newCDKButtonbox (cdkscreen,
			WIN_XPOS (entry->win),
			WIN_YPOS (entry->win) + entry->boxHeight - 1,
			1, entry->boxWidth - 1,
			NULL, 1, 2,
			buttons, 2, A_REVERSE,
			TRUE, FALSE);

   /* Set the lower left and right characters of the box. */
   setCDKEntryLLChar (entry, ACS_LTEE);
   setCDKEntryLRChar (entry, ACS_RTEE);
   setCDKButtonboxULChar (buttonWidget, ACS_LTEE);
   setCDKButtonboxURChar (buttonWidget, ACS_RTEE);

  /*
   * Bind the Tab key in the entry field to send a
   * Tab key to the button box widget.
   */
   bindCDKObject (vENTRY, entry, KEY_TAB, entryCB, buttonWidget);

   /* Activate the entry field. */
   drawCDKButtonbox (buttonWidget, TRUE);
   info = copyChar (activateCDKEntry (entry, NULL));
   selection = buttonWidget->currentButton;

   /* Clean up. */
   destroyCDKButtonbox (buttonWidget);
   destroyCDKEntry (entry);
   destroyCDKScreen (cdkscreen);
   endCDK();
   delwin (cursesWin);

   /* Spit out some info. */
   printf ("You typed in (%s) and selected button (%s)\n", info, buttons[selection]);
   exit (0);
}

int entryCB (EObjectType cdktype, void *object, void *clientData, chtype key)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)clientData;
   injectCDKButtonbox (buttonbox, key);
   return 0;
}
