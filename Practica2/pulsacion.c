#include "pulsacion.h"
#include <LPC210x.H> /* LPC210x definitions */

//Dichas variables serán 1 si el boton esta pulsado y 0 en caso contrario
static volatile int eint1_pulsacion = 0;
static volatile int eint2_pulsacion = 0;

// variable para comprobar que se hacen las interrupciones que deber?an hacerse
static volatile unsigned int eint1_count = 0;
static volatile unsigned int eint2_count = 0;


void eint1_ISR (void) __irq;   // Generate Interrupt 

void eint2_ISR (void) __irq;    // Generate Interrupt 

/************************
Esta funcion incrementa el numero de interrupciones de eint_1, además deshabilita las interrupciones, guarda un evento
de boton 1 en la cola y nos indica que dicho botón esta siendo pulsado. */
void eint1_ISR (void) __irq{
	eint1_count++;
	VICVectAddr = 2;		
	EXTINT |= 0x2;			//Resetea la interrupcion
	eint1_pulsacion = 1;
	VICIntEnClr = 0x00008000;		//Deshabilitar la interrupcion de eint1
	cola_guardar_eventos(evento_boton1,0);//Guarda un evento de boton 1 en la cola de eventos
}

/************************
Esta funcion incrementa el numero de interrupciones de eint_2, además deshabilita las interrupciones, guarda un evento
de boton 2 en la cola y nos indica que dicho botón esta siendo pulsado. */
void eint2_ISR (void) __irq{
	eint2_count++;
	VICVectAddr = 0;		
	EXTINT |= 0x4;			//Resetea la interrupcion
	eint2_pulsacion = 1;
	VICIntEnClr = 0x00010000;		//Deshabilitar la interrupcion de eint2
	cola_guardar_eventos(evento_boton2,0);//Guarda un evento de boton 2 en la cola de eventos
}

/************************
Esta funcion inicializa eint_1. */
void eint1_init(void){
	//Valores iniciales de eint_1
	eint1_pulsacion = 0;
	eint1_count = 0;
	
	EXTINT =  EXTINT | 2;        // clear interrupt flag     	
	// configuration of the IRQ slot number 2 of the VIC for EXTINT0
	VICVectAddr2 = (unsigned long)eint1_ISR;          // set interrupt vector in 0
    // 0x20 bit 5 enables vectored IRQs. 
		// 14 is the number of the interrupt assigned. Number 14 is the EINT0 (see table 40 of the LPC2105 user manual  
	PINSEL0 		= PINSEL0 & 0xcfffffff;	//Poner los bits 28-29 a 0-0 para resetear ese pin
	PINSEL0 		= PINSEL0 | 0x20000000;					//Enable the EXTINT1 interrupt. Pones los 28-29 a 1-0 (eso es eint1)
	
	VICVectCntl2 = 0x20 | 15;                   
  VICIntEnable = VICIntEnable | 32768;                  // Enable eint1 Interrupt
}

/************************
Esta funcion devulve el valor de eint1_pulsacion. */
int button_nueva_pulsacion_1(void){
	return eint1_pulsacion;
}

/************************
Esta función limpia la pulsacion del boton 1 , es decir , cuando eint_1 se deje de pulsar es necesaria dicha función. */
void button_clear_nueva_pulsacion_1(void){
	eint1_pulsacion = 0;
}
/************************
 Esta función devuelve el número de interrupciones de eint_1 que deben hacerse*/
unsigned int eint1_read_count(void){
	return eint1_count;
}

/************************
Esta funcion inicializa eint_2. */
void eint2_init(void){
	//valores iniciales de eint_2
	eint2_pulsacion = 0;
	eint2_count = 0;
	
	EXTINT =  EXTINT | 4;        // clear interrupt flag    porque son los bits 2-1-0 = eint2-eint1-eint0 	
	// configuration of the IRQ slot number 2 of the VIC for EXTINT0
	VICVectAddr3 = (unsigned long)eint2_ISR;          // set interrupt vector in 0
    // 0x20 bit 5 enables vectored IRQs. 
		// 14 is the number of the interrupt assigned. Number 14 is the EINT0 (see table 40 of the LPC2105 user manual  
	PINSEL0 		= PINSEL0 & 0x3fffffff;	//Poner los bits 28-29 a 0-0 para resetear ese pin
	PINSEL0 		= PINSEL0 | 0x80000000;					//Enable the EXTINT1 interrupt. Pones los 28-29 a 1-0 (eso es eint1)
	
	VICVectCntl3 = 0x20 | 16;                   
  VICIntEnable = VICIntEnable | 65536;                  // Enable eint1 Interrupt
}

/************************
Esta funcion devulve el valor de eint2_pulsacion. */
int button_nueva_pulsacion_2(void){
	return eint2_pulsacion;
}

/************************
Esta función limpia la pulsacion del boton 2 , es decir , cuando eint_1 se deje de pulsar es necesaria dicha función. */
void button_clear_nueva_pulsacion_2(void){
	eint2_pulsacion = 0;
}

/************************
 Esta función devuelve el número de interrupciones de eint_2 que deben hacerse*/
unsigned int eint2_read_count(void){
	return eint2_count;
}

