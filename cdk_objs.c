#include <cdk_int.h>

/*
 * Default method-functions for CDK objects.
 *
 * $Author: tom $
 * $Date: 2003/11/27 21:48:51 $
 * $Revision: 1.6 $
 */

/*
 * Set the object's upper-left-corner line-drawing character.
 */
void
setCdkULchar(CDKOBJS * obj, chtype ch)
{
    obj->ULChar = ch;
}

/*
 * Set the object's upper-right-corner line-drawing character.
 */
void
setCdkURchar(CDKOBJS * obj, chtype ch)
{
    obj->URChar = ch;
}

/*
 * Set the object's lower-left-corner line-drawing character.
 */
void
setCdkLLchar(CDKOBJS * obj, chtype ch)
{
    obj->LLChar = ch;
}

/*
 * Set the object's upper-right-corner line-drawing character.
 */
void
setCdkLRchar(CDKOBJS * obj, chtype ch)
{
    obj->LRChar = ch;
}

/*
 * Set the object's horizontal line-drawing character.
 */
void
setCdkHZchar(CDKOBJS * obj, chtype ch)
{
    obj->HZChar = ch;
}

/*
 * Set the object's vertical line-drawing character.
 */
void
setCdkVTchar(CDKOBJS * obj, chtype ch)
{
    obj->VTChar = ch;
}

/*
 * Set the object's box-attributes.
 */
void
setCdkBXattr(CDKOBJS * obj, chtype ch)
{
    obj->BXAttr = ch;
}

/*
 * Set the widget's title.
 */
int
setCdkTitle(CDKOBJS * obj, char *title, int boxWidth)
{
   if (obj != 0)
   {
      cleanCdkTitle(obj);

      if (title != 0)
      {
	 chtype *holder;
	 char **temp = 0;
	 int titleWidth;
	 int x;
	 int len;
	 int align;
	 int maxWidth = 0;

	 /* We need to split the title on \n. */
	 temp = CDKsplitString (title, '\n');
	 obj->titleLines = CDKcountStrings (temp);

	 obj->title = typeCallocN(chtype *, obj->titleLines + 1);
	 obj->titlePos = typeCallocN(int, obj->titleLines + 1);
	 obj->titleLen = typeCallocN(int, obj->titleLines + 1);

	 if (boxWidth >= 0)
	 {
	    /* We need to determine the widest title line. */
	    for (x=0; x < obj->titleLines; x++)
	    {
	       holder = char2Chtype (temp[x], &len, &align);
	       maxWidth = MAXIMUM (maxWidth, len);
	       freeChtype (holder);
	    }
	    boxWidth = MAXIMUM (boxWidth, maxWidth + 2 * obj->borderSize);
	 }
	 else
	 {
	    boxWidth = - (boxWidth - 1);
	 }

	 /* For each line in the title, convert from char * to chtype * */
	 titleWidth = boxWidth - (2 * obj->borderSize);
	 for (x=0; x < obj->titleLines; x++)
	 {
	    obj->title[x]    = char2Chtype (temp[x], &obj->titleLen[x], &obj->titlePos[x]);
	    obj->titlePos[x] = justifyString (titleWidth, obj->titleLen[x], obj->titlePos[x]);
	 }
	 CDKfreeStrings(temp);
      }
   }
   return boxWidth;
}

/*
 * Draw the widget's title.
 */
void
drawCdkTitle(WINDOW *win, CDKOBJS * obj)
{
   if (obj != 0)
   {
      int x;

      for (x=0; x < obj->titleLines; x++)
      {
	 writeChtype (win,
		      obj->titlePos[x] + obj->borderSize,
		      x + 1,
		      obj->title[x],
		      HORIZONTAL, 0,
		      obj->titleLen[x]);
      }
   }
}

/*
 * Remove storage for the widget's title.
 */
void
cleanCdkTitle(CDKOBJS * obj)
{
   if (obj != 0)
   {
      CDKfreeChtypes(obj->title);
      obj->title = 0;

      if (obj->titlePos != 0)
      {
	 free (obj->titlePos);
	 obj->titlePos = 0;
      }

      if (obj->titleLen != 0)
      {
	 free (obj->titleLen);
	 obj->titlePos = 0;
      }

      obj->titleLines = 0;
   }
}
