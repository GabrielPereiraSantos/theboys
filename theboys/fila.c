// TAD fila generica
// Autor: Gabriel Pereira dos Santos, GRR 20263239

#include <stdio.h>
#include <stdlib.h>
#include "fila.h"

struct fila_t *fila_cria ()
{
  struct fila_t *f = malloc (sizeof (struct fila_t)) ;
  if (!f)
    return NULL ;

  f->prim = NULL ;
  f->fim = NULL ;
  f->num = 0 ;
  return f ;
}

struct fila_t *fila_destroi (struct fila_t *f)
{
  struct fila_nodo_t *aux ;

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

int fila_insere (struct fila_t *f, void *item)
{
  struct fila_nodo_t *novo, *aux ;

  if (!f || !item)
    return -1 ;

  // mesmo ponteiro duas vezes e erro
  for (aux = f->prim; aux; aux = aux->prox)
    if (aux->item == item)
      return -1 ;

  novo = malloc (sizeof (struct fila_nodo_t)) ;
  if (!novo)
    return -1 ;

  novo->item = item ;
  novo->prox = NULL ;
  if (f->fim)
    f->fim->prox = novo ;
  else
    f->prim = novo ;
  f->fim = novo ;
  f->num++ ;
  return f->num ;
}

void *fila_retira (struct fila_t *f)
{
  struct fila_nodo_t *aux ;
  void *item ;

  if (!f || !f->prim)
    return NULL ;

  aux = f->prim ;
  item = aux->item ;
  f->prim = aux->prox ;
  if (!f->prim)
    f->fim = NULL ;
  free (aux) ;
  f->num-- ;
  return item ;
}

int fila_tamanho (struct fila_t *f)
{
  if (!f)
    return -1 ;
  return f->num ;
}

// itens tratados como int*, formato "a b c"
void fila_imprime (struct fila_t *f)
{
  struct fila_nodo_t *aux ;

  if (!f)
    return ;

  for (aux = f->prim; aux; aux = aux->prox)
  {
    printf ("%d", *(int *) aux->item) ;
    if (aux->prox)
      printf (" ") ;
  }
}
