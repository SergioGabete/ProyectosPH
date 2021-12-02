
#ifndef GESTOR_ALARMAS_H
#define GESTOR_ALARMAS_H


#include <inttypes.h>
#include "evento.h"

//Dos vectores uno para mantener la integridad de no meter

void gestor_alarmas_init(void);

void gestor_alarmas_introducir_evento(uint32_t evento);

void gestor_alarmas_restar_periodo(void);

void gestor_alarmas_quitar_alarma(uint8_t id);

void gestor_alarmas_resetear_power_down(void);

void gestor_alarmas_activar_alarma(uint8_t id);

#endif

