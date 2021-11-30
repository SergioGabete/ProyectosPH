
#ifndef GESTOR_ALARMAS_H
#define GESTOR_ALARMAS_H


#include <inttypes.h>
#include "evento.h"

//Dos vectores uno para mantener la integridad de no meter

void gestor_alarmas_init(void);

void gestor_alarmas_introducir_evento(struct evento evento_introducir);

void gestor_alarmas_restar_periodo(void);

void gestor_alarmas_quitar_alarma(uint8_t id);

void gestor_alarmas_resetear_power_down(void);

#endif

