/*
 *
 * Galaxy.c
 * Monday, 5/23/1994.
 *
 */

#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GnuARG.h>
#include <Time.h>


#define PI 3.1415926535

#define InRange(v,min,max) (((v)<(max))&&((v)>(min)))

void Usage (void)
   {
   printf ("GALAXY    xyplan.dat  builder          by Craig Fitzgerald   %s\n", __DATE__, __TIME__);
   printf ("\n");
   printf ("This program builds the planet position file used by VGA Planets.\n");
   printf ("\n");
   printf ("USAGE: GALAXY [options] OutFile\n");
   printf ("\n");
   printf ("WHERE: options are 0 or more of:\n");
   printf ("        /XRange  = n  Horiz size of galaxy (def: 2000)\n");
   printf ("        /YRange  = n  Vert size of galaxy (def: 2000)\n");
   printf ("        /Planets = n  # of planets in galaxy (def: 500)\n");
   printf ("        /Density = n  Another method of specifying #planets. (1.0=normal)\n");
   printf ("        /Shape   = n  Galaxy Shape (0=square, 1=circle, 2=dense ctr 3=spiral)\n");
   printf ("        /Cluster = n  group into patches (n = # of patches)\n");
   printf ("        /CSV  = file  Dump planets in comma separated list.\n");
   printf ("        /Names        Include planet names in CSV dump.\n\n");
   printf ("        /Init    = n  Random Number Seed.\n");
   printf ("        /MinDist = n  Minimum distance between stars (def: 10).\n");

   printf ("  Shape type 2 (galaxy) is a circle with the planet density increasing\n");
   printf ("  towards the center.  When using the circular shape types, the minimum\n");
   printf ("  of the XRange and YRange is used to define the circle diameter.\n");
   exit (0);
   }



void Square (USHORT uPlanets, 
             USHORT uXRange, USHORT uYRange, 
             USHORT X[],     USHORT Y[])
   {
   USHORT uXOffset, uYOffset, i, j;
   BOOL   bWorking;

   uXOffset = 1000 - uXRange / 2;
   uYOffset = 1000 - uYRange / 2;

   for (i=0; i<uPlanets; i++)
      {
      for (bWorking=TRUE; bWorking; )
         {
         X[i] = rand () % uXRange + uXOffset;

         Y[i] = rand () % uYRange + uYOffset;

         bWorking = FALSE;
         for (j=0; j<i; j++)
            if ((X[j] == X[i]) && (Y[j] == Y[i]))
               {
               bWorking = TRUE;
               continue;
               }
         }
      }
   }


void Circle (USHORT uPlanets, USHORT uRange, USHORT X[], USHORT Y[])
   {
   USHORT i, j;
   BOOL   bUnique, bOutside;
   double bgRad, bgRadsq, bgX, bgY;

   bgRad   = uRange / 2;
   bgRadsq = bgRad * bgRad;

   for (i=0; i<uPlanets; i++)
      {
      for (bUnique=FALSE; !bUnique; )
         {
         for (bOutside=TRUE; bOutside; )
            {
            bgX = (bgRad * 2) * ((double)rand () / 32768.0) - bgRad;
            bgY = (bgRad * 2) * ((double)rand () / 32768.0) - bgRad;

            if (bgX*bgX + bgY*bgY < bgRadsq)
               bOutside = FALSE;
            }

         X[i] = (USHORT)bgX + 1000;
         Y[i] = (USHORT)bgY + 1000;

         bUnique = TRUE;
         for (j=0; j<i; j++)
            if ((X[j] == X[i]) && (Y[j] == Y[i]))
               {
               bUnique = FALSE;
               continue;
               }
         }
      }
   }




void Galaxy (USHORT uPlanets, USHORT uRange,
             USHORT X[],      USHORT Y[])
   {
   double  bgAngle, bgx, bgy, bgRad, bgCtr;
   USHORT  uRadius, i, j;
   BOOL    bWorking;

   uRadius = uRange / 2;
   bgCtr = 1000.0;

   for (i=0; i<uPlanets; i++)
      {
      for (bWorking=TRUE; bWorking; )
         {
         bgAngle  = (PI * 2)  * ((double)rand () / 32768.0);
         bgRad    = (uRadius) * ((double)rand () / 32768.0);

         bgx = cos (bgAngle) * bgRad + bgCtr;
         bgy = sin (bgAngle) * bgRad + bgCtr;

         X[i] = (USHORT)bgx;
         Y[i] = (USHORT)bgy;

         bWorking = FALSE;
         for (j=0; j<i; j++)
            if ((X[j] == X[i]) && (Y[j] == Y[i]))
               {
               bWorking = TRUE;
               continue;
               }
         }
      }
   }




void Galaxy2(USHORT uPlanets, USHORT uRange,
             USHORT X[],      USHORT Y[])
   {
   double  bgD, bgT, bgx, bgy, bgSq, bgArm, bgDiv;
   double  bgRad, bgCtr, bgRnd, bgFuzz, bgMinFuzz;
   USHORT  i, j;
   BOOL    bWorking;

   bgCtr = 1000.0;
   bgRad = (double)uRange / 2.0;
   bgSq  = sqrt (bgRad);
   bgD   = (4.0*PI) / bgSq;

   bgDiv = (1 / sqrt (bgRad)) * 125;

   bgMinFuzz = bgRad / (6 * bgDiv);

   for (i=0; i<uPlanets; i++)
      {
      for (bWorking=TRUE; bWorking; )
         {
         /*--- Get distance from galactic ctr weighted outward ---*/
         if (rand () % 2)
            {
            bgRnd = bgSq * ((double) rand () / 32768.0);
            bgRnd = bgRad - (bgRnd * bgRnd);
            }
         else
            {
            bgRnd = bgRad * ((double)rand () / 32768.0);
            }

         /*--- Choose an arm ---*/
         bgArm = (rand () % 2 ? PI : 0);

         /*--- get initial X and Y position ---*/
         bgT = sqrt (bgRnd) * bgD;
         bgx = cos (bgT + bgArm) * bgRnd;
         bgy = sin (bgT + bgArm) * bgRnd;

         /*--- fuzz the X position ---*/
         bgFuzz = sqrt (max (bgRnd/bgDiv, bgMinFuzz)) * ((double)rand () / 32768.0);
         bgFuzz = bgFuzz * bgFuzz;
         bgFuzz = (rand () % 2 ? bgFuzz : 0 - bgFuzz);
         X[i] = (USHORT)(bgx + bgCtr + bgFuzz);

         /*--- fuzz the Y position ---*/
         bgFuzz = sqrt (max(bgRnd/bgDiv, bgMinFuzz)) * ((double)rand () / 32768.0);
         bgFuzz = bgFuzz * bgFuzz;
         bgFuzz = (rand () % 2 ? bgFuzz : 0 - bgFuzz);
         Y[i] = (USHORT)(bgy + bgCtr + bgFuzz);

         if ( (X[i] > (USHORT)(1000 + uRange / 2)) ||
              (X[i] < (USHORT)(1000 - uRange / 2)) ||
              (Y[i] > (USHORT)(1000 + uRange / 2)) ||
              (Y[i] < (USHORT)(1000 - uRange / 2)))
            {
            bWorking = TRUE;
            continue;
            }
         bWorking = FALSE;
         for (j=0; j<i; j++)
            if ((X[j] == X[i]) && (Y[j] == Y[i]))
               {
               bWorking = TRUE;
               continue;
               }
         }
      }
   }





void FillRange (USHORT uPlanets, LONG lBaseTotal, USHORT Array[])
   {
   LONG lDiff, lDel;
   LONG lTotal = 0;
   USHORT i;

   if (uPlanets >= 500)
      return;

   for (i=0; i<uPlanets; i++)
      lTotal += (LONG)Array[i]; 

   lDiff = (lBaseTotal - lTotal);

   for (i=uPlanets; i<500; i++)
      {
      lDel = max (0L, min (1999L, lDiff));
      Array[i] = (USHORT)lDel;
      lDiff -= lDel;
      }
   }



void FixRange (LONG lBaseTotal, USHORT Array[])
   {
   USHORT i;
   LONG   lDel, lTotal, lNew;

   for (lTotal = 0, i=0; i<500; i++)
      lTotal += Array[i]; 

   /*--- coarse fix ---*/
   while (lDel = (lBaseTotal - lTotal) / 500)
      {
      for (i=0; i<500; i++)
         {
         lNew = (LONG)Array[i] + lDel;
         lNew = max (0L, min (1999L, lNew));
         lTotal += lNew - (LONG)Array[i];
         Array[i] = (USHORT)lNew;
         }
      }

   /*--- fine fix ---*/
   for (i=0; lTotal != lBaseTotal; i++)
      {
      if (i >= 500) 
         i = 0;
      lDel = (lTotal < lBaseTotal ? 1L : -1L);
      if ((lDel > 0 && Array[i] < 1999) ||
          (lDel < 0 && Array[i] > 0))
         {
         Array[i] += (USHORT)lDel;
         lTotal   += lDel;
         }
      }
   }

void SpaceStars (USHORT uPlanets, SHORT uMinDist, USHORT X[], USHORT Y[])
   {
   BOOL   bWorking;
   short  x1, y1, x2, y2, move;
   USHORT i, j;

   bWorking = TRUE;
   while (bWorking)
      {
      printf (".");
      bWorking = FALSE;
      for (i=0; i<uPlanets; i++)
         {
         x1 = X[i], y1 = Y[i];
         for (j=0; j<uPlanets; j++)
            {
            if (j == i)
               continue;

            x2 = X[j], y2 = Y[j];

            if (!InRange(x1-x2,0-uMinDist,uMinDist))
               continue;
            if (!InRange(y1-y2,0-uMinDist,uMinDist))
               continue;

            move = rand () % ((x1>x2 ? uMinDist+2 : 0-uMinDist-2) - (x1-x2));
            if (InRange (x1+move, 0, 2000) && InRange (x2-move, 0, 2000))
               X[i]+=move, X[j]-=move;

            move = rand () % ((y1>y2 ? uMinDist+2 : 0-uMinDist-2) - (y1-y2));
            if (InRange (y1+move, 0, 2000) && InRange (y2-move, 0, 2000))
               Y[i]+=move, Y[j]-=move;

            bWorking = TRUE;
            j = 500;
            }
         }
      }
   return;
   }


#define XTOTAL   998681 - 500000
#define YTOTAL  1002633 - 500000


int _cdecl main (int argc, char *argv[])
   {
   FILE   *fp, *fpNames;
   char   sz[25];
   BOOL   bNames;
   double bgDensity, bgVol, bgRad;
   USHORT X[500], Y[500];
   USHORT uXRange, uYRange, uShape;
   USHORT uCluster, uMinDist, uPlanets;
   USHORT Zero     =  0;
   USHORT x, y, i;

   ArgBuildBlk ("? *^Help *^XRange% *^YRange% *^Planets% *^Shape% *^Cluster% "
                "*^Init% *^CSV% *^Names *^Density% *^MinDist%");

   if (ArgFillBlk (argv))
      return printf ("BuildArg Error: %s", ArgGetErr ());

   if (ArgIs("?") || ArgIs ("Help") || !ArgIs (NULL))
      Usage ();

   if (!(fp = fopen (ArgGet (NULL, 0), "wb")))
      return printf ("Unable to open output file : %s", ArgGet (NULL, 0));

   uXRange  = (ArgIs ("XRange")  ? atoi (ArgGet ("XRange" , 0)) : 2000);
   uYRange  = (ArgIs ("YRange")  ? atoi (ArgGet ("YRange" , 0)) : 2000);
   uShape   = (ArgIs ("Shape" )  ? atoi (ArgGet ("Shape"  , 0)) : 0);
   uCluster = (ArgIs ("Cluster") ? atoi (ArgGet ("Cluster", 0)) : 0);
   uMinDist = (ArgIs ("MinDist") ? atoi (ArgGet ("MinDist", 0)) : 10);
   uPlanets = (ArgIs ("Planets") ? atoi (ArgGet ("Planets", 0)) : 500);

   if (ArgIs ("Density"))  
      {
      bgDensity = atof (ArgGet ("Density" , 0));
      bgVol     = (double)uXRange * (double)uYRange;
      if (uShape==1 || uShape==2)
         {
         bgRad = (double)min (uXRange, uYRange) / 2.0;
         bgVol = bgRad * bgRad * PI;
         }
      uPlanets  = (USHORT)((bgDensity * bgVol) / 8000.0);
      }

   srand ((ArgIs ("Init")?atoi (ArgGet("Init", 0)):(USHORT)time(NULL)%1000));

   /*--- Limit values to legal values ---*/
   uXRange = max (100, min (2000, uXRange ));
   uYRange = max (100, min (2000, uYRange ));
   uPlanets= max (1,   min (500,  uPlanets));
   uShape  = max (0,   min (3,    uShape  ));
   uCluster= max (0,   min (50,   uCluster));
   uMinDist= max (0,   min (200,  uMinDist));

   switch (uShape)
      {
      case 1:  
         Circle (uPlanets, min (uXRange, uYRange), X, Y); 
         break;
      case 2:  
         Galaxy (uPlanets, min (uXRange, uYRange), X, Y); 
         break;
      case 3:  
         Galaxy2 (uPlanets, min (uXRange, uYRange), X, Y); 
         break;
      default: 
         Square (uPlanets, uXRange, uYRange, X, Y);
      }

   /*--- Add Remaining Planets ---*/
   FillRange (uPlanets, XTOTAL, X);
   FillRange (uPlanets, YTOTAL, Y);

   /*--- Fix Planet Totals ---*/
   FixRange (XTOTAL, X);
   FixRange (YTOTAL, Y);

   /*--- Make sure there is sufficient space around the stars ---*/
   SpaceStars (uPlanets, uMinDist, X, Y);

   for (i=0; i<500; i++)
      {
      x = X[i] + 1000;
      y = Y[i] + 1000;
      fwrite (&x, 2, 1, fp);
      fwrite (&y, 2, 1, fp);
      fwrite (&Zero, 2, 1, fp);
      }
   fclose (fp);

   /*--- write CSV file if requested ---*/
   if (ArgIs ("CSV"))
      {
      bNames = ArgIs ("Names");

      if (!(fp = fopen (ArgGet ("CSV", 0), "wt")))
         return printf ("Unable to open output file : %s", ArgGet ("CSV", 0));

      if (bNames)
         if (!(fpNames = fopen ("planet.nm", "rb")))
            return printf ("Unable to open planet name file : planet.nm");

      for (i=0; i<uPlanets; i++)
         {
         fprintf (fp, "%d,%d,%d", i+1, X[i]+1000, Y[i]+1000);
         if (bNames)
            {
            fread (sz, 1, 20, fpNames);
            sz[20] = '\0';
            fprintf (fp, ",%s\n", sz);
            }
         else
            {
            fprintf (fp, "\n");
            }
         }
      fclose (fp);
      }
   return 0;
   }

