#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2003/11/27 22:05:01 $
 * $Revision: 1.1 $
 */

/*
 * This allows the user to view a file.
 */
int viewFile (CDKSCREEN *screen, char *title, char *filename, char **buttons, int buttonCount)
{
   CDKVIEWER *viewer	= 0;
   int selected		= -1;
   int lines		= 0;
   char **info		= 0;

   /* Open the file and read the contents. */
   lines = CDKreadFile (filename, &info);

   /* If we couldn't read the file, return an error. */
   if (lines == -1)
   {
      return (lines);
   }

   /* Create the file viewer to view the file selected.*/
   viewer = newCDKViewer (screen, CENTER, CENTER, -6, -16,
				buttons, buttonCount,
				A_REVERSE, TRUE, TRUE);

   /* Set up the viewer title, and the contents to the widget. */
   setCDKViewer (viewer, title, info, lines, A_REVERSE, TRUE, TRUE, TRUE);

   /* Activate the viewer widget. */
   selected = activateCDKViewer (viewer, 0);

   /* Clean up. */
   CDKfreeStrings (info);

   /* Make sure they exited normally. */
   if (viewer->exitType != vNORMAL)
   {
      destroyCDKViewer (viewer);
      return (-1);
   }

   /* Clean up and return the button index selected. */
   destroyCDKViewer (viewer);
   return selected;
}
