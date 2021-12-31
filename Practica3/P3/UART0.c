#include "UART0.h"
#include <LPC210X.H>   
#include "cola.h"
#include "evento.h"
#include "Gestor_Alarmas.h"

static volatile int buffer_entrada[10];//buffer de entrada que sirve para guardar lo que el usuario escribe en la Uart
static volatile int indice=-1;
//static volatile int ultimo=0;

void uart0_ISR (void) __irq; 

/************************
Esta funcion inicializa la uart, en primer lugar, configuramos los puertos series,
posteriormente,tratamos el ViCVector y finalmente activamos las interrupciones de RDA Y THRE.*/
void uart0_init(){
	PINSEL0 = PINSEL0 | 0x00000005;                  /* Enable RxD0 and TxD0              */
  U0LCR = 0x83;                          /* 8 bits, no Parity, 1 Stop bit     */
  U0DLL = 200;                            /* 9600 Baud Rate @ 15MHz VPB Clock  */
  U0LCR = 0x03;                          /* DLAB = 0                          */
	
	//Aqui ira lo del vic
	VICVectAddr7 = (unsigned long)uart0_ISR;
	VICVectCntl7 = 0x20|6;
	VICIntEnable = VICIntEnable | (1<<6);
	U0IER = 0x3;					//activamos las interrupciones de RDA Y THRE
	
}
/************************
Esta funcion es la rutina de tratamiento de la interrupción de UART0.
Se va a detectar que evento escribe el usuario por pantalla por medio de un buffer, 
dependiendo el tipo de evento que se escribe se guardará en cola el evento descrito.*/
void uart0_ISR (void) __irq {
		if ((U0IIR&0x4)&&(U0LSR & 0x01)){
				int ultimo = U0RBR;
				U0THR = ultimo; //mostrar los caracteres del comando que se esta escribiendo	
				//Se trata el caracter recibido y se encola el tipo de accion
				cola_guardar_eventos_interrupciones(evento_letra_introducida,ultimo);
				
		}
		//Cuando ha terminado de enviar el carácter anterior, genera un evento
		//evento_continuar_mensaje, avisando al planificador que el THR ya está libre y puede
		//utilizar sendchar(int ch) para enviar otro carácter.
		if ((U0IIR&0x1)){	//	(U0IIR & 0x1) == 0x1	//
				cola_guardar_eventos_interrupciones(evento_continuar_mensaje,0);	//En el planificador se llama a sudoku enviar mensaje	
		}
		VICVectAddr = 0;
}

/************************
Esta funcion envia un caracter ch al puerto serie.*/
int uart0_sendchar(int ch){
	return (U0THR = ch);		//U0THR hace que se muestre en la UART0, se escribe y sirve de salida
}

