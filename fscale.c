#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2003/11/30 21:15:51 $
 * $Revision: 1.20 $
 */

/*
 * Declare file local prototypes.
 */
static void drawCDKFScaleField (CDKFSCALE *scale);

DeclareCDKObjects(FSCALE, FScale, setCdk, Float);

/*
 * This function creates a scale widget.
 */
CDKFSCALE *newCDKFScale (CDKSCREEN *cdkscreen, int xplace, int yplace, char *title, char *label, chtype fieldAttr, int fieldWidth, float start, float low, float high, float inc, float fastinc, int digits, boolean Box, boolean shadow)
{
   CDKFSCALE *scale	= 0;
   int parentWidth	= getmaxx(cdkscreen->window);
   int parentHeight	= getmaxy(cdkscreen->window);
   int boxHeight	= 3;
   int boxWidth		= fieldWidth + 2;
   int horizontalAdjust, oldWidth;
   int xpos		= xplace;
   int ypos		= yplace;
   int junk;

   if ((scale = newCDKObject(CDKFSCALE, &my_funcs)) == 0)
      return (0);

   setCDKFScaleBox (scale, Box);

   /* Set some basic values of the scale field. */
   scale->label		= 0;
   scale->labelLen	= 0;
   scale->labelWin	= 0;

  /*
   * If the fieldWidth is a negative value, the fieldWidth will
   * be COLS-fieldWidth, otherwise, the fieldWidth will be the
   * given width.
   */
   fieldWidth = setWidgetDimension (parentWidth, fieldWidth, 0);
   boxWidth = fieldWidth + 2;

   /* Translate the label char *pointer to a chtype pointer. */
   if (label != 0)
   {
      scale->label	= char2Chtype (label, &scale->labelLen, &junk);
      boxWidth		= scale->labelLen + fieldWidth + 2;
   }

   oldWidth = boxWidth;
   boxWidth = setCdkTitle(ObjOf(scale), title, boxWidth);
   horizontalAdjust = (boxWidth - oldWidth) / 2;

   boxHeight += TitleLinesOf(scale);

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);
   fieldWidth = (fieldWidth > (boxWidth - scale->labelLen - 2) ? (boxWidth - scale->labelLen - 2) : fieldWidth);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the scale window. */
   scale->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the main window null??? */
   if (scale->win == 0)
   {
      destroyCDKObject(scale);
      return (0);
   }

   /* Create the scale label window. */
   if (scale->label != 0)
   {
      scale->labelWin = subwin (scale->win, 1,
				scale->labelLen + 2,
				ypos + TitleLinesOf(scale) + 1,
				xpos + horizontalAdjust + 1);
   }

   /* Create the scale field window. */
   scale->fieldWin = subwin (scale->win, 1, fieldWidth,
				ypos + TitleLinesOf(scale) + 1,
				xpos + scale->labelLen + horizontalAdjust + 1);
   keypad (scale->fieldWin, TRUE);
   keypad (scale->win, TRUE);

   /* Create the scale field. */
   ScreenOf(scale)		= cdkscreen;
   ObjOf(scale)->inputWindow	= scale->win;
   scale->parent		= cdkscreen->window;
   scale->shadowWin		= 0;
   scale->boxWidth		= boxWidth;
   scale->boxHeight		= boxHeight;
   scale->fieldWidth		= fieldWidth;
   scale->fieldAttr		= (chtype)fieldAttr;
   scale->current		= low;
   scale->low			= low;
   scale->high			= high;
   scale->current		= start;
   scale->inc			= inc;
   scale->fastinc		= fastinc;
   scale->digits		= digits;
   scale->exitType		= vNEVER_ACTIVATED;
   scale->shadow		= shadow;
   scale->preProcessFunction	= 0;
   scale->preProcessData	= 0;
   scale->postProcessFunction	= 0;
   scale->postProcessData	= 0;

   /* Do we want a shadow??? */
   if (shadow)
   {
      scale->shadowWin	= newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
   }

   /* Clean the key bindings. */
   cleanCDKObjectBindings (vFSCALE, scale);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vFSCALE, scale);

   /* Return the pointer. */
   return (scale);
}

/*
 * This allows the person to use the scale field.
 */
float activateCDKFScale (CDKFSCALE *scale, chtype *actions)
{
   float ret;

   /* Draw the scale widget. */
   drawCDKFScale (scale, ObjOf(scale)->box);

   /* Check if actions is null. */
   if (actions == 0)
   {
      chtype input = 0;
      for (;;)
      {
	 /* Get the input. */
	 input = getcCDKObject (ObjOf(scale));

	 /* Inject the character into the widget. */
	 ret = injectCDKFScale (scale, input);
	 if (scale->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }
   else
   {
      int length = chlen (actions);
      int x = 0;

      /* Inject each character one at a time. */
      for (x=0; x < length; x++)
      {
	 ret = injectCDKFScale (scale, actions[x]);
	 if (scale->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }

   /* Set the exit type and return. */
   scale->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This function injects a single character into the widget.
 */
static int _injectCDKFScale (CDKOBJS *object, chtype input)
{
   CDKFSCALE *scale = (CDKFSCALE *)object;
   int ppReturn = 1;
   float ret = unknownFloat;
   bool complete = FALSE;

   /* Set the exit type. */
   scale->exitType = vEARLY_EXIT;

   /* Draw the field. */
   drawCDKFScaleField (scale);

   /* Check if there is a pre-process function to be called. */
   if (scale->preProcessFunction != 0)
   {
      /* Call the pre-process function. */
      ppReturn = scale->preProcessFunction (vFSCALE, scale, scale->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a key binding. */
      if (checkCDKObjectBind(vFSCALE, scale, input) != 0)
      {
	 scale->exitType = vESCAPE_HIT;
	 complete = TRUE;
      }
      else
      {
	 switch (input)
	 {
	    case KEY_LEFT : case 'd' : case '-' : case KEY_DOWN :
		 if (scale->current > scale->low)
		 {
		    scale->current -= scale->inc;
		    if (scale->current < scale->low)
		    {
			scale->current = scale->low;
		    }
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_RIGHT : case 'u' : case '+' : case KEY_UP :
		 if (scale->current < scale->high)
		 {
		    scale->current += scale->inc;
		    if (scale->current > scale->high)
		    {
		        scale->current = scale->high;
		    }
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_PPAGE : case 'U' : case CONTROL('B') :
		 if ((scale->current + scale->fastinc) <= scale->high)
		 {
		    scale->current += scale->fastinc;
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	       case KEY_NPAGE : case 'D' : case CONTROL('F') :
		 if ((scale->current - scale->fastinc) >= scale->low)
		 {
		    scale->current -= scale->fastinc;
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_HOME : case 'g' : case '0' :
		 scale->current = scale->low;
		 break;

	    case KEY_END : case 'G' : case '$' :
		 scale->current = scale->high;
		 break;

	    case KEY_RETURN : case TAB : case KEY_ENTER :
		 scale->exitType = vNORMAL;
		 ret = (scale->current);
		 complete = TRUE;
		 break;

	    case KEY_ESC :
		 scale->exitType = vESCAPE_HIT;
		 ret = (scale->current);
		 complete = TRUE;
		 break;

	    case CDK_REFRESH :
		eraseCDKScreen (ScreenOf(scale));
		refreshCDKScreen (ScreenOf(scale));
		break;

	    default :
		 break;
	 }
      }

      /* Should we call a post-process? */
      if (!complete && (scale->postProcessFunction != 0))
      {
	 scale->postProcessFunction (vFSCALE, scale, scale->postProcessData, input);
      }
   }

   if (!complete) {
      drawCDKFScaleField (scale);
      scale->exitType = vEARLY_EXIT;
   }

   ResultOf(scale).valueFloat = ret;
   return ret != 0.0;
}

/*
 * This moves the scale field to the given location.
 */
static void _moveCDKFScale (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKFSCALE *scale = (CDKFSCALE *)object;
   int currentX = getbegx(scale->win);
   int currentY = getbegy(scale->win);
   int xpos	= xplace;
   int ypos	= yplace;
   int xdiff	= 0;
   int ydiff	= 0;

   /*
    * If this is a relative move, then we will adjust where we want
    * to move to.
    */
   if (relative)
   {
      xpos = getbegx(scale->win) + xplace;
      ypos = getbegy(scale->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(scale), &xpos, &ypos, scale->boxWidth, scale->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(scale->win, -xdiff, -ydiff);
   moveCursesWindow(scale->labelWin, -xdiff, -ydiff);
   moveCursesWindow(scale->fieldWin, -xdiff, -ydiff);
   moveCursesWindow(scale->shadowWin, -xdiff, -ydiff);

   /* Touch the windows so they 'move'. */
   touchwin (WindowOf(scale));
   wrefresh (WindowOf(scale));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKFScale (scale, ObjOf(scale)->box);
   }
}

/*
 * This function draws the scale widget.
 */
static void _drawCDKFScale (CDKOBJS *object, boolean Box)
{
   CDKFSCALE *scale = (CDKFSCALE *)object;

   /* Draw the shadow. */
   if (scale->shadowWin != 0)
   {
      drawShadow (scale->shadowWin);
   }

   /* Box the widget if asked. */
   if (Box)
   {
      drawObjBox (scale->win, ObjOf(scale));
   }

   drawCdkTitle (scale->win, object);

   /* Draw the label. */
   if (scale->labelWin != 0)
   {
      writeChtype (scale->labelWin, 0, 0,
			scale->label,
			HORIZONTAL, 0,
			scale->labelLen);
   }
   touchwin (scale->win);
   wrefresh (scale->win);

   /* Draw the field window. */
   drawCDKFScaleField (scale);
}

/*
 * This draws the scale widget.
 */
static void drawCDKFScaleField (CDKFSCALE *scale)
{
   char temp[256], format[256];

   /* Erase the field. */
   werase (scale->fieldWin);

   /* Draw the value in the field. */
   sprintf (format, "%%.%if", scale->digits);
   sprintf (temp, format, scale->current);
   writeCharAttrib (scale->fieldWin,
			scale->fieldWidth-(int)strlen(temp)-1, 0, temp,
			scale->fieldAttr,
			HORIZONTAL, 0,
			(int)strlen(temp));

   /* Refresh the field window. */
   touchwin (scale->fieldWin);
   wrefresh (scale->fieldWin);
}

/*
 * This sets the background color of the widget.
 */
void setCDKFScaleBackgroundColor (CDKFSCALE *scale, char *color)
{
   chtype *holder = 0;
   int junk1, junk2;

   /* Make sure the color isn't null. */
   if (color == 0)
   {
      return;
   }

   /* Convert the value of the environment variable to a chtype. */
   holder = char2Chtype (color, &junk1, &junk2);

   /* Set the widgets background color. */
   setCDKFScaleBackgroundAttrib (scale, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This sets the background attribute of the widget.
 */
void setCDKFScaleBackgroundAttrib (CDKFSCALE *scale, chtype attrib)
{
   /* Set the widgets background attribute. */
   wbkgd (scale->win, attrib);
   wbkgd (scale->fieldWin, attrib);
   if (scale->labelWin != 0)
   {
      wbkgd (scale->labelWin, attrib);
   }
}

/*
 * This function destroys the scale widget.
 */
static void _destroyCDKFScale (CDKOBJS *object)
{
   CDKFSCALE *scale = (CDKFSCALE *)object;

   freeChtype (scale->label);
   cleanCdkTitle (object);

   /* Clean up the windows. */
   deleteCursesWindow (scale->fieldWin);
   deleteCursesWindow (scale->labelWin);
   deleteCursesWindow (scale->shadowWin);
   deleteCursesWindow (scale->win);

   /* Unregister this object. */
   unregisterCDKObject (vFSCALE, scale);
}

/*
 * This function erases the scale widget from the screen.
 */
static void _eraseCDKFScale (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKFSCALE *scale = (CDKFSCALE *)object;

      eraseCursesWindow (scale->labelWin);
      eraseCursesWindow (scale->fieldWin);
      eraseCursesWindow (scale->win);
      eraseCursesWindow (scale->shadowWin);
   }
}

/*
 * These functions set specific attributes of the widget.
 */
void setCDKFScale (CDKFSCALE *scale, float low, float high, float value, boolean Box)
{
   setCDKFScaleLowHigh (scale, low, high);
   setCDKFScaleValue (scale, value);
   setCDKFScaleBox (scale, Box);
}

/*
 * This sets the low and high values of the scale widget.
 */
void setCDKFScaleLowHigh (CDKFSCALE *scale, float low, float high)
{
   /* Make sure the values aren't out of bounds. */
   if (low <= high)
   {
      scale->low	= low;
      scale->high	= high;
   }
   else if (low > high)
   {
      scale->low	= high;
      scale->high	= low;
   }
}
float getCDKFScaleLowValue (CDKFSCALE *scale)
{
   return scale->low;
}
float getCDKFScaleHighValue (CDKFSCALE *scale)
{
   return scale->high;
}

/*
 * This sets the digits.
 */
void setCDKFScaleDigits (CDKFSCALE *scale, int digits)
{
    scale->digits = MAXIMUM (0, digits);
}

int getCDKFScaleDigits (CDKFSCALE *scale)
{
    return scale->digits;
}

/*
 * This sets the scale value.
 */
void setCDKFScaleValue (CDKFSCALE *scale, float value)
{
   if ((value >= scale->low) && (value <= scale->high))
   {
      scale->current = value;
   }
}
float getCDKFScaleValue (CDKFSCALE *scale)
{
   return scale->current;
}

/*
 * This sets the scale box attribute.
 */
void setCDKFScaleBox (CDKFSCALE *scale, boolean Box)
{
   ObjOf(scale)->box = Box;
   ObjOf(scale)->borderSize = Box ? 1 : 0;
}
boolean getCDKFScaleBox (CDKFSCALE *scale)
{
   return ObjOf(scale)->box;
}

/*
 * This function sets the pre-process function.
 */
void setCDKFScalePreProcess (CDKFSCALE *scale, PROCESSFN callback, void *data)
{
   scale->preProcessFunction = callback;
   scale->preProcessData = data;
}

/*
 * This function sets the post-process function.
 */
void setCDKFScalePostProcess (CDKFSCALE *scale, PROCESSFN callback, void *data)
{
   scale->postProcessFunction = callback;
   scale->postProcessData = data;
}

static void _focusCDKFScale(CDKOBJS *object GCC_UNUSED)
{
   /* FIXME */
}

static void _unfocusCDKFScale(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _refreshDataCDKFScale(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _saveDataCDKFScale(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}
