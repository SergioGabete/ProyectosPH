#ifndef GESTOR_PULSACION_H
#define GESTOR_PULSACION_H
#include "pulsacion.h"
#include "evento.h"


enum {
	PULSADO = 0,
	NO_PULSADO = 1
};

void eint_init(void); 
	
unsigned int nueva_pulsacion_0(void);
void clear_nueva_pulsacion_0(void);
void actualizar_estado_0(void);
unsigned int leer_estado_0(void);

unsigned int nueva_pulsacion_1(void);
void clear_nueva_pulsacion_1(void);
void actualizar_estado_1(void);
unsigned int leer_estado_1(void);
//int gestor_pulsacion(int entrada);
#endif
