// lista de eventos futuros

#ifndef LEF
#define LEF

#include "entidades.h"

void agenda (struct mundo_t *m, int tempo, int tipo, int heroi, int base,
             int missao) ;

// tipo -1 tira todos os eventos do heroi
void remove_eventos_heroi (struct mundo_t *m, int h, int tipo) ;

#endif
