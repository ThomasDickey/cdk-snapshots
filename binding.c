#include "cdk.h"

/*
 * $Author: tom $
 * $Date: 1999/05/23 02:42:46 $
 * $Revision: 1.32 $
 */

/*
 * Declare file local prototypes.
 */
static int mapChtype (chtype key);

/*
 * This inserts a binding.
 */
void bindCDKObject (EObjectType cdktype, void *object, chtype key, BINDFN function, void * data)
{
   int Index = mapChtype (key);
   CDKOBJS *obj = (CDKOBJS *)object;

  /*
   * When an alarm is set and this function is entered, a very wild
   * value for the key is provided, and the index gets messed up big time.
   * So we will make sure that index is a valid value before using it.
   */
   if ((Index >= 0) && (Index < MAX_BINDINGS))
   {
      if (cdktype == vFSELECT)
      {
	 bindCDKObject (vENTRY, ((CDKFSELECT *)object)->entryField, key, function, data);
      }
      else if (cdktype == vALPHALIST)
      {
	 bindCDKObject (vENTRY, ((CDKALPHALIST *)object)->entryField, key, function, data);
      }
      else
      {
	 if (Index >= obj->bindingCount)
	 {
	    unsigned need = (Index + 1) * sizeof(CDKBINDING);

	    if (obj->bindingList != 0)
	       obj->bindingList = (CDKBINDING *)realloc(obj->bindingList, need);
	    else
	       obj->bindingList = (CDKBINDING *)malloc(need);

	    obj->bindingCount = Index + 1;
	 }

	 if (obj->bindingList != 0)
	 {
	    obj->bindingList[Index].bindFunction = function;
	    obj->bindingList[Index].bindData	 = data;
	 }
      }
   }
}

/*
 * This removes a binding on an object.
 */
void unbindCDKObject (EObjectType cdktype, void *object, chtype key)
{
   int Index = mapChtype(key);
   CDKOBJS *obj = (CDKOBJS *)object;

  /*
   * When an alarm is set and this function is entered, a very wild
   * value for the key is provided, and the index gets messed up big time.
   * So we will make sure that index is a valid value before using it.
   */
   if (cdktype == vFSELECT)
   {
      unbindCDKObject (vENTRY, ((CDKFSELECT *)object)->entryField, key);
   }
   else if (cdktype == vALPHALIST)
   {
      unbindCDKObject (vENTRY, ((CDKALPHALIST *)object)->entryField, key);
   }
   else if (Index >= 0 && Index < obj->bindingCount)
   {
      obj->bindingList[Index].bindFunction	= (BINDFN)NULL;
      obj->bindingList[Index].bindData		= (void *)NULL;
   }
}

/*
 * This sets all the bindings for the given objects.
 */
void cleanCDKObjectBindings (EObjectType cdktype, void *object)
{
  /*
   * Since dereferencing a void pointer is a no-no, we have to cast
   * our pointer correctly.
   */
   if (cdktype == vFSELECT)
   {
      cleanCDKObjectBindings (vENTRY, ((CDKFSELECT *)object)->entryField);
      cleanCDKObjectBindings (vSCROLL, ((CDKFSELECT *)object)->scrollField);
   }
   else if (cdktype == vALPHALIST)
   {
      cleanCDKObjectBindings (vENTRY, ((CDKALPHALIST *)object)->entryField);
      cleanCDKObjectBindings (vSCROLL, ((CDKALPHALIST *)object)->scrollField);
   }
   else
   {
      int x;
      CDKOBJS *obj = (CDKOBJS *)object;
      for (x=0; x < obj->bindingCount; x++)
      {
         (obj)->bindingList[x].bindFunction	= (BINDFN)NULL;
         (obj)->bindingList[x].bindData		= (void *)NULL;
      }
   }
}

/*
 * This checks to see iof the binding for the key exists. If it does then it
 * runs the command and returns a TRUE. If it doesn't it returns a FALSE. This
 * way we can 'overwrite' coded bindings.
 */
int checkCDKObjectBind (EObjectType cdktype, void *object, chtype key)
{
   int Index = mapChtype (key);
   CDKOBJS *obj = (CDKOBJS *)object;

  /*
   * When an alarm is set and this function is entered, a very wild
   * value for the key is provided, and the index gets messed up big time.
   * So we will make sure that index is a valid value before using it.
   */
   if ((Index >= 0) && (Index < obj->bindingCount))
   {
      if ( (obj)->bindingList[Index].bindFunction != (BINDFN) NULL )
      {
         BINDFN function	= obj->bindingList[Index].bindFunction;
         void * data		= obj->bindingList[Index].bindData;
         return function (cdktype, object, data, key);
      }
   }
   return (FALSE);
}

/*
 * This translates non ascii characters like KEY_UP to an 'equivalent'
 * ascii value.
 */
static int mapChtype (chtype key)
{
   if (key == KEY_UP)
   {
       return (257);
   }
   else if (key == KEY_DOWN)
   {
       return (258);
   }
   else if (key == KEY_LEFT)
   {
       return (259);
   }
   else if (key == KEY_RIGHT)
   {
       return (260);
   }
   else if (key == KEY_NPAGE)
   {
       return (261);
   }
   else if (key == KEY_PPAGE)
   {
       return (262);
   }
   else if (key == KEY_HOME)
   {
       return (263);
   }
   else if (key == KEY_END)
   {
       return (264);
   }
   else if (key == KEY_F0)
   {
       return (265);
   }
   else if (key == KEY_F1)
   {
       return (266);
   }
   else if (key == KEY_F2)
   {
       return (267);
   }
   else if (key == KEY_F3)
   {
       return (268);
   }
   else if (key == KEY_F4)
   {
       return (269);
   }
   else if (key == KEY_F5)
   {
       return (270);
   }
   else if (key == KEY_F6)
   {
       return (271);
   }
   else if (key == KEY_F7)
   {
       return (272);
   }
   else if (key == KEY_A1)
   {
       return (273);
   }
   else if (key == KEY_A3)
   {
       return (274);
   }
   else if (key == KEY_B2)
   {
       return (275);
   }
   else if (key == KEY_C1)
   {
       return (276);
   }
   else if (key == KEY_C3)
   {
       return (277);
   }
   else if (key == KEY_ESC)
   {
       return (278);
   }
   else
   {
      return ((char) key);
   }
}
