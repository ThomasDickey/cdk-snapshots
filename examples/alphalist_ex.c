/* $Id: alphalist_ex.c,v 1.22 2006/05/07 23:15:34 tom Exp $ */

#include <cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "alphalist_ex";
#endif

/*
 * This program demonstrates the Cdk alphalist widget.
 *
 * Options (in addition to normal CLI parameters):
 *	-c	create the data after the widget
 */
static CDKSCREEN *cdkscreen = 0;
static char **myUserList = 0;
static int userSize;

typedef struct
{
   int deleted;			/* index in current list which is deleted */
   int original;		/* index in myUserList[] of deleted item */
   int position;		/* position before delete */
   int topline;			/* top-line before delete */
} UNDO;

static UNDO *myUndoList;
static int undoSize;

/*
 * This reads the passwd file and retrieves user information.
 */
static int getUserList (char ***list)
{
   struct passwd *ent;
   int x = 0;
   unsigned used = 0;

   while ((ent = getpwent ()) != 0)
   {
      used = CDKallocStrings (list, ent->pw_name, x++, used);
   }
   endpwent ();
   return x;
}

#define CB_PARAMS EObjectType cdktype GCC_UNUSED, void* object GCC_UNUSED, void* clientdata GCC_UNUSED, chtype key GCC_UNUSED

static void fill_undo (CDKALPHALIST *widget, int deleted, char *data)
{
   int top = getCDKScrollCurrentTop (widget->scrollField);
   int item = getCDKAlphalistCurrentItem (widget);
   int n;

   myUndoList[undoSize].deleted = deleted;
   myUndoList[undoSize].topline = top;
   myUndoList[undoSize].original = -1;
   myUndoList[undoSize].position = item;
   for (n = 0; n < userSize; ++n)
   {
      if (!strcmp (myUserList[n], data))
      {
	 myUndoList[undoSize].original = n;
	 break;
      }
   }
   ++undoSize;
}

static int do_delete (CB_PARAMS)
{
   CDKALPHALIST *widget = (CDKALPHALIST *)clientdata;
   int size;
   char **list = getCDKAlphalistContents (widget, &size);
   int result = FALSE;

   if (size)
   {
      int save = getCDKScrollCurrentTop (widget->scrollField);
      int first = getCDKAlphalistCurrentItem (widget);
      int n;

      fill_undo (widget, first, list[first]);
      for (n = first; n < size; ++n)
	 list[n] = list[n + 1];
      setCDKAlphalistContents (widget, list, size - 1);
      setCDKScrollCurrentTop (widget->scrollField, save);
      setCDKAlphalistCurrentItem (widget, first);
      drawCDKAlphalist (widget, BorderOf (widget));
      result = TRUE;
   }
   return result;
}

static int do_delete1 (CB_PARAMS)
{
   CDKALPHALIST *widget = (CDKALPHALIST *)clientdata;
   int size;
   char **list = getCDKAlphalistContents (widget, &size);
   int result = FALSE;

   if (size)
   {
      int save = getCDKScrollCurrentTop (widget->scrollField);
      int first = getCDKAlphalistCurrentItem (widget);

      if (first-- > 0)
      {
	 int n;

	 fill_undo (widget, first, list[first]);
	 for (n = first; n < size; ++n)
	    list[n] = list[n + 1];
	 setCDKAlphalistContents (widget, list, size - 1);
	 setCDKScrollCurrentTop (widget->scrollField, save);
	 setCDKAlphalistCurrentItem (widget, first);
	 drawCDKAlphalist (widget, BorderOf (widget));
	 result = TRUE;
      }
   }
   return result;
}

static int do_help (CB_PARAMS)
{
   static char *message[] =
   {
      "Alpha List tests:",
      "",
      "F1 = help (this message)",
      "F2 = delete current item",
      "F3 = delete previous item",
      "F4 = reload all items",
      "F5 = undo deletion",
      0
   };
   popupLabel (cdkscreen, message, CDKcountStrings (message));
   return TRUE;
}

static int do_reload (CB_PARAMS)
{
   int result = FALSE;

   if (userSize)
   {
      CDKALPHALIST *widget = (CDKALPHALIST *)clientdata;
      setCDKAlphalistContents (widget, myUserList, userSize);
      setCDKAlphalistCurrentItem (widget, 0);
      drawCDKAlphalist (widget, BorderOf (widget));
      result = TRUE;
   }
   return result;
}

static int do_undo (CB_PARAMS)
{
   int result = FALSE;

   if (undoSize > 0)
   {
      CDKALPHALIST *widget = (CDKALPHALIST *)clientdata;
      int size;
      int n;
      char **oldlist = getCDKAlphalistContents (widget, &size);
      char **newlist = (char **)malloc ((++size + 1) * sizeof (char *));

      --undoSize;
      newlist[size] = 0;
      for (n = size - 1; n > myUndoList[undoSize].deleted; --n)
      {
	 newlist[n] = copyChar (oldlist[n - 1]);
      }
      newlist[n--] = copyChar (myUserList[myUndoList[undoSize].original]);
      while (n >= 0)
      {
	 newlist[n] = copyChar (oldlist[n]);
	 --n;
      }
      setCDKAlphalistContents (widget, newlist, size);
      setCDKScrollCurrentTop (widget->scrollField, myUndoList[undoSize].topline);
      setCDKAlphalistCurrentItem (widget, myUndoList[undoSize].position);
      drawCDKAlphalist (widget, BorderOf (widget));
      result = TRUE;
   }
   return result;
}

int main (int argc, char **argv)
{
   CDKALPHALIST *alphaList	= 0;
   WINDOW *cursesWin		= 0;
   char *title			= "<C></B/24>Alpha List\n<C>Title";
   char *label			= "</B>Account: ";
   char *word			= 0;
   char **userList		= 0;
   char *mesg[5], temp[256];

   CDK_PARAMS params;

   CDKparseParams (argc, argv, &params, "c" CDK_CLI_PARAMS);

   /* Get the user list. */
   userSize = getUserList (&userList);
   if (userSize <= 0)
   {
      fprintf (stderr, "Cannot get user list\n");
      ExitProgram (EXIT_FAILURE);
   }
   myUserList = copyCharList (userList);
   myUndoList = (UNDO *) malloc (userSize * sizeof (UNDO));
   undoSize = 0;

   /* Set up CDK. */
   cursesWin = initscr ();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start color. */
   initCDKColor ();

   /* Create the alpha list widget. */
   alphaList = newCDKAlphalist (cdkscreen,
				CDKparamValue (&params, 'X', CENTER),
				CDKparamValue (&params, 'Y', CENTER),
				CDKparamValue (&params, 'H', 0),
				CDKparamValue (&params, 'W', 0),
				title, label,
				CDKparamNumber (&params, 'c') ? 0 : userList,
				CDKparamNumber (&params, 'c') ? 0 : userSize,
				'_', A_REVERSE,
				CDKparamValue (&params, 'N', TRUE),
				CDKparamValue (&params, 'S', FALSE));
   if (alphaList == 0)
   {
      destroyCDKScreen (cdkscreen);
      endCDK ();

      fprintf (stderr, "Cannot create widget\n");
      ExitProgram (EXIT_FAILURE);
   }

   bindCDKObject (vALPHALIST, alphaList, '?', do_help, NULL);
   bindCDKObject (vALPHALIST, alphaList, KEY_F1, do_help, NULL);
   bindCDKObject (vALPHALIST, alphaList, KEY_F2, do_delete, alphaList);
   bindCDKObject (vALPHALIST, alphaList, KEY_F3, do_delete1, alphaList);
   bindCDKObject (vALPHALIST, alphaList, KEY_F4, do_reload, alphaList);
   bindCDKObject (vALPHALIST, alphaList, KEY_F5, do_undo, alphaList);

   if (CDKparamNumber (&params, 'c'))
   {
      setCDKAlphalistContents (alphaList, userList, userSize);
   }

   /* Let them play with the alpha list. */
   word = activateCDKAlphalist (alphaList, 0);

   /* Determine what the user did. */
   if (alphaList->exitType == vESCAPE_HIT)
   {
      mesg[0] = "<C>You hit escape. No word was selected.";
      mesg[1] = "";
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
   }
   else if (alphaList->exitType == vNORMAL)
   {
      mesg[0] = "<C>You selected the following";
      sprintf (temp, "<C>(%.*s)", (int)(sizeof (temp) - 10), word);
      mesg[1] = temp;
      mesg[2] = "";
      mesg[3] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 4);
   }

   freeCharList (myUserList, userSize);
   free (myUserList);

   destroyCDKAlphalist (alphaList);
   destroyCDKScreen (cdkscreen);
   endCDK ();

   ExitProgram (EXIT_SUCCESS);
}
