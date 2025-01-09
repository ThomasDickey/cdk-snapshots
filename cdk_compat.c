/*
 * $Id: cdk_compat.c,v 1.6 2025/01/09 00:20:21 tom Exp $
 * These are functions that are obsolete, but provided as a porting aid.
 * The obsolete functions use fixed limits, and inconsistent data handling.
 */
#include "cdk_compat.h"

/*
 * This opens the current directory and reads the contents.
 */
int getDirectoryContents (char *directory, char **list, int maxListSize)
{
   char **temp = NULL;
   int counter = CDKgetDirectoryContents (directory, &temp);
   int n;

   for (n = 0; n < counter && n < maxListSize; ++n)
   {
      list[n] = copyChar (temp[n]);
   }
   CDKfreeStrings (temp);

   return counter;
}

/*
 * This reads a file and sticks it into the char ** provided.
 */
int readFile (char *filename, char **array, int maxlines)
{
   char **temp = NULL;
   int lines = CDKreadFile (filename, &temp);
   int n;

   for (n = 0; n < maxlines; ++n)
   {
      if (n < lines)
      {
	 array[n] = copyChar (temp[n]);
      }
      else
      {
	 array[n] = copyChar ("");
	 break;
      }
   }
   CDKfreeStrings (temp);

   return (lines);
}

/*
 * This splits a string into X parts given the split character.
 */
int splitString (char *string, char **items, char splitChar)
{
   char **temp = CDKsplitString (string, splitChar);
   int chunks = 0;

   for (chunks = 0; chunks < MAX_LINES && temp[chunks] != NULL; ++chunks)
   {
      items[chunks] = copyChar (temp[chunks]);
   }
   CDKfreeStrings (temp);

   return chunks;
}
