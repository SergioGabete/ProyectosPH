#ifndef EVENTO_H
#define EVENTO_H

#include <inttypes.h>

//struct que representa un evento
struct evento{
	double instante_evento;	//instante en el que ocurrio el evento
	uint8_t ID_evento;	//identificador para los distintos tipos de eventos
	uint32_t auxData;	//informacion auxiliar
};

enum IDs{
	evento_boton1, //id0
	evento_boton2, //id1
	evento_power_down, //id2
	resta_Periodos, //id3
	evento_alarma_pulsaciones_1,	//id4
	evento_alarma_pulsaciones_2, //id5
	evento_visualizacion_GPIO,	//id6
	evento_visualizar_led,		//id7
	Set_Alarm,			//id8
};


#endif

