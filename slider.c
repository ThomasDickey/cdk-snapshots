#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2003/12/06 16:39:02 $
 * $Revision: 1.63 $
 */

/*
 * Declare file local prototypes.
 */
static void drawCDKSliderField (CDKSLIDER *slider);

DeclareCDKObjects(SLIDER, Slider, setCdk, Int);

/*
 * This function creates a slider widget.
 */
CDKSLIDER *newCDKSlider (CDKSCREEN *cdkscreen, int xplace, int yplace, char *title, char *label, chtype filler, int fieldWidth, int start, int low, int high, int inc, int fastInc, boolean Box, boolean shadow)
{
   CDKSLIDER *slider	= 0;
   int parentWidth	= getmaxx(cdkscreen->window);
   int parentHeight	= getmaxy(cdkscreen->window);
   int boxHeight;
   int boxWidth		= 0;
   int horizontalAdjust, oldWidth;
   int xpos		= xplace;
   int ypos		= yplace;
   int highValueLen	= intlen (high);
   int junk;

   if ((slider = newCDKObject(CDKSLIDER, &my_funcs)) == 0)
      return (0);

   setCDKSliderBox (slider, Box);
   boxHeight		= (BorderOf(slider) * 2) + 1;

   /* Set some basic values of the slider field. */
   slider->label	= 0;
   slider->labelLen	= 0;
   slider->labelWin	= 0;

  /*
   * If the fieldWidth is a negative value, the fieldWidth will
   * be COLS-fieldWidth, otherwise, the fieldWidth will be the
   * given width.
   */
   fieldWidth = setWidgetDimension (parentWidth, fieldWidth, 0);

   /* Translate the label char *pointer to a chtype pointer. */
   if (label != 0)
   {
      slider->label	= char2Chtype (label, &slider->labelLen, &junk);
      boxWidth		= slider->labelLen + fieldWidth + highValueLen + 2 * BorderOf(slider);
   }
   else
   {
      boxWidth = fieldWidth + highValueLen + 2*BorderOf(slider);
   }

   oldWidth = boxWidth;
   boxWidth = setCdkTitle(ObjOf(slider), title, boxWidth);
   horizontalAdjust = (boxWidth - oldWidth) / 2;

   boxHeight += TitleLinesOf(slider);

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);
   fieldWidth = (fieldWidth > (boxWidth-slider->labelLen-highValueLen-1) ?
		 (boxWidth-slider->labelLen-highValueLen-1) : fieldWidth);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the slider window. */
   slider->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the main window null??? */
   if (slider->win == 0)
   {
      destroyCDKObject(slider);
      return (0);
   }
   keypad (slider->win, TRUE);

   /* Create the slider label window. */
   if (slider->label != 0)
   {
      slider->labelWin = subwin (slider->win, 1,
					slider->labelLen,
					ypos + TitleLinesOf(slider) + BorderOf(slider),
					xpos + horizontalAdjust + BorderOf(slider));
      if (slider->labelWin == 0)
      {
	 destroyCDKObject(slider);
	 return (0);
      }
   }

   /* Create the slider field window. */
   slider->fieldWin = subwin (slider->win, 1,
				fieldWidth + highValueLen-1,
				ypos + TitleLinesOf(slider) + BorderOf(slider),
				xpos + slider->labelLen + horizontalAdjust + BorderOf(slider));
   if (slider->fieldWin == 0)
   {
      destroyCDKObject(slider);
      return (0);
   }
   keypad (slider->fieldWin, TRUE);

   /* Create the slider field. */
   ScreenOf(slider)		= cdkscreen;
   slider->parent		= cdkscreen->window;
   slider->shadowWin		= 0;
   slider->boxWidth		= boxWidth;
   slider->boxHeight		= boxHeight;
   slider->fieldWidth		= fieldWidth-1;
   slider->filler		= filler;
   slider->low			= low;
   slider->high			= high;
   slider->current		= start;
   slider->inc			= inc;
   slider->fastinc		= fastInc;
   slider->barFieldWidth	= fieldWidth - highValueLen;
   slider->exitType		= vNEVER_ACTIVATED;
   ObjOf(slider)->inputWindow	= slider->win;
   slider->shadow		= shadow;
   slider->preProcessFunction	= 0;
   slider->preProcessData	= 0;
   slider->postProcessFunction	= 0;
   slider->postProcessData	= 0;

   /* Determine the step value. */
   slider->step = (float)((float)slider->fieldWidth/(float)(high-low));

   /* Set the start value. */
   if (start < low)
   {
      slider->current = low;
   }

   /* Do we want a shadow??? */
   if (shadow)
   {
      slider->shadowWin = newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
      if (slider->shadowWin == 0)
      {
	 destroyCDKObject(slider);
	 return (0);
      }
   }

   /* Clean the key bindings. */
   cleanCDKObjectBindings (vSLIDER, slider);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vSLIDER, slider);

   /* Return the pointer. */
   return (slider);
}

/*
 * This allows the person to use the slider field.
 */
int activateCDKSlider (CDKSLIDER *slider, chtype *actions)
{
   int ret;

   /* Draw the slider widget. */
   drawCDKSlider (slider, ObjOf(slider)->box);

   /* Check if actions is null. */
   if (actions == 0)
   {
      chtype input = 0;
      for (;;)
      {
	 /* Get the input. */
	 input = getcCDKObject (ObjOf(slider));

	 /* Inject the character into the widget. */
	 ret = injectCDKSlider (slider, input);
	 if (slider->exitType != vEARLY_EXIT)
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
	 ret = injectCDKSlider (slider, actions[x]);
	 if (slider->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }

   /* Set the exit type and return. */
   slider->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This function injects a single character into the widget.
 */
static int _injectCDKSlider (CDKOBJS *object, chtype input)
{
   CDKSLIDER *slider = (CDKSLIDER *)object;
   int ppReturn = 1;
   int ret = unknownInt;
   bool complete = FALSE;

   /* Set the exit type. */
   slider->exitType = vEARLY_EXIT;

   /* Draw the field. */
   drawCDKSliderField (slider);

   /* Check if there is a pre-process function to be called. */
   if (slider->preProcessFunction != 0)
   {
      /* Call the pre-process function. */
      ppReturn = slider->preProcessFunction (vSLIDER, slider, slider->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a key binding. */
      if (checkCDKObjectBind(vSLIDER, slider, input) != 0)
      {
	 slider->exitType = vESCAPE_HIT;
	 complete = TRUE;
      }
      else
      {
	 switch (input)
	 {
	    case KEY_LEFT : case 'd' : case '-' : case KEY_DOWN :
		 if (slider->current > slider->low)
		 {
		    slider->current -= slider->inc;
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_RIGHT : case 'u' : case '+' : case KEY_UP :
		 if (slider->current < slider->high)
		 {
		    slider->current += slider->inc;
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_PPAGE : case 'U' : case CONTROL('B') :
		 if ((slider->current + slider->fastinc) <= slider->high)
		 {
		    slider->current += slider->fastinc;
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	       case KEY_NPAGE : case 'D' : case CONTROL('F') :
		 if ((slider->current - slider->fastinc) >= slider->low)
		 {
		    slider->current -= slider->fastinc;
		 }
		 else
		 {
		    Beep();
		 }
		 break;

	    case KEY_HOME : case 'g' : case '0' :
		 slider->current = slider->low;
		 break;

	    case KEY_END : case 'G' : case '$' :
		 slider->current = slider->high;
		 break;

	    case KEY_ESC :
		 slider->exitType = vESCAPE_HIT;
		 complete = TRUE;
		 break;

	    case KEY_RETURN : case KEY_TAB : case KEY_ENTER :
		 slider->exitType = vNORMAL;
		 ret = (slider->current);
		 complete = TRUE;
		 break;

	    case CDK_REFRESH :
		 eraseCDKScreen (ScreenOf(slider));
		 refreshCDKScreen (ScreenOf(slider));
		 break;

	    default :
		 break;
	 }
      }

      /* Should we call a post-process? */
      if (!complete && (slider->postProcessFunction != 0))
      {
	 slider->postProcessFunction (vSLIDER, slider, slider->postProcessData, input);
      }
   }

   if (!complete) {
      drawCDKSliderField (slider);
      slider->exitType = vEARLY_EXIT;
   }

   ResultOf(slider).valueInt = ret;
   return (ret != unknownInt);
}

/*
 * This moves the slider field to the given location.
 */
static void _moveCDKSlider (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKSLIDER *slider = (CDKSLIDER *)object;
   int currentX = getbegx(slider->win);
   int currentY = getbegy(slider->win);
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
      xpos = getbegx(slider->win) + xplace;
      ypos = getbegy(slider->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(slider), &xpos, &ypos, slider->boxWidth, slider->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(slider->win, -xdiff, -ydiff);
   moveCursesWindow(slider->labelWin, -xdiff, -ydiff);
   moveCursesWindow(slider->fieldWin, -xdiff, -ydiff);
   moveCursesWindow(slider->shadowWin, -xdiff, -ydiff);

   /* Touch the windows so they 'move'. */
   touchwin (WindowOf(slider));
   wrefresh (WindowOf(slider));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKSlider (slider, ObjOf(slider)->box);
   }
}

/*
 * This function draws the slider widget.
 */
static void _drawCDKSlider (CDKOBJS *object, boolean Box)
{
   CDKSLIDER *slider = (CDKSLIDER *)object;

   /* Draw the shadow. */
   if (slider->shadowWin != 0)
   {
       drawShadow (slider->shadowWin);
   }

   /* Box the widget if asked. */
   if (Box)
   {
      drawObjBox (slider->win, ObjOf(slider));
   }

   drawCdkTitle (slider->win, object);

   /* Draw the label. */
   if (slider->labelWin != 0)
   {
      writeChtype (slider->labelWin, 0, 0, slider->label, HORIZONTAL, 0, slider->labelLen);
      wrefresh (slider->labelWin);
   }

   /* Draw the field window. */
   drawCDKSliderField (slider);
}

/*
 * This draws the slider widget.
 */
static void drawCDKSliderField (CDKSLIDER *slider)
{
   int fillerCharacters, len, x;
   char temp[256];

   /* Determine how many filler characters need to be drawn. */
   fillerCharacters = (int)((slider->current-slider->low) * slider->step);

   /* Empty the field. */
   werase (slider->fieldWin);

   /* Add the character to the window. */
   for (x=0; x < fillerCharacters; x++)
   {
      mvwaddch (slider->fieldWin, 0, x, slider->filler);
   }

   /* Draw the value in the field. */
   sprintf (temp, "%d", slider->current);
   len = (int)strlen(temp);
   writeCharAttrib (slider->fieldWin, slider->fieldWidth,
			0, temp, A_NORMAL, HORIZONTAL, 0, len);

   /* Redraw the field. */
   touchwin (slider->fieldWin);
   wrefresh (slider->fieldWin);
}

/*
 * This sets the background color of the widget.
 */
void setCDKSliderBackgroundColor (CDKSLIDER *slider, char *color)
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
   setCDKSliderBackgroundAttrib (slider, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This sets the background attribute of the widget.
 */
void setCDKSliderBackgroundAttrib (CDKSLIDER *slider, chtype attrib)
{
   /* Set the widgets background attribute. */
   wbkgd (slider->win, attrib);
   wbkgd (slider->fieldWin, attrib);
   if (slider->labelWin != 0)
   {
      wbkgd (slider->labelWin, attrib);
   }
}

/*
 * This function destroys the slider widget.
 */
static void _destroyCDKSlider (CDKOBJS *object)
{
   if (object != 0)
   {
      CDKSLIDER *slider = (CDKSLIDER *)object;

      cleanCdkTitle (object);
      freeChtype (slider->label);

      /* Clean up the windows. */
      deleteCursesWindow (slider->fieldWin);
      deleteCursesWindow (slider->labelWin);
      deleteCursesWindow (slider->shadowWin);
      deleteCursesWindow (slider->win);

      /* Unregister this object. */
      unregisterCDKObject (vSLIDER, slider);
   }
}

/*
 * This function erases the slider widget from the screen.
 */
static void _eraseCDKSlider (CDKOBJS *object)
{
   if (validCDKObject (object))
   {
      CDKSLIDER *slider = (CDKSLIDER *)object;

      eraseCursesWindow (slider->labelWin);
      eraseCursesWindow (slider->fieldWin);
      eraseCursesWindow (slider->win);
      eraseCursesWindow (slider->shadowWin);
   }
}

/*
 * This function sets the low/high/current values of the slider widget.
 */
void setCDKSlider (CDKSLIDER *slider, int low, int high, int value, boolean Box)
{
   setCDKSliderLowHigh (slider, low, high);
   setCDKSliderValue (slider, value);
   setCDKSliderBox (slider, Box);
}

/*
 * This function sets the low/high/current value of the widget.
 */
void setCDKSliderValue (CDKSLIDER *slider, int value)
{
   slider->current = value;
}
int getCDKSliderValue (CDKSLIDER *slider)
{
   return slider->current;
}

/*
 * This function sets the low/high values of the widget.
 */
void setCDKSliderLowHigh (CDKSLIDER *slider, int low, int high)
{
   /* Make sure the values aren't out of bounds. */
   if (low <= high)
   {
      slider->low	= low;
      slider->high	= high;
   }
   else if (low > high)
   {
      slider->low	= high;
      slider->high	= low;
   }

   /* Make sure the user hasn't done something silly. */
   if (slider->current < low)
   {
      slider->current = low;
   }
   if (slider->current > high)
   {
      slider->current = high;
   }

   /* Keep the other values. */
   slider->barFieldWidth = slider->fieldWidth - intlen (slider->high);

   /* Determine the step value. */
   slider->step = (float)((float)slider->fieldWidth / (float)(high-low));
}
int getCDKSliderLowValue (CDKSLIDER *slider)
{
   return slider->low;
}
int getCDKSliderHighValue (CDKSLIDER *slider)
{
   return slider->high;
}

/*
 * This sets the box attribute of the slider widget.
 */
void setCDKSliderBox (CDKSLIDER *slider, boolean Box)
{
   ObjOf(slider)->box = Box;
   ObjOf(slider)->borderSize = Box ? 1 : 0;
}
boolean getCDKSliderBox (CDKSLIDER *slider)
{
   return ObjOf(slider)->box;
}

/*
 * This function sets the pre-process function.
 */
void setCDKSliderPreProcess (CDKSLIDER *slider, PROCESSFN callback, void *data)
{
   slider->preProcessFunction = callback;
   slider->preProcessData = data;
}

/*
 * This function sets the post-process function.
 */
void setCDKSliderPostProcess (CDKSLIDER *slider, PROCESSFN callback, void *data)
{
   slider->postProcessFunction = callback;
   slider->postProcessData = data;
}

static void _focusCDKSlider(CDKOBJS *object GCC_UNUSED)
{
   /* FIXME */
}

static void _unfocusCDKSlider(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _refreshDataCDKSlider(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}

static void _saveDataCDKSlider(CDKOBJS *entry GCC_UNUSED)
{
   /* FIXME */
}
