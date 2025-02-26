/* $Id: buttonbox_ex.c,v 1.22 2025/01/09 00:20:21 tom Exp $ */

#include <cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "buttonbox_ex";
#endif

static BINDFN_PROTO (entryCB);

/*
 * This program demonstrates the Cdk buttonbox widget.
 */
int main (void)
{
   /* *INDENT-EQLS* */
   CDKSCREEN *cdkscreen         = NULL;
   CDKBUTTONBOX *buttonWidget   = NULL;
   CDKENTRY *entry              = NULL;
   const char *buttons[]        =
   {
      " OK ",
      " Cancel "
   };
   char *info                   = NULL;
   int selection;

   cdkscreen = initCDKScreen (NULL);

   /* Start color. */
   initCDKColor ();

   /* Create the entry widget. */
   entry = newCDKEntry (cdkscreen, CENTER, CENTER,
			"<C>Enter a name", "Name ", A_NORMAL, '.', vMIXED,
			40, 0, 256, TRUE, FALSE);
   if (entry == NULL)
   {
      destroyCDKScreen (cdkscreen);
      endCDK ();

      fprintf (stderr, "Cannot create entry-widget\n");
      ExitProgram (EXIT_FAILURE);
   }


   /* Create the button box widget. */
   buttonWidget = newCDKButtonbox (cdkscreen,
				   getbegx (entry->win),
				   getbegy (entry->win) + entry->boxHeight - 1,
				   1, entry->boxWidth - 1,
				   NULL, 1, 2,
				   (CDK_CSTRING2) buttons, 2, A_REVERSE,
				   TRUE, FALSE);
   if (buttonWidget == NULL)
   {
      destroyCDKScreen (cdkscreen);
      endCDK ();

      fprintf (stderr, "Cannot create buttonbox-widget\n");
      ExitProgram (EXIT_FAILURE);
   }

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
   endCDK ();

   printf ("You typed in (%s) and selected button (%s)\n",
	   (info != NULL) ? info : "<null>",
	   buttons[selection]);

   freeChar (info);
   ExitProgram (EXIT_SUCCESS);
}

static int entryCB (EObjectType cdktype GCC_UNUSED,
		    void *object GCC_UNUSED,
		    void *clientData, chtype key)
{
   CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)clientData;
   (void)injectCDKButtonbox (buttonbox, key);
   return (TRUE);
}
