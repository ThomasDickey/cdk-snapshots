#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2025/01/09 00:20:21 $
 * $Revision: 1.4 $
 */

/*
 * This allows the user to view information.
 */
int viewInfo (CDKSCREEN *screen,
	      const char *title,
	      CDK_CSTRING2 info,
	      int count,
	      CDK_CSTRING2 buttons,
	      int buttonCount,
	      boolean interpret)
{
   CDKVIEWER *viewer = NULL;
   int selected;

   /* Create the file viewer to view the file selected. */
   viewer = newCDKViewer (screen, CENTER, CENTER, -6, -16,
			  buttons, buttonCount,
			  A_REVERSE, TRUE, TRUE);

   /* Set up the viewer title, and the contents to the widget. */
   setCDKViewer (viewer, title, info, count, A_REVERSE, interpret, TRUE, TRUE);

   /* Activate the viewer widget. */
   selected = activateCDKViewer (viewer, NULL);

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
