/* $Id: radio_ex.c,v 1.8 2003/11/16 18:42:45 tom Exp $ */

#include <cdk.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName="radio_ex";
#endif

/*
 * This program demonstrates the Cdk radio widget.
 */
int main (void)
{
   /* Declare variables. */
   CDKSCREEN *cdkscreen = 0;
   CDKRADIO *radio	= 0;
   WINDOW *cursesWin	= 0;
   char *title		= "<C></5>Select a filename";
   char **item		= 0;
   char *mesg[5], temp[100];
   int selection, count;

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Set up CDK Colors. */
   initCDKColor();

   /* Use the current diretory list to fill the radio list. */
   count = CDKgetDirectoryContents (".", &item);

   /* Create the radio list. */
   radio = newCDKRadio (cdkscreen, CENTER, CENTER, RIGHT,
			10, 40, title, item, count,
			'#'|A_REVERSE, 1,
			A_REVERSE, TRUE, FALSE);

   /* Check if the radio list is null. */
   if (radio == 0)
   {
      /* Exit CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a message and exit. */
      printf ("Oops. Can't seem to create the radio widget. Is the window too small??\n");
      exit (1);
   }

   /* loop until user selects a file, or cancels */
   for (;;)
   {
      /* Activate the radio list. */
      selection = activateCDKRadio (radio, 0);

      /* Check the exit status of the widget. */
      if (radio->exitType == vESCAPE_HIT)
      {
	 mesg[0] = "<C>You hit escape. No item selected.";
	 mesg[1] = "",
	 mesg[2] = "<C>Press any key to continue.";
	 popupLabel (cdkscreen, mesg, 3);
	 break;
      }
      else if (radio->exitType == vNORMAL)
      {
	 struct stat sb;

	 if (stat(item[selection], &sb) == 0
	  && (sb.st_mode & S_IFMT) == S_IFDIR) {
	    char **nitem = 0;

	    mesg[0] = "<C>You selected a directory";
	    sprintf (temp, "<C>%s", item[selection]);
	    mesg[1] = temp;
	    mesg[2] = "";
	    mesg[3] = "<C>Press any key to continue.";
	    popupLabel (cdkscreen, mesg, 4);
	    if ((count = CDKgetDirectoryContents (item[selection], &nitem)) > 0)
	    {
	       CDKfreeStrings(item);
	       item = nitem;
	       chdir(item[selection]);
	       setCDKRadioItems (radio, item, count);
	    }
	 } else {
	    mesg[0] = "<C>You selected the filename";
	    sprintf (temp, "<C>%s", item[selection]);
	    mesg[1] = temp;
	    mesg[2] = "";
	    mesg[3] = "<C>Press any key to continue.";
	    popupLabel (cdkscreen, mesg, 4);
	    break;
	 }
      }
   }

   /* Clean up. */
   CDKfreeStrings (item);
   destroyCDKRadio (radio);
   destroyCDKScreen (cdkscreen);
   delwin (cursesWin);
   endCDK();
   exit (0);
}
