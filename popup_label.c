#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2003/11/27 22:16:35 $
 * $Revision: 1.1 $
 */

/*
 * This pops up a message.
 */
void popupLabel (CDKSCREEN *screen, char **mesg, int count)
{
   /* Declare local variables. */
   CDKLABEL *popup = 0;
   int oldCursState;

   /* Create the label. */
   popup = newCDKLabel (screen, CENTER, CENTER, mesg, count, TRUE, FALSE);

   oldCursState = curs_set(0);
   /* Draw it on the screen. */
   drawCDKLabel (popup, TRUE);

   /* Wait for some input. */
   keypad (popup->win, TRUE);
   getcCDKObject (ObjOf(popup));

   /* Kill it. */
   destroyCDKLabel (popup);

   /* Clean the screen. */
   curs_set(oldCursState);
   eraseCDKScreen (screen);
   refreshCDKScreen (screen);
}

/*
 * This pops up a message.
 */
void popupLabelAttrib (CDKSCREEN *screen, char **mesg, int count, chtype attrib)
{
   /* Declare local variables. */
   CDKLABEL *popup = 0;
   int oldCursState;

   /* Create the label. */
   popup = newCDKLabel (screen, CENTER, CENTER, mesg, count, TRUE, FALSE);
   setCDKLabelBackgroundAttrib(popup, attrib);

   oldCursState = curs_set(0);
   /* Draw it on the screen. */
   drawCDKLabel (popup, TRUE);

   /* Wait for some input. */
   keypad (popup->win, TRUE);
   getcCDKObject (ObjOf(popup));

   /* Kill it. */
   destroyCDKLabel (popup);

   /* Clean the screen. */
   curs_set(oldCursState);
   eraseCDKScreen (screen);
   refreshCDKScreen (screen);
}
