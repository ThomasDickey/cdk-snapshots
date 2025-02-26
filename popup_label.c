#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2025/01/09 00:20:21 $
 * $Revision: 1.5 $
 */

/*
 * This pops up a message.
 */
void popupLabel (CDKSCREEN *screen, CDK_CSTRING2 mesg, int count)
{
   CDKLABEL *popup = NULL;
   int oldCursState;
   boolean functionKey;

   /* Create the label. */
   popup = newCDKLabel (screen, CENTER, CENTER, mesg, count, TRUE, FALSE);

   oldCursState = curs_set (0);
   /* Draw it on the screen. */
   drawCDKLabel (popup, TRUE);

   /* Wait for some input. */
   keypad (popup->win, TRUE);
   getchCDKObject (ObjOf (popup), &functionKey);

   /* Kill it. */
   destroyCDKLabel (popup);

   /* Clean the screen. */
   curs_set (oldCursState);
   eraseCDKScreen (screen);
   refreshCDKScreen (screen);
}

/*
 * This pops up a message.
 */
void popupLabelAttrib (CDKSCREEN *screen, CDK_CSTRING2 mesg, int count, chtype attrib)
{
   CDKLABEL *popup = NULL;
   int oldCursState;
   boolean functionKey;

   /* Create the label. */
   popup = newCDKLabel (screen, CENTER, CENTER, mesg, count, TRUE, FALSE);
   setCDKLabelBackgroundAttrib (popup, attrib);

   oldCursState = curs_set (0);
   /* Draw it on the screen. */
   drawCDKLabel (popup, TRUE);

   /* Wait for some input. */
   keypad (popup->win, TRUE);
   getchCDKObject (ObjOf (popup), &functionKey);

   /* Kill it. */
   destroyCDKLabel (popup);

   /* Clean the screen. */
   curs_set (oldCursState);
   eraseCDKScreen (screen);
   refreshCDKScreen (screen);
}
