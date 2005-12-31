/* $Id: bind_ex.c,v 1.16 2005/12/26 22:04:35 tom Exp $ */

#include <cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "bind_ex";
#endif

static int dialogHelpCB (EObjectType cdktype GCC_UNUSED,
			 void *object,
			 void *clientData GCC_UNUSED,
			 chtype key GCC_UNUSED)
{
   CDKDIALOG *dialog = (CDKDIALOG *) object;
   char *mesg[5];

   /* Check which button we are on. */
   if (dialog->currentButton == 0)
   {
      mesg[0] = "<C></U>Help for </U>Who<!U>.";
      mesg[1] = "<C>When this button is picked the name of the current";
      mesg[2] = "<C>user is displayed on the screen in a popup window.";
      popupLabel (ScreenOf (dialog), mesg, 3);
   }
   else if (dialog->currentButton == 1)
   {
      mesg[0] = "<C></U>Help for </U>Time<!U>.";
      mesg[1] = "<C>When this button is picked the current time is";
      mesg[2] = "<C>displayed on the screen in a popup window.";
      popupLabel (ScreenOf (dialog), mesg, 3);
   }
   else if (dialog->currentButton == 2)
   {
      mesg[0] = "<C></U>Help for </U>Date<!U>.";
      mesg[1] = "<C>When this button is picked the current date is";
      mesg[2] = "<C>displayed on the screen in a popup window.";
      popupLabel (ScreenOf (dialog), mesg, 3);
   }
   else if (dialog->currentButton == 3)
   {
      mesg[0] = "<C></U>Help for </U>Quit<!U>.";
      mesg[1] = "<C>When this button is picked the dialog box is exited.";
      popupLabel (ScreenOf (dialog), mesg, 2);
   }
   return (FALSE);
}

int main(int argc, char **argv)
{
   /* Declare variables. */
   CDKSCREEN	*cdkscreen;
   CDKDIALOG	*question;
   WINDOW	*cursesWin;
   char		*buttons[40];
   char		*message[40];
   char		*info[5];
   char		*loginName;
   char		temp[256];
   int		selection;
   time_t	clck;
   struct tm	*currentTime;

   CDK_PARAMS params;

   CDKparseParams(argc, argv, &params, CDK_MIN_PARAMS);

   /* Set up CDK. */
   cursesWin = initscr ();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start color. */
   initCDKColor ();

   /* Set up the dialog box. */
   message[0] = "<C></U>Simple Command Interface";
   message[1] = "Pick the command you wish to run.";
   message[2] = "<C>Press </R>?<!R> for help.";
   buttons[0] = "Who";
   buttons[1] = "Time";
   buttons[2] = "Date";
   buttons[3] = "Quit";

   /* Create the dialog box. */
   question = newCDKDialog (cdkscreen,
			    CDKparamValue(&params, 'X', CENTER),
			    CDKparamValue(&params, 'Y', CENTER),
			    message, 3, buttons, 4, A_REVERSE,
			    TRUE,
			    CDKparamValue(&params, 'N', TRUE),
			    CDKparamValue(&params, 'S', FALSE));

   /* Check if we got a null value back. */
   if (question == (CDKDIALOG *) 0)
   {
      destroyCDKScreen (cdkscreen);

      /* End curses... */
      endCDK ();

      /* Spit out a message. */
      printf ("Oops. Can't seem to create the dialog box. ");
      printf ("Is the window too small?\n");
      ExitProgram (EXIT_FAILURE);
   }

   /* Create the key binding. */
   bindCDKObject (vDIALOG, question, '?', dialogHelpCB, 0);

   /* Activate the dialog box. */
   selection = 0;
   while (selection != 3)
   {
      /* Get the users button selection. */
      selection = activateCDKDialog (question, (chtype *) 0);

      /* Check the results. */
      if (selection == 0)
      {
	 /* Get the users login name. */
	 info[0] = "<C>     </U>Login Name<!U>     ";
	 loginName = getlogin ();
	 if (loginName == (char *)0)
	 {
	    strcpy (temp, "<C></R>Unknown");
	 }
	 else
	 {
	    sprintf (temp, "<C><%.*s>", (int)(sizeof(temp) - 10), loginName);
	 }
	 info[1] = copyChar (temp);
	 popupLabel (ScreenOf (question), info, 2);
	 freeChar (info[1]);
      }
      else if (selection == 1)
      {
	 /* Print out the time. */
	 time (&clck);
	 currentTime = localtime (&clck);
	 sprintf (temp, "<C>%2d:%02d:%02d",
		  currentTime->tm_hour,
		  currentTime->tm_min,
		  currentTime->tm_sec);
	 info[0] = "<C>   </U>Current Time<!U>   ";
	 info[1] = copyChar (temp);
	 popupLabel (ScreenOf (question), info, 2);
	 freeChar (info[1]);
      }
      else if (selection == 2)
      {
	 /* Print out the date. */
	 time (&clck);
	 currentTime = localtime (&clck);
	 sprintf (temp, "<C>%d/%02d/%02d",
		  currentTime->tm_mday,
		  currentTime->tm_mon + 1,
		  currentTime->tm_year % 100);
	 info[0] = "<C>   </U>Current Date<!U>   ";
	 info[1] = copyChar (temp);
	 popupLabel (ScreenOf (question), info, 2);
	 freeChar (info[1]);
      }
   }

   /* Clean up. */
   destroyCDKDialog (question);
   destroyCDKScreen (cdkscreen);
   endCDK ();
   ExitProgram (EXIT_SUCCESS);
}
