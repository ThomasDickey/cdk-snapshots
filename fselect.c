#include "cdk.h"

/*
 * $Author: tom $
 * $Date: 1999/05/30 00:16:28 $
 * $Revision: 1.21 $
 */

/*
 * Declare file local prototypes.
 */
static int fselectAdjustScrollCB (EObjectType objectType, void *object, void *clientData, chtype key);
static int displayFileInfoCB (EObjectType objectType, void *object, void *clientData, chtype key);
static int completeFilenameCB (EObjectType objectType, void *object, void *clientData, chtype key);
static char *expandFilename (char *filename);

/*
 * Declare file local variables.
 */
extern char *GPasteBuffer;

DeclareCDKObjects(my_funcs,Fselect)

/*
 * This creates a file selection widget.
 */
CDKFSELECT *newCDKFselect (CDKSCREEN *cdkscreen, int xplace, int yplace, int height, int width, char *title, char *label, chtype fieldAttribute, chtype fillerChar, chtype highlight, char *dAttribute, char *fAttribute, char *lAttribute, char *sAttribute, boolean Box, boolean shadow)
{
  /* Set up some variables. */
   CDKFSELECT *fselect	= newCDKObject(CDKFSELECT, &my_funcs);
   int parentWidth	= getmaxx(cdkscreen->window) - 1;
   int parentHeight	= getmaxy(cdkscreen->window) - 1;
   int boxWidth		= width;
   int boxHeight	= height;
   int xpos		= xplace;
   int ypos		= yplace;
   int entryWidth, x, labelLen, junk;
   chtype *chtypeString;

  /*
   * If the height is a negative value, the height will
   * be ROWS-height, otherwise, the height will be the
   * given height.
   */
   boxHeight = setWidgetDimension (parentHeight, height, 0);

  /*
   * If the width is a negative value, the width will
   * be COLS-width, otherwise, the width will be the
   * given width.
   */
   boxWidth = setWidgetDimension (parentWidth, width, 0);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make sure the box isn't too small. */
   boxWidth = (boxWidth < 15 ? 15 : boxWidth);
   boxHeight = (boxHeight < 6 ? 6 : boxHeight);

   /* Make the file selector window. */
   fselect->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Is the window NULL? */
   if (fselect->win == (WINDOW *)NULL)
   {
      return ((CDKFSELECT *)NULL);
   }
   keypad (fselect->win, TRUE);

   /* Set some variables. */
   ScreenOf(fselect)		= cdkscreen;
   fselect->parent		= cdkscreen->window;
   fselect->dirAttribute	= copyChar (dAttribute);
   fselect->fileAttribute	= copyChar (fAttribute);
   fselect->linkAttribute	= copyChar (lAttribute);
   fselect->sockAttribute	= copyChar (sAttribute);
   fselect->highlight		= highlight;
   fselect->fillerCharacter	= fillerChar;
   fselect->fieldAttribute	= fieldAttribute;
   fselect->boxHeight		= boxHeight;
   fselect->boxWidth		= boxWidth;
   fselect->fileCounter		= 0;
   fselect->pwd			= (char *)NULL;
   fselect->exitType		= vNEVER_ACTIVATED;
   ObjOf(fselect)->box		= Box;
   fselect->shadow		= shadow;
   fselect->shadowWin		= (WINDOW *)NULL;

   /* Zero out the contents of the directory listing. */
   for (x=0; x < MAX_ITEMS; x++)
   {
      fselect->dirContents[x] = (char *)NULL;
   }

   /* Get the present working directory. */
   fselect->pwd	= getcwd (NULL, 512);

   /* Get the contents of the current directory. */
   setCDKFselectDirContents (fselect);

   /* Create the entry field in the selector. */
   chtypeString = char2Chtype (label, &labelLen, &junk);
   freeChtype (chtypeString);
   entryWidth = boxWidth - labelLen - 3;
   fselect->entryField = newCDKEntry (cdkscreen,
					getbegx(fselect->win),
					getbegy(fselect->win),
					title, label,
					fieldAttribute, fillerChar,
					vMIXED, entryWidth, 0, 512,
					Box, FALSE);

   /* Make sure the widget was created. */
   if (fselect->entryField == (CDKENTRY *)NULL)
   {
      /* Clean up. */
      for (x=0; x < MAX_ITEMS; x++)
      {
         freeChar (fselect->dirContents[x]);
      }
      freeChar (fselect->pwd);
      freeChar (fselect->dirAttribute);
      freeChar (fselect->fileAttribute);
      freeChar (fselect->linkAttribute);
      freeChar (fselect->sockAttribute);
      deleteCursesWindow (fselect->win);
      return ((CDKFSELECT *)NULL);
   }

   /* Set the lower left/right characters of the entry field. */
   setCDKEntryLLChar (fselect->entryField, ACS_LTEE);
   setCDKEntryLRChar (fselect->entryField, ACS_RTEE);

   /* Define the callbacks for the entry field. */
   bindCDKObject (vENTRY, fselect->entryField, KEY_UP, fselectAdjustScrollCB, fselect);
   bindCDKObject (vENTRY, fselect->entryField, KEY_PPAGE, fselectAdjustScrollCB, fselect);
   bindCDKObject (vENTRY, fselect->entryField, '', fselectAdjustScrollCB, fselect);
   bindCDKObject (vENTRY, fselect->entryField, KEY_DOWN, fselectAdjustScrollCB, fselect);
   bindCDKObject (vENTRY, fselect->entryField, KEY_NPAGE, fselectAdjustScrollCB, fselect);
   bindCDKObject (vENTRY, fselect->entryField, '', fselectAdjustScrollCB, fselect);
   bindCDKObject (vENTRY, fselect->entryField, KEY_TAB, completeFilenameCB, fselect);
   bindCDKObject (vENTRY, fselect->entryField, '', displayFileInfoCB, fselect);

   /* Put the current working directory in the entry field. */
   setCDKEntryValue (fselect->entryField, fselect->pwd);

   /* Create the scrolling list in the selector. */
   fselect->scrollField	= newCDKScroll (cdkscreen,
					getbegx(fselect->win),
					getbegy(fselect->win) + (fselect->entryField)->titleLines + 2,
					RIGHT,
					boxHeight - (fselect->entryField)->titleLines - 3,
					boxWidth-2,
					(char *)NULL,
					fselect->dirContents,
					fselect->fileCounter,
					NONUMBERS, fselect->highlight,
					Box, FALSE);

   /* Set the lower left/right characters of the entry field. */
   setCDKScrollULChar (fselect->scrollField, ACS_LTEE);
   setCDKScrollURChar (fselect->scrollField, ACS_RTEE);

   /* Do we want a shadow? */
   if (shadow)
   {
      fselect->shadowWin = newwin (boxHeight, boxWidth, ypos + 1, xpos + 1);
   }

   /* Register this baby. */
   registerCDKObject (cdkscreen, vFSELECT, fselect);

   /* Return the file selector pointer. */
   return (fselect);
}

/*
 * This erases the file selector from the screen.
 */
static void _eraseCDKFselect (CDKOBJS *object)
{
   CDKFSELECT * fselect = (CDKFSELECT *)object;

   eraseCDKScroll (fselect->scrollField);
   eraseCDKEntry (fselect->entryField);
   eraseCursesWindow (fselect->win);
}

/*
 * This moves the fselect field to the given location.
 */
static void _moveCDKFselect (CDKOBJS *object, int xplace, int yplace, boolean relative, boolean refresh_flag)
{
   CDKFSELECT *fselect = (CDKFSELECT *)object;
   /* Declare local variables. */
   int currentX = getbegx(fselect->win);
   int currentY = getbegy(fselect->win);
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
      xpos = getbegx(fselect->win) + xplace;
      ypos = getbegy(fselect->win) + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (WindowOf(fselect), &xpos, &ypos, fselect->boxWidth, fselect->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   moveCursesWindow(fselect->win, -xdiff, -ydiff);

   /* If there is a shadow box we have to move it too. */
   if (fselect->shadow)
   {
      moveCursesWindow(fselect->shadowWin, -xdiff, -ydiff);
   }

   /* Move the sub-widgets. */
   moveCDKEntry (fselect->entryField, xplace, yplace, relative, FALSE);
   moveCDKScroll (fselect->scrollField, xplace, yplace, relative, FALSE);
   touchwin (fselect->entryField->win);
   touchwin (fselect->scrollField->win);

   /* Redraw the window, if they asked for it. */
   if (refresh_flag)
   {
      drawCDKFselect (fselect, ObjOf(fselect)->box);
   }
}

/*
 * This draws the file selector widget.
 */
static void _drawCDKFselect (CDKOBJS *object, boolean Box GCC_UNUSED)
{
   CDKFSELECT *fselect = (CDKFSELECT *)object;

   /* Draw in the shadow if we need to. */
   if (fselect->shadowWin != (WINDOW *)NULL)
   {
      drawShadow (fselect->shadowWin);
   }

   /* Draw in the entry field. */
   drawCDKEntry (fselect->entryField, ObjOf(fselect->entryField)->box);

   /* Draw in the scroll field. */
   drawCDKScroll (fselect->scrollField, ObjOf(fselect->scrollField)->box);
}

/*
 * This means you want to use the given file selector. It takes input
 * from the keyboard, and when it's done, it fills the entry info
 * element of the structure with what was typed.
 */
char *activateCDKFselect (CDKFSELECT *fselect, chtype *actions)
{
   /* Declare local variables. */
   chtype input	= (chtype)NULL;
   char *ret	= (char *)NULL;

   /* Draw the widget. */
   drawCDKFselect (fselect, ObjOf(fselect)->box);

   /* Check if 'actions' is NULL. */
   if (actions == (chtype *)NULL)
   {
      for (;;)
      {
         /* Get the input. */
         input = wgetch (fselect->entryField->fieldWin);

         /* Inject the character into the widget. */
         ret = injectCDKFselect (fselect, input);
         if (fselect->exitType != vEARLY_EXIT)
         {
            return ret;
         }
      }
   }
   else
   {
      int length = chlen (actions);
      int x =0;

      /* Inject each character one at a time. */
      for (x=0; x < length; x++)
      {
         ret = injectCDKFselect (fselect, actions[x]);
         if (fselect->exitType != vEARLY_EXIT)
         {
            return ret;
         }
      }
   }

   /* Set the exit type and exit. */
   fselect->exitType = vEARLY_EXIT;
   return (char *)NULL;
}

/*
 * This injects a single character into the file selector.
 */
char *injectCDKFselect (CDKFSELECT *fselect, chtype input)
{
   /* Declare local variables. */
   char 	*filename;
   boolean	file;

   /* Let the user play. */
   filename = injectCDKEntry (fselect->entryField, input);

   /* Copy the entry field exitType to the fileselector. */
   fselect->exitType = fselect->entryField->exitType;

   /* If we exited early, make sure we don't interpret it as a file. */
   if (fselect->exitType == vEARLY_EXIT)
   {
      return (char *)NULL;
   }

   /* Can we change into the directory? */
   file = chdir (filename);
   chdir (fselect->pwd);

   /* If it's not a directory, return the filename. */
   if (file != 0)
   {
      /* It's a regular file, create the full path. */
      fselect->pathname = copyChar (filename);

      /* Return the complete pathname. */
      return (fselect->pathname);
   }
   else
   {
      /* Set the file selector information. */
      setCDKFselect (fselect, filename,
			fselect->fieldAttribute, fselect->fillerCharacter,
			fselect->highlight,
			fselect->dirAttribute, fselect->fileAttribute,
			fselect->linkAttribute, fselect->sockAttribute,
			ObjOf(fselect)->box);

      /* Redraw the scrolling list. */
      drawCDKScroll (fselect->scrollField, ObjOf(fselect->scrollField)->box);
   }

   /* Set the exit type and return a null pointer. */
   fselect->exitType = vEARLY_EXIT;
   return (char *)NULL;
}

/*
 * This function sets the information inside the file selector.
 */
void setCDKFselect (CDKFSELECT *fselect, char *directory, chtype fieldAttrib, chtype filler, chtype highlight, char *dirAttribute, char *fileAttribute, char *linkAttribute, char *sockAttribute, boolean Box GCC_UNUSED)
{
   /* Declare local variables. */
   CDKSCROLL *fscroll	= fselect->scrollField;
   CDKENTRY *fentry	= fselect->entryField;
   char *tempDir	= (char *)NULL;
   char *mesg[10], newDirectory[2000], temp[100];
   int ret;

   /* Keep the info sent to us. */
   fselect->fieldAttribute	= fieldAttrib;
   fselect->fillerCharacter	= filler;
   fselect->highlight		= highlight;
   strcpy (newDirectory, directory);

   /* Set the attributes of the entry field/scrolling list. */
   setCDKEntryFillerChar (fentry, filler);
   setCDKScrollHighlight (fscroll, highlight);

   /* Only do the directory stuff if the directory is not NULL. */
   if (directory != (char *)NULL)
   {
      /* Try to expand the directory if it starts with a ~ */
      if (directory[0] == '~')
      {
         tempDir = expandFilename (directory);
         if (tempDir != (char *)NULL)
         {
            strcpy (newDirectory, tempDir);
            freeChar (tempDir);
         }
      }

      /* Change directories. */
      ret = chdir (newDirectory);
      if (ret != 0)
      {
         /* Beep at them. */
         Beep();

         /* Couldn't get into the directory, pop up a little message. */
         sprintf (temp, "<C>Could not change into %s", newDirectory);
         mesg[0] = copyChar (temp);

#ifdef HAVE_STRERROR
         sprintf (temp, "<C></U>%s", strerror(errno));
         mesg[1] = copyChar (temp);
#else
         sprintf (temp, "<C></U>Unknown reason.");
         mesg[1] = copyChar (temp);
#endif

         mesg[2] = " ";
         mesg[3] = "<C>Press Any Key To Continue.";

         /* Pop Up a message. */
         popupLabel (ScreenOf(fselect), mesg, 4);

         /* Clean up some memory. */
         freeChar (mesg[0]);
         freeChar (mesg[1]);

         /* Get out of here. */
         eraseCDKFselect (fselect);
         drawCDKFselect (fselect, ObjOf(fselect)->box);
         return;
      }
   }

   /*
    * If the information coming in is the same as the information
    * that is already there, there is no need to destroy it.
    */
   if (fselect->pwd != directory)
   {
      /* Remove the old pointer	and set the new value. */
      freeChar (fselect->pwd);
      fselect->pwd = getcwd (NULL, 512);
   }
   if (fselect->fileAttribute != fileAttribute)
   {
      /* Remove the old pointer	and set the new value. */
      freeChar (fselect->fileAttribute);
      fselect->fileAttribute = copyChar (fileAttribute);
   }
   if (fselect->dirAttribute != dirAttribute)
   {
      /* Remove the old pointer	and set the new value. */
      freeChar (fselect->dirAttribute);
      fselect->dirAttribute = copyChar (dirAttribute);
   }
   if (fselect->linkAttribute != linkAttribute)
   {
      /* Remove the old pointer	and set the new value. */
      freeChar (fselect->linkAttribute);
      fselect->linkAttribute = copyChar (linkAttribute);
   }
   if (fselect->sockAttribute != sockAttribute)
   {
      /* Remove the old pointer	and set the new value. */
      freeChar (fselect->sockAttribute);
      fselect->sockAttribute = copyChar (sockAttribute);
   }

   /* Set the contents of the entry field. */
   setCDKEntryValue (fentry, fselect->pwd);
   drawCDKEntry (fentry, ObjOf(fentry)->box);

   /* Get the directory contents. */
   if (setCDKFselectDirContents (fselect) == 0)
   {
      Beep();
      return;
   }

   /* Set the values in the scrolling list. */
   setCDKScrollItems (fscroll,
			fselect->dirContents,
			fselect->fileCounter,
			FALSE);
}

/*
 * This creates a list of the files in the current directory.
 */
int setCDKFselectDirContents (CDKFSELECT *fselect)
{
   /* Declare local variables. */
   struct stat fileStat;
   char *dirList[MAX_ITEMS];
   char temp[200], mode;
   int fileCount;
   int x = 0;

   /* Get the directory contents. */
   fileCount = getDirectoryContents (fselect->pwd, dirList, MAX_ITEMS);
   if (fileCount == -1)
   {
      /* We couldn't read the directory. Return. */
      return 0;
   }

   /* Clean out the old directory list. */
   for (x=0; x < fselect->fileCounter; x++)
   {
      freeChar (fselect->dirContents[x]);
   }
   fselect->fileCounter = fileCount;

   /* Set the properties of the files. */
   for (x=0; x < fselect->fileCounter; x++)
   {
      /* Stat the file. */
      lstat (dirList[x], &fileStat);

      /* Check the mode. */
      mode = ' ';
      if (((fileStat.st_mode & S_IXUSR) != 0) ||
		((fileStat.st_mode & S_IXGRP) != 0) ||
		((fileStat.st_mode & S_IXOTH) != 0))
      {
          mode = '*';
      }

      /* Create the filename. */
      if (S_ISLNK (fileStat.st_mode))
      {
         sprintf (temp, "%s%s@", fselect->linkAttribute, dirList[x]);
         fselect->dirContents[x] = copyChar (temp);
      }
      else if (S_ISSOCK (fileStat.st_mode))
      {
         sprintf (temp, "%s%s&", fselect->sockAttribute, dirList[x]);
         fselect->dirContents[x] = copyChar (temp);
      }
      else if (S_ISREG(fileStat.st_mode))
      {
         sprintf (temp, "%s%s%c", fselect->fileAttribute, dirList[x], mode);
         fselect->dirContents[x] = copyChar (temp);
      }
      else if (S_ISDIR(fileStat.st_mode))
      {
         sprintf (temp, "%s%s/", fselect->dirAttribute, dirList[x]);
         fselect->dirContents[x] = copyChar (temp);
      }
      else
      {
         sprintf (temp, "%s%c", dirList[x], mode);
         fselect->dirContents[x] = copyChar (temp);
      }

      /* Free up this piece of memory. */
      freeChar (dirList[x]);
   }
   return 1;
}
char **getCDKFselectDirContents (CDKFSELECT *fselect, int *count)
{
   (*count) = fselect->fileCounter;
   return fselect->dirContents;
}

/*
 * This sets the current directory of the file selector.
 */
int setCDKFselectDirectory (CDKFSELECT *fselect, char *directory)
{
   /* Declare local variables. */
   CDKENTRY *fentry	= fselect->entryField;
   CDKSCROLL *fscroll	= fselect->scrollField;
   int ret;

   /*
    * If the directory supplied is the same as what is already
    * there, return.
    */
   if (fselect->pwd == directory)
   {
      return 1;
   }

   /* Try to chdir into the given directory. */
   ret = chdir (directory);
   if (ret != 0)
   {
      return 0;
   }

  /*
   * Remove the pwd pointer and set it
   * with the present working directory.
   */
   freeChar (fselect->pwd);
   fselect->pwd = getcwd (NULL, 512);

   /* Set the contents of the entry field. */
   setCDKEntryValue (fentry, fselect->pwd);
   drawCDKEntry (fentry, ObjOf(fentry)->box);

   /* Get the directory contents. */
   if (setCDKFselectDirContents (fselect) == 0)
   {
      return 0;
   }

   /* Set the values in the scrolling list. */
   setCDKScrollItems (fscroll, fselect->dirContents,
			fselect->fileCounter,
			FALSE);
   return 1;
}
char *getCDKFselectDirectory (CDKFSELECT *fselect)
{
   return fselect->pwd;
}
/*
 * This sets the filler character of the entry field.
 */
void setCDKFselectFillerChar (CDKFSELECT *fselect, chtype filler)
{
   CDKENTRY *fentry = fselect->entryField;
   fselect->fillerCharacter = filler;
   setCDKEntryFillerChar (fentry, filler);
}
chtype getCDKFselectFillerChar (CDKFSELECT *fselect)
{
   return fselect->fillerCharacter;
}

/*
 * This sets the highlight bar of the scrolling list.
 */
void setCDKFselectHighlight (CDKFSELECT *fselect, chtype highlight)
{
   CDKSCROLL *fscroll = (CDKSCROLL *)fselect->scrollField;
   fselect->highlight = highlight;
   setCDKScrollHighlight (fscroll, highlight);

}
chtype getCDKFselectHighlight (CDKFSELECT *fselect)
{
   return fselect->highlight;
}

/*
 * This sets the attribute of the directory attribute in the
 * scrolling list.
 */
void setCDKFselectDirAttribute (CDKFSELECT *fselect, char *attribute)
{
   /* Make sure they are not one in the same. */
   if (fselect->dirAttribute == attribute)
   {
      return;
   }

   /* Clean out the old attribute, and set the new. */
   freeChar (fselect->dirAttribute);
   fselect->dirAttribute = copyChar (attribute);

  /*
   * We need to reset the contents of the scrolling
   * list using the new attribute.
   */
   setCDKFselectDirContents (fselect);
}
char *getCDKFselectDirAttribute (CDKFSELECT *fselect)
{
   return fselect->dirAttribute;
}

/*
 * This sets the attribute of the link attribute in the
 * scrolling list.
 */
void setCDKFselectLinkAttribute (CDKFSELECT *fselect, char *attribute)
{
   /* Make sure they are not one in the same. */
   if (fselect->linkAttribute == attribute)
   {
      return;
   }

   /* Clean out the old attribute, and set the new. */
   freeChar (fselect->linkAttribute);
   fselect->linkAttribute = copyChar (attribute);

  /*
   * We need to reset the contents of the scrolling
   * list using the new attribute.
   */
   setCDKFselectDirContents (fselect);
}
char *getCDKFselectLinkAttribute (CDKFSELECT *fselect)
{
   return fselect->linkAttribute;
}

/*
 * This sets the attribute of the link attribute in the
 * scrolling list.
 */
void setCDKFselectSocketAttribute (CDKFSELECT *fselect, char *attribute)
{
   /* Make sure they are not one in the same. */
   if (fselect->sockAttribute == attribute)
   {
      return;
   }

   /* Clean out the old attribute, and set the new. */
   freeChar (fselect->sockAttribute);
   fselect->sockAttribute = copyChar (attribute);

  /*
   * We need to reset the contents of the scrolling
   * list using the new attribute.
   */
   setCDKFselectDirContents (fselect);
}
char *getCDKFselectSocketAttribute (CDKFSELECT *fselect)
{
   return fselect->sockAttribute;
}

/*
 * This sets the attribute of the link attribute in the
 * scrolling list.
 */
void setCDKFselectFileAttribute (CDKFSELECT *fselect, char *attribute)
{
   /* Make sure they are not one in the same. */
   if (fselect->fileAttribute == attribute)
   {
      return;
   }

   /* Clean out the old attribute, and set the new. */
   freeChar (fselect->fileAttribute);
   fselect->fileAttribute = copyChar (attribute);

  /*
   * We need to reset the contents of the scrolling
   * list using the new attribute.
   */
   setCDKFselectDirContents (fselect);
}
char *getCDKFselectFileAttribute (CDKFSELECT *fselect)
{
   return fselect->fileAttribute;
}

/*
 * This sets the box attribute of the widget.
 */
void setCDKFselectBox (CDKFSELECT *fselect, boolean Box)
{
   ObjOf(fselect)->box = Box;
}
boolean getCDKFselectBox (CDKFSELECT *fselect)
{
   return ObjOf(fselect)->box;
}

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKFselectULChar (CDKFSELECT *fselect, chtype character)
{
   setCDKEntryULChar (fselect->entryField, character);
}
void setCDKFselectURChar (CDKFSELECT *fselect, chtype character)
{
   setCDKEntryURChar (fselect->entryField, character);
}
void setCDKFselectLLChar (CDKFSELECT *fselect, chtype character)
{
   setCDKScrollLLChar (fselect->scrollField, character);
}
void setCDKFselectLRChar (CDKFSELECT *fselect, chtype character)
{
   setCDKScrollLRChar (fselect->scrollField, character);
}
void setCDKFselectVerticalChar (CDKFSELECT *fselect, chtype character)
{
   setCDKEntryVerticalChar (fselect->entryField, character);
   setCDKScrollVerticalChar (fselect->scrollField, character);
}
void setCDKFselectHorizontalChar (CDKFSELECT *fselect, chtype character)
{
   setCDKEntryHorizontalChar (fselect->entryField, character);
   setCDKScrollHorizontalChar (fselect->scrollField, character);
}
void setCDKFselectBoxAttribute (CDKFSELECT *fselect, chtype character)
{
   setCDKEntryBoxAttribute (fselect->entryField, character);
   setCDKScrollBoxAttribute (fselect->scrollField, character);
}

/*
 * This sets the background color of the widget.
 */
void setCDKFselectBackgroundColor (CDKFSELECT *fselect, char *color)
{
   if (color != (char *)NULL)
   {
      setCDKEntryBackgroundColor (fselect->entryField, color);
      setCDKScrollBackgroundColor (fselect->scrollField, color);
   }
}

/*
 * This destroys the file selector.
 */
void destroyCDKFselect (CDKFSELECT *fselect)
{
   int x;

   /* Erase the file selector. */
   eraseCDKFselect (fselect);

   /* Free up the character pointers. */
   freeChar (fselect->pwd);
   freeChar (fselect->pathname);
   freeChar (fselect->dirAttribute);
   freeChar (fselect->fileAttribute);
   freeChar (fselect->linkAttribute);
   freeChar (fselect->sockAttribute);
   for (x=0; x < fselect->fileCounter; x++)
   {
      freeChar (fselect->dirContents[x]);
   }

   /* Destroy the other Cdk objects. */
   destroyCDKScroll (fselect->scrollField);
   destroyCDKEntry (fselect->entryField);

   /* Free up the window pointers. */
   deleteCursesWindow (fselect->shadowWin);
   deleteCursesWindow (fselect->win);

   /* Unregister the object. */
   unregisterCDKObject (vFSELECT, fselect);

   /* Free up the object pointer. */
   free (fselect);
}

/*
 ********************************
 * Callback functions.
 ********************************
 */

/*
 * This is a callback to the scrolling list which displays information
 * about the current file. (and the whole directory as well)
 */
static int displayFileInfoCB (EObjectType objectType GCC_UNUSED, void *object, void *clientData, chtype key GCC_UNUSED)
{
   /* Declare local variables. */
   CDKENTRY		*entry		= (CDKENTRY *)object;
   CDKFSELECT		*fselect	= (CDKFSELECT *)clientData;
   CDKLABEL		*infoLabel;
   struct stat		fileStat;
   struct passwd	*pwEnt;
   struct group		*grEnt;
   char			*filename;
   char			*filetype;
   char 		*mesg[10];
   char			temp[100];
   char			stringMode[15];
   int			len;
   int			intMode;

   /* Get the file name. */
   filename	= fselect->entryField->info;

   /* Get specific information about the files. */
   lstat (filename, &fileStat);

   /* Determine the file type. */
   if (S_ISLNK (fileStat.st_mode))
   {
      filetype = "Symbolic Link";
   }
   else if (S_ISSOCK (fileStat.st_mode))
   {
      filetype = "Socket";
   }
   else if (S_ISREG(fileStat.st_mode))
   {
      filetype = "Regular File";
   }
   else if (S_ISDIR(fileStat.st_mode))
   {
      filetype = "Directory";
   }
   else if (S_ISCHR(fileStat.st_mode))
   {
      filetype = "Character Device";
   }
   else if (S_ISBLK(fileStat.st_mode))
   {
      filetype = "Block Device";
   }
   else if (S_ISFIFO(fileStat.st_mode))
   {
      filetype = "FIFO Device";
   }
   else
   {
      filetype = "Unknown";
   }

   /* Get the user name and group name. */
   pwEnt = getpwuid (fileStat.st_uid);
   grEnt = getgrgid (fileStat.st_gid);

   /* Convert the mode_t type to both string and int. */
   intMode = mode2Char (stringMode, fileStat.st_mode);

   /* Create the message. */
   sprintf (temp, "Directory  : </U>%s", fselect->pwd);
   mesg[0] = copyChar (temp);

   sprintf (temp, "Filename   : </U>%s", filename);
   mesg[1] = copyChar (temp);

   sprintf (temp, "Owner      : </U>%s<!U> (%d)", pwEnt->pw_name, (int)fileStat.st_uid);
   mesg[2] = copyChar (temp);

   sprintf (temp, "Group      : </U>%s<!U> (%d)", grEnt->gr_name, (int)fileStat.st_gid);
   mesg[3] = copyChar (temp);

   sprintf (temp, "Permissions: </U>%s<!U> (%d)", stringMode, intMode);
   mesg[4] = copyChar (temp);

   sprintf (temp, "Size       : </U>%ld<!U> bytes", fileStat.st_size);
   mesg[5] = copyChar (temp);

   sprintf (temp, "Last Access: </U>%s", ctime (&fileStat.st_atime));
   len = (int)strlen (temp);
   temp[len] = '\0'; temp[len-1] = '\0';
   mesg[6] = copyChar (temp);

   sprintf (temp, "Last Change: </U>%s", ctime (&fileStat.st_ctime));
   len = (int)strlen (temp);
   temp[len] = '\0'; temp[len-1] = '\0';
   mesg[7] = copyChar (temp);

   sprintf (temp, "File Type  : </U>%s", filetype);
   mesg[8] = copyChar (temp);

   /* Create the pop up label. */
   infoLabel = newCDKLabel (entry->obj.screen, CENTER, CENTER,
				mesg, 9, TRUE, FALSE);
   drawCDKLabel (infoLabel, TRUE);
   wgetch (infoLabel->win);

   /* Clean up some memory. */
   destroyCDKLabel (infoLabel);
   freeChar (mesg[0]); freeChar (mesg[1]); freeChar (mesg[2]);
   freeChar (mesg[3]); freeChar (mesg[4]); freeChar (mesg[5]);
   freeChar (mesg[6]); freeChar (mesg[7]); freeChar (mesg[8]);

   /* Redraw the file selector. */
   drawCDKFselect (fselect, ObjOf(fselect)->box);
   return (TRUE);
}

/*
 * This tires to complete the filename.
 */
static int completeFilenameCB (EObjectType objectType GCC_UNUSED, void *object GCC_UNUSED, void *clientData, chtype key GCC_UNUSED)
{
   CDKFSELECT *fselect	= (CDKFSELECT *)clientData;
   CDKSCROLL *scrollp	= (CDKSCROLL *)fselect->scrollField;
   CDKENTRY *entry	= (CDKENTRY *)fselect->entryField;
   char *filename	= copyChar (entry->info);
   char *basename	= baseName (filename);
   char *dirname	= dirName (filename);
   char *dirPWD		= dirName (fselect->pwd);
   char *basePWD	= baseName (fselect->pwd);
   char *newFilename	= (char *)NULL;
   chtype *tempChtype	= (chtype *)NULL;
   char *tempChar	= (char *)NULL;
   int filenameLen	= 0;
   int currentIndex	= 0;
   int matches		= 0;
   int baseChars	= 0;
   int secondaryMatches	= 0;
   int isDirectory	= 0;
   char *list[MAX_ITEMS], temp[1000];
   int Index, pos, ret, j, j2, x;
   int difference, absoluteDifference;

   /* Make sure the filename is not NULL. */
   if (filename == (char *)NULL)
   {
      Beep();
      return (TRUE);
   }
   filenameLen = (int)strlen (filename);

   /* If the filename length is equal to zero, just leave. */
   if (filenameLen == 0)
   {
      Beep();
      return (TRUE);
   }

   /* Try to expand the filename if it starts with a ~ */
   if (filename[0] == '~')
   {
      newFilename = expandFilename (filename);
      if (newFilename != (char *)NULL)
      {
         freeChar (filename);
         filename = newFilename;
         setCDKEntryValue (entry, filename);
         drawCDKEntry (entry, ObjOf(entry)->box);
      }
   }

   /* Make sure we can change into the directory. */
   isDirectory = chdir (filename);
   chdir (fselect->pwd);

   /* If we can, change into the directory. */
   if (isDirectory == 0)
   {
     /*
      * The basenames of both the present working directory and
      * the basename of the filename are different. The filename
      * provided is a directory; therefore we should chdir into
      * it and reload the file selector.
      */
      setCDKFselect (fselect, filename,
			fselect->fieldAttribute, fselect->fillerCharacter,
			fselect->highlight,
			fselect->dirAttribute, fselect->fileAttribute,
			fselect->linkAttribute, fselect->sockAttribute,
			ObjOf(fselect)->box);
   }
   else
   {
     /*
      * The basenames of both the present working directory and
      * the basename of the filename are different. The filename
      * provided is a file; therefore we should chdir into the
      * basename of the file and reload the file selector.
      * Then we should continue to try to complete the filename.
      * (which is done below)
      */
      setCDKFselect (fselect, dirname,
			fselect->fieldAttribute, fselect->fillerCharacter,
			fselect->highlight,
			fselect->dirAttribute, fselect->fileAttribute,
			fselect->linkAttribute, fselect->sockAttribute,
			ObjOf(fselect)->box);

     /*
      * Set the entry field with the filename so the current
      * filename selection shows up.
      */
      setCDKEntryValue (entry, filename);
      drawCDKEntry (entry, ObjOf(entry)->box);
   }

   /* Clean up our pointers. */
   freeChar (basename);
   freeChar (dirname);
   freeChar (dirPWD);
   freeChar (basePWD);

   /* Create the file list. */
   for (x=0; x < fselect->fileCounter; x++)
   {
      /* We need to remove the special characters from the filenames. */
      tempChtype = char2Chtype (fselect->dirContents[x], &j, &j2);
      tempChar = chtype2Char (tempChtype);

      /* Create the pathname. */
      if (strcmp (fselect->pwd, "/") == 0)
      {
         sprintf (temp, "/%s", tempChar);
      }
      else
      {
         sprintf (temp, "%s/%s", fselect->pwd, tempChar);
      }
      list[x] = copyChar (temp);

      /* Clean up. */
      freeChtype (tempChtype);
      freeChar (tempChar);
   }

   /* Look for a unique filename match. */
   Index = searchList (list, fselect->fileCounter, filename);

   /* If the index is less than zero, return we didn't find a match. */
   if (Index < 0)
   {
      /* Clean up. */
      for (x=0; x < fselect->fileCounter; x++)
      {
         freeChar (list[x]);
      }
      freeChar (filename);
      Beep();
      return (TRUE);
   }

   /* Create the filename of the found file. */
   sprintf (temp, "%s", list[Index]);
   temp[(int)strlen(temp)-1] = '\0';

   /* Did we find the last file in the list? */
   if (Index == fselect->fileCounter)
   {
      /* Set the filename in the entry field. */
      setCDKEntryValue (entry, list[Index]);
      drawCDKEntry (entry, ObjOf(entry)->box);

      /* Clean up. */
      for (x=0; x < fselect->fileCounter; x++)
      {
         freeChar (list[x]);
      }
      freeChar (filename);
      return (TRUE);
   }

   /* Move to the current item in the scrolling list. */
   difference		= Index - scrollp->currentItem;
   absoluteDifference	= abs (difference);
   if (difference < 0)
   {
      for (x=0; x < absoluteDifference; x++)
      {
         injectCDKScroll (scrollp, KEY_UP);
      }
   }
   else if (difference > 0)
   {
      for (x=0; x < absoluteDifference; x++)
      {
         injectCDKScroll (scrollp, KEY_DOWN);
      }
   }
   drawCDKScroll (scrollp, ObjOf(scrollp)->box);

   /* Ok, we found a match, is the next item similar? */
   ret = strncmp (list[Index + 1], filename, filenameLen);
   if (ret == 0)
   {
      currentIndex = Index;
      baseChars = filenameLen;
      matches = 0;
      pos = 0;

      /* Determine the number of files which match. */
      while (currentIndex < fselect->fileCounter)
      {
         if (list[currentIndex] != NULL)
         {
            if (strncmp (list[currentIndex], filename, filenameLen) == 0)
            {
               matches++;
            }
         }
         currentIndex++;
      }

      /* Start looking for the common base characters. */
      for (;;)
      {
         secondaryMatches = 0;
         for (x=Index; x < Index + matches; x++)
         {
            if (list[Index][baseChars] == list[x][baseChars])
            {
               secondaryMatches++;
            }
         }

         if (secondaryMatches != matches)
         {
            Beep();
            break;
         }

         /* Inject the character into the entry field. */
         injectCDKEntry (fselect->entryField, list[Index][baseChars]);
         baseChars++;
      }
   }
   else
   {
      /* Set the entry field with the found item. */
      setCDKEntryValue (entry, temp);
      drawCDKEntry (entry, ObjOf(entry)->box);
   }

   /* Clean up. */
   for (x=0; x < fselect->fileCounter; x++)
   {
      freeChar (list[x]);
   }
   freeChar (filename);
   return (TRUE);
}

/*
 * This allows the user to delete a file.
 */
void deleteFileCB (EObjectType objectType GCC_UNUSED, void *object, void *clientData)
{
   /* Declare local variables. */
   CDKSCROLL	*fscroll	= (CDKSCROLL *)object;
   CDKFSELECT	*fselect	= (CDKFSELECT *)clientData;
   char		*buttons[]	= {"No", "Yes"};
   CDKDIALOG	*question;
   char		*mesg[10], *filename, temp[100];

   /* Get the filename which is to be deleted. */
   filename = chtype2Char (fscroll->item[fscroll->currentItem]);
   filename[(int)strlen(filename)-1] = '\0';

   /* Create the dialog message. */
   mesg[0] = "<C>Are you sure you want to delete the file:";
   sprintf (temp, "<C></U>%s?", filename); mesg[1] = copyChar (temp);

   /* Create the dialog box. */
   question = newCDKDialog (ScreenOf(fselect), CENTER, CENTER,
			mesg, 2, buttons, 2, A_REVERSE,
			TRUE, TRUE, FALSE);

   /* Clean up this pointer. (we may need it again real soon.) */
   freeChar (mesg[1]);

   /* If the said yes then try to nuke it. */
   if (activateCDKDialog (question, (chtype *)NULL) == 1)
   {
      /* If we were successful, reload the scrolling list. */
      if (unlink (filename) == 0)
      {
         /* Set the file selector information. */
         setCDKFselect (fselect, fselect->pwd,
		fselect->fieldAttribute, fselect->fillerCharacter, fselect->highlight,
		fselect->dirAttribute, fselect->fileAttribute,
		fselect->linkAttribute, fselect->sockAttribute,
		ObjOf(fselect)->box);
      }
      else
      {
         /* Pop up a message. */
#ifdef HAVE_STRERROR
         sprintf (temp, "<C>Can't delete file: <%s>", strerror (errno));
#else
         sprintf (temp, "<C>Can not delete file. Unknown reason.");
#endif
         mesg[0] = copyChar (temp);
         mesg[1] = " ";
         mesg[2] = "<C>Press any key to continue.";
         popupLabel (ScreenOf(fselect), mesg, 3);
         freeChar (mesg[0]);
      }
   }

   /* Clean up. */
   destroyCDKDialog (question);

   /* Redraw the file selector. */
   drawCDKFselect (fselect, ObjOf(fselect)->box);
}

/*
 * This function sets the pre-process function.
 */
void setCDKFselectPreProcess (CDKFSELECT *fselect, PROCESSFN callback, void *data)
{
   setCDKEntryPreProcess (fselect->entryField, callback, data);
   setCDKScrollPreProcess (fselect->scrollField, callback, data);
}

/*
 * This function sets the post-process function.
 */
void setCDKFselectPostProcess (CDKFSELECT *fselect, PROCESSFN callback, void *data)
{
   setCDKEntryPostProcess (fselect->entryField, callback, data);
   setCDKScrollPostProcess (fselect->scrollField, callback, data);
}

/*
 * Start of callback functions.
 */
static int fselectAdjustScrollCB (EObjectType objectType GCC_UNUSED, void *object GCC_UNUSED, void *clientData, chtype key)
{
   CDKFSELECT *fselect	= (CDKFSELECT *)clientData;
   CDKSCROLL *scrollp	= (CDKSCROLL *)fselect->scrollField;
   CDKENTRY *entry	= (CDKENTRY *)fselect->entryField;
   char *current, temp[1024];
   int len;

   /* Move the scrolling list. */
   injectCDKScroll (fselect->scrollField, key);

   /* Get the currently highlighted filename. */
   current = chtype2Char (scrollp->item[scrollp->currentItem]);
   len = (int)strlen (current);
   current[len-1] = '\0';

   /* Are we in the root partition. */
   if (strcmp (fselect->pwd, "/") == 0)
   {
      sprintf (temp, "/%s", current);
   }
   else
   {
      sprintf (temp, "%s/%s", fselect->pwd, current);
   }
   freeChar (current);

   /* Set the value in the entry field. */
   setCDKEntryValue (entry, temp);
   drawCDKEntry (entry, ObjOf(entry)->box);
   return (TRUE);
}

/*
 * This takes a ~ type account name and returns the full
 * pathname.
 */
static char *expandFilename (char *filename)
{
   struct passwd *accountInfo;
   char accountName[256], pathname[1024], fullPath[2048];
   int slashFound = 0;
   int pos = 0;
   int len, x;

   /* Make sure the filename is not NULL. */
   if ((filename == (char *)NULL) || (len = strlen (filename)) == 0)
   {
      return (char *)NULL;
  }

   /* Make sure the first character is a tilde. */
   if (filename[0] != '~')
   {
      return (char *)NULL;
   }

   /* Find the account name in the filename. */
   for (x=1; x < len; x++)
   {
      if (filename[x] != '/' && slashFound == 0)
      {
         accountName[pos++] = filename[x];
      }
      else
      {
         if (slashFound == 0)
         {
            accountName[pos] = '\0';
            pos = 0;
            slashFound = 1;
         }
         else
         {
            pathname[pos++] = filename[x];
         }
      }
   }

   /* Determine if the account name has a home directory. */
   accountInfo = getpwnam (accountName);
   if (accountInfo == NULL)
   {
      return (char *)NULL;
   }

  /*
   * Construct the full pathname. We do this because someone
   * may have had a pathname at the end of the account name
   * and we want to keep it.
   */
   sprintf (fullPath, "%s/%s", accountInfo->pw_dir, pathname);
   return copyChar (fullPath);
}
