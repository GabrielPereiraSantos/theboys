// evento fim e estatisticas finais

#include <stdio.h>
#include "relatorio.h"
#include "auxiliares.h"

void ev_fim (struct mundo_t *m)
{
  int i ;
  int cumpridas = 0 ;
  int mortos = 0 ;
  int soma = 0 ;
  int tmin = m->missoes[0].tentativas ;
  int tmax = m->missoes[0].tentativas ;

  printf ("%6d: FIM\n\n", m->relogio) ;

  for (i = 0; i < N_HEROIS; i++)
  {
    if (m->herois[i].vivo)
      printf ("HEROI %2d VIVO ", i) ;
    else
    {
      printf ("HEROI %2d MORTO ", i) ;
      mortos++ ;
    }
    printf ("PAC %3d VEL %4d EXP %4d HABS ", m->herois[i].paciencia,
            m->herois[i].velocidade, m->herois[i].experiencia) ;
    imprime_cjto (m->herois[i].habs) ;
    printf ("\n") ;
  }

  for (i = 0; i < N_BASES; i++)
    printf ("BASE %2d LOT %2d FILA MAX %2d MISSOES %d\n", i,
            m->bases[i].lotacao, m->bases[i].fila_max, m->bases[i].missoes) ;

  for (i = 0; i < N_MISSOES; i++)
  {
    if (m->missoes[i].cumprida)
      cumpridas++ ;
    soma = soma + m->missoes[i].tentativas ;
    if (m->missoes[i].tentativas < tmin)
      tmin = m->missoes[i].tentativas ;
    if (m->missoes[i].tentativas > tmax)
      tmax = m->missoes[i].tentativas ;
  }

  printf ("\nEVENTOS TRATADOS: %d\n", m->eventos_tratados) ;
  printf ("MISSOES CUMPRIDAS: %d/%d (%.1f%%)\n", cumpridas, N_MISSOES,
          100.0 * cumpridas / N_MISSOES) ;
  printf ("INVASÕES REALIZADAS: %d/%d (%.1f%%)\n", m->invasoes_realizadas,
          N_INVASOES, 100.0 * m->invasoes_realizadas / N_INVASOES) ;
  printf ("TENTATIVAS/MISSAO: MIN %d, MAX %d, MEDIA %.1f\n", tmin, tmax,
          (double) soma / N_MISSOES) ;
  printf ("TAXA MORTALIDADE: %.1f%%\n", 100.0 * mortos / N_HEROIS) ;
}
