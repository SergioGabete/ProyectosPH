#include <LPC210X.H>           
#include "timer.h"
#include "evento.h"
#include "cola.h"

/**************************************
Variables que cuentan el numero de veces que ha alcanzado el maximo sus respectivos contadores*/
static volatile unsigned long timer0_int_count = 0;

static volatile unsigned long timer1_int_count = 0;

//Variable que controla el periodo de las alarmas
static volatile unsigned int periodo_global = 0;

static volatile unsigned long timer1_swi_lectura = 0;

void timer0_ISR (void) __irq;   // Generate Interrupt 

void timer1_ISR (void) __irq;    // Generate Interrupt 
 
/************************
Funcion que inicializa el timer 0*/
void timer0_temporizador_iniciar(void){
		timer0_int_count = 0;	
	// configuration of Timer 0
		T0MR0 = (unsigned long)14999.9;                        // Esto se puede cambiar luego 149999
	//este se pone asi porque con este timer nos interesa poder poner alarmas asi no
	//se interrumpen MR0 y MR1
    T0MCR = 3;                     // Generates an interrupt and resets the count when the value of MR0 is reached
    T0TCR = 0;                             // Timer0 Enable Lo pones a 0 para que este quieto hasta que lo empecemos
    // configuration of the IRQ slot number 0 of the VIC for Timer 0 Interrupt
		VICVectAddr0 = (unsigned long)timer0_ISR;          // set interrupt vector in 0
    // 0x20 bit 5 enables vectored IRQs. 
		// 4 is the number of the interrupt assigned. Number 4 is the Timer 0 (see table 40 of the LPC2105 user manual  
		VICVectCntl0 = 0x20 | 4;                   
    VICIntEnable = VICIntEnable | 0x00000010; 	// Enable Timer0 Interrupt
		VICIntSelect = VICIntSelect | 0x10;
}

/************************
Funcion que pone ne marcha el timer0*/
void timer0_temporizador_empezar(void){
	//T0TCR = 1;							//Timer0 Enable y lo reseteas
	T0TCR = 3; // resetea la cuenta
	T0TCR = 1; // empieza el contador
}


/************************
Funcion que devuelve el tiempo que ha contado el timer0 hasta el momento*/
unsigned long timer0_temporizador_leer(void){
	return timer0_int_count*0xffffffff/60+T0TC/60;
	//timer0_int_count no va multiplicado por nada porque usamos 1 milisegundo
	//Devuelve el numero de tics*milisegundos que cuenta cada tic + lo que lleva el timer0
}

/************************
Funcion que para el timer0 y devuelve lo que lleva contado hasta el momento*/
unsigned long timer0_temporizador_parar(void){
	double t0_tiempo = timer0_temporizador_leer();
	T0TCR = 0;
	//Hay que resetear tambien T0MR0 
	return t0_tiempo;
}

//Como se va a utilizar T0MR1 habra que añadir cosas para que no se reseten cosas que no queremos
void timer0_temporizador_periodico(unsigned int periodo){
	//Como el periodo es 1 milisegundo siempre, siempre encolaremos esto en la rutina de interrupcion
	
	//cola_guardar_eventos(resta_Periodos,0);
	periodo_global = periodo;
	
}

/************************
Rutina de servicio de interrupcion del timer0*/
void timer0_ISR (void) __irq {
			timer0_int_count++;
			//timer0_int_count_periodos++;
			/*if((timer0_int_count_periodos % periodo_global == 0) && (timer0_int_count_periodos >= 0)){ //Se ha alcanzado el temporizador por tanto se resetean los conteos
				timer0_int_count_periodos = 0;
				cola_guardar_eventos(alarma_timer0,0);
			}*/
			if(periodo_global != 0){	//Se encola el evento que resta 1 a los temporizadores de las alarmas
				cola_guardar_eventos(resta_Periodos,0);
			}
			//timer0_temporizador_periodico(1);
			T0IR = 1;                            // Acknowledge Interrupt
			VICVectAddr = 0;
	 
}


//////////////////////////////////////
//Configuracion del timer1
///////////////////////////////////

/************************
Funcion que inicializa el timer1*/
void timer1_temporizador_iniciar(void){
		timer1_int_count = 0;	
	// configuration of Timer 0
		T1MR0 = 0xfffffffe;                        // Esto se puede cambiar luego 149999
	//este se pone asi porque con este timer nos interesa poder poner alarmas asi no
	//se interrumpen MR0 y MR1
    T1MCR = 3;                     // Generates an interrupt and resets the count when the value of MR0 is reached
    T1TCR = 0;                             // Timer0 Enable Lo pones a 0 para que este quieto hasta que lo empecemos
    // configuration of the IRQ slot number 0 of the VIC for Timer 0 Interrupt
		VICVectAddr1 = (unsigned long)timer1_ISR;          // set interrupt vector in 0
    // 0x20 bit 5 enables vectored IRQs. 
		// 4 is the number of the interrupt assigned. Number 4 is the Timer 0 (see table 40 of the LPC2105 user manual 
		//Estas dos lineas a lo mejor hay que cambiarlas Revisralas
		VICVectCntl1 = 0x20 | 5;                   
    VICIntEnable = VICIntEnable | 0x00000020;                 
}

/************************
Funcion que pone ne marcha el timer1*/
void timer1_temporizador_empezar(void){
	//T0TCR = 1;							//Timer0 Enable y lo reseteas
	T1TCR = 3; // resetea la cuenta
	T1TCR = 1; // empieza el contador
}

/************************
Funcion que devuelve el tiempo que ha contado el timer1 hasta el momento*/
unsigned long timer1_temporizador_leer(void){
	return timer1_int_count*0xffffffff/60+T1TC/60;
	//Devuelve el tiempo que llevaba en milisegundos
}

/************************
Funcion que para el timer1 y devuelve lo que lleva contado hasta el momento*/
unsigned long timer1_temporizador_parar(void){
	//double t1_tiempo = (double)timer1_int_count + T1TC*(1 / 14999.9);
	unsigned long t1_tiempo = timer1_temporizador_leer();
	T1TCR = 0;
	//Hay que resetear tambien T0MR0 ? o al poner a 0 el enable del T0TC 
	return t1_tiempo;
}


/************************
Rutina de servicio de interrupcion del timer1*/
void timer1_ISR (void) __irq {
		  timer1_int_count++;
			T1IR = 1;                            // Acknowledge Interrupt
			VICVectAddr = 1;
	 
}


uint32_t __swi(0) clock_gettime(void);
uint32_t __SWI_0 (void) {
	//timer1_swi_lectura = timer1_temporizador_leer();
	return timer1_int_count*0xffffffff/60+T1TC/60;
}




