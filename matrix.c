#include "cdk.h"
#include <limits.h>

/*
 * $Author: glovem $
 * $Date: 1998/03/02 16:31:18 $
 * $Revision: 1.103 $
 */

/*
 * Declare file local prototypes.
 */
void highlightCDKMatrixCell (CDKMATRIX *matrix);
void CDKMatrixCallBack (CDKMATRIX *matrix, chtype input);
void drawCDKMatrixCell (CDKMATRIX *matrix,
			int srow, int scol,
			int vrow, int vcol,
			chtype attr, boolean box);
void redrawTitles (CDKMATRIX *matrix, int row, int col);

/*
 * Declare file local variables.
 */
extern char *GPasteBuffer;

/*
 * This function creates the matrix widget.
 */
CDKMATRIX *newCDKMatrix (CDKSCREEN *cdkscreen, int xplace, int yplace, int rows, int cols, int vrows, int vcols, char *title, char **rowtitles, char **coltitles, int *colwidths, int *colvalues, int rspace, int cspace, chtype filler, int dominant, boolean boxMatrix, boolean boxCell, boolean shadow)
{
   /* Declare local variables. */
   CDKMATRIX *matrix	= (CDKMATRIX *)malloc (sizeof(CDKMATRIX));
   int parentWidth	= WIN_WIDTH (cdkscreen->window);
   int parentHeight	= WIN_HEIGHT (cdkscreen->window);
   chtype *junk		= (chtype *)NULL;
   int boxHeight	= 0;
   int boxWidth		= 0;
   int xpos		= xplace;
   int ypos		= yplace;
   int maxWidth		= INT_MIN;
   int maxRowTitleWidth	= 0;
   int rowSpace		= MAXIMUM (0, rspace);
   int colSpace		= MAXIMUM (0, cspace);
   int begx		= 0;
   int begy		= 0;
   int cellWidth	= 0;
   char *temp[MAX_LINES];
   int x, y, z, w, len, junk2;

   /* Make sure that the number of rows/cols/vrows/vcols is not zero. */
   if (rows == 0 || cols == 0 || vrows == 0 || vcols == 0)
   {
      /* Free up any used memory. */
      free (matrix);

      /* Return a NULL pointer. */
      return ((CDKMATRIX *)NULL);
   }

  /*
   * Make sure the number of virtual cells is not larger than
   * the physical size.
   */
   vrows = (vrows > rows ? rows : vrows);
   vcols = (vcols > cols ? cols : vcols);

   /* We need to determine the width of the matrix box. */
   if (title != (char *)NULL)
   {
      /* We need to split the title on \n. */
      matrix->titleLines = splitString (title, temp, '\n');
      for (x=0; x < matrix->titleLines; x++)
      {
         junk = char2Chtype (temp[x], &len, &junk2);
         maxWidth = MAXIMUM (maxWidth, len);
         freeChtype (junk);
         freeChar (temp[x]);
      }
   }
   else
   {
      /* No title? Set the required variables. */
      matrix->titleLines = 0;
   }
   boxWidth = maxWidth;

   /* Determine the height of the box. */
   if (vrows == 1)
   {
      boxHeight = 6 + matrix->titleLines;
   }
   else
   {
      if (rowSpace == 0)
      {
         boxHeight = 6 + matrix->titleLines + ((vrows - 1) * 2);
      }
      else
      {
         boxHeight = 3 + matrix->titleLines + (vrows * 3) + ((vrows-1) * (rowSpace-1));
      }
   }

   /* Determine the maximum row title width */
   for (x=1; x <= rows; x++)
   {
      matrix->rowtitle[x] = char2Chtype (rowtitles[x], &matrix->rowtitleLen[x], &matrix->rowtitlePos[x]);
      maxRowTitleWidth = MAXIMUM (maxRowTitleWidth, matrix->rowtitleLen[x]);
   }
   matrix->maxrt = maxRowTitleWidth + 2;

   /* We need to rejustify the row title cell info. */
   for (x=1; x <= rows; x++)
   {
      matrix->rowtitlePos[x] = justifyString (matrix->maxrt, matrix->rowtitleLen[x], matrix->rowtitlePos[x]);
   }

   /* Determine the width of the matrix. */
   maxWidth = 2 + matrix->maxrt;
   for (x=1; x <= vcols; x++)
   {
      maxWidth += colwidths[x] + 2 + colSpace;
   }
   maxWidth -= (colSpace-1);
   boxWidth = MAXIMUM (maxWidth, boxWidth);

   /* Translate the char * items to chtype * */
   if (title != (char *)NULL)
   {
      /* We need to split the title on \n. */
      matrix->titleLines = splitString (title, temp, '\n');
 
      /* For each line in the title, convert from char * to chtype * */
      for (x=0; x < matrix->titleLines; x++)
      {
         matrix->title[x]	= char2Chtype (temp[x], &matrix->titleLen[x], &matrix->titlePos[x]);
         matrix->titlePos[x]	= justifyString (boxWidth, matrix->titleLen[x], matrix->titlePos[x]);
         freeChar (temp[x]);
      }
   }

  /*
   * Make sure the dimensions of the window didn't 
   * extend beyond the dimensions of the parent window.
   */
   boxWidth = (boxWidth > parentWidth ? parentWidth : boxWidth);
   boxHeight = (boxHeight > parentHeight ? parentHeight : boxHeight);

   /* Rejustify the x and y positions if we need to. */
   alignxy (cdkscreen->window, &xpos, &ypos, boxWidth, boxHeight);

   /* Make the pop-up window. */
   matrix->win = newwin (boxHeight, boxWidth, ypos, xpos);

   /* Check if the new window is NULL or not. */
   if (matrix->win == (WINDOW *)NULL)
   {
      /* Free up any used memory. */
      for (z=1; z <= rows; z++)
      {
         freeChtype (matrix->rowtitle[z]);
      }
      free (matrix);

      /* Return a NULL pointer. */
      return ((CDKMATRIX *)NULL);
   }

   /* Make the subwindows in the pop-up. */
   begx = xpos;
   begy = ypos + 1 + matrix->titleLines;

   /* Make the 'empty' 0x0 cell. */
   matrix->cell[0][0] = subwin (matrix->win, 3, matrix->maxrt, begy, begx);
   begx += matrix->maxrt + 1;

   /* Make the column titles. */
   for (x=1; x <= vcols; x++)
   {
      cellWidth = colwidths[x] + 3;
      matrix->cell[0][x] = subwin (matrix->win, 1, cellWidth, begy, begx);

      /* Is the subwindow NULL */
      if (matrix->cell[0][x] == (WINDOW *)NULL)
      {
         /* Free up any used memory. */
         for (z=1; z <= rows; z++)
         {
            freeChtype (matrix->rowtitle[z]);
         }

         /* We have to delete any windows created so far. */
         for (z=1; z < x; z++)
         {
            deleteCursesWindow (matrix->cell[0][z]);
         }
         free (matrix);

         /* Return a NULL pointer. */
         return ((CDKMATRIX *)NULL);
      }
      begx +=  cellWidth + colSpace - 1;
   }
   begy++;

   /* Make the main cell body */
   for (x=1; x <= vrows; x++)
   {
      /* Make the row titles */
      matrix->cell[x][0] = subwin (matrix->win, 3, matrix->maxrt, begy, xpos+1);

      /* Is the window NULL */
      if (matrix->cell[x][0] == (WINDOW *)NULL)
      {
         /* Free up any used memory. */
         for (z=1; z <= rows; z++)
         {
            freeChtype (matrix->rowtitle[z]);
         }

         /* We have to delete any windows created so far. */
         for (z=1; z <= vcols; z++)
         {
            deleteCursesWindow (matrix->cell[0][z]);
         }
         for (z=1; z < x ; z++)
         {
            deleteCursesWindow (matrix->cell[z][0]);
         }
         free (matrix);

         /* Return a NULL pointer. */
         return ((CDKMATRIX *)NULL);
      }

      /* Set the start of the x position. */
      begx = xpos + matrix->maxrt + 1;

      /* Make the cells */
      for (y=1; y <= vcols; y++)
      {
         cellWidth = colwidths[y] + 3;
         matrix->cell[x][y] = subwin (matrix->win, 3, cellWidth, begy, begx);

         /* Is the cell NULL??? */
         if (matrix->cell[x][y] == (WINDOW *)NULL)
         {
            /* Free up any used memory. */
            for (y=1; y <= rows; y++)
            {
               freeChtype (matrix->rowtitle[y]);
            }

            /* We have to delete any windows created so far. */
            for (y=1; y <= vcols; y++)
            {
               deleteCursesWindow (matrix->cell[0][y]);
            }
            for (y=1; y < vrows ; y++)
            {
               deleteCursesWindow (matrix->cell[y][0]);
            }
            for (w=1; w <= vrows; w++)
            {
               for (z=1; z <= y; z++)
               {
                  deleteCursesWindow (matrix->cell[w][z]);
               }
            }
            free (matrix);

            /* Return a NULL pointer. */
            return ((CDKMATRIX *)NULL);
         }
         begx += cellWidth + colSpace - 1;
         keypad (matrix->cell[x][y], TRUE);
      }
      begy += rowSpace + 2;
   }
   keypad (matrix->win, TRUE);

   /* Copy the titles into the structure. */
   for (x=1; x <= cols; x++)
   {
      matrix->coltitle[x]	= char2Chtype (coltitles[x], &matrix->coltitleLen[x], &matrix->coltitlePos[x]);
      matrix->coltitlePos[x]	= justifyString (colwidths[x], matrix->coltitleLen[x], matrix->coltitlePos[x]);
      matrix->colwidths[x]	= colwidths[x];
   }

   /* Make room for the cell information. */
   for (x=1; x <= rows; x++)
   {
      for (y=1; y <= cols; y++)
      {
         matrix->info[x][y]	= (char *)malloc (sizeof (char) * 256);
         matrix->colvalues[y]	= colvalues[y];
         matrix->colwidths[y]	= colwidths[y];
         cleanChar (matrix->info[x][y], colwidths[y]+1, '\0');
      }
   }

   /* Keep the rest of the info. */
   matrix->parent		= cdkscreen->window;
   matrix->rows			= rows;
   matrix->cols			= cols;
   matrix->vrows		= vrows;
   matrix->vcols		= vcols;
   matrix->boxWidth		= boxWidth;
   matrix->boxHeight		= boxHeight;
   matrix->rowSpace		= rowSpace;
   matrix->colSpace		= colSpace;
   matrix->filler		= filler;
   matrix->dominant		= dominant;
   matrix->row			= 1;
   matrix->col			= 1;
   matrix->crow			= 1;
   matrix->ccol			= 1;
   matrix->trow			= 1;
   matrix->lcol			= 1;
   matrix->oldcrow		= 1;
   matrix->oldccol		= 1;
   matrix->oldvrow		= 1;
   matrix->oldvcol		= 1;
   matrix->exitType		= vNEVER_ACTIVATED;
   matrix->boxMatrix		= boxMatrix;
   matrix->boxCell		= boxCell;
   matrix->shadow		= shadow;
   matrix->highlight		= A_REVERSE;
   matrix->ULChar		= ACS_ULCORNER;
   matrix->URChar		= ACS_URCORNER;
   matrix->LLChar		= ACS_LLCORNER;
   matrix->LRChar		= ACS_LRCORNER;
   matrix->HChar		= ACS_HLINE;
   matrix->VChar		= ACS_VLINE;
   matrix->BoxAttrib		= A_NORMAL;
   matrix->callbackfn		= (void *)&CDKMatrixCallBack;
   matrix->preProcessFunction	= (PROCESSFN)NULL;
   matrix->preProcessData	= (void *)NULL;
   matrix->postProcessFunction	= (PROCESSFN)NULL;
   matrix->postProcessData	= (void *)NULL;
   matrix->shadowWin		= (WINDOW *)NULL;

   /* Do we want a shadow??? */
   if (shadow)
   {
      matrix->shadowWin = newwin (boxHeight, boxWidth, ypos+1, xpos+1);
   }

   /* Clean the key bindings. */
   cleanCDKObjectBindings (vMATRIX, matrix);

   /* Register this baby. */
   registerCDKObject (cdkscreen, vMATRIX, matrix);

   /* Return the matrix pointer */
   return (matrix);
}

/* 
 * This activates the matrix.
 */
int activateCDKMatrix (CDKMATRIX *matrix, chtype *actions)
{
   /* Declare local variables. */
   int ret;

   /* Draw the matrix */
   drawCDKMatrix (matrix, matrix->boxMatrix);
   
   /* Check if actions is NULL. */
   if (actions == (chtype *)NULL)
   {
      chtype input = (chtype)NULL;
      for (;;)
      {
         /* Get the input. */
         input = wgetch (matrix->cell[matrix->crow][matrix->ccol]);

         /* Inject the character into the widget. */
         ret = injectCDKMatrix (matrix, input);
         if (matrix->exitType != vEARLY_EXIT)
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
         ret = injectCDKMatrix (matrix, actions[x]);
         if (matrix->exitType != vEARLY_EXIT)
         {
            return ret;
         }
      }
   }
   
   /* Set the exit type and exit. */
   matrix->exitType = vEARLY_EXIT;
   return -1;
}

/*
 * This injects a single character into the matrix widget.
 */
int injectCDKMatrix (CDKMATRIX *matrix, chtype input)
{
   /* Declare local variables. */
   int refreshCells	= FALSE;
   int movedCell	= FALSE;
   int charcount	= (int)strlen (matrix->info[matrix->row][matrix->col]);
   int ppReturn		= 1;
   int x, y, infolen;

   /* Set the exit type. */
   matrix->exitType = vEARLY_EXIT;

   /* Move the cursor to the correct position within the cell. */
   if (matrix->colwidths[matrix->ccol] == 1)
   {
      wmove (matrix->cell[matrix->crow][matrix->ccol], 1, 1);
   }
   else
   {
      wmove (matrix->cell[matrix->crow][matrix->ccol], 1, 
		(int)strlen (matrix->info[matrix->row][matrix->col])+1);
   }
   wrefresh (matrix->cell[matrix->crow][matrix->ccol]);

   /* Put the focus on the current cell */
   attrbox (matrix->cell[matrix->crow][matrix->ccol],
   		ACS_ULCORNER,	ACS_URCORNER,
   		ACS_LLCORNER,	ACS_LRCORNER,
   		ACS_HLINE,	ACS_VLINE,
   		A_BOLD);
   wrefresh (matrix->cell[matrix->crow][matrix->ccol]);
   highlightCDKMatrixCell (matrix);

   /* Check if there is a pre-process function to be called. */
   if (matrix->preProcessFunction != (PROCESSFN)NULL)
   {
      /* Call the pre-process function. */
      ppReturn = ((PROCESSFN)(matrix->preProcessFunction)) (vMATRIX, matrix, matrix->preProcessData, input);
   }

   /* Should we continue? */
   if (ppReturn != 0)
   {
      /* Check the key bindings. */
      if (checkCDKObjectBind (vMATRIX, matrix, input) != 0)
      {
         matrix->exitType = vESCAPE_HIT;
         return -1;
      }
      else
      {
         switch (input)
         {
            case CDK_TRANSPOSE :
                 break;

            case CDK_BEGOFLINE :
                 break;

            case CDK_ENDOFLINE :
                 break;

            case KEY_BACKSPACE : case DELETE : case '' : case KEY_DC :
                 if (matrix->colvalues[matrix->col] == vVIEWONLY)
                 {
                   Beep();
                 }
                 else
                 {
                    if (charcount > 0)
                    {
                       charcount--;
                       mvwdelch (matrix->cell[matrix->crow][matrix->ccol], 1, charcount+1);
                       mvwinsch (matrix->cell[matrix->crow][matrix->ccol], 1, charcount+1, matrix->filler);
                       wrefresh (matrix->cell[matrix->crow][matrix->ccol]);
                       matrix->info[matrix->row][matrix->col][charcount] = '\0';
                    }
                    else
                    {
                       Beep();
                    }
                 }
                 break;
   
            case KEY_RIGHT : case KEY_TAB :
                 if (matrix->ccol != matrix->vcols)
                 {
                    /* We are moving to the right... */
                    matrix->col++;
                    matrix->ccol++;
                    movedCell = TRUE;
                 }
                 else
                 {
                    /* We have to shift the columns to the right. */
                    if (matrix->col != matrix->cols)
                    {
                       matrix->lcol++;
                       matrix->col++;
   
                       /* Redraw the column titles. */
                       if (matrix->rows > matrix->vrows)
                       {
                          redrawTitles (matrix, FALSE, TRUE);
                       }
                       refreshCells = TRUE;
                       movedCell = TRUE;
                    }
                    else
                    {
                       /* We are at the far right column, we need  */
                       /* shift down one row, if we can. */
                       if (matrix->row == matrix->rows)
                       {
                          Beep();
                       }
                       else
                       {
                          /* Set up the columns info. */
                          matrix->col	= 1;
                          matrix->lcol	= 1;
                          matrix->ccol	= 1;
   
                          /* Shift the rows... */
                          if (matrix->crow != matrix->vrows)
                          {
                             matrix->row++;
                             matrix->crow++;
                          }
                          else
                          {
                             matrix->row++;
                             matrix->trow++;
                          }
                          redrawTitles (matrix, TRUE, TRUE);
                          refreshCells	= TRUE;
                          movedCell	= TRUE;
                       }
                    }
                 }
                 break;
   
            case KEY_LEFT :
                 if (matrix->ccol != 1)
                 {
                    /* We are moving to the left... */
                    matrix->col--;
                    matrix->ccol--;
                    movedCell = TRUE;
                 }
                 else
                 {
                    /* Are we at the far left??? */
                    if (matrix->lcol != 1)
                    {
                       matrix->lcol--;
                       matrix->col--;
   
                       /* Redraw the column titles. */
                       if (matrix->cols > matrix->vcols)
                       {
                          redrawTitles (matrix, FALSE, TRUE);
                       }
                       refreshCells = TRUE;
                       movedCell = TRUE;
                    }
                    else
                    {
                       /* Shift up one line if we can... */
                       if (matrix->row == 1)
                       {
                          Beep();
                       }
                       else
                       {
                          /* Set up the columns info. */
                          matrix->col	= matrix->cols;
                          matrix->lcol	= matrix->cols - matrix->vcols + 1;
                          matrix->ccol	= matrix->vcols;
   
                          /* Shift the rows... */
                          if (matrix->crow != 1)
                          {
                             matrix->row--;
                             matrix->crow--;
                          }
                          else
                          {
                             matrix->row--;
                             matrix->trow--;
                          }
                          redrawTitles (matrix, TRUE, TRUE);
                          refreshCells	= TRUE;
                          movedCell	= TRUE;
                       }
                    }
                 }
                 break;
      
            case KEY_UP :
                 if (matrix->crow != 1)
                 {
                    matrix->row--;
                    matrix->crow--;
                    movedCell	= TRUE;
                 }
                 else
                 {
                    if (matrix->trow != 1)
                    {
                       matrix->trow--;
                       matrix->row--;
                       
                       /* Redraw the row titles. */
                       if (matrix->rows > matrix->vrows)
                       {
                          redrawTitles (matrix, TRUE, FALSE);
                       }
                       refreshCells	= TRUE;
                       movedCell	= TRUE;
                    }
                    else
                    {
                       Beep();
                    }
                 }
                 break;
   
            case KEY_DOWN :
                 if (matrix->crow != matrix->vrows)
                 {
                    matrix->row ++;
                    matrix->crow ++;
                    movedCell	= TRUE;
                 }
                 else
                 {
                    if ((matrix->trow + matrix->vrows - 1) != matrix->rows)
                    {
                       matrix->trow++;
                       matrix->row++;
                       
                       /* Redraw the titles. */
                       if (matrix->rows > matrix->vrows)
                       {
                          redrawTitles (matrix, TRUE, FALSE);
                       }
                       refreshCells	= TRUE;
                       movedCell	= TRUE;
                    }
                    else
                    {
                       Beep();
                    }
                 }
                 break;
   
            case KEY_NPAGE : case '' :
                 if (matrix->rows > matrix->vrows)
                 {
                    if ((matrix->trow + ((matrix->vrows - 1) * 2)) <= matrix->rows)
                    {
                       matrix->trow	+= matrix->vrows - 1;
                       matrix->row	+= matrix->vrows - 1;
                       redrawTitles (matrix, TRUE, FALSE);
                       refreshCells	= TRUE;
                       movedCell	= TRUE;
                    }
                    else
                    {
                       Beep();
                    }
                 }
                 else
                 {
                    Beep();
                 }
                 break;
   
            case KEY_PPAGE : case '' :
                 if (matrix->rows > matrix->vrows)
                 {
                    if ((matrix->trow - ((matrix->vrows - 1) * 2)) >= 1)
                    {
                       matrix->trow	-= matrix->vrows - 1;
                       matrix->row	-= matrix->vrows - 1;
                       redrawTitles (matrix, TRUE, FALSE);
                       refreshCells	= TRUE;
                       movedCell	= TRUE;
                    }
                    else
                    {
                       Beep();
                    }
                 }
                 else
                 {
                    Beep();
                 }
                 break;
   
   	 case '' :
              jumpToCell (matrix, -1, -1);
              drawCDKMatrix (matrix, matrix->boxMatrix);
              break;
   
   	 case CDK_PASTE :
              if (GPasteBuffer == (char *)NULL || (int)strlen (GPasteBuffer) > matrix->colwidths[matrix->ccol])
              {
                 Beep();
              }
              else
              {
                 strcpy (matrix->info[matrix->trow+matrix->crow-1][matrix->lcol+matrix->ccol-1], GPasteBuffer);
                 drawCDKMatrixCell (matrix, matrix->crow, matrix->ccol, matrix->row, matrix->col, A_NORMAL, matrix->boxCell);
              }
              break;
   
   	 case CDK_COPY :
              freeChar (GPasteBuffer);
              GPasteBuffer	= copyChar (matrix->info[matrix->trow+matrix->crow-1][matrix->lcol+matrix->ccol-1]);
              break;
   
   	 case CDK_CUT :
              freeChar (GPasteBuffer);
              GPasteBuffer	= copyChar (matrix->info[matrix->trow+matrix->crow-1][matrix->lcol+matrix->ccol-1]);
              cleanChar (matrix->info[matrix->trow+matrix->crow-1][matrix->lcol+matrix->ccol-1], matrix->colwidths[matrix->lcol+matrix->ccol-1], '\0');
              drawCDKMatrixCell (matrix, matrix->crow, matrix->ccol, matrix->row, matrix->col, A_NORMAL, matrix->boxCell);
              break;
   
   	 case CDK_ERASE :
              cleanChar (matrix->info[matrix->trow+matrix->crow-1][matrix->lcol+matrix->ccol-1], matrix->colwidths[matrix->lcol+matrix->ccol-1], '\0');
              drawCDKMatrixCell (matrix, matrix->crow, matrix->ccol, matrix->row, matrix->col, A_NORMAL, matrix->boxCell);
              break;
   
   	 case KEY_RETURN : case KEY_ENTER :
              if (!matrix->boxCell)
              {
                 attrbox (matrix->cell[matrix->oldcrow][matrix->oldccol],
              			' ',	' ',
              			' ',	' ',
              			' ',	' ',
              			A_NORMAL);
              }
              else
              {
                 drawCDKMatrixCell (matrix,
              			matrix->oldcrow,
              			matrix->oldccol,
              			matrix->oldvrow,
              			matrix->oldvcol,
              			A_NORMAL,
              			matrix->boxCell);
              }
              wrefresh (matrix->cell[matrix->crow][matrix->ccol]);
              matrix->exitType = vNORMAL;
              return 0;
   	   break;
   
   	 case KEY_ESC :
              if (!matrix->boxCell)
              {
                 attrbox (matrix->cell[matrix->oldcrow][matrix->oldccol],
              			' ',	' ',
              			' ',	' ',
              			' ',	' ',
              			A_NORMAL);
              }
              else
              {
                 drawCDKMatrixCell (matrix,
              			matrix->oldcrow,
              			matrix->oldccol,
              			matrix->oldvrow,
              			matrix->oldvcol,
              			A_NORMAL,
              			matrix->boxCell);
              }
              wrefresh (matrix->cell[matrix->crow][matrix->ccol]);
              matrix->exitType = vESCAPE_HIT;
              return -1;
   	   break;
   
         case CDK_REFRESH :
              eraseCDKScreen (matrix->screen);
              refreshCDKScreen (matrix->screen);
              break;
   
         default :
              ((MATRIXCB)matrix->callbackfn)(matrix, input);
              break;
         }
      }
   
      /* Did we change cells? */
      if (movedCell)
      {
         /* un-highlight the old box  */
         if (!matrix->boxCell)
         {
            attrbox (matrix->cell[matrix->oldcrow][matrix->oldccol],
   			' ',	' ',
   			' ',	' ',
   			' ',	' ',
   			A_NORMAL);
         }
         else
         {
            drawCDKMatrixCell (matrix,
   				matrix->oldcrow,
   				matrix->oldccol,
   				matrix->oldvrow,
   				matrix->oldvcol,
   				(chtype)NULL,
   				matrix->boxCell);
         }
         wrefresh (matrix->cell[matrix->oldcrow][matrix->oldccol]);
   
         /* Highlight the new cell. */
         attrbox (matrix->cell[matrix->crow][matrix->ccol],
   			ACS_ULCORNER,	ACS_URCORNER,
   			ACS_LLCORNER,	ACS_LRCORNER,
   			ACS_HLINE,	ACS_VLINE,
   			A_BOLD);
         wrefresh (matrix->cell[matrix->crow][matrix->ccol]);
         highlightCDKMatrixCell (matrix);
      }
   
      /* Redraw each cell. */
      if (refreshCells)
      {
         /* Fill in the cells. */
         for (x=1; x <= matrix->vrows; x++)
         {
            for (y=1; y <= matrix->vcols; y++)
            {
               drawCDKMatrixCell (matrix, x, y,
   					matrix->trow+x-1,
   					matrix->lcol+y-1,
   					(chtype)NULL,
   					matrix->boxCell);
            }
         }

         /* Highlight the current cell. */
         attrbox (matrix->cell[matrix->crow][matrix->ccol],
   			ACS_ULCORNER,	ACS_URCORNER,
   			ACS_LLCORNER,	ACS_LRCORNER,
   			ACS_HLINE,	ACS_VLINE,
   			A_BOLD);
         wrefresh (matrix->cell[matrix->crow][matrix->ccol]);
         highlightCDKMatrixCell (matrix);
      }
   
      /* Move to the correct position in the cell. */
      if (refreshCells || movedCell)
      {
         if (matrix->colwidths[matrix->ccol] == 1)
         {
            wmove (matrix->cell[matrix->crow][matrix->ccol], 1, 1);
         }
         else
         {
            infolen = (int)strlen (matrix->info[matrix->trow+matrix->crow-1][matrix->lcol+matrix->ccol-1]);
            wmove (matrix->cell[matrix->crow][matrix->ccol], 1, infolen+1);
         }
         wrefresh (matrix->cell[matrix->crow][matrix->ccol]);
      }

      /* Should we call a post-process? */
      if (matrix->postProcessFunction != (PROCESSFN)NULL)
      {
         ((PROCESSFN)(matrix->postProcessFunction)) (vMATRIX, matrix, matrix->postProcessData, input);
      }
   }

   /* Set the variables we need. */
   matrix->oldcrow	= matrix->crow;
   matrix->oldccol	= matrix->ccol;
   matrix->oldvrow	= matrix->row;
   matrix->oldvcol	= matrix->col;

   /* Set the exit type and exit. */
   matrix->exitType = vEARLY_EXIT;
   return -1;
}

/* 
 * This allows the programmer to define their own key mappings.	
 */
void CDKMatrixCallBack (CDKMATRIX *matrix, chtype input)
{
   /* Declare local variables. */
   EDisplayType disptype	= (EDisplayType)matrix->colvalues[matrix->col];
   int charcount		= (int)strlen (matrix->info[matrix->row][matrix->col]);
   chtype newchar		= (chtype)NULL;

   /* Check the types */
   if (disptype == vINT && !isdigit((int)input))
   {
      Beep();
   }
   else if ((disptype == vCHAR || disptype == vUCHAR || \
		disptype == vLCHAR || disptype == vUHCHAR || \
		disptype == vLHCHAR) && isdigit((int)input))
   {
      Beep();
   }
   else if (disptype == vVIEWONLY)
   {
      Beep();
   }
   else 
   {
      /* Check the width of the string. */
      if (charcount == matrix->colwidths[matrix->col])
      {
         Beep();
      }
      else
      {
         /* We will make any needed adjustments to the case of the character. */
         newchar = input;
         if ((disptype == vUCHAR || disptype == vUCHAR || \
		disptype == vUMIXED || disptype == vUHMIXED) \
		&& !isdigit((int)input))
         {
            newchar = toupper(input);
         }
         else if ((disptype == vUCHAR || disptype == vUCHAR || \
			disptype == vUMIXED || disptype == vUHMIXED) && \
			!isdigit((int)input))
         {
            newchar = tolower(input);
         }

         /* Update the screen. */
         wmove (matrix->cell[matrix->crow][matrix->ccol], 1, (int)strlen (matrix->info[matrix->row][matrix->col])+1);
         waddch (matrix->cell[matrix->crow][matrix->ccol], newchar);
         wrefresh (matrix->cell[matrix->crow][matrix->ccol]);

         /* Update the character pointer. */
         matrix->info[matrix->row][matrix->col][charcount++] = newchar;
         matrix->info[matrix->row][matrix->col][charcount] = '\0';
      }
   }
}

/*
 * Highlight the new field.	
 */
void highlightCDKMatrixCell (CDKMATRIX *matrix)
{
   /* Declare local variables. */
   chtype highlight	= matrix->highlight;
   int x		= 0;
   int infolen		= (int)strlen (matrix->info[matrix->row][matrix->col]);

  /*
   * Given the dominance of the colors/attributes, we need to set the
   * current cell attribute.
   */
   if (matrix->dominant == ROW)
   {
      highlight	= matrix->rowtitle[matrix->crow][0] & A_ATTRIBUTES;
   }
   else if (matrix->dominant == COL)
   {
      highlight	= matrix->coltitle[matrix->ccol][0] & A_ATTRIBUTES;
   }

   /* If the column is only one char. */
   for (x=1; x <= matrix->colwidths[matrix->ccol]; x++)
   {
      if (x <= infolen)
      {
        mvwaddch (matrix->cell[matrix->crow][matrix->ccol], 1, x,
			matrix->info[matrix->row][matrix->col][x-1]|highlight);
      }
      else
      {
        mvwaddch (matrix->cell[matrix->crow][matrix->ccol], 1, x,
			matrix->filler | highlight);
      }
   }
   wmove (matrix->cell[matrix->crow][matrix->ccol], 1, infolen+1);
   wrefresh (matrix->cell[matrix->crow][matrix->ccol]);
}

/*
 * This moves the matrix field to the given location.
 */
void moveCDKMatrix (CDKMATRIX *matrix, int xplace, int yplace, boolean relative, boolean refresh)
{
   /* Declare local variables. */
   int currentX = matrix->win->_begx;
   int currentY = matrix->win->_begy;
   int xpos	= xplace;
   int ypos	= yplace;
   int xdiff	= 0;
   int ydiff	= 0;
   int x, y;

   /*
    * If this is a relative move, then we will adjust where we want
    * to move to.
     */
   if (relative)
   {
      xpos = matrix->win->_begx + xplace;
      ypos = matrix->win->_begy + yplace;
   }

   /* Adjust the window if we need to. */
   alignxy (matrix->screen->window, &xpos, &ypos, matrix->boxWidth, matrix->boxHeight);

   /* Get the difference. */
   xdiff = currentX - xpos;
   ydiff = currentY - ypos;

   /* Move the window to the new location. */
   matrix->win->_begx = xpos;
   matrix->win->_begy = ypos;

   for (x=0; x <= matrix->vrows; x++)
   {
      for (y=0; y <= matrix->vcols; y++)
      {
         matrix->cell[x][y]->_begx -= xdiff;
         matrix->cell[x][y]->_begy -= ydiff;
      }
   }

   /* If there is a shadow box we have to move it too. */
   if (matrix->shadowWin != (WINDOW *)NULL)
   {
      matrix->shadowWin->_begx -= xdiff;
      matrix->shadowWin->_begy -= ydiff;
   }

   /* Touch the windows so they 'move'. */
   touchwin (matrix->screen->window);
   wrefresh (matrix->screen->window);

   /* Redraw the window, if they asked for it. */
   if (refresh)
   {
      drawCDKMatrix (matrix, matrix->boxMatrix);
   }
}

/*
 * This allows the user to use the cursor keys to adjust the
 * position of the widget.
 */
void positionCDKMatrix (CDKMATRIX *matrix)
{
   /* Declare some variables. */
   int origX	= matrix->win->_begx;
   int origY	= matrix->win->_begy;
   chtype key	= (chtype)NULL;

   /* Let them move the widget around until they hit return. */
   while ((key != KEY_RETURN) && (key != KEY_ENTER))
   {
      key = wgetch (matrix->win);
      if (key == KEY_UP || key == '8')
      {
         if (matrix->win->_begy > 0)
         {
            moveCDKMatrix (matrix, 0, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_DOWN || key == '2')
      {
         if (matrix->win->_begy+matrix->win->_maxy < matrix->screen->window->_maxy-1)
         {
            moveCDKMatrix (matrix, 0, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_LEFT || key == '4')
      {
         if (matrix->win->_begx > 0)
         {
            moveCDKMatrix (matrix, -1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == KEY_RIGHT || key == '6')
      {
         if (matrix->win->_begx+matrix->win->_maxx < matrix->screen->window->_maxx-1)
         {
            moveCDKMatrix (matrix, 1, 0, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '7')
      {
         if (matrix->win->_begy > 0 && matrix->win->_begx > 0)
         {
            moveCDKMatrix (matrix, -1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '9')
      {
         if (matrix->win->_begx+matrix->win->_maxx < matrix->screen->window->_maxx-1 &&
		matrix->win->_begy > 0)
         {
            moveCDKMatrix (matrix, 1, -1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '1')
      {
         if (matrix->win->_begx > 0 && matrix->win->_begx+matrix->win->_maxx < matrix->screen->window->_maxx-1)
         {
            moveCDKMatrix (matrix, -1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '3')
      {
         if (matrix->win->_begx+matrix->win->_maxx < matrix->screen->window->_maxx-1 &&
		matrix->win->_begy+matrix->win->_maxy < matrix->screen->window->_maxy-1)
         {
            moveCDKMatrix (matrix, 1, 1, TRUE, TRUE);
         }
         else
         {
            Beep();
         }
      }
      else if (key == '5')
      {
         moveCDKMatrix (matrix, CENTER, CENTER, FALSE, TRUE);
      }
      else if (key == 't')
      {
         moveCDKMatrix (matrix, matrix->win->_begx, TOP, FALSE, TRUE);
      }
      else if (key == 'b')
      {
         moveCDKMatrix (matrix, matrix->win->_begx, BOTTOM, FALSE, TRUE);
      }
      else if (key == 'l')
      {
         moveCDKMatrix (matrix, LEFT, matrix->win->_begy, FALSE, TRUE);
      }
      else if (key == 'r')
      {
         moveCDKMatrix (matrix, RIGHT, matrix->win->_begy, FALSE, TRUE);
      }
      else if (key == 'c')
      {
         moveCDKMatrix (matrix, CENTER, matrix->win->_begy, FALSE, TRUE);
      }
      else if (key == 'C')
      {
         moveCDKMatrix (matrix, matrix->win->_begx, CENTER, FALSE, TRUE);
      }
      else if (key == CDK_REFRESH)
      {
         eraseCDKScreen (matrix->screen);
         refreshCDKScreen (matrix->screen);
      }
      else if (key == KEY_ESC)
      {
         moveCDKMatrix (matrix, origX, origY, FALSE, TRUE);
      }
      else if ((key != KEY_RETURN) && (key != KEY_ENTER))
      {
         Beep();
      }
   }
}

/*
 * This draws a cell within a matrix.
 */
void drawCDKMatrixCell (CDKMATRIX *matrix, int row, int col, int vrow, int vcol, chtype attr, boolean Box)
{
   /* Declare local variables. */
   WINDOW *cell		= matrix->cell[row][col];
   chtype highlight	= matrix->filler & A_ATTRIBUTES;
   int rows		= matrix->vrows;
   int cols		= matrix->vcols;
   int infolen		= (int)strlen (matrix->info[vrow][vcol]);
   int x;

   /*
    * Given the dominance of the colors/attributes, we need to set the
    * current cell attribute.
    */
   if (matrix->dominant == ROW)
   {
      highlight	= matrix->rowtitle[row][0] & A_ATTRIBUTES;
   }
   else if (matrix->dominant == COL)
   {
      highlight	= matrix->coltitle[col][0] & A_ATTRIBUTES;
   }

   /* Draw in the cell info. */
   for (x=1; x <= matrix->colwidths[col]; x++)
   {
      if (x <= infolen)
      {
         mvwaddch (cell, 1, x, matrix->info[vrow][vcol][x-1]|highlight);
      }
      else
      {
         mvwaddch (cell, 1, x, matrix->filler|highlight);
      }
   }
   wmove (cell, 1, infolen+1);
   wrefresh (cell);

   /* Only draw the box iff the user asked for a box. */
   if (!Box)
   {
      return;
   }

   /*
    * If the value of the column spacing is greater than 0 then these
    * are independent boxes.
    */
   if (matrix->colSpace != 0 && matrix->rowSpace != 0)
   {
      attrbox (cell, ACS_ULCORNER, ACS_URCORNER,
      		ACS_LLCORNER, ACS_LRCORNER,
      		ACS_HLINE, ACS_VLINE,
		attr);
      return;
   }
   if (matrix->colSpace != 0 && matrix->rowSpace == 0)
   {
      if (row == 1)
      {
         attrbox (cell,	ACS_ULCORNER, ACS_URCORNER,
      			ACS_LTEE, ACS_RTEE,
      			ACS_HLINE, ACS_VLINE,
			attr);
         return;
      }
      else if (row > 1 && row < rows)
      {
         attrbox (cell,	ACS_LTEE, ACS_RTEE,
      			ACS_LTEE, ACS_RTEE,
      			ACS_HLINE, ACS_VLINE,
			attr);
         return;
      }
      else if (row == rows)
      {
         attrbox (cell,	ACS_LTEE, ACS_RTEE,
      			ACS_LLCORNER, ACS_LRCORNER,
      			ACS_HLINE, ACS_VLINE,
			attr);
         return;
      }
   }
   if (matrix->colSpace == 0 && matrix->rowSpace != 0)
   {
      if (col == 1)
      {
         attrbox (cell,	ACS_ULCORNER, ACS_TTEE,
      			ACS_LLCORNER, ACS_BTEE,
      			ACS_HLINE, ACS_VLINE,
			attr);
         return;
      }
      else if (col > 1 && col < cols)
      {
         attrbox (cell,	ACS_TTEE, ACS_TTEE,
      			ACS_BTEE, ACS_BTEE,
      			ACS_HLINE, ACS_VLINE,
			attr);
         return;
      }
      else if (col == cols)
      {
         attrbox (cell,	ACS_TTEE, ACS_URCORNER,
      			ACS_BTEE, ACS_LRCORNER,
      			ACS_HLINE, ACS_VLINE,
			attr);
         return;
      }
   }

   /* Start drawing the matrix. */
   if (row == 1)
   {
      if (col == 1)
      {
         /* Draw the top left corner */
         attrbox (cell,	ACS_ULCORNER, ACS_TTEE,
			ACS_LTEE, ACS_PLUS,
      			ACS_HLINE, ACS_VLINE,
			attr);
      }
      else if (col > 1 && col < cols)
      {
         /* Draw the top middle box */
         attrbox (cell,	ACS_TTEE, ACS_TTEE,
      			ACS_PLUS, ACS_PLUS,
      			ACS_HLINE, ACS_VLINE,
			attr);
      }
      else if (col == cols)
      {
         /* Draw the top right corner */
         attrbox (cell,	ACS_TTEE, ACS_URCORNER,
      			ACS_PLUS, ACS_LTEE,
      			ACS_HLINE, ACS_VLINE,
			attr);
      }
   }
   else if (row > 1 && row < rows)
   {
      if (col == 1)
      {
         /* Draw the middle left box */
         attrbox (cell,	ACS_LTEE, ACS_PLUS,
     			ACS_LTEE, ACS_PLUS,
     			ACS_HLINE, ACS_VLINE,
			attr);
      }
      else if (col > 1 && col < cols)
      {
         /* Draw the middle box */
         attrbox (cell,	ACS_PLUS, ACS_PLUS,
      			ACS_PLUS, ACS_PLUS,
      			ACS_HLINE, ACS_VLINE,
			attr);
      }
      else if (col == cols)
      {
         /* Draw the middle right box */
         attrbox (cell,	ACS_PLUS, ACS_RTEE,
      			ACS_PLUS, ACS_RTEE,
      			ACS_HLINE, ACS_VLINE,
			attr);
      }
   }
   else if (row == rows)
   {
      if (col == 1)
      {
         /* Draw the bottom left corner */
         attrbox (cell,	ACS_LTEE, ACS_PLUS,
      			ACS_LLCORNER, ACS_BTEE,
      			ACS_HLINE, ACS_VLINE,
			attr);
      }
      else if (col > 1 && col < cols)
      {
         /* Draw the bottom middle box */
         attrbox (cell,	ACS_PLUS, ACS_PLUS,
      			ACS_BTEE, ACS_BTEE,
      			ACS_HLINE, ACS_VLINE,
			attr);
      }
      else if (col == cols)
      {
         /* Draw the bottom right corner */
         attrbox (cell,	ACS_PLUS, ACS_RTEE,
      			ACS_BTEE, ACS_LRCORNER,
      			ACS_HLINE, ACS_VLINE,
			attr);
      }
   }

   /* Highlight the current cell. */
   attrbox (matrix->cell[matrix->crow][matrix->ccol],
		ACS_ULCORNER,	ACS_URCORNER,
   		ACS_LLCORNER,	ACS_LRCORNER,
   		ACS_HLINE,	ACS_VLINE,
   		A_BOLD);
   wrefresh (matrix->cell[matrix->crow][matrix->ccol]);
   highlightCDKMatrixCell (matrix);
}

/*
 * This function draws the matrix widget.
 */
void drawCDKMatrix (CDKMATRIX *matrix, boolean Box)
{
   /* Declare local variables. */
   int x, y;

   /* Did we ask for a shadow??? */
   if (matrix->shadowWin != (WINDOW *)NULL)
   {
      drawShadow (matrix->shadowWin);
   }

   /* Should we box the matrix??? */
   if (Box)
   {
      attrbox (matrix->win,
		matrix->ULChar, matrix->URChar,
		matrix->LLChar, matrix->LRChar,
		matrix->HChar,  matrix->VChar,
		matrix->BoxAttrib);
      wrefresh (matrix->win);
   }

   /* Draw in the title. */
   if (matrix->titleLines != 0)
   {
      for (x=0; x < matrix->titleLines; x++)
      {
         writeChtype (matrix->win,
			matrix->titlePos[x],
			x+1,
			matrix->title[x],
			HORIZONTAL, 0,
			matrix->titleLen[x]);
      }
   }
   touchwin (matrix->win);
   wrefresh (matrix->win);

   /* Draw in the column titles. */
   for (x=1; x <= matrix->vcols; x++)
   {
      writeChtype (matrix->cell[0][x],
			matrix->coltitlePos[matrix->lcol+x-1], 0,
			matrix->coltitle[matrix->lcol+x-1], 
			HORIZONTAL, 
			0, matrix->coltitleLen[matrix->lcol+x-1]);
      wrefresh (matrix->cell[0][x]);
   }

   /* Fill in the rest of the matrix. */
   for (x=1; x <= matrix->vrows; x++)
   {
      /* Draw in the row titles */
      writeChtype (matrix->cell[x][0],
			matrix->rowtitlePos[matrix->trow+x-1], 1,
			matrix->rowtitle[matrix->trow+x-1], 
			HORIZONTAL, 
			0, matrix->rowtitleLen[matrix->trow+x-1]);
      wrefresh (matrix->cell[x][0]);

      /* Draw in the cells.. */
      for (y=1; y <= matrix->vcols; y++)
      {
         drawCDKMatrixCell (matrix, x, y, 
				matrix->trow+x-1,
				matrix->lcol+y-1,
				A_NORMAL,
				matrix->boxCell);
      }
   }

   /* Highlight the current cell. */
   attrbox (matrix->cell[matrix->crow][matrix->ccol],
   		ACS_ULCORNER,	ACS_URCORNER,
   		ACS_LLCORNER,	ACS_LRCORNER,
   		ACS_HLINE,	ACS_VLINE,
   		A_BOLD);
   wrefresh (matrix->cell[matrix->crow][matrix->ccol]);
   highlightCDKMatrixCell (matrix);
}

/*
 * This function destroys the matrix widget.
 */
void destroyCDKMatrix (CDKMATRIX *matrix)
{
   /* Declare local variables. */
   int x = 0;
   int y = 0;

   /* Erase the object. */
   eraseCDKMatrix (matrix);

   /* Clear out the title. */
   if (matrix->titleLines != 0)
   {
      for (x=0; x < matrix->titleLines; x++)
      {
         freeChtype (matrix->title[x]);
      }
   }

   /* Clear out the col titles. */
   for (x=1; x <= matrix->cols; x++)
   {
      freeChtype (matrix->coltitle[x]);
   }

   /* Clear out the row titles. */
   for (x=1; x <= matrix->rows; x++)
   {
      freeChtype (matrix->rowtitle[x]);
   }

   /* Clear out the matrix cells. */
   for (x=1; x <= matrix->rows; x++)
   {
      for (y=1; y <= matrix->cols; y++)
      {
         freeChar (matrix->info[x][y]);
      }
   }
 
   /* Clear the matrix windows. */
   deleteCursesWindow (matrix->cell[0][0]);
   for (x=1; x <= matrix->vrows; x++)
   {
      deleteCursesWindow (matrix->cell[x][0]);
   }
   for (x=1; x <= matrix->vcols; x++)
   {
      deleteCursesWindow (matrix->cell[0][x]);
   }
   for (x=1; x <= matrix->vrows; x++)
   {
      for (y=1; y <= matrix->vcols; y++)
      {
         deleteCursesWindow (matrix->cell[x][y]);
      }
   }
   deleteCursesWindow (matrix->shadowWin);
   deleteCursesWindow (matrix->win);
   
   /* Unregister this object. */
   unregisterCDKObject (vMATRIX, matrix);

   /* Finish cleaning up. */
   free (matrix);
}

/*
 * This function erases the matrix widget from the screen.
 */
void eraseCDKMatrix (CDKMATRIX *matrix)
{
   /* Declare local variables. */
   int x	= 0;
   int y	= 0;

   /* Clear the matrix cells. */
   eraseCursesWindow (matrix->cell[0][0]);
   for (x=1; x <= matrix->vrows; x++)
   {
      eraseCursesWindow (matrix->cell[x][0]);
   }
   for (x=1; x <= matrix->vcols; x++)
   {
      eraseCursesWindow (matrix->cell[0][x]);
   }
   for (x=1; x <= matrix->vrows; x++)
   {
      for (y=1; y <= matrix->vcols; y++)
      {
         eraseCursesWindow (matrix->cell[x][y]);
      }
   }
   eraseCursesWindow (matrix->shadowWin);
   eraseCursesWindow (matrix->win);
}

/*
 * This function sets the values of the matrix widget.
 */
void setCDKMatrix (CDKMATRIX *matrix, char *info[MAX_MATRIX_ROWS][MAX_MATRIX_COLS], int rows, int *subSize)
{
   /* Declare local variables. */
   int x	= 0;
   int y	= 0;

   /* Clear out all the cells. */
   for (x=1; x <= matrix->rows; x++)
   {
      for (y=1; y <= matrix->cols; y++)
      {
         /* Clean the old info, and copy in the new. */
         if (matrix->info[x][y] != (char *)NULL)
         {
            /* Clean the cell information. */
            cleanChar (matrix->info[x][y], matrix->colwidths[y], '\0');
         }
      }
   }

   /* Copy in the new info. */
   for (x=1; x <= rows; x++)
   {
      for (y=1; y <= subSize[x]; y++)
      {
         /* Copy in the new information. */
         if (info[x][y] != (char *)NULL)
         {
            strncpy (matrix->info[x][y], info[x][y], matrix->colwidths[y]);
         }
      }
   }
}

/*
 * This cleans out the information cells in the matrix widget.
 */
void cleanCDKMatrix (CDKMATRIX *matrix)
{
   /* Declare local variables. */
   int x	= 0;
   int y	= 0;

   for (x=1; x <= matrix->rows; x++)
   {
      for (y=1; y <= matrix->cols; y++)
      {
         cleanChar (matrix->info[x][y], matrix->colwidths[y], '\0');
      }
   }
}

/*
 * This allows us to hyper-warp to a cell.
 */
int jumpToCell (CDKMATRIX *matrix, int row, int col)
{
   /* Declare local variables. */
   CDKSCALE *scale	= (CDKSCALE *)NULL;
   int newRow		= row;
   int newCol		= col;

  /*
   * Only create the row scale if needed.
   */
   if ((row == -1) || (row > matrix->rows))
   {
      /* Create the row scale widget. */
      scale = newCDKScale (matrix->screen, 
				CENTER, CENTER,
				"<C>Jump to which row.",
				"</5/B>Row: ", A_NORMAL, 5,
				1, 1, matrix->rows, 1, 1, TRUE, FALSE);

      /* Activate the scale and get the row. */
      newRow = activateCDKScale (scale, (chtype *)NULL);
      destroyCDKScale (scale);
   }

  /*
   * Only create the column scale if needed.
   */
   if ((col == -1) || (col > matrix->cols))
   {
      /* Create the column scale widget. */
      scale = newCDKScale (matrix->screen, 
				CENTER, CENTER,
				"<C>Jump to which column",
				"</5/B>Col: ", A_NORMAL, 5,
				1, 1, matrix->cols, 1, 1, TRUE, FALSE);

      /* Activate the scale and get the column. */
      newCol = activateCDKScale (scale, (chtype *)NULL);
      destroyCDKScale (scale);
   }

   /* Hyper-warp.... */
   if (newRow != matrix->row || newCol != matrix->col)
   {
      return (moveToCDKMatrixCell (matrix, newRow, newCol));
   }
   else
   {
      return 1;
   }
}

/*
 * This allows us to move to a given cell.
 */
int moveToCDKMatrixCell (CDKMATRIX *matrix, int newrow, int newcol)
{
   /* Declare local variables. */
   int rowShift	= newrow - matrix->row;
   int colShift	= newcol - matrix->col;

   /* Make sure we arent asking to move out of the matrix. */
   if (newrow > matrix->rows || newcol > matrix->cols || newrow <= 0 || newcol <= 0)
   {
      return 0;
   }
   
   /* Did we move up/down???? */
   if (rowShift > 0)
   {
      /* We are moving down. */
      if (matrix->vrows == matrix->cols)
      {
         matrix->trow	= 1;
         matrix->crow	= newrow;
         matrix->row	= newrow;
      }
      else
      {
         if ((rowShift + matrix->vrows) < matrix->rows)
         {
            /* Just shift down by rowShift... */
            matrix->trow	+= rowShift;
            matrix->crow	= 1;
            matrix->row		+= rowShift;
         }
         else
         {
            /* We need to munge with the values... */
            matrix->trow	= matrix->rows - matrix->vrows + 1;
            matrix->crow	= ((rowShift + matrix->vrows) - matrix->rows) + 1;
            matrix->row		= newrow;
         }
      }
   }
   else if (rowShift < 0)
   {
      /* We are moving up. */
      if (matrix->vrows == matrix->rows)
      {
         matrix->trow	= 1;
         matrix->row	= newrow;
         matrix->crow	= newrow;
      }
      else
      {
         if ((rowShift + matrix->vrows) > 1)
         {
            /* Just shift up by rowShift... */
            matrix->trow	+= rowShift;
            matrix->row		+= rowShift;
            matrix->crow	= 1;
         }
         else
         {
            /* We need to munge with the values... */
            matrix->trow	= 1;
            matrix->crow	= 1;
            matrix->row		= 1;
         }
      }
   }

   /* Did we move left/right ???? */
   if (colShift > 0)
   {
      /* We are moving right. */
      if (matrix->vcols == matrix->cols)
      {
         matrix->lcol	= 1;
         matrix->ccol	= newcol;
         matrix->col	= newcol;
      }
      else
      {
         if ((colShift + matrix->vcols) < matrix->cols)
         {
            matrix->lcol	+= colShift;
            matrix->ccol	= 1;
            matrix->col		+= colShift;
         }
         else
         {
            /* We need to munge with the values... */
            matrix->lcol	= matrix->cols - matrix->vcols + 1;
            matrix->ccol	= ((colShift + matrix->vcols) - matrix->cols) + 1;
            matrix->col		= newcol;
         }
      }
   }
   else if (colShift < 0)
   {
      /* We are moving left. */
      if (matrix->vcols == matrix->cols)
      {
            matrix->lcol	= 1;
            matrix->col		= newcol;
            matrix->ccol	= newcol;
      }
      else
      {
         if ((colShift + matrix->vcols) > 1)
         {
            /* Just shift left by colShift... */
            matrix->lcol	+= colShift;
            matrix->col		+= colShift;
            matrix->ccol	= 1;
         }
         else
         {
            matrix->lcol	= 1;
            matrix->col		= 1;
            matrix->ccol	= 1;
         }
      }
   }

   /* Keep the 'old' values around for redrawing sake. */
   matrix->oldcrow	= matrix->crow;
   matrix->oldccol	= matrix->ccol;
   matrix->oldvrow	= matrix->row;
   matrix->oldvcol	= matrix->col;

   /* Lets ... */
   return 1;
}

/*
 * This redraws the titles indicated...
 */
void redrawTitles (CDKMATRIX *matrix, int rowTitles, int colTitles)
{
   /* Declare local variables. */
   int x = 0;

   /* Redraw the row titles. */
   if (rowTitles)
   {
      for (x=1; x <= matrix->vrows; x++)
      {
         werase (matrix->cell[x][0]);
         writeChtype (matrix->cell[x][0],
			matrix->rowtitlePos[matrix->trow+x-1], 1,
			matrix->rowtitle[matrix->trow+x-1], 
			HORIZONTAL, 
			0, matrix->rowtitleLen[matrix->trow+x-1]);
         wrefresh (matrix->cell[x][0]);
      }
   }

   /* Redraw the column titles. */
   if (colTitles)
   {
      for (x=1; x <=matrix->vcols; x++)
      {
         werase (matrix->cell[0][x]);
         writeChtype (matrix->cell[0][x],
			matrix->coltitlePos[matrix->lcol+x-1], 0,
			matrix->coltitle[matrix->lcol+x-1], 
			HORIZONTAL, 
			0, matrix->coltitleLen[matrix->lcol+x-1]);
      			wrefresh (matrix->cell[0][x]);
      }
   }
}

/*
 * This sets the value of a matrix cell.
 */
int setCDKMatrixCell (CDKMATRIX *matrix, int row, int col, char *value)
{
   /* Make sure the row/col combination is within the matrix. */
   if (row > matrix->rows || col > matrix->cols || row <= 0 || col <= 0)
   {
      return -1;
   }
   
   /* Clean out the old value. */
   cleanChar (matrix->info[row][col], matrix->colwidths[col]+1, '\0');

   /* Copy the new version in and return. */
   strncpy (matrix->info[row][col], value, matrix->colwidths[col]);
   return 1;
}

/*
 * This gets the value of a matrix cell.
 */
char *getCDKMatrixCell (CDKMATRIX *matrix, int row, int col)
{
   /* Make sure the row/col combination is within the matrix. */
   if (row > matrix->rows || col > matrix->cols || row <= 0 || col <= 0)
   {
      return (char *)NULL;
   }
   return matrix->info[row][col];
}

/*
 * This returns the current row/col cell.
 */
int getCDKMatrixCol (CDKMATRIX *matrix)
{
   return matrix->col;
}
int getCDKMatrixRow (CDKMATRIX *matrix)
{
   return matrix->row;
}

/*
 * These functions set the drawing characters of the widget.
 */
void setCDKMatrixULChar (CDKMATRIX *matrix, chtype character)
{
   matrix->ULChar = character;
}
void setCDKMatrixURChar (CDKMATRIX *matrix, chtype character)
{
   matrix->URChar = character;
}
void setCDKMatrixLLChar (CDKMATRIX *matrix, chtype character)
{
   matrix->LLChar = character;
}
void setCDKMatrixLRChar (CDKMATRIX *matrix, chtype character)
{
   matrix->LRChar = character;
}
void setCDKMatrixVerticalChar (CDKMATRIX *matrix, chtype character)
{
   matrix->VChar = character;
}
void setCDKMatrixHorizontalChar (CDKMATRIX *matrix, chtype character)
{
   matrix->HChar = character;
}
void setCDKMatrixBoxAttribute (CDKMATRIX *matrix, chtype character)
{
   matrix->BoxAttrib = character;
}

/*
 * This sets the background color of the widget.
 */ 
void setCDKMatrixBackgroundColor (CDKMATRIX *matrix, char *color)
{
   chtype *holder = (chtype *)NULL;
   int x, y, junk1, junk2;

   /* Make sure the color isn't NULL. */
   if (color == (char *)NULL)
   {
      return;
   }

   /* Convert the value of the environment variable to a chtype. */
   holder = char2Chtype (color, &junk1, &junk2);

   /* Set the widgets background color. */
   wbkgd (matrix->win, holder[0]);
   for (x=0; x <= matrix->vrows; x++)
   {
      for (y=0; y <= matrix->vcols; y++)
      {
         wbkgd (matrix->cell[x][y], holder[0]);
      }
   }

   /* Clean up. */
   freeChtype (holder);
}

/*
 * This function sets the pre-process function.
 */
void setCDKMatrixPreProcess (CDKMATRIX *matrix, PROCESSFN callback, void *data)
{
   matrix->preProcessFunction = callback;
   matrix->preProcessData = data;
}
 
/*
 * This function sets the post-process function.
 */
void setCDKMatrixPostProcess (CDKMATRIX *matrix, PROCESSFN callback, void *data)
{
   matrix->postProcessFunction = callback;
   matrix->postProcessData = data;
}
