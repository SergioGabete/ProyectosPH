#include "UART0.h"
#include <LPC210X.H> 
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Gestor_Serial.h"


static int indice_mensaje =0, num_caracteres =0;	//indice del caracter y numero de caracteres totales de lo que queremos escribir
static char* mensaje;

/************************************
Esta función continua escribiendo el mensaje que queremos escribir por la UART, para ello
preguntamos si el indice_mensaje es menor que el numero de caracteres, si es así le decimos que escriba en indice e
lo incrementamos una unidad, en caso contrario, pondremos el numero de caracteres a cero y el puntero a mensaje a NULL*/
void gestor_serial_continuar_mensaje(){
			if(indice_mensaje<num_caracteres){
				uart0_sendchar(mensaje[indice_mensaje]);
				indice_mensaje = indice_mensaje +1;
			}else if(num_caracteres!=0){
				num_caracteres=0;
				mensaje=NULL;
				//cola_guardar_eventos(evento_fin_mensaje,0);
			}	
}

/*********************************
Esta función empeiza a escribir el mensaje que queremos por la UART,primero resetea la variable indice_mensaje, 
posteriormente comprueba el numero de caracteres y finalmente llama a la función de la uart que escribe.*/
void gestor_serial_enviar_mensaje(char msg[]){
			indice_mensaje=0;
			num_caracteres=strlen(msg);
			mensaje=msg;
			uart0_sendchar('\n');		
}

