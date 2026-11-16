// eventos de missao e morte

#include <stdio.h>
#include "eventos_missao.h"
#include "auxiliares.h"
#include "lef.h"

void ev_morre (struct mundo_t *m, int h, int b, int missao)
{
  if (cjto_pertence (m->bases[b].presentes, h))
    cjto_retira (m->bases[b].presentes, h) ;
  m->herois[h].vivo = false ;
  m->herois[h].ativo = false ;

  remove_eventos_heroi (m, h, -1) ;
  agenda (m, m->relogio, EV_AVISA, -1, b, -1) ;

  printf ("%6d: MORRE    HEROI %2d MISSAO %d\n", m->relogio, h, missao) ;
}

void ev_missao (struct mundo_t *m, int id)
{
  struct missao_t *missao = &m->missoes[id] ;
  int dist[N_BASES], ordem[N_BASES] ;
  struct cjto_t *uniao = NULL ;
  struct cjto_t *aux ;
  int i, j, k, b, temp ;
  int escolhida = -1 ;
  double risco ;

  missao->tentativas++ ;
  printf ("%6d: MISSAO %d TENT %d HAB REQ: ", m->relogio, id,
          missao->tentativas) ;
  imprime_cjto (missao->habs) ;
  printf ("\n") ;

  for (i = 0; i < N_BASES; i++)
  {
    dist[i] = distancia (m->bases[i].x, m->bases[i].y, missao->x, missao->y) ;
    ordem[i] = i ;
  }

  // ordena bases pela distancia
  for (i = 1; i < N_BASES; i++)
  {
    temp = ordem[i] ;
    j = i - 1 ;
    while (j >= 0 && dist[ordem[j]] > dist[temp])
    {
      ordem[j + 1] = ordem[j] ;
      j-- ;
    }
    ordem[j + 1] = temp ;
  }

  for (k = 0; k < N_BASES && escolhida == -1; k++)
  {
    b = ordem[k] ;
    if (!m->bases[b].ativa)
      continue ;

    printf ("%6d: MISSAO %d BASE %d DIST %d HEROIS ", m->relogio, id, b,
            dist[b]) ;
    imprime_cjto (m->bases[b].presentes) ;
    printf ("\n") ;

    // junta habilidades dos herois ativos
    uniao = cjto_cria (N_HABILIDADES) ;
    for (i = 0; i < N_HEROIS; i++)
    {
      if (cjto_pertence (m->bases[b].presentes, i) && m->herois[i].ativo)
      {
        printf ("%6d: MISSAO %d HAB HEROI %2d: ", m->relogio, id, i) ;
        imprime_cjto (m->herois[i].habs) ;
        printf ("\n") ;

        aux = cjto_uniao (uniao, m->herois[i].habs) ;
        cjto_destroi (uniao) ;
        uniao = aux ;
      }
    }

    printf ("%6d: MISSAO %d UNIAO HAB BASE %d: ", m->relogio, id, b) ;
    imprime_cjto (uniao) ;
    printf ("\n") ;

    if (cjto_contem (uniao, missao->habs) == 1)
      escolhida = b ;
    else
      uniao = cjto_destroi (uniao) ;
  }

  if (escolhida == -1)
  {
    printf ("%6d: MISSAO %d IMPOSSIVEL\n", m->relogio, id) ;
    agenda (m, m->relogio + 24 * 60, EV_MISSAO, -1, -1, id) ;
    return ;
  }

  missao->cumprida = true ;
  m->bases[escolhida].missoes++ ;
  printf ("%6d: MISSAO %d CUMPRIDA BASE %d HABS: ", m->relogio, id,
          escolhida) ;
  imprime_cjto (uniao) ;
  printf ("\n") ;
  cjto_destroi (uniao) ;

  for (i = 0; i < N_HEROIS; i++)
  {
    if (cjto_pertence (m->bases[escolhida].presentes, i) && m->herois[i].ativo)
    {
      risco = missao->perigo /
              (m->herois[i].paciencia + m->herois[i].experiencia + 1.0) ;
      if (risco > aleat (0, 30))
        agenda (m, m->relogio, EV_MORRE, i, escolhida, id) ;
      else
        m->herois[i].experiencia++ ;
    }
  }
}
