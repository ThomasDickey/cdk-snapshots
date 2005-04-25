/* $Id: scroll_ex.c,v 1.20 2005/04/15 19:44:43 tom Exp $ */

#include <cdk.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "scroll_ex";
#endif

static char *newLabel (char *prefix)
{
   static int count;
   static char result[80];
   sprintf (result, "%s%d", prefix, ++count);
   return result;
}

static int addItemCB (EObjectType cdktype GCC_UNUSED, void *object,
		      void *clientData GCC_UNUSED,
		      chtype input GCC_UNUSED)
{
   CDKSCROLL *s = (CDKSCROLL *)object;

   addCDKScrollItem (s, newLabel ("add"));

   refreshCDKScreen (ScreenOf (s));

   return (TRUE);
}

static int insItemCB (EObjectType cdktype GCC_UNUSED, void *object,
		      void *clientData GCC_UNUSED,
		      chtype input GCC_UNUSED)
{
   CDKSCROLL *s = (CDKSCROLL *)object;

   insertCDKScrollItem (s, newLabel ("insert"));

   refreshCDKScreen (ScreenOf (s));

   return (TRUE);
}

static int delItemCB (EObjectType cdktype GCC_UNUSED, void *object,
		      void *clientData GCC_UNUSED,
		      chtype input GCC_UNUSED)
{
   CDKSCROLL *s = (CDKSCROLL *)object;

   deleteCDKScrollItem (s, getCDKScrollCurrent (s));

   refreshCDKScreen (ScreenOf (s));

   return (TRUE);
}

/*
 * This program demonstrates the Cdk scrolling list widget.
 */
int main (int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN *cdkscreen = 0;
   CDKSCROLL *scrollList = 0;
   WINDOW *cursesWin = 0;
   char *title = "<C></5>Pick a file";
   char **item = 0;
   char *mesg[5], temp[256];
   int selection, count;

   CDK_PARAMS params;

   CDKparseParams (argc, argv, &params, "s:t:" CDK_CLI_PARAMS);

   /* Set up CDK. */
   cursesWin = initscr ();
   cdkscreen = initCDKScreen (cursesWin);

   /* Set up CDK Colors. */
   initCDKColor ();

   /* Use the current diretory list to fill the radio list. */
   count = CDKgetDirectoryContents (".", &item);

   /* Create the scrolling list. */
   scrollList = newCDKScroll (cdkscreen,
			      CDKparamValue (&params, 'X', CENTER),
			      CDKparamValue (&params, 'Y', CENTER),
			      CDKparsePosition(CDKparamString2(&params, 's', "RIGHT")),
			      CDKparamValue (&params, 'H', 10),
			      CDKparamValue (&params, 'W', 50),
			      CDKparamString2(&params, 't', title),
			      item, count,
			      TRUE,
			      A_REVERSE,
			      CDKparamValue (&params, 'N', TRUE),
			      CDKparamValue (&params, 'S', FALSE));

   /* Is the scrolling list null? */
   if (scrollList == 0)
   {
      /* Exit CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK ();

      /* Print out a message and exit. */
      printf
	      ("Oops. Could not make scrolling list. Is the window too small?\n");
      exit (EXIT_FAILURE);
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
   bindCDKObject (vSCROLL, scrollList, 'a', addItemCB, NULL);
   bindCDKObject (vSCROLL, scrollList, 'i', insItemCB, NULL);
   bindCDKObject (vSCROLL, scrollList, 'd', delItemCB, NULL);

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
      char *theItem = chtype2Char (scrollList->item[selection]);
      mesg[0] = "<C>You selected the following file";
      sprintf (temp, "<C>%.*s", (int)(sizeof (temp) - 20), theItem);
      mesg[1] = temp;
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
      freeChar (theItem);
   }

   /* Clean up. */
   CDKfreeStrings (item);
   destroyCDKScroll (scrollList);
   destroyCDKScreen (cdkscreen);
   endCDK ();
   exit (EXIT_SUCCESS);
}
