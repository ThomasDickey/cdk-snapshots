/* $Id: scroll_ex.c,v 1.12 2003/11/29 13:33:13 tom Exp $ */

#include <cdk.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName="scroll_ex";
#endif

/*
 * This program demonstrates the Cdk scrolling list widget.
 */
int main(int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkscreen		= 0;
   CDKSCROLL *scrollList	= 0;
   WINDOW *cursesWin		= 0;
   char *title			= "<C></5>Pick a file";
   char **item			= 0;
   char *mesg[5], temp[256];
   int selection, count;

   CDK_PARAMS params;

   CDKparseParams(argc, argv, &params, CDK_CLI_PARAMS);

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Set up CDK Colors. */
   initCDKColor();

   /* Use the current diretory list to fill the radio list. */
   count = CDKgetDirectoryContents (".", &item);

   /* Create the scrolling list. */
   scrollList = newCDKScroll (cdkscreen,
			      CDKparamValue(&params, 'X', CENTER),
			      CDKparamValue(&params, 'Y', CENTER),
			      RIGHT,
			      CDKparamValue(&params, 'H', 10),
			      CDKparamValue(&params, 'W', 50),
			      title, item, count,
			      TRUE,
			      A_REVERSE,
			      CDKparamValue(&params, 'N', TRUE),
			      CDKparamValue(&params, 'S', FALSE));

   /* Is the scrolling list null? */
   if (scrollList == 0)
   {
      /* Exit CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a message and exit. */
      printf ("Oops. Could not make scrolling list. Is the window too small?\n");
      exit (1);
   }

#if 0
   drawCDKScroll (scrollList, 1);

   setCDKScrollPosition (scrollList, 10);
   drawCDKScroll (scrollList, 1);
   sleep (3);

   setCDKScrollPosition (scrollList, 20);
   drawCDKScroll (scrollList, 1);
   sleep (3);

   setCDKScrollPosition (scrollList, 30);
   drawCDKScroll (scrollList, 1);
   sleep (3);

   setCDKScrollPosition (scrollList, 70);
   drawCDKScroll (scrollList, 1);
   sleep (3);
#endif

   /* Activate the scrolling list. */
   selection = activateCDKScroll (scrollList, 0);

   /* Determine how the widget was exited. */
   if (scrollList->exitType == vESCAPE_HIT)
   {
      mesg[0] = "<C>You hit escape. No file selected.";
      mesg[1] = "",
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
   }
   else if (scrollList->exitType == vNORMAL)
   {
      mesg[0] = "<C>You selected the following file";
      sprintf (temp, "<C>%s", item[selection]);
      mesg[1] = copyChar (temp);
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
      freeChar (mesg[1]);
   }

   /* Clean up. */
   CDKfreeStrings (item);
   destroyCDKScroll (scrollList);
   destroyCDKScreen (cdkscreen);
   delwin (cursesWin);
   endCDK();
   exit (0);
}
