#include "cdk.h"
#include <limits.h>

/*
 * $Author: glovem $
 * $Date: 1997/07/24 13:30:33 $
 * $Revision: 1.18 $
 */

/*
 * Declare file local variables.
 */
char *monthsOfTheYear[] = {"NULL", "January", "February", "March", "April",
				"May", "June", "July", "August", "September",
				"October", "November", "December"};
int daysOfTheMonth[] = {-1, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/*
 * Declare file local prototypes.
 */
int isLeapYear (int year);
int getMonthLength (int year, int month);
int getMonthStartWeekday (int year, int month);
time_t getCurrentTime (CDKCALENDAR *calendar);
void verifyCalendarDate (CDKCALENDAR *calendar);
void incrementCalendarDay (CDKCALENDAR *calendar, int adjust);
void decrementCalendarDay (CDKCALENDAR *calendar, int adjust);
void incrementCalendarMonth (CDKCALENDAR *calendar, int adjust);
void decrementCalendarMonth (CDKCALENDAR *calendar, int adjust);
void incrementCalendarYear (CDKCALENDAR *calendar, int adjust);
void decrementCalendarYear (CDKCALENDAR *calendar, int adjust);
void drawCDKCalendarField (CDKCALENDAR *calendar);

/*
 * This creates a calendar widget.
 */
CDKCALENDAR *newCDKCalendar(CDKSCREEN *cdkscreen, int xplace, int yplace, char *title, int day, int month, int year, chtype dayAttrib, chtype monthAttrib, chtype yearAttrib, chtype highlight, boolean box, boolean shadow)
{
   /* Maintain the calendar information. */
   CDKCALENDAR *calendar	= (CDKCALENDAR *)malloc (sizeof (CDKCALENDAR));
   int parentWidth		= WIN_WIDTH (cdkscreen->window);
   int parentHeight		= WIN_HEIGHT (cdkscreen->window);
   int boxWidth			= 24;
   int boxHeight		= 11;
   int xpos			= xplace;
   int ypos			= yplace;
   int len			= 0;
   int maxWidth			= INT_MIN;
   chtype *junk			= (chtype *)NULL;
   char *temp[MAX_LINES];
   int x, y, z, junk2;
   struct tm *dateInfo;
   time_t clck;

   /* We need to determine the width of the calendar box. */
   if (title != (char *)NULL)
   {
      /* We need to split the title on \n. */
      calendar->titleLines = splitString (title, temp, '\n');

      /* For each element, determine the width. */
      for (x=0; x < calendar->titleLines; x++)
      {
         junk = char2Chtype (temp[x], &len, &junk2);
         maxWidth = MAXIMUM (maxWidth, len);
         freeChtype (junk);
      }
      maxWidth += 2;
    
      /* Set the box width. */
      boxWidth = MAXIMUM (maxWidth, boxWidth);

      /* For each line in the title, convert from char * to chtype * */
      for (x=0; x < calendar->titleLines; x++)
      {
         calendar->title[x]	= char2Chtype (temp[x], &calendar->titleLen[x], &calendar->titlePos[x]);
         calendar->titlePos[x]	= justifyString (boxWidth, calendar->titleLen[x], calendar->titlePos[x]);
         freeChar (temp[x]);
      }
   }
   else
   {
      /* No title? Set the required variables. */
      calendar->titleLines = 0;
   }
   boxHeight += calendar->titleLines;

  /*
   * Make sure we didn't extend beyond the dimensions of the window.
   */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Create the calendar window. */
   calendar->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the window NULL? */
   if (calendar->win == (WINDOW *)NULL)
   {
      /* Clean up the pointers. */
      free (calendar);
 
      /* Exit with NULL. */
      return ((CDKCALENDAR *)NULL);
   }
   keypad (calendar->win, TRUE);

   /* Set some variables. */
   calendar->xOffset = (int)((boxWidth - 20) / 2);
   calendar->fieldWidth = boxWidth - 2;

   /* Set the rest of the widget values. */
   calendar->parent			= cdkscreen->window;
   calendar->labelWin			= subwin (calendar->win, 1, calendar->fieldWidth, ypos+calendar->titleLines+1, xpos+calendar->xOffset);
   calendar->fieldWin			= subwin (calendar->win, 7, calendar->fieldWidth, ypos+calendar->titleLines+3, xpos+calendar->xOffset-2);
   calendar->shadowWin			= (WINDOW *)NULL;
   calendar->xpos			= xpos;
   calendar->ypos			= ypos;
   calendar->boxWidth			= boxWidth;
   calendar->boxHeight			= boxHeight;
   calendar->day			= day;
   calendar->month			= month;
   calendar->year			= year;
   calendar->dayAttrib			= dayAttrib;
   calendar->monthAttrib		= monthAttrib;
   calendar->yearAttrib			= yearAttrib;
   calendar->highlight			= highlight;
   calendar->width			= boxWidth;
   calendar->ULChar			= ACS_ULCORNER;
   calendar->URChar			= ACS_URCORNER;
   calendar->LLChar			= ACS_LLCORNER;
   calendar->LRChar			= ACS_LRCORNER;
   calendar->HChar			= ACS_HLINE;
   calendar->VChar			= ACS_VLINE;
   calendar->BoxAttrib			= A_NORMAL;
   calendar->exitType			= vNEVER_ACTIVATED;
   calendar->box			= box;
   calendar->shadow			= shadow;
   calendar->preProcessFunction		= (PROCESSFN)NULL;
   calendar->preProcessData		= (void *)NULL;
   calendar->postProcessFunction	= (PROCESSFN)NULL;
   calendar->postProcessData		= (void *)NULL;

   /* Clear out the markers. */
   for (z=0; z < MAX_YEARS; z++)
   {
      for (y=0; y < MAX_MONTHS; y++)
      {
         for (x=0; x < MAX_DAYS; x++)
         {
            calendar->marker[x][y][z] = (chtype)NULL;
         }
      }
   }

   /* If the day/month/year values were 0, then use today's date. */
   if ((calendar->day == 0) && (calendar->month == 0) && (calendar->year == 0))
   {
      time (&clck);
      dateInfo = localtime (&clck);
 
      calendar->day        = dateInfo->tm_mday;
      calendar->month      = dateInfo->tm_mon+1;
      calendar->year       = dateInfo->tm_year+1900;
   }

   /* Verify the dates provided. */
   verifyCalendarDate (calendar);

   /* Determine which day the month starts on. */
   calendar->weekDay = getMonthStartWeekday (calendar->year, calendar->month);

   /* If a shadow was requested, then create the shadow window. */
   if (shadow)
   {
      calendar->shadowWin = newwin (boxHeight, boxWidth, ypos+1, xpos+1);
   }

   /* Clean the key bindings. */
   cleanCDKObjectBindings (vCALENDAR, calendar);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vCALENDAR, calendar);

   /* Return the calendar pointer. */
   return (calendar);
}

/* 
 * This function lets the user play with this widget.
 */
time_t activateCDKCalendar (CDKCALENDAR *calendar, chtype *actions)
{
   /* Declare local variables. */
   chtype input	= (chtype)NULL;
   time_t ret	= -1;
   
   /* Draw the widget. */
   drawCDKCalendar (calendar, calendar->box);

   /* Check if 'actions' is NULL. */
   if (actions == (chtype *)NULL)
   {
      for (;;)
      {
         /* Get the input. */
         input = wgetch (calendar->win);

         /* Inject the character into the widget. */
         ret = injectCDKCalendar (calendar, input);
         if (calendar->exitType != vEARLY_EXIT)
         {
            return ret;
         }
      }
   }
   else
   {
      int length = chlen (actions);
      int x =0;

      /* Inject each character one at a time. */
      for (x=0; x < length; x++)
      {
         ret = injectCDKCalendar (calendar, actions[x]);
         if (calendar->exitType != vEARLY_EXIT)
         {
            return ret;
         }
      }
   }
   return ret;
}

/*
 * This injects a single character into the widget.
 */
time_t injectCDKCalendar (CDKCALENDAR *calendar, chtype input)
{
   /* Declare local variables. */
   int ppReturn	= 1;

   /* Set the exit type. */
   calendar->exitType = vEARLY_EXIT;

   /* Refresh the calendar field. */
   drawCDKCalendarField (calendar);

   /* Check if there is a pre-process function to be called. */
   if (calendar->preProcessFunction != (PROCESSFN)NULL)
   {
      /* Call the pre-process function. */
      ppReturn = ((PROCESSFN)(calendar->preProcessFunction)) (vCALENDAR, calendar, calendar->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check a predefined binding. */
      if (checkCDKObjectBind (vCALENDAR, calendar, input) != 0)
      {
         calendar->exitType = vESCAPE_HIT;
         return -1;
      }
      else
      {
         switch (input)
         {
            case KEY_UP :
                 decrementCalendarDay (calendar, 7);
                 break;

            case KEY_DOWN :
                 incrementCalendarDay (calendar, 7);
                 break;
      
            case KEY_LEFT :
                 decrementCalendarDay (calendar, 1);
                 break;
      
            case KEY_RIGHT :
                 incrementCalendarDay (calendar, 1);
                 break;
      
            case 'n' : case KEY_NPAGE : case '':
                 incrementCalendarMonth (calendar, 1);
                 break;

            case 'N' :
                 incrementCalendarMonth (calendar, 6);
                 break;

            case 'p' : case KEY_PPAGE : case '':
                 decrementCalendarMonth (calendar, 1);
                 break;

            case 'P' :
                 decrementCalendarMonth (calendar, 6);
                 break;

            case '-' :
                 decrementCalendarYear (calendar, 1);
                 break;

            case '+' :
                 incrementCalendarYear (calendar, 1);
                 break;

            case 'T' : case 't' : case KEY_HOME:
                 setCDKCalendarDate (calendar, -1, -1, -1);
                 break;

            case KEY_ESC :
                 calendar->exitType = vESCAPE_HIT;
                 return (time_t)-1;
                 break;

            case KEY_RETURN : case KEY_TAB : case KEY_ENTER :
                 calendar->exitType = vNORMAL;
                 return getCurrentTime (calendar);
                 break;
      
            case CDK_REFRESH :
                 eraseCDKScreen (calendar->screen);
                 refreshCDKScreen (calendar->screen);
                 break;
         }
      }

      /* Should we do a post-process? */
      if (calendar->postProcessFunction != (PROCESSFN)NULL)
      {
         ((PROCESSFN)(calendar->postProcessFunction)) (vCALENDAR, calendar, calendar->postProcessData, input);
      }
   }

   /* Set the exit type then exit. */
   calendar->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This moves the calendar field to the given location.
 */
void moveCDKCalendar (CDKCALENDAR *calendar, int xplace, int yplace, boolean relative, boolean refresh)
{
   /* Declare local variables. */
   int currentX = calendar->win->_begx;
   int currentY = calendar->win->_begy;
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
      xpos = calendar->win->_begx + xplace;
      ypos = calendar->win->_begy + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (calendar->screen->window, &xpos, &ypos, calendar->boxWidth, calendar->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   calendar->win->_begx = xpos;
   calendar->win->_begy = ypos;
   calendar->fieldWin->_begx -= xdiff;
   calendar->fieldWin->_begy -= ydiff;
   if (calendar->labelWin != (WINDOW *)NULL)
   {
      calendar->labelWin->_begx -= xdiff;
      calendar->labelWin->_begy -= ydiff;
   }

   /* If there is a shadow box we have to move it too. */
   if (calendar->shadowWin != (WINDOW *)NULL)
   {
      calendar->shadowWin->_begx -= xdiff;
      calendar->shadowWin->_begy -= ydiff;
   }

   /* Touch the windows so they 'move'. */
   touchwin (calendar->screen->window);
   wrefresh (calendar->screen->window);

   /* Redraw the window, if they asked for it. */
   if (refresh)
   {
      drawCDKCalendar (calendar, calendar->box);
   }
}

/*
 * This allows the user to use the cursor keys to adjust the
 * position of the widget.
 */
void positionCDKCalendar (CDKCALENDAR *calendar)
{
   /* Declare some variables. */
   int origX	= calendar->win->_begx;
   int origY	= calendar->win->_begy;
   chtype key	= (chtype)NULL;

   /* Let them move the widget around until they hit return. */
   while ((key != KEY_RETURN) && (key != KEY_ENTER))
   {
      key = wgetch (calendar->win);
      if (key == KEY_UP || key == '8')
      {
         if (calendar->win->_begy > 0)
         {
            moveCDKCalendar (calendar, 0, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_DOWN || key == '2')
      {
         if (calendar->win->_begy+calendar->win->_maxy < calendar->screen->window->_maxy-1)
         {
            moveCDKCalendar (calendar, 0, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_LEFT || key == '4')
      {
         if (calendar->win->_begx > 0)
         {
            moveCDKCalendar (calendar, -1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_RIGHT || key == '6')
      {
         if (calendar->win->_begx+calendar->win->_maxx < calendar->screen->window->_maxx-1)
         {
            moveCDKCalendar (calendar, 1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '7')
      {
         if (calendar->win->_begy > 0 && calendar->win->_begx > 0)
         {
            moveCDKCalendar (calendar, -1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '9')
      {
         if (calendar->win->_begx+calendar->win->_maxx < calendar->screen->window->_maxx-1 &&
		calendar->win->_begy > 0)
         {
            moveCDKCalendar (calendar, 1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '1')
      {
         if (calendar->win->_begx > 0 && calendar->win->_begx+calendar->win->_maxx < calendar->screen->window->_maxx-1)
         {
            moveCDKCalendar (calendar, -1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '3')
      {
         if (calendar->win->_begx+calendar->win->_maxx < calendar->screen->window->_maxx-1 &&
		calendar->win->_begy+calendar->win->_maxy < calendar->screen->window->_maxy-1)
         {
            moveCDKCalendar (calendar, 1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '5')
      {
         moveCDKCalendar (calendar, CENTER, CENTER, FALSE, TRUE);
      }
      else if (key == 't')
      {
         moveCDKCalendar (calendar, calendar->win->_begx, TOP, FALSE, TRUE);
      }
      else if (key == 'b')
      {
         moveCDKCalendar (calendar, calendar->win->_begx, BOTTOM, FALSE, TRUE);
      }
      else if (key == 'l')
      {
         moveCDKCalendar (calendar, LEFT, calendar->win->_begy, FALSE, TRUE);
      }
      else if (key == 'r')
      {
         moveCDKCalendar (calendar, RIGHT, calendar->win->_begy, FALSE, TRUE);
      }
      else if (key == 'c')
      {
         moveCDKCalendar (calendar, CENTER, calendar->win->_begy, FALSE, TRUE);
      }
      else if (key == 'C')
      {
         moveCDKCalendar (calendar, calendar->win->_begx, CENTER, FALSE, TRUE);
      }
      else if (key == CDK_REFRESH)
      {
         eraseCDKScreen (calendar->screen);
         refreshCDKScreen (calendar->screen);
      }
      else if (key == KEY_ESC)
      {
         moveCDKCalendar (calendar, origX, origY, FALSE, TRUE);
      }
      else if ((key != KEY_RETURN) && (key != KEY_ENTER))
      {
         Beep();
      }
   }
}

/*
 * This draws the calendar widget.
 */
void drawCDKCalendar (CDKCALENDAR *calendar, boolean Box)
{
   char *header		= "Su Mo Tu We Th Fr Sa";
   int headerLen	= (int)strlen (header);
   int x;

   /* Is there a shadow? */
   if (calendar->shadowWin != (WINDOW *)NULL)
   {
      drawShadow (calendar->shadowWin);
   }

   /* Box the widget if asked. */
   if (Box)
   {
      attrbox (calendar->win,
		calendar->ULChar, calendar->URChar,
		calendar->LLChar, calendar->LRChar,
		calendar->HChar,  calendar->VChar,
		calendar->BoxAttrib);
   }

   /* Draw in the title. */
   if (calendar->titleLines != 0)
   {
      for (x=0; x < calendar->titleLines; x++)
      {
         writeChtype (calendar->win,
			calendar->titlePos[x],
			x+1,
			calendar->title[x],
			HORIZONTAL, 0,
			calendar->titleLen[x]);
      }
   }

   /* Draw in the day-of-the-week header. */
   writeChar (calendar->win,
		calendar->xOffset, calendar->titleLines+2,
		header, HORIZONTAL, 0, headerLen);

   /* Refresh the main window. */
   touchwin (calendar->win);
   wrefresh (calendar->win);

   /* Draw in the calendar field. */
   drawCDKCalendarField (calendar);
}

/*
 * This draws the month field.
 */
void drawCDKCalendarField (CDKCALENDAR *calendar)
{
   /* Declare local variables. */
   char *monthName	= monthsOfTheYear[calendar->month];
   int monthNameLength	= (int)strlen (monthName);
   int monthLength	= getMonthLength (calendar->year, calendar->month);
   int yearIndex	= calendar->year - 1900;
   int yearLen		= 0;
   int day		= 1;
   int x, y, div, mod;
   char temp[10];

   /* Get the length of the month. */
   monthLength = getMonthLength (calendar->year, calendar->month);

   /* Draw in the first line of the days. */
   for (x=calendar->weekDay; x < 7; x++)
   {
      div = day / 10;
      mod = day % 10;

      if (calendar->day == day)
      {
         mvwaddch (calendar->fieldWin, 1, ((x+1)*3)-1, (div+48) | calendar->highlight);
         mvwaddch (calendar->fieldWin, 1, ((x+1)*3), (mod+48) | calendar->highlight);
      }
      else
      {
         /* Check if there is a marker to set. */
         if (calendar->marker[day][calendar->month][yearIndex] != (chtype)NULL)
         {
            chtype marker = calendar->marker[day][calendar->month][yearIndex] | calendar->dayAttrib;
            mvwaddch (calendar->fieldWin, 1, ((x+1)*3)-1, (div+48) | marker);
            mvwaddch (calendar->fieldWin, 1, ((x+1)*3), (mod+48) | marker);
         }
         else
         {
            mvwaddch (calendar->fieldWin, 1, ((x+1)*3)-1, (div+48) | calendar->dayAttrib);
            mvwaddch (calendar->fieldWin, 1, ((x+1)*3), (mod+48) | calendar->dayAttrib);
         }
      }
      day++;
   }
   for (x=2; x <= 6; x++)
   {
      for (y=1; y <= 7; y++)
      {
         if (day <= monthLength)
         {
            div = day / 10;
            mod = day % 10;

            if (calendar->day == day)
            {
               mvwaddch (calendar->fieldWin, x, (y*3)-1, (div+48) | calendar->highlight);
               mvwaddch (calendar->fieldWin, x, (y*3), (mod+48) | calendar->highlight);
            }
            else
            {
               /* Check if there is a marker to set. */
               if (calendar->marker[day][calendar->month][yearIndex] != (chtype)NULL)
               {
                  chtype marker = calendar->marker[day][calendar->month][yearIndex] | calendar->dayAttrib;
                  mvwaddch (calendar->fieldWin, x, (y*3)-1, (div+48) | marker);
                  mvwaddch (calendar->fieldWin, x, (y*3), (mod+48) | marker);
               }
               else
               {
                  mvwaddch (calendar->fieldWin, x, (y*3)-1, (div+48) | calendar->dayAttrib);
                  mvwaddch (calendar->fieldWin, x, (y*3), (mod+48) | calendar->dayAttrib);
               }
            }
         }
         day++;
      }
   }
   touchwin (calendar->fieldWin);
   wrefresh (calendar->fieldWin);
   
   /* Draw the month in. */
   if (calendar->labelWin != (WINDOW *)NULL)
   {
      writeChar (calendar->labelWin, 0, 0,
			monthName, HORIZONTAL, 0, monthNameLength);

      /* Draw the year in. */
      sprintf (temp, "%d", calendar->year);
      yearLen = (int)strlen (temp);
      writeChar (calendar->labelWin,
			calendar->fieldWidth-yearLen-2, 0,
			temp, HORIZONTAL, 0, yearLen);

      touchwin (calendar->labelWin);
      wrefresh (calendar->labelWin);
   }
}

/*
 * This sets multiple attributes of the widget.
 */
void setCDKCalendar (CDKCALENDAR *calendar, int day, int month, int year, chtype dayAttrib, chtype monthAttrib, chtype yearAttrib, chtype highlight, boolean Box)
{
   setCDKCalendarDate (calendar, day, month, year);
   setCDKCalendarDayAttribute (calendar, dayAttrib);
   setCDKCalendarMonthAttribute (calendar, monthAttrib);
   setCDKCalendarYearAttribute (calendar, yearAttrib);
   setCDKCalendarHighlight (calendar, highlight);
   setCDKCalendarBox (calendar, Box);
}

/*
 * This sets the date and some attributes.
 */
void setCDKCalendarDate (CDKCALENDAR *calendar, int day, int month, int year)
{
   /* Declare local variables. */
   struct tm *dateInfo;
   time_t clck;

   /*
    * Get the current dates and set the default values for
    * the day/month/year values for the calendar.
    */ 
   time (&clck);
   dateInfo = localtime (&clck);

   /* Set the date elements if we need too. */
   calendar->day	= (day == -1 ? dateInfo->tm_mday : day);
   calendar->month	= (month == -1 ? dateInfo->tm_mon+1 : month);
   calendar->year	= (year == -1 ? dateInfo->tm_year+1900 : year);

   /* Verify the date information. */
   verifyCalendarDate (calendar);

   /* Get the start of the current month. */
   calendar->weekDay = getMonthStartWeekday (calendar->year, calendar->month);
}

/*
 * This returns the current date on the calendar.
 */
void getCDKCalendarDate (CDKCALENDAR *calendar, int *day, int *month, int *year)
{
   (*day)	= calendar->day;
   (*month)	= calendar->month;
   (*year)	= calendar->year;
}

/*
 * This sets the attribute of the days in the calendar.
 */
void setCDKCalendarDayAttribute (CDKCALENDAR *calendar, chtype attribute)
{
   calendar->dayAttrib = attribute;
}
chtype getCDKCalendarDayAttribute (CDKCALENDAR *calendar)
{
   return calendar->dayAttrib;
}

/*
 * This sets the attribute of the month names in the calendar.
 */
void setCDKCalendarMonthAttribute (CDKCALENDAR *calendar, chtype attribute)
{
   calendar->monthAttrib = attribute;
}
chtype getCDKCalendarMonthAttribute (CDKCALENDAR *calendar)
{
   return calendar->monthAttrib;
}

/*
 * This sets the attribute of the year in the calendar.
 */
void setCDKCalendarYearAttribute (CDKCALENDAR *calendar, chtype attribute)
{
   calendar->yearAttrib = attribute;
}
chtype getCDKCalendarYearAttribute (CDKCALENDAR *calendar)
{
   return calendar->yearAttrib;
}

/*
 * This sets the attribute of the highlight box.
 */
void setCDKCalendarHighlight (CDKCALENDAR *calendar, chtype highlight)
{
   calendar->highlight = highlight;
}
chtype getCDKCalendarHighlight (CDKCALENDAR *calendar)
{
   return calendar->highlight;
}

/*
  * This sets the box attibute of the widget.
 */
void setCDKCalendarBox (CDKCALENDAR *calendar, boolean Box)
{
   calendar->box = Box;
}
boolean getCDKCalendarBox (CDKCALENDAR *calendar)
{
   return calendar->box;
}

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKCalendarULChar (CDKCALENDAR *calendar, chtype character)
{
   calendar->ULChar = character;
}
void setCDKCalendarURChar (CDKCALENDAR *calendar, chtype character)
{
   calendar->URChar = character;
}
void setCDKCalendarLLChar (CDKCALENDAR *calendar, chtype character)
{
   calendar->LLChar = character;
}
void setCDKCalendarLRChar (CDKCALENDAR *calendar, chtype character)
{
   calendar->LRChar = character;
}
void setCDKCalendarVerticalChar (CDKCALENDAR *calendar, chtype character)
{
   calendar->VChar = character;
}
void setCDKCalendarHorizontalChar (CDKCALENDAR *calendar, chtype character)
{
   calendar->HChar = character;
}
void setCDKCalendarBoxAttribute (CDKCALENDAR *calendar, chtype character)
{
   calendar->BoxAttrib = character;
}

/*
 * This sets the background color of the widget.
 */ 
void setCDKCalendarBackgroundColor (CDKCALENDAR *calendar, char *color)
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
   wbkgd (calendar->win, holder[0]);
   wbkgd (calendar->fieldWin, holder[0]);
   if (calendar->labelWin != (WINDOW *)NULL)
   {
      wbkgd (calendar->labelWin, holder[0]);
   }

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This erases the calendar widget.
 */
void eraseCDKCalendar (CDKCALENDAR *calendar)
{
   eraseCursesWindow (calendar->labelWin);
   eraseCursesWindow (calendar->fieldWin);
   eraseCursesWindow (calendar->win);
   eraseCursesWindow (calendar->shadowWin);
}

/*
 * This destroys the calendar object pointer.
 */
void destroyCDKCalendar (CDKCALENDAR *calendar)
{
   int x;

   /* Erase the old calendar. */
   eraseCDKCalendar (calendar);

   /* Free up the character pointers. */
   for (x=0; x < calendar->titleLines; x++)
   {
      freeChtype (calendar->title[x]);
   }

   /* Free up the window pointers. */
   deleteCursesWindow (calendar->labelWin);
   deleteCursesWindow (calendar->fieldWin);
   deleteCursesWindow (calendar->shadowWin);
   deleteCursesWindow (calendar->win);
 
   /* Unregister the object. */
   unregisterCDKObject (vCALENDAR, calendar);

   /* Free the object pointer. */
   free (calendar);
}

/*
 * This sets a marker on the calendar.
 */
void setCDKCalendarMarker (CDKCALENDAR *calendar, int day, int month, int year, chtype marker)
{
   int yearIndex = year-1900;

   /* Check to see if a marker has not already been set. */
   if (calendar->marker[day][month][yearIndex] != (chtype)NULL)
   {
      chtype duplicateMarker = calendar->marker[day][month][yearIndex] | A_BLINK;
      calendar->marker[day][month][yearIndex] = duplicateMarker;
   }
   else
   {
      calendar->marker[day][month][yearIndex] = marker;
   }
}

/*
 * This sets a marker on the calendar.
 */
void removeCDKCalendarMarker (CDKCALENDAR *calendar, int day, int month, int year)
{
   int yearIndex = year-1900;
   calendar->marker[day][month][yearIndex] = (chtype)NULL;
}

/*
 * This function sets the pre-process function.
 */
void setCDKCalendarPreProcess (CDKCALENDAR *calendar, PROCESSFN callback, void *data)
{
   calendar->preProcessFunction = callback;
   calendar->preProcessData = data;
}

/*
 * This function sets the post-process function.
 */
void setCDKCalendarPostProcess (CDKCALENDAR *calendar, PROCESSFN callback, void *data)
{
   calendar->postProcessFunction = callback;
   calendar->postProcessData = data;
}

/*
 * This makes sure that the dates provided exist.
 */
void verifyCalendarDate (CDKCALENDAR *calendar)
{
   int monthLength;

   /* Make sure the given year is not less than 1900. */
   if (calendar->year < 1900)
   {
      calendar->year = 1900;
   }

   /* Make sure the month is within range. */
   if (calendar->month > 12)
   {
      calendar->month = 12;
   }
   if (calendar->month < 1)
   {
      calendar->month = 1;
   }

   /* Make sure the day given is within range of the month. */
   monthLength = getMonthLength (calendar->year, calendar->month);
   if (calendar->day < 1)
   {
      calendar->day = 1;
   }
   if (calendar->day > monthLength)
   {
      calendar->day = monthLength;
   }
}

/*
 * This returns what day of the week the month starts on.
 */
int getMonthStartWeekday (int year, int month)
{
   struct tm Date;

   /* Set the tm structure correctly. */
   Date.tm_sec			= 0;
   Date.tm_min			= 0;
   Date.tm_hour			= 10;
   Date.tm_mday			= 1;
   Date.tm_mon			= month-1;
   Date.tm_year			= year-1900;
   Date.tm_isdst		= 1;

   /* Call the mktime function to fill in the holes. */
   if (mktime (&Date) == (time_t)-1)
   {
      return 0;
   }
   return Date.tm_wday;
}

/*
 * This function returns a 1 if it's a leap year and 0 if it's not.
 */
int isLeapYear (int year)
{
   if (year % 4 == 0 ||
	year % 4 == 0)
   {
      return 1;
   }
   return 0;
}

/*
 * This increments the current day by the given value.
 */
void incrementCalendarDay (CDKCALENDAR *calendar, int adjust)
{
   int monthLength = getMonthLength (calendar->year, calendar->month);
  
   /* Make sure we adjust the day correctly. */
   if (adjust + calendar->day > monthLength)
   {
      /* Have to increment the month by one. */
      calendar->day = calendar->day + adjust - monthLength;
      incrementCalendarMonth (calendar, 1);
   }
   else
   {
      calendar->day += adjust;
      drawCDKCalendarField (calendar);
   }
}

/*
 * This decrments the current day by the given value.
 */
void decrementCalendarDay (CDKCALENDAR *calendar, int adjust)
{
   int monthLength = getMonthLength (calendar->year, calendar->month);
  
   /* Make sure we adjust the day correctly. */
   if (calendar->day-adjust < 1)
   {
      /* Set the day according to the length of the month. */
      if (calendar->month == 12)
      {
          /* Make sure we aren't going past the year limit. */
          if (calendar->year == 1900)
          {
             char *mesg[] = {"<C></U>Error", "Can not go past the year 1900"};
             Beep();
             popupLabel (calendar->screen, mesg, 2);
             return;
          }
          else
          {
             monthLength = getMonthLength (calendar->year-1, 1);
             calendar->day = monthLength - (adjust-calendar->day);
          }
      }
      else
      {
          calendar->day = monthLength - (adjust-calendar->day);
      }

      /* Have to decrement the month by one. */
      decrementCalendarMonth (calendar, 1);
   }
   else
   {
      calendar->day -= adjust;
      drawCDKCalendarField (calendar);
   }
}

/*
 * This increments the current month by the given value.
 */
void incrementCalendarMonth (CDKCALENDAR *calendar, int adjust)
{
   int monthLength;

   /* Are we at the end of the year. */
   if (calendar->month + adjust > 12)
   {
      calendar->month = (calendar->month+adjust) - 12;
      calendar->year++;
   } 
   else
   {
      calendar->month += adjust;
   }

   /* Get the length of the current month. */
   monthLength = getMonthLength (calendar->year, calendar->month);
   if (calendar->day > monthLength)
   {
      calendar->day = monthLength;
   }

   /* Get the start of the current month. */
   calendar->weekDay = getMonthStartWeekday (calendar->year, calendar->month);

   /* Redraw the calendar. */
   eraseCDKCalendar (calendar);
   drawCDKCalendar (calendar, calendar->box);
}

/*
 * This decrements the current month by the given value.
 */
void decrementCalendarMonth (CDKCALENDAR *calendar, int adjust)
{
   int monthLength;

   /* Are we at the end of the year. */
   if (calendar->month == 1)
   {
      if (calendar->year == 1900)
      {
         char *mesg[] = {"<C></U>Error", "Can not go past the year 1900"};
         Beep();
         popupLabel (calendar->screen, mesg, 2);
         return;
      }
      else
      {
         calendar->month = 12 - adjust;
         calendar->year--;
      }
   } 
   else
   {
      calendar->month -= adjust;
   }

   /* Get the length of the current month. */
   monthLength = getMonthLength (calendar->year, calendar->month);
   if (calendar->day > monthLength)
   {
      calendar->day = monthLength;
   }

   /* Get the start of the current month. */
   calendar->weekDay = getMonthStartWeekday (calendar->year, calendar->month);

   /* Redraw the calendar. */
   eraseCDKCalendar (calendar);
   drawCDKCalendar (calendar, calendar->box);
}

/*
 * This increments the current year by the given value.
 */
void incrementCalendarYear (CDKCALENDAR *calendar, int adjust)
{
   /* Increment the year. */
   calendar->year += adjust;

   /* If we are in Feb make sure we don't trip into voidness. */
   if (calendar->month == 2)
   {
      int monthLength = getMonthLength (calendar->year, calendar->month);
      if (calendar->day > monthLength)
      {
         calendar->day = monthLength;
      }
   }

   /* Get the start of the current month. */
   calendar->weekDay = getMonthStartWeekday (calendar->year, calendar->month);

   /* Redraw the calendar. */
   eraseCDKCalendar (calendar);
   drawCDKCalendar (calendar, calendar->box);
}

/*
 * This decrements the current year by the given value.
 */
void decrementCalendarYear (CDKCALENDAR *calendar, int adjust)
{
   /* Make sure we don't go out of bounds. */
   if (calendar->year - adjust < 1900)
   {
      char *mesg[] = {"<C></U>Error", "Can not go past the year 1900"};
      Beep();
      popupLabel (calendar->screen, mesg, 2);
      return;
   }

   /* Decrement the year. */
   calendar->year -= adjust;

   /* If we are in Feb make sure we don't trip into voidness. */
   if (calendar->month == 2)
   {
      int monthLength = getMonthLength (calendar->year, calendar->month);
      if (calendar->day > monthLength)
      {
         calendar->day = monthLength;
      }
   }

   /* Get the start of the current month. */
   calendar->weekDay = getMonthStartWeekday (calendar->year, calendar->month);

   /* Redraw the calendar. */
   eraseCDKCalendar (calendar);
   drawCDKCalendar (calendar, calendar->box);
}

/*
 * This returns the length of the current month.
 */
int getMonthLength (int year, int month)
{
   int monthLength = daysOfTheMonth[month];

   if (month == 2)
   {
      monthLength += isLeapYear (year);
   }
   return monthLength;
}

/*
 * This returns what day of the week the month starts on.
 */
time_t getCurrentTime (CDKCALENDAR *calendar)
{
   struct tm Date, *dateInfo;
   time_t clck;

   /* Determine the current time and determine if we are in DST. */
   time (&clck);
   dateInfo = localtime (&clck);

   /* Set the tm structure correctly. */
   Date.tm_sec		= 1;
   Date.tm_min		= 0;
   Date.tm_hour		= 0;
   Date.tm_mday		= calendar->day;
   Date.tm_mon		= calendar->month-1;
   Date.tm_year		= calendar->year-1900;
   Date.tm_isdst	= dateInfo->tm_isdst;

   /* Call the mktime function to fill in the holes. */
   return mktime (&Date);
}
