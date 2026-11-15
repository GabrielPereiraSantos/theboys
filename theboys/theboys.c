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

// #ifndef permite reduzir o mundo com -D nos testes
#ifndef T_FIM_DO_MUNDO
#define T_FIM_DO_MUNDO 525600
#endif
#ifndef N_HABILIDADES
#define N_HABILIDADES 10
#endif

#define T_INICIO        0
#define N_TAMANHO_MUNDO 20000
#define N_HEROIS        (N_HABILIDADES * 5)
#define N_BASES         (N_HEROIS / 5)
#define N_MISSOES       (T_FIM_DO_MUNDO / 100)
#define N_INVASOES      (T_FIM_DO_MUNDO / 10000)

enum tipo_evento
{
  EV_CHEGA, EV_ESPERA, EV_DESISTE, EV_AVISA, EV_ENTRA, EV_SAI, EV_VIAJA,
  EV_MORRE, EV_MISSAO, EV_INVADE, EV_RECUPERA, EV_RECONSTROI, EV_FIM
} ;

struct evento_t
{
  int tempo ;
  int tipo ;
  int heroi ;   // -1 se nao se aplica
  int base ;
  int missao ;  // missao (MORRE/MISSAO) ou invasao (INVADE)
} ;

// morto: vivo = false; debilitado: vivo = true e ativo = false
struct heroi_t
{
  int id ;
  struct cjto_t *habs ;
  int paciencia ;
  int velocidade ;
  int experiencia ;
  int base ;
  bool vivo ;
  bool ativo ;
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

// inteiro aleatorio em [min, max]
static int aleat (int min, int max)
{
  return min + rand () % (max - min + 1) ;
}

static int distancia (int x1, int y1, int x2, int y2)
{
  double dx = x1 - x2 ;
  double dy = y1 - y2 ;
  return (int) sqrt (dx * dx + dy * dy) ;
}

// formato "[ a b c ]" ou "[ ]"
static void imprime_cjto (struct cjto_t *c)
{
  printf ("[ ") ;
  if (cjto_card (c) > 0)
  {
    cjto_imprime (c) ;
    printf (" ") ;
  }
  printf ("]") ;
}

static void agenda (struct mundo_t *m, int tempo, int tipo, int heroi,
                    int base, int missao)
{
  struct evento_t *ev = malloc (sizeof (struct evento_t)) ;
  if (!ev)
  {
    fprintf (stderr, "erro de alocacao\n") ;
    exit (1) ;
  }
  ev->tempo = tempo ;
  ev->tipo = tipo ;
  ev->heroi = heroi ;
  ev->base = base ;
  ev->missao = missao ;
  if (fprio_insere (m->lef, ev, tipo, tempo) < 0)
  {
    free (ev) ;
    fprintf (stderr, "erro ao inserir na LEF\n") ;
    exit (1) ;
  }
}

// fprio nao remove item arbitrario: esvazia e reinsere os que ficam
// tipo < 0 remove todos os eventos do heroi
static void remove_eventos_heroi (struct mundo_t *m, int h, int tipo)
{
  struct fprio_t *nova = fprio_cria () ;
  struct evento_t *ev ;
  int t, p ;

  if (!nova)
  {
    fprintf (stderr, "erro de alocacao\n") ;
    exit (1) ;
  }

  while ((ev = fprio_retira (m->lef, &t, &p)))
  {
    if (ev->heroi == h && (tipo < 0 || ev->tipo == tipo))
      free (ev) ;
    else
      fprio_insere (nova, ev, t, p) ;
  }
  fprio_destroi (m->lef) ;
  m->lef = nova ;
}

static void cria_mundo (struct mundo_t *m)
{
  int i ;

  m->relogio = T_INICIO ;
  m->eventos_tratados = 0 ;
  m->invasoes_realizadas = 0 ;
  m->lef = fprio_cria () ;

  for (i = 0; i < N_HEROIS; i++)
  {
    struct heroi_t *h = &m->herois[i] ;
    h->id = i ;
    h->experiencia = 0 ;
    h->paciencia = aleat (0, 100) ;
    h->velocidade = aleat (50, 5000) ;
    h->habs = cjto_aleat (aleat (1, 3), N_HABILIDADES) ;
    h->base = -1 ;
    h->vivo = true ;
    h->ativo = true ;
    h->fim_penalidade = 0 ;
  }

  for (i = 0; i < N_BASES; i++)
  {
    struct base_t *b = &m->bases[i] ;
    b->id = i ;
    b->x = aleat (0, N_TAMANHO_MUNDO - 1) ;
    b->y = aleat (0, N_TAMANHO_MUNDO - 1) ;
    b->lotacao = aleat (3, 10) ;
    // cap = N_HEROIS pois guarda ids; lotacao controlada a parte
    b->presentes = cjto_cria (N_HEROIS) ;
    b->espera = fila_cria () ;
    b->ativa = true ;
    b->fila_max = 0 ;
    b->missoes = 0 ;
  }

  for (i = 0; i < N_MISSOES; i++)
  {
    struct missao_t *ms = &m->missoes[i] ;
    ms->id = i ;
    ms->x = aleat (0, N_TAMANHO_MUNDO - 1) ;
    ms->y = aleat (0, N_TAMANHO_MUNDO - 1) ;
    ms->habs = cjto_aleat (aleat (6, 10), N_HABILIDADES) ;
    ms->perigo = aleat (0, 100) ;
    ms->tentativas = 0 ;
    ms->cumprida = false ;
  }

  for (i = 0; i < N_INVASOES; i++)
  {
    struct invasao_t *inv = &m->invasoes[i] ;
    inv->id = i ;
    inv->base = aleat (0, N_BASES - 1) ;
    inv->tph = aleat (0, 4320) ;
    inv->tpb = aleat (0, 7200) ;
  }

  if (!m->lef)
  {
    fprintf (stderr, "erro de alocacao\n") ;
    exit (1) ;
  }
  for (i = 0; i < N_HEROIS; i++)
    if (!m->herois[i].habs)
    {
      fprintf (stderr, "erro de alocacao\n") ;
      exit (1) ;
    }
  for (i = 0; i < N_BASES; i++)
    if (!m->bases[i].presentes || !m->bases[i].espera)
    {
      fprintf (stderr, "erro de alocacao\n") ;
      exit (1) ;
    }
  for (i = 0; i < N_MISSOES; i++)
    if (!m->missoes[i].habs)
    {
      fprintf (stderr, "erro de alocacao\n") ;
      exit (1) ;
    }
}

static void eventos_iniciais (struct mundo_t *m)
{
  int i ;

  for (i = 0; i < N_HEROIS; i++)
    agenda (m, aleat (0, 4320), EV_CHEGA, i, aleat (0, N_BASES - 1), -1) ;

  for (i = 0; i < N_MISSOES; i++)
    agenda (m, aleat (0, T_FIM_DO_MUNDO), EV_MISSAO, -1, -1, i) ;

  for (i = 0; i < N_INVASOES; i++)
    agenda (m, aleat (4321, T_FIM_DO_MUNDO), EV_INVADE, -1,
            m->invasoes[i].base, i) ;

  agenda (m, T_FIM_DO_MUNDO, EV_FIM, -1, -1, -1) ;
}

static void destroi_mundo (struct mundo_t *m)
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

static void ev_chega (struct mundo_t *m, int h, int b)
{
  struct heroi_t *her = &m->herois[h] ;
  struct base_t *base = &m->bases[b] ;
  int pres = cjto_card (base->presentes) ;
  int fila = fila_tamanho (base->espera) ;
  bool espera ;

  her->base = b ;

  // sem teste de base ativa: heroi pode entrar em base inativa
  if (pres < base->lotacao && fila == 0)
    espera = true ;
  else
    espera = her->paciencia > 10 * fila ;

  printf ("%6d: CHEGA     HEROI %2d BASE %d (%2d/%2d) %s\n", m->relogio,
          h, b, pres, base->lotacao, espera ? "ESPERA" : "DESISTE") ;

  if (espera)
    agenda (m, m->relogio, EV_ESPERA, h, b, -1) ;
  else
    agenda (m, m->relogio, EV_DESISTE, h, b, -1) ;
}

static void ev_espera (struct mundo_t *m, int h, int b)
{
  struct base_t *base = &m->bases[b] ;
  int *id = malloc (sizeof (int)) ;
  int tam ;

  if (!id)
  {
    fprintf (stderr, "erro de alocacao\n") ;
    exit (1) ;
  }
  *id = h ;

  printf ("%6d: ESPERA HEROI %2d BASE %d (%2d)\n", m->relogio, h, b,
          fila_tamanho (base->espera)) ;

  fila_insere (base->espera, id) ;
  tam = fila_tamanho (base->espera) ;
  if (tam > base->fila_max)
    base->fila_max = tam ;

  agenda (m, m->relogio, EV_AVISA, -1, b, -1) ;
}

static void ev_desiste (struct mundo_t *m, int h, int b)
{
  int d = aleat (0, N_BASES - 1) ;

  agenda (m, m->relogio, EV_VIAJA, h, d, -1) ;
  printf ("%6d: DESIST HEROI %2d BASE %d\n", m->relogio, h, b) ;
}

static void ev_avisa (struct mundo_t *m, int b)
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

  while (cjto_card (base->presentes) < base->lotacao
         && fila_tamanho (base->espera) > 0)
  {
    id = fila_retira (base->espera) ;
    cjto_insere (base->presentes, *id) ;
    agenda (m, m->relogio, EV_ENTRA, *id, b, -1) ;
    printf ("%6d: AVISA     PORTEIRO BASE %d ADMITE %2d\n", m->relogio,
            b, *id) ;
    free (id) ;
  }
}

static void ev_entra (struct mundo_t *m, int h, int b)
{
  struct base_t *base = &m->bases[b] ;
  int tpb = 15 + m->herois[h].paciencia * aleat (1, 20) ;

  agenda (m, m->relogio + tpb, EV_SAI, h, b, -1) ;
  printf ("%6d: ENTRA    HEROI %2d BASE %d (%2d/%2d) SAI %d\n", m->relogio,
          h, b, cjto_card (base->presentes), base->lotacao,
          m->relogio + tpb) ;
}

static void ev_sai (struct mundo_t *m, int h, int b)
{
  struct base_t *base = &m->bases[b] ;
  int d ;

  // protecao: heroi ja fora da base
  if (!cjto_pertence (base->presentes, h))
    return ;

  cjto_retira (base->presentes, h) ;
  d = aleat (0, N_BASES - 1) ;
  agenda (m, m->relogio, EV_VIAJA, h, d, -1) ;
  agenda (m, m->relogio, EV_AVISA, -1, b, -1) ;

  printf ("%6d: SAI      HEROI %2d BASE %d (%2d/%2d)\n", m->relogio, h, b,
          cjto_card (base->presentes), base->lotacao) ;
}

// duracao truncada: (int) (distancia / velocidade)
static void ev_viaja (struct mundo_t *m, int h, int d)
{
  struct heroi_t *her = &m->herois[h] ;
  struct base_t *orig = &m->bases[her->base] ;
  struct base_t *dest = &m->bases[d] ;
  int dist = distancia (orig->x, orig->y, dest->x, dest->y) ;
  int vel = her->velocidade > 0 ? her->velocidade : 1 ;
  int duracao = dist / vel ;

  agenda (m, m->relogio + duracao, EV_CHEGA, h, d, -1) ;
  printf ("%6d: VIAJA    HEROI %2d BASE %d BASE %d DIST %d VEL %d CHEGA %d\n",
          m->relogio, h, her->base, d, dist, her->velocidade,
          m->relogio + duracao) ;
}

static void ev_morre (struct mundo_t *m, int h, int b, int ms)
{
  struct heroi_t *her = &m->herois[h] ;

  if (cjto_pertence (m->bases[b].presentes, h))
    cjto_retira (m->bases[b].presentes, h) ;
  her->vivo = false ;
  her->ativo = false ;

  remove_eventos_heroi (m, h, -1) ;
  agenda (m, m->relogio, EV_AVISA, -1, b, -1) ;

  printf ("%6d: MORRE    HEROI %2d MISSAO %d\n", m->relogio, h, ms) ;
}

static void ev_missao (struct mundo_t *m, int id)
{
  struct missao_t *ms = &m->missoes[id] ;
  int dist[N_BASES], ordem[N_BASES] ;
  struct cjto_t *uniao = NULL, *tmp ;
  int i, j, k, aux, bmp = -1 ;

  ms->tentativas++ ;
  printf ("%6d: MISSAO %d TENT %d HAB REQ: ", m->relogio, id,
          ms->tentativas) ;
  imprime_cjto (ms->habs) ;
  printf ("\n") ;

  // ordena bases por distancia (insercao, estavel)
  for (i = 0; i < N_BASES; i++)
  {
    dist[i] = distancia (m->bases[i].x, m->bases[i].y, ms->x, ms->y) ;
    ordem[i] = i ;
  }
  for (i = 1; i < N_BASES; i++)
  {
    aux = ordem[i] ;
    for (j = i - 1; j >= 0 && dist[ordem[j]] > dist[aux]; j--)
      ordem[j + 1] = ordem[j] ;
    ordem[j + 1] = aux ;
  }

  for (k = 0; k < N_BASES && bmp < 0; k++)
  {
    struct base_t *base = &m->bases[ordem[k]] ;

    if (!base->ativa)
      continue ;

    printf ("%6d: MISSAO %d BASE %d DIST %d HEROIS ", m->relogio, id,
            base->id, dist[base->id]) ;
    imprime_cjto (base->presentes) ;
    printf ("\n") ;

    // uniao so dos herois ativos
    uniao = cjto_cria (N_HABILIDADES) ;
    for (i = 0; i < N_HEROIS; i++)
    {
      if (!cjto_pertence (base->presentes, i) || !m->herois[i].ativo)
        continue ;
      printf ("%6d: MISSAO %d HAB HEROI %2d: ", m->relogio, id, i) ;
      imprime_cjto (m->herois[i].habs) ;
      printf ("\n") ;
      tmp = cjto_uniao (uniao, m->herois[i].habs) ;
      cjto_destroi (uniao) ;
      uniao = tmp ;
    }

    printf ("%6d: MISSAO %d UNIAO HAB BASE %d: ", m->relogio, id, base->id) ;
    imprime_cjto (uniao) ;
    printf ("\n") ;

    if (cjto_contem (uniao, ms->habs) == 1)
      bmp = base->id ;
    else
      uniao = cjto_destroi (uniao) ;
  }

  if (bmp < 0)
  {
    printf ("%6d: MISSAO %d IMPOSSIVEL\n", m->relogio, id) ;
    agenda (m, m->relogio + 24 * 60, EV_MISSAO, -1, -1, id) ;
    return ;
  }

  ms->cumprida = true ;
  m->bases[bmp].missoes++ ;
  printf ("%6d: MISSAO %d CUMPRIDA BASE %d HABS: ", m->relogio, id, bmp) ;
  imprime_cjto (uniao) ;
  printf ("\n") ;
  cjto_destroi (uniao) ;

  for (i = 0; i < N_HEROIS; i++)
  {
    struct heroi_t *her = &m->herois[i] ;
    double risco ;

    if (!cjto_pertence (m->bases[bmp].presentes, i) || !her->ativo)
      continue ;

    risco = ms->perigo / (her->paciencia + her->experiencia + 1.0) ;
    if (risco > aleat (0, 30))
      agenda (m, m->relogio, EV_MORRE, i, bmp, id) ;
    else
      her->experiencia++ ;
  }
}

static void ev_invade (struct mundo_t *m, int id, int b)
{
  struct invasao_t *inv = &m->invasoes[id] ;
  struct base_t *base = &m->bases[b] ;
  bool primeiro = true ;
  int i ;

  if (!base->ativa)
    return ;

  m->invasoes_realizadas++ ;
  printf ("%6d: INVADE   BASE %d   INVADE %d\n", m->relogio, b, id) ;
  printf ("%6d: INVADE %d BASE %d HEROIS INATIVOS [", m->relogio, id, b) ;

  for (i = 0; i < N_HEROIS; i++)
  {
    struct heroi_t *her = &m->herois[i] ;

    if (!cjto_pertence (base->presentes, i))
      continue ;

    her->ativo = false ;
    if (m->relogio + inv->tph > her->fim_penalidade)
      her->fim_penalidade = m->relogio + inv->tph ;
    agenda (m, m->relogio + inv->tph, EV_RECUPERA, i, b, -1) ;

    // ENTRA pendente tambem agendaria um SAI futuro
    remove_eventos_heroi (m, i, EV_SAI) ;
    remove_eventos_heroi (m, i, EV_ENTRA) ;
    agenda (m, m->relogio, EV_SAI, i, b, -1) ;

    printf (primeiro ? "%d" : ", %d", i) ;
    primeiro = false ;
  }
  printf ("]\n") ;

  base->ativa = false ;
  agenda (m, m->relogio + inv->tpb, EV_RECONSTROI, -1, b, -1) ;
  printf ("%6d: INVADE %d BASE %d INATIVA\n", m->relogio, id, b) ;
}

// so reativa ao fim da penalidade mais longa
static void ev_recupera (struct mundo_t *m, int h)
{
  struct heroi_t *her = &m->herois[h] ;

  if (her->vivo && m->relogio >= her->fim_penalidade)
    her->ativo = true ;
}

static void ev_reconstroi (struct mundo_t *m, int b)
{
  m->bases[b].ativa = true ;
}

static void ev_fim (struct mundo_t *m)
{
  int i, cumpridas = 0, mortos = 0, soma = 0 ;
  int tmin = m->missoes[0].tentativas, tmax = m->missoes[0].tentativas ;

  printf ("%6d: FIM\n\n", m->relogio) ;

  for (i = 0; i < N_HEROIS; i++)
  {
    struct heroi_t *her = &m->herois[i] ;
    printf ("HEROI %2d %s PAC %3d VEL %4d EXP %4d HABS ", i,
            her->vivo ? "VIVO" : "MORTO", her->paciencia, her->velocidade,
            her->experiencia) ;
    imprime_cjto (her->habs) ;
    printf ("\n") ;
    if (!her->vivo)
      mortos++ ;
  }

  for (i = 0; i < N_BASES; i++)
    printf ("BASE %2d LOT %2d FILA MAX %2d MISSOES %d\n", i,
            m->bases[i].lotacao, m->bases[i].fila_max, m->bases[i].missoes) ;

  for (i = 0; i < N_MISSOES; i++)
  {
    struct missao_t *ms = &m->missoes[i] ;
    if (ms->cumprida)
      cumpridas++ ;
    soma += ms->tentativas ;
    if (ms->tentativas < tmin)
      tmin = ms->tentativas ;
    if (ms->tentativas > tmax)
      tmax = ms->tentativas ;
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

static void simula (struct mundo_t *m)
{
  struct evento_t *ev ;
  int tipo, prio ;
  bool fim = false ;

  while (!fim && (ev = fprio_retira (m->lef, &tipo, &prio)))
  {
    // evento antigo de heroi morto
    if (ev->heroi >= 0 && !m->herois[ev->heroi].vivo)
    {
      free (ev) ;
      continue ;
    }

    m->relogio = ev->tempo ;
    m->eventos_tratados++ ;

    switch (ev->tipo)
    {
      case EV_CHEGA:      ev_chega (m, ev->heroi, ev->base) ; break ;
      case EV_ESPERA:     ev_espera (m, ev->heroi, ev->base) ; break ;
      case EV_DESISTE:    ev_desiste (m, ev->heroi, ev->base) ; break ;
      case EV_AVISA:      ev_avisa (m, ev->base) ; break ;
      case EV_ENTRA:      ev_entra (m, ev->heroi, ev->base) ; break ;
      case EV_SAI:        ev_sai (m, ev->heroi, ev->base) ; break ;
      case EV_VIAJA:      ev_viaja (m, ev->heroi, ev->base) ; break ;
      case EV_MORRE:      ev_morre (m, ev->heroi, ev->base, ev->missao) ;
                          break ;
      case EV_MISSAO:     ev_missao (m, ev->missao) ; break ;
      case EV_INVADE:     ev_invade (m, ev->missao, ev->base) ; break ;
      case EV_RECUPERA:   ev_recupera (m, ev->heroi) ; break ;
      case EV_RECONSTROI: ev_reconstroi (m, ev->base) ; break ;
      case EV_FIM:        ev_fim (m) ; fim = true ; break ;
    }
    free (ev) ;
  }
}

// semente opcional em argv[1] para depuracao
int main (int argc, char *argv[])
{
  static struct mundo_t mundo ;

  if (argc > 1)
    srand (atoi (argv[1])) ;
  else
    srand (time (NULL)) ;

  cria_mundo (&mundo) ;
  eventos_iniciais (&mundo) ;
  simula (&mundo) ;
  destroi_mundo (&mundo) ;

  return (0) ;
}
