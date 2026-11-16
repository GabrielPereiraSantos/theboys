// eventos de chegada, espera, entrada, saida e viagem

#ifndef EVENTOS_BASE
#define EVENTOS_BASE

#include "entidades.h"

void ev_chega (struct mundo_t *m, int h, int b) ;
void ev_espera (struct mundo_t *m, int h, int b) ;
void ev_desiste (struct mundo_t *m, int h, int b) ;
void ev_avisa (struct mundo_t *m, int b) ;
void ev_entra (struct mundo_t *m, int h, int b) ;
void ev_sai (struct mundo_t *m, int h, int b) ;
void ev_viaja (struct mundo_t *m, int h, int d) ;

#endif
