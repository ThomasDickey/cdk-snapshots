#include "cdk.h"

/*
 * $Author: tom $
 * $Date: 1999/05/16 02:28:20 $
 * $Revision: 1.31 $
 */

/*
 * This sets up a basic set of color pairs. These can be redefined
 * if wanted...
 */
void initCDKColor (void)
{
#ifdef HAVE_COLOR
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
   int brx	= window->_maxx - 1;
   int bry	= window->_maxy;
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
#ifdef HAVE_LIBNCURSES
   int y2	= win->_maxy;
   int x2	= win->_maxx;
#else
   int y2	= win->_maxy-1;
   int x2	= win->_maxx-1;
#endif
   int count	= 0;
   int x, y;

   /* Draw horizontal lines. */
   if (horz != (chtype)NULL)
   {
      for (x=x1; x<=x2; x++)
      {
         if (attr != (chtype)NULL)
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
   if (vert != (chtype)NULL)
   {
      for (y=y1; y<=y2; y++)
      {
         if (attr != (chtype)NULL)
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
   if (tlc != (chtype)NULL)
   {
      if (attr != (chtype)NULL)
      {
         mvwaddch (win, y1, x1, tlc | attr);
      }
      else
      {
         mvwaddch (win, y1, x1, tlc);
      }
      count++;
   }
   if (trc != (chtype)NULL)
   {
      if (attr != (chtype)NULL)
      {
         mvwaddch (win, y1, x2, trc | attr);
      }
      else
      {
         mvwaddch (win, y1, x2, trc);
      }
      count++;
   }
   if (blc != (chtype)NULL)
   {
      if (attr != (chtype)NULL)
      {
         mvwaddch (win, y2, x1, blc | attr);
      }
      else
      {
         mvwaddch (win, y2, x1, blc);
      }
      count++;
   }
   if (brc != (chtype)NULL)
   {
      if (attr != (chtype)NULL)
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
   /* De=clare some local vars.	*/
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
      x	= startx;
      y	= starty;
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
   /* Declare local variables. */
   int x = 0;

   /* Make sure the window is not NULL. */
   if (shadowWin == (WINDOW *)NULL)
   {
      return;
   }

   /* Draw the line on the bottom. */
   for (x=1; x <= shadowWin->_maxx; x++)
   {
      mvwaddch (shadowWin, shadowWin->_maxy, x, ACS_HLINE | A_DIM);
   }

   /* Draw the line on the right. */
   for (x=0; x < shadowWin->_maxy; x++)
   {
      mvwaddch (shadowWin, x, shadowWin->_maxx, ACS_VLINE |A_DIM);
   }
   mvwaddch (shadowWin, 0, shadowWin->_maxx, ACS_URCORNER | A_DIM);
   mvwaddch (shadowWin, shadowWin->_maxy, 0, ACS_LLCORNER | A_DIM);
   mvwaddch (shadowWin, shadowWin->_maxy, shadowWin->_maxx, ACS_LRCORNER | A_DIM);
   wrefresh (shadowWin);
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
      display = MINIMUM(display,window->_maxx-1);
      for (x=0; x < display ; x++)
      {
         /* Draw the message on a horizontal axis. */
         mvwaddch (window, ypos, xpos+x, string[x+start] | A_NORMAL);
      }
   }
   else
   {
      /* Draw the message on a vertical axis. */
      display = MINIMUM(display,window->_maxy-1);
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
      display		= MINIMUM(display,window->_maxx-1);
      for (x=0; x < display ; x++)
      {
         mvwaddch (window, ypos, xpos+x, (string[x+start] & A_CHARTEXT) | attr);
      }
   }
   else
   {
      /* Draw the message on a vertical axis. */
      display		= MINIMUM(display,window->_maxy-1);
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
      display = MINIMUM(diff,window->_maxx-1);
      for (x=0; x < display; x++)
      {
         mvwaddch (window, ypos, xpos+x, string[x+start]);
      }
   }
   else
   {
      /* Draw the message on a vertical axis. */
      display = MINIMUM(diff,window->_maxy-1);
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
      display = MINIMUM(diff,window->_maxx-1);
      for (x=0; x < display; x++)
      {
         plain = string[x+start] & A_CHARTEXT;
         mvwaddch (window, ypos, xpos+x, plain | attr);
      }
   }
   else
   {
      /* Draw the message on a vertical axis. */
      display = MINIMUM(diff,window->_maxy-1);
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
   CDKLABEL *popup = (CDKLABEL *)NULL;

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
   CDKDIALOG *popup	= (CDKDIALOG *)NULL;
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
   choice = activateCDKDialog (popup, NULL);

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
   CDKSCROLL *scrollp	= (CDKSCROLL *)NULL;
   int selected		= -1;
   int height		= 10;
   int width		= -1;
   int len		= 0;
   int x;

   /* Determine the height of the list. */
   if (listSize < 10)
   {
      height = listSize + (title == NULL ? 2 : 3);
   }

   /* Determine the width of the list. */
   for (x=0; x < listSize; x++)
   {
      int temp = strlen (list[x]) + 10;
      width = MAXIMUM (width, temp);
   }
   if (title != (char *)NULL)
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
   if (scrollp == (CDKSCROLL *)NULL)
   {
      refreshCDKScreen (screen);
      return -1;
   }

   /* Let the user play. */
   selected = activateCDKScroll (scrollp, (chtype)NULL);

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
   CDKENTRY *widget	= (CDKENTRY *)NULL;
   char *value		= (char *)NULL;

   /* Create the widget. */
   widget = newCDKEntry (screen, CENTER, CENTER, title, label,
				A_NORMAL, '.', vMIXED, 40, 0,
				5000, TRUE, FALSE);

   /* Set the default value. */
   setCDKEntryValue (widget, initValue);

   /* Get the string. */
   value = activateCDKEntry (widget, NULL);

   /* Make sure they exited normally. */
   if (widget->exitType != vNORMAL)
   {
      destroyCDKEntry (widget);
      return (char *)NULL;
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
   CDKVIEWER *viewer	= (CDKVIEWER *)NULL;
   int selected		= -1;
   int lines		= 0;
   char *info[MAX_LINES];
   int x;

   /* Open the file and read the contents. */
   lines = readFile (filename, info, MAX_LINES);

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
   selected = activateCDKViewer (viewer, (chtype *)NULL);

   /* Clean up. */
   for (x=0; x < lines; x++)
   {
      freeChar (info[x]);
   }

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
   CDKVIEWER *viewer	= (CDKVIEWER *)NULL;
   int selected		= -1;

   /* Create the file viewer to view the file selected.*/
   viewer = newCDKViewer (screen, CENTER, CENTER, -6, -16,
				buttons, buttonCount,
				A_REVERSE, TRUE, TRUE);

   /* Set up the viewer title, and the contents to the widget. */
   setCDKViewer (viewer, title, info, count, A_REVERSE, interpret, TRUE, TRUE);

   /* Activate the viewer widget. */
   selected = activateCDKViewer (viewer, (chtype *)NULL);

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
   CDKFSELECT *fselect	= (CDKFSELECT *)NULL;
   char *filename	= (char *)NULL;
   char *holder		= (char *)NULL;

   /* Create the file selector. */
   fselect = newCDKFselect (screen, CENTER, CENTER, -4, -20,
				title, "File: ",
				A_NORMAL, '_', A_REVERSE,
				"</5>", "</48>", "</N>", "</N>",
				TRUE, FALSE);

   /* Let the user play. */
   holder = activateCDKFselect (fselect, (chtype)NULL);

   /* Check the way the user exited the selector. */
   if (fselect->exitType != vNORMAL)
   {
      destroyCDKFselect (fselect);
      refreshCDKScreen (screen);
      return ((char *)NULL);
   }

   /* Otherwise... */
   filename = strdup (holder);
   destroyCDKFselect (fselect);
   refreshCDKScreen (screen);
   return (filename);
}
