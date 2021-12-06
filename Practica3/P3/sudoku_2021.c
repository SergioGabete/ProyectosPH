#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
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
#include "Gestor_Serial.h"
#include <inttypes.h>
#include "UART0.h"
//#include "tableros.h"
//#include "cuadricula.h"

//static char informacionJuego[] ="\n\nBienvenido al sudoku\n\n Vamos a comenzar " ;
static char informacionJuego[] ="Bienvenido al sudoku\n\n Vamos a comenzar " ;
static int indice_mensaje =0, num_caracteres =0;
static char* mensaje;

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

static int parar=0;
static uint32_t estado_GPIO=0;


//static int numFilas = 19;
//static int numColumnas = 109;
//static int tablero[numFilas][numColumnas];

int sudoku_parar(){
	return parar;
}

void sudoku_retomar_ejecucion(){
	parar = 0;
}

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

void sudoku_inicializar(){
	candidatos_actualizar_c(cuadricula_C_C);
}


void sudoku_mensajeinicial(){
	sudoku_enviar_mensaje(informacionJuego);
}

void sudoku_reiniciar(){
	for(int i=0;i<NUM_FILAS;i++){
		for(int j=0;j<NUM_FILAS;j++){
			cuadricula_C_C[i][j] = cuadricula_C_C_Aux[i][j];
		}
	}
	candidatos_actualizar_c(cuadricula_C_C);
}

void sudoku_evento_boton1(){
	
		//Quitar alarma del idle
		double t0 = timer1_temporizador_leer();
	
		//gestor_pulsacion_boton1_pretado();
		
		uint8_t i = gestor_IO_leer_fila();
		uint8_t j = gestor_IO_leer_columna();
		uint8_t valor = gestor_IO_leer_valor_introducir();
		
		uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
		//Estas las hacemos en celda para leer ya que el gestor no debe saber nada de que bits leer 
		uint8_t pista = celda_leer_pista(celda);  
		uint16_t candidatos_celda = celda_leer_candidatos(celda);
		
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
			//tiempoProcesado = t1 - t0;
		}
		//Si se introduce los valores fila=0, columna=0 y valor=0 acaba el programa
		if(gestor_IO_reiniciar(i,j,valor) == 1){	//Esto a lo mejor hay que cambiarlo, no se si es enn el gestorIO
			//	parar = 1;
			//Reiniciar
			sudoku_reiniciar();
			candidatos_actualizar_c(cuadricula_C_C);
			}

}



void sudoku_evento_visualizacion_GPIO(){
		uint8_t i = gestor_IO_leer_fila();
		uint8_t j = gestor_IO_leer_columna();
		uint8_t valor = gestor_IO_leer_valor_introducir();
		uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);	//Devuelve 0
		uint8_t pista = celda_leer_pista(celda); 
		
		uint8_t valor_celda = celda_leer_valor(celda);
		uint16_t candidatos_celda =celda_leer_candidatos(celda);
		uint8_t bitSucio = celda_leer_error(celda);

		gestor_IO_escribir_celda(valor_celda);
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


int sudoku_2021_comprobar_columnas(int fila, int columna){
	if(fila >0 && fila <=9 && columna >0 && columna <= 9){
			return 1;
	}
	else{
		return 0;
	}
}

uint8_t sudoku_2021_obtener_valor_celda(int fila, int columna){
			//Devuelve el valor de los 8 digitos
			return cuadricula_C_C[fila-1][columna-1];
}

void sudoku_2021_borrar_valor(int fila,int columna){
	//Para borrar habrá que comprobar que el valor de las columnas es válido, y posteriormente comprobar si es una pista,
	//es decir, no se puede borrar o si no lo es que se borrará
	if(sudoku_2021_comprobar_columnas(fila, columna) == 1){
		uint8_t pista = (cuadricula_C_C[fila-1][columna-1] >> 4) & 0x1;
		
		if(pista !=1 ) {
			celda_borrar_celda(&cuadricula_C_C[fila][columna]);
			candidatos_actualizar_c(cuadricula_C_C);	//Para evitar valores corruptos se vuelve a actualizar todo el valor
		}
	}
}

void sudoku_evento_boton2(){
		//gestor_pulsacion_boton2_pretado();
		uint8_t i = gestor_IO_leer_fila();
		uint8_t j = gestor_IO_leer_columna();
		uint8_t valor = gestor_IO_leer_valor_introducir();
		uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
		uint8_t pista = celda_leer_pista(celda); 
		uint16_t candidatos_celda = celda_leer_candidatos(celda);
		//sudoku_2021_borrar_valor(i,j);
		if(pista != 1){	//Si la celda no es una pista inicial se borra el valor
			celda_borrar_celda(&cuadricula_C_C[i][j]);
			candidatos_actualizar_c(cuadricula_C_C);	//Para evitar valores corruptos se vuelve a actualizar todo el valor
		}
		//Si se introduce los valores fila=0, columna=0 y valor=0 acaba el programa
		if(gestor_IO_reiniciar(i,j,valor) == 1){
				//parar = 1;
				sudoku_reiniciar();
			candidatos_actualizar_c(cuadricula_C_C);
			}
}


void sudoku_introducir_jugada(uint32_t aux){
		uint8_t i = aux >> 16;
		uint8_t j = aux >> 8;
		uint8_t valor = aux;
		
		uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
		//Estas las hacemos en celda para leer ya que el gestor no debe saber nada de que bits leer 
		uint8_t pista = celda_leer_pista(celda);  
		uint16_t candidatos_celda = celda_leer_candidatos(celda);
		
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
			
			
		}		//Esto es del if(se_puede_modificar(pista,valor) == 1){
		
		
		//Si se introduce los valores fila=0, columna=0 y valor=0 acaba el programa
		if(gestor_IO_reiniciar(i,j,valor) == 1){	//Esto a lo mejor hay que cambiarlo, no se si es enn el gestorIO
			//	parar = 1;
			//Reiniciar
			sudoku_reiniciar();
			candidatos_actualizar_c(cuadricula_C_C);	//Creo que esta linea hay que quitarla porque ya lo ha en la funcion reiniciar
			}
		//sudoku_mostrar_tablero();
}

void sudoku_continuar_mensaje(){
			if(indice_mensaje<num_caracteres){
				uart0_sendchar(mensaje[indice_mensaje]);
				indice_mensaje = indice_mensaje +1;
			}else if(num_caracteres!=0){
				num_caracteres=0;
				//cola_guardar_eventos(evento_fin_mensaje,0);
			}	
}

void sudoku_fin_mensaje(){
	
}

void sudoku_enviar_mensaje(char msg[]){
			indice_mensaje=0;
			num_caracteres=strlen(msg);
			mensaje=msg;
			uart0_sendchar(mensaje[indice_mensaje++]);			//Preguntar a enrique si se puede invocar sendchar desde aqui
}
/*
void sudoku_mostrar_tablero(){
	//generar matriz, numFilas = 19, numColumnas = 113
	//necesito los candidatos, valor y pista
	int numFilas = 19;
	int numColumnas = 109;
	uint16_t celda;
	uint8_t pista;
	uint16_t candidatos_celda;
	uint8_t valor_celda;
	int tablero[numFilas][numColumnas];
	
//			celda = celda_leer_contenido(cuadricula_C_C[i][j]);
//			pista = celda_leer_pista(celda); 
//			candidatos_celda = celda_leer_candidatos(celda);
//			valor_celda = celda_leer_valor(celda);
	
	for(int i=0;i < numColumnas;i++){
		tablero[0][i] = '+';
		tablero[numFilas-1][i] = '+';
	}
	//Se ponen los bordes de las celdas y tal
	tablero[1][0]='|';tablero[3][0]='|';tablero[5][0]='|';tablero[7][0]='|';tablero[9][0]='|';tablero[11][0]='|';tablero[13][0]='|';tablero[15][0]='|';tablero[17][0]='|';
	tablero[1][12]='|';tablero[3][12]='|';tablero[5][12]='|';tablero[7][12]='|';tablero[9][12]='|';tablero[11][12]='|';tablero[13][12]='|';tablero[15][12]='|';tablero[17][12]='|';
	tablero[1][24]='|';tablero[3][24]='|';tablero[5][24]='|';tablero[7][24]='|';tablero[9][24]='|';tablero[11][24]='|';tablero[13][24]='|';tablero[15][24]='|';tablero[17][24]='|';
	tablero[1][36]='|';tablero[3][36]='|';tablero[5][36]='|';tablero[7][36]='|';tablero[9][36]='|';tablero[11][36]='|';tablero[13][36]='|';tablero[15][36]='|';tablero[17][36]='|';
	tablero[1][48]='|';tablero[3][48]='|';tablero[5][48]='|';tablero[7][48]='|';tablero[9][48]='|';tablero[11][48]='|';tablero[13][48]='|';tablero[15][48]='|';tablero[17][48]='|';
	tablero[1][60]='|';tablero[3][60]='|';tablero[5][60]='|';tablero[7][60]='|';tablero[9][60]='|';tablero[11][60]='|';tablero[13][60]='|';tablero[15][60]='|';tablero[17][60]='|';
	tablero[1][72]='|';tablero[3][72]='|';tablero[5][72]='|';tablero[7][72]='|';tablero[9][72]='|';tablero[11][72]='|';tablero[13][72]='|';tablero[15][72]='|';tablero[17][72]='|';
	tablero[1][84]='|';tablero[3][84]='|';tablero[5][84]='|';tablero[7][84]='|';tablero[9][84]='|';tablero[11][84]='|';tablero[13][84]='|';tablero[15][84]='|';tablero[17][84]='|';
	tablero[1][96]='|';tablero[3][96]='|';tablero[5][96]='|';tablero[7][96]='|';tablero[9][96]='|';tablero[11][96]='|';tablero[13][96]='|';tablero[15][96]='|';tablero[17][96]='|';
	tablero[1][108]='|';tablero[3][108]='|';tablero[5][108]='|';tablero[7][108]='|';tablero[9][108]='|';tablero[11][108]='|';tablero[13][108]='|';tablero[15][108]='|';tablero[17][108]='|';
	for(int i=2;i<numFilas-1;i=i+2){
		for(int j=0;j<numColumnas;j++){
			tablero[i][j] = '-';
		}
	}
	
	
	
			
	gestor_serial_escribir_linea(tablero,numFilas,numColumnas);
}
*/
/*
Lo de que las variable solo accesibles por un modulo y que son globales solo deben ser estaticas HECHO
Si no tiene que ser global para ese modulo pues no la hagas global a ese modulo										HECHO
Lo de que los modulos .h solo deben ser visibles para los que los usan o sea que sean privados		HECHO

El planificador solo recibe eventos y lllama a los modulos para tratar esos eventos
El gestor_IO gestiona todo lo de visualizar la GPIO y ese evento alarma, el planificador solo llama a los modulos
Hacer un main separado que inicialice las cosas y llame al planificador y al sudoku

Tambien al desencolar hay seccion critica	HECHO
//Meter lo de desactivar interrupciones solo en la cola	HECHO

No llamar a nada de hardware que no sea el el modulo que lo gestiona	HECHO


//Pasarle el struct al gestor de alarmas	HECHO
//Quitar los doubles de los timers que se pierde precision	HECHO

Revisar un poco el tema de los eventos	HECHO

Tocar la cola con lo del overflow	HECHO

Añadir mas de 8 tipos de alarmas	NO HECO pero se ignora

Hacer lo del latido idle				HECHO
*/

