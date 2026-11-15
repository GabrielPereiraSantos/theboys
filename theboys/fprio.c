// TAD fila de prioridades generica
// Autor: Gabriel Pereira dos Santos, GRR 20263239

#include <stdio.h>
#include <stdlib.h>
#include "fprio.h"

struct fprio_t *fprio_cria ()
{
  struct fprio_t *f = malloc (sizeof (struct fprio_t)) ;
  if (!f)
    return NULL ;

  f->prim = NULL ;
  f->fim = NULL ;
  f->num = 0 ;
  return f ;
}

struct fprio_t *fprio_destroi (struct fprio_t *f)
{
  struct fpnodo_t *aux ;

  if (!f)
    return NULL ;

  while (f->prim)
  {
    aux = f->prim ;
    f->prim = aux->prox ;
    free (aux->item) ;
    free (aux) ;
  }
  free (f) ;
  return NULL ;
}

int fprio_insere (struct fprio_t *f, void *item, int tipo, int prio)
{
  struct fpnodo_t *novo, *ant, *aux ;

  if (!f || !item)
    return -1 ;

  for (aux = f->prim; aux; aux = aux->prox)
    if (aux->item == item)
      return -1 ;

  novo = malloc (sizeof (struct fpnodo_t)) ;
  if (!novo)
    return -1 ;

  novo->item = item ;
  novo->tipo = tipo ;
  novo->prio = prio ;

  // avanca ate o primeiro de prio maior (fifo nos empates)
  ant = NULL ;
  aux = f->prim ;
  while (aux && aux->prio <= prio)
  {
    ant = aux ;
    aux = aux->prox ;
  }

  novo->prox = aux ;
  if (ant)
    ant->prox = novo ;
  else
    f->prim = novo ;
  if (!aux)
    f->fim = novo ;

  f->num++ ;
  return f->num ;
}

void *fprio_retira (struct fprio_t *f, int *tipo, int *prio)
{
  struct fpnodo_t *aux ;
  void *item ;

  if (!f || !f->prim || !tipo || !prio)
    return NULL ;

  aux = f->prim ;
  item = aux->item ;
  *tipo = aux->tipo ;
  *prio = aux->prio ;
  f->prim = aux->prox ;
  if (!f->prim)
    f->fim = NULL ;
  free (aux) ;
  f->num-- ;
  return item ;
}

int fprio_tamanho (struct fprio_t *f)
{
  if (!f)
    return -1 ;
  return f->num ;
}

void fprio_imprime (struct fprio_t *f)
{
  struct fpnodo_t *aux ;

  if (!f)
    return ;

  for (aux = f->prim; aux; aux = aux->prox)
  {
    printf ("(%d %d)", aux->tipo, aux->prio) ;
    if (aux->prox)
      printf (" ") ;
  }
}
