// programa principal do projeto "The Boys - 2024/2"
// Autor: Gabriel Pereira dos Santos, GRR 20263239

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#include "conjunto.h"
#include "fila.h"
#include "fprio.h"

#define T_INICIO        0
#define T_FIM_DO_MUNDO  525600
#define N_TAMANHO_MUNDO 20000
#define N_HABILIDADES   10
#define N_HEROIS        (N_HABILIDADES * 5)
#define N_BASES         (N_HEROIS / 5)
#define N_MISSOES       (T_FIM_DO_MUNDO / 100)
#define N_INVASOES      (T_FIM_DO_MUNDO / 10000)

// tipos de evento
#define EV_CHEGA      0
#define EV_ESPERA     1
#define EV_DESISTE    2
#define EV_AVISA      3
#define EV_ENTRA      4
#define EV_SAI        5
#define EV_VIAJA      6
#define EV_MORRE      7
#define EV_MISSAO     8
#define EV_INVADE     9
#define EV_RECUPERA   10
#define EV_RECONSTROI 11
#define EV_FIM        12

struct evento_t
{
  int tempo ;
  int tipo ;
  int heroi ;   // -1 se nao tem heroi
  int base ;
  int missao ;  // no INVADE guarda a invasao
} ;

struct heroi_t
{
  int id ;
  struct cjto_t *habs ;
  int paciencia ;
  int velocidade ;
  int experiencia ;
  int base ;
  bool vivo ;
  bool ativo ;          // false quando debilitado
  int fim_penalidade ;
} ;

struct base_t
{
  int id ;
  int lotacao ;
  struct cjto_t *presentes ;
  struct fila_t *espera ;
  bool ativa ;
  int x, y ;
  int fila_max ;
  int missoes ;
} ;

struct missao_t
{
  int id ;
  struct cjto_t *habs ;
  int perigo ;
  int x, y ;
  int tentativas ;
  bool cumprida ;
} ;

struct invasao_t
{
  int id ;
  int base ;
  int tph ;
  int tpb ;
} ;

struct mundo_t
{
  struct heroi_t herois[N_HEROIS] ;
  struct base_t bases[N_BASES] ;
  struct missao_t missoes[N_MISSOES] ;
  struct invasao_t invasoes[N_INVASOES] ;
  struct fprio_t *lef ;
  int relogio ;
  int eventos_tratados ;
  int invasoes_realizadas ;
} ;

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

// imprime "[ a b c ]"
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
// tipo -1 tira todos os eventos dele
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
