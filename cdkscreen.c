#include <cdk.h>
#include <signal.h>

/*
 * $Author: tom $
 * $Date: 2000/09/23 00:16:19 $
 * $Revision: 1.61 $
 */

static void segvTrap (int sig);

static boolean validObjType(EObjectType type)
{
   switch (type) {
   case vALPHALIST:
   case vBUTTONBOX:
   case vCALENDAR:
   case vDIALOG:
   case vENTRY:
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
      return TRUE;
   default:
      return FALSE;
   }
}

void *_newCDKObject(unsigned size, CDKFUNCS *funcs)
{
   void *result = malloc(size);
   memset(result, 0, sizeof(CDKOBJS));
   ((CDKOBJS *)result)->fn = funcs;
   return result;
}

/*
 * This creates a new CDK screen.
 */
CDKSCREEN *initCDKScreen(WINDOW *window)
{
   CDKSCREEN *screen = (CDKSCREEN *)malloc (sizeof(CDKSCREEN));
   int x;

   /* Set signal trap handlers. */
   signal (SIGSEGV, segvTrap);
   signal (SIGBUS, segvTrap);

   /* Set up basic curses settings. */
   noecho();
   cbreak();

   /* Initialize the CDKSCREEN pointer. */
   screen->objectCount	= 0;
   screen->window	= window;
   for (x=0; x < MAX_OBJECTS; x++)
   {
      screen->object[x] = 0;
      screen->cdktype[x] = vNULL;
   }

   /* OK, we are done. */
   return (screen);
}

/*
 * This registers a CDK object with a screen.
 */
void registerCDKObject (CDKSCREEN *screen, EObjectType cdktype, void *object)
{
   /* Set some basic vars. */
   int objectNumber			= screen->objectCount;
   CDKOBJS *obj				= (CDKOBJS *)object;

   screen->object[objectNumber]		= obj;

   if (validObjType(cdktype)) {
      (obj)->screenIndex		= objectNumber;
      (obj)->screen			= screen;
      screen->cdktype[objectNumber]	= cdktype;
      screen->objectCount++;
   }
}

/*
 * This removes an object from the CDK screen.
 */
void unregisterCDKObject (EObjectType cdktype, void *object)
{
   /* Declare some vars. */
   CDKSCREEN *screen;
   int Index, x;

   if (validObjType(cdktype)) {
      CDKOBJS *obj = (CDKOBJS *)object;

      screen	= (obj)->screen;
      Index	= (obj)->screenIndex;

      /*
       * If this is the last object -1 then this is the last. If not
       * we have to shuffle all the other objects to the left.
       */
      for (x=Index; x < screen->objectCount-1; x++)
      {
	 cdktype		= screen->cdktype[x+1];
	 screen->cdktype[x]	= cdktype;
	 screen->object[x]	= screen->object[x+1];
	 (screen->object[x])->screenIndex = x;
      }

      /* Clear out the last widget on the screen list. */
      x = screen->objectCount--;
      screen->object[x]		= 0;
      screen->cdktype[x]	= vNULL;
   }
}

/*
 * This 'brings' a CDK object to the top of the stack.
 */
void raiseCDKObject (EObjectType cdktype, void *object)
{
   CDKOBJS *swapobject;
   EObjectType swaptype;
   int swapindex;
   int toppos;

   if (validObjType(cdktype)) {
      CDKOBJS *obj	= (CDKOBJS *)object;
      CDKSCREEN *parent	= obj->screen;

      toppos		= parent->objectCount - 1;

      swapobject	= parent->object[toppos];
      swaptype		= parent->cdktype[toppos];
      swapindex		= (obj)->screenIndex;

      (obj)->screenIndex	= toppos;
      parent->object[toppos]	= obj;
      parent->cdktype[toppos]	= cdktype;

      (swapobject)->screenIndex	= swapindex;
      parent->object[swapindex]	= swapobject;
      parent->cdktype[swapindex] = swaptype;
   }
}

/*
 * This 'lowers' an object.
 */
void lowerCDKObject (EObjectType cdktype, void *object)
{
   CDKOBJS *swapobject;
   EObjectType swaptype;
   int swapindex;
   int toppos;

   if (validObjType(cdktype)) {
      CDKOBJS *obj = (CDKOBJS *)object;
      CDKSCREEN *parent	= obj->screen;

      toppos		= 0;

      swapobject	= parent->object[toppos];
      swaptype		= parent->cdktype[toppos];
      swapindex		= (obj)->screenIndex;

      (obj)->screenIndex	= toppos;
      parent->object[toppos]	= obj;
      parent->cdktype[toppos]	= cdktype;

      (swapobject)->screenIndex	= swapindex;
      parent->object[swapindex]	= swapobject;
      parent->cdktype[swapindex] = swaptype;
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
   for (x=0; x < objectCount; x++)
   {
      CDKOBJS *obj = cdkscreen->object[x];

      if (validObjType (cdkscreen->cdktype[x]))
	 obj->fn->drawObj(obj, obj->box);
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
   for (x=0; x < objectCount; x++)
   {
      if (validObjType (cdkscreen->cdktype[x])) {
	  cdkscreen->object[x]->fn->eraseObj(cdkscreen->object[x]);
      }
   }

   /* Refresh the screen. */
   wrefresh (cdkscreen->window);
}

/*
 * This destroys a CDK screen.
 */
void destroyCDKScreen (CDKSCREEN *screen)
{
   free (screen);
}

/*
 * This is added to remain consistent.
 */
void endCDK(void)
{
   /* Turn echoing back on. */
   echo();

   /* Turn off cbreak. */
   nocbreak();

   /* End the curses windows. */
   endwin();

#ifdef HAVE_XCURSES
   XCursesExit();
#endif
}

/*
 * This is the function called when we trap a SEGV or a BUS error.
 */
static void segvTrap (int sig)
{
   static int nested;
   if (!nested++)
   {
      endCDK();
      printf ("core dumped. your fault! (signal %d)\n", sig);
   }
   abort();
}
