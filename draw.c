#include <cdk.h>

/*
 * $Author: tom $
 * $Date: 2001/12/30 23:10:57 $
 * $Revision: 1.40 $
 */

/*
 * This sets up a basic set of color pairs. These can be redefined
 * if wanted...
 */
void initCDKColor (void)
{
#ifdef HAVE_START_COLOR
   /* Declare local variables. */
   int color[]	= {
			COLOR_WHITE,	COLOR_RED,	COLOR_GREEN,
			COLOR_YELLOW,	COLOR_BLUE,	COLOR_MAGENTA,
			COLOR_CYAN,	COLOR_BLACK
		};
   int pair	= 1;
   int x, y;

   /* Start color first. */
   start_color();

   /* Create the color pairs. */
   for (x=0; x < 8; x++)
   {
      for (y=0; y < 8; y++)
      {
	 init_pair (pair++, color[x], color[y]);
      }
   }
#endif
}

/*
 * This prints out a box around a window with attributes
 */
void boxWindow (WINDOW *window, chtype attr)
{
   /* Set some variables. */
   int tlx	= 0;
   int tly	= 0;
   int brx	= getmaxx(window) - 1;
   int bry	= getmaxy(window) - 1;
   int x, y;

   /* Draw horizontal lines. */
   for (x=tlx; x<=brx;x++)
   {
       mvwaddch (window, tly, x, ACS_HLINE | attr);
       mvwaddch (window, bry, x, ACS_HLINE | attr);
    }

   /* Draw vertical lines. */
   for (y=tly; y<=bry;y++)
   {
       mvwaddch (window, y, tlx, ACS_VLINE | attr);
       mvwaddch (window, y, brx, ACS_VLINE | attr);
    }

   /* Draw in the corners. */
   mvwaddch (window, tly, tlx, ACS_ULCORNER | attr);
   mvwaddch (window, tly, brx, ACS_URCORNER | attr);
   mvwaddch (window, bry, tlx, ACS_LLCORNER | attr);
   mvwaddch (window, bry, brx, ACS_LRCORNER | attr);
   wrefresh (window);
}

/*
 * This draws a box with attributes and lets the user define
 * each element of the box.
 */
void attrbox (WINDOW *win, chtype tlc, chtype trc, chtype blc, chtype brc, chtype horz, chtype vert, chtype attr)
{
   /* Set the coordinates. */
   int x1	= 0;
   int y1	= 0;
   int y2	= getmaxy(win) - 1;
   int x2	= getmaxx(win) - 1;
   int count	= 0;
   int x, y;

   /* Draw horizontal lines. */
   if (horz != 0)
   {
      for (x=x1; x<=x2; x++)
      {
	 if (attr != 0)
	 {
	    mvwaddch (win, y1, x, horz | attr);
	    mvwaddch (win, y2, x, horz | attr);
	 }
	 else
	 {
	    mvwaddch (win, y1, x, horz);
	    mvwaddch (win, y2, x, horz);
	 }
      }
      count++;
   }

   /* Draw vertical lines. */
   if (vert != 0)
   {
      for (y=y1; y<=y2; y++)
      {
	 if (attr != 0)
	 {
	    mvwaddch (win, y, x1, vert | attr);
	    mvwaddch (win, y, x2, vert | attr);
	 }
	 else
	 {
	    mvwaddch (win, y, x1, vert);
	    mvwaddch (win, y, x2, vert);
	 }
      }
      count++;
   }

   /* Draw in the corners. */
   if (tlc != 0)
   {
      if (attr != 0)
      {
	 mvwaddch (win, y1, x1, tlc | attr);
      }
      else
      {
	 mvwaddch (win, y1, x1, tlc);
      }
      count++;
   }
   if (trc != 0)
   {
      if (attr != 0)
      {
	 mvwaddch (win, y1, x2, trc | attr);
      }
      else
      {
	 mvwaddch (win, y1, x2, trc);
      }
      count++;
   }
   if (blc != 0)
   {
      if (attr != 0)
      {
	 mvwaddch (win, y2, x1, blc | attr);
      }
      else
      {
	 mvwaddch (win, y2, x1, blc);
      }
      count++;
   }
   if (brc != 0)
   {
      if (attr != 0)
      {
	 mvwaddch (win, y2, x2, brc | attr);
      }
      else
      {
	 mvwaddch (win, y2, x2, brc);
      }
      count++;
   }
   if (count != 0)
   {
      wrefresh (win);
   }
}

/*
 * This draws a line on the given window. (odd angle lines not working yet)
 */
void drawLine  (WINDOW *window, int startx, int starty, int endx, int endy, chtype line)
{
   /* De=clare some local vars. */
   int xdiff	= endx - startx;
   int ydiff	= endy - starty;
   int x	= 0;
   int y	= 0;

   /* Determine if we are drawing a horizontal or vertical line. */
   if (ydiff == 0)
   {
      /* Horizontal line.      <--------- X --------> */
      for (x=0; x < xdiff; x++)
      {
	 mvwaddch (window, starty, startx + x, line);
      }
   }
   else if (xdiff == 0)
   {
      /* Vertical line. */
      for (y=0; y < ydiff; y++)
      {
	 mvwaddch (window, starty + y, startx, line);
      }
   }
   else
   {
      /* We need to determine the angle of the line. */
      int height	= xdiff;
      int width		= ydiff;
      int xratio	= (height > width ? 1 : (width / height));
      int yratio	= (width > height ? (width / height) : 1);
      int xadj		= 0;
      int yadj		= 0;

      /* Set the vars. */
      x = startx;
      y = starty;
      while (x != endx && y != endy)
      {
	 /* Add the char to the window. */
	 mvwaddch (window, y, x, line);

	 /* Make the x and y adjustments. */
	 if (xadj != xratio)
	 {
	    x	= (xdiff < 0 ? x-1 : x+1);
	    xadj++;
	 }
	 else
	 {
	    xadj	= 0;
	 }
	 if (yadj != yratio)
	 {
	    y	= (ydiff < 0 ? y-1 : y+1);
	    yadj++;
	 }
	 else
	 {
	    yadj = 0;
	 }
      }
   }
}

/*
 * This draws a shadow around a window.
 */
void drawShadow (WINDOW *shadowWin)
{
   if (shadowWin != 0)
   {
      int x = 0;
      int x_hi = getmaxx(shadowWin) - 1;
      int y_hi = getmaxy(shadowWin) - 1;

      /* Draw the line on the bottom. */
      for (x=1; x <= x_hi; x++)
      {
	 mvwaddch (shadowWin, y_hi, x, ACS_HLINE    | A_DIM);
      }

      /* Draw the line on the right. */
      for (x=0; x < y_hi; x++)
      {
	 mvwaddch (shadowWin, x, x_hi, ACS_VLINE    | A_DIM);
      }
      mvwaddch (shadowWin, 0,	 x_hi, ACS_URCORNER | A_DIM);
      mvwaddch (shadowWin, y_hi, 0,    ACS_LLCORNER | A_DIM);
      mvwaddch (shadowWin, y_hi, x_hi, ACS_LRCORNER | A_DIM);
      wrefresh (shadowWin);
   }
}

/*
 * This writes out a char * string with no attributes.
 */
void writeChar (WINDOW *window, int xpos, int ypos, char *string, int align, int start, int end)
{
   /* Declare local variables. */
   int display = end - start;
   int x;

   /* Check the alignment of the message. */
   if (align == HORIZONTAL)
   {
      display = MINIMUM(display,getmaxx(window)-1);
      for (x=0; x < display ; x++)
      {
	 /* Draw the message on a horizontal axis. */
	 mvwaddch (window, ypos, xpos+x, string[x+start] | A_NORMAL);
      }
   }
   else
   {
      /* Draw the message on a vertical axis. */
      display = MINIMUM(display,getmaxy(window)-1);
      for (x=0; x < display ; x++)
      {
	 mvwaddch (window, ypos+x, xpos, string[x+start] | A_NORMAL);
      }
   }
}

/*
 * This writes out a char * string with attributes.
 */
void writeCharAttrib (WINDOW *window, int xpos, int ypos, char *string, chtype attr, int align, int start, int end)
{
   /* Declare local variables. */
   int display = end - start;
   int x;

   /* Check the alignment of the message. */
   if (align == HORIZONTAL)
   {
      /* Draw the message on a horizontal axis. */
      display = MINIMUM(display,getmaxx(window)-1);
      for (x=0; x < display ; x++)
      {
	 mvwaddch (window, ypos, xpos+x, (string[x+start] & A_CHARTEXT) | attr);
      }
   }
   else
   {
      /* Draw the message on a vertical axis. */
      display = MINIMUM(display,getmaxy(window)-1);
      for (x=0; x < display ; x++)
      {
	 mvwaddch (window, ypos+x, xpos, (string[x+start] & A_CHARTEXT) | attr);
      }
   }
}

/*
 * This writes out a chtype * string.
 */
void writeChtype (WINDOW *window, int xpos, int ypos, chtype *string, int align, int start, int end)
{
   /* Declare local variables. */
   int diff		= 0;
   int display		= 0;
   int x		= 0;

   /* Determine how much we need to display. */
   if (end >= start)
   {
      diff = end - start;
   }

   /* Check the alignment of the message. */
   if (align == HORIZONTAL)
   {
      /* Draw the message on a horizontal axis. */
      display = MINIMUM(diff,getmaxx(window)-xpos);
      for (x=0; x < display; x++)
      {
	 mvwaddch (window, ypos, xpos+x, string[x+start]);
      }
   }
   else
   {
      /* Draw the message on a vertical axis. */
      display = MINIMUM(diff,getmaxy(window)-ypos);
      for (x=0; x < display; x++)
      {
	 mvwaddch (window, ypos+x, xpos, string[x+start]);
      }
   }
}

/*
 * This writes out a chtype * string forcing the chtype string
 * to be printed out with the given attributes instead.
 */
void writeChtypeAttrib (WINDOW *window, int xpos, int ypos, chtype *string, chtype attr, int align, int start, int end)
{
   /* Declare local variables. */
   int diff		= 0;
   int display		= 0;
   int x		= 0;
   chtype plain;

   /* Determine how much we need to display. */
   if ( end >= start)
   {
      diff = end - start;
   }

   /* Check the alignment of the message. */
   if (align == HORIZONTAL)
   {
      /* Draw the message on a horizontal axis. */
      display = MINIMUM(diff,getmaxx(window)-xpos);
      for (x=0; x < display; x++)
      {
	 plain = string[x+start] & A_CHARTEXT;
	 mvwaddch (window, ypos, xpos+x, plain | attr);
      }
   }
   else
   {
      /* Draw the message on a vertical axis. */
      display = MINIMUM(diff,getmaxy(window)-ypos);
      for (x=0; x < display; x++)
      {
	 plain = string[x+start] & A_CHARTEXT;
	 mvwaddch (window, ypos+x, xpos, plain | attr);
      }
   }
}

/*
 * This pops up a message.
 */
void popupLabel (CDKSCREEN *screen, char **mesg, int count)
{
   /* Declare local variables. */
   CDKLABEL *popup = 0;

   /* Create the label. */
   popup = newCDKLabel (screen, CENTER, CENTER, mesg, count, TRUE, FALSE);

   /* Draw it on the screen. */
   drawCDKLabel (popup, TRUE);

   /* Wait for some input. */
   keypad (popup->win, TRUE);
   wgetch (popup->win);

   /* Kill it. */
   destroyCDKLabel (popup);

   /* Clean the screen. */
   eraseCDKScreen (screen);
   refreshCDKScreen (screen);
}

/*
 * This pops up a dialog box.
 */
int popupDialog (CDKSCREEN *screen, char **mesg, int mesgCount, char **buttons, int buttonCount)
{
   /* Declare local variables. */
   CDKDIALOG *popup	= 0;
   int choice;

   /* Create the dialog box. */
   popup = newCDKDialog (screen, CENTER, CENTER,
				mesg, mesgCount,
				buttons, buttonCount,
				A_REVERSE, TRUE,
				TRUE, FALSE);

   /* Activate the dialog box */
   drawCDKDialog (popup, TRUE);

   /* Get the choice. */
   choice = activateCDKDialog (popup, 0);

   /* Destroy the dialog box. */
   destroyCDKDialog (popup);

   /* Clean the screen. */
   eraseCDKScreen (screen);
   refreshCDKScreen (screen);

   /* Return the choice. */
   return choice;
}

/*
 * This returns the a selected value in a list.
 */
int getListIndex (CDKSCREEN *screen, char *title, char **list, int listSize, boolean numbers)
{
   CDKSCROLL *scrollp	= 0;
   int selected		= -1;
   int height		= 10;
   int width		= -1;
   int len		= 0;
   int x;

   /* Determine the height of the list. */
   if (listSize < 10)
   {
      height = listSize + (title == 0 ? 2 : 3);
   }

   /* Determine the width of the list. */
   for (x=0; x < listSize; x++)
   {
      int temp = strlen (list[x]) + 10;
      width = MAXIMUM (width, temp);
   }
   if (title != 0)
   {
      len = strlen (title);
   }
   width = MAXIMUM (width, len);
   width += 5;

   /* Create the scrolling list. */
   scrollp = newCDKScroll (screen, CENTER, CENTER, RIGHT,
				height, width, title,
				list, listSize, numbers,
				A_REVERSE, TRUE, FALSE);

   /* Check if we made the list. */
   if (scrollp == 0)
   {
      refreshCDKScreen (screen);
      return -1;
   }

   /* Let the user play. */
   selected = activateCDKScroll (scrollp, 0);

   /* Check how they exited. */
   if (scrollp->exitType != vNORMAL)
   {
      selected = -1;
   }

   /* Clean up. */
   destroyCDKScroll (scrollp);
   refreshCDKScreen (screen);
   return selected;
}

/*
 * This gets information from a user.
 */
char *getString (CDKSCREEN *screen, char *title, char *label, char *initValue)
{
   CDKENTRY *widget	= 0;
   char *value		= 0;

   /* Create the widget. */
   widget = newCDKEntry (screen, CENTER, CENTER, title, label,
				A_NORMAL, '.', vMIXED, 40, 0,
				5000, TRUE, FALSE);

   /* Set the default value. */
   setCDKEntryValue (widget, initValue);

   /* Get the string. */
   value = activateCDKEntry (widget, 0);

   /* Make sure they exited normally. */
   if (widget->exitType != vNORMAL)
   {
      destroyCDKEntry (widget);
      return 0;
   }

   /* Return a copy of the string typed in. */
   value = copyChar (getCDKEntryValue (widget));
   destroyCDKEntry (widget);
   return value;
}

/*
 * This allows the user to view a file.
 */
int viewFile (CDKSCREEN *screen, char *title, char *filename, char **buttons, int buttonCount)
{
   CDKVIEWER *viewer	= 0;
   int selected		= -1;
   int lines		= 0;
   char **info		= 0;

   /* Open the file and read the contents. */
   lines = CDKreadFile (filename, &info);

   /* If we couldn't read the file, return an error. */
   if (lines == -1)
   {
      return (lines);
   }

   /* Create the file viewer to view the file selected.*/
   viewer = newCDKViewer (screen, CENTER, CENTER, -6, -16,
				buttons, buttonCount,
				A_REVERSE, TRUE, TRUE);

   /* Set up the viewer title, and the contents to the widget. */
   setCDKViewer (viewer, title, info, lines, A_REVERSE, TRUE, TRUE, TRUE);

   /* Activate the viewer widget. */
   selected = activateCDKViewer (viewer, 0);

   /* Clean up. */
   CDKfreeStrings (info);

   /* Make sure they exited normally. */
   if (viewer->exitType != vNORMAL)
   {
      destroyCDKViewer (viewer);
      return (-1);
   }

   /* Clean up and return the button index selected. */
   destroyCDKViewer (viewer);
   return selected;
}

/*
 * This allows the user to view information.
 */
int viewInfo (CDKSCREEN *screen, char *title, char **info, int count, char **buttons, int buttonCount, boolean interpret)
{
   CDKVIEWER *viewer	= 0;
   int selected		= -1;

   /* Create the file viewer to view the file selected.*/
   viewer = newCDKViewer (screen, CENTER, CENTER, -6, -16,
				buttons, buttonCount,
				A_REVERSE, TRUE, TRUE);

   /* Set up the viewer title, and the contents to the widget. */
   setCDKViewer (viewer, title, info, count, A_REVERSE, interpret, TRUE, TRUE);

   /* Activate the viewer widget. */
   selected = activateCDKViewer (viewer, 0);

   /* Make sure they exited normally. */
   if (viewer->exitType != vNORMAL)
   {
      destroyCDKViewer (viewer);
      return (-1);
   }

   /* Clean up and return the button index selected. */
   destroyCDKViewer (viewer);
   return selected;
}

/*
 * This allows a person to select a file.
 */
char *selectFile (CDKSCREEN *screen, char *title)
{
   CDKFSELECT *fselect	= 0;
   char *filename	= 0;
   char *holder		= 0;

   /* Create the file selector. */
   fselect = newCDKFselect (screen, CENTER, CENTER, -4, -20,
				title, "File: ",
				A_NORMAL, '_', A_REVERSE,
				"</5>", "</48>", "</N>", "</N>",
				TRUE, FALSE);

   /* Let the user play. */
   holder = activateCDKFselect (fselect, 0);

   /* Check the way the user exited the selector. */
   if (fselect->exitType != vNORMAL)
   {
      destroyCDKFselect (fselect);
      refreshCDKScreen (screen);
      return (0);
   }

   /* Otherwise... */
   filename = strdup (holder);
   destroyCDKFselect (fselect);
   refreshCDKScreen (screen);
   return (filename);
}
