#include <stddef.h>
#include "cola.h"
#include "timer.h"
#include "evento.h"
#include <LPC210X.H> 
#include "GPIO.h"

struct evento cola_eventos[32];
uint8_t ultimo_evento_guardado = 0; //representa indice ultimo evento guardado
uint8_t ultimo_evento_procesado=0; //posicion ultimo elemento
int overflow = 0;

/************************
Esta funcion guarda los valores de ID_envento y auxData en la cola de evento, además comprueba si se ha llegado 
al overflow y si esto ocurre active el bit 30 de la GPIO.*/
void cola_guardar_eventos(uint8_t ID_evento, uint32_t auxData){
		
		//Si ha llegado a overflow, se marca como salida el pin 30 de la GPIO y se escribe sobre el.
		if(overflow==1){ 
			GPIO_marcar_salida(30,1);
			GPIO_escribir(30,1,1);
			while(1){}
		}
		//Guarda los parametros en sus respectivas variables e incrementa en una el valor de ultimo evento.
		cola_eventos[ultimo_evento_guardado].ID_evento = ID_evento;
		cola_eventos[ultimo_evento_guardado].auxData = auxData;
		cola_eventos[ultimo_evento_guardado].instante_evento = timer1_temporizador_leer();
		ultimo_evento_guardado= (ultimo_evento_guardado + 1) % 32;
		
		//Si el ultimo evento que se ha guardado es el mismo que el que se ha procesado activa la variable overflow
		if(ultimo_evento_guardado == ultimo_evento_procesado){
			overflow=1;
		}
	
	//cola_eventos[ultimo_evento_guardado] = nuevo;
	//ultimo_evento_guardado = (ultimo_evento_guardado+1)%32;
			
}

/************************
Esta función devuelve un 0 si no tiene ningun evento para procesar , y devuelve un 1 en caso contrario..*/
int cola_comprobar_nuevos_eventos(void){
	if(ultimo_evento_procesado == ultimo_evento_guardado){// no tiene nuevos eventos para tratar
			return 0;
	}
	else{
		return 1;
	}
}

/************************
Esta funcion devuelve el ultimo envento sin tratar  de la cola de eventos..*/
struct evento cola_evento_sin_tratar(void){
		
		struct evento antiguo;
		antiguo.ID_evento=cola_eventos[ultimo_evento_procesado].ID_evento;
		antiguo.auxData= cola_eventos[ultimo_evento_procesado].auxData;
	  antiguo.instante_evento = cola_eventos[ultimo_evento_procesado].instante_evento;
		ultimo_evento_procesado = (ultimo_evento_procesado + 1) % 32;
		
		overflow = 0;
		
		return antiguo;
		
}

