#include <stddef.h>
#include "cola.h"
#include "timer.h"
#include "evento.h"
#include "Gestor_Alarmas.h"

//Vector para mantener la integridad de no meter 2 alarmas del mismo tipo de evento
//El indice que se maneja es el id del evento que va de 1 a 8

//Vector cola que sirve para mantener el orden 

static volatile struct evento cola_gestor[8];
static volatile uint8_t periodicas[8];
static volatile uint8_t activada[8];
static volatile uint32_t retardos[8];

/************************
Funcion que inicializa el gestor de alarmas para su posterior funcionamiento.*/
void gestor_alarmas_init(void){
	struct evento inicial;
	for(int i=0;i<8;i++){
		inicial.auxData=0;
		inicial.ID_evento=9;
		cola_gestor[i]=inicial;
		periodicas[i]=0;
		activada[i]=0;
		retardos[i]=0;
	}
}

/************************
Esta funcion introduce un evento si ni hay ningun evento más de este tipo.*/
void gestor_alarmas_introducir_evento(uint32_t estructura_alarma){
	//Extraer informacion del evento Set_Alarm (Se extrae del AUX)
	uint8_t ID_evento = estructura_alarma >> 24; //tipo de evento
	uint8_t esPeriodica=(estructura_alarma >> 23) & 0x01; 
	uint32_t retardo=estructura_alarma & 0x007FFFFF;	//tiempo evento
	//Se crea el evento especifico que ha introducido el Set_Alarm
	struct evento evento_especifico;
	evento_especifico.ID_evento =ID_evento ;
	evento_especifico.instante_evento = retardo;
	//Como necesito volver a usar el retardo en caso de que sea periodica pues 
	//lo guardo en el propio evento en su campo aux y me evito usar otro vector de almacenaje
	evento_especifico.auxData=retardo;
	//Comprobamos si la alarma de ese evento no esta activada 
	if(activada[ID_evento] == 0){		
		cola_gestor[ID_evento] = evento_especifico;
		periodicas[ID_evento] = esPeriodica;
		activada[ID_evento] = 1;
		retardos[ID_evento] = retardo;
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
				cola_guardar_eventos(cola_gestor[i].ID_evento, cola_gestor[i].auxData);
				//Ahora al haber acabado se comprueba si es periodica para eliminarlo o no
				if(periodicas[i] == 0){ //Al no ser periodica se elimina
					activada[i] = 0;
				}else{ //En el caso de ser periodica hay que volver a meter el periodo original en el vector
					retardos[i]=cola_gestor[i].auxData;
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
