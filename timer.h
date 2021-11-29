#ifndef TIMER_H
#define TIMER_H

#include <inttypes.h>

void timer1_temporizador_iniciar(void);
void timer1_temporizador_empezar(void);
double timer1_temporizador_leer(void);
double timer1_temporizador_parar(void);

void timer0_temporizador_iniciar(void);
void timer0_temporizador_empezar(void);
double timer0_temporizador_leer(void);
double timer0_temporizador_parar(void);
void timer0_temporizador_periodico(unsigned int periodo);



#endif


