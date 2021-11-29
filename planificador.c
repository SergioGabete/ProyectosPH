#include "planificador.h"

static CELDA
cuadricula_C_C[NUM_FILAS][NUM_COLUMNAS] =
{
0x0015, 0x0000, 0x0000, 0x0013, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
0x0000, 0x0000, 0x0000, 0x0000, 0x0019, 0x0000, 0x0000, 0x0000, 0x0015, 0, 0, 0, 0, 0, 0, 0,
0x0000, 0x0019, 0x0016, 0x0017, 0x0000, 0x0015, 0x0000, 0x0013, 0x0000, 0, 0, 0, 0, 0, 0, 0,
0x0000, 0x0018, 0x0000, 0x0019, 0x0000, 0x0000, 0x0016, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
0x0000, 0x0000, 0x0015, 0x0018, 0x0016, 0x0011, 0x0014, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
0x0000, 0x0000, 0x0014, 0x0012, 0x0000, 0x0013, 0x0000, 0x0017, 0x0000, 0, 0, 0, 0, 0, 0, 0,
0x0000, 0x0017, 0x0000, 0x0015, 0x0000, 0x0019, 0x0012, 0x0016, 0x0000, 0, 0, 0, 0, 0, 0, 0,
0x0016, 0x0000, 0x0000, 0x0000, 0x0018, 0x0000, 0x0000, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0012, 0x0000, 0x0000, 0x0011, 0, 0, 0, 0, 0, 0, 0
};

static CELDA
cuadricula_C_C_Aux[NUM_FILAS][NUM_COLUMNAS] =
{
0x0015, 0x0000, 0x0000, 0x0013, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
0x0000, 0x0000, 0x0000, 0x0000, 0x0019, 0x0000, 0x0000, 0x0000, 0x0015, 0, 0, 0, 0, 0, 0, 0,
0x0000, 0x0019, 0x0016, 0x0017, 0x0000, 0x0015, 0x0000, 0x0013, 0x0000, 0, 0, 0, 0, 0, 0, 0,
0x0000, 0x0018, 0x0000, 0x0019, 0x0000, 0x0000, 0x0016, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
0x0000, 0x0000, 0x0015, 0x0018, 0x0016, 0x0011, 0x0014, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
0x0000, 0x0000, 0x0014, 0x0012, 0x0000, 0x0013, 0x0000, 0x0017, 0x0000, 0, 0, 0, 0, 0, 0, 0,
0x0000, 0x0017, 0x0000, 0x0015, 0x0000, 0x0019, 0x0012, 0x0016, 0x0000, 0, 0, 0, 0, 0, 0, 0,
0x0016, 0x0000, 0x0000, 0x0000, 0x0018, 0x0000, 0x0000, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0012, 0x0000, 0x0000, 0x0011, 0, 0, 0, 0, 0, 0, 0
};


/****************************************
*	La funcion realiza una accion dependiendo del evento que le llegue al planificador*/
void planificador_tratar_evento(struct evento evento_sin_tratar){
	int parar;
	uint32_t estado_GPIO=0;
	double tiempoProcesado=0;
	uint32_t configuracionVicEnable;	//Variable que alamacena la configuracion del registro VICIntEnable
	uint32_t configuracionVicClr;			//Variable que alamacena la configuracion del registro VICIntEnClr
	if(evento_sin_tratar.ID_evento == resta_Periodos){	
		gestor_alarmas_restar_periodo();	//cada milisegundo llega un evento evento_resta_periodos
																			//Para restar 1 al periodo de las alarmas activas
	}
	if(evento_sin_tratar.ID_evento == Set_Alarm){ 
		gestor_alarmas_introducir_evento(evento_sin_tratar);
		//Al llegat un evento Set_Alarm se llama a la funcion del Gestor_Alarma para poner una alarma
	}
	
	if(evento_sin_tratar.ID_evento == evento_boton1){
		//Se usa t0 para calcular el tiempo de procesado de la entrada
		double t0 = timer1_temporizador_leer();
		nueva_pulsacion_0();
		actualizar_estado_0();
		//Se desactivan las interrupciones al entrar en la seccion critica
		configuracionVicEnable=VICIntEnable;
		configuracionVicClr=VICIntEnClr;
		VICIntEnClr = 0xffffffff;
		cola_guardar_eventos(Set_Alarm,0x04800064);
		//Se activan las interrupciones al salir de la sección critica
		VICIntEnable = configuracionVicEnable;
		VICIntEnClr = configuracionVicClr;
		uint8_t i = gestor_IO_leer(16,4);	//Variable que contiene la fila seleccionada por el usuario
		uint8_t j = gestor_IO_leer(20,4);	//Variable que contiene la columna seleccionada por el usuario
		uint8_t valor = gestor_IO_leer(24,4);	//Variable que contiene el nuevo valor seleccionado por el usuario
		uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
		uint8_t pista = (celda >> 4) & 0x01; 
			uint16_t candidatos_celda = (celda >> 7) & 0x01ff; 
		if(pista != 1 && (valor > 0 && valor <=9)){	//Si la celda no es una pista inicial y el valor a introducir esta entre 0 y 9 se modifica la celda
			celda_actualizar_celda(&cuadricula_C_C[i][j],valor);
			candidatos_propagar_c(cuadricula_C_C,i,j);	//Tras insertar el valor, se propaga al resto de celdas
			if(valor_en_candidatos(candidatos_celda,valor) == 1){		//Si el valor introducido es correcto se activa el led de validacion
				gestor_IO_escribir(13,1,1);		//Se activa el led de la GPIO
				cola_guardar_eventos(Set_Alarm, 0x070003e8);	//Se programa una alarma para desactivar el led tras un segundo
			}else{
				celda_actualizar_celda(&cuadricula_C_C[i][j],0x0020);	//Si el valor introducido es erroneo se activa el bit de la celda que indica un valor erroneo
			}
			//se calcula la diferencia de las variables de tiempo del procesado de la entrada
			double t1 = timer1_temporizador_leer();
			tiempoProcesado = t1 - t0;
		}
		//Si se introduce los valores fila=0, columna=0 y valor=0 acaba el programa
		if((i==0) && (j==0) & (valor == 0)){
				parar = 1;
			}
	}
	if(evento_sin_tratar.ID_evento == evento_boton2){
		nueva_pulsacion_1();
		actualizar_estado_1();
		configuracionVicEnable=VICIntEnable;
		configuracionVicClr=VICIntEnClr;
		VICIntEnClr = 0xffffffff;
		cola_guardar_eventos(Set_Alarm,0x05800064);
		VICIntEnable = configuracionVicEnable;
		VICIntEnClr = configuracionVicClr;
		uint8_t i = gestor_IO_leer(16,4);
		uint8_t j = gestor_IO_leer(20,4);
		uint8_t valor = gestor_IO_leer(24,4);
		uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
		uint8_t pista = (celda >> 4) & 0x01;  
		if(pista != 1){	//Si la celda no es una pista inicial se borra el valor
			celda_borrar_celda(&cuadricula_C_C[i][j]);
			candidatos_actualizar_c(cuadricula_C_C);	//Para evitar valores corruptos se vuelve a actualizar todo el valor
		}
		//Si se introduce los valores fila=0, columna=0 y valor=0 acaba el programa
		if((i==0) && (j==0) & (valor == 0)){
				parar = 1;
			}	
	}
	
	if(evento_sin_tratar.ID_evento == evento_alarma_pulsaciones_1){
		if(leer_estado_0()==PULSADO){
			//Si llega la alarma y el boton sigue pulsado no se hace nada
		}
		else{
			//En caso de que no este pulsado se actualiza el estado a no pulsado
			gestor_alarmas_quitar_alarma(evento_alarma_pulsaciones_1);
			clear_nueva_pulsacion_0();
			actualizar_estado_0();
		}
	}
	if(evento_sin_tratar.ID_evento == evento_alarma_pulsaciones_2){
		if(leer_estado_1()==PULSADO){
			//Si llega la alarma y el boton sigue pulsado no se hace nada
		}
		else{
			//En caso de que no este pulsado se actualiza el estado a no pulsado
			actualizar_estado_1();
			clear_nueva_pulsacion_1();
			gestor_alarmas_quitar_alarma(evento_alarma_pulsaciones_2);
		}
	} 
	if(evento_sin_tratar.ID_evento == evento_visualizacion_GPIO){
		//Se extraen los valores de la celda seleccionada por el usuario
		uint8_t i = gestor_IO_leer(16,4);
		uint8_t j = gestor_IO_leer(20,4);
		uint8_t valor = gestor_IO_leer(24,4);
		uint16_t celda=celda_leer_contenido(cuadricula_C_C[i][j]);
		//Se extraen los candidatos y el valor de la celda seleccionada
		uint8_t valor_celda = celda & 0x000f;
		uint16_t candidatos_celda =celda >> 7;
		uint8_t bitSucio = (celda >> 5) & 0x01;
		uint8_t pista = (celda >> 4) & 0x01;
		//Se escriben en la GPIO los candidatos y el valor de la celda seleccionada
		gestor_IO_escribir(0,4,valor_celda);
		gestor_IO_escribir(4,12,candidatos_celda);
		//Si la celda es una pista inicial o un valor erroneo se activa el led 
		if(bitSucio == 1 || pista == 1){
			gestor_IO_escribir(13,1,1);
		}
		//Se lee el estado de la GPIO para ver si ha cambiado 
		uint32_t estadoNuevo = gestor_IO_leer(0,32);
		if(estadoNuevo != estado_GPIO){	//Si el estado es distinto significa que el usuario sigue jugando y se vuelve a poner la alarma
			gestor_alarmas_quitar_alarma(2);
			cola_guardar_eventos(Set_Alarm,0x02003A98);
		}
		estado_GPIO =gestor_IO_leer(0,32); //Se actualiza el estado
	}
	if(evento_sin_tratar.ID_evento == evento_visualizar_led){
		//Se elimina el led tras 1 segundo
		gestor_IO_escribir(13,1,0);
	}
	if(evento_sin_tratar.ID_evento == evento_power_down){
		powerdown_procesador();
	}
	
}