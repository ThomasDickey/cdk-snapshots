#include <cdk.h>
#include <signal.h>

#ifdef HAVE_SETLOCALE
#include <locale.h>
#endif

/*
 * $Author: tom $
 * $Date: 2003/04/18 21:30:20 $
 * $Revision: 1.69 $
 */

typedef struct _all_screens
{
   struct _all_screens *link;
   CDKSCREEN *screen;
}
ALL_SCREENS;

static ALL_SCREENS *all_screens;

typedef struct _all_objects
{
   struct _all_objects *link;
   CDKOBJS *object;
}
ALL_OBJECTS;

static ALL_OBJECTS *all_objects;

/*
 * This is the function called when we trap a SEGV or a BUS error.
 */
static void segvTrap (int sig)
{
   static int nested;
   if (!nested++)
   {
      endCDK ();
      printf ("core dumped. your fault! (signal %d)\n", sig);
   }
   abort ();
}

static boolean validObjType (CDKOBJS * obj, EObjectType type)
{
   bool valid = FALSE;

   if (obj != 0 && ObjTypeOf (obj) == type)
   {
      switch (type)
      {
      case vALPHALIST:
      case vBUTTON:
      case vBUTTONBOX:
      case vCALENDAR:
      case vDIALOG:
      case vENTRY:
      case vFSCALE:
      case vFSELECT:
      case vGRAPH:
      case vHISTOGRAM:
      case vITEMLIST:
      case vLABEL:
      case vMARQUEE:
      case vMATRIX:
      case vMENTRY:
      case vMENU:
      case vRADIO:
      case vSCALE:
      case vSCROLL:
      case vSELECTION:
      case vSLIDER:
      case vSWINDOW:
      case vTEMPLATE:
      case vVIEWER:
	 valid = TRUE;
	 break;
      case vTRAVERSE:		/* not really an object */
      case vNULL:
	 break;
      }
   }
   return valid;
}

/*
 * Returns true if we have done a "new" on this object but no "destroy"
 */
bool validCDKObject (CDKOBJS * obj)
{
   bool result = FALSE;
   if (obj != 0)
   {
      ALL_OBJECTS *ptr;

      for (ptr = all_objects; ptr != 0; ptr = ptr->link)
      {
	 if (ptr->object == obj)
	 {
	    result = validObjType (obj, ObjTypeOf (obj));
	    break;
	 }
      }
   }
   return result;
}

void *_newCDKObject (unsigned size, const CDKFUNCS * funcs)
{
   ALL_OBJECTS *item;
   CDKOBJS *result = 0;
   if ((item = (ALL_OBJECTS *) calloc (1, sizeof (ALL_OBJECTS))) != 0)
   {
      if ((result = (CDKOBJS *) calloc (1, size)) != 0)
      {
	 result->fn = funcs;
	 result->hasFocus = TRUE;

	 item->link = all_objects;
	 item->object = result;
	 all_objects = item;
      }
      else
      {
	 free (item);
      }
   }
   return (void *)result;
}

void _destroyCDKObject (CDKOBJS * obj)
{
   ALL_OBJECTS *p, *q;

   if (validCDKObject (obj))
   {
      for (p = all_objects, q = 0; p != 0; q = p, p = p->link)
      {
	 if (p->object == obj)
	 {
	    /* delink it first, to avoid problems with recursion */
	    if (q != 0)
	       q->link = p->link;
	    else
	       all_objects = p->link;

	    MethodPtr (obj, destroyObj) (obj);
	    free (obj);
	    break;
	 }
      }
   }
}

/*
 * This creates a new CDK screen.
 */
CDKSCREEN *initCDKScreen (WINDOW *window)
{
   ALL_SCREENS *item;
   CDKSCREEN *screen = 0;

   /* initialization, for the first time */
   if (all_screens == 0)
   {
      /* Set signal trap handlers. */
      signal (SIGSEGV, segvTrap);
      signal (SIGBUS, segvTrap);

      /* Set up basic curses settings. */
#ifdef HAVE_SETLOCALE
      setlocale(LC_ALL, "");
#endif
      noecho ();
      cbreak ();
   }

   if ((item = (ALL_SCREENS *) malloc (sizeof (ALL_SCREENS))) != 0)
   {
      if ((screen = (CDKSCREEN *)calloc (1, sizeof (CDKSCREEN))) != 0)
      {
	 item->link = all_screens;
	 item->screen = screen;
	 all_screens = item;

	 /* Initialize the CDKSCREEN pointer. */
	 screen->objectCount = 0;
	 screen->window = window;

	 /* OK, we are done. */
      }
      else
      {
	 free (item);
      }
   }
   return (screen);
}

/*
 * This registers a CDK object with a screen.
 */
void registerCDKObject (CDKSCREEN *screen, EObjectType cdktype, void *object)
{
   int objectNumber = screen->objectCount;
   CDKOBJS *obj = (CDKOBJS *) object;

   if (objectNumber < MAX_OBJECTS - 1)
   {
      if (validObjType (obj, cdktype))
      {
	 (obj)->screenIndex = objectNumber;
	 (obj)->screen = screen;
	 screen->object[objectNumber] = obj;
	 screen->objectCount++;
      }
   }
}

/*
 * This removes an object from the CDK screen.
 */
void unregisterCDKObject (EObjectType cdktype, void *object)
{
   CDKOBJS *obj = (CDKOBJS *) object;
   /* Declare some vars. */
   CDKSCREEN *screen;
   int Index, x;

   if (validObjType (obj, cdktype) && obj->screenIndex >= 0)
   {
      screen = (obj)->screen;
      Index = (obj)->screenIndex;
      obj->screenIndex = -1;

      /*
       * If this is the last object -1 then this is the last. If not
       * we have to shuffle all the other objects to the left.
       */
      for (x = Index; x < screen->objectCount - 1; x++)
      {
	 screen->object[x] = screen->object[x + 1];
	 (screen->object[x])->screenIndex = x;
      }

      /* Clear out the last widget on the screen list. */
      x = screen->objectCount--;
      screen->object[x] = 0;
   }
}

/*
 * This 'brings' a CDK object to the top of the stack.
 */
void raiseCDKObject (EObjectType cdktype, void *object)
{
   CDKOBJS *obj = (CDKOBJS *) object;
   CDKOBJS *swapobject;
   int swapindex;
   int toppos;

   if (validObjType (obj, cdktype))
   {
      CDKSCREEN *parent = obj->screen;

      toppos = parent->objectCount - 1;

      swapobject = parent->object[toppos];
      swapindex = (obj)->screenIndex;

      (obj)->screenIndex = toppos;
      parent->object[toppos] = obj;

      (swapobject)->screenIndex = swapindex;
      parent->object[swapindex] = swapobject;
   }
}

/*
 * This 'lowers' an object.
 */
void lowerCDKObject (EObjectType cdktype, void *object)
{
   CDKOBJS *obj = (CDKOBJS *) object;
   CDKOBJS *swapobject;
   int swapindex;
   int toppos;

   if (validObjType (obj, cdktype))
   {
      CDKSCREEN *parent = obj->screen;

      toppos = 0;

      swapobject = parent->object[toppos];
      swapindex = (obj)->screenIndex;

      (obj)->screenIndex = toppos;
      parent->object[toppos] = obj;

      (swapobject)->screenIndex = swapindex;
      parent->object[swapindex] = swapobject;
   }
}

/*
 * This calls refreshCDKScreen. (made consistent with widgets)
 */
void drawCDKScreen (CDKSCREEN *cdkscreen)
{
   refreshCDKScreen (cdkscreen);
}

/*
 * This refreshes all the objects in the screen.
 */
void refreshCDKScreen (CDKSCREEN *cdkscreen)
{
   int objectCount = cdkscreen->objectCount;
   int x;

   /* Refresh the screen. */
   touchwin (cdkscreen->window);
   wrefresh (cdkscreen->window);

   /* We just call the drawObject function. */
   for (x = 0; x < objectCount; x++)
   {
      CDKOBJS *obj = cdkscreen->object[x];

      if (validObjType (obj, ObjTypeOf (obj)))
	 obj->fn->drawObj (obj, obj->box);
   }
}

/*
 * This clears all the objects in the screen.
 */
void eraseCDKScreen (CDKSCREEN *cdkscreen)
{
   int objectCount = cdkscreen->objectCount;
   int x;

   /* We just call the drawObject function. */
   for (x = 0; x < objectCount; x++)
   {
      CDKOBJS *obj = cdkscreen->object[x];

      if (validObjType (obj, ObjTypeOf (obj)))
      {
	 obj->fn->eraseObj (obj);
      }
   }

   /* Refresh the screen. */
   wrefresh (cdkscreen->window);
}

/*
 * Destroy all of the objects on a screen
 */
void destroyCDKScreenObjects (CDKSCREEN *cdkscreen)
{
   int x;

   for (x = 0; x < cdkscreen->objectCount; x++)
   {
      CDKOBJS *obj = cdkscreen->object[x];
      int before = cdkscreen->objectCount;

      if (validObjType (obj, ObjTypeOf (obj)))
      {
	 MethodPtr (obj, eraseObj) (obj);
	 _destroyCDKObject (obj);
	 x -= (cdkscreen->objectCount - before);
      }
   }
}

/*
 * This destroys a CDK screen.
 */
void destroyCDKScreen (CDKSCREEN *screen)
{
   ALL_SCREENS *p, *q;

   for (p = all_screens, q = 0; p != 0; q = p, p = p->link)
   {
      if (screen == p->screen)
      {
	 if (q != 0)
	    q->link = p->link;
	 else
	    all_screens = p->link;
	 free (p);
	 free (screen);
	 break;
      }
   }
}

/*
 * This is added to remain consistent.
 */
void endCDK (void)
{
   /* Turn echoing back on. */
   echo ();

   /* Turn off cbreak. */
   nocbreak ();

   /* End the curses windows. */
   endwin ();

#ifdef HAVE_XCURSES
   XCursesExit ();
#endif
}
