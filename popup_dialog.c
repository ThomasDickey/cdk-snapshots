#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2025/01/09 00:20:21 $
 * $Revision: 1.4 $
 */

/*
 * This pops up a dialog box.
 */
int popupDialog (CDKSCREEN *screen, CDK_CSTRING2 mesg, int mesgCount, CDK_CSTRING2 buttons, int buttonCount)
{
   /* Declare local variables. */
   CDKDIALOG *popup	= NULL;
   int choice;

   /* Create the dialog box. */
   popup = newCDKDialog (screen, CENTER, CENTER,
			 mesg, mesgCount,
			 buttons, buttonCount,
			 A_REVERSE, TRUE,
			 TRUE, FALSE);

   /* Activate the dialog box */
   drawCDKDialog (popup, TRUE);

   /* Get the choice. */
   choice = activateCDKDialog (popup, NULL);

   /* Destroy the dialog box. */
   destroyCDKDialog (popup);

   /* Clean the screen. */
   eraseCDKScreen (screen);
   refreshCDKScreen (screen);

   /* Return the choice. */
   return choice;
}
