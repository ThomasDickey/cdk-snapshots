#include <cdk.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int swapSignCB (EObjectType cdktype, void *object, void *clientData, chtype key);
CDKLABEL *GLabel	= (CDKLABEL *)NULL;
int GSign		= 1;

int main(void)
{
  CDKSCREEN    *screen	= (CDKSCREEN *)NULL;
  CDKENTRY     *entry	= (CDKENTRY *)NULL;
  CDKTEMPLATE  *temp	= (CDKTEMPLATE *)NULL;
  WINDOW       *window	= (WINDOW *)NULL;
  char         *plate	= "#####.##";
  char         *overlay	= "00000.00";
  static char  *bankName, *amount;
  float        number;
  char *mesg[1];
  char holder[100];

  window = initscr();
  screen = initCDKScreen(window);

  mesg[0] = "+";
  entry  = newCDKEntry(screen , 5, 5, NULL,"BANK   : ",
            A_NORMAL,' ',vCHAR,20,0,20,FALSE,FALSE);
  temp   = newCDKTemplate(screen , 5, 7, NULL,"AMOUNT : ",
            plate, overlay , FALSE, FALSE);
  bindCDKObject (vTEMPLATE, temp, '-', swapSignCB, NULL);
  bindCDKObject (vTEMPLATE, temp, '+', swapSignCB, NULL);
  bindCDKObject (vTEMPLATE, temp, '=', swapSignCB, NULL);

  GLabel  = newCDKLabel (screen, 30, 7, mesg, 1, FALSE, FALSE);

  drawCDKScreen (screen);

  bankName	= activateCDKEntry (entry, NULL);
  amount	= activateCDKTemplate (temp, NULL);
  strcpy (holder, bankName);

  destroyCDKLabel(GLabel);
  destroyCDKEntry(entry);
  destroyCDKScreen(screen);
  delwin(window);
  endCDK();

  number = atof (amount);
  number = (float)((float)number / (float)100) * GSign;
  printf("\n\nThe string is : %s\n", holder);
  printf("The number is : %8.2f\n", number);

  exit(0);
}

int swapSignCB (EObjectType cdktype, void *object, void *clientData, chtype key)
{
   char *mesg[3];

   if (key == '-')
   {
      GSign = -1;
      mesg[0] = "-";
   }
   else
   {
      GSign = 1;
      mesg[0] = "+";
   }

   setCDKLabelMessage (GLabel, mesg, 1);
   drawCDKLabel (GLabel, GLabel->box);
   return 0;
}
