#include "cdk.h"

/*
 * $Author: glover $
 * $Date: 1997/04/06 22:59:36 $
 * $Revision: 1.7 $
 */

FILE	*CDKDEBUG;


/*
 * This starts debugging for CDK.
 */
FILE *startCDKDebug (char *filename)
{
   char *defFile = "cdkdebug.log";

   /* Check if the filename is NULL. */
   if (filename == (char *)NULL)
   {
      filename = defFile;
   }

   /* Try to open the file. */
   return ( fopen (filename, "w") );
}

/*
 * This writes a message to the debug file.
 */
void writeCDKDebugMessage (FILE *fd, char *filename, char *function, int line, char *message)
{
   /* Print the message as long as the file descr. is not NULL.	*/
   if (fd != NULL)
   {
      fprintf (fd, "%s::%s (Line %d) %s\n", filename, function, line, message);
   }
}

/*
 * This turns off the debugging for CDK.
 */
void stopCDKDebug (FILE *fd)
{
   if (fd != NULL)
   {
      fclose (fd);
   }
}
