// programa principal do projeto "The Boys - 2024/2"
// Autor: Gabriel Pereira dos Santos, GRR 20263239

#include <stdlib.h>
#include <time.h>

#include "entidades.h"
#include "universo.h"
#include "simulacao.h"

int main (int argc, char *argv[])
{
  struct mundo_t mundo ;

  // semente no argv[1] para testar
  if (argc > 1)
    srand (atoi (argv[1])) ;
  else
    srand (time (NULL)) ;

  cria_mundo (&mundo) ;
  eventos_iniciais (&mundo) ;
  simula (&mundo) ;
  destroi_mundo (&mundo) ;

  return 0 ;
}
