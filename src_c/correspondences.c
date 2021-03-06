/****************************************************************************

Routine:	       	correspondences.c

Author/Copyright:      	Hans-Gerd Maas

Address:	      	Institute of Geodesy and Photogrammetry
	      		ETH - Hoenggerberg
	      		CH - 8093 Zurich

Creation Date:	       	1988/89

Description:	       	establishment of correspondences for 2/3/4 cameras

****************************************************************************/

#include "ptv.h"
#include "parameters.h"
#include "epi.h"
#include "tools.h"

/****************************************************************************/
/*--------------- 4 camera model: consistent quadruplets -------------------*/
/****************************************************************************/

void correspondences_4 (volume_par *vpar, control_par *cpar)
{
  int 	i,j,k,l,m,n,o,  i1,i2,i3;
  int   count, match0=0, match4=0, match3=0, match2=0, match1=0;
  int 	p1,p2,p3,p4, p31, p41, p42;
  int  	pt1;
  int 	tim[4][nmax];
  double       	xa12,ya12,xb12,yb12,X,Y,Z;
  double       	corr;
  candidate   	cand[maxcand];
  n_tupel     	*con0;
  correspond  	*list[4][4];

  for (j=0; j<4; j++) for (i=0; i<nmax; i++) tim[j][i] = 0;

  /* allocate memory for lists of correspondences */
  for (i1 = 0; i1 < cpar->num_cams - 1; i1++)
    for (i2 = i1 + 1; i2 < cpar->num_cams; i2++)
    list[i1][i2] = (correspond *) malloc (num[i1] * sizeof (correspond));

  con0 = (n_tupel *) malloc (4*nmax * sizeof (n_tupel));

  /*  initialize ...  */
  sprintf (buf,"Establishing correspondences");
  match=0; match0=0; match2=0;

  for (i1 = 0; i1 < cpar->num_cams - 1; i1++)
    for (i2 = i1 + 1; i2 < cpar->num_cams; i2++)
      for (i=0; i<num[i1]; i++)
	{
	  list[i1][i2][i].p1 = 0;
	  list[i1][i2][i].n = 0;
	}
  for (i = 0; i < nmax; i++) {
    for (j = 0; j < 4; j++) {
        tim[j][i] = 0;
        con0[i].p[j] = -1;
    }
    con0[i].corr = 0;
  }

  /* -------------if only one cam and 2D--------- */ //by Beat L�thi June 2007
  if(cpar->num_cams == 1){
	  if(res_name[0]==0){
          sprintf (res_name, "rt_is");
	  }
	 fp1 = fopen (res_name, "w");
		fprintf (fp1, "%4d\n", num[0]);
	  for (i=0; i<num[0]; i++){
          o = epi_mm_2D (geo[0][i].x,geo[0][i].y,
		      Ex[0], I[0],  G[0], mmp, vpar,
		      &X,&Y,&Z);
          pix[0][geo[0][i].pnr].tnr=i;
		  fprintf (fp1, "%4d", i+1);
		  fprintf (fp1, " %9.3f %9.3f %9.3f", X, Y, Z);
          fprintf (fp1, " %4d", geo[0][i].pnr);
          fprintf (fp1, " %4d", -1);
          fprintf (fp1, " %4d", -1);
          fprintf (fp1, " %4d\n", -1);
	  }
	  fclose (fp1);
	  match1=num[0];
  }
  /* -------------end of only one cam and 2D ------------ */

  /* matching  1 -> 2,3,4  +  2 -> 3,4  +  3 -> 4 */
  for (i1 = 0; i1 < cpar->num_cams - 1; i1++)
    for (i2 = i1 + 1; i2 < cpar->num_cams; i2++) {
     printf ("Establishing correspondences  %d - %d\n", i1, i2);
     /* establish correspondences from num[i1] points of img[i1] to img[i2] */

      for (i=0; i<num[i1]; i++)	if (geo[i1][i].x != -999) {
      o = epi_mm (geo[i1][i].x,geo[i1][i].y,
		      Ex[i1], I[i1], G[i1], Ex[i2], I[i2], G[i2], mmp, vpar,
		      &xa12, &ya12, &xb12, &yb12);
	  
    /////ich glaube, da muss ich einsteigen, wenn alles erledigt ist.
	  ///////mit bild_1 x,y Epipole machen und dann selber was schreiben um die Distanz zu messen.
	  ///////zu Punkt in bild_2.

	  /* origin point in the list */
	  p1 = i;  list[i1][i2][p1].p1 = p1;	pt1 = geo[i1][p1].pnr;

	  /* search for a conjugate point in geo[i2] */
      find_candidate_plus (geo[i2], pix[i2], num[i2],
			       xa12, ya12, xb12, yb12, 
			       pix[i1][pt1].n,pix[i1][pt1].nx,pix[i1][pt1].ny,
			       pix[i1][pt1].sumg, cand, &count, i2, vpar);
	  /* write all corresponding candidates to the preliminary list */
	  /* of correspondences */
	  if (count > maxcand)	{ count = maxcand; }
	  for (j=0; j<count; j++)
	    {
	      list[i1][i2][p1].p2[j] = cand[j].pnr;
	      list[i1][i2][p1].corr[j] = cand[j].corr;
	      list[i1][i2][p1].dist[j] = cand[j].tol;
	    }
	  list[i1][i2][p1].n = count;
	}
  }

  /* ------------------------------------------------------------------ */
  /* search consistent quadruplets in the list */
  if (cpar->num_cams == 4)
    {
      printf ("Search consistent quadruplets\n");
	printf("num0=%d\n",num[0]);
      for (i=0, match0=0; i<num[0]; i++)
	{
	  p1 = list[0][1][i].p1;
	  for (j=0; j<list[0][1][i].n; j++)
	    for (k=0; k<list[0][2][i].n; k++)
	      for (l=0; l<list[0][3][i].n; l++)
		{
		  p2 = list[0][1][i].p2[j];
		  p3 = list[0][2][i].p2[k];
		  p4 = list[0][3][i].p2[l];
		  for (m=0; m<list[1][2][p2].n; m++)
		    for (n=0; n<list[1][3][p2].n; n++)
		      {
			p31 = list[1][2][p2].p2[m];
			p41 = list[1][3][p2].p2[n];
			if (p3 == p31  &&  p4 == p41)
			  for (o=0; o<list[2][3][p3].n; o++)
			    {
			      p42 = list[2][3][p3].p2[o];
			      if (p4 == p42)
				{
				  corr = (list[0][1][i].corr[j]
					  + list[0][2][i].corr[k]
					  + list[0][3][i].corr[l]
					  + list[1][2][p2].corr[m]
					  + list[1][3][p2].corr[n]
					  + list[2][3][p3].corr[o])
				    / (list[0][1][i].dist[j]
				       + list[0][2][i].dist[k]
				       + list[0][3][i].dist[l]
				       + list[1][2][p2].dist[m]
				       + list[1][3][p2].dist[n]
				       + list[2][3][p3].dist[o]);
				  if (corr > vpar->corrmin)
				    {
				      /* accept as preliminary match */
				      con0[match0].p[0] = p1;
				      con0[match0].p[1] = p2;
				      con0[match0].p[2] = p3;
				      con0[match0].p[3] = p4;
				      con0[match0++].corr = corr;
				      if (match0 == 4*nmax)	/* security */
					{
					  printf ("Overflow in correspondences:");
					  printf (" > %d matches\n", match0);
					  i = num[0];
					}
				    }
				}
			    }
		      }
		}
	}


      /* -------------------------------------------------------------------- */

      /* sort quadruplets for match quality (.corr) */
      quicksort_con (con0, match0);

      /* -------------------------------------------------------------------- */

      /* take quadruplets from the top to the bottom of the sorted list */
      /* only if none of the points has already been used */
      for (i=0, match=0; i<match0; i++)
	{
	  p1 = con0[i].p[0];	if (p1 > -1)	if (++tim[0][p1] > 1)	continue;
	  p2 = con0[i].p[1];	if (p2 > -1)	if (++tim[1][p2] > 1)	continue;
	  p3 = con0[i].p[2];	if (p3 > -1)	if (++tim[2][p3] > 1)	continue;
	  p4 = con0[i].p[3];	if (p4 > -1)	if (++tim[3][p4] > 1)	continue;
	  con[match++] = con0[i];
	}

      match4 = match;
      printf ("%d consistent quadruplets, \n", match4);	puts (buf);
    }

  /* ----------------------------------------------------------------------- */
  /* ----------------------------------------------------------------------- */

  /* search consistent triplets :  123, 124, 134, 234 */
  if ((cpar->num_cams ==4 && allCam_flag==0) || cpar->num_cams ==3)
    {
   //   puts ("Search consistent triplets");
      printf("Search consistent triplets");
      match0=0;
      for (i1 = 0; i1 < cpar->num_cams - 2; i1++)
        for (i2 = i1 + 1; i2 < cpar->num_cams - 1; i2++)
            for (i3 = i2 + 1; i3 < cpar->num_cams; i3++)
	    for (i=0; i<num[i1]; i++)
	      {
		p1 = list[i1][i2][i].p1;
		if (p1 > nmax  ||  tim[i1][p1] > 0)	continue;

		for (j=0; j<list[i1][i2][i].n; j++)
		  for (k=0; k<list[i1][i3][i].n; k++)
		    {
		      p2 = list[i1][i2][i].p2[j];
		      if (p2 > nmax  ||  tim[i2][p2] > 0)	continue;
		      p3 = list[i1][i3][i].p2[k];
		      if (p3 > nmax  ||  tim[i3][p3] > 0)	continue;

		      for (m=0; m<list[i2][i3][p2].n; m++)
			{
			  p31 = list[i2][i3][p2].p2[m];
			  if (p3 == p31)
			    {
			      corr = (list[i1][i2][i].corr[j]
				      + list[i1][i3][i].corr[k]
				      + list[i2][i3][p2].corr[m])
				/ (list[i1][i2][i].dist[j]
				   + list[i1][i3][i].dist[k]
				   + list[i2][i3][p2].dist[m]);
                if (corr > vpar->corrmin) {
                    for (n = 0; n < cpar->num_cams; n++) con0[match0].p[n] = -2;
                        con0[match0].p[i1] = p1;
                        con0[match0].p[i2] = p2;
                        con0[match0].p[i3] = p3;
                        con0[match0++].corr = corr;
                    }
				    if (match0 == 4*nmax) {   /* security */
                        printf ("Overflow in correspondences:");
                        printf (" > %d matches\n", match0);
                        i = num[i1]; /* Break out of the outer loop over i */
					}
			    }
			}
		    }
	      }

      /* ----------------------------------------------------------------------- */

      /* sort triplets for match quality (.corr) */
      quicksort_con (con0, match0);

      /* ----------------------------------------------------------------------- */

      /* pragmatic version: */
      /* take triplets from the top to the bottom of the sorted list */
      /* only if none of the points has already been used */
      for (i=0; i<match0; i++)
	{
	  p1 = con0[i].p[0];	if (p1 > -1)	if (++tim[0][p1] > 1)	continue;
	  p2 = con0[i].p[1];	if (p2 > -1)	if (++tim[1][p2] > 1)	continue;
	  p3 = con0[i].p[2];	if (p3 > -1)	if (++tim[2][p3] > 1)	continue;
	  p4 = con0[i].p[3];	if (p4 > -1  && cpar->num_cams > 3) if (++tim[3][p4] > 1) continue;

	  con[match++] = con0[i];
	}

      match3 = match - match4;
     // sprintf (buf, "%d consistent quadruplets, %d triplets ", match4, match3);
      //puts (buf);
printf ( "%d consistent quadruplets, %d triplets ", match4, match3);
 printf("\nCheckpoint 8\n");
      /* repair artifact (?) */
      if (cpar->num_cams == 3) for (i=0; i<match; i++)	con[i].p[3] = -1;
    }

  /* ----------------------------------------------------------------------- */
  /* ----------------------------------------------------------------------- */

  /* search consistent pairs :  12, 13, 14, 23, 24, 34 */
  /* only if an object model is available or if only 2 images are used */
      if(cpar->num_cams > 1 && allCam_flag==0){
	printf("Search pairs");


  match0 = 0;
  for (i1 = 0; i1 < cpar->num_cams - 1; i1++)
    if ( cpar->num_cams == 2 || (num[0] < 64 && num[1] < 64 && num[2] < 64 && num[3] < 64))
      for (i2 = i1 + 1; i2 < cpar->num_cams; i2++)
	for (i=0; i<num[i1]; i++)
	  {
	    p1 = list[i1][i2][i].p1;
	    if (p1 > nmax  ||  tim[i1][p1] > 0)	continue;

	    /* take only unambigous pairs */
	    if (list[i1][i2][i].n != 1)	continue;

	    p2 = list[i1][i2][i].p2[0];
	    if (p2 > nmax  ||  tim[i2][p2] > 0)	continue;

	    corr = list[i1][i2][i].corr[0] / list[i1][i2][i].dist[0];

	    if (corr > vpar->corrmin)
	      {
		con0[match0].p[i1] = p1;
		con0[match0].p[i2] = p2;
		con0[match0++].corr = corr;
	      }
	  }

  /* ----------------------------------------------------------------------- */


  /* sort pairs for match quality (.corr) */
  quicksort_con (con0, match0);

  /* ----------------------------------------------------------------------- */


  /* take pairs from the top to the bottom of the sorted list */
  /* only if none of the points has already been used */
  for (i=0; i<match0; i++)
    {
      p1 = con0[i].p[0];	if (p1 > -1)	if (++tim[0][p1] > 1)	continue;
      p2 = con0[i].p[1];	if (p2 > -1)	if (++tim[1][p2] > 1)	continue;
      p3 = con0[i].p[2];	if (p3 > -1  && cpar->num_cams > 2)
	if (++tim[2][p3] > 1)	continue;
      p4 = con0[i].p[3];	if (p4 > -1  && cpar->num_cams > 3)
	if (++tim[3][p4] > 1)	continue;

      con[match++] = con0[i];
    }
  } //end pairs?

  match2 = match-match4-match3;
  if(cpar->num_cams == 1){
 printf ( "determined %d points from 2D", match1);
  }
  else{
 printf ("%d consistent quadruplets(red), %d triplets(green) and %d unambigous pairs\n",
	      match4, match3, match2);
  }
  /* ----------------------------------------------------------------------- */

  /* give each used pix the correspondence number */
  for (i=0; i<match; i++)
    {
      for (j = 0; j < cpar->num_cams; j++)
	{
	  p1 = geo[j][con[i].p[j]].pnr;
	  if (p1 > -1 && p1 < 1202590843)
	    {
	      pix[j][p1].tnr= i;
	    }
	}
    }

  /* draw crosses on canvas */
  if (display) {
    int count1=0;
	j=0;
	for (i=0; i<num[j]; i++)
	  {			      	
	    p1 = pix[j][i].tnr;
	    if (p1 == -1 )
	      {
		count1++;
	      }
	  }
printf("unused inside = %d\n",count1);
  }

// fill globals for cython postprocessing
// denis
match4_g=match4;
match3_g=match3;
match2_g=match2;
match1_g=match1;

  /* ----------------------------------------------------------------------- */
  /* free memory for lists of correspondences */
  for (i1 = 0; i1 < cpar->num_cams - 1; i1++)
    for (i2 = i1 + 1; i2 < cpar->num_cams; i2++)
        free (list[i1][i2]);

  free (con0);

  printf ("Correspondences done");
}

