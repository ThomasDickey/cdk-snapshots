#include "cdk.h"

/*
 * $Author: tom $
 * $Date: 1999/05/15 13:21:10 $
 * $Revision: 1.135 $
 */

/*
 * As much as I hate to do this, I need to define a global char * pointer.
 * This is for the injectCDKXXX routines which return a char * type. Since
 * returning a NULL pointer is acceptable, this is the only other solution.
 */
char *GCdkObjects[] = {"", "ENTRY", "MENTRY", "LABEL", "SCROLL", "DIALOG", \
			"SCALE", "MARQUEE", "MENU", "MATRIX", "HISTOGRAM", \
			"SELECTION", "VIEWER", "GRAPH", "RADIO", "TEMPLATE", \
			"SWINDOW", "ITEMLIST", "FSELECT", "SLIDER", \
			"ALPHALIST", "CALENDAR"};
char *GPasteBuffer = (char *)NULL;

/*
 * This beeps then flushes the stdout stream.
 */
void Beep(void)
{
   beep();
   fflush (stdout);
}

/*
 * This sets a string to the given character.
 */
void cleanChar (char *s, int len, char character)
{
   int x;
   for (x=0; x < len; x++)
   {
      s[x] = character;
   }
   s[--x] = '\0';
}

void cleanChtype (chtype *s, int len, chtype character)
{
   int x;
   for (x=0; x < len; x++)
   {
      s[x] = character;
   }
   s[--x] = '\0';
}

/*
 * This takes an x and y position and realigns the values iff they sent in
 * values like CENTER, LEFT, RIGHT, ...
 */
void alignxy (WINDOW *window, int *xpos, int *ypos, int boxWidth, int boxHeight)
{
   /* Justify the X position, if we need to.  */
   if ((*xpos) == LEFT)
   {
      (*xpos) = window->_begx + 1;
   }
   else if ((*xpos) == RIGHT)
   {
      (*xpos) = (window->_maxx - boxWidth) + window->_begx - 1;
   }
   else if ((*xpos) == CENTER)
   {
      (*xpos) = (int)(((window->_maxx)-boxWidth) / 2) + window->_begx;
   }
   else
   {
      /* If we have gone out of bounds, then readjust.  */
      if (((*xpos) + boxWidth) > window->_maxx)
      {
         (*xpos) = (window->_maxx - boxWidth);
      }
      else if ((*xpos) < 0)
      {
         (*xpos) = window->_begx + 1;
      }
      else
      {
         (*xpos) = window->_begx + (*xpos);
      }
   }

   /* Justify the Y position, if we need to.  */
   if ((*ypos) == TOP)
   {
      (*ypos) = window->_begy + 1;
   }
   else if ((*ypos) == BOTTOM)
   {
      (*ypos) = (window->_maxy) - boxHeight + window->_begy - 1;
   }
   else if ((*ypos) == CENTER)
   {
      (*ypos) = (int)((window->_maxy - boxHeight) / 2) + window->_begy;
   }
   else
   {
      /* If we have gone out of bounds, then readjust.  */
      if (((*ypos) + boxHeight) > window->_maxy)
      {
         (*ypos) = (window->_maxy - boxHeight);
      }
      else if ((*ypos) < 0)
      {
         (*ypos) = window->_begy + 1;
      }
      else
      {
         (*ypos) = window->_begy + (*ypos);
      }
   }
}

/*
 * This takes a string, a field width and a justifycation type
 * and returns the justifcation adjustment to make, to fill
 * the justification requirement.
 */
int justifyString (int boxWidth, int mesgLength, int justify)
{
  /*
   * Make sure the message isn't longer than the width.
   * If it is, return 1.
    */
   if (mesgLength >= boxWidth)
   {
      return (1);
   }

   /* Try to justify the message.  */
   if (justify == LEFT)
   {
      return (1);
   }
   else if (justify == RIGHT)
   {
      return ((boxWidth - mesgLength - 1));
   }
   else if (justify == CENTER)
   {
      return ((int)((boxWidth - mesgLength) / 2));
   }
   else
   {
      return (justify);
   }
}

/*
 * This returns a substring of the given string.
 */
char *substring (char *string, int start, int width)
{
   char *newstring	= (char *)NULL;
   int mesglen		= 0;
   int y		= 0;
   int x		= 0;
   int lastchar		= 0;

   /* Make sure the string isn't NULL. */
   if (string == (char *)NULL)
   {
      return (char *)NULL;
   }
   mesglen = (int)strlen (string);

   /* Make sure we start in the correct place. */
   if (start > mesglen)
   {
      return (newstring);
   }

   /* Create the new string. */
   newstring = (char *)malloc (sizeof (char) * (width+3));
   /*cleanChar (newstring, width+3, '\0');*/

   if ((start+width) > mesglen)
   {
      lastchar = mesglen;
   }
   else
   {
      lastchar = width + start;
   }

   for (x=start; x<=lastchar; x++)
   {
      newstring[y++] = string[x];
   }
   newstring[lastchar+1] = '\0';
   newstring[lastchar+2] = '\0';
   return (newstring);
}

/*
 * This frees a string if it is not null. This is a safety
 * measure. Some compilers let you free a null string. I
 * don't like that idea.
 */
void freeChar (char *string)
{
   if (string != (char *)NULL)
   {
      free (string);
      string = (char *)NULL;
   }
}

void freeChtype (chtype *string)
{
   if (string != (chtype *)NULL)
   {
      free (string);
      string = (chtype *)NULL;
   }
}

/*
 * This performs a safe copy of a string. This means it adds the NULL
 * terminator on the end of the string.
 */
char *copyChar (char *original)
{
   /* Declare local variables.  */
   char *newstring;

   /* Make sure the string is not NULL.  */
   if (original == (char *)NULL)
   {
      return (char *)NULL;
   }
   newstring = (char *)malloc (sizeof (char) * ((int)(strlen(original)+1)));

   /* Copy from one to the other.  */
   strcpy (newstring, original);

   /* Return the new string.  */
   return (newstring);
}

chtype *copyChtype (chtype *original)
{
   /* Declare local variables.  */
   chtype *newstring;
   int len, x;

   /* Make sure the string is not NULL.  */
   if (original == (chtype *)NULL)
   {
      return (chtype *)NULL;
   }

   /* Create the new string.  */
   len		= chlen (original);
   newstring	= (chtype *)malloc (sizeof(chtype) * (len+4));
   if (newstring == (chtype *)NULL)
   {
      return (original);
   }
 
   /* Copy from one to the other.  */
   for (x=0; x < len; x++)
   {
      newstring[x] = original[x];
   }
   newstring[len] = '\0';
   newstring[len+1] = '\0';

   /* Return the new string.  */
   return (newstring);
}

/*
 * This reads a file and sticks it into the char ** provided.
 */
int readFile (char *filename, char **array, int maxlines)
{
   FILE	*fd;
   char temp[515];
   int	lines	= 0;

   /* Clean out temp.  */
   cleanChar (temp, 514, '\0');

   /* Can we open the file?  */
   if ((fd = fopen (filename, "r")) == NULL)
   {
      return (-1);
   }
                                                                                
   /* Start reading the file in.  */
   while ((fgets (temp, 512, fd) != (char *)NULL) && lines < maxlines)
   {
      int len		= (int)strlen(temp);
      temp[len-1]	= '\0';
      array[lines]	= copyChar (temp);
      lines++;
   }
   fclose (fd);

   /* Clean up and return.  */
   array[lines] = "";
   return (lines);
}

/*
 * This function takes a character string, full of format markers
 * and translates them into a chtype * array. This is better suited
 * to curses, because curses uses chtype almost exclusively
 */
chtype *char2Chtype (char *string, int *to, int *align)
{
   chtype attrib	= A_NORMAL;
   int insideMarker	= FALSE;
   int start		= 0;
   int from		= 0;
   int pair		= 0;
   int x 		= 3;
   int len		= 0;
   chtype newstring[500], lastChar;
   char temp[50];
   int adjust;

   /* Is the string NULL?  */
   if (string == (char *)NULL)
   {
      (*align) = LEFT;
      (*to) = 0;
      return ((chtype *)NULL);
   }
   len = (int)strlen(string);

   /* Set up some default values.  */
   (*to)		= 0;
   (*align)		= LEFT;
   temp[0]		= '\0';
   pair			= 0;

   /* Lets make some room for the chtype string.  */
   /*cleanChtype (newstring, 500, '\0');*/

   /* Look for an alignment marker.  */
   if (string[0] == '<' && string[1] == 'C' && string[2] == '>')
   {
      (*align)	= CENTER;
      start	= 3;
   }
   else if (string[0] == '<' && string[1] == 'R' && string[2] == '>')
   {
      (*align)	= RIGHT;
      start	= 3;
   }
   else if (string[0] == '<' && string[1] == 'L' && string[2] == '>')
   {
      (*align)	= LEFT;
      start	= 3;
   }
   else if (string[0] == '<' && string[1] == 'B' && string[2] == '=')
   {
      /* Set the item index value in the string.  */
      newstring[0] = ' ';
      newstring[1] = ' ';
      newstring[2] = ' ';

      /* Pull out the bullet marker.  */
      while (string[x] != '>')
      {
         newstring[x] = string[x] | A_BOLD;
         x++;
      }
      newstring[x++] = ' ';

      /* Set the alignment variables.  */
      (*align)	= LEFT;
      start	= x;
      (*to)	= x;
   }
   else if (string[0] == '<' && string[1] == 'I' && string[2] == '=')
   {
      cleanChar (temp, 49, '\0');
      from=2;
      x=0;

      /* Strip out the number.  */
      while (string[++from] != '>')
      {
         if (isdigit((int)string[from]))
         {
            temp[x++] = string[from];
         }
      }

      /* Set the alignment variables.  */
      (*align)	= LEFT;
      start	= x + 4;
      
      /* Add on the spaces.  */
      adjust = atoi (temp);
      for (x=0; x < adjust; x++)
      {
         newstring[(*to)++] = ' ';
      }
   }
      
   /* Set the format marker boolean to false.  */
   insideMarker	= FALSE;

   /* Start parsing the character string.  */
   for (from=start; from < len; from++)
   {
      /* Are we inside a format marker?  */
      if (! insideMarker)
      {
         if (string[from] == '<' && string[from+1] == '/')
         {
            insideMarker = 1;
         }
         else if (string[from] == '<' && string[from+1] == '!')
         {
            insideMarker = 2;
         }
         else if (string[from] == '<' && string[from+1] == '#')
         {
            insideMarker = 3;
         }
         else if (string[from] == '\\' && string[from+1] == '<')
         {
            from++;
            newstring[(*to)++]	= (A_CHARTEXT & string[from++]) | attrib;
         }
         else if (string[from] == '\t')
         {
            for (x=0; x < 8; x++)
            {
               newstring[(*to)++] = ' ';
            }
         }
         else
         {
            newstring[(*to)++]	= (A_CHARTEXT & string[from]) | attrib;
         }
      }
      else
      {
         /* We are in the format marker.  */
         if (string[from] == '>')
         {
            insideMarker = 0;
         }
         else if (string[from] == '#')
         {
            lastChar = (chtype)NULL;
            if (string[from+1] == 'L' && string[from+2] == 'L')
            {
               newstring[(*to)++] = ACS_LLCORNER | attrib;
               lastChar = ACS_LLCORNER;
               from += 2;
            }
            else if (string[from+1] == 'L' && string[from+2] == 'R')
            {
               newstring[(*to)++] = ACS_LRCORNER | attrib;
               lastChar = ACS_LRCORNER;
               from += 2;
            }
            else if (string[from+1] == 'U' && string[from+2] == 'R')
            {
               newstring[(*to)++] = ACS_URCORNER | attrib;
               lastChar = ACS_URCORNER;
               from += 2;
            }
            else if (string[from+1] == 'U' && string[from+2] == 'L')
            {
               newstring[(*to)++] = ACS_ULCORNER | attrib;
               lastChar = ACS_ULCORNER;
               from += 2;
            }
            else if (string[from+1] == 'R' && string[from+2] == 'T')
            {
               newstring[(*to)++] = ACS_RTEE | attrib;
               lastChar = ACS_RTEE;
               from += 2;
            }
            else if (string[from+1] == 'L' && string[from+2] == 'T')
            {
               newstring[(*to)++] = ACS_LTEE | attrib;
               lastChar = ACS_LTEE;
               from += 2;
            }
            else if (string[from+1] == 'B' && string[from+2] == 'T')
            {
               newstring[(*to)++] = ACS_BTEE | attrib;
               lastChar = ACS_BTEE;
               from += 2;
            }
            else if (string[from+1] == 'T' && string[from+2] == 'T')
            {
               newstring[(*to)++] = ACS_TTEE | attrib;
               lastChar = ACS_TTEE;
               from += 2;
            }
            else if (string[from+1] == 'H' && string[from+2] == 'L')
            {
               newstring[(*to)++] = ACS_HLINE | attrib;
               lastChar = ACS_HLINE;
               from += 2;
            }
            else if (string[from+1] == 'V' && string[from+2] == 'L')
            {
               newstring[(*to)++] = ACS_VLINE | attrib;
               lastChar = ACS_VLINE;
               from += 2;
            }
            else if (string[from+1] == 'P' && string[from+2] == 'L')
            {
               newstring[(*to)++] = ACS_PLUS | attrib;
               lastChar = ACS_PLUS;
               from += 2;
            }
            else if (string[from+1] == 'D' && string[from+2] == 'I')
            {
               newstring[(*to)++] = ACS_DIAMOND | attrib;
               lastChar = ACS_DIAMOND;
               from += 2;
            }
            else if (string[from+1] == 'C' && string[from+2] == 'B')
            {
               newstring[(*to)++] = ACS_CKBOARD | attrib;
               lastChar = ACS_CKBOARD;
               from += 2;
            }
            else if (string[from+1] == 'D' && string[from+2] == 'G')
            {
               newstring[(*to)++] = ACS_DEGREE | attrib;
               lastChar = ACS_DEGREE;
               from += 2;
            }
            else if (string[from+1] == 'P' && string[from+2] == 'M')
            {
               newstring[(*to)++] = ACS_PLMINUS | attrib;
               lastChar = ACS_PLMINUS;
               from += 2;
            }
            else if (string[from+1] == 'B' && string[from+2] == 'U')
            {
               newstring[(*to)++] = ACS_BULLET | attrib;
               lastChar = ACS_BULLET;
               from += 2;
            }
            else if (string[from+1] == 'S' && string[from+2] == '1')
            {
               newstring[(*to)++] = ACS_S1 | attrib;
               lastChar = ACS_S1;
               from += 2;
            }
            else if (string[from+1] == 'S' && string[from+2] == '9')
            {
               newstring[(*to)++] = ACS_S9 | attrib;
               lastChar = ACS_S9;
               from += 2;
            }
            else if (string[from+1] == 'L' && string[from+2] == 'A')
            {
               newstring[(*to)++] = ACS_LARROW | attrib;
               lastChar = ACS_LARROW;
               from += 2;
            }
            else if (string[from+1] == 'R' && string[from+2] == 'A')
            {
               newstring[(*to)++] = ACS_RARROW | attrib;
               lastChar = ACS_RARROW;
               from += 2;
            }
            else if (string[from+1] == 'U' && string[from+2] == 'A')
            {
               newstring[(*to)++] = ACS_UARROW | attrib;
               lastChar = ACS_UARROW;
               from += 2;
            }
            else if (string[from+1] == 'D' && string[from+2] == 'A')
            {
               newstring[(*to)++] = ACS_DARROW | attrib;
               lastChar = ACS_DARROW;
               from += 2;
            }

            /* Check for a possible numeric modifier.  */
            if (string[from+1] == '(' && lastChar != (chtype)NULL)
            {
               /* Set up some variables.  */
               cleanChar (temp, 49, '\0');
               from++;
               x=0;

               /* Strip out the number.  */
               while (string[++from] != ')')
               {
                  if (isdigit((int)string[from]))
                  {
                     temp[x++] = string[from];
                  }
               }

               /* Convert the number.  */
               if (x != 0)
               {
                  adjust = atoi (temp);
                  for (x=0; x < adjust; x++)
                  {
                     newstring[(*to)++] = lastChar | attrib;
                  }
               }
            }
         }
         else if (string[from] == '/' && string[from+1] == 'B')
         {
            attrib	= attrib | A_BOLD;
            from++;
         }
         else if (string[from] == '!' && string[from+1] == 'B')
         {
            attrib	= attrib & (~A_BOLD);
            from++;
         }
         else if (string[from] == '/' && string[from+1] == 'U')
         {
            attrib	= attrib | A_UNDERLINE;
            from++;
         }
         else if (string[from] == '!' && string[from+1] == 'U')
         {
            attrib	= attrib & (~A_UNDERLINE);
            from++;
         }
         else if (string[from] == '/' && string[from+1] == 'S')
         {
            attrib	= attrib | A_STANDOUT;
            from++;
         }
         else if (string[from] == '!' && string[from+1] == 'S')
         {
            attrib	= attrib & (~A_STANDOUT);
            from++;
         }
         else if (string[from] == '/' && string[from+1] == 'R')
         {
            attrib	= attrib | A_REVERSE;
            from++;
         }
         else if (string[from] == '!' && string[from+1] == 'R')
         {
            attrib	= attrib & (~A_REVERSE);
            from++;
         }
         else if (string[from] == '/' && string[from+1] == 'K')
         {
            attrib	= attrib | A_BLINK;
            from++;
         }
         else if (string[from] == '!' && string[from+1] == 'K')
         {
            attrib	= attrib & (~A_BLINK);
            from++;
         }
         else if (string[from] == '/' && string[from+1] == 'D')
         {
            attrib	= attrib | A_DIM;
            from++;
         }
         else if (string[from] == '!' && string[from+1] == 'D')
         {
            attrib	= attrib & (~A_DIM);
            from++;
         }
         else if (string[from] == '/' && isdigit((int)string[from+1]) && isdigit((int)string[from+2]))
         {
#ifdef HAVE_COLOR
            sprintf (temp, "%c%c", string[from+1],string[from+2]);
            pair	= atoi(temp);
            attrib	= attrib | COLOR_PAIR(pair);
#else
            attrib	= attrib | A_BOLD;
            from += 2;
#endif
         }
         else if (string[from] == '!' && isdigit((int)string[from+1]) && isdigit((int)string[from+2]))
         {
#ifdef HAVE_COLOR
            sprintf (temp, "%c%c", string[from+1],string[from+2]);
            pair	= atoi(temp);
            attrib	= attrib & (~COLOR_PAIR(pair));
#else
            attrib	= attrib & (~A_BOLD);
            from += 2;
#endif
         }
         else if (string[from] == '/' && isdigit((int)string[from+1]))
         {
#ifdef HAVE_COLOR
            pair	= string[++from] - '0';
            attrib	= attrib | COLOR_PAIR(pair);
#else
            attrib	= attrib | A_BOLD;
            from++;
#endif
         }
         else if (string[from] == '!' && isdigit((int)string[from+1]))
         {
#ifdef HAVE_COLOR
            pair	= string[++from] - '0';
            attrib	= attrib & (~COLOR_PAIR(pair));
#else
            attrib	= attrib & (~A_BOLD);
            from++;
#endif
         }
      }
   }

   /* Add the NULL character to the end of the string. */
   newstring[(*to)] = '\0';
   newstring[(*to)+1] = '\0';

   /*
    * If there are no characters, put the attribute into the
    * the first character of the array.
     */
   if (chlen(newstring) == 0)
   {
      newstring[0] = attrib;
   }

   /* Return the new string.  */
   return (copyChtype (newstring));
}

/*
 * This determines the length of a chtype string
 */
int chlen (chtype *string)
{
   /* Declare local variables.  */
   int x = 0;

   /* Make sure we wern't given a NULL string.  */
   if (string == (chtype *)NULL)
   {
      return (0);
   }

   /* Find out how long this string is.  */
   while (string[x++] != (chtype)NULL);
   return ((x-1));
}

/*
 * This returns a pointer to char * of a chtype *
 */
char *chtype2Char (chtype *string)
{
   /* Declare local variables.  */
   char *newstring;
   int len = 0;
   int x;

   /* Is the string NULL?  */
   if (string == (chtype *)NULL)
   {
      return ((char *)NULL);
   }

   /* Get the length of the string.  */
   len = chlen(string);

   /* Make the new string.  */
   newstring = (char *)malloc (sizeof (char) * (len+1));
   /*cleanChar (newstring, len+1, '\0');*/

   /* Start translating.  */
   for (x=0; x < len; x++)
   {
      newstring[x] = (char)(string[x] & A_CHARTEXT);
   }

   /* Force a NULL character on the end of the string.  */
   newstring[len] = '\0';

   /* Return it.  */
   return (newstring);
}

/*
 * This takes a character pointer and returns the equivalent
 * display type.
 */
EDisplayType char2DisplayType (char *string)
{
   /* Make sure we cover our bases... */
   if (string == (char *)NULL)
   {
      return (EDisplayType)vINVALID;
   }

   /* Start looking. */
   if (strcmp (string, "CHAR") == 0)
   {
      return (EDisplayType)vCHAR;
   }
   else if (strcmp (string, "HCHAR") == 0)
   {
      return (EDisplayType)vHCHAR;
   }
   else if (strcmp (string, "INT") == 0)
   {
      return (EDisplayType)vINT;
   }
   else if (strcmp (string, "HINT") == 0)
   {
      return (EDisplayType)vHINT;
   }
   else if (strcmp (string, "UCHAR") == 0)
   {
      return (EDisplayType)vUCHAR;
   }
   else if (strcmp (string, "LCHAR") == 0)
   {
      return (EDisplayType)vLCHAR;
   }
   else if (strcmp (string, "UHCHAR") == 0)
   {
      return (EDisplayType)vUHCHAR;
   }
   else if (strcmp (string, "LHCHAR") == 0)
   {
      return (EDisplayType)vLHCHAR;
   }
   else if (strcmp (string, "MIXED") == 0)
   {
      return (EDisplayType)vMIXED;
   }
   else if (strcmp (string, "HMIXED") == 0)
   {
      return (EDisplayType)vHMIXED;
   }
   else if (strcmp (string, "UMIXED") == 0)
   {
      return (EDisplayType)vUMIXED;
   }
   else if (strcmp (string, "LMIXED") == 0)
   {
      return (EDisplayType)vLMIXED;
   }
   else if (strcmp (string, "UHMIXED") == 0)
   {
      return (EDisplayType)vUHMIXED;
   }
   else if (strcmp (string, "LHMIXED") == 0)
   {
      return (EDisplayType)vLHMIXED;
   }
   else if (strcmp (string, "VIEWONLY") == 0)
   {
      return (EDisplayType)vVIEWONLY;
   }
   else
   {
      return (EDisplayType)vINVALID;
   }
}

/*
 * This swaps two elements in an array.
 */
void swapIndex (char *list[], int i, int j)
{
   char *temp;
   temp = list[i];
   list[i] = list[j];
   list[j] = temp;
}

/*
 * This function is a quick sort alg which sort an array of
 * char *. I wanted to use to stdlib qsort, but couldn't get the
 * thing to work, so I wrote my own. I'll use qsort if I can get
 * it to work.
 */
void quickSort (char *list[], int left, int right)
{
   int i, last;

   /* If there are fewer than 2 elements, return.  */
   if (left >= right)
   {
      return;
   }

   swapIndex (list, left, (left+right)/2);
   last = left;
   
   for (i=left+1; i <= right; i++)
   {
      if (strcmp (list[i], list[left]) < 0)
      {
         swapIndex (list, ++last, i);
      }
   }

   swapIndex (list, left, last);
   quickSort (list, left, last-1);
   quickSort (list, last+1, right);
}

/*
 * This strips white space off of the given string.
 */
void stripWhiteSpace (EStripType stripType, char *string)
{
   /* Declare local variables.  */
   int stringLength = 0;
   int alphaChar = 0;
   int x = 0;
   
   /* Make sure the string is not NULL.  */
   if (string == (char *)NULL)
   {
      return;
   }

   /* Get the length of the string.  */
   stringLength = (int)strlen(string);
   if (stringLength == 0)
   {
      return;
   }

   /* Strip the white space from the front.  */
   if (stripType == vFRONT || stripType == vBOTH)
   {
      /* Find the first non-whitespace character.  */
      while (string[alphaChar] == ' ' || string[alphaChar] == '\t')
      {
         alphaChar++;
      }

      /* Trim off the white space.  */
      if (alphaChar != stringLength)
      {
         for (x=0; x < (stringLength-alphaChar); x++)
         {
            string[x] = string[x+alphaChar];
         }
         string[stringLength-alphaChar] = '\0';
      }
      else
      {
         /* Set the string to zero.  */
         string = (char *)memset (string, 0, stringLength);
      }
   }

   /* Get the length of the string.  */
   stringLength = (int)strlen(string)-1;

   /* Strip the space from behind if it was asked for.  */
   if (stripType == vBACK || stripType == vBOTH)
   {
      /* Find the first non-whitespace character.  */
      while (string[stringLength] == ' ' || string[stringLength] == '\t')
      {
         string[stringLength--] = '\0';
      }
   }
}

/*
 * This splits a string into X parts given the split character.
 */
int splitString (char *string, char *items[], char splitChar)
{
   /* Declare local variables.  */
   char temp[1024];
   int stringLength = 0;
   int chunks = 0;
   int pos = 0;
   int x;

   /* Check if the given string is NULL.  */
   if (string == (char *)NULL)
   {
      return (0);
   }

   /* Get the length of the string.  */
   stringLength = (int)strlen (string);
   if (stringLength == 0)
   {
      return (0);
   }

   /* Zero out the temp string. */
   cleanChar (temp, 1024, '\0');

   /* Start looking for the string. */
   for (x=0; x < stringLength; x++)
   {
      if (string[x] == splitChar)
      {
         /* Copy the string into the array. */
         items[chunks++] = strdup (temp);

         /* Zero out the temp string.  */
         cleanChar (temp, 1024, '\0');

         /* Reset the position counter.  */
         pos = 0;
      }
      else
      {
         temp[pos++] = string[x];
      }
   }

   /* Don't forget the last one.  */
   items[chunks++] = strdup (temp);
   return chunks;
}

/*
 * This function takes a mode_t type and creates a string represntation
 * of the permission mode. 
 */
int mode2Char (char *string, mode_t mode)
{
   /* Declare local variables.  */
   int permissions = 0;

   /* Clean the string.  */
   cleanChar (string, 11, '-');
   string[11] = '\0';

   /* Determine the file type.  */
   if (S_ISLNK (mode))
   {
      string[0] = 'l';
   }
   else if (S_ISSOCK (mode))
   {
      string[0] = '@';
   }
   else if (S_ISREG(mode))
   {
      string[0] = '-';
   }
   else if (S_ISDIR(mode))
   {
      string[0] = 'd';
   }
   else if (S_ISCHR(mode))
   {
      string[0] = 'c';
   }
   else if (S_ISBLK(mode))
   {
      string[0] = 'b';
   }
   else if (S_ISFIFO(mode))
   {
      string[0] = '&';
   }
   else
   {
      return -1;
   }

   /* Readable by owner?  */
   if ((mode&S_IRUSR) != 0)
   {
      string[1] = 'r';
      permissions += 400;
   }

   /* Writable by owner?  */
   if ((mode & S_IWUSR) != 0)
   {
      string[2] = 'w';
      permissions += 200;
   }

   /* Executable by owner?  */
   if ((mode & S_IXUSR) != 0)
   {
      string[3] = 'x';
      permissions += 100;
   }

   /* Readable by group?  */
   if ((mode & S_IRGRP) != 0)
   {
      string[4] = 'r';
      permissions += 40;
   }

   /* Writable by group?  */
   if ((mode & S_IWGRP) != 0)
   {
      string[5] = 'w';
      permissions += 20;
   }

   /* Executable by group?  */
   if ((mode & S_IXGRP) != 0)
   {
      string[6] = 'x';
      permissions += 10;
   }

   /* Readable by other?  */
   if ((mode & S_IROTH) != 0)
   {
      string[7] = 'r';
      permissions += 4;
   }

   /* Writable by other?  */
   if ((mode & S_IWOTH) != 0)
   {
      string[8] = 'w';
      permissions += 2;
   }

   /* Executable by other?  */
   if ((mode & S_IXOTH) != 0)
   {
      string[9] = 'x';
      permissions += 1;
   }

   /* Check for suid.  */
   if ((mode & S_ISUID) != 0)
   {
      string[3] = 's';
      permissions += 4000;
   }

   /* Check for sgid.  */
   if ((mode & S_ISGID) != 0)
   {
      string[6] = 's';
      permissions += 2000;
   }

   /* Check for sticky bit.  */
   if ((mode & S_ISVTX) != 0)
   {
      string[0] = 't';
      permissions += 1000;
   }

   /* Check for unusual permissions.  */
   if (((mode & S_IXUSR) == 0) &&
	((mode & S_IXGRP) == 0) &&
	((mode & S_IXOTH) == 0) &&
	(mode & S_ISUID) != 0)
   {
      string[3] = 'S';
   }

   return permissions;
}

/*
 * This returns the length of the integer.
 */
int intlen (int value)
{
   int len = (int)log10 (value) + 1;
   return len;
}

/*
 * This opens the current directory and reads the contents.
 */
int getDirectoryContents (char *directory, char **list, int maxListSize)
{
   /* Declare local variables.  */
   struct dirent *dirStruct;
   int counter = 0;
   DIR *dp;

   /* Open the directory.  */
   dp = opendir (directory);

   /* Could we open the directory?  */
   if (dp == NULL)
   {
      return -1;
   }

   /* Read the directory.  */
   while ((dirStruct = readdir (dp)) != NULL)
   {
      if (counter <= maxListSize)
      {
         list[counter++] = copyChar (dirStruct->d_name);
      }
   }

   /* Close the directory.  */
   closedir (dp);

   /* Sort the info.  */
   quickSort (list, 0, counter-1);

   /* Return the number of files in the directory.  */
   return counter;
}

/* 
 * This looks for a subset of a word in the given list.
 */
int searchList (char **list, int listSize, char *pattern)
{
   /* Declare local variables.  */
   int len	= 0;
   int Index	= -1;
   int x, ret;

   /* Make sure the pattern isn't NULL. */
   if (pattern == (char *)NULL)
   {
      return Index;
   }
   len = (int)strlen (pattern);

   /* Cycle through the list looking for the word. */
   for (x=0; x < listSize; x++)
   {
      /* Do a string compare. */
      ret = strncmp (list[x], pattern, len);

     /*
      * If 'ret' is less than 0, then the current word is
      * alphabetically less than the provided word. At this
      * point we will set the index to the current position.
      * If 'ret' is greater than 0, then the current word is
      * alphabettically greater than the given word. We should
      * return with index, which might contain the last best 
      * match. If they are equal, then we've found it.
      */
      if (ret < 0)
      {
         Index = ret;
      }
      else if (ret > 0)
      {
         return Index;
      }
      else
      {
         return x;
      }
   }
   return -1;
}

/*
 * This function checks to see if a link has been requested.
 */
int checkForLink (char *line, char *filename)
{
   int len	= 0;
   int fPos	= 0;
   int x	= 3;

   /* Make sure the line isn't NULL. */
   if ((line == (char *)NULL) || (strlen (line) == 0))
   {
      return -1;
   }
  len = (int)strlen (line);

   /* Strip out the filename. */
   if (line[0] == '<' && line[1] == 'F' && line[2] == '=')
   {
      /* Strip out the filename.  */
      while (x < len)
      {
         if (line[x] == '>')
         {
            break;
         }
         filename[fPos++] = line[x++];
      }
      filename[fPos] = '\0';
      return 1;
   }
   return 0;
}

/*
 * This strips out the filename from the pathname. I would have
 * used rindex but it seems that not all the C libraries support
 * it. :(
 */
char *baseName (char *pathname)
{
   char *base		= (char *)NULL;
   int pathLen		= 0;
   int pos		= 0;
   int Index		= -1;
   int x		= 0;

   /* Check if the string is NULL.  */
   if (pathname == (char *)NULL)
   {
      return (char *)NULL;
   }
   base = copyChar (pathname);
   pathLen = (int)strlen (pathname);

   /* Find the last '/' in the pathname. */
   x = pathLen - 1;
   while ((pathname[x] != '\0') && (Index == -1) && (x > 0))
   {
      if (pathname[x] == '/')
      {
         Index = x;
         break;
      }
      x--;
   }

  /*
   * If the index is -1, we never found one. Return a pointer
   * to the string given to us.
   */
   if (Index == -1)
   {
      return base;
   }

   /* Clean out the base pointer. */
   memset (base, '\0', pathLen);

  /*
   * We have found an index. Copy from the index to the
   * end of the string into a new string.
   */
   for (x=Index+1; x < pathLen; x++)
   {
      base[pos++] = pathname[x];
   }
   return base;
}

/*
 * This strips out the directory from the pathname. I would have
 * used rindex but it seems that not all the C libraries support
 * it. :(
 */
char *dirName (char *pathname)
{
   char *dir		= (char *)NULL;
   int pathLen		= 0;
   int x		= 0;

   /* Check if the string is NULL.  */
   if (pathname == (char *)NULL)
   {
      return (char *)NULL;
   }
   dir = copyChar (pathname);
   pathLen = (int)strlen (pathname);

   /* Starting from the end, look for the first '/' character. */
   x = pathLen;
   while ((dir[x] != '/') && (x > 0))
   {
      dir[x--] = '\0';
   }

   /* Now dir either has nothing or the basename. */
   if (dir[0] == '\0')
   {
      /* If it has nothing, return nothing. */
      return copyChar ("");
   }

   /* Otherwise, return the path. */
   return dir;
}

/*
 * If the dimension is a negative value, the dimension will
 * be the full height/width of the parent window - the value
 * of the dimension. Otherwise, the dimension will be the 
 * given value.
 */
int setWidgetDimension (int parentDim, int proposedDim, int adjustment)
{
   int dimension = 0;

   /* If the user passed in FULL, return the number of rows. */
   if ((proposedDim == FULL) || (proposedDim == 0))
   {
      return parentDim;
   }

   /* If they gave a positive value, return it. */
   if (proposedDim >= 0)
   {
      if (proposedDim >= parentDim)
      {
         return parentDim;
      }
      return (proposedDim + adjustment);
   }

  /*
   * If they gave a negative value, then return the
   * dimension of the parent minus the value given.
   */
   dimension = parentDim + proposedDim;

   /* Just to make sure. */
   if (dimension < 0)
   {
      return parentDim;
   }
   return dimension;
}

/*
 * This safely erases a given window.
 */
void eraseCursesWindow (WINDOW *window)
{
   if (window != (WINDOW *)NULL)
   {
      werase (window);
      touchwin (window);
      wrefresh (window);
   }
}

/*
 * This safely deletes a given window.
 */
void deleteCursesWindow (WINDOW *window)
{
   if (window != (WINDOW *)NULL)
   {
      delwin (window);
      window = (WINDOW *)NULL;
   }
}

#ifndef HAVE_USLEEP
void usleep (int delay);
void usleep (int delay)
{
   int x;
   for (x = 0; x <= (delay * 250) ; x++) {}
}
#endif
