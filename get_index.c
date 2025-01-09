#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2025/01/09 00:20:21 $
 * $Revision: 1.4 $
 */

/*
 * This returns a selected value in a list.
 */
int getListIndex (CDKSCREEN *screen,
		  const char *title,
		  CDK_CSTRING2 list,
		  int listSize,
		  boolean numbers)
{
   /* *INDENT-EQLS* */
   CDKSCROLL *scrollp   = NULL;
   int selected         = -1;
   int height           = 10;
   int width            = -1;
   int len              = 0;
   int x;

   /* Determine the height of the list. */
   if (listSize < 10)
   {
      height = listSize + (title == NULL ? 2 : 3);
   }

   /* Determine the width of the list. */
   for (x = 0; x < listSize; x++)
   {
      int temp = (int)strlen (list[x]) + 10;
      width = MAXIMUM (width, temp);
   }
   if (title != NULL)
   {
      len = (int)strlen (title);
   }
   width = MAXIMUM (width, len);
   width += 5;

   /* Create the scrolling list. */
   scrollp = newCDKScroll (screen, CENTER, CENTER, RIGHT,
			   height, width, title,
			   (CDK_CSTRING2) list, listSize, numbers,
			   A_REVERSE, TRUE, FALSE);

   /* Check if we made the list. */
   if (scrollp == NULL)
   {
      refreshCDKScreen (screen);
      return -1;
   }

   /* Let the user play. */
   selected = activateCDKScroll (scrollp, NULL);

   /* Check how they exited. */
   if (scrollp->exitType != vNORMAL)
   {
      selected = -1;
   }

   /* Clean up. */
   destroyCDKScroll (scrollp);
   refreshCDKScreen (screen);
   return selected;
}
