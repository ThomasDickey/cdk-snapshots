#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2004/09/01 00:20:14 $
 * $Revision: 1.48 $
 */

#ifndef KEY_MAX
#define KEY_MAX 512
#endif

/*
 * This inserts a binding.
 */
void bindCDKObject (EObjectType cdktype, void *object, chtype key, BINDFN function, void * data)
{
   CDKOBJS *obj = (CDKOBJS *)object;

   if (key < KEY_MAX)
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
	 if (key >= 0 && (unsigned) key >= obj->bindingCount)
	 {
	    unsigned next = (key + 1);

	    if (obj->bindingList != 0)
	       obj->bindingList = typeReallocN(CDKBINDING, obj->bindingList, next);
	    else
	       obj->bindingList = typeMallocN(CDKBINDING, next);

	    memset (&(obj->bindingList[obj->bindingCount]), 0,
		    (next - obj->bindingCount) * sizeof(CDKBINDING));
	    obj->bindingCount = next;
	 }

	 if (obj->bindingList != 0)
	 {
	    obj->bindingList[key].bindFunction = function;
	    obj->bindingList[key].bindData = data;
	 }
      }
   }
}

/*
 * This removes a binding on an object.
 */
void unbindCDKObject (EObjectType cdktype, void *object, chtype key)
{
   CDKOBJS *obj = (CDKOBJS *)object;

   if (cdktype == vFSELECT)
   {
      unbindCDKObject (vENTRY, ((CDKFSELECT *)object)->entryField, key);
   }
   else if (cdktype == vALPHALIST)
   {
      unbindCDKObject (vENTRY, ((CDKALPHALIST *)object)->entryField, key);
   }
   else if ((unsigned) key < obj->bindingCount)
   {
      obj->bindingList[key].bindFunction = 0;
      obj->bindingList[key].bindData = 0;
   }
}

/*
 * This removes all the bindings for the given objects.
 */
void cleanCDKObjectBindings (EObjectType cdktype, void *object)
{
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
      unsigned x;
      CDKOBJS *obj = (CDKOBJS *)object;

      for (x=0; x < obj->bindingCount; x++)
      {
	 (obj)->bindingList[x].bindFunction = 0;
	 (obj)->bindingList[x].bindData = 0;
      }
   }
}

/*
 * This checks to see if the binding for the key exists:
 * If it does then it runs the command and returns its value, normally TRUE.
 * If it doesn't it returns a FALSE.  This way we can 'overwrite' coded
 * bindings.
 */
int checkCDKObjectBind (EObjectType cdktype, void *object, chtype key)
{
   CDKOBJS *obj = (CDKOBJS *)object;

   if ((unsigned) key < obj->bindingCount)
   {
      if ((obj)->bindingList[key].bindFunction != 0)
      {
	 BINDFN function = obj->bindingList[key].bindFunction;
	 void * data = obj->bindingList[key].bindData;

	 return function (cdktype, object, data, key);
      }
   }
   return (FALSE);
}

/*
 * This is a dummy function used to ensure that the constant for mapping has
 * a distinct address.
 */
int getcCDKBind (EObjectType cdktype GCC_UNUSED, void * object GCC_UNUSED, void * clientData GCC_UNUSED, chtype input GCC_UNUSED)
{
   return 0;
}

/*
 * Read from the input window, filtering keycodes as needed.
 */
int getcCDKObject (CDKOBJS *obj)
{
   int result = wgetch (InputWindowOf (obj));

   if (result >= 0
       && (unsigned)result < obj->bindingCount
       && obj->bindingList[result].bindFunction == getcCDKBind)
   {
      result = (int)(long)obj->bindingList[result].bindData;
   }
   else
   {
      switch (result)
      {
      case '\r':
      case '\n':
	 result = KEY_ENTER;
	 break;
      case '\t':
	 result = KEY_TAB;
	 break;
      case DELETE:
	 result = KEY_DC;
	 break;
      case '\b':	/* same as CTRL('H'), for ASCII */
	 result = KEY_BACKSPACE;
	 break;
      case CDK_BEGOFLINE:
	 result = KEY_HOME;
	 break;
      case CDK_ENDOFLINE:
	 result = KEY_END;
	 break;
      case CDK_FORCHAR:
	 result = KEY_RIGHT;
	 break;
      case CDK_BACKCHAR:
	 result = KEY_LEFT;
	 break;
      case CDK_NEXT:
	 result = KEY_TAB;
	 break;
      case CDK_PREV:
	 result = KEY_BTAB;
	 break;
      }
   }
   return result;
}
