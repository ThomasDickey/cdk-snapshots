#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2003/11/30 21:15:51 $
 * $Revision: 1.100 $
 */

/*
 * Declare file local prototypes.
 */
static void drawCDKScaleField (CDKSCALE *scale);

DeclareCDKObjects(SCALE, Scale, setCdk, Int);

/*
 * This function creates a scale widget.
 */
CDKSCALE *newCDKScale (CDKSCREEN *cdkscreen, int xplace, int yplace, char *title, char *label, chtype fieldAttr, int fieldWidth, int start, int low, int high, int inc, int fastinc, boolean Box, boolean shadow)
{
   CDKSCALE *scale	= 0;
   int parentWidth	= getmaxx(cdkscreen->window);
   int parentHeight	= getmaxy(cdkscreen->window);
   int boxHeight;
   int boxWidth;
   int horizontalAdjust, oldWidth;
   int xpos		= xplace;
   int ypos		= yplace;
   int junk;

   if ((scale = newCDKObject(CDKSCALE, &my_funcs)) == 0)
      return (0);

   setCDKScaleBox (scale, Box);

   boxHeight		= (BorderOf(scale) * 2) + 1;
   boxWidth		= fieldWidth + 2*BorderOf(scale);

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
   boxWidth = fieldWidth + 2*BorderOf(scale);

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
   fieldWidth = (fieldWidth > (boxWidth - scale->labelLen - 2*BorderOf(scale)) ? (boxWidth - scale->labelLen - 2*BorderOf(scale)) : fieldWidth);

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
				scale->labelLen,
				ypos + TitleLinesOf(scale) + BorderOf(scale),
				xpos + horizontalAdjust + BorderOf(scale));
   }

   /* Create the scale field window. */
   scale->fieldWin = subwin (scale->win, 1, fieldWidth,
				ypos + TitleLinesOf(scale) + BorderOf(scale),
				xpos + scale->labelLen + horizontalAdjust + BorderOf(scale));
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
   cleanCDKObjectBindings (vSCALE, scale);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vSCALE, scale);

   /* Return the pointer. */
   return (scale);
}

/*
 * This allows the person to use the scale field.
 */
int activateCDKScale (CDKSCALE *scale, chtype *actions)
{
   int ret;

   /* Draw the scale widget. */
   drawCDKScale (scale, ObjOf(scale)->box);

   /* Check if actions is null. */
   if (actions == 0)
   {
      chtype input = 0;
      for (;;)
      {
	 /* Get the input. */
	 input = getcCDKObject (ObjOf(scale));

	 /* Inject the character into the widget. */
	 ret = injectCDKScale (scale, input);
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
	 ret = injectCDKScale (scale, actions[x]);
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
static int _injectCDKScale (CDKOBJS *object, chtype input)
{
   CDKSCALE *scale = (CDKSCALE *)object;
   int ppReturn = 1;
   int ret = unknownInt;
   bool complete = FALSE;

   /* Set the exit type. */
   scale->exitType = vEARLY_EXIT;

   /* Draw the field. */
   drawCDKScaleField (scale);

   /* Check if there is a pre-process function to be called. */
   if (scale->preProcessFunction != 0)
   {
      /* Call the pre-process function. */
      ppReturn = scale->preProcessFunction (vSCALE, scale, scale->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a key binding. */
      if (checkCDKObjectBind(vSCALE, scale, input) != 0)
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
	 scale->postProcessFunction (vSCALE, scale, scale->postProcessData, input);
      }
   }

   if (!complete) {
      drawCDKScaleField (scale);
      scale->exitType = vEARLY_EXIT;
   }

   ResultOf(scale).valueInt = ret;
   return (ret != unknownInt);
}

/*
 * This moves the scale field to the given location.
 */
static void _moveCDKScale (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKSCALE *scale = (CDKSCALE *)object;
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
      drawCDKScale (scale, ObjOf(scale)->box);
   }
}

/*
 * This function draws the scale widget.
 */
static void _drawCDKScale (CDKOBJS *object, boolean Box)
{
   CDKSCALE *scale = (CDKSCALE *)object;

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
   drawCDKScaleField (scale);
}

/*
 * This draws the scale widget.
 */
static void drawCDKScaleField (CDKSCALE *scale)
{
   char temp[256];

   /* Erase the field. */
   werase (scale->fieldWin);

   /* Draw the value in the field. */
   sprintf (temp, "%d", scale->current);
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
void setCDKScaleBackgroundColor (CDKSCALE *scale, char *color)
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
   setCDKScaleBackgroundAttrib (scale, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This sets the background attribute of the widget.
 */
void setCDKScaleBackgroundAttrib (CDKSCALE *scale, chtype attrib)
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
static void _destroyCDKScale (CDKOBJS *object)
{
   CDKSCALE *scale = (CDKSCALE *)object;

   cleanCdkTitle (object);
   freeChtype (scale->label);

   /* Clean up the windows. */
   deleteCursesWindow (scale->fieldWin);
   deleteCursesWindow (scale->labelWin);
   deleteCursesWindow (scale->shadowWin);
   deleteCursesWindow (scale->win);

   /* Unregister this object. */
   unregisterCDKObject (vSCALE, scale);
}

/*
 * This function erases the scale widget from the screen.
 */
static void _eraseCDKScale (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKSCALE *scale = (CDKSCALE *)object;

      eraseCursesWindow (scale->labelWin);
      eraseCursesWindow (scale->fieldWin);
      eraseCursesWindow (scale->win);
      eraseCursesWindow (scale->shadowWin);
   }
}

/*
 * These functions set specific attributes of the widget.
 */
void setCDKScale (CDKSCALE *scale, int low, int high, int value, boolean Box)
{
   setCDKScaleLowHigh (scale, low, high);
   setCDKScaleValue (scale, value);
   setCDKScaleBox (scale, Box);
}

/*
 * This sets the low and high values of the scale widget.
 */
void setCDKScaleLowHigh (CDKSCALE *scale, int low, int high)
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
int getCDKScaleLowValue (CDKSCALE *scale)
{
   return scale->low;
}
int getCDKScaleHighValue (CDKSCALE *scale)
{
   return scale->high;
}

/*
 * This sets the scale value.
 */
void setCDKScaleValue (CDKSCALE *scale, int value)
{
   if ((value >= scale->low) && (value <= scale->high))
   {
      scale->current = value;
   }
}
int getCDKScaleValue (CDKSCALE *scale)
{
   return scale->current;
}

/*
 * This sets the scale box attribute.
 */
void setCDKScaleBox (CDKSCALE *scale, boolean Box)
{
   ObjOf(scale)->box = Box;
   ObjOf(scale)->borderSize = Box ? 1 : 0;
}
boolean getCDKScaleBox (CDKSCALE *scale)
{
   return ObjOf(scale)->box;
}

/*
 * This function sets the pre-process function.
 */
void setCDKScalePreProcess (CDKSCALE *scale, PROCESSFN callback, void *data)
{
   scale->preProcessFunction = callback;
   scale->preProcessData = data;
}

/*
 * This function sets the post-process function.
 */
void setCDKScalePostProcess (CDKSCALE *scale, PROCESSFN callback, void *data)
{
   scale->postProcessFunction = callback;
   scale->postProcessData = data;
}

static void _focusCDKScale(CDKOBJS *object GCC_UNUSED)
{
   /* FIXME */
}

static void _unfocusCDKScale(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _refreshDataCDKScale(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _saveDataCDKScale(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}
