#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2025/01/09 00:20:21 $
 * $Revision: 1.58 $
 *
 * Notes:
 *
 * The cdktype parameter passed to bindCDKObject, etc., is redundant since
 * the object parameter also has the same information.  For compatibility
 * just use it for a sanity check.
 */

#ifndef KEY_MAX
#define KEY_MAX 512
#endif

static CDKOBJS *bindableObject (EObjectType * cdktype, void *object)
{
   CDKOBJS *obj = (CDKOBJS *)object;

   if (obj != NULL && *cdktype == ObjTypeOf (obj))
   {
      if (*cdktype == vFSELECT)
      {
	 *cdktype = vENTRY;
	 object = ((CDKFSELECT *)object)->entryField;
      }
      else if (*cdktype == vALPHALIST)
      {
	 *cdktype = vENTRY;
	 object = ((CDKALPHALIST *)object)->entryField;
      }
   }
   else
   {
      object = NULL;
   }
   return (CDKOBJS *)object;
}

/*
 * This inserts a binding.
 */
void bindCDKObject (EObjectType cdktype,
		    void *object,
		    chtype key,
		    BINDFN function,
		    void *data)
{
   CDKOBJS *obj = bindableObject (&cdktype, object);

   if ((key < KEY_MAX) && obj != NULL)
   {
      if (key != 0 && (unsigned)key >= obj->bindingCount)
      {
	 unsigned next = (unsigned) (key + 1);

	 if (obj->bindingList != NULL)
	    obj->bindingList = typeReallocN (CDKBINDING, obj->bindingList, next);
	 else
	    obj->bindingList = typeMallocN (CDKBINDING, next);

	 memset (&(obj->bindingList[obj->bindingCount]), 0,
		 (next - obj->bindingCount) * sizeof (CDKBINDING));
	 obj->bindingCount = next;
      }

      if (obj->bindingList != NULL)
      {
	 obj->bindingList[key].bindFunction = function;
	 obj->bindingList[key].bindData = data;
      }
   }
}

/*
 * This removes a binding on an object.
 */
void unbindCDKObject (EObjectType cdktype, void *object, chtype key)
{
   CDKOBJS *obj = bindableObject (&cdktype, object);

   if (obj != NULL && ((unsigned)key < obj->bindingCount))
   {
      obj->bindingList[key].bindFunction = NULL;
      obj->bindingList[key].bindData = NULL;
   }
}

/*
 * This removes all the bindings for the given objects.
 */
void cleanCDKObjectBindings (EObjectType cdktype, void *object)
{
   CDKOBJS *obj = bindableObject (&cdktype, object);

   if (obj != NULL && obj->bindingList != NULL)
   {
      unsigned x;

      for (x = 0; x < obj->bindingCount; x++)
      {
	 (obj)->bindingList[x].bindFunction = NULL;
	 (obj)->bindingList[x].bindData = NULL;
      }
      freeAndNull ((obj)->bindingList);
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
   CDKOBJS *obj = bindableObject (&cdktype, object);

   if (obj != NULL && ((unsigned)key < obj->bindingCount))
   {
      if ((obj)->bindingList[key].bindFunction != NULL)
      {
	 BINDFN function = obj->bindingList[key].bindFunction;
	 void *data = obj->bindingList[key].bindData;

	 return function (cdktype, object, data, key);
      }
   }
   return (FALSE);
}

/*
 * This checks to see if the binding for the key exists.
 */
bool isCDKObjectBind (EObjectType cdktype, void *object, chtype key)
{
   bool result = FALSE;
   CDKOBJS *obj = bindableObject (&cdktype, object);

   if (obj != NULL && ((unsigned)key < obj->bindingCount))
   {
      if ((obj)->bindingList[key].bindFunction != NULL)
	 result = TRUE;
   }
   return (result);
}

/*
 * This is a dummy function used to ensure that the constant for mapping has
 * a distinct address.
 */
int getcCDKBind (EObjectType cdktype GCC_UNUSED,
		 void *object GCC_UNUSED,
		 void *clientData GCC_UNUSED,
		 chtype input GCC_UNUSED)
{
   return 0;
}

/*
 * Read from the input window, filtering keycodes as needed.
 */
int getcCDKObject (CDKOBJS *obj)
{
   EObjectType cdktype = ObjTypeOf (obj);
   CDKOBJS *test = bindableObject (&cdktype, obj);
   int result = wgetch (InputWindowOf (obj));

   if (result >= 0
       && test != NULL
       && (unsigned)result < test->bindingCount
       && test->bindingList[result].bindFunction == getcCDKBind)
   {
      result = (int)(long)test->bindingList[result].bindData;
   }
   else if (test == NULL
	    || (unsigned)result >= test->bindingCount
	    || test->bindingList[result].bindFunction == NULL)
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
      case '\b':		/* same as CTRL('H'), for ASCII */
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

/*
 * Use this function rather than getcCDKObject(), since we can extend it to
 * handle wide-characters.
 */
int getchCDKObject (CDKOBJS *obj, boolean *functionKey)
{
   int key = getcCDKObject (obj);
   if (functionKey != NULL)
   {
      *functionKey = (key >= KEY_MIN && key <= KEY_MAX);
   }
   return key;
}
