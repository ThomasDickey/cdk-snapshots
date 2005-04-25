/* $Id: radio_ex.c,v 1.14 2005/04/15 21:42:42 tom Exp $ */

#include <cdk.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "radio_ex";
#endif

/*
 * This program demonstrates the Cdk radio widget.
 */
int main(int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkscreen = 0;
   CDKRADIO *radio	= 0;
   WINDOW *cursesWin	= 0;
   char *title		= "<C></5>Select a filename";
   char **item		= 0;
   char *mesg[5], temp[256];
   int selection, count;

   CDK_PARAMS params;

   CDKparseParams(argc, argv, &params, "s:t:" CDK_CLI_PARAMS);

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Set up CDK Colors. */
   initCDKColor();

   /* Use the current diretory list to fill the radio list. */
   count = CDKgetDirectoryContents (".", &item);

   /* Create the radio list. */
   radio = newCDKRadio (cdkscreen,
			CDKparamValue(&params, 'X', CENTER),
			CDKparamValue(&params, 'Y', CENTER),
			CDKparsePosition(CDKparamString2(&params, 's', "RIGHT")),
			CDKparamValue(&params, 'H', 10),
			CDKparamValue(&params, 'W', 40),
			CDKparamString2(&params, 't', title),
			item, count,
			'#'|A_REVERSE, 1,
			A_REVERSE,
			CDKparamValue(&params, 'N', TRUE),
			CDKparamValue(&params, 'S', FALSE));

   /* Check if the radio list is null. */
   if (radio == 0)
   {
      /* Exit CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a message and exit. */
      printf ("Oops. Can't seem to create the radio widget. Is the window too small??\n");
      exit (EXIT_FAILURE);
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
	    sprintf (temp, "<C>%.*s", (int)(sizeof(temp) - 20), item[selection]);
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
	    sprintf (temp, "<C>%.*s", (int)(sizeof(temp) - 20), item[selection]);
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
   endCDK();
   exit (EXIT_SUCCESS);
}
