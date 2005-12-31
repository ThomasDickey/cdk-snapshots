/* $Id: graph_ex.c,v 1.12 2005/12/26 22:04:35 tom Exp $ */

#include <cdk_test.h>

#ifdef HAVE_XCURSES
char *XCursesProgramName = "graph_ex";
#endif

int main(int argc, char **argv)
{
   /* Declare vars. */
   CDKSCREEN *cdkscreen = 0;
   CDKGRAPH *graph	= 0;
   CDKLABEL *pausep	= 0;
   WINDOW *cursesWin	= 0;
   char *title		= 0;
   char *xtitle		= 0;
   char *ytitle		= 0;
   char *graphChars	= 0;
   char *mesg[2];
   int values[20], colors[20];
   int count;

   CDK_PARAMS params;

   CDKparseParams(argc, argv, &params, CDK_CLI_PARAMS); /* -N, -S unused */

   /* Set up CDK. */
   cursesWin = initscr();
   cdkscreen = initCDKScreen (cursesWin);

   /* Start CDK Colors. */
   initCDKColor();

   /* Create the graph values. */
   values[0]	= 10;	colors[0]	= 5;
   values[1]	= 15;	colors[1]	= 5;
   values[2]	= 20;	colors[2]	= 5;
   values[3]	= 25;	colors[3]	= 5;
   values[4]	= 30;	colors[4]	= 5;
   values[5]	= 35;	colors[5]	= 5;
   values[6]	= 40;	colors[6]	= 5;
   values[7]	= 45;	colors[7]	= 5;
   values[8]	= 50;	colors[8]	= 5;
   values[9]	= 55;	colors[9]	= 5;
   count	= 10;
   title	= "<C>Test Graph";
   xtitle	= "<C>X AXIS TITLE";
   ytitle	= "<C>Y AXIS TITLE";
   graphChars	= "0123456789";

   /* Create the label values. */
   mesg[0] = "Press any key when done viewing the graph.";

   /* Create the graph widget. */
   graph = newCDKGraph (cdkscreen,
			CDKparamValue(&params, 'X', CENTER),
			CDKparamValue(&params, 'Y', CENTER),
			CDKparamValue(&params, 'H', 10),
			CDKparamValue(&params, 'W', 20),
			title, xtitle, ytitle);

   /* Is the graph null? */
   if (graph == 0)
   {
      /* Shut down CDK. */
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a message and exit. */
      printf ("Oops. Can not make the graph widget. Is the window too small?\n");
      ExitProgram (EXIT_FAILURE);
   }

   /* Create the label widget. */
   pausep = newCDKLabel (cdkscreen, CENTER, BOTTOM, mesg, 1, TRUE, FALSE);
   if (pausep == 0)
   {
      /* Shut down CDK. */
      destroyCDKGraph (graph);
      destroyCDKScreen (cdkscreen);
      endCDK();

      /* Print out a message and exit. */
      printf ("Oops. Can not make the label widget. Is the window too small?\n");
      ExitProgram (EXIT_FAILURE);
   }

   /* Set the graph values. */
   setCDKGraph (graph, values, count, graphChars, FALSE, vPLOT);

   /* Draw the screen. */
   refreshCDKScreen (cdkscreen);
   drawCDKGraph (graph, FALSE);
   drawCDKLabel (pausep, TRUE);

   /* Pause until the user says so... */
   waitCDKLabel (pausep, 0);

   /* Clean up. */
   destroyCDKGraph (graph);
   destroyCDKLabel (pausep);
   destroyCDKScreen (cdkscreen);
   endCDK();
   ExitProgram (EXIT_SUCCESS);
}
