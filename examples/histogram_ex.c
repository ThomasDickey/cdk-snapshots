#include "cdk.h"

#ifdef HAVE_XCURSES
char *XCursesProgramName="histogram_ex";
#endif

int main (int argc, char **argv)
{
   /* Declare vars. */
   CDKSCREEN *cdkscreen	= (CDKSCREEN *)NULL;
   CDKHISTOGRAM	*volume	= (CDKHISTOGRAM *)NULL;
   CDKHISTOGRAM	*bass	= (CDKHISTOGRAM *)NULL;
   CDKHISTOGRAM	*treble	= (CDKHISTOGRAM *)NULL;
   WINDOW *cursesWin	= (WINDOW *)NULL;
   char *volumeTitle	= "<C></5>Volume<!5>";
   char *bassTitle	= "<C></5>Bass  <!5>";
   char *trebleTitle	= "<C></5>Treble<!5>";

   /* Set up CDK. */ 
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start CDK Color. */
   initCDKColor();

   /* Create the histogram objects. */
   volume = newCDKHistogram (cdkscreen, 10, 10, 1, -2, 
				HORIZONTAL, volumeTitle,
				TRUE, FALSE);
   if (volume == (CDKHISTOGRAM *)NULL)
   {
      /* Exit CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK();
      
      /* Print out a message and exit. */
      printf ("Oops. Can not make volume histogram. Is the window big enough??\n");
      exit (1);
   }

   bass = newCDKHistogram (cdkscreen, 10, 14, 1, -2, 
				HORIZONTAL, bassTitle,
				TRUE, FALSE);
   if (bass == (CDKHISTOGRAM *)NULL)
   {
      /* Exit CDK. */
      destroyCDKHistogram (volume);
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a message and exit. */
      printf ("Oops. Can not make bass histogram. Is the window big enough??\n");
      exit (1);
   }

   treble = newCDKHistogram (cdkscreen, 10, 18, 1, -2,
				HORIZONTAL, trebleTitle,
				TRUE, FALSE);
   if (treble == (CDKHISTOGRAM *)NULL)
   {
      /* Exit CDK. */
      destroyCDKHistogram (volume);
      destroyCDKHistogram (bass);
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a message and exit. */
      printf ("Oops. Can not make treble histogram. Is the window big enough??\n");
      exit (1);
   }

   /* Set the histogram values. */
   setCDKHistogram (volume, vPERCENT, CENTER, A_BOLD, 0, 10, 6, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   setCDKHistogram (bass  , vPERCENT, CENTER, A_BOLD, 0, 10, 3, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   setCDKHistogram (treble, vPERCENT, CENTER, A_BOLD, 0, 10, 7, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   refreshCDKScreen (cdkscreen);
   sleep (4);

   /* Set the histogram values. */
   setCDKHistogram (volume, vPERCENT, CENTER, A_BOLD, 0, 10, 8, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   setCDKHistogram (bass  , vPERCENT, CENTER, A_BOLD, 0, 10, 1, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   setCDKHistogram (treble, vPERCENT, CENTER, A_BOLD, 0, 10, 9, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   refreshCDKScreen (cdkscreen);
   sleep (4);

   /* Set the histogram values. */
   setCDKHistogram (volume, vPERCENT, CENTER, A_BOLD, 0, 10, 10, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   setCDKHistogram (bass  , vPERCENT, CENTER, A_BOLD, 0, 10, 7,  ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   setCDKHistogram (treble, vPERCENT, CENTER, A_BOLD, 0, 10, 10, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   refreshCDKScreen (cdkscreen);
   sleep (4);

   /* Set the histogram values. */
   setCDKHistogram (volume, vPERCENT, CENTER, A_BOLD, 0, 10, 1, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   setCDKHistogram (bass  , vPERCENT, CENTER, A_BOLD, 0, 10, 8, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   setCDKHistogram (treble, vPERCENT, CENTER, A_BOLD, 0, 10, 3, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   refreshCDKScreen (cdkscreen);
   sleep (4);

   /* Set the histogram values. */
   setCDKHistogram (volume, vPERCENT, CENTER, A_BOLD, 0, 10, 3, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   setCDKHistogram (bass  , vPERCENT, CENTER, A_BOLD, 0, 10, 3, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   setCDKHistogram (treble, vPERCENT, CENTER, A_BOLD, 0, 10, 3, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   refreshCDKScreen (cdkscreen);
   sleep (4);

   /* Set the histogram values. */
   setCDKHistogram (volume, vPERCENT, CENTER, A_BOLD, 0, 10, 10, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   setCDKHistogram (bass  , vPERCENT, CENTER, A_BOLD, 0, 10, 10, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   setCDKHistogram (treble, vPERCENT, CENTER, A_BOLD, 0, 10, 10, ' '|A_REVERSE|COLOR_PAIR(3), TRUE);
   refreshCDKScreen (cdkscreen);
   sleep (4);

   /* Clean up. */
   destroyCDKHistogram (volume);
   destroyCDKHistogram (bass);
   destroyCDKHistogram (treble);
   destroyCDKScreen (cdkscreen);
   delwin (cursesWin);
   endCDK();
   exit (0);
}
