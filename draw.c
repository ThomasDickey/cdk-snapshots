#include <cdk_int.h>

/*
 * $Author: tom $
 * $Date: 2004/08/31 01:52:59 $
 * $Revision: 1.56 $
 */

/*
 * This sets up a basic set of color pairs. These can be redefined
 * if wanted...
 */
void initCDKColor (void)
{
#ifdef HAVE_START_COLOR
   int color[]	= {
			COLOR_WHITE,	COLOR_RED,	COLOR_GREEN,
			COLOR_YELLOW,	COLOR_BLUE,	COLOR_MAGENTA,
			COLOR_CYAN,	COLOR_BLACK
		};
   int pair = 1;
   int fg, bg;
   int limit;

   if (has_colors()) {
      start_color();

      limit = (COLORS < 8) ? COLORS : 8;

      /* Create the color pairs. */
      for (fg=0; fg < limit; fg++)
      {
	 for (bg=0; bg < limit; bg++)
	 {
	    init_pair (pair++, color[fg], color[bg]);
	 }
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

   /* Draw horizontal lines. */
   mvwhline(window, tly, 0, ACS_HLINE | attr, getmaxx(window));
   mvwhline(window, bry, 0, ACS_HLINE | attr, getmaxx(window));

   /* Draw vertical lines. */
   mvwvline(window, 0, tlx, ACS_VLINE | attr, getmaxy(window));
   mvwvline(window, 0, brx, ACS_VLINE | attr, getmaxy(window));

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

   /* Draw horizontal lines. */
   if (horz != 0)
   {
      mvwhline(win, y1, 0, horz | attr, getmaxx(win));
      mvwhline(win, y2, 0, horz | attr, getmaxx(win));
      count++;
   }

   /* Draw vertical lines. */
   if (vert != 0)
   {
      mvwvline(win, 0, x1, vert | attr, getmaxy(win));
      mvwvline(win, 0, x2, vert | attr, getmaxy(win));
      count++;
   }

   /* Draw in the corners. */
   if (tlc != 0)
   {
      mvwaddch (win, y1, x1, tlc | attr);
      count++;
   }
   if (trc != 0)
   {
      mvwaddch (win, y1, x2, trc | attr);
      count++;
   }
   if (blc != 0)
   {
      mvwaddch (win, y2, x1, blc | attr);
      count++;
   }
   if (brc != 0)
   {
      mvwaddch (win, y2, x2, brc | attr);
      count++;
   }
   if (count != 0)
   {
      wrefresh (win);
   }
}

/*
 * Draw a box around the given window using the object's defined line-drawing
 * characters.
 */
void drawObjBox (WINDOW *win, CDKOBJS *object)
{
   attrbox (win,
	    object->ULChar,
	    object->URChar,
	    object->LLChar,
	    object->LRChar,
	    object->HZChar,
	    object->VTChar,
	    object->BXAttr);
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
      if (xdiff > 0)
	 mvwhline (window, starty, startx, line, xdiff);
   }
   else if (xdiff == 0)
   {
      if (ydiff > 0)
	 mvwvline (window, starty, startx, line, ydiff);
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
      int x_hi = getmaxx(shadowWin) - 1;
      int y_hi = getmaxy(shadowWin) - 1;

      /* Draw the line on the bottom. */
      mvwhline(shadowWin, y_hi, 1, ACS_HLINE | A_DIM, x_hi);

      /* Draw the line on the right. */
      mvwvline(shadowWin, 0, x_hi, ACS_VLINE | A_DIM, y_hi);

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
      if (display > 0)
	 mvwaddnstr (window, ypos, xpos, string + start, display);
   }
   else
   {
      /* Draw the message on a vertical axis. */
      display = MINIMUM(display,getmaxy(window)-1);
      for (x=0; x < display ; x++)
      {
	 mvwaddch (window, ypos+x, xpos, CharOf(string[x+start]));
      }
   }
}

void writeBlanks (WINDOW *window, int xpos, int ypos, int align, int start, int end)
{

   if (start < end)
   {
      char *blanks = 0;

      if (blanks == 0 || (int) strlen(blanks) < (end - start))
      {
	 unsigned want = (end - start) + 1000;
	 freeChar (blanks);
	 blanks = (char *)malloc (want);
	 cleanChar (blanks, want-1, ' ');
      }
      writeChar (window, xpos, ypos, blanks, align, start, end);
      freeChar (blanks);
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
	 mvwaddch (window, ypos, xpos+x, CharOf(string[x+start]) | attr);
      }
   }
   else
   {
      /* Draw the message on a vertical axis. */
      display = MINIMUM(display,getmaxy(window)-1);
      for (x=0; x < display ; x++)
      {
	 mvwaddch (window, ypos+x, xpos, CharOf(string[x+start]) | attr);
      }
   }
}

#ifndef HAVE_WADDCHNSTR
/* NetBSD bug */
static int cdk_waddchnstr (WINDOW *window, const chtype *string, int len)
{
   int y, x, n;
   getyx(window, y, x);
   for (n = 0; n < len; ++n)
   {
      if (mvwaddch(window, y, x + n, string[n]) == ERR)
	 return ERR;
   }
   return OK;
}
#undef  waddchnstr
#define waddchnstr(window, string, len) cdk_waddchnstr(window, string, len)
#endif

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
      if (display > 0)
      {
	 wmove(window, ypos, xpos);
	 waddchnstr (window, string + start, display);
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
	 plain = CharOf(string[x+start]);
	 mvwaddch (window, ypos, xpos+x, plain | attr);
      }
   }
   else
   {
      /* Draw the message on a vertical axis. */
      display = MINIMUM(diff,getmaxy(window)-ypos);
      for (x=0; x < display; x++)
      {
	 plain = CharOf(string[x+start]);
	 mvwaddch (window, ypos+x, xpos, plain | attr);
      }
   }
}
