#ifndef PULSACION_H
#define PULSACION_H
#include "cola.h"
#include "evento.h"

//unsigned int nueva_pulsacion_0(void);
//void clear_nueva_pulsacion_0(void);
//void actualizar_estado_0(void);
//unsigned int leer_estado_0(void);

void eint1_init(void);
int button_nueva_pulsacion_1(void); //Devuelve la variable que indica si se ha pulsado el boton correspondiente
void button_clear_nueva_pulsacion_1(void); //Resetea el indidicador de que ha habido una pulsacion
unsigned int eint1_read_count(void);


void eint2_init(void);
int button_nueva_pulsacion_2(void);
void button_clear_nueva_pulsacion_2(void);
unsigned int eint2_read_count(void);


#endif /* EVENTO_H */
