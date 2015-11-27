#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define value(h, d) (int) ceil(fabs(h)/d)
#define t(step, timestep) step*timestep

typedef unsigned int ui;

typedef struct{
  int x, y;
  int step;
} drop;

typedef struct{
  float average, sqrSum;
} Point;

/* Global Variables */
ui larg, alt, L, H, T, v, N, s, steps, ndrops=0, pdistx, pdisty;
int procs;
drop *drops;
Point **points;
float e, P, timestep, hmax, pmax, delta, **lake;
FILE *PPMFile, *averageFile;


/*h(p,t)=(p-vt)e^(-(p-vt)^2-t/10)*/
float h(float p, float t)
{
  float d = p-v*t;
  return d*exp(-d*d-t/10);
}

void createOutputFiles()
{
  PPMFile = fopen ("out.ppm","w");
  averageFile = fopen ("out.txt","w");
  if (PPMFile!=NULL) {
    fprintf(PPMFile, "P3\n%d %d\n255\n", larg, alt);
  }
}

void closeOutputFiles() {
  fclose(PPMFile);
  fclose(averageFile);
}

void updatePoint(ui i, ui j)
{
  float height = 0;
  ui k;
  int x, y;
  /* Paralelizável */
  for (k = 0; k < ndrops; ++k) {
    x = (drops[k].x - i) * pdistx;
    y = (drops[k].y - j) * pdisty;
    height += h(sqrt(x*x+y*y), t(steps, timestep));
  }
  if (height < e && height > -e) {
    lake[i][j] = 0;
    return;
  }
  if (height > hmax) {
    hmax = height;
  }
  else if (-height > pmax) {
    pmax = -height;
  }
  lake[i][j] = height;
  points[i][j].average += height;
  points[i][j].sqrSum += height*height;
}

void evaluatePointForPPM(ui i, ui j)
{
  if (lake[i][j] > 0) {
    if (j == 0)
      fprintf(PPMFile, "0 0 %d", value(lake[i][j], delta));
    else
      fprintf(PPMFile, " 0 0 %d", value(lake[i][j], delta));
  } else {
    if (j == 0)
      fprintf(PPMFile, "%d 0 0", value(lake[i][j], delta));
    else
      fprintf(PPMFile, " %d 0 0", value(lake[i][j], delta));
  }

}

int main(int argc, char const *argv[])
{
  FILE * pFile;
  if (argc != 3) {
    printf("Wrong number of arguments\n");
    return 1;
  }
  pFile = fopen (argv[1],"r");
  if (pFile!=NULL)
  {
    ui i, j;
    procs = atoi(argv[2]);
    fscanf (pFile, "(%u,%u)\n(%u,%u)\n", &larg, &alt, &L, &H);
    fscanf (pFile, "%u\n%u\n%f\n%u\n%f\n%u\n", &T, &v, &e, &N, &P, &s);
    fclose (pFile);
    timestep = ((float) T)/ ((float) N);
    P /= 100;
    srand(s);

    /* Mallocs */
    lake = (float**) malloc (L*sizeof(float*));
    points = (Point**) malloc (L*sizeof(Point*));
    for (i = 0; i < L; ++i) {
      lake[i] = (float*) malloc (H*sizeof(float));
      points[i] = (Point*) malloc (H*sizeof(Point));
    }
    drops = (drop*) malloc (N*sizeof(drop));


    pdistx = (float) larg / (float) L;
    pdisty = (float) alt / (float) H;

    for (steps = 0; steps < N; ++steps) { /* Paralelizável, mas prioridade baixa */

      pmax = 0;
      hmax = 0;

      /* Atualiza os pontos */
      for(i = 0; i < L; ++i) { /* Paralelizável */
        for (j = 0; j < H; ++j) { /* Paralelizável */
          updatePoint(i, j);
        }
      }

      /* Add drops */
      if (((float) rand())/RAND_MAX < P) {
        drop d;
        d.x = rand()%L;
        d.y = rand()%H;
        d.step = steps;
        drops[ndrops++] = d;
      }
    }

    delta = fmaxf(hmax, pmax)/255;

    createOutputFiles();
    for(i = 0; i < L; ++i) { /* Paralelizável */
      for (j = 0; j < H; ++j) { /* Paralelizável */
        evaluatePointForPPM(i, j);
        /* Average and StdDev */
        points[i][j].average /= N;
        fprintf(averageFile, "%d %d %12.7f %12.7f\n", i, j, points[i][j].average, sqrt(points[i][j].sqrSum/N - points[i][j].average*points[i][j].average));
      }
      fprintf(PPMFile, "\n");
    }


    closeOutputFiles();
  }
  return 0;
}