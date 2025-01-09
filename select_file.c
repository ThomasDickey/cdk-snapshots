#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2025/01/09 00:20:21 $
 * $Revision: 1.3 $
 */

/*
 * This allows a person to select a file.
 */
char *selectFile (CDKSCREEN *screen, const char *title)
{
   /* *INDENT-EQLS* */
   CDKFSELECT *fselect  = NULL;
   char *filename       = NULL;
   char *holder         = NULL;

   /* Create the file selector. */
   fselect = newCDKFselect (screen, CENTER, CENTER, -4, -20,
			    title, "File: ",
			    A_NORMAL, '_', A_REVERSE,
			    "</5>", "</48>", "</N>", "</N>",
			    TRUE, FALSE);

   /* Let the user play. */
   holder = activateCDKFselect (fselect, NULL);

   /* Check the way the user exited the selector. */
   if (fselect->exitType != vNORMAL)
   {
      destroyCDKFselect (fselect);
      refreshCDKScreen (screen);
      return (NULL);
   }

   /* Otherwise... */
   filename = copyChar (holder);
   destroyCDKFselect (fselect);
   refreshCDKScreen (screen);
   return (filename);
}
