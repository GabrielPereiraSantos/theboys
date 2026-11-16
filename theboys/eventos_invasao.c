// eventos de invasao, recuperacao e reconstrucao

#include <stdio.h>
#include "eventos_invasao.h"
#include "lef.h"

void ev_invade (struct mundo_t *m, int id, int b)
{
  struct invasao_t *inv = &m->invasoes[id] ;
  int i ;
  int qtd = 0 ;

  if (!m->bases[b].ativa)
    return ;

  m->invasoes_realizadas++ ;
  printf ("%6d: INVADE   BASE %d   INVADE %d\n", m->relogio, b, id) ;
  printf ("%6d: INVADE %d BASE %d HEROIS INATIVOS [", m->relogio, id, b) ;

  for (i = 0; i < N_HEROIS; i++)
  {
    if (cjto_pertence (m->bases[b].presentes, i))
    {
      m->herois[i].ativo = false ;
      // se ja estava debilitado fica com o fim mais tarde
      if (m->relogio + inv->tph > m->herois[i].fim_penalidade)
        m->herois[i].fim_penalidade = m->relogio + inv->tph ;
      agenda (m, m->relogio + inv->tph, EV_RECUPERA, i, b, -1) ;

      // tira SAI e ENTRA pendentes, senao sai de novo depois
      remove_eventos_heroi (m, i, EV_SAI) ;
      remove_eventos_heroi (m, i, EV_ENTRA) ;
      agenda (m, m->relogio, EV_SAI, i, b, -1) ;

      if (qtd > 0)
        printf (", ") ;
      printf ("%d", i) ;
      qtd++ ;
    }
  }
  printf ("]\n") ;

  m->bases[b].ativa = false ;
  agenda (m, m->relogio + inv->tpb, EV_RECONSTROI, -1, b, -1) ;
  printf ("%6d: INVADE %d BASE %d INATIVA\n", m->relogio, id, b) ;
}

void ev_recupera (struct mundo_t *m, int h)
{
  if (m->herois[h].vivo && m->relogio >= m->herois[h].fim_penalidade)
    m->herois[h].ativo = true ;
}

void ev_reconstroi (struct mundo_t *m, int b)
{
  m->bases[b].ativa = true ;
}
