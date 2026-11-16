// laco principal da simulacao

#include <stdlib.h>
#include "simulacao.h"
#include "eventos_base.h"
#include "eventos_missao.h"
#include "eventos_invasao.h"
#include "relatorio.h"

void simula (struct mundo_t *m)
{
  struct evento_t *ev ;
  int tipo, prio ;
  bool fim = false ;

  while (!fim && fprio_tamanho (m->lef) > 0)
  {
    ev = fprio_retira (m->lef, &tipo, &prio) ;

    // ignora evento de heroi morto
    if (ev->heroi != -1 && !m->herois[ev->heroi].vivo)
    {
      free (ev) ;
      continue ;
    }

    m->relogio = ev->tempo ;
    m->eventos_tratados++ ;

    switch (ev->tipo)
    {
      case EV_CHEGA:
        ev_chega (m, ev->heroi, ev->base) ;
        break ;
      case EV_ESPERA:
        ev_espera (m, ev->heroi, ev->base) ;
        break ;
      case EV_DESISTE:
        ev_desiste (m, ev->heroi, ev->base) ;
        break ;
      case EV_AVISA:
        ev_avisa (m, ev->base) ;
        break ;
      case EV_ENTRA:
        ev_entra (m, ev->heroi, ev->base) ;
        break ;
      case EV_SAI:
        ev_sai (m, ev->heroi, ev->base) ;
        break ;
      case EV_VIAJA:
        ev_viaja (m, ev->heroi, ev->base) ;
        break ;
      case EV_MORRE:
        ev_morre (m, ev->heroi, ev->base, ev->missao) ;
        break ;
      case EV_MISSAO:
        ev_missao (m, ev->missao) ;
        break ;
      case EV_INVADE:
        ev_invade (m, ev->missao, ev->base) ;
        break ;
      case EV_RECUPERA:
        ev_recupera (m, ev->heroi) ;
        break ;
      case EV_RECONSTROI:
        ev_reconstroi (m, ev->base) ;
        break ;
      case EV_FIM:
        ev_fim (m) ;
        fim = true ;
        break ;
    }
    free (ev) ;
  }
}
