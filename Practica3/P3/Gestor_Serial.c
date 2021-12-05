#include "UART0.h"
#include <LPC210X.H>   
#include "Gestor_Serial.h"


void gestor_serial_escribir_linea(int ch[19][109],int numFilas, int numColumnas){
	for(int i=0;i<numColumnas;i++){
		for(int j=0;j<numColumnas;j++){
			uart0_sendchar(ch[i][j]);
		}
	}
	uart0_sendchar('\n');
}

