// criacao e destruicao do mundo

#include "universo.h"
#include "auxiliares.h"
#include "lef.h"

void cria_mundo (struct mundo_t *m)
{
  int i ;

  m->relogio = T_INICIO ;
  m->eventos_tratados = 0 ;
  m->invasoes_realizadas = 0 ;
  m->lef = fprio_cria () ;

  for (i = 0; i < N_HEROIS; i++)
  {
    m->herois[i].id = i ;
    m->herois[i].experiencia = 0 ;
    m->herois[i].paciencia = aleat (0, 100) ;
    m->herois[i].velocidade = aleat (50, 5000) ;
    m->herois[i].habs = cjto_aleat (aleat (1, 3), N_HABILIDADES) ;
    m->herois[i].base = -1 ;
    m->herois[i].vivo = true ;
    m->herois[i].ativo = true ;
    m->herois[i].fim_penalidade = 0 ;
  }

  for (i = 0; i < N_BASES; i++)
  {
    m->bases[i].id = i ;
    m->bases[i].x = aleat (0, N_TAMANHO_MUNDO - 1) ;
    m->bases[i].y = aleat (0, N_TAMANHO_MUNDO - 1) ;
    m->bases[i].lotacao = aleat (3, 10) ;
    // cap N_HEROIS pq guarda id de heroi
    m->bases[i].presentes = cjto_cria (N_HEROIS) ;
    m->bases[i].espera = fila_cria () ;
    m->bases[i].ativa = true ;
    m->bases[i].fila_max = 0 ;
    m->bases[i].missoes = 0 ;
  }

  for (i = 0; i < N_MISSOES; i++)
  {
    m->missoes[i].id = i ;
    m->missoes[i].x = aleat (0, N_TAMANHO_MUNDO - 1) ;
    m->missoes[i].y = aleat (0, N_TAMANHO_MUNDO - 1) ;
    m->missoes[i].habs = cjto_aleat (aleat (6, 10), N_HABILIDADES) ;
    m->missoes[i].perigo = aleat (0, 100) ;
    m->missoes[i].tentativas = 0 ;
    m->missoes[i].cumprida = false ;
  }

  for (i = 0; i < N_INVASOES; i++)
  {
    m->invasoes[i].id = i ;
    m->invasoes[i].base = aleat (0, N_BASES - 1) ;
    m->invasoes[i].tph = aleat (0, 4320) ;
    m->invasoes[i].tpb = aleat (0, 7200) ;
  }
}

void eventos_iniciais (struct mundo_t *m)
{
  int i, base, tempo ;

  for (i = 0; i < N_HEROIS; i++)
  {
    base = aleat (0, N_BASES - 1) ;
    tempo = aleat (0, 4320) ;
    agenda (m, tempo, EV_CHEGA, i, base, -1) ;
  }

  for (i = 0; i < N_MISSOES; i++)
  {
    tempo = aleat (0, T_FIM_DO_MUNDO) ;
    agenda (m, tempo, EV_MISSAO, -1, -1, i) ;
  }

  for (i = 0; i < N_INVASOES; i++)
  {
    tempo = aleat (4321, T_FIM_DO_MUNDO) ;
    agenda (m, tempo, EV_INVADE, -1, m->invasoes[i].base, i) ;
  }

  agenda (m, T_FIM_DO_MUNDO, EV_FIM, -1, -1, -1) ;
}

void destroi_mundo (struct mundo_t *m)
{
  int i ;

  for (i = 0; i < N_HEROIS; i++)
    cjto_destroi (m->herois[i].habs) ;

  for (i = 0; i < N_BASES; i++)
  {
    cjto_destroi (m->bases[i].presentes) ;
    fila_destroi (m->bases[i].espera) ;
  }

  for (i = 0; i < N_MISSOES; i++)
    cjto_destroi (m->missoes[i].habs) ;

  fprio_destroi (m->lef) ;
}
