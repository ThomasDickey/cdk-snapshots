#include "cdk.h"

/*
 * $Author: tom $
 * $Date: 1999/06/05 21:44:46 $
 * $Revision: 1.149 $
 */

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
#define BORDER 1
void alignxy (WINDOW *window, int *xpos, int *ypos, int boxWidth, int boxHeight)
{
   int first, gap, last;

   first = (getbegx(window) + BORDER);
   last  = (getmaxx(window) - 2*BORDER + 1);
   if ((gap = (last - boxWidth)) < 0) gap = 0;
   last  = first + gap;

   switch (*xpos)
   {
   case LEFT:
      (*xpos) = first;
      break;
   case RIGHT:
      (*xpos) = first + gap;
      break;
   case CENTER:
      (*xpos) = first + (gap / 2);
      break;
   }

   if ((*xpos) > last)
      (*xpos) = last;
   else if ((*xpos) < first)
      (*xpos) = first;

   first = (getbegy(window) + BORDER);
   last  = (getmaxy(window) - 2*BORDER + 1);
   if ((gap = (last - boxHeight)) < 0) gap = 0;
   last  = first + gap;

   switch (*ypos)
   {
   case TOP:
      (*ypos) = first;
      break;
   case BOTTOM:
      (*ypos) = first + gap;
      break;
   case CENTER:
      (*ypos) = first + (gap/2);
      break;
   }

   if ((*ypos) > last)
      (*ypos) = last;
   else if ((*ypos) < first)
      (*ypos) = first;
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
   newstring = (char *)malloc (sizeof (char) * (width + 3));
   /*cleanChar (newstring, width + 3, '\0');*/

   if ((start + width) > mesglen)
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
   newstring[lastchar + 1] = '\0';
   newstring[lastchar + 2] = '\0';
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
   newstring = (char *)malloc (sizeof (char) * ((int)(strlen(original) + 1)));

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
   newstring	= (chtype *)malloc (sizeof(chtype) * (len + 4));
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
   newstring[len + 1] = '\0';

   /* Return the new string.  */
   return (newstring);
}

/*
 * This reads a file and sticks it into the char ** provided.
 */
int readFile (char *filename, char **array, int maxlines)
{
   FILE	*fd;
   char temp[BUFSIZ];
   int	lines	= 0;

   /* Can we open the file?  */
   if ((fd = fopen (filename, "r")) == NULL)
   {
      return (-1);
   }

   /* Start reading the file in.  */
   while ((fgets (temp, sizeof(temp), fd) != (char *)NULL) && lines < maxlines)
   {
      array[lines]	= copyChar (temp);
      lines++;
   }
   fclose (fd);

   /* Clean up and return.  */
   array[lines] = "";
   return (lines);
}

#define DigitOf(c) ((c)-'0')

static int parseAttribute(char *string, int from, chtype *mask)
{
   int pair = 0;

   *mask = 0;
   switch (string[from + 1])
   {
   case 'B':	*mask = A_BOLD;		break;
   case 'D':	*mask = A_DIM;		break;
   case 'K':	*mask = A_BLINK;	break;
   case 'R':	*mask = A_REVERSE;	break;
   case 'S':	*mask = A_STANDOUT;	break;
   case 'U':	*mask = A_UNDERLINE;	break;
   }

   if (*mask != 0)
   {
      from++;
   }
   else if (isdigit((int)string[from + 1]) && isdigit((int)string[from + 2]))
   {
#ifdef HAVE_COLOR
      pair	= DigitOf(string[from + 1]) * 10 + DigitOf(string[from + 2]);
      *mask	= COLOR_PAIR(pair);
#else
      *mask	= A_BOLD;
#endif
      from += 2;
   }
   else if (isdigit((int)string[from + 1]))
   {
#ifdef HAVE_COLOR
      pair	= DigitOf(string[from + 1]);
      *mask	= COLOR_PAIR(pair);
#else
      *mask	= A_BOLD;
#endif
      from++;
   }
   return from;
}

/*
 * This function takes a character string, full of format markers
 * and translates them into a chtype * array. This is better suited
 * to curses, because curses uses chtype almost exclusively
 */
chtype *char2Chtype (char *string, int *to, int *align)
{
   chtype *result = 0;
   chtype attrib;
   chtype lastChar;
   chtype mask;
   int adjust;
   int from;
   int insideMarker;
   int len;
   int pass;
   int start;
   int used;
   int x;

   (*to) = 0;
   *align = LEFT;

   if (string != 0)
   {
      len = (int)strlen(string);
      used = 0;
      /*
       * We make two passes because we may have indents and tabs to expand, and
       * do not know in advance how large the result will be.
       */
      for (pass = 0; pass < 2; pass++)
      {
	 if (pass != 0)
	 {
	    if ((result = malloc((used+2) * sizeof(chtype))) == 0)
	    {
	       used = 0;
	       break;
	    }
	 }
	 adjust = 0;
	 attrib = A_NORMAL;
	 lastChar = 0;
	 start = 0;
	 used = 0;
	 x = 3;

	 /* Look for an alignment marker.  */
	 if (!strncmp(string, "<C>", 3))
	 {
	    (*align) = CENTER;
	    start = 3;
	 }
	 else if (!strncmp(string, "<R>", 3))
	 {
	    (*align) = RIGHT;
	    start = 3;
	 }
	 else if (!strncmp(string, "<L>", 3))
	 {
	    start = 3;
	 }
	 else if (!strncmp(string, "<B=", 3))
	 {
	    /* Set the item index value in the string.  */
	    if (result != 0)
	    {
	       result[0] = ' ';
	       result[1] = ' ';
	       result[2] = ' ';
	    }

	    /* Pull out the bullet marker.  */
	    while (string[x] != '>' && string[x] != 0)
	    {
	       if (result != 0)
		  result[x] = string[x] | A_BOLD;
	       x++;
	    }
	    adjust = 1;

	    /* Set the alignment variables.  */
	    start = x;
	    used = x;
	 }
	 else if (!strncmp(string, "<I=", 3))
	 {
	    from = 2;
	    x = 0;

	    while (string[++from] != '>' && string[from] != 0)
	    {
	       if (isdigit((int)string[from]))
	       {
		  adjust = (adjust * 10) + DigitOf(string[from]);
	       }
	    }

	    start = x + 4;
	 }

	 while (adjust-- > 0)
	 {
	    if (result != 0)
	       result[used] = ' ';
	    used++;
	 }

	 /* Set the format marker boolean to false.  */
	 insideMarker	= FALSE;

	 /* Start parsing the character string.  */
	 for (from = start; from < len; from++)
	 {
	    /* Are we inside a format marker?  */
	    if (! insideMarker)
	    {
	       if (string[from] == '<'
		&& (string[from + 1] == '/'
		 || string[from + 1] == '!'
		 || string[from + 1] == '#'))
	       {
		  insideMarker = TRUE;
	       }
	       else if (string[from] == '\\' && string[from + 1] == '<')
	       {
		  from++;
		  if (result != 0)
		     result[used] = (A_CHARTEXT & string[from]) | attrib;
		  used++;
		  from++;
	       }
	       else if (string[from] == '\t')
	       {
		  for (x=0; x < 8; x++)
		  {
		     if (result != 0)
			result[used] = ' ';
		     used++;
		  }
	       }
	       else
	       {
		  if (result != 0)
		     result[used] = (A_CHARTEXT & string[from]) | attrib;
		  used++;
	       }
	    }
	    else
	    {
	       switch (string[from])
	       {
	       case '>':
		  insideMarker = 0;
		  break;
	       case '#':
	       {
		  lastChar = 0;
		  switch(string[from + 2])
		  {
		  case 'L':
		     switch (string[from + 1])
		     {
		     case 'L': lastChar = ACS_LLCORNER; break;
		     case 'U': lastChar = ACS_LRCORNER; break;
		     case 'H': lastChar = ACS_HLINE; break;
		     case 'V': lastChar = ACS_VLINE; break;
		     case 'P': lastChar = ACS_PLUS; break;
		     }
		     break;
		  case 'U':
		     switch (string[from + 1])
		     {
		     case 'L': lastChar = ACS_ULCORNER; break;
		     case 'U': lastChar = ACS_URCORNER; break;
		     }
		     break;
		  case 'T':
		     switch (string[from + 1])
		     {
		     case 'T': lastChar = ACS_TTEE; break;
		     case 'R': lastChar = ACS_RTEE; break;
		     case 'L': lastChar = ACS_LTEE; break;
		     case 'B': lastChar = ACS_BTEE; break;
		     }
		     break;
		  case 'A':
		     switch (string[from + 1])
		     {
		     case 'L': lastChar = ACS_LARROW; break;
		     case 'R': lastChar = ACS_RARROW; break;
		     case 'U': lastChar = ACS_UARROW; break;
		     case 'D': lastChar = ACS_DARROW; break;
		     }
		     break;
		  default:
		     if (string[from + 1] == 'D'
		      && string[from + 2] == 'I')
			lastChar = ACS_DIAMOND;
		     else
		     if (string[from + 1] == 'C'
		      && string[from + 2] == 'B')
			lastChar = ACS_CKBOARD;
		     else
		     if (string[from + 1] == 'D'
		      && string[from + 2] == 'G')
			lastChar = ACS_DEGREE;
		     else
		     if (string[from + 1] == 'P'
		      && string[from + 2] == 'M')
			lastChar = ACS_PLMINUS;
		     else
		     if (string[from + 1] == 'B'
		      && string[from + 2] == 'U')
			lastChar = ACS_BULLET;
		     else
		     if (string[from + 1] == 'S'
		      && string[from + 2] == '1')
			lastChar = ACS_S1;
		     else
		     if (string[from + 1] == 'S'
		      && string[from + 2] == '9')
			lastChar = ACS_S9;
		  }

		  if (lastChar != 0)
		  {
		     adjust = 1;
		     from += 2;
		  }
		  else if (string[from + 1] == '(')
		  /* Check for a possible numeric modifier.  */
		  {
		     from++;
		     adjust = 0;

		     while (string[++from] != ')' && string[from] != 0)
		     {
			if (isdigit((int)string[from]))
			{
			   adjust = (adjust * 10) + DigitOf(string[from]);
			}
		     }
		  }
		  for (x=0; x < adjust; x++)
		  {
		     if (result != 0)
			result[used] = lastChar | attrib;
		     used++;
		  }
		  break;
	       }
	       case '/':
		  from = parseAttribute(string, from, &mask);
		  attrib = attrib | mask;
		  break;
	       case '!':
		  from = parseAttribute(string, from, &mask);
		  attrib = attrib & ~mask;
		  break;
	       }
	    }
	 }

	 if (result != 0)
	 {
	    result[used] = 0;
	    result[used + 1] = 0;
	 }

	 /*
	  * If there are no characters, put the attribute into the
	  * the first character of the array.
	  */
	 if (used == 0
	  && result != 0)
	 {
	    result[0] = attrib;
	 }
      }
      *to = used;
   }
   return result;
}

/*
 * This determines the length of a chtype string
 */
int chlen (chtype *string)
{
   int result = 0;

   if (string != 0)
   {
      while (string[result] != 0)
	 result++;
   }

   return (result);
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
   newstring = (char *)malloc (sizeof (char) * (len + 1));
   /*cleanChar (newstring, len + 1, '\0');*/

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
   static const struct {
      const char *name;
      EDisplayType code;
   } table[] = {
      { "CHAR",		vCHAR },
      { "HCHAR",	vHCHAR },
      { "INT",		vINT },
      { "HINT",		vHINT },
      { "UCHAR",	vUCHAR },
      { "LCHAR",	vLCHAR },
      { "UHCHAR",	vUHCHAR },
      { "LHCHAR",	vLHCHAR },
      { "MIXED",	vMIXED },
      { "HMIXED",	vHMIXED },
      { "UMIXED",	vUMIXED },
      { "LMIXED",	vLMIXED },
      { "UHMIXED",	vUHMIXED },
      { "LHMIXED",	vLHMIXED },
      { "VIEWONLY",	vVIEWONLY },
      { NULL, 		vINVALID },
   };

   /* Make sure we cover our bases... */
   if (string != 0)
   {
      int n;
      for (n = 0; table[n].name != 0; n++)
      {
	 if (!strcmp(string, table[n].name))
	    return table[n].code;
      }
   }
   return (EDisplayType)vINVALID;
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

   swapIndex (list, left, (left + right)/2);
   last = left;

   for (i=left + 1; i <= right; i++)
   {
      if (strcmp (list[i], list[left]) < 0)
      {
         swapIndex (list, ++last, i);
      }
   }

   swapIndex (list, left, last);
   quickSort (list, left, last-1);
   quickSort (list, last + 1, right);
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
            string[x] = string[x + alphaChar];
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

static unsigned countChar(char *string, int separator)
{
   unsigned result = 0;
   int ch;

   while ((ch = *string++) != 0)
   {
      if (ch == separator)
	 result++;
   }
   return result;
}

/*
 * Split a string into a list of strings.
 */
char **CDKsplitString(char *string, int separator)
{
   char **result = 0;
   char *first;
   char *temp;
   unsigned item;
   unsigned need;

   if (string != 0)
   {
      need = countChar(string, separator) + 2;
      if ((result = (char **)malloc(need * sizeof(char *))) != 0)
      {
	 item = 0;
	 first = string;
	 for(;;)
	 {
	    while (*string != 0 && *string != separator)
	       string++;

	    need = string - first;
	    if ((temp = malloc(need+1)) == 0)
	       break;

	    memcpy(temp, first, need);
	    temp[need] = 0;
	    result[item++] = temp;

	    if (*string++ == 0)
	       break;
	    first = string;
	 }
	 result[item] = 0;
      }
   }
   return result;
}

/*
 * Count the number of items in a list of strings.
 */
unsigned CDKcountStrings(char **list)
{
   unsigned result = 0;
   if (list != 0)
   {
      while (*list++ != 0)
	 result++;
   }
   return result;
}

/*
 * Free a list of strings
 */
void CDKfreeStrings(char **list)
{
   if (list != 0)
   {
      void *base = (void *)list;
      while (*list != 0)
	 free(*list++);
      free(base);
   }
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
   if (value < 0)
      return 1 + intlen(-value);
   else if (value >= 10)
      return 1 + intlen(value/10);
   return 1;
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
   for (x=Index + 1; x < pathLen; x++)
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

/*
 * This moves a given window
 */
void moveCursesWindow (WINDOW *window, int xdiff, int ydiff)
{
   window->_begx += xdiff;
   window->_begy += ydiff;
}

/*
 * Return an integer like 'floor()', which returns a double.
 */
int floorCDK(double value)
{
   int result = (int)value;
   if (result > value)	/* e.g., value < 0.0 and value is not an integer */
      result--;
   return result;
}

/*
 * Return an integer like 'ceil()', which returns a double.
 */
int ceilCDK(double value)
{
   return -floorCDK(-value);
}

#ifndef HAVE_USLEEP
void usleep (int delay);
void usleep (int delay)
{
   int x;
   for (x = 0; x <= (delay * 250) ; x++) {}
}
#endif
