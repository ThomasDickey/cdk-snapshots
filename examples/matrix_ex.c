/* $Id: matrix_ex.c,v 1.14 2005/12/26 22:04:35 tom Exp $ */

#include <cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "matrix_ex";
#endif

/*
 * This program demonstrates the Cdk matrix widget.
 */
int main (int argc, char **argv)
{
   /* Declare local variables. */
   CDKSCREEN *cdkscreen		= 0;
   CDKMATRIX *courseList	= 0;
   WINDOW *cursesWin		= 0;
   char *title			= 0;
   int rows			= 8;
   int cols			= 5;
   int vrows			= 3;
   int vcols			= 5;
   char *coltitle[10], *rowtitle[10], *mesg[10];
   int colwidth[10], colvalue[10];

   CDK_PARAMS params;

   CDKparseParams (argc, argv, &params, CDK_MIN_PARAMS);

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start CDK Colors. */
   initCDKColor();

   /* Create the horizontal and vertical matrix labels. */
#define set_col(n, width, string) \
   coltitle[n] = string;   colwidth[n] = width ; colvalue[n] = vUMIXED

   set_col(1, 7, "</B/5>Course");
   set_col(2, 7, "</B/33>Lec 1");
   set_col(3, 7, "</B/33>Lec 2");
   set_col(4, 7, "</B/33>Lec 3");
   set_col(5, 1, "</B/7>Flag");

#define set_row(n, string) \
   rowtitle[n] = "<C></B/6>" string

   set_row(1, "Course 1");
   set_row(2, "Course 2");
   set_row(3, "Course 3");
   set_row(4, "Course 4");
   set_row(5, "Course 5");
   set_row(6, "Course 6");
   set_row(7, "Course 7");
   set_row(8, "Course 8");

   /* Create the title. */
   title = "<C>This is the CDK\n<C>matrix widget.\n<C><#LT><#HL(30)><#RT>";

   /* Create the matrix object. */
   courseList = newCDKMatrix (cdkscreen,
			      CDKparamValue(&params, 'X', CENTER),
			      CDKparamValue(&params, 'Y', CENTER),
			      rows, cols, vrows, vcols,
			      title, rowtitle, coltitle,
			      colwidth, colvalue,
			      -1, -1, '.',
			      COL, TRUE,
			      CDKparamValue(&params, 'N', TRUE),
			      CDKparamValue(&params, 'S', TRUE));

   /* Check to see if the matrix is null. */
   if (courseList == 0)
   {
      /* Clean up. */
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a little message. */
      printf ("Oops. Can't seem to create the matrix widget. Is the window too small ?\n");
      ExitProgram (EXIT_FAILURE);
   }

   /* Activate the matrix. */
   activateCDKMatrix (courseList, 0);

   /* Check if the user hit escape or not. */
   if (courseList->exitType == vESCAPE_HIT)
   {
      mesg[0] = "<C>You hit escape. No information passed back.";
      mesg[1] = "",
      mesg[2] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 3);
   }
   else if (courseList->exitType == vNORMAL)
   {
      char temp[80];

      sprintf(temp, "Current cell (%d,%d)", courseList->crow, courseList->ccol);
      mesg[0] = "<L>You exited the matrix normally.";
      mesg[1] = temp;
      mesg[2] = "<L>To get the contents of the matrix cell, you can";
      mesg[3] = "<L>use getCDKMatrixCell():";
      mesg[4] = getCDKMatrixCell(courseList, courseList->crow, courseList->ccol);
      mesg[5] = "";
      mesg[6] = "<C>Press any key to continue.";
      popupLabel (cdkscreen, mesg, 7);
   }

   /* Clean up. */
   destroyCDKMatrix (courseList);
   destroyCDKScreen (cdkscreen);
   endCDK();
   ExitProgram (EXIT_SUCCESS);
}
