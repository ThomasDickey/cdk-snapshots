/* $Id: radio1_ex.c,v 1.3 2002/07/14 21:39:11 moloney Exp $ */

#include "cdk.h"

#ifdef HAVE_XCURSES
char *XCursesProgramName = "radio_ex";
#endif

/*
 * This program demonstrates the Cdk radio widget.
 */
int main (void)
{
   /* Declare variables. */
   CDKSCREEN *cdkscreen = (CDKSCREEN *) NULL;
   CDKRADIO *radio = (CDKRADIO *) NULL;
   WINDOW *cursesWin = (WINDOW *) NULL;
   char *item[5] =
   {"Choice A", "Choice B", "Choice C"};
   char *mesg[5], temp[100];
   int selection;

   /* Set up CDK. */
   cursesWin = initscr ();
   cdkscreen = initCDKScreen (cursesWin);

   /* Set up CDK Colors. */
   initCDKColor ();

   /* Create the radio list. */
   radio = newCDKRadio (cdkscreen, CENTER, CENTER, NONE,
			5, 20, NULL, item, 3,
			'#' | A_REVERSE, 1,
			A_REVERSE, FALSE, FALSE);

   /* Check if the radio list is NULL. */
   if (radio == (CDKRADIO *) NULL)
   {
      /* Exit CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK ();

      /* Print out a message and exit. */
      printf ("Oops. Can't seem to create the radio widget. ");
      printf ("Is the window too small??\n");
      exit (1);
   }

   /* Activate the radio list. */
   selection = activateCDKRadio (radio, (chtype *) NULL);

   /* Check the exit status of the widget. */
   if (radio->exitType == vESCAPE_HIT)
   {
      mesg[0] = "<C>You hit escape. No item selected.";
      mesg[1] = "",
	 mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
   }
   else if (radio->exitType == vNORMAL)
   {
      mesg[0] = "<C>You selected the filename";
      sprintf (temp, "<C>%s", item[selection]);
      mesg[1] = copyChar (temp);
      mesg[2] = "";
      mesg[3] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 4);
      freeChar (mesg[1]);
   }

   destroyCDKRadio (radio);
   destroyCDKScreen (cdkscreen);
   delwin (cursesWin);
   endCDK ();
   exit (0);
}
