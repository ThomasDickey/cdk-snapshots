#include "cdk.h"

/*
 * $Author: tom $
 * $Date: 2000/01/16 22:50:37 $
 * $Revision: 1.61 $
 */

DeclareCDKObjects(my_funcs,Dialog);

/*
 * This function creates a dialog widget.
 */
CDKDIALOG *newCDKDialog (CDKSCREEN *cdkscreen, int xplace, int yplace, char **mesg, int rows, char **buttonLabel, int buttonCount, chtype highlight, boolean separator, boolean Box, boolean shadow)
{
   /* Declare local variables. */
   CDKDIALOG *dialog	= newCDKObject(CDKDIALOG, &my_funcs);
   int boxWidth		= MIN_DIALOG_WIDTH;
   int boxHeight	= rows + 3 + separator;
   int maxmessagewidth	= -1;
   int buttonwidth	= 0;
   int xpos		= xplace;
   int ypos		= yplace;
   int temp		= 0;
   int buttonadj	= 0;
   int x		= 0;

   /* Translate the char * message to a chtype * */
   for (x=0; x < rows; x++)
   {
      dialog->info[x]	= char2Chtype (mesg[x], &dialog->infoLen[x], &dialog->infoPos[x]);
      maxmessagewidth	= MAXIMUM(maxmessagewidth, dialog->infoLen[x]);
   }

   /* Translate the button label char * to a chtype * */
   for (x = 0; x < buttonCount; x++)
   {
      dialog->buttonLabel[x]	= char2Chtype (buttonLabel[x], &dialog->buttonLen[x], &temp);
      buttonwidth		+= dialog->buttonLen[x] + 1;
   }
   buttonwidth--;

   /* Determine the final dimensions of the box. */
   boxWidth	= MAXIMUM(boxWidth, maxmessagewidth);
   boxWidth	= MAXIMUM(boxWidth, buttonwidth);
   boxWidth	= boxWidth + 4;

   /* Now we have to readjust the x and y positions. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Set up the dialog box attributes. */
   ScreenOf(dialog)		= cdkscreen;
   dialog->parent		= cdkscreen->window;
   dialog->win			= newwin (boxHeight, boxWidth, ypos, xpos);
   dialog->shadowWin		= 0;
   dialog->buttonCount		= buttonCount;
   dialog->currentButton	= 0;
   dialog->messageRows		= rows;
   dialog->boxHeight		= boxHeight;
   dialog->boxWidth		= boxWidth;
   dialog->highlight		= highlight;
   dialog->separator		= separator;
   dialog->exitType		= vNEVER_ACTIVATED;
   ObjOf(dialog)->box		= Box;
   dialog->shadow		= shadow;
   dialog->ULChar		= ACS_ULCORNER;
   dialog->URChar		= ACS_URCORNER;
   dialog->LLChar		= ACS_LLCORNER;
   dialog->LRChar		= ACS_LRCORNER;
   dialog->HChar		= ACS_HLINE;
   dialog->VChar		= ACS_VLINE;
   dialog->BoxAttrib		= A_NORMAL;
   dialog->preProcessFunction	= 0;
   dialog->preProcessData	= 0;
   dialog->postProcessFunction	= 0;
   dialog->postProcessData	= 0;

   /* If we couldn't create the window, we should return a null value. */
   if (dialog->win == 0)
   {
      /* Couldn't create the window. Clean up used memory. */
      for (x=0; x < dialog->messageRows ; x++)
      {
	 freeChtype (dialog->info[x]);
      }
      for (x=0; x < dialog->buttonCount; x++)
      {
	 freeChtype (dialog->buttonLabel[x]);
      }

      /* Remove the memory used by the dialog pointer. */
      free (dialog);

      /* Return a null dialog box. */
      return (0);
   }
   keypad (dialog->win, TRUE);

   /* Find the button positions. */
   buttonadj = ((int)((boxWidth-buttonwidth)/2));
   for (x = 0; x < buttonCount; x++)
   {
      dialog->buttonPos[x]	= buttonadj;
      buttonadj			= buttonadj + dialog->buttonLen[x] + 1;
   }

   /* Create the string alignments. */
   for (x=0; x < rows; x++)
   {
      dialog->infoPos[x] = justifyString (boxWidth, dialog->infoLen[x], dialog->infoPos[x]);
   }

   /* Was there a shadow? */
   if (shadow)
   {
      dialog->shadowWin = newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
   }

   /* Empty the key bindings. */
   cleanCDKObjectBindings (vDIALOG, dialog);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vDIALOG, dialog);

   /* Return the dialog box pointer. */
   return (dialog);
}

/*
 * This lets the user select the button.
 */
int activateCDKDialog (CDKDIALOG *dialog, chtype *actions)
{
   /* Declare local variables. */
   chtype input = 0;
   int ret;

   /* Draw the dialog box. */
   drawCDKDialog (dialog, ObjOf(dialog)->box);

   /* Lets move to the first button. */
   writeChtypeAttrib (dialog->win,
			dialog->buttonPos[dialog->currentButton],
			dialog->boxHeight-2,
			dialog->buttonLabel[dialog->currentButton],
			dialog->highlight,
			HORIZONTAL,
			0, dialog->buttonLen[dialog->currentButton]);
   wrefresh (dialog->win);

   /* Check if actions is null. */
   if (actions == 0)
   {
      for (;;)
      {
	 /* Get the input. */
	 input = wgetch (dialog->win);

	 /* Inject the character into the widget. */
	 ret = injectCDKDialog (dialog, input);
	 if (dialog->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }
   else
   {
      int length = chlen (actions);
      int x = 0;

      /* Inject each character one at a time. */
      for (x=0; x < length; x++)
      {
	 ret = injectCDKDialog (dialog, actions[x]);
	 if (dialog->exitType != vEARLY_EXIT)
	 {
	    return ret;
	 }
      }
   }

   /* Set the exit type and exit. */
   dialog->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This injects a single character into the dialog widget.
 */
int injectCDKDialog (CDKDIALOG *dialog, chtype input)
{
   int firstButton	= 0;
   int lastButton	= dialog->buttonCount - 1;
   int ppReturn		= 1;

   /* Set the exit type. */
   dialog->exitType = vEARLY_EXIT;

   /* Check if there is a pre-process function to be called. */
   if (dialog->preProcessFunction != 0)
   {
      ppReturn = ((PROCESSFN)(dialog->preProcessFunction)) (vDIALOG, dialog, dialog->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check for a key binding. */
      if (checkCDKObjectBind (vDIALOG, dialog, input) != 0)
      {
	 dialog->exitType = vESCAPE_HIT;
	 return -1;
      }
      else
      {
	 switch (input)
	 {
	    case KEY_LEFT : case CDK_PREV :
		 if (dialog->currentButton == firstButton)
		 {
		    dialog->currentButton = lastButton;;
		 }
		 else
		 {
		    dialog->currentButton--;
		 }
		 break;

	    case KEY_RIGHT : case CDK_NEXT : case KEY_TAB : case ' ' :
		 if (dialog->currentButton == lastButton)
		 {
		    dialog->currentButton = firstButton;
		 }
		 else
		 {
		    dialog->currentButton++;
		 }
		 break;

	    case KEY_UP : case KEY_DOWN :
		 Beep();
		 break;

	    case CDK_REFRESH :
		 eraseCDKScreen (ScreenOf(dialog));
		 refreshCDKScreen (ScreenOf(dialog));
		 break;

	    case KEY_ESC :
		 dialog->exitType = vESCAPE_HIT;
		 return -1;

	    case KEY_RETURN : case KEY_ENTER :
		 dialog->exitType = vNORMAL;
		 return dialog->currentButton;

	 default :
	    break;
	 }
      }

      /* Should we call a post-process? */
      if (dialog->postProcessFunction != 0)
      {
	 ((PROCESSFN)(dialog->postProcessFunction)) (vDIALOG, dialog, dialog->postProcessData, input);
      }
   }

   /* Redraw the buttons. */
   drawCDKDialogButtons (dialog);
   wrefresh (dialog->win);

   /* Exit the dialog box. */
   dialog->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This moves the dialog field to the given location.
 */
static void _moveCDKDialog (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKDIALOG *dialog = (CDKDIALOG *)object;
   /* Declare local variables. */
   int currentX = getbegx(dialog->win);
   int currentY = getbegy(dialog->win);
   int xpos	= xplace;
   int ypos	= yplace;
   int xdiff	= 0;
   int ydiff	= 0;

   /*
    * If this is a relative move, then we will adjust where we want
    * to move to.
    */
   if (relative)
   {
      xpos = getbegx(dialog->win) + xplace;
      ypos = getbegy(dialog->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(dialog), &xpos, &ypos, dialog->boxWidth, dialog->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(dialog->win, -xdiff, -ydiff);

   /* If there is a shadow box we have to move it too. */
   if (dialog->shadowWin != 0)
   {
      moveCursesWindow(dialog->shadowWin, -xdiff, -ydiff);
   }

   /* Touch the windows so they 'move'. */
   touchwin (WindowOf(dialog));
   wrefresh (WindowOf(dialog));

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKDialog (dialog, ObjOf(dialog)->box);
   }
}

/*
 * This function draws the dialog widget.
 */
static void _drawCDKDialog (CDKOBJS *object, boolean Box)
{
   CDKDIALOG *dialog = (CDKDIALOG *)object;
   int x = 0;

   /* Is there a shadow? */
   if (dialog->shadowWin != 0)
   {
      drawShadow (dialog->shadowWin);
   }

   /* Box the widget if they asked. */
   if (Box)
   {
      attrbox (dialog->win,
		dialog->ULChar, dialog->URChar,
		dialog->LLChar, dialog->LRChar,
		dialog->HChar,	dialog->VChar,
		dialog->BoxAttrib);
   }

   /* Draw in the message. */
   for (x=0; x < dialog->messageRows; x++)
   {
      writeChtype (dialog->win,
			dialog->infoPos[x], x + 1,
			dialog->info[x],
			HORIZONTAL, 0,
			dialog->infoLen[x]);
   }

   /* Draw in the buttons. */
   drawCDKDialogButtons (dialog);

   /* Refresh the window. */
   touchwin (dialog->win);
   wrefresh (dialog->win);
}

/*
 * This function destroys the dialog widget.
 */
void destroyCDKDialog (CDKDIALOG *dialog)
{
   /* Declare local variables. */
   int x = 0;

   /* Erase the object. */
   eraseCDKDialog (dialog);

   /* Clean up the char pointers. */
   for (x=0; x < dialog->messageRows ; x++)
   {
      freeChtype (dialog->info[x]);
   }
   for (x=0; x < dialog->buttonCount; x++)
   {
      freeChtype (dialog->buttonLabel[x]);
   }

   /* Clean up the windows. */
   deleteCursesWindow (dialog->win);
   deleteCursesWindow (dialog->shadowWin);

   /* Unregister this object. */
   unregisterCDKObject (vDIALOG, dialog);

   /* Finish cleaning up. */
   free (dialog);
}

/*
 * This function erases the dialog widget from the screen.
 */
static void _eraseCDKDialog (CDKOBJS *object)
{
   CDKDIALOG *dialog = (CDKDIALOG *)object;
   eraseCursesWindow (dialog->win);
   eraseCursesWindow (dialog->shadowWin);
}

/*
 * This sets attributes of the dialog box.
 */
void setCDKDialog (CDKDIALOG *dialog, chtype highlight, boolean separator, boolean Box)
{
   setCDKDialogHighlight (dialog, highlight);
   setCDKDialogSeparator (dialog, separator);
   setCDKDialogBox (dialog, Box);
}

/*
 * This sets the highlight attribute for the buttons.
 */
void setCDKDialogHighlight (CDKDIALOG *dialog, chtype highlight)
{
   dialog->highlight = highlight;
}
chtype getCDKDialogHighlight (CDKDIALOG *dialog)
{
   return dialog->highlight;
}

/*
 * This sets whether or not the dialog box will have a separator line.
 */
void setCDKDialogSeparator (CDKDIALOG *dialog, boolean separator)
{
   dialog->separator = separator;
}
boolean getCDKDialogSeparator (CDKDIALOG *dialog)
{
   return dialog->separator;
}

/*
 * This sets the box attribute of the widget.
 */
void setCDKDialogBox (CDKDIALOG *dialog, boolean Box)
{
   ObjOf(dialog)->box = Box;
}
boolean getCDKDialogBox (CDKDIALOG *dialog)
{
   return ObjOf(dialog)->box;
}

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKDialogULChar (CDKDIALOG *dialog, chtype character)
{
   dialog->ULChar = character;
}
void setCDKDialogURChar (CDKDIALOG *dialog, chtype character)
{
   dialog->URChar = character;
}
void setCDKDialogLLChar (CDKDIALOG *dialog, chtype character)
{
   dialog->LLChar = character;
}
void setCDKDialogLRChar (CDKDIALOG *dialog, chtype character)
{
   dialog->LRChar = character;
}
void setCDKDialogVerticalChar (CDKDIALOG *dialog, chtype character)
{
   dialog->VChar = character;
}
void setCDKDialogHorizontalChar (CDKDIALOG *dialog, chtype character)
{
   dialog->HChar = character;
}
void setCDKDialogBoxAttribute (CDKDIALOG *dialog, chtype character)
{
   dialog->BoxAttrib = character;
}

/*
 * This sets the background color of the widget.
 */
void setCDKDialogBackgroundColor (CDKDIALOG *dialog, char *color)
{
   chtype *holder = 0;
   int junk1, junk2;

   /* Make sure the color isn't null. */
   if (color == 0)
   {
      return;
   }

   /* Convert the value of the environment variable to a chtype. */
   holder = char2Chtype (color, &junk1, &junk2);

   /* Set the widgets background color. */
   wbkgd (dialog->win, holder[0]);

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This draws the dialog buttons and the separation line.
 */
void drawCDKDialogButtons (CDKDIALOG *dialog)
{
   /* Declare local variables. */
   int x;

   for (x=0; x < dialog->buttonCount; x++)
   {
      writeChtype (dialog->win,
			dialog->buttonPos[x],
			dialog->boxHeight-2,
			dialog->buttonLabel[x],
			HORIZONTAL, 0,
			dialog->buttonLen[x]);
   }
   writeChtypeAttrib (dialog->win,
			dialog->buttonPos[dialog->currentButton],
			dialog->boxHeight-2,
			dialog->buttonLabel[dialog->currentButton],
			dialog->highlight,
			HORIZONTAL, 0,
			dialog->buttonLen[dialog->currentButton]);

   /* Draw the separation line. */
   if (dialog->separator)
   {
      for (x=1; x < dialog->boxWidth-1; x++)
      {
	 mvwaddch (dialog->win, dialog->boxHeight-3, x, ACS_HLINE | dialog->BoxAttrib);
      }
      mvwaddch (dialog->win, dialog->boxHeight-3, 0, ACS_LTEE | dialog->BoxAttrib);
      mvwaddch (dialog->win, dialog->boxHeight-3, getmaxx(dialog->win)-1, ACS_RTEE | dialog->BoxAttrib);
   }
}

/*
 * This function sets the pre-process function.
 */
void setCDKDialogPreProcess (CDKDIALOG *dialog, PROCESSFN callback, void *data)
{
   dialog->preProcessFunction = callback;
   dialog->preProcessData = data;
}

/*
 * This function sets the post-process function.
 */
void setCDKDialogPostProcess (CDKDIALOG *dialog, PROCESSFN callback, void *data)
{
   dialog->postProcessFunction = callback;
   dialog->postProcessData = data;
}
