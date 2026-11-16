// eventos de chegada, espera, entrada, saida e viagem

#include <stdio.h>
#include <stdlib.h>
#include "eventos_base.h"
#include "auxiliares.h"
#include "lef.h"

void ev_chega (struct mundo_t *m, int h, int b)
{
  int presentes = cjto_card (m->bases[b].presentes) ;
  int fila = fila_tamanho (m->bases[b].espera) ;
  int lotacao = m->bases[b].lotacao ;
  bool espera ;

  m->herois[h].base = b ;

  // nao testa base ativa, heroi pode entrar em base inativa
  if (presentes < lotacao && fila == 0)
    espera = true ;
  else
    espera = m->herois[h].paciencia > 10 * fila ;

  if (espera)
  {
    printf ("%6d: CHEGA     HEROI %2d BASE %d (%2d/%2d) ESPERA\n",
            m->relogio, h, b, presentes, lotacao) ;
    agenda (m, m->relogio, EV_ESPERA, h, b, -1) ;
  }
  else
  {
    printf ("%6d: CHEGA     HEROI %2d BASE %d (%2d/%2d) DESISTE\n",
            m->relogio, h, b, presentes, lotacao) ;
    agenda (m, m->relogio, EV_DESISTE, h, b, -1) ;
  }
}

void ev_espera (struct mundo_t *m, int h, int b)
{
  int *id = malloc (sizeof (int)) ;
  *id = h ;

  printf ("%6d: ESPERA HEROI %2d BASE %d (%2d)\n", m->relogio, h, b,
          fila_tamanho (m->bases[b].espera)) ;

  fila_insere (m->bases[b].espera, id) ;
  if (fila_tamanho (m->bases[b].espera) > m->bases[b].fila_max)
    m->bases[b].fila_max = fila_tamanho (m->bases[b].espera) ;

  agenda (m, m->relogio, EV_AVISA, -1, b, -1) ;
}

void ev_desiste (struct mundo_t *m, int h, int b)
{
  int destino = aleat (0, N_BASES - 1) ;

  agenda (m, m->relogio, EV_VIAJA, h, destino, -1) ;
  printf ("%6d: DESIST HEROI %2d BASE %d\n", m->relogio, h, b) ;
}

void ev_avisa (struct mundo_t *m, int b)
{
  struct base_t *base = &m->bases[b] ;
  int *id ;

  printf ("%6d: AVISA     PORTEIRO BASE %d (%2d/%2d) FILA [ ", m->relogio,
          b, cjto_card (base->presentes), base->lotacao) ;
  if (fila_tamanho (base->espera) > 0)
  {
    fila_imprime (base->espera) ;
    printf (" ") ;
  }
  printf ("]\n") ;

  while (cjto_card (base->presentes) < base->lotacao &&
         fila_tamanho (base->espera) > 0)
  {
    id = fila_retira (base->espera) ;
    cjto_insere (base->presentes, *id) ;
    agenda (m, m->relogio, EV_ENTRA, *id, b, -1) ;
    printf ("%6d: AVISA     PORTEIRO BASE %d ADMITE %2d\n", m->relogio,
            b, *id) ;
    free (id) ;
  }
}

void ev_entra (struct mundo_t *m, int h, int b)
{
  int tpb = 15 + m->herois[h].paciencia * aleat (1, 20) ;

  agenda (m, m->relogio + tpb, EV_SAI, h, b, -1) ;
  printf ("%6d: ENTRA    HEROI %2d BASE %d (%2d/%2d) SAI %d\n", m->relogio,
          h, b, cjto_card (m->bases[b].presentes), m->bases[b].lotacao,
          m->relogio + tpb) ;
}

void ev_sai (struct mundo_t *m, int h, int b)
{
  int destino ;

  // heroi nao esta na base
  if (!cjto_pertence (m->bases[b].presentes, h))
    return ;

  cjto_retira (m->bases[b].presentes, h) ;
  destino = aleat (0, N_BASES - 1) ;
  agenda (m, m->relogio, EV_VIAJA, h, destino, -1) ;
  agenda (m, m->relogio, EV_AVISA, -1, b, -1) ;

  printf ("%6d: SAI      HEROI %2d BASE %d (%2d/%2d)\n", m->relogio, h, b,
          cjto_card (m->bases[b].presentes), m->bases[b].lotacao) ;
}

void ev_viaja (struct mundo_t *m, int h, int d)
{
  struct heroi_t *heroi = &m->herois[h] ;
  int atual = heroi->base ;
  int dist, vel, duracao ;

  dist = distancia (m->bases[atual].x, m->bases[atual].y,
                    m->bases[d].x, m->bases[d].y) ;

  // evita divisao por zero
  vel = heroi->velocidade ;
  if (vel <= 0)
    vel = 1 ;

  // divisao inteira, trunca
  duracao = dist / vel ;

  agenda (m, m->relogio + duracao, EV_CHEGA, h, d, -1) ;
  printf ("%6d: VIAJA    HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d\n",
          m->relogio, h, atual, d, dist, heroi->velocidade,
          m->relogio + duracao) ;
}
