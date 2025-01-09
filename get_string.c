#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2025/01/09 00:20:21 $
 * $Revision: 1.4 $
 */

/*
 * This gets information from a user.
 */
char *getString (CDKSCREEN *screen,
		 const char *title,
		 const char *label,
		 const char *initValue)
{
   /* *INDENT-EQLS* */
   CDKENTRY *widget     = NULL;
   char *value          = NULL;

   /* Create the widget. */
   widget = newCDKEntry (screen, CENTER, CENTER, title, label,
			 A_NORMAL, '.', vMIXED, 40, 0,
			 5000, TRUE, FALSE);

   /* Set the default value. */
   setCDKEntryValue (widget, initValue);

   /* Get the string. */
   (void)activateCDKEntry (widget, NULL);

   /* Make sure they exited normally. */
   if (widget->exitType != vNORMAL)
   {
      destroyCDKEntry (widget);
      return NULL;
   }

   /* Return a copy of the string typed in. */
   value = copyChar (getCDKEntryValue (widget));
   destroyCDKEntry (widget);
   return value;
}
