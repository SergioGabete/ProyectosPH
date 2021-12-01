#include <stddef.h>
#include "cola.h"
#include "timer.h"
#include "evento.h"
#include "Gestor_Alarmas.h"

//Vector para mantener la integridad de no meter 2 alarmas del mismo tipo de evento
//El indice que se maneja es el id del evento que va de 1 a 8

//Vector cola que sirve para mantener el orden 

static volatile uint8_t ID[8];//Conocer informacion de los eventos
static volatile uint8_t periodicas[8];
static volatile uint8_t activada[8];
static volatile uint32_t retardos[8]; //cuanto le falta a cada una de las alarmas
static volatile uint32_t retardos_alarma[8];

/************************
Funcion que inicializa el gestor de alarmas para su posterior funcionamiento.*/
void gestor_alarmas_init(void){
	for(int i=0;i<8;i++){
		periodicas[i]=0;
		activada[i]=0;
		retardos[i]=0;
		retardos_alarma[i]=0;
	}
}

/************************
Esta funcion introduce un evento si ni hay ningun evento más de este tipo.*/
void gestor_alarmas_introducir_evento(uint32_t estructura_alarma){
	//Extraer informacion del evento Set_Alarm (Se extrae del AUX)
	uint8_t ID_evento = estructura_alarma >> 24; //tipo de evento
	uint8_t esPeriodica=(estructura_alarma >> 23) & 0x01; 
	uint32_t retardo=estructura_alarma & 0x007FFFFF;	//tiempo evento
	
	
	if(activada[ID_evento] == 0){		
		ID[ID_evento] = ID_evento;
		periodicas[ID_evento] = esPeriodica;
		activada[ID_evento] = 1;
		retardos[ID_evento] = retardo;
		retardos_alarma[ID_evento] = retardo;
	} 
}
/************************
Funcion que resta 1 a todos los periodos de los eventos activos*/
void gestor_alarmas_restar_periodo(void){
	for(int i=0;i<8;i++){
		if(activada[i] == 1){	//Comprobar si la alarma esta activada
			retardos[i] = retardos[i] - 1;
			//Si el retardo es 0 pues lo encolas porque ha acabado la alarma
			if(retardos[i] == 0){
				//Nos daria igual el valor de AuxData ????
				cola_guardar_eventos(ID[i], 0);
				//Ahora al haber acabado se comprueba si es periodica para eliminarlo o no
				if(periodicas[i] == 0){ //Al no ser periodica se elimina
					activada[i] = 0;
				}else{ //En el caso de ser periodica hay que volver a meter el periodo original en el vector
					retardos[i]=retardos_alarma[i];	
				}
			}
		}
	}
}

/************************
Esta funcion procede a quitar una alarma que tiene como identicador el id que le pasamos como parametro..*/
void gestor_alarmas_quitar_alarma(uint8_t id){
	activada[id]=0;
}

void gestor_alarmas_resetear_power_down(){
	gestor_alarmas_quitar_alarma(2);							//Preguntar si esto de verdad va dentro del gestor de alarmas
	cola_guardar_eventos(Set_Alarm,0x02003A98);
}
