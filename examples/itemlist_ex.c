#include "cdk.h"

#ifdef HAVE_XCURSES
char *XCursesProgramName="itemlist_ex";
#endif

/*
 * This program demonstrates the Cdk itemlist widget.
 */
int main (void)
{
   /* Declare local variables. */
   CDKSCREEN *cdkscreen		= (CDKSCREEN *)NULL;
   CDKITEMLIST *monthlist	= (CDKITEMLIST *)NULL;
   WINDOW *cursesWin		= (WINDOW *)NULL;
   char *title			= "<C>Pick A Month";
   char *label			= "</U/5>Month:";
   char *info[MAX_ITEMS], temp[256], *mesg[10];
   int choice, startMonth;
   struct tm *dateInfo;
   time_t clck;

   /*
    * Get the current date and set the default month to the
    * current month.
    */ 
    time (&clck);
    dateInfo	= localtime (&clck);
    startMonth	= dateInfo->tm_mon;

   /* Set up CDK. */ 
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start CDK colors. */
   initCDKColor();

   /* Create the choice list. */
   info[0]	= "<C></5>January";
   info[1]	= "<C></5>February";
   info[2]	= "<C></B/19>March";
   info[3]	= "<C></5>April";
   info[4]	= "<C></5>May";
   info[5]	= "<C></K/5>June";
   info[6]	= "<C></12>July";
   info[7]	= "<C></5>August";
   info[8]	= "<C></5>September";
   info[9]	= "<C></32>October";
   info[10]	= "<C></5>November";
   info[11]	= "<C></11>December";

   /* Create the itemlist widget. */
   monthlist	= newCDKItemlist (cdkscreen, CENTER, CENTER,
   					title, label, info, 12,
					startMonth, TRUE, FALSE);

   /* Is the widget NULL? */
   if (monthlist == (CDKITEMLIST *)NULL)
   {
      /* Clean up. */
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a little message. */
      printf ("Oops. Can't seem to create the itemlist box. Is the window too small?\n");
      exit (1);
   }

   /* Activate the widget. */
   choice = activateCDKItemlist (monthlist, (chtype *)NULL);

   /* Check how they exited from the widget. */
   if (monthlist->exitType == vESCAPE_HIT)
   {
      mesg[0] = "<C>You hit escape. No item selected.";
      mesg[1] = "";
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (ScreenOf(monthlist), mesg, 3);
   }
   else if (monthlist->exitType == vNORMAL)
   {
      sprintf (temp, "<C>You selected the %dth item which is", choice);
      mesg[0] = copyChar (temp);
      mesg[1] = info[choice];
      mesg[2] = "";
      mesg[3] = "<C>Press any key to continue.";
      popupLabel (ScreenOf(monthlist), mesg, 4);
      freeChar (mesg[0]);
   }

   /* Clean up. */
   destroyCDKItemlist (monthlist);
   destroyCDKScreen (cdkscreen);
   delwin (cursesWin);
   endCDK();
   exit (0);
}
