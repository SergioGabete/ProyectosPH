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
	evento_idle, //id0
	evento_confirmar_jugada, //id1
	evento_power_down, //id2
	resta_Periodos, //id3
	evento_alarma_pulsaciones_1,	//id4
	evento_alarma_pulsaciones_2, //id5
	evento_visualizacion_GPIO,	//id6
	evento_visualizar_led,		//id7
	Set_Alarm,			//id8
	evento_boton1,	//id 9
	evento_rst,		//10
	evento_new,		//11
	evento_jugada,		//12
	evento_continuar_mensaje,	//13
	evento_reset_power_down,	//14
	evento_fin_partida,			//15
	evento_boton2,			//16
	evento_letra_introducida,	//17
};


#endif

