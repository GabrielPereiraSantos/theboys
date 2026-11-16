// lista de eventos futuros

#include <stdlib.h>
#include "lef.h"

void agenda (struct mundo_t *m, int tempo, int tipo, int heroi, int base,
             int missao)
{
  struct evento_t *ev = malloc (sizeof (struct evento_t)) ;

  ev->tempo = tempo ;
  ev->tipo = tipo ;
  ev->heroi = heroi ;
  ev->base = base ;
  ev->missao = missao ;
  fprio_insere (m->lef, ev, tipo, tempo) ;
}

// tira tudo da lef e devolve so o que nao for do heroi
void remove_eventos_heroi (struct mundo_t *m, int h, int tipo)
{
  struct fprio_t *nova = fprio_cria () ;
  struct evento_t *ev ;
  int t, p ;

  ev = fprio_retira (m->lef, &t, &p) ;
  while (ev != NULL)
  {
    if (ev->heroi == h && (tipo == -1 || ev->tipo == tipo))
      free (ev) ;
    else
      fprio_insere (nova, ev, t, p) ;
    ev = fprio_retira (m->lef, &t, &p) ;
  }

  fprio_destroi (m->lef) ;
  m->lef = nova ;
}
