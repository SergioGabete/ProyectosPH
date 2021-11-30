#include "planificador.h"
#include <stddef.h>
#include "sudoku_2021.h"
#include <time.h>
#include "timer.h"
#include "evento.h"
#include "cola.h"
#include <LPC210x.H> 
#include "Gestor_Alarmas.h"
#include "Gestor_Pulsacion.h"
#include "GPIO.h"
#include "Gestor_IO.h"
#include "pw_id_control.h"
#include "cuadricula.h"

static int parar=0;
static uint32_t estado_GPIO=0;
static double tiempoProcesado=0;

/****************************************
* La funcion devuelve 1 si valorNuevo no se encuentra dentro de los candidatos
	En casocontrario devuelve 0*/
int valor_en_candidatos(uint16_t candidatos_celda, uint8_t valorNuevo){
	if( ((candidatos_celda >> (valorNuevo -1) ) & 0x1) == 0){
		return 1;
	}else{
		return 0;
	}
}

int se_puede_modificar(uint8_t pista, uint8_t valor){
	if(pista != 1 && (valor > 0 && valor <=9)){
		return 1;
	}else{
		return 0;
	}
}

int planificador_parar(){
	return parar;
}

void planificador_retormar_ejecucion(){
	parar = 0;
}

/****************************************
*	La funcion realiza una accion dependiendo del evento que le llegue al planificador*/
void planificador_tratar_evento(struct evento evento_sin_tratar){
	//int parar;
	//uint32_t estado_GPIO=0;
	//double tiempoProcesado=0;
//	uint32_t configuracionVicEnable;	//Variable que alamacena la configuracion del registro VICIntEnable
//	uint32_t configuracionVicClr;			//Variable que alamacena la configuracion del registro VICIntEnClr
	
	
	if(evento_sin_tratar.ID_evento == resta_Periodos){	
		gestor_alarmas_restar_periodo();	//cada milisegundo llega un evento evento_resta_periodos
																			//Para restar 1 al periodo de las alarmas activas
	}
	if(evento_sin_tratar.ID_evento == Set_Alarm){ 
		gestor_alarmas_introducir_evento(evento_sin_tratar.auxData);
		//Al llegat un evento Set_Alarm se llama a la funcion del Gestor_Alarma para poner una alarma
	}
	
	if(evento_sin_tratar.ID_evento == evento_boton1){
		//Se usa t0 para calcular el tiempo de procesado de la entrada
		double t0 = timer1_temporizador_leer();
		
		/*gestor_pulsacion_nueva_pulsacion_0();		//Meter estas 2 y el setalarm en el gestor pulsacion
		gestor_pulsacion_actualizar_estado_0();
		cola_guardar_eventos(Set_Alarm,0x04800064);*/
		gestor_pulsacion_boton1_pretado();
		
		uint8_t i = gestor_IO_leer_fila();
		uint8_t j = gestor_IO_leer_columna();
		uint8_t valor = gestor_IO_leer_valor_introducir();
		
		uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
		//Estas las hacemos en celda para leer ya que el gestor no debe saber nada de que bits leer 
		uint8_t pista = celda_leer_pista(celda); 
		uint16_t candidatos_celda = celda_leer_candidatos(celda);
		//if(pista != 1 && (valor > 0 && valor <=9)){
		if(se_puede_modificar(pista,valor) == 1){	//Si la celda no es una pista inicial y el valor a introducir esta entre 0 y 9 se modifica la celda
			celda_actualizar_celda(&cuadricula_C_C[i][j],valor);
			candidatos_propagar_c(cuadricula_C_C,i,j);	//Tras insertar el valor, se propaga al resto de celdas
			if(valor_en_candidatos(candidatos_celda,valor) == 1){		//Si el valor introducido es correcto se activa el led de validacion
				//gestor_IO_escribir_led();	//Se activa el led de la GPIO
				//cola_guardar_eventos(Set_Alarm, 0x070003e8);	//Se programa una alarma para desactivar el led tras un segundo
				gestor_IO_confirmar_escritura(); 
				
			}else{
				//celda_actualizar_celda(&cuadricula_C_C[i][j],0x0020);	//Si el valor introducido es erroneo se activa el bit de la celda que indica un valor erroneo
				//Esto a lo mejor hay que cambiarlo ^^
				//mejor ponerlo en plan celda poner error o algo asi
				celda_introducir_error(&cuadricula_C_C[i][j]);	//Si el valor introducido es erroneo se activa el bit de la celda que indica un valor erroneo
				
			}
			//se calcula la diferencia de las variables de tiempo del procesado de la entrada
			double t1 = timer1_temporizador_leer();
			tiempoProcesado = t1 - t0;
		}
		//Si se introduce los valores fila=0, columna=0 y valor=0 acaba el programa
		if(gestor_IO_reiniciar(i,j,valor) == 1){	//Esto a lo mejor hay que cambiarlo, no se si es enn el gestorIO
				parar = 1;
			}
	}
	
	
	if(evento_sin_tratar.ID_evento == evento_boton2){
//		gestor_pulsacion_nueva_pulsacion_1();	//Son del gestor			//Meter estas 2 en el gestor y el evento setAlarm
//		gestor_pulsacion_actualizar_estado_1();	//Son del gestor
//		cola_guardar_eventos(Set_Alarm,0x05800064);	//Meter con esas 2 en el gestor	
		gestor_pulsacion_boton2_pretado();
//		VICIntEnable = configuracionVicEnable;
//		VICIntEnClr = configuracionVicClr;
		uint8_t i = gestor_IO_leer_fila();
		uint8_t j = gestor_IO_leer_columna();
		uint8_t valor = gestor_IO_leer_valor_introducir();
		uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
		uint8_t pista = celda_leer_pista(celda); 
		uint16_t candidatos_celda = celda_leer_candidatos(celda);
		if(pista != 1){	//Si la celda no es una pista inicial se borra el valor
			celda_borrar_celda(&cuadricula_C_C[i][j]);
			candidatos_actualizar_c(cuadricula_C_C);	//Para evitar valores corruptos se vuelve a actualizar todo el valor
		}
		//Si se introduce los valores fila=0, columna=0 y valor=0 acaba el programa
		if(gestor_IO_reiniciar(i,j,valor) == 1){
				parar = 1;
			}	
	}
	
	if(evento_sin_tratar.ID_evento == evento_alarma_pulsaciones_1){
		gestor_pulsacion_alarma_boton1();		//estado_GPIO funcion no se si deberia comunicarse directamente con el gestor
		//Mejor preguntar a enrique
	}
	if(evento_sin_tratar.ID_evento == evento_alarma_pulsaciones_2){
		gestor_pulsacion_alarma_boton2();	//estado_GPIO funcion no se si deberia comunicarse directamente con el gestor
		//Mejor preguntar a enrique
	} 
	if(evento_sin_tratar.ID_evento == evento_visualizacion_GPIO){
		//Se extraen los valores de la celda seleccionada por el usuario
		//Se extraen los candidatos y el valor de la celda seleccionad
		
		uint8_t i = gestor_IO_leer_fila();
		uint8_t j = gestor_IO_leer_columna();
		uint8_t valor = gestor_IO_leer_valor_introducir();
		uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
		uint8_t pista = celda_leer_pista(celda); 
		//uint16_t candidatos_celda = celda_leer_candidatos(celda);
		
		uint8_t valor_celda = celda_leer_valor(celda);
		uint16_t candidatos_celda =celda_leer_candidatos(celda);
		uint8_t bitSucio = celda_leer_error(celda);
		//uint8_t pista = (celda >> 4) & 0x01;
		
		//Se escriben en la GPIO los candidatos y el valor de la celda seleccionada
		//gestor_IO_escribir(0,4,valor_celda);
		gestor_IO_escribir_celda(valor);
		//gestor_IO_escribir(4,12,candidatos_celda);
		gestor_IO_escribir_candidatos(candidatos_celda);
		
		//Si la celda es una pista inicial o un valor erroneo se activa el led 
		if(bitSucio == 1 || pista == 1){
			//gestor_IO_escribir(13,1,1);
			gestor_IO_escribir_led();
		}
		//Se lee el estado de la GPIO para ver si ha cambiado 
		//uint32_t estadoNuevo = gestor_IO_leer(0,32);
		uint32_t estadoNuevo = gestor_IO_leer_estado();
		if(estadoNuevo != estado_GPIO){	//Si el estado es distinto significa que el usuario sigue jugando y se vuelve a poner la alarma
			gestor_alarmas_resetear_power_down();
		}
		estado_GPIO =gestor_IO_leer_estado(); //Se actualiza el estado
	}
	if(evento_sin_tratar.ID_evento == evento_visualizar_led){
		gestor_IO_quitar_led();
	}
	if(evento_sin_tratar.ID_evento == evento_power_down){
		powerdown_procesador();
	}
	
}


