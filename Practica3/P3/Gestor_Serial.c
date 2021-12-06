#include "UART0.h"
#include <LPC210X.H> 
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "Gestor_Serial.h"


void gestor_serial_escribir_linea(int ch[19][109],int numFilas, int numColumnas){
	for(int i=0;i<numColumnas;i++){
		for(int j=0;j<numColumnas;j++){
			uart0_sendchar(ch[i][j]);
		}
	}
	uart0_sendchar('\n');
}

void gestor_serial_introducirmensaje(char informacion[]){
			int longitud = strlen(informacion);
			for(int i =0 ; i < longitud; i++){
				uart0_sendchar(informacion[i]);
			}
}