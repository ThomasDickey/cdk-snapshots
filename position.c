#include "cdk.h"

/*
 * $Author: tom $
 * $Date: 1999/05/30 00:04:59 $
 * $Revision: 1.1 $
 */

#undef  ObjOf
#define ObjOf(ptr)    (ptr)

/*
 * This allows the user to use the cursor keys to adjust the
 * position of the widget.
 */
void positionCDKObject (CDKOBJS *obj, WINDOW *win)
{
   int origX	= getbegx(win);
   int origY	= getbegy(win);
   chtype key	= (chtype)NULL;

   /* Let them move the widget around until they hit return. */
   while ((key != KEY_RETURN) && (key != KEY_ENTER))
   {
      key = wgetch (win);
      if (key == KEY_UP || key == '8')
      {
         if (getbegy(win) > 0)
         {
            moveCDKObject (obj, 0, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_DOWN || key == '2')
      {
         if (getendy(win) < getmaxy(WindowOf(obj))-1)
         {
            moveCDKObject (obj, 0, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_LEFT || key == '4')
      {
         if (getbegx(win) > 0)
         {
            moveCDKObject (obj, -1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_RIGHT || key == '6')
      {
         if (getendx(win) < getmaxx(WindowOf(obj))-1)
         {
            moveCDKObject (obj, 1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '7')
      {
         if (getbegy(win) > 0 && getbegx(win) > 0)
         {
            moveCDKObject (obj, -1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '9')
      {
         if (getendx(win) < getmaxx(WindowOf(obj))-1
	  && getbegy(win) > 0)
         {
            moveCDKObject (obj, 1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '1')
      {
         if (getbegx(win) > 0 && getendx(win) < getmaxx(WindowOf(obj))-1)
         {
            moveCDKObject (obj, -1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '3')
      {
         if (getendx(win) < getmaxx(WindowOf(obj))-1
	  && getendy(win) < getmaxy(WindowOf(obj))-1)
         {
            moveCDKObject (obj, 1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '5')
      {
         moveCDKObject (obj, CENTER, CENTER, FALSE, TRUE);
      }
      else if (key == 't')
      {
         moveCDKObject (obj, getbegx(win), TOP, FALSE, TRUE);
      }
      else if (key == 'b')
      {
         moveCDKObject (obj, getbegx(win), BOTTOM, FALSE, TRUE);
      }
      else if (key == 'l')
      {
         moveCDKObject (obj, LEFT, getbegy(win), FALSE, TRUE);
      }
      else if (key == 'r')
      {
         moveCDKObject (obj, RIGHT, getbegy(win), FALSE, TRUE);
      }
      else if (key == 'c')
      {
         moveCDKObject (obj, CENTER, getbegy(win), FALSE, TRUE);
      }
      else if (key == 'C')
      {
         moveCDKObject (obj, getbegx(win), CENTER, FALSE, TRUE);
      }
      else if (key == CDK_REFRESH)
      {
         eraseCDKScreen (ScreenOf(obj));
         refreshCDKScreen (ScreenOf(obj));
      }
      else if (key == KEY_ESC)
      {
         moveCDKObject (obj, origX, origY, FALSE, TRUE);
      }
      else if ((key != KEY_RETURN) && (key != KEY_ENTER))
      {
         Beep();
      }
   }
}
