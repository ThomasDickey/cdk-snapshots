#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2005/04/24 18:02:27 $
 * $Revision: 1.2 $
 */

/*
 * This allows the user to view a file.
 */
int viewFile (CDKSCREEN *screen, char *title, char *filename, char **buttons, int buttonCount)
{
   int lines		= 0;
   char **info		= 0;

   /* Open the file and read the contents. */
   lines = CDKreadFile (filename, &info);

   /* If we couldn't read the file, return an error. */
   if (lines == -1)
   {
      return (lines);
   }

   return viewInfo(screen, title, info, lines, buttons, buttonCount, TRUE);
}
