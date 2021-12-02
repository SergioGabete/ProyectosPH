#ifndef PLANIFICADOR_h
#define PLANIFICADOR_h
#include "evento.h"
#include "Gestor_Alarmas.h"
#include "Gestor_IO.h"
#include "pw_id_control.h"
#include "celda.h"
#include "sudoku_2021.h"
#include "pulsacion.h"
#include "Gestor_pulsacion.h"
#include "timer.h"

void planificador_tratar_evento(struct evento evento_sin_tratar);

int planificador_parar(void);
void planificador_retormar_ejecucion(void);

void planificador_init(void);

void planificador_reiniciar(void);

#endif

