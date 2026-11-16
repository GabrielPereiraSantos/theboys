// eventos de invasao, recuperacao e reconstrucao

#ifndef EVENTOS_INVASAO
#define EVENTOS_INVASAO

#include "entidades.h"

void ev_invade (struct mundo_t *m, int id, int b) ;
void ev_recupera (struct mundo_t *m, int h) ;
void ev_reconstroi (struct mundo_t *m, int b) ;

#endif
