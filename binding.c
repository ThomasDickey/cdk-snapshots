#include "cdk.h"

/*
 * $Author: glovem $
 * $Date: 1998/04/07 16:19:01 $
 * $Revision: 1.28 $
 */

/*
 * Declare file local prototypes.
 */
int mapChtype (chtype key);

/*
 * This inserts a binding.
 */
void bindCDKObject (EObjectType cdktype, void *object, chtype key, BINDFN function, void * data)
{
   int index = mapChtype (key);

  /*
   * When an alarm is set and this function is entered, a very wild
   * value for the key is provided, and the index gets messed up big time.
   * So we will make sure that index is a valid value before using it.
   */
   if ((index < 0) || (index > MAX_BINDINGS))
   {
      return;
   }

  /*
   * Since dereferencing a void pointer is a no-no, we have to cast
   * our pointer correctly.
    */
   if (cdktype == vENTRY)
   {
      ((CDKENTRY *)object)->bindFunction[index]		= function;
      ((CDKENTRY *)object)->bindData[index]		= data;
   }
   else if (cdktype == vMENTRY)
   {
      ((CDKMENTRY *)object)->bindFunction[index]	= function;
      ((CDKMENTRY *)object)->bindData[index]		= data;
   }
   else if (cdktype == vSCROLL)
   {
      ((CDKSCROLL *)object)->bindFunction[index]	= function;
      ((CDKSCROLL *)object)->bindData[index]		= data;
   }
   else if (cdktype == vDIALOG)
   {
      ((CDKDIALOG *)object)->bindFunction[index]	= function;
      ((CDKDIALOG *)object)->bindData[index]		= data;
   }
   else if (cdktype == vSCALE)
   {
      ((CDKSCALE *)object)->bindFunction[index]		= function;
      ((CDKSCALE *)object)->bindData[index]		= data;
   }
   else if (cdktype == vMENU)
   {
      ((CDKMENU *)object)->bindFunction[index]		= function;
      ((CDKMENU *)object)->bindData[index]		= data;
   }
   else if (cdktype == vMATRIX)
   {
      ((CDKMATRIX *)object)->bindFunction[index]	= function;
      ((CDKMATRIX *)object)->bindData[index]		= data;
   }
   else if (cdktype == vSELECTION)
   {
      ((CDKSELECTION *)object)->bindFunction[index]	= function;
      ((CDKSELECTION *)object)->bindData[index]		= data;
   }
   else if (cdktype == vVIEWER)
   {
      ((CDKVIEWER *)object)->bindFunction[index]	= function;
      ((CDKVIEWER *)object)->bindData[index]		= data;
   }
   else if (cdktype == vRADIO)
   {
      ((CDKRADIO *)object)->bindFunction[index]		= function;
      ((CDKRADIO *)object)->bindData[index]		= data;
   }
   else if (cdktype == vTEMPLATE)
   {
      ((CDKTEMPLATE *)object)->bindFunction[index]	= function;
      ((CDKTEMPLATE *)object)->bindData[index]		= data;
   }
   else if (cdktype == vSWINDOW)
   {
      ((CDKSWINDOW *)object)->bindFunction[index]	= function;
      ((CDKSWINDOW *)object)->bindData[index]		= data;
   }
   else if (cdktype == vITEMLIST)
   {
      ((CDKITEMLIST *)object)->bindFunction[index]	= function;
      ((CDKITEMLIST *)object)->bindData[index]		= data;
   }
   else if (cdktype == vSLIDER)
   {
      ((CDKSLIDER *)object)->bindFunction[index]	= function;
      ((CDKSLIDER *)object)->bindData[index]		= data;
   }
   else if (cdktype == vFSELECT)
   {
      bindCDKObject (vENTRY, ((CDKFSELECT *)object)->entryField, key, function, data);
   }
   else if (cdktype == vALPHALIST)
   {
      bindCDKObject (vENTRY, ((CDKALPHALIST *)object)->entryField, key, function, data);
   }
   else if (cdktype == vCALENDAR)
   {
      ((CDKCALENDAR *)object)->bindFunction[index]	= function;
      ((CDKCALENDAR *)object)->bindData[index]		= data;
   }
   else if (cdktype == vBUTTONBOX)
   {
      ((CDKBUTTONBOX *)object)->bindFunction[index]	= function;
      ((CDKBUTTONBOX *)object)->bindData[index]		= data;
   }
}

/*
 * This removes a binding on an object.
 */
void unbindCDKObject (EObjectType cdktype, void *object, chtype key)
{
   int index = mapChtype(key);

  /*
   * When an alarm is set and this function is entered, a very wild
   * value for the key is provided, and the index gets messed up big time.
   * So we will make sure that index is a valid value before using it.
   */
   if ((index < 0) || (index > MAX_BINDINGS))
   {
      return;
   }

  /*
   * Since dereferencing a void pointer is a no-no, we have to cast
   * our pointer correctly.
   */
   if (cdktype == vENTRY)
   {
      ((CDKENTRY *)object)->bindFunction[index]		= (BINDFN)NULL;
      ((CDKENTRY *)object)->bindData[index]		= (void *)NULL;
   }
   else if (cdktype == vMENTRY)
   {
      ((CDKMENTRY *)object)->bindFunction[index]	= (BINDFN)NULL;
      ((CDKMENTRY *)object)->bindData[index]		= (void *)NULL;
   }
   else if (cdktype == vSCROLL)
   {
      ((CDKSCROLL *)object)->bindFunction[index]	= (BINDFN)NULL;
      ((CDKSCROLL *)object)->bindData[index]		= (void *)NULL;
   }
   else if (cdktype == vDIALOG)
   {
      ((CDKDIALOG *)object)->bindFunction[index]	= (BINDFN)NULL;
      ((CDKDIALOG *)object)->bindData[index]		= (void *)NULL;
   }
   else if (cdktype == vSCALE)
   {
      ((CDKSCALE *)object)->bindFunction[index]		= (BINDFN)NULL;
      ((CDKSCALE *)object)->bindData[index]		= (void *)NULL;
   }
   else if (cdktype == vMENU)
   {
      ((CDKMENU *)object)->bindFunction[index]		= (BINDFN)NULL;
      ((CDKMENU *)object)->bindData[index]		= (void *)NULL;
   }
   else if (cdktype == vMATRIX)
   {
      ((CDKMATRIX *)object)->bindFunction[index]	= (BINDFN)NULL;
      ((CDKMATRIX *)object)->bindData[index]		= (void *)NULL;
   }
   else if (cdktype == vSELECTION)
   {
      ((CDKSELECTION *)object)->bindFunction[index]	= (BINDFN)NULL;
      ((CDKSELECTION *)object)->bindData[index]		= (void *)NULL;
   }
   else if (cdktype == vVIEWER)
   {
      ((CDKVIEWER *)object)->bindFunction[index]	= (BINDFN)NULL;
      ((CDKVIEWER *)object)->bindData[index]		= (void *)NULL;
   }
   else if (cdktype == vRADIO)
   {
      ((CDKRADIO *)object)->bindFunction[index]		= (BINDFN)NULL;
      ((CDKRADIO *)object)->bindData[index]		= (void *)NULL;
   }
   else if (cdktype == vTEMPLATE)
   {
      ((CDKTEMPLATE *)object)->bindFunction[index]	= (BINDFN)NULL;
      ((CDKTEMPLATE *)object)->bindData[index]		= (void *)NULL;
   }
   else if (cdktype == vSWINDOW)
   {
      ((CDKSWINDOW *)object)->bindFunction[index]	= (BINDFN)NULL;
      ((CDKSWINDOW *)object)->bindData[index]		= (void *)NULL;
   }
   else if (cdktype == vITEMLIST)
   {
      ((CDKITEMLIST *)object)->bindFunction[index]	= (BINDFN)NULL;
      ((CDKITEMLIST *)object)->bindData[index]		= (void *)NULL;
   }
   else if (cdktype == vSLIDER)
   {
      ((CDKSLIDER *)object)->bindFunction[index]	= (BINDFN)NULL;
      ((CDKSLIDER *)object)->bindData[index]		= (void *)NULL;
   }
   else if (cdktype == vFSELECT)
   {
      unbindCDKObject (vENTRY, ((CDKFSELECT *)object)->entryField, key);
   }
   else if (cdktype == vALPHALIST)
   {
      unbindCDKObject (vENTRY, ((CDKALPHALIST *)object)->entryField, key);
   }
   else if (cdktype == vCALENDAR)
   {
      ((CDKCALENDAR *)object)->bindFunction[index]	= (BINDFN)NULL;
      ((CDKCALENDAR *)object)->bindData[index]		= (void *)NULL;
   }
   else if (cdktype == vBUTTONBOX)
   {
      ((CDKBUTTONBOX *)object)->bindFunction[index]	= (BINDFN)NULL;
      ((CDKBUTTONBOX *)object)->bindData[index]		= (void *)NULL;
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
   if (cdktype == vENTRY)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKENTRY *)object)->bindFunction[x]		= (BINDFN)NULL;
         ((CDKENTRY *)object)->bindData[x]		= (void *)NULL;
      }
   }
   else if (cdktype == vMENTRY)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKMENTRY *)object)->bindFunction[x]		= (BINDFN)NULL;
         ((CDKMENTRY *)object)->bindData[x]		= (void *)NULL;
      }
   }
   else if (cdktype == vSCROLL)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKSCROLL *)object)->bindFunction[x]		= (BINDFN)NULL;
         ((CDKSCROLL *)object)->bindData[x]		= (void *)NULL;
      }
   }
   else if (cdktype == vDIALOG)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKDIALOG *)object)->bindFunction[x]		= (BINDFN)NULL;
         ((CDKDIALOG *)object)->bindData[x]		= (void *)NULL;
      }
   }
   else if (cdktype == vSCALE)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKSCALE *)object)->bindFunction[x]		= (BINDFN)NULL;
         ((CDKSCALE *)object)->bindData[x]		= (void *)NULL;
      }
   }
   else if (cdktype == vMENU)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKMENU *)object)->bindFunction[x]		= (BINDFN)NULL;
         ((CDKMENU *)object)->bindData[x]		= (void *)NULL;
      }
   }
   else if (cdktype == vMATRIX)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKMATRIX *)object)->bindFunction[x]		= (BINDFN)NULL;
         ((CDKMATRIX *)object)->bindData[x]		= (void *)NULL;
      }
   }
   else if (cdktype == vSELECTION)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKSELECTION *)object)->bindFunction[x]	= (BINDFN)NULL;
         ((CDKSELECTION *)object)->bindData[x]		= (void *)NULL;
      }
   }
   else if (cdktype == vVIEWER)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKVIEWER *)object)->bindFunction[x]		= (BINDFN)NULL;
         ((CDKVIEWER *)object)->bindData[x]		= (void *)NULL;
      }
   }
   else if (cdktype == vRADIO)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKRADIO *)object)->bindFunction[x]		= (BINDFN)NULL;
         ((CDKRADIO *)object)->bindData[x]		= (void *)NULL;
      }
   }
   else if (cdktype == vTEMPLATE)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKTEMPLATE *)object)->bindFunction[x]	= (BINDFN)NULL;
         ((CDKTEMPLATE *)object)->bindData[x]		= (void *)NULL;
      }
   }
   else if (cdktype == vSWINDOW)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKSWINDOW *)object)->bindFunction[x]	= (BINDFN)NULL;
         ((CDKSWINDOW *)object)->bindData[x]		= (void *)NULL;
      }
   }
   else if (cdktype == vITEMLIST)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKITEMLIST *)object)->bindFunction[x]	= (BINDFN)NULL;
         ((CDKITEMLIST *)object)->bindData[x]		= (void *)NULL;
      }
   }
   else if (cdktype == vSLIDER)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKSLIDER *)object)->bindFunction[x]		= (BINDFN)NULL;
         ((CDKSLIDER *)object)->bindData[x]		= (void *)NULL;
      }
   }
   else if (cdktype == vFSELECT)
   {
      cleanCDKObjectBindings (vENTRY, ((CDKFSELECT *)object)->entryField);
      cleanCDKObjectBindings (vSCROLL, ((CDKFSELECT *)object)->scrollField);
   }
   else if (cdktype == vALPHALIST)
   {
      cleanCDKObjectBindings (vENTRY, ((CDKALPHALIST *)object)->entryField);
      cleanCDKObjectBindings (vSCROLL, ((CDKALPHALIST *)object)->scrollField);
   }
   else if (cdktype == vCALENDAR)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKCALENDAR *)object)->bindFunction[x]	= (BINDFN)NULL;
         ((CDKCALENDAR *)object)->bindData[x]		= (void *)NULL;
      }
   }
   else if (cdktype == vBUTTONBOX)
   {
      int x;
      for (x=0; x < MAX_BINDINGS; x++)
      {
         ((CDKBUTTONBOX *)object)->bindFunction[x]	= (BINDFN)NULL;
         ((CDKBUTTONBOX *)object)->bindData[x]		= (void *)NULL;
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
   int index = mapChtype (key);

  /*
   * When an alarm is set and this function is entered, a very wild
   * value for the key is provided, and the index gets messed up big time.
   * So we will make sure that index is a valid value before using it.
   */
   if ((index < 0) || (index > MAX_BINDINGS))
   {
      return (TRUE);
   }

  /*
   * Since dereferencing a void pointer is a no-no, we have to cast
   * our pointers correctly.
   */
   if (cdktype == vENTRY)
   {
      if ( ((CDKENTRY *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= (BINDFN) ((CDKENTRY *)object)->bindFunction[index];
         void * data		= (void *) ((CDKENTRY *)object)->bindData[index];
         return function (vENTRY, object, data, key);
      }
   }
   else if (cdktype == vMENTRY)
   {
      if ( ((CDKMENTRY *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKMENTRY *)object)->bindFunction[index];
         void * data		= ((CDKMENTRY *)object)->bindData[index];
         return function (vMENTRY, object, data, key);
      }
   }
   else if (cdktype == vSCROLL)
   {
      if ( ((CDKSCROLL *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKSCROLL *)object)->bindFunction[index];
         void * data		= ((CDKSCROLL *)object)->bindData[index];
         return function (vSCROLL, object, data, key);
      }
   }
   else if (cdktype == vDIALOG)
   {
      if ( ((CDKDIALOG *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKDIALOG *)object)->bindFunction[index];
         void * data		= ((CDKDIALOG *)object)->bindData[index];
         return function (vDIALOG, object, data, key);
      }
   }
   else if (cdktype == vSCALE)
   {
      if ( ((CDKSCALE *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKSCALE *)object)->bindFunction[index];
         void * data		= ((CDKSCALE *)object)->bindData[index];
         return function (vSCALE, object, data, key);
      }
   }
   else if (cdktype == vMENU)
   {
      if ( ((CDKMENU *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKMENU *)object)->bindFunction[index];
         void * data		= ((CDKMENU *)object)->bindData[index];
         return function (vMENU, object, data, key);
      }
   }
   else if (cdktype == vMATRIX)
   {
      if ( ((CDKMATRIX *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKMATRIX *)object)->bindFunction[index];
         void * data		= ((CDKMATRIX *)object)->bindData[index];
         return function (vMATRIX, object, data, key);
      }
   }
   else if (cdktype == vSELECTION)
   {
      if ( ((CDKSELECTION *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKSELECTION *)object)->bindFunction[index];
         void * data		= ((CDKSELECTION *)object)->bindData[index];
         return function (vSELECTION, object, data, key);
      }
   }
   else if (cdktype == vVIEWER)
   {
      if ( ((CDKVIEWER *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKVIEWER *)object)->bindFunction[index];
         void * data		= ((CDKVIEWER *)object)->bindData[index];
         return function (vVIEWER, object, data, key);
      }
   }
   else if (cdktype == vRADIO)
   {
      if ( ((CDKRADIO *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKRADIO *)object)->bindFunction[index];
         void * data		= ((CDKRADIO *)object)->bindData[index];
         return function (vRADIO, object, data, key);
      }
   }
   else if (cdktype == vTEMPLATE)
   {
      if ( ((CDKTEMPLATE *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKTEMPLATE *)object)->bindFunction[index];
         void * data		= ((CDKTEMPLATE *)object)->bindData[index];
         return function (vTEMPLATE, object, data, key);
      }
   }
   else if (cdktype == vSWINDOW)
   {
      if ( ((CDKSWINDOW *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKSWINDOW *)object)->bindFunction[index];
         void * data		= ((CDKSWINDOW *)object)->bindData[index];
         return function (vSWINDOW, object, data, key);
      }
   }
   else if (cdktype == vITEMLIST)
   {
      if ( ((CDKITEMLIST *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKITEMLIST *)object)->bindFunction[index];
         void * data		= ((CDKITEMLIST *)object)->bindData[index];
         return function (vITEMLIST, object, data, key);
      }
   }
   else if (cdktype == vSLIDER)
   {
      if ( ((CDKSLIDER *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKSLIDER *)object)->bindFunction[index];
         void * data		= ((CDKSLIDER *)object)->bindData[index];
         return function (vSLIDER, object, data, key);
      }
   }
   else if (cdktype == vALPHALIST)
   {
      if ( ((CDKALPHALIST *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKALPHALIST *)object)->bindFunction[index];
         void * data		= ((CDKALPHALIST *)object)->bindData[index];
         return function (vALPHALIST, object, data, key);
      }
   }
   else if (cdktype == vCALENDAR)
   {
      if ( ((CDKCALENDAR *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKCALENDAR *)object)->bindFunction[index];
         void * data		= ((CDKCALENDAR *)object)->bindData[index];
         return function (vCALENDAR, object, data, key);
      }
   }
   else if (cdktype == vBUTTONBOX)
   {
      if ( ((CDKBUTTONBOX *)object)->bindFunction[index] != (BINDFN) NULL )
      {
         BINDFN function	= ((CDKBUTTONBOX *)object)->bindFunction[index];
         void * data		= ((CDKBUTTONBOX *)object)->bindData[index];
         return function (vBUTTONBOX, object, data, key);
      }
   }
   return (FALSE);
}

/*
 * This translates non ascii characters like KEY_UP to an 'equivalent'
 * ascii value.
 */
int mapChtype (chtype key)
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
