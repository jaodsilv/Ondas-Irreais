#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define updateDelta() delta = ((float) max(hmax, pmax))/255
#define value(h, d) (float) ceil(h/d)

typedef unsigned int ui;

typedef struct{
  int blue;
  int red;
} Color;

typedef struct{
  int x, y;
  int step;
} drop;

/* Global Variables */
ui larg, alt, L, H, T, v, N, s, steps, ndrops=0;
int procs, **lake;
Color **colors;
drop *drops;
float e, P, timestep, hmax, pmax, delta;


/*h(p,t)=(p-vt)e^(-(p-vt)^2-t/10)*/
float h(float p, float t)
{
  float d = p-v*t;
  return d*exp(-d*d-t/10);
}

FILE * createPPM()
{
  FILE * pFile;
  pFile = fopen ("out.ppm","w");
  if (pFile!=NULL) {
    fprintf(pFile, "P3\n%d %d\n255\n", larg, alt);
  }
  return pFile;
}

void updatePoint(ui i, ui j)
{
  float height = 0;
  ui k;
  /* Paralelizável */
  for (k = 0; k < ndrops; ++k) {
    /* TODO for each drop calculate distance sqrt(dx^2+dy^2) from (i,j) */
    /* TODO For each distance calculate h() and sum to height */
  }
  if (height < e && height > -e) {
    lake[i][j] = 0;
    return;
  }
  if (height > hmax) hmax = height;
  else if (-height > pmax) pmax = height;
  lake[i][j] = height;
}

void updateColor(int i, int j)
{
  if (lake[i][j] > 0) {
    colors[i][j].red = 0;
    colors[i][j].blue = value(lake[i][j], delta);
  } else {
    colors[i][j].blue = 0;
    colors[i][j].red = value(lake[i][j], delta);
  }
}

int main(int argc, char const *argv[])
{
  FILE * pFile;
  ui i, j;
  if (argc != 3) {
    printf("Wrong number of arguments\n");
    return 1;
  }
  pFile = fopen (argv[1],"r");
  if (pFile!=NULL)
  {
    procs = atoi(argv[2]);
    fscanf (pFile, "(%u,%u)\n(%u,%u)\n%u\n%u\n%f\n%u\n%f\n%u\n", &larg, &alt, &L, &H, &T, &v, &e, &N, &P, &s);
    fclose (pFile);
    timestep = ((float) T)/ ((float) N);
    srand(s);

    /* Mallocs */
    lake = (int**) malloc (L*sizeof(int*));
    colors = (Color**) malloc (L*sizeof(Color*));
    for (i = 0; i < L; ++i) {
      lake[i] = (int*) malloc (H*sizeof(int));
      colors[i] = (Color*) malloc (H*sizeof(Color));
    }
    drops = (drop*) malloc (N*sizeof(drop));

    for (steps = 0; steps < N; ++steps) { /* Paralelizável, mas prioridade baixa */
      if (((float) rand())/RAND_MAX < P) {
        drop d;
        d.x = rand()%L;
        d.y = rand()%H;
        d.step = steps;
        drops[ndrops++] = d;
      }

      /* Atualiza os pontos */
      for(i = 0; i < larg; ++i) { /* Paralelizável */
        for (j = 0; j < alt; ++j) { /* Paralelizável */
          updatePoint(i, j);
        }
      }
    }
  }
  return 0;
}