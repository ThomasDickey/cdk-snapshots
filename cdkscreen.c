#include "cdk.h"
#include <signal.h>

/*
 * $Author: glovem $
 * $Date: 1998/03/02 16:31:18 $
 * $Revision: 1.52 $
 */

void segvTrap (int sig);

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
      screen->object[x] = (void *)NULL;
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
   screen->object[objectNumber]		= (void *)object;

  /*
   * Since dereferencing a void pointer is a no-no, we have to cast
   * our pointer correctly.
   */
   switch (cdktype)
   {
      case vALPHALIST:
           ((CDKALPHALIST *)object)->screenIndex	= objectNumber;
           ((CDKALPHALIST *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vALPHALIST;
           break;

      case vBUTTONBOX:
           ((CDKBUTTONBOX *)object)->screenIndex	= objectNumber;
           ((CDKBUTTONBOX *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vBUTTONBOX;
           break;

      case vCALENDAR:
           ((CDKCALENDAR *)object)->screenIndex		= objectNumber;
           ((CDKCALENDAR *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vCALENDAR;
           break;

      case vDIALOG:
           ((CDKDIALOG *)object)->screenIndex		= objectNumber;
           ((CDKDIALOG *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vDIALOG;
           break;
   
      case vENTRY:
           ((CDKENTRY *)object)->screenIndex		= objectNumber;
           ((CDKENTRY *)object)->screen			= screen;
           screen->cdktype[objectNumber]		= vENTRY;
           break;

      case vFSELECT:
           ((CDKFSELECT *)object)->screenIndex		= objectNumber;
           ((CDKFSELECT *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vFSELECT;
           break;

      case vGRAPH:
           ((CDKGRAPH *)object)->screenIndex		= objectNumber;
           ((CDKGRAPH *)object)->screen			= screen;
           screen->cdktype[objectNumber]		= vGRAPH;
           break;

      case vHISTOGRAM:
           ((CDKHISTOGRAM *)object)->screenIndex	= objectNumber;
           ((CDKHISTOGRAM *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vHISTOGRAM;
           break;

      case vITEMLIST:
           ((CDKITEMLIST *)object)->screenIndex		= objectNumber;
           ((CDKITEMLIST *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vITEMLIST;
           break;

      case vLABEL:
           ((CDKLABEL *)object)->screenIndex		= objectNumber;
           ((CDKLABEL *)object)->screen			= screen;
           screen->cdktype[objectNumber]		= vLABEL;
           break;

      case vMARQUEE:
           ((CDKMARQUEE *)object)->screenIndex		= objectNumber;
           ((CDKMARQUEE *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vMARQUEE;
           break;
   
      case vMATRIX:
           ((CDKMATRIX *)object)->screenIndex		= objectNumber;
           ((CDKMATRIX *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vMATRIX;
           break;

      case vMENTRY:
           ((CDKMENTRY *)object)->screenIndex		= objectNumber;
           ((CDKMENTRY *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vMENTRY;
           break;

      case vMENU:
           ((CDKMENU *)object)->screenIndex		= objectNumber;
           ((CDKMENU *)object)->screen			= screen;
           screen->cdktype[objectNumber]		= vMENU;
           break;

      case vRADIO:
           ((CDKRADIO *)object)->screenIndex		= objectNumber;
           ((CDKRADIO *)object)->screen			= screen;
           screen->cdktype[objectNumber]		= vRADIO;
           break;

      case vSCALE:
           ((CDKSCALE *)object)->screenIndex		= objectNumber;
           ((CDKSCALE *)object)->screen			= screen;
           screen->cdktype[objectNumber]		= vSCALE;
           break;
   
      case vSCROLL:
           ((CDKSCROLL *)object)->screenIndex		= objectNumber;
           ((CDKSCROLL *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vSCROLL;
           break;

      case vSELECTION:
           ((CDKSELECTION *)object)->screenIndex	= objectNumber;
           ((CDKSELECTION *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vSELECTION;
           break;

      case vSLIDER:
           ((CDKSLIDER *)object)->screenIndex		= objectNumber;
           ((CDKSLIDER *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vSLIDER;
           break;

      case vSWINDOW:
           ((CDKSWINDOW *)object)->screenIndex		= objectNumber;
           ((CDKSWINDOW *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vSWINDOW;
           break;

      case vTEMPLATE:
           ((CDKTEMPLATE *)object)->screenIndex		= objectNumber;
           ((CDKTEMPLATE *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vTEMPLATE;
           break;

      case vVIEWER:
           ((CDKVIEWER *)object)->screenIndex		= objectNumber;
           ((CDKVIEWER *)object)->screen		= screen;
           screen->cdktype[objectNumber]		= vVIEWER;
           break;

      default:
           return;
           break;
   }

   /* Don't forget to increment the widget count. */
   screen->objectCount++;
}

/*
 * This removes an object from the CDK screen.
 */
void unregisterCDKObject (EObjectType cdktype, void *object)
{
   /* Declare some vars. */
   CDKSCREEN *screen;
   int index, x;

  /*
   * Since dereferencing a void pointer is a no-no, we have to cast
   * our pointer correctly.
   */
   switch (cdktype)
   {
      case vBUTTONBOX:
           screen	= ((CDKBUTTONBOX *)object)->screen;
           index	= ((CDKBUTTONBOX *)object)->screenIndex;
           break;

      case vLABEL:
           screen	= ((CDKLABEL *)object)->screen;
           index	= ((CDKLABEL *)object)->screenIndex;
           break;

      case vENTRY:
           screen	= ((CDKENTRY *)object)->screen;
           index	= ((CDKENTRY *)object)->screenIndex;
           break;

      case vMENTRY:
           screen	= ((CDKMENTRY *)object)->screen;
           index	= ((CDKMENTRY *)object)->screenIndex;
           break;

      case vSCROLL:
           screen	= ((CDKSCROLL *)object)->screen;
           index	= ((CDKSCROLL *)object)->screenIndex;
           break;

      case vDIALOG:
           screen	= ((CDKDIALOG *)object)->screen;
           index	= ((CDKDIALOG *)object)->screenIndex;
           break;

      case vSCALE:
           screen	= ((CDKSCALE *)object)->screen;
           index	= ((CDKSCALE *)object)->screenIndex;
           break;

      case vMARQUEE:
           screen	= ((CDKMARQUEE *)object)->screen;
           index	= ((CDKMARQUEE *)object)->screenIndex;
           break;

      case vMENU:
           screen	= ((CDKMENU *)object)->screen;
           index	= ((CDKMENU *)object)->screenIndex;
           break;

      case vMATRIX:
           screen	= ((CDKMATRIX *)object)->screen;
           index	= ((CDKMATRIX *)object)->screenIndex;
           break;

      case vHISTOGRAM:
           screen	= ((CDKHISTOGRAM *)object)->screen;
           index	= ((CDKHISTOGRAM *)object)->screenIndex;
           break;

      case vSELECTION:
           screen	= ((CDKSELECTION *)object)->screen;
           index	= ((CDKSELECTION *)object)->screenIndex;
           break;

      case vVIEWER:
           screen	= ((CDKVIEWER *)object)->screen;
           index	= ((CDKVIEWER *)object)->screenIndex;
           break;

      case vGRAPH:
           screen	= ((CDKGRAPH *)object)->screen;
           index	= ((CDKGRAPH *)object)->screenIndex;
           break;

      case vRADIO:
           screen	= ((CDKRADIO *)object)->screen;
           index	= ((CDKRADIO *)object)->screenIndex;
           break;

      case vTEMPLATE:
           screen	= ((CDKTEMPLATE *)object)->screen;
           index	= ((CDKTEMPLATE *)object)->screenIndex;
           break;

      case vSWINDOW:
           screen	= ((CDKSWINDOW *)object)->screen;
           index	= ((CDKSWINDOW *)object)->screenIndex;
           break;

      case vITEMLIST:
           screen	= ((CDKITEMLIST *)object)->screen;
           index	= ((CDKITEMLIST *)object)->screenIndex;
           break;

      case vFSELECT:
           screen	= ((CDKFSELECT *)object)->screen;
           index	= ((CDKFSELECT *)object)->screenIndex;
           break;

      case vSLIDER:
           screen	= ((CDKSLIDER *)object)->screen;
           index	= ((CDKSLIDER *)object)->screenIndex;
           break;

      case vALPHALIST:
           screen	= ((CDKALPHALIST *)object)->screen;
           index	= ((CDKALPHALIST *)object)->screenIndex;
           break;

      case vCALENDAR:
           screen	= ((CDKCALENDAR *)object)->screen;
           index	= ((CDKCALENDAR *)object)->screenIndex;
           break;

      default:
           return;
           break;
   }

  /*
   * If this is the last object -1 then this is the last. If not
   * we have to shuffle all the other objects to the left.
   */
   for (x=index; x < screen->objectCount-1; x++)
   {
      cdktype			= screen->cdktype[x+1];
      screen->cdktype[x]	= cdktype;
      screen->object[x]		= screen->object[x+1];

      /* Adjust the widget's screen index. */
      switch (cdktype)
      {
         case vBUTTONBOX:
              ((CDKBUTTONBOX *)screen->object[x])->screenIndex = x;
              break;

         case vLABEL:
              ((CDKLABEL *)screen->object[x])->screenIndex = x;
              break;

         case vENTRY:
              ((CDKENTRY *)screen->object[x])->screenIndex = x;
              break;

         case vMENTRY:
              ((CDKMENTRY *)screen->object[x])->screenIndex = x;
              break;

         case vSCROLL:
              ((CDKSCROLL *)screen->object[x])->screenIndex = x;
              break;

         case vDIALOG:
              ((CDKDIALOG *)screen->object[x])->screenIndex = x;
              break;

         case vSCALE:
              ((CDKSCALE *)screen->object[x])->screenIndex = x;
              break;

         case vMARQUEE:
              ((CDKMARQUEE *)screen->object[x])->screenIndex = x;
              break;

         case vMENU:
              ((CDKMENU *)screen->object[x])->screenIndex = x;
              break;

         case vMATRIX:
              ((CDKMATRIX *)screen->object[x])->screenIndex = x;
              break;

         case vHISTOGRAM:
              ((CDKHISTOGRAM *)screen->object[x])->screenIndex = x;
              break;

         case vSELECTION:
              ((CDKSELECTION *)screen->object[x])->screenIndex = x;
              break;

         case vVIEWER:
              ((CDKVIEWER *)screen->object[x])->screenIndex = x;
              break;

         case vGRAPH:
              ((CDKGRAPH *)screen->object[x])->screenIndex = x;
              break;

         case vRADIO:
              ((CDKRADIO *)screen->object[x])->screenIndex = x;
              break;

         case vTEMPLATE:
              ((CDKTEMPLATE *)screen->object[x])->screenIndex = x;
              break;

         case vSWINDOW:
              ((CDKSWINDOW *)screen->object[x])->screenIndex = x;
              break;

         case vITEMLIST:
              ((CDKITEMLIST *)screen->object[x])->screenIndex = x;
              break;

         case vFSELECT:
              ((CDKFSELECT *)screen->object[x])->screenIndex = x;
              break;

         case vSLIDER:
              ((CDKSLIDER *)screen->object[x])->screenIndex = x;
              break;

         case vALPHALIST:
              ((CDKALPHALIST *)screen->object[x])->screenIndex = x;
              break;

         case vCALENDAR:
              ((CDKCALENDAR *)screen->object[x])->screenIndex = x;
              break;

         default:
              break;
      }
   }

   /* Clear out the last widget on the screen list. */
   screen->object[screen->objectCount]	= (void *)NULL;
   screen->cdktype[screen->objectCount]	= vNULL;
   screen->objectCount--;
}

/*
 * This 'brings' a CDK object to the top of the stack.
 */
void raiseCDKObject (EObjectType cdktype, void *object)
{
   /* Declare some vars. */
   void *swapobject	= (void *)NULL;
   int toppos		= -1;
   int swapindex	= -1;
   EObjectType swaptype;

   /* Should never dereference a void * pointer. */
   if (cdktype == vLABEL)
   {
      toppos		= ((CDKLABEL *)object)->screen->objectCount - 1;
      swapobject	= (void *)((CDKLABEL *)object)->screen->object[toppos];
      swaptype		= ((CDKLABEL *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKLABEL *)object)->screenIndex;

      ((CDKLABEL *)object)->screenIndex			= toppos;
      ((CDKLABEL *)object)->screen->object[toppos]	= (void *)object;
      ((CDKLABEL *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vBUTTONBOX)
   {
      toppos		= ((CDKBUTTONBOX *)object)->screen->objectCount;
      swapobject	= ((CDKBUTTONBOX *)object)->screen->object[toppos];
      swaptype		= ((CDKBUTTONBOX *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKBUTTONBOX *)object)->screenIndex;

 
      ((CDKBUTTONBOX *)object)->screenIndex		= toppos;
      ((CDKBUTTONBOX *)object)->screen->object[toppos]	= (void *)object;
      ((CDKBUTTONBOX *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vENTRY)
   {
      toppos		= ((CDKENTRY *)object)->screen->objectCount;
      swapobject	= ((CDKENTRY *)object)->screen->object[toppos];
      swaptype		= ((CDKENTRY *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKENTRY *)object)->screenIndex;

 
      ((CDKENTRY *)object)->screenIndex			= toppos;
      ((CDKENTRY *)object)->screen->object[toppos]	= (void *)object;
      ((CDKENTRY *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vMENTRY)
   {
      toppos		= ((CDKMENTRY *)object)->screen->objectCount;
      swapobject	= ((CDKMENTRY *)object)->screen->object[toppos];
      swaptype		= ((CDKMENTRY *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKMENTRY *)object)->screenIndex;

      ((CDKMENTRY *)object)->screenIndex		= toppos;
      ((CDKMENTRY *)object)->screen->object[toppos]	= (void *)object;
      ((CDKMENTRY *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vSCROLL)
   {
      toppos		= ((CDKSCROLL *)object)->screen->objectCount;
      swapobject	= ((CDKSCROLL *)object)->screen->object[toppos];
      swaptype		= ((CDKSCROLL *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKSCROLL *)object)->screenIndex;

      ((CDKSCROLL *)object)->screenIndex		= toppos;
      ((CDKSCROLL *)object)->screen->object[toppos]	= (void *)object;
      ((CDKSCROLL *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vDIALOG)
   {
      toppos		= ((CDKDIALOG *)object)->screen->objectCount;
      swapobject	= ((CDKDIALOG *)object)->screen->object[toppos];
      swaptype		= ((CDKDIALOG *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKDIALOG *)object)->screenIndex;

      ((CDKDIALOG *)object)->screenIndex		= toppos;
      ((CDKDIALOG *)object)->screen->object[toppos]	= (void *)object;
      ((CDKDIALOG *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vSCALE)
   {
      toppos		= ((CDKSCALE *)object)->screen->objectCount;
      swapobject	= ((CDKSCALE *)object)->screen->object[toppos];
      swaptype		= ((CDKSCALE *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKSCALE *)object)->screenIndex;

      ((CDKSCALE *)object)->screenIndex			= toppos;
      ((CDKSCALE *)object)->screen->object[toppos]	= (void *)object;
      ((CDKSCALE *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vMARQUEE)
   {
      toppos		= ((CDKMARQUEE *)object)->screen->objectCount;
      swapobject	= ((CDKMARQUEE *)object)->screen->object[toppos];
      swaptype		= ((CDKMARQUEE *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKMARQUEE *)object)->screenIndex;

      ((CDKMARQUEE *)object)->screenIndex		= toppos;
      ((CDKMARQUEE *)object)->screen->object[toppos]	= (void *)object;
      ((CDKMARQUEE *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vMENU)
   {
      toppos		= ((CDKMENU *)object)->screen->objectCount;
      swapobject	= ((CDKMENU *)object)->screen->object[toppos];
      swaptype		= ((CDKMENU *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKMENU *)object)->screenIndex;

      ((CDKMENU *)object)->screenIndex			= toppos;
      ((CDKMENU *)object)->screen->object[toppos]	= (void *)object;
      ((CDKMENU *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vMATRIX)
   {
      toppos		= ((CDKMATRIX *)object)->screen->objectCount;
      swapobject	= ((CDKMATRIX *)object)->screen->object[toppos];
      swaptype		= ((CDKMATRIX *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKMATRIX *)object)->screenIndex;

      ((CDKMATRIX *)object)->screenIndex		= toppos;
      ((CDKMATRIX *)object)->screen->object[toppos]	= (void *)object;
      ((CDKMATRIX *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vHISTOGRAM)
   {
      toppos		= ((CDKHISTOGRAM *)object)->screen->objectCount;
      swapobject	= ((CDKHISTOGRAM *)object)->screen->object[toppos];
      swaptype		= ((CDKHISTOGRAM *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKHISTOGRAM *)object)->screenIndex;

      ((CDKHISTOGRAM *)object)->screenIndex		= toppos;
      ((CDKHISTOGRAM *)object)->screen->object[toppos]	= (void *)object;
      ((CDKHISTOGRAM *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vSELECTION)
   {
      toppos		= ((CDKSELECTION *)object)->screen->objectCount;
      swapobject	= ((CDKSELECTION *)object)->screen->object[toppos];
      swaptype		= ((CDKSELECTION *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKSELECTION *)object)->screenIndex;

      ((CDKSELECTION *)object)->screenIndex		= toppos;
      ((CDKSELECTION *)object)->screen->object[toppos]	= (void *)object;
      ((CDKSELECTION *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vVIEWER)
   {
      toppos		= ((CDKVIEWER *)object)->screen->objectCount;
      swapobject	= ((CDKVIEWER *)object)->screen->object[toppos];
      swaptype		= ((CDKVIEWER *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKVIEWER *)object)->screenIndex;

      ((CDKVIEWER *)object)->screenIndex		= toppos;
      ((CDKVIEWER *)object)->screen->object[toppos]	= (void *)object;
      ((CDKVIEWER *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vGRAPH)
   {
      toppos		= ((CDKGRAPH *)object)->screen->objectCount;
      swapobject	= ((CDKGRAPH *)object)->screen->object[toppos];
      swaptype		= ((CDKGRAPH *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKGRAPH *)object)->screenIndex;

      ((CDKGRAPH *)object)->screenIndex			= toppos;
      ((CDKGRAPH *)object)->screen->object[toppos]	= (void *)object;
      ((CDKGRAPH *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vRADIO)
   {
      toppos		= ((CDKRADIO *)object)->screen->objectCount;
      swapobject	= ((CDKRADIO *)object)->screen->object[toppos];
      swaptype		= ((CDKRADIO *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKRADIO *)object)->screenIndex;

      ((CDKRADIO *)object)->screenIndex			= toppos;
      ((CDKRADIO *)object)->screen->object[toppos]	= (void *)object;
      ((CDKRADIO *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vTEMPLATE)
   {
      toppos		= ((CDKTEMPLATE *)object)->screen->objectCount;
      swapobject	= ((CDKTEMPLATE *)object)->screen->object[toppos];
      swaptype		= ((CDKTEMPLATE *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKTEMPLATE *)object)->screenIndex;

      ((CDKTEMPLATE *)object)->screenIndex		= toppos;
      ((CDKTEMPLATE *)object)->screen->object[toppos]	= (void *)object;
      ((CDKTEMPLATE *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vSWINDOW)
   {
      toppos		= ((CDKSWINDOW *)object)->screen->objectCount;
      swapobject	= ((CDKSWINDOW *)object)->screen->object[toppos];
      swaptype		= ((CDKSWINDOW *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKSWINDOW *)object)->screenIndex;

      ((CDKSWINDOW *)object)->screenIndex		= toppos;
      ((CDKSWINDOW *)object)->screen->object[toppos]	= (void *)object;
      ((CDKSWINDOW *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vITEMLIST)
   {
      toppos		= ((CDKITEMLIST *)object)->screen->objectCount;
      swapobject	= ((CDKITEMLIST *)object)->screen->object[toppos];
      swaptype		= ((CDKITEMLIST *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKITEMLIST *)object)->screenIndex;

      ((CDKITEMLIST *)object)->screenIndex		= toppos;
      ((CDKITEMLIST *)object)->screen->object[toppos]	= (void *)object;
      ((CDKITEMLIST *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vFSELECT)
   {
      toppos		= ((CDKFSELECT *)object)->screen->objectCount;
      swapobject	= ((CDKFSELECT *)object)->screen->object[toppos];
      swaptype		= ((CDKFSELECT *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKFSELECT *)object)->screenIndex;

      ((CDKFSELECT *)object)->screenIndex		= toppos;
      ((CDKFSELECT *)object)->screen->object[toppos]	= (void *)object;
      ((CDKFSELECT *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vSLIDER)
   {
      toppos		= ((CDKSLIDER *)object)->screen->objectCount;
      swapobject	= ((CDKSLIDER *)object)->screen->object[toppos];
      swaptype		= ((CDKSLIDER *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKSLIDER *)object)->screenIndex;

      ((CDKSLIDER *)object)->screenIndex		= toppos;
      ((CDKSLIDER *)object)->screen->object[toppos]	= (void *)object;
      ((CDKSLIDER *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vALPHALIST)
   {
      toppos		= ((CDKALPHALIST *)object)->screen->objectCount;
      swapobject	= ((CDKALPHALIST *)object)->screen->object[toppos];
      swaptype		= ((CDKALPHALIST *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKALPHALIST *)object)->screenIndex;

      ((CDKALPHALIST *)object)->screenIndex		= toppos;
      ((CDKALPHALIST *)object)->screen->object[toppos]	= (void *)object;
      ((CDKALPHALIST *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vCALENDAR)
   {
      toppos		= ((CDKCALENDAR *)object)->screen->objectCount;
      swapobject	= ((CDKCALENDAR *)object)->screen->object[toppos];
      swaptype		= ((CDKCALENDAR *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKCALENDAR *)object)->screenIndex;

      ((CDKCALENDAR *)object)->screenIndex		= toppos;
      ((CDKCALENDAR *)object)->screen->object[toppos]	= (void *)object;
      ((CDKCALENDAR *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else
   {
      return;
   }
   
   /* OK, Lets swap 'em.... */
   if (swaptype == vLABEL)
   {
      ((CDKLABEL *)object)->screenIndex			= swapindex;
      ((CDKLABEL *)object)->screen->object[swapindex]	= swapobject;
      ((CDKLABEL *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vBUTTONBOX)
   {
      ((CDKBUTTONBOX *)object)->screenIndex			= swapindex;
      ((CDKBUTTONBOX *)object)->screen->object[swapindex]	= swapobject;
      ((CDKBUTTONBOX *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vENTRY)
   {
      ((CDKENTRY *)object)->screenIndex			= swapindex;
      ((CDKENTRY *)object)->screen->object[swapindex]	= swapobject;
      ((CDKENTRY *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vMENTRY)
   {
      ((CDKMENTRY *)object)->screenIndex		= swapindex;
      ((CDKMENTRY *)object)->screen->object[swapindex]	= swapobject;
      ((CDKMENTRY *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vSCROLL)
   {
      ((CDKSCROLL *)object)->screenIndex		= swapindex;
      ((CDKSCROLL *)object)->screen->object[swapindex]	= swapobject;
      ((CDKSCROLL *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vDIALOG)
   {
      ((CDKDIALOG *)object)->screenIndex		= swapindex;
      ((CDKDIALOG *)object)->screen->object[swapindex]	= swapobject;
      ((CDKDIALOG *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vSCALE)
   {
      ((CDKSCALE *)object)->screenIndex			= swapindex;
      ((CDKSCALE *)object)->screen->object[swapindex]	= swapobject;
      ((CDKSCALE *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vMARQUEE)
   {
      ((CDKMARQUEE *)object)->screenIndex			= swapindex;
      ((CDKMARQUEE *)object)->screen->object[swapindex]		= swapobject;
      ((CDKMARQUEE *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vMENU)
   {
      ((CDKMENU *)object)->screenIndex			= swapindex;
      ((CDKMENU *)object)->screen->object[swapindex]	= swapobject;
      ((CDKMENU *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vMATRIX)
   {
      ((CDKMATRIX *)object)->screenIndex		= swapindex;
      ((CDKMATRIX *)object)->screen->object[swapindex]	= swapobject;
      ((CDKMATRIX *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vHISTOGRAM)
   {
      ((CDKHISTOGRAM *)object)->screenIndex			= swapindex;
      ((CDKHISTOGRAM *)object)->screen->object[swapindex]	= swapobject;
      ((CDKHISTOGRAM *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vSELECTION)
   {
      ((CDKSELECTION *)object)->screenIndex			= swapindex;
      ((CDKSELECTION *)object)->screen->object[swapindex]	= swapobject;
      ((CDKSELECTION *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vVIEWER)
   {
      ((CDKVIEWER *)object)->screenIndex		= swapindex;
      ((CDKVIEWER *)object)->screen->object[swapindex]	= swapobject;
      ((CDKVIEWER *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vGRAPH)
   {
      ((CDKGRAPH *)object)->screenIndex			= swapindex;
      ((CDKGRAPH *)object)->screen->object[swapindex]	= swapobject;
      ((CDKGRAPH *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vRADIO)
   {
      ((CDKRADIO *)object)->screenIndex			= swapindex;
      ((CDKRADIO *)object)->screen->object[swapindex]	= swapobject;
      ((CDKRADIO *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vTEMPLATE)
   {
      ((CDKTEMPLATE *)object)->screenIndex			= swapindex;
      ((CDKTEMPLATE *)object)->screen->object[swapindex]	= swapobject;
      ((CDKTEMPLATE *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vSWINDOW)
   {
      ((CDKSWINDOW *)object)->screenIndex			= swapindex;
      ((CDKSWINDOW *)object)->screen->object[swapindex]		= swapobject;
      ((CDKSWINDOW *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vITEMLIST)
   {
      ((CDKITEMLIST *)object)->screenIndex			= swapindex;
      ((CDKITEMLIST *)object)->screen->object[swapindex]	= swapobject;
      ((CDKITEMLIST *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vFSELECT)
   {
      ((CDKFSELECT *)object)->screenIndex			= swapindex;
      ((CDKFSELECT *)object)->screen->object[swapindex]		= swapobject;
      ((CDKFSELECT *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vSLIDER)
   {
      ((CDKSLIDER *)object)->screenIndex			= swapindex;
      ((CDKSLIDER *)object)->screen->object[swapindex]		= swapobject;
      ((CDKSLIDER *)object)->screen->cdktype[swapindex]		= swaptype;
   }
   else if (swaptype == vALPHALIST)
   {
      ((CDKALPHALIST *)object)->screenIndex			= swapindex;
      ((CDKALPHALIST *)object)->screen->object[swapindex]	= swapobject;
      ((CDKALPHALIST *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vCALENDAR)
   {
      ((CDKCALENDAR *)object)->screenIndex			= swapindex;
      ((CDKCALENDAR *)object)->screen->object[swapindex]	= swapobject;
      ((CDKCALENDAR *)object)->screen->cdktype[swapindex]	= swaptype;
   }
}
   
/*
 * This 'lowers' an object.
*/
void lowerCDKObject (EObjectType cdktype, void *object)
{
   /* Declare some vars. */
   void *swapobject	= (void *)NULL;
   int toppos		= 0;
   int swapindex	= -1;
   EObjectType swaptype;

   /* Should never dereference a void * pointer. */
   if (cdktype == vLABEL)
   {
      swapobject	= ((CDKLABEL *)object)->screen->object[toppos];
      swaptype		= ((CDKLABEL *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKLABEL *)object)->screenIndex;

      ((CDKLABEL *)object)->screenIndex			= toppos;
      ((CDKLABEL *)object)->screen->object[toppos]	= (void *)object;
      ((CDKLABEL *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vBUTTONBOX)
   {
      swapobject	= ((CDKBUTTONBOX *)object)->screen->object[toppos];
      swaptype		= ((CDKBUTTONBOX *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKBUTTONBOX *)object)->screenIndex;
 
      ((CDKBUTTONBOX *)object)->screenIndex		= toppos;
      ((CDKBUTTONBOX *)object)->screen->object[toppos]	= (void *)object;
      ((CDKBUTTONBOX *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vENTRY)
   {
      swapobject	= ((CDKENTRY *)object)->screen->object[toppos];
      swaptype		= ((CDKENTRY *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKENTRY *)object)->screenIndex;

      ((CDKENTRY *)object)->screenIndex			= toppos;
      ((CDKENTRY *)object)->screen->object[toppos]	= (void *)object;
      ((CDKENTRY *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vMENTRY)
   {
      swapobject	= ((CDKMENTRY *)object)->screen->object[toppos];
      swaptype		= ((CDKMENTRY *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKMENTRY *)object)->screenIndex;

      ((CDKMENTRY *)object)->screenIndex		= toppos;
      ((CDKMENTRY *)object)->screen->object[toppos]	= (void *)object;
      ((CDKMENTRY *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vSCROLL)
   {
      swapobject	= ((CDKSCROLL *)object)->screen->object[toppos];
      swaptype		= ((CDKSCROLL *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKSCROLL *)object)->screenIndex;

      ((CDKSCROLL *)object)->screenIndex		= toppos;
      ((CDKSCROLL *)object)->screen->object[toppos]	= (void *)object;
      ((CDKSCROLL *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vDIALOG)
   {
      swapobject	= ((CDKDIALOG *)object)->screen->object[toppos];
      swaptype		= ((CDKDIALOG *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKDIALOG *)object)->screenIndex;

      ((CDKDIALOG *)object)->screenIndex		= toppos;
      ((CDKDIALOG *)object)->screen->object[toppos]	= (void *)object;
      ((CDKDIALOG *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vSCALE)
   {
      swapobject	= ((CDKSCALE *)object)->screen->object[toppos];
      swaptype		= ((CDKSCALE *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKSCALE *)object)->screenIndex;

      ((CDKSCALE *)object)->screenIndex			= toppos;
      ((CDKSCALE *)object)->screen->object[toppos]	= (void *)object;
      ((CDKSCALE *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vMARQUEE)
   {
      swapobject	= ((CDKMARQUEE *)object)->screen->object[toppos];
      swaptype		= ((CDKMARQUEE *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKMARQUEE *)object)->screenIndex;

      ((CDKMARQUEE *)object)->screenIndex		= toppos;
      ((CDKMARQUEE *)object)->screen->object[toppos]	= (void *)object;
      ((CDKMARQUEE *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vMENU)
   {
      swapobject	= ((CDKMENU *)object)->screen->object[toppos];
      swaptype		= ((CDKMENU *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKMENU *)object)->screenIndex;

      ((CDKMENU *)object)->screenIndex			= toppos;
      ((CDKMENU *)object)->screen->object[toppos]	= (void *)object;
      ((CDKMENU *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vMATRIX)
   {
      swapobject	= ((CDKMATRIX *)object)->screen->object[toppos];
      swaptype		= ((CDKMATRIX *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKMATRIX *)object)->screenIndex;

      ((CDKMATRIX *)object)->screenIndex		= toppos;
      ((CDKMATRIX *)object)->screen->object[toppos]	= (void *)object;
      ((CDKMATRIX *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vHISTOGRAM)
   {
      swapobject	= ((CDKHISTOGRAM *)object)->screen->object[toppos];
      swaptype		= ((CDKHISTOGRAM *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKHISTOGRAM *)object)->screenIndex;

      ((CDKHISTOGRAM *)object)->screenIndex		= toppos;
      ((CDKHISTOGRAM *)object)->screen->object[toppos]	= (void *)object;
      ((CDKHISTOGRAM *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vSELECTION)
   {
      swapobject	= ((CDKSELECTION *)object)->screen->object[toppos];
      swaptype		= ((CDKSELECTION *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKSELECTION *)object)->screenIndex;

      ((CDKSELECTION *)object)->screenIndex		= toppos;
      ((CDKSELECTION *)object)->screen->object[toppos]	= (void *)object;
      ((CDKSELECTION *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vVIEWER)
   {
      swapobject	= ((CDKVIEWER *)object)->screen->object[toppos];
      swaptype		= ((CDKVIEWER *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKVIEWER *)object)->screenIndex;

      ((CDKVIEWER *)object)->screenIndex		= toppos;
      ((CDKVIEWER *)object)->screen->object[toppos]	= (void *)object;
      ((CDKVIEWER *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vGRAPH)
   {
      swapobject	= ((CDKGRAPH *)object)->screen->object[toppos];
      swaptype		= ((CDKGRAPH *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKGRAPH *)object)->screenIndex;

      ((CDKGRAPH *)object)->screenIndex			= toppos;
      ((CDKGRAPH *)object)->screen->object[toppos]	= (void *)object;
      ((CDKGRAPH *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vRADIO)
   {
      swapobject	= ((CDKRADIO *)object)->screen->object[toppos];
      swaptype		= ((CDKRADIO *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKRADIO *)object)->screenIndex;

      ((CDKRADIO *)object)->screenIndex			= toppos;
      ((CDKRADIO *)object)->screen->object[toppos]	= (void *)object;
      ((CDKRADIO *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vTEMPLATE)
   {
      swapobject	= ((CDKTEMPLATE *)object)->screen->object[toppos];
      swaptype		= ((CDKTEMPLATE *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKTEMPLATE *)object)->screenIndex;

      ((CDKTEMPLATE *)object)->screenIndex		= toppos;
      ((CDKTEMPLATE *)object)->screen->object[toppos]	= (void *)object;
      ((CDKTEMPLATE *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vSWINDOW)
   {
      swapobject	= ((CDKSWINDOW *)object)->screen->object[toppos];
      swaptype		= ((CDKSWINDOW *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKSWINDOW *)object)->screenIndex;

      ((CDKSWINDOW *)object)->screenIndex		= toppos;
      ((CDKSWINDOW *)object)->screen->object[toppos]	= (void *)object;
      ((CDKSWINDOW *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vITEMLIST)
   {
      swapobject	= ((CDKITEMLIST *)object)->screen->object[toppos];
      swaptype		= ((CDKITEMLIST *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKITEMLIST *)object)->screenIndex;

      ((CDKITEMLIST *)object)->screenIndex		= toppos;
      ((CDKITEMLIST *)object)->screen->object[toppos]	= (void *)object;
      ((CDKITEMLIST *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vFSELECT)
   {
      swapobject	= ((CDKFSELECT *)object)->screen->object[toppos];
      swaptype		= ((CDKFSELECT *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKFSELECT *)object)->screenIndex;

      ((CDKFSELECT *)object)->screenIndex		= toppos;
      ((CDKFSELECT *)object)->screen->object[toppos]	= (void *)object;
      ((CDKFSELECT *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vSLIDER)
   {
      swapobject	= ((CDKSLIDER *)object)->screen->object[toppos];
      swaptype		= ((CDKSLIDER *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKSLIDER *)object)->screenIndex;

      ((CDKSLIDER *)object)->screenIndex		= toppos;
      ((CDKSLIDER *)object)->screen->object[toppos]	= (void *)object;
      ((CDKSLIDER *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vALPHALIST)
   {
      swapobject	= ((CDKALPHALIST *)object)->screen->object[toppos];
      swaptype		= ((CDKALPHALIST *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKALPHALIST *)object)->screenIndex;

      ((CDKALPHALIST *)object)->screenIndex		= toppos;
      ((CDKALPHALIST *)object)->screen->object[toppos]	= (void *)object;
      ((CDKALPHALIST *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else if (cdktype == vCALENDAR)
   {
      swapobject	= ((CDKCALENDAR *)object)->screen->object[toppos];
      swaptype		= ((CDKCALENDAR *)object)->screen->cdktype[toppos];
      swapindex		= ((CDKCALENDAR *)object)->screenIndex;

      ((CDKCALENDAR *)object)->screenIndex		= toppos;
      ((CDKCALENDAR *)object)->screen->object[toppos]	= (void *)object;
      ((CDKCALENDAR *)object)->screen->cdktype[toppos]	= cdktype;
   }
   else
   {
      return;
   }
   
   /* OK, Lets swap 'em.... */
   if (swaptype == vLABEL)
   {
      ((CDKLABEL *)object)->screenIndex			= swapindex;
      ((CDKLABEL *)object)->screen->object[swapindex]	= swapobject;
      ((CDKLABEL *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vBUTTONBOX)
   {
      ((CDKBUTTONBOX *)object)->screenIndex			= swapindex;
      ((CDKBUTTONBOX *)object)->screen->object[swapindex]	= swapobject;
      ((CDKBUTTONBOX *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vENTRY)
   {
      ((CDKENTRY *)object)->screenIndex			= swapindex;
      ((CDKENTRY *)object)->screen->object[swapindex]	= swapobject;
      ((CDKENTRY *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vMENTRY)
   {
      ((CDKMENTRY *)object)->screenIndex		= swapindex;
      ((CDKMENTRY *)object)->screen->object[swapindex]	= swapobject;
      ((CDKMENTRY *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vSCROLL)
   {
      ((CDKSCROLL *)object)->screenIndex		= swapindex;
      ((CDKSCROLL *)object)->screen->object[swapindex]	= swapobject;
      ((CDKSCROLL *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vDIALOG)
   {
      ((CDKDIALOG *)object)->screenIndex		= swapindex;
      ((CDKDIALOG *)object)->screen->object[swapindex]	= swapobject;
      ((CDKDIALOG *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vSCALE)
   {
      ((CDKSCALE *)object)->screenIndex			= swapindex;
      ((CDKSCALE *)object)->screen->object[swapindex]	= swapobject;
      ((CDKSCALE *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vMARQUEE)
   {
      ((CDKMARQUEE *)object)->screenIndex			= swapindex;
      ((CDKMARQUEE *)object)->screen->object[swapindex]		= swapobject;
      ((CDKMARQUEE *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vMENU)
   {
      ((CDKMENU *)object)->screenIndex			= swapindex;
      ((CDKMENU *)object)->screen->object[swapindex]	= swapobject;
      ((CDKMENU *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vMATRIX)
   {
      ((CDKMATRIX *)object)->screenIndex		= swapindex;
      ((CDKMATRIX *)object)->screen->object[swapindex]	= swapobject;
      ((CDKMATRIX *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vHISTOGRAM)
   {
      ((CDKHISTOGRAM *)object)->screenIndex			= swapindex;
      ((CDKHISTOGRAM *)object)->screen->object[swapindex]	= swapobject;
      ((CDKHISTOGRAM *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vSELECTION)
   {
      ((CDKSELECTION *)object)->screenIndex			= swapindex;
      ((CDKSELECTION *)object)->screen->object[swapindex]	= swapobject;
      ((CDKSELECTION *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vVIEWER)
   {
      ((CDKVIEWER *)object)->screenIndex		= swapindex;
      ((CDKVIEWER *)object)->screen->object[swapindex]	= swapobject;
      ((CDKVIEWER *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vGRAPH)
   {
      ((CDKGRAPH *)object)->screenIndex			= swapindex;
      ((CDKGRAPH *)object)->screen->object[swapindex]	= swapobject;
      ((CDKGRAPH *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vRADIO)
   {
      ((CDKRADIO *)object)->screenIndex			= swapindex;
      ((CDKRADIO *)object)->screen->object[swapindex]	= swapobject;
      ((CDKRADIO *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vTEMPLATE)
   {
      ((CDKTEMPLATE *)object)->screenIndex			= swapindex;
      ((CDKTEMPLATE *)object)->screen->object[swapindex]	= swapobject;
      ((CDKTEMPLATE *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vSWINDOW)
   {
      ((CDKSWINDOW *)object)->screenIndex			= swapindex;
      ((CDKSWINDOW *)object)->screen->object[swapindex]		= swapobject;
      ((CDKSWINDOW *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vITEMLIST)
   {
      ((CDKITEMLIST *)object)->screenIndex			= swapindex;
      ((CDKITEMLIST *)object)->screen->object[swapindex]	= swapobject;
      ((CDKITEMLIST *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vFSELECT)
   {
      ((CDKFSELECT *)object)->screenIndex			= swapindex;
      ((CDKFSELECT *)object)->screen->object[swapindex]		= swapobject;
      ((CDKFSELECT *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vSLIDER)
   {
      ((CDKSLIDER *)object)->screenIndex			= swapindex;
      ((CDKSLIDER *)object)->screen->object[swapindex]		= swapobject;
      ((CDKSLIDER *)object)->screen->cdktype[swapindex]		= swaptype;
   }
   else if (swaptype == vALPHALIST)
   {
      ((CDKALPHALIST *)object)->screenIndex			= swapindex;
      ((CDKALPHALIST *)object)->screen->object[swapindex]	= swapobject;
      ((CDKALPHALIST *)object)->screen->cdktype[swapindex]	= swaptype;
   }
   else if (swaptype == vCALENDAR)
   {
      ((CDKCALENDAR *)object)->screenIndex			= swapindex;
      ((CDKCALENDAR *)object)->screen->object[swapindex]	= swapobject;
      ((CDKCALENDAR *)object)->screen->cdktype[swapindex]	= swaptype;
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
      switch (cdkscreen->cdktype[x])
      {
         case vLABEL:
              {
                 CDKLABEL *label = (CDKLABEL *)(cdkscreen->object[x]);
                 drawCDKLabel (label, label->box);
              }
              break;

         case vBUTTONBOX:
              {
                 CDKBUTTONBOX *buttonbox = (CDKBUTTONBOX *)(cdkscreen->object[x]);
                 drawCDKButtonbox (buttonbox, buttonbox->box);
              }
              break;

         case vENTRY:
              {
                 CDKENTRY *entry = (CDKENTRY *)(cdkscreen->object[x]);
                 drawCDKEntry (entry, entry->box);
              }
              break;

         case vMENTRY:
              {
                 CDKMENTRY *mentry = (CDKMENTRY *)(cdkscreen->object[x]);
                 drawCDKMentry (mentry, mentry->box);
              }
              break;

         case vSCROLL:
              {
                 CDKSCROLL *scroll = (CDKSCROLL *)(cdkscreen->object[x]);
                 drawCDKScroll (scroll, scroll->box);
              }
              break;

         case vDIALOG:
              {
                 CDKDIALOG *dialog = (CDKDIALOG *)(cdkscreen->object[x]);
                 drawCDKDialog (dialog, dialog->box);
              }
              break;

         case vSCALE:
              {
                 CDKSCALE *scale = (CDKSCALE *)(cdkscreen->object[x]);
                 drawCDKScale (scale, scale->box);
              }
              break;

         case vMARQUEE:
              {
                 CDKMARQUEE *marquee = (CDKMARQUEE *)(cdkscreen->object[x]);
                 drawCDKMarquee (marquee, marquee->box);
              }
              break;

         case vMENU:
              {
                 CDKMENU *menu = (CDKMENU *)(cdkscreen->object[x]);
                 drawCDKMenu (menu);
              }
              break;

         case vMATRIX:
              {
                 CDKMATRIX *matrix = (CDKMATRIX *)(cdkscreen->object[x]);
                 drawCDKMatrix (matrix, matrix->boxMatrix);
              }
              break;

         case vHISTOGRAM:
              {
                 CDKHISTOGRAM *histogram = (CDKHISTOGRAM *)(cdkscreen->object[x]);
                 drawCDKHistogram (histogram, histogram->box);
              }
              break;

         case vSELECTION:
              {
                 CDKSELECTION *selection = (CDKSELECTION *)(cdkscreen->object[x]);
                 drawCDKSelection (selection, selection->box);
              }
              break;

         case vVIEWER:
              {
                 CDKVIEWER *viewer = (CDKVIEWER *)(cdkscreen->object[x]);
                 drawCDKViewer (viewer, viewer->box);
              }
              break;

         case vGRAPH:
              {
                 CDKGRAPH *graph = (CDKGRAPH *)(cdkscreen->object[x]);
                 drawCDKGraph (graph, graph->box);
              }
              break;

         case vRADIO:
              {
                 CDKRADIO *radio = (CDKRADIO *)(cdkscreen->object[x]);
                 drawCDKRadio (radio, radio->box);
              }
              break;

         case vTEMPLATE:
              {
                 CDKTEMPLATE *cdktemplate = (CDKTEMPLATE *)(cdkscreen->object[x]);
                 drawCDKTemplate (cdktemplate, cdktemplate->box);
              }
              break;

         case vSWINDOW:
              {
                 CDKSWINDOW *swindow = (CDKSWINDOW *)(cdkscreen->object[x]);
                 drawCDKSwindow (swindow, swindow->box);
              }
              break;

         case vITEMLIST:
              {
                 CDKITEMLIST *itemlist = (CDKITEMLIST *)(cdkscreen->object[x]);
                 drawCDKItemlist (itemlist, itemlist->box);
              }
              break;

         case vFSELECT:
              {
                 CDKFSELECT *fselect = (CDKFSELECT *)(cdkscreen->object[x]);
                 drawCDKFselect (fselect, fselect->box);
              }
              break;

         case vSLIDER:
              {
                 CDKSLIDER *slider = (CDKSLIDER *)(cdkscreen->object[x]);
                 drawCDKSlider (slider, slider->box);
              }
              break;

         case vALPHALIST:
              {
                 CDKALPHALIST *alphalist = (CDKALPHALIST *)(cdkscreen->object[x]);
                 drawCDKAlphalist (alphalist, alphalist->box);
              }
              break;

         case vCALENDAR:
              {
                 CDKCALENDAR *calendar = (CDKCALENDAR *)(cdkscreen->object[x]);
                 drawCDKCalendar (calendar, calendar->box);
              }
              break;

         default:
              break;
      }
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
      switch (cdkscreen->cdktype[x])
      {
         case vALPHALIST:
              eraseCDKAlphalist ((CDKALPHALIST *)cdkscreen->object[x]);
              break;
   
         case vBUTTONBOX:
              eraseCDKButtonbox ((CDKBUTTONBOX *)cdkscreen->object[x]);
              break;
   
         case vCALENDAR:
              eraseCDKCalendar ((CDKCALENDAR *)cdkscreen->object[x]);
              break;
   
         case vDIALOG:
              eraseCDKDialog ((CDKDIALOG *)cdkscreen->object[x]);
              break;
   
         case vENTRY:
              eraseCDKEntry ((CDKENTRY *)cdkscreen->object[x]);
              break;
   
         case vFSELECT:
              eraseCDKFselect ((CDKFSELECT *)cdkscreen->object[x]);
              break;
   
         case vGRAPH:
              eraseCDKGraph ((CDKGRAPH *)cdkscreen->object[x]);
              break;
   
         case vHISTOGRAM:
              eraseCDKHistogram ((CDKHISTOGRAM *)cdkscreen->object[x]);
              break;
   
         case vITEMLIST:
              eraseCDKItemlist ((CDKITEMLIST *)cdkscreen->object[x]);
              break;
   
         case vLABEL:
              eraseCDKLabel ((CDKLABEL *)(cdkscreen->object[x]));
              break;
   
         case vMARQUEE:
              eraseCDKMarquee ((CDKMARQUEE *)cdkscreen->object[x]);
              break;
   
         case vMATRIX:
              eraseCDKMatrix ((CDKMATRIX *)cdkscreen->object[x]);
              break;
   
         case vMENTRY:
              eraseCDKMentry ((CDKMENTRY *)cdkscreen->object[x]);
              break;
   
         case vMENU:
              eraseCDKMenu ((CDKMENU *)cdkscreen->object[x]);
              break;
   
         case vRADIO:
              eraseCDKRadio ((CDKRADIO *)cdkscreen->object[x]);
              break;
   
         case vSCALE:
              eraseCDKScale ((CDKSCALE *)cdkscreen->object[x]);
              break;
   
         case vSCROLL:
              eraseCDKScroll ((CDKSCROLL *)cdkscreen->object[x]);
              break;
   
         case vSELECTION:
              eraseCDKSelection ((CDKSELECTION *)cdkscreen->object[x]);
              break;
   
         case vSLIDER:
              eraseCDKSlider ((CDKSLIDER *)cdkscreen->object[x]);
              break;
   
         case vSWINDOW:
              eraseCDKSwindow ((CDKSWINDOW *)cdkscreen->object[x]);
              break;
   
         case vTEMPLATE:
              eraseCDKTemplate ((CDKTEMPLATE *)cdkscreen->object[x]);
              break;
   
         case vVIEWER:
              eraseCDKViewer ((CDKVIEWER *)cdkscreen->object[x]);
              break;

          default:
              break;
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
void endCDK()
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
void segvTrap (int signal)
{
   endCDK();
   printf ("core dumped. your fault! (signal %d)\n", signal);
}
