#include <cdk.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName="scale_ex";
#endif

/*
 * This program demonstrates the Cdk scale widget.
 */
int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkscreen = 0;
   CDKFSCALE *scale	= 0;
   WINDOW *cursesWin	= 0;
   char *title		= "<C>Select a value";
   char *label		= "</5>Current value";
   float low		= -1.2;
   float high		= 2.4;
   float inc		= 0.2;
   char temp[256], *mesg[5];
   int ret;
   float selection;

   /* Parse up the command line.*/
   while (1)
   {
      ret = getopt (argc, argv, "l:h:i:");

      /* Are there any more command line options to parse. */
      if (ret == -1)
      {
	 break;
      }

      switch (ret)
      {
	 case 'l':
	      low = atof (optarg);
	      break;

	 case 'h':
	      high = atof (optarg);
	      break;

	 case 'i':
	      inc = atof (optarg);
	      break;
      }
   }

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start CDK Colors. */
   initCDKColor();

   /* Create the scale. */
   scale = newCDKFScale (cdkscreen, CENTER, CENTER,
			title, label, A_NORMAL,
			10, low, low, high,
			inc, (inc*2.), 1, TRUE, FALSE);

   /* Is the scale null? */
   if (scale == 0)
   {
      /* Exit CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a message. */
      printf ("Oops. Can't make the scale widget. Is the window too small?\n");
      exit (1);
   }

   /* Activate the scale. */
   selection = activateCDKFScale (scale, 0);

   /* Check the exit value of the scale widget. */
   if (scale->exitType == vESCAPE_HIT)
   {
      mesg[0] = "<C>You hit escape. No value selected.";
      mesg[1] = "",
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
   }
   else if (scale->exitType == vNORMAL)
   {
      sprintf (temp, "<C>You selected %f", selection);
      mesg[0] = copyChar (temp);
      mesg[1] = "",
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
      freeChar (mesg[0]);
   }

   /* Clean up. */
   destroyCDKFScale (scale);
   destroyCDKScreen (cdkscreen);
   delwin (cursesWin);
   endCDK();
   exit (0);
}
