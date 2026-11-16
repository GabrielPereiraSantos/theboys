// funcoes auxiliares

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "auxiliares.h"

int aleat (int min, int max)
{
  return min + rand () % (max - min + 1) ;
}

int distancia (int x1, int y1, int x2, int y2)
{
  double dx = x1 - x2 ;
  double dy = y1 - y2 ;
  return sqrt (dx * dx + dy * dy) ;
}

void imprime_cjto (struct cjto_t *c)
{
  printf ("[ ") ;
  if (cjto_card (c) > 0)
  {
    cjto_imprime (c) ;
    printf (" ") ;
  }
  printf ("]") ;
}
