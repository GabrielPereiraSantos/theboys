// constantes, tipos de evento e structs do mundo

#ifndef ENTIDADES
#define ENTIDADES

#include <stdbool.h>
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

#endif
