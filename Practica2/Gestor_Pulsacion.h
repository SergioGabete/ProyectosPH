#ifndef GESTOR_PULSACION_H
#define GESTOR_PULSACION_H
#include "pulsacion.h"
#include "evento.h"


enum {
	PULSADO = 0,
	NO_PULSADO = 1
};

void gestor_pulsacion_eint_init(void); 
	
unsigned int gestor_pulsacion_nueva_pulsacion_0(void);
void gestor_pulsacion_clear_nueva_pulsacion_0(void);
void gestor_pulsacion_actualizar_estado_0(void);
unsigned int gestor_pulsacion_leer_estado_0(void);

unsigned int gestor_pulsacion_nueva_pulsacion_1(void);
void gestor_pulsacion_clear_nueva_pulsacion_1(void);
void gestor_pulsacion_actualizar_estado_1(void);
unsigned int gestor_pulsacion_leer_estado_1(void);

void gestor_pulsacion_boton1_pretado(void);
void gestor_pulsacion_boton2_pretado(void);

void gestor_pulsacion_alarma_boton1(void);
void gestor_pulsacion_alarma_boton2(void);
//int gestor_pulsacion(int entrada);
#endif
