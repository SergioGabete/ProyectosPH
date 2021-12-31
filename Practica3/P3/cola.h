#ifndef COLA_H
#define COLA_H


#include <inttypes.h>
//#include "evento.h"



//En esta funci�n se mete la estampilla del timer asi que habra que mirar como funciona para poder poner bien las estampillas temporales
void cola_guardar_eventos(uint8_t ID_evento, uint32_t auxData);

void cola_guardar_eventos_interrupciones(uint8_t ID_evento, uint32_t auxData);

int cola_comprobar_nuevos_eventos(void);

struct evento cola_evento_sin_tratar(void);


#endif

