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
//#include "tableros.h"
#include "cuadricula.h"


static volatile int parar;
static volatile uint32_t estado_GPIO=0;
static volatile double tiempoProcesado=0;

void candidatos_propagar_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
	uint8_t fila, uint8_t columna)
{
    uint8_t j, i , init_i, init_j, end_i, end_j;
    /* puede ayudar esta "look up table" a mejorar el rendimiento */
    const uint8_t init_region[NUM_FILAS] = {0, 0, 0, 3, 3, 3, 6, 6, 6};

    /* valor que se propaga */
    uint8_t valor = celda_leer_valor(cuadricula[fila][columna]);

    /* recorrer fila descartando valor de listas candidatos */
    for (j=0;j<NUM_FILAS;j++)
	//celda_eliminar_candidato(&cuadricula[fila][j],valor);
		celda_eliminar_candidato(&cuadricula[fila][j],valor);

    /* recorrer columna descartando valor de listas candidatos */
    for (i=0;i<NUM_FILAS;i++)
	//celda_eliminar_candidato(&cuadricula[i][columna],valor);
		celda_eliminar_candidato(&cuadricula[i][columna],valor);

    /* determinar fronteras región */
    init_i = init_region[fila];
    init_j = init_region[columna];
    end_i = init_i + 3;
    end_j = init_j + 3;

    /* recorrer region descartando valor de listas candidatos */
    for (i=init_i; i<end_i; i++) {
      for(j=init_j; j<end_j; j++) {
	      //celda_eliminar_candidato(&cuadricula[i][j],valor);
				celda_eliminar_candidato(&cuadricula[i][columna],valor);
	    }
    }
}

int candidatos_actualizar_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS])
{
   int celdas_vacias = 0;
   uint8_t i;
   uint8_t j;
	//Primero se ponen a 0 las listas de candidatos para que en el caso
	//de eliminar o cambiar el valor de una celda no se corrompa el
	//tablero entero
	for(i=0;i<NUM_FILAS;i++){
		for(j=0;j<NUM_FILAS;j++){
			celda_inicializar_candidatos(&cuadricula[i][j]);
		}
	}
	//Una vez inicializadas las listas de candidatos se debe recorrer el 
	//tablero para actualizar las listas de los candidatos de las celdas
		for(i=0;i<NUM_FILAS;i++){
			for(j=0;j<NUM_FILAS;j++){
				//En caso de que la celda sea 0 se suma 1 a la variable
				//que cuenta las celdas vacias
				if(celda_leer_valor(cuadricula[i][j])==0x00){
					celdas_vacias=celdas_vacias+1;
					//En caso de que el valor de la celda no sea 0 se debe propagar 
					//ese valor a lo largo de la fila, columna y region
				}	else{		
					candidatos_propagar_c(cuadricula,i,j);
				}					
			}
		}
		//se devuelve el numero de celdas vacias
		return celdas_vacias;
}

///****************************************
//* La funcion devuelve 1 si valorNuevo no se encuentra dentro de los candidatos
//	En casocontrario devuelve 0*/
//int valor_en_candidatos(uint16_t candidatos_celda, uint8_t valorNuevo){
//	if( ((candidatos_celda >> (valorNuevo -1) ) & 0x1) == 0){
//		return 1;
//	}else{
//		return 0;
//	}
//}

///****************************************
//*	La funcion realiza una accion dependiendo del evento que le llegue al planificador*/
//void planificador_tratar_evento(struct evento evento_sin_tratar){
//	uint32_t configuracionVicEnable;	//Variable que alamacena la configuracion del registro VICIntEnable
//	uint32_t configuracionVicClr;			//Variable que alamacena la configuracion del registro VICIntEnClr
//	if(evento_sin_tratar.ID_evento == resta_Periodos){	
//		gestor_alarmas_restar_periodo();	//cada milisegundo llega un evento evento_resta_periodos
//																			//Para restar 1 al periodo de las alarmas activas
//	}
//	if(evento_sin_tratar.ID_evento == Set_Alarm){ 
//		gestor_alarmas_introducir_evento(evento_sin_tratar);
//		//Al llegat un evento Set_Alarm se llama a la funcion del Gestor_Alarma para poner una alarma
//	}
//	
//	if(evento_sin_tratar.ID_evento == evento_boton1){
//		//Se usa t0 para calcular el tiempo de procesado de la entrada
//		double t0 = timer1_temporizador_leer();
//		nueva_pulsacion_0();
//		actualizar_estado_0();
//		//Se desactivan las interrupciones al entrar en la seccion critica
//		configuracionVicEnable=VICIntEnable;
//		configuracionVicClr=VICIntEnClr;
//		VICIntEnClr = 0xffffffff;
//		cola_guardar_eventos(Set_Alarm,0x04800064);
//		//Se activan las interrupciones al salir de la sección critica
//		VICIntEnable = configuracionVicEnable;
//		VICIntEnClr = configuracionVicClr;
//		uint8_t i = gestor_IO_leer(16,4);	//Variable que contiene la fila seleccionada por el usuario
//		uint8_t j = gestor_IO_leer(20,4);	//Variable que contiene la columna seleccionada por el usuario
//		uint8_t valor = gestor_IO_leer(24,4);	//Variable que contiene el nuevo valor seleccionado por el usuario
//		uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
//		uint8_t pista = (celda >> 4) & 0x01; 
//			uint16_t candidatos_celda = (celda >> 7) & 0x01ff; 
//		if(pista != 1 && (valor > 0 && valor <=9)){	//Si la celda no es una pista inicial y el valor a introducir esta entre 0 y 9 se modifica la celda
//			celda_actualizar_celda(&cuadricula_C_C[i][j],valor);
//			candidatos_propagar_c(cuadricula_C_C,i,j);	//Tras insertar el valor, se propaga al resto de celdas
//			if(valor_en_candidatos(candidatos_celda,valor) == 1){		//Si el valor introducido es correcto se activa el led de validacion
//				gestor_IO_escribir(13,1,1);		//Se activa el led de la GPIO
//				cola_guardar_eventos(Set_Alarm, 0x070003e8);	//Se programa una alarma para desactivar el led tras un segundo
//			}else{
//				celda_actualizar_celda(&cuadricula_C_C[i][j],0x0020);	//Si el valor introducido es erroneo se activa el bit de la celda que indica un valor erroneo
//			}
//			//se calcula la diferencia de las variables de tiempo del procesado de la entrada
//			double t1 = timer1_temporizador_leer();
//			tiempoProcesado = t1 - t0;
//		}
//		//Si se introduce los valores fila=0, columna=0 y valor=0 acaba el programa
//		if((i==0) && (j==0) & (valor == 0)){
//				parar = 1;
//			}
//	}
//	if(evento_sin_tratar.ID_evento == evento_boton2){
//		nueva_pulsacion_1();
//		actualizar_estado_1();
//		configuracionVicEnable=VICIntEnable;
//		configuracionVicClr=VICIntEnClr;
//		VICIntEnClr = 0xffffffff;
//		cola_guardar_eventos(Set_Alarm,0x05800064);
//		VICIntEnable = configuracionVicEnable;
//		VICIntEnClr = configuracionVicClr;
//		uint8_t i = gestor_IO_leer(16,4);
//		uint8_t j = gestor_IO_leer(20,4);
//		uint8_t valor = gestor_IO_leer(24,4);
//		uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
//		uint8_t pista = (celda >> 4) & 0x01;  
//		if(pista != 1){	//Si la celda no es una pista inicial se borra el valor
//			celda_borrar_celda(&cuadricula_C_C[i][j]);
//			candidatos_actualizar_c(cuadricula_C_C);	//Para evitar valores corruptos se vuelve a actualizar todo el valor
//		}
//		//Si se introduce los valores fila=0, columna=0 y valor=0 acaba el programa
//		if((i==0) && (j==0) & (valor == 0)){
//				parar = 1;
//			}	
//	}
//	
//	if(evento_sin_tratar.ID_evento == evento_alarma_pulsaciones_1){
//		if(leer_estado_0()==PULSADO){
//			//Si llega la alarma y el boton sigue pulsado no se hace nada
//		}
//		else{
//			//En caso de que no este pulsado se actualiza el estado a no pulsado
//			gestor_alarmas_quitar_alarma(evento_alarma_pulsaciones_1);
//			clear_nueva_pulsacion_0();
//			actualizar_estado_0();
//		}
//	}
//	if(evento_sin_tratar.ID_evento == evento_alarma_pulsaciones_2){
//		if(leer_estado_1()==PULSADO){
//			//Si llega la alarma y el boton sigue pulsado no se hace nada
//		}
//		else{
//			//En caso de que no este pulsado se actualiza el estado a no pulsado
//			actualizar_estado_1();
//			clear_nueva_pulsacion_1();
//			gestor_alarmas_quitar_alarma(evento_alarma_pulsaciones_2);
//		}
//	} 
//	if(evento_sin_tratar.ID_evento == evento_visualizacion_GPIO){
//		//Se extraen los valores de la celda seleccionada por el usuario
//		uint8_t i = gestor_IO_leer(16,4);
//		uint8_t j = gestor_IO_leer(20,4);
//		uint8_t valor = gestor_IO_leer(24,4);
//		uint16_t celda=celda_leer_contenido(cuadricula_C_C[i][j]);
//		//Se extraen los candidatos y el valor de la celda seleccionada
//		uint8_t valor_celda = celda & 0x000f;
//		uint16_t candidatos_celda =celda >> 7;
//		uint8_t bitSucio = (celda >> 5) & 0x01;
//		uint8_t pista = (celda >> 4) & 0x01;
//		//Se escriben en la GPIO los candidatos y el valor de la celda seleccionada
//		gestor_IO_escribir(0,4,valor_celda);
//		gestor_IO_escribir(4,12,candidatos_celda);
//		//Si la celda es una pista inicial o un valor erroneo se activa el led 
//		if(bitSucio == 1 || pista == 1){
//			gestor_IO_escribir(13,1,1);
//		}
//		//Se lee el estado de la GPIO para ver si ha cambiado 
//		uint32_t estadoNuevo = gestor_IO_leer(0,32);
//		if(estadoNuevo != estado_GPIO){	//Si el estado es distinto significa que el usuario sigue jugando y se vuelve a poner la alarma
//			gestor_alarmas_quitar_alarma(2);
//			cola_guardar_eventos(Set_Alarm,0x02003A98);
//		}
//		estado_GPIO =gestor_IO_leer(0,32); //Se actualiza el estado
//	}
//	if(evento_sin_tratar.ID_evento == evento_visualizar_led){
//		//Se elimina el led tras 1 segundo
//		gestor_IO_escribir(13,1,0);
//	}
//	if(evento_sin_tratar.ID_evento == evento_power_down){
//		powerdown_procesador();
//	}
//	
//}




//int main (void) {
//	struct evento evento_sin_tratar;
//	while(1){
//		//Se inicializan todos los perifericos del sistema
//		eint_init();
//		timer1_temporizador_iniciar();
//		timer1_temporizador_empezar();
//		timer0_temporizador_iniciar();
//		timer0_temporizador_empezar();
//		timer0_temporizador_periodico(1);
//		gestor_alarmas_init();
//		gestor_IO_iniciar();
//		//Poner alarma para la visualizacion constante de la GPIO
//		cola_guardar_eventos(Set_Alarm,0x068000C8);
//		parar=0;
//		candidatos_actualizar_c(cuadricula_C_C);
//		cola_guardar_eventos(Set_Alarm,0x02003A98);
//		while(parar == 0){  //Mientras no se introduzca el reset de la partida se sigue ejecutando
//			if(cola_comprobar_nuevos_eventos() == 1){ //Si hay eventos nuevos sin tratar se desencola un evento
//				evento_sin_tratar = cola_evento_sin_tratar();
//				planificador_tratar_evento(evento_sin_tratar);
//			}else{
//				//Si no hay eventos a tratar se pasa a modo idle
//				idle_procesador();
//			}
//		}
//		//Si ha habido un reset de la partida se actualiza el tablero a sus valores iniciales
//		for(int i=0;i<NUM_FILAS;i++){
//			for(int j=0;j<NUM_COLUMNAS;j++){
//				cuadricula_C_C[i][j] = cuadricula_C_C_Aux[i][j];
//			}
//		}
//		//Se reinician los timers
//		double timer0_finalizacion = timer0_temporizador_parar();
//		double timer1_finalizacion = timer1_temporizador_parar();
//	}
//	
//}





/*
Lo de que las variable solo accesibles por un modulo y que son globales solo deben ser estaticas HECHO
Si no tiene que ser global para ese modulo pues no la hagas global a ese modulo										HECHO
Lo de que los modulos .h solo deben ser visibles para los que los usan o sea que sean privados		HECHO

El planificador solo recibe eventos y lllama a los modulos para tratar esos eventos
El gestor_IO gestiona todo lo de visualizar la GPIO y ese evento alarma, el planificador solo llama a los modulos
Hacer un main separado que inicialice las cosas y llame al planificador y al sudoku

Tambien al desencolar hay seccion critica
//Meter lo de desactivar interrupciones solo en la cola

No llamar a nada de hardware que no sea el el modulo que lo gestiona


//Pasarle el struct al gestor de alarmas
//Quitar los doubles de los timers que se pierde precision

Revisar un poco el tema de los eventos

Tocar la cola con lo del overflow

Añadir mas de 8 tipos de alarmas

Hacer lo del latido idle				LISTO
*/

