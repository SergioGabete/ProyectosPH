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
//#include "cuadricula.h"

static int parar =0;



void planificador_init(){
	struct evento evento_sin_tratar;
		while(parar == 0){  //Esto debe ser una funcion de sudoku para saber si parar
			if(cola_comprobar_nuevos_eventos() == 1){ //Si hay eventos nuevos sin tratar se desencola un evento
				evento_sin_tratar = cola_evento_sin_tratar();
				planificador_tratar_evento(evento_sin_tratar);
			}else{
				//Si no hay eventos a tratar se pasa a modo idle
				//gestor_IO_activar_iddle();
				idle_procesador();					//MAL cambiarlo
				//gestor_IO_desactivar_iddle();
			}
		}
	
}


/****************************************
*	La funcion realiza una accion dependiendo del evento que le llegue al planificador*/
void planificador_tratar_evento(struct evento evento_sin_tratar){
	//quitar alarma del idle
	switch(evento_sin_tratar.ID_evento){
		case resta_Periodos:
			gestor_alarmas_restar_periodo();
			break;
		case Set_Alarm:
			gestor_alarmas_introducir_evento(evento_sin_tratar.auxData);
			break;
		case evento_boton1:
			gestor_pulsacion_boton1_pretado();
			sudoku_evento_boton1();
			break;
		case evento_boton2:
			gestor_pulsacion_boton2_pretado();
			sudoku_evento_boton2();
			break;
		case evento_alarma_pulsaciones_1:
			gestor_pulsacion_alarma_boton1();
			break;
		case evento_alarma_pulsaciones_2:
			gestor_pulsacion_alarma_boton1();
			break;
		case evento_visualizacion_GPIO:
			sudoku_evento_visualizacion_GPIO();
			break;
		case  evento_visualizar_led:
			gestor_IO_quitar_led();
			break;
		case evento_power_down:
			//Se para el modo idle
			gestor_alarmas_quitar_alarma(evento_idle);
			powerdown_procesador();
			gestor_alarmas_activar_alarma(evento_idle);
			break;
		case evento_idle:
			gestor_IO_evento_idle();
			break;
		case evento_rst:
			parar = 1;		//Esto no se si corresponde al planificador
			break;
		case evento_new:
			sudoku_reiniciar();
			break;
		case evento_jugada:
			sudoku_introducir_jugada(evento_sin_tratar.auxData);								//No se si se deberia meter esa funcion nueva de sudoku o usar cosas de la GPIO
			break;
		case evento_continuar_mensaje:
			sudoku_continuar_mensaje();
			break;
		default:
			;
			break;
	}
	
	//Poner alarma del idle
	//Llega una alarma de tipo latido que la vaya quitando
		
}


		//Se usa t0 para calcular el tiempo de procesado de la entrada
//		double t0 = timer1_temporizador_leer();
//		
//		/*gestor_pulsacion_nueva_pulsacion_0();		//Meter estas 2 y el setalarm en el gestor pulsacion
//		gestor_pulsacion_actualizar_estado_0();
//		cola_guardar_eventos(Set_Alarm,0x04800064);*/
//		gestor_pulsacion_boton1_pretado();
//		
//		uint8_t i = gestor_IO_leer_fila();
//		uint8_t j = gestor_IO_leer_columna();
//		uint8_t valor = gestor_IO_leer_valor_introducir();
//		
//		uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
//		//Estas las hacemos en celda para leer ya que el gestor no debe saber nada de que bits leer 
//		uint8_t pista = celda_leer_pista(celda);  
//		uint16_t candidatos_celda = celda_leer_candidatos(celda);
//		//if(pista != 1 && (valor > 0 && valor <=9)){
//		if(se_puede_modificar(pista,valor) == 1){	//Si la celda no es una pista inicial y el valor a introducir esta entre 0 y 9 se modifica la celda
//			celda_actualizar_celda(&cuadricula_C_C[i][j],valor);
//			candidatos_propagar_c(cuadricula_C_C,i,j);	//Tras insertar el valor, se propaga al resto de celdas
//			if(valor_en_candidatos(candidatos_celda,valor) == 1){		//Si el valor introducido es correcto se activa el led de validacion
//				//gestor_IO_escribir_led();	//Se activa el led de la GPIO
//				//cola_guardar_eventos(Set_Alarm, 0x070003e8);	//Se programa una alarma para desactivar el led tras un segundo
//				gestor_IO_confirmar_escritura(); 
//				
//			}else{
//				//celda_actualizar_celda(&cuadricula_C_C[i][j],0x0020);	//Si el valor introducido es erroneo se activa el bit de la celda que indica un valor erroneo
//				//Esto a lo mejor hay que cambiarlo ^^
//				//mejor ponerlo en plan celda poner error o algo asi
//				celda_introducir_error(&cuadricula_C_C[i][j]);	//Si el valor introducido es erroneo se activa el bit de la celda que indica un valor erroneo
//				
//			}
//			//se calcula la diferencia de las variables de tiempo del procesado de la entrada
//			double t1 = timer1_temporizador_leer();
//			tiempoProcesado = t1 - t0;
//		}
//		//Si se introduce los valores fila=0, columna=0 y valor=0 acaba el programa
//		if(gestor_IO_reiniciar(i,j,valor) == 1){	//Esto a lo mejor hay que cambiarlo, no se si es enn el gestorIO
//				parar = 1;
//			}
//	}
//	
//	

//	if(evento_sin_tratar.ID_evento == evento_boton2){
////		gestor_pulsacion_nueva_pulsacion_1();	//Son del gestor			//Meter estas 2 en el gestor y el evento setAlarm
////		gestor_pulsacion_actualizar_estado_1();	//Son del gestor
////		cola_guardar_eventos(Set_Alarm,0x05800064);	//Meter con esas 2 en el gestor	
//		gestor_pulsacion_boton2_pretado();
////		VICIntEnable = configuracionVicEnable;
////		VICIntEnClr = configuracionVicClr;
//		uint8_t i = gestor_IO_leer_fila();
//		uint8_t j = gestor_IO_leer_columna();
//		uint8_t valor = gestor_IO_leer_valor_introducir();
//		uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
//		uint8_t pista = celda_leer_pista(celda); 
//		uint16_t candidatos_celda = celda_leer_candidatos(celda);
//		if(pista != 1){	//Si la celda no es una pista inicial se borra el valor
//			celda_borrar_celda(&cuadricula_C_C[i][j]);
//			candidatos_actualizar_c(cuadricula_C_C);	//Para evitar valores corruptos se vuelve a actualizar todo el valor
//		}
//		//Si se introduce los valores fila=0, columna=0 y valor=0 acaba el programa
//		if(gestor_IO_reiniciar(i,j,valor) == 1){
//				parar = 1;
//		}	


















//		uint8_t i = gestor_IO_leer_fila();
//		uint8_t j = gestor_IO_leer_columna();
//		uint8_t valor = gestor_IO_leer_valor_introducir();
//		uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);	//Devuelve 0
//		uint8_t pista = celda_leer_pista(celda); 
//		//uint16_t candidatos_celda = celda_leer_candidatos(celda);
//		
//		uint8_t valor_celda = celda_leer_valor(celda);
//		uint16_t candidatos_celda =celda_leer_candidatos(celda);
//		uint8_t bitSucio = celda_leer_error(celda);
//		//uint8_t pista = (celda >> 4) & 0x01;
//		
//		//Se escriben en la GPIO los candidatos y el valor de la celda seleccionada
//		//gestor_IO_escribir(0,4,valor_celda);
//		gestor_IO_escribir_celda(valor_celda);
//		//gestor_IO_escribir(4,12,candidatos_celda);
//		gestor_IO_escribir_candidatos(candidatos_celda);
//		
//		//Si la celda es una pista inicial o un valor erroneo se activa el led 
//		if(bitSucio == 1 || pista == 1){
//			//gestor_IO_escribir(13,1,1);
//			gestor_IO_escribir_led();
//		}
//		//Se lee el estado de la GPIO para ver si ha cambiado 
//		//uint32_t estadoNuevo = gestor_IO_leer(0,32);
//		uint32_t estadoNuevo = gestor_IO_leer_estado();
//		if(estadoNuevo != estado_GPIO){	//Si el estado es distinto significa que el usuario sigue jugando y se vuelve a poner la alarma
//			gestor_alarmas_resetear_power_down();
//		}
//		estado_GPIO =gestor_IO_leer_estado(); //Se actualiza el estado


