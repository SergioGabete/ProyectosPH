#ifndef TIMER_H
#define TIMER_H

#include <inttypes.h>

void timer1_temporizador_iniciar(void);
void timer1_temporizador_empezar(void);
unsigned int timer1_temporizador_leer(void);
unsigned int timer1_temporizador_parar(void);

void timer0_temporizador_iniciar(void);
void timer0_temporizador_empezar(void);
unsigned int timer0_temporizador_leer(void);
unsigned int timer0_temporizador_parar(void);
void timer0_temporizador_periodico(unsigned int periodo);



#endif


