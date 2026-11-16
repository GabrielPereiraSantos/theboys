// eventos de missao e morte

#ifndef EVENTOS_MISSAO
#define EVENTOS_MISSAO

#include "entidades.h"

void ev_missao (struct mundo_t *m, int id) ;
void ev_morre (struct mundo_t *m, int h, int b, int missao) ;

#endif
