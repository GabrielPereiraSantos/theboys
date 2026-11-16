// Autor: Gabriel Pereira dos Santos, GRR 20263239

#include <stdlib.h>
#include <time.h>

#include "entidades.h"
#include "universo.h"
#include "simulacao.h"

int main ()
{
  struct mundo_t mundo ;

  srand (time (NULL)) ;

  cria_mundo (&mundo) ;
  eventos_iniciais (&mundo) ;
  simula (&mundo) ;
  destroi_mundo (&mundo) ;

  return 0 ;
}
