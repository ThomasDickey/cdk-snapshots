#include "cdk.h"

#ifdef HAVE_XCURSES
char *XCursesProgramName="buttonbox_ex";
#endif

static BINDFN_PROTO(entryCB);

/*
 * This program demonstrates the Cdk buttonbox widget.
 */
int main (void)
{
   /* Declare variables. */
   CDKSCREEN *cdkscreen		= 0;
   CDKBUTTONBOX *buttonWidget	= 0;
   CDKENTRY *entry		= 0;
   WINDOW *cursesWin		= 0;
   char *buttons[]		= {" OK ", " Cancel "};
   char *info			= 0;
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
			getbegx(entry->win),
			getbegy(entry->win) + entry->boxHeight - 1,
			1, entry->boxWidth - 1,
			0, 1, 2,
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
   info = copyChar (activateCDKEntry (entry, 0));
   selection = buttonWidget->currentButton;

   /* Clean up. */
   destroyCDKButtonbox (buttonWidget);
   destroyCDKEntry (entry);
   destroyCDKScreen (cdkscreen);
   delwin (cursesWin);
   endCDK();

   /* Spit out some info. */
   printf ("You typed in (%s) and selected button (%s)\n", info, buttons[selection]);
   exit (0);
}

static void entryCB (EObjectType cdktype GCC_UNUSED, void *object GCC_UNUSED, void *clientData, chtype key)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)clientData;
   injectCDKButtonbox (buttonbox, key);
}
