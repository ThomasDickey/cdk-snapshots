#include "cdk.h"
#include <limits.h>

/*
 * $Author: tom $
 * $Date: 1999/05/30 00:16:28 $
 * $Revision: 1.41 $
 */

/*
 * Declare file local prototypes.
 */
static void drawCDKSliderField (CDKSLIDER *slider);

DeclareCDKObjects(my_funcs,Slider)

/*
 * This function creates a slider widget.
 */
CDKSLIDER *newCDKSlider (CDKSCREEN *cdkscreen, int xplace, int yplace, char *title, char *label, chtype filler, int fieldWidth, int start, int low, int high, int inc, int fastInc, boolean Box, boolean shadow)
{
   /* Declare local variables. */
   CDKSLIDER *slider	= newCDKObject(CDKSLIDER, &my_funcs);
   chtype *holder	= (chtype *)NULL;
   int parentWidth	= getmaxx(cdkscreen->window) - 1;
   int parentHeight	= getmaxy(cdkscreen->window) - 1;
   int boxHeight	= 3;
   int boxWidth		= 0;
   int maxWidth		= INT_MIN;
   int horizontalAdjust	= 0;
   int xpos		= xplace;
   int ypos		= yplace;
   int highValueLen	= intlen (high);
   char *temp[256];
   int x, len, junk, junk2;

   /* Set some basic values of the slider field. */
   slider->label	= (chtype *)NULL;
   slider->labelLen	= 0;
   slider->labelWin	= (WINDOW *)NULL;
   slider->titleLines	= 0;

  /*
   * If the fieldWidth is a negative value, the fieldWidth will
   * be COLS-fieldWidth, otherwise, the fieldWidth will be the
   * given width.
   */
   fieldWidth = setWidgetDimension (parentWidth, fieldWidth, 0);

   /* Translate the label char *pointer to a chtype pointer. */
   if (label != (char *)NULL)
   {
      slider->label	= char2Chtype (label, &slider->labelLen, &junk);
      boxWidth		= slider->labelLen + fieldWidth + highValueLen + 2;
   }
   else
   {
      boxWidth = fieldWidth + highValueLen + 2;
   }

   /* Translate the char * items to chtype * */
   if (title != (char *)NULL)
   {
      /* We need to split the title on \n. */
      slider->titleLines = splitString (title, temp, '\n');

      /* We need to determine the widest title line. */
      for (x=0; x < slider->titleLines; x++)
      {
         holder = char2Chtype (temp[x], &len, &junk2);
         maxWidth = MAXIMUM (maxWidth, len);
         freeChtype (holder);
      }

      /*
       * If one of the title lines is wider than the field and the label,
       * the box width will expand to accomodate.
       */
       if (maxWidth > boxWidth)
       {
          horizontalAdjust = (int)((maxWidth - boxWidth) / 2) + 1;
          boxWidth = maxWidth + 2;
       }

      /* For each line in the title, convert from char * to chtype * */
      for (x=0; x < slider->titleLines; x++)
      {
         slider->title[x]	= char2Chtype (temp[x], &slider->titleLen[x], &slider->titlePos[x]);
         slider->titlePos[x]	= justifyString (boxWidth, slider->titleLen[x], slider->titlePos[x]);
         freeChar (temp[x]);
      }
   }
   else
   {
      /* No title? Set the required variables. */
      slider->titleLines = 0;
   }
   boxHeight += slider->titleLines;

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);
   fieldWidth = (fieldWidth > (boxWidth-slider->labelLen-highValueLen-1) ? (boxWidth-slider->labelLen-highValueLen-1) : fieldWidth);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the slider window. */
   slider->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the main window NULL??? */
   if (slider->win == (WINDOW *) NULL)
   {
      freeChtype (slider->label);
      free (slider);

      /* Return a NULL pointer. */
      return ((CDKSLIDER *) NULL);
   }
   keypad (slider->win, TRUE);

   /* Create the slider label window. */
   if (slider->label != (chtype *)NULL)
   {
      slider->labelWin = subwin (slider->win, 1,
					slider->labelLen + 2,
					ypos + slider->titleLines + 1,
					xpos + horizontalAdjust + 1);
   }

   /* Create the slider field window. */
   slider->fieldWin = subwin (slider->win, 1,
				fieldWidth + highValueLen-1,
				ypos + slider->titleLines + 1,
				xpos + slider->labelLen + horizontalAdjust + 1);
   keypad (slider->fieldWin, TRUE);

   /* Create the slider field. */
   ScreenOf(slider)		= cdkscreen;
   slider->parent		= cdkscreen->window;
   slider->shadowWin		= (WINDOW *)NULL;
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
   ObjOf(slider)->box		= Box;
   slider->shadow		= shadow;
   slider->preProcessFunction	= (PROCESSFN)NULL;
   slider->preProcessData	= (void *)NULL;
   slider->postProcessFunction	= (PROCESSFN)NULL;
   slider->postProcessData	= (void *)NULL;
   slider->ULChar		= ACS_ULCORNER;
   slider->URChar		= ACS_URCORNER;
   slider->LLChar		= ACS_LLCORNER;
   slider->LRChar		= ACS_LRCORNER;
   slider->HChar		= ACS_HLINE;
   slider->VChar		= ACS_VLINE;
   slider->BoxAttrib		= A_NORMAL;

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
      slider->shadowWin	= newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
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
   /* Declare local variables. */
   int ret;

   /* Draw the slider widget. */
   drawCDKSlider (slider, ObjOf(slider)->box);

   /* Check if actions is NULL. */
   if (actions == (chtype *)NULL)
   {
      chtype input = (chtype)NULL;
      for (;;)
      {
         /* Get the input. */
         input = wgetch (slider->fieldWin);

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
int injectCDKSlider (CDKSLIDER *slider, chtype input)
{
   /* Declare some local variables. */
   int ppReturn = 1;

   /* Set the exit type. */
   slider->exitType = vEARLY_EXIT;

   /* Draw the field. */
   drawCDKSliderField (slider);

   /* Check if there is a pre-process function to be called. */
   if (slider->preProcessFunction != (PROCESSFN)NULL)
   {
      /* Call the pre-process function. */
      ppReturn = ((PROCESSFN)(slider->preProcessFunction)) (vSLIDER, slider, slider->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a key binding. */
      if (checkCDKObjectBind(vSLIDER, slider, input) != 0)
      {
         slider->exitType = vESCAPE_HIT;
         return -1;
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

            case KEY_PPAGE : case 'U' : case '' :
                 if ((slider->current + slider->fastinc) <= slider->high)
                 {
                    slider->current += slider->fastinc;
                 }
                 else
                 {
                    Beep();
                 }
                 break;

               case KEY_NPAGE : case 'D' : case '' :
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
                 return -1;

            case KEY_RETURN : case KEY_TAB : case KEY_ENTER :
                 slider->exitType = vNORMAL;
                 return (slider->current);

            case CDK_REFRESH :
                 eraseCDKScreen (ScreenOf(slider));
                 refreshCDKScreen (ScreenOf(slider));
                 break;

            default :
                 break;
         }
      }

      /* Should we call a post-process? */
      if (slider->postProcessFunction != (PROCESSFN)NULL)
      {
         ((PROCESSFN)(slider->postProcessFunction)) (vSLIDER, slider, slider->postProcessData, input);
      }
   }

   /* Draw the field window. */
   drawCDKSliderField (slider);

   /* Set the exit type and return. */
   slider->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This moves the slider field to the given location.
 */
static void _moveCDKSlider (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKSLIDER *slider = (CDKSLIDER *)object;
   /* Declare local variables. */
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
   if (slider->labelWin != (WINDOW *)NULL)
   {
      moveCursesWindow(slider->labelWin, -xdiff, -ydiff);
   }
   moveCursesWindow(slider->fieldWin, -xdiff, -ydiff);

   /* If there is a shadow box we have to move it too. */
   if (slider->shadowWin != (WINDOW *)NULL)
   {
      moveCursesWindow(slider->shadowWin, -xdiff, -ydiff);
   }

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
   int x;

   /* Draw the shadow. */
   if (slider->shadowWin != (WINDOW *)NULL)
   {
       drawShadow (slider->shadowWin);
   }

   /* Box the widget if asked. */
   if (Box)
   {
      attrbox (slider->win,
		slider->ULChar, slider->URChar,
		slider->LLChar, slider->LRChar,
		slider->HChar,  slider->VChar,
		slider->BoxAttrib);
   }

   /* Draw in the title if there is one. */
   if (slider->titleLines != 0)
   {
      for (x=0; x < slider->titleLines; x++)
      {
         writeChtype (slider->win,
			slider->titlePos[x],
			x + 1,
			slider->title[x],
			HORIZONTAL, 0,
			slider->titleLen[x]);
      }
      wrefresh (slider->win);
   }

   /* Draw the label. */
   if (slider->labelWin != (WINDOW *)NULL)
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
   /* Declare the local variables. */
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
 * These functions set the drawing characters of the widget.
 */
void setCDKSliderULChar (CDKSLIDER *slider, chtype character)
{
   slider->ULChar = character;
}
void setCDKSliderURChar (CDKSLIDER *slider, chtype character)
{
   slider->URChar = character;
}
void setCDKSliderLLChar (CDKSLIDER *slider, chtype character)
{
   slider->LLChar = character;
}
void setCDKSliderLRChar (CDKSLIDER *slider, chtype character)
{
   slider->LRChar = character;
}
void setCDKSliderVerticalChar (CDKSLIDER *slider, chtype character)
{
   slider->VChar = character;
}
void setCDKSliderHorizontalChar (CDKSLIDER *slider, chtype character)
{
   slider->HChar = character;
}
void setCDKSliderBoxAttribute (CDKSLIDER *slider, chtype character)
{
   slider->BoxAttrib = character;
}

/*
 * This sets the background color of the widget.
 */
void setCDKSliderBackgroundColor (CDKSLIDER *slider, char *color)
{
   chtype *holder = (chtype *)NULL;
   int junk1, junk2;

   /* Make sure the color isn't NULL. */
   if (color == (char *)NULL)
   {
      return;
   }

   /* Convert the value of the environment variable to a chtype. */
   holder = char2Chtype (color, &junk1, &junk2);

   /* Set the widgets background color. */
   wbkgd (slider->win, holder[0]);
   wbkgd (slider->fieldWin, holder[0]);
   if (slider->labelWin != (WINDOW *)NULL)
   {
      wbkgd (slider->labelWin, holder[0]);
   }

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This function destroys the slider widget.
 */
void destroyCDKSlider (CDKSLIDER *slider)
{
   int x;

   /* Erase the object. */
   eraseCDKSlider (slider);

   /* Clean up the char pointers. */
   freeChtype (slider->label);
   for (x=0; x < slider->titleLines; x++)
   {
      freeChtype (slider->title[x]);
   }

   /* Clean up the windows. */
   deleteCursesWindow (slider->fieldWin);
   deleteCursesWindow (slider->labelWin);
   deleteCursesWindow (slider->shadowWin);
   deleteCursesWindow (slider->win);

   /* Unregister this object. */
   unregisterCDKObject (vSLIDER, slider);

   /* Finish cleaning up. */
   free (slider);
}

/*
 * This function erases the slider widget from the screen.
 */
static void _eraseCDKSlider (CDKOBJS *object)
{
   CDKSLIDER *slider = (CDKSLIDER *)object;

   eraseCursesWindow (slider->labelWin);
   eraseCursesWindow (slider->fieldWin);
   eraseCursesWindow (slider->win);
   eraseCursesWindow (slider->shadowWin);
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
