#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*h(p,t)=(p-vt)e^(-(p-vt)^2-t/10)*/
double h(double p, double t, unsigned int v) {
  double d = p-v*t;
  return d*exp(-d*d-t/10);
}


int main(int argc, char const *argv[])
{
  FILE * pFile;
  pFile = fopen (argv[1],"r");
  if (pFile!=NULL)
  {
    unsigned int larg, alt, L, H, T, v, N, s;
    int procs;
    long double e, P, timestep;
    procs = atoi(argv[2]);
    fscanf (pFile, "(%u,%u)\n", &larg, &alt);
    fscanf (pFile, "(%u,%u)\n", &L, &H);
    fscanf (pFile, "%u\n", &T);
    fscanf (pFile, "%u\n", &v);
    fscanf (pFile, "%Lf\n", &e);
    fscanf (pFile, "%u\n", &N);
    fscanf (pFile, "%Lf\n", &P);
    fscanf (pFile, "%u\n", &s);
    fclose (pFile);
    timestep = ((double) T)/ ((double) N);
    printf("(%d,%d)\n(%d,%d)\n%d\n%d\n%Lf\n%d\n%Lf\n%d\n%Lf\n%d\n", larg, alt, L, H, T, v, e, N, P, s, timestep, procs);
  }
  return 0;
}