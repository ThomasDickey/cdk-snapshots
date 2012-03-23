#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2012/03/22 00:38:47 $
 * $Revision: 1.2 $
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
   CDKENTRY *widget     = 0;
   char *value          = 0;

   /* Create the widget. */
   widget = newCDKEntry (screen, CENTER, CENTER, title, label,
			 A_NORMAL, '.', vMIXED, 40, 0,
			 5000, TRUE, FALSE);

   /* Set the default value. */
   setCDKEntryValue (widget, initValue);

   /* Get the string. */
   value = activateCDKEntry (widget, 0);

   /* Make sure they exited normally. */
   if (widget->exitType != vNORMAL)
   {
      destroyCDKEntry (widget);
      return 0;
   }

   /* Return a copy of the string typed in. */
   value = copyChar (getCDKEntryValue (widget));
   destroyCDKEntry (widget);
   return value;
}
