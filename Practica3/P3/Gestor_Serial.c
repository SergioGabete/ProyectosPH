#include "UART0.h"
#include <LPC210X.H> 
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Gestor_Serial.h"


static int indice_mensaje =0, num_caracteres =0;
static char* mensaje;
//static char mensajeFinal[2000];

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

void gestor_serial_enviar_mensaje(char msg[]){
			indice_mensaje=0;
			num_caracteres=strlen(msg);
			mensaje=msg;
			uart0_sendchar('\n');			//Preguntar a enrique si se puede invocar sendchar desde aqui
}

