#include "UART0.h"
#include <LPC210X.H>   
#include "cola.h"
#include "evento.h"
#include "Gestor_Alarmas.h"

static volatile int buffer_entrada[10];
static volatile int indice=-1;
//static volatile int ultimo=0;

void uart0_ISR (void) __irq; 


void uart0_init(){
	PINSEL0 = PINSEL0 | 0x00000005;                  /* Enable RxD0 and TxD0              */
  U0LCR = 0x83;                          /* 8 bits, no Parity, 1 Stop bit     */
  U0DLL = 97;                            /* 9600 Baud Rate @ 15MHz VPB Clock  */
  U0LCR = 0x03;                          /* DLAB = 0                          */
	
	//Aqui ira lo del vic
	VICVectAddr7 = (unsigned long)uart0_ISR;
	VICVectCntl7 = 0x20|6;
	VICIntEnable = VICIntEnable | (1<<6);
	U0IER = 0x3;
	
}
//Recordar poner lo de reiniciar power down pero hay que preguntarle a Enrique
void uart0_ISR (void) __irq {
	
		if ((U0IIR&0x4)&&(U0LSR & 0x01)){		//((U0IIR & 0x4) == 0x4)&&((U0LSR & 0x01) == 0x1)
				gestor_alarmas_resetear_power_down();
				int ultimo = U0RBR;
				U0THR = ultimo; //mostrar lo escrito	
				//Se trata el caracter recibido y se encola el tipo de accion
				if(ultimo == '#'){
						indice =0;
				}					//Si es el comando que indica empiece de comando se empieza a leer comando
				if(indice != -1){
					buffer_entrada[indice] = ultimo;
					indice = indice+1;
					if(ultimo == '!'){	//Si el caracter es ultimo se resetea el indice
						if((buffer_entrada[0] == '#')&&(buffer_entrada[1] == 'R')&&(buffer_entrada[2] == 'S')&&(buffer_entrada[3] == 'T') && (buffer_entrada[4] == '!')){	//RST
							cola_guardar_eventos(evento_rst,0);
						}
						if((buffer_entrada[0] == '#')&&(buffer_entrada[1] == 'N')&&(buffer_entrada[2] == 'E')&&(buffer_entrada[3] == 'W') && (buffer_entrada[4] == '!')){	//NEW
							cola_guardar_eventos(evento_new,0);
						}
//						else{
//							uart0_sendchar('E');uart0_sendchar('R');uart0_sendchar('R');
//						}
						if((buffer_entrada[0] == '#')&&
							(buffer_entrada[1] - '0' >=0 && buffer_entrada[1] - '0' <= 8)&&	//Si son numeros validos pues se introduce
							(buffer_entrada[2] - '0' >=0 && buffer_entrada[2] - '0' <= 8)&&
							(buffer_entrada[3] - '0' >=0 && buffer_entrada[3] - '0' <= 8)&&
							(buffer_entrada[5] == '!') &&
								((buffer_entrada[1] - '0' + buffer_entrada[2] - '0' + buffer_entrada[3] - '0') % 8 ==  buffer_entrada[4] - '0')){ //Jugada correcta
								//Como guardar informacion en el auxiliar
								//00000000 fila columna valor
									uint32_t auxiliar = 0;
									uint32_t fila =buffer_entrada[1] - '0';
									uint32_t columna =buffer_entrada[2] - '0';
									uint32_t valor =buffer_entrada[3] - '0';
									auxiliar = auxiliar | valor;
									auxiliar = auxiliar | (columna << 8);
									auxiliar = auxiliar | (fila << 16);
									cola_guardar_eventos(evento_jugada,auxiliar);
						}
						indice = -1;
					}
				}
				
		}
		if ((U0IIR&0x1)){	//	(U0IIR & 0x1) == 0x1	//
				//Esperar respuesta de resano
				cola_guardar_eventos(evento_continuar_mensaje,0);	//En el planificador se llama a sudoku enviar mensaje
				
		}
		
		VICVectAddr = 0;
		
		//APUNTE: cuando se entra en power down y vuelves a normal no se puede volver a escribir en la UART
}

int uart0_sendchar(int ch){
	return (U0THR = ch);		//U0THR hace que se muestre en la UART0, se escribe y sirve de salida
}

