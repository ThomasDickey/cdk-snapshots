#include "cdk.h"

/*
 * $Author: tom $
 * $Date: 2002/07/17 22:59:36 $
 * $Revision: 1.7 $
 */

#define KEY_CTRLX '\030'
#define KEY_CTRLL '\014'
#define KEY_CTRLR '\022'

static void handleMenu (CDKOBJS * menu)
{
   int done = 0;
   FocusObj (menu);

   while (!done)
   {
      int key = getcCDKObject (menu);

      switch (key)
      {
      case KEY_TAB:
      case KEY_ESC:
	 {
	    done = 1;
	    break;
	 }
      default:
	 {
	    int m = injectCDKMenu ((CDKMENU *) menu, key);
	    done = (m >= 0);
	    break;
	 }
      }
   }
   UnfocusObj (menu);
}

/*
 * Save data in widgets on a screen
 */
static void saveDataCDKScreen (CDKSCREEN *screen)
{
   int i;

   for (i = 0; i < screen->objectCount; ++i)
      SaveDataObj (screen->object[i]);
}

/*
 * Refresh data in widgets on a screen
 */
static void refreshDataCDKScreen (CDKSCREEN *screen)
{
   int i;

   for (i = 0; i < screen->objectCount; ++i)
      RefreshDataObj (screen->object[i]);
}


/*
 * ======================================================================
 * Public Interface
 */


void resetCDKScreen (CDKSCREEN *screen)
{
   refreshDataCDKScreen (screen);
}

void exitOKCDKScreen (CDKSCREEN *screen)
{
   screen->exitStatus = CDKSCREEN_EXITOK;
}

void exitCancelCDKScreen (CDKSCREEN *screen)
{
   screen->exitStatus = CDKSCREEN_EXITCANCEL;
}

void exitOKCDKScreenOf (CDKOBJS * obj)
{
   exitOKCDKScreen (obj->screen);
}

void exitCancelCDKScreenOf (CDKOBJS * obj)
{
   exitCancelCDKScreen (obj->screen);
}

void resetCDKScreenOf (CDKOBJS * obj)
{
   resetCDKScreen (obj->screen);
}

/*
 * Traverse the widgets on a screen
 */
int traverseCDKScreen (CDKSCREEN *screen)
{
   int i;			/* current object */

   if (screen->objectCount < 1)
      return 0;

   refreshDataCDKScreen (screen);

   i = -1;
   do
   {
      ++i;
      if (i >= screen->objectCount)
	 return 0;		/* no widgets on this screen accept focus */
   }
   while (!AcceptsFocusObj (screen->object[i]));

   FocusObj (screen->object[i]);

   screen->exitStatus = CDKSCREEN_NOEXIT;

   while (screen->exitStatus == CDKSCREEN_NOEXIT)
   {
      int key = getcCDKObject (screen->object[i]);

      switch (key)
      {
      case KEY_BTAB:
	 {
	    UnfocusObj (screen->object[i]);
	    do
	    {
	       --i;
	       if (i < 0)
		  i = screen->objectCount - 1;
	    }
	    while (!AcceptsFocusObj (screen->object[i]));
	    FocusObj (screen->object[i]);
	    break;
	 }
      case KEY_TAB:
	 {
	    UnfocusObj (screen->object[i]);
	    do
	    {
	       ++i;
	       if (i >= screen->objectCount)
		  i = 0;
	    }
	    while (!AcceptsFocusObj (screen->object[i]));
	    FocusObj (screen->object[i]);
	    break;
	 }
      case KEY_F10:
	 {
	    /* save data and exit */
	    exitOKCDKScreen (screen);
	    break;
	 }
      case KEY_CTRLX:
	 {
	    exitCancelCDKScreen (screen);
	    break;
	 }
      case KEY_CTRLR:
	 {
	    /* reset data to defaults */
	    resetCDKScreen (screen);
	    FocusObj (screen->object[i]);
	    break;
	 }
      case KEY_CTRLL:
	 {
	    /* redraw screen */
	    refreshCDKScreen (screen);
	    FocusObj (screen->object[i]);
	    break;
	 }
      case KEY_ESC:
	 {
	    /* find and enable drop down menu */
	    int j;

	    for (j = 0; j < screen->objectCount; ++j)
	       if (ObjTypeOf(screen->object[j]) == vMENU)
	       {
		  handleMenu (screen->object[j]);
		  FocusObj (screen->object[i]);
		  break;
	       }
	    break;
	 }
      default:
	 {
	    InjectObj (screen->object[i], key);
	    break;
	 }
      }
   }

   if (screen->exitStatus == CDKSCREEN_EXITOK)
   {
      saveDataCDKScreen (screen);
      return 1;
   }
   else
      return 0;
}
