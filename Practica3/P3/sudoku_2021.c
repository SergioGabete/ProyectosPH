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
#include "Gestor_Serial.h"
#include "RTC.h"


static char informacionJuego[] ="Bienvenido al sudoku\nPara jugar puede introducir los siguientes comandos:\n$NEW para empezar una nueva partida\n$RST para detener la partida\n$FCVR donde F es la fila a introducir, C la columna y V el valor. R sera la suma de los 3 anteriores modulo 8" ;

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
static int tiempo_minutos;
static int tiempo_segundos;


static char mensajeFinal2[1000];
static char mensajeFinal[1000];
static char mensajeInicial[1000];

//Variables para saber si estamos en jugada introducida por comandos o por GPIO
static uint8_t iComando;
static uint8_t jComando;
static uint8_t valorComando;
static int estoy_en_comando =0;		//Si es 0 significa que no esta en comando

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
	gestor_serial_enviar_mensaje(informacionJuego);
}

void sudoku_reiniciar(){
	for(int i=0;i<NUM_FILAS;i++){
		for(int j=0;j<NUM_FILAS;j++){
			cuadricula_C_C[i][j] = cuadricula_C_C_Aux[i][j];
		}
	}
	candidatos_actualizar_c(cuadricula_C_C);
}


//Convierte el int en char s[]
void sudoku_convertir(int n, char s[])
 {
     int i = 0;
     char c;
     do {       
         s[i++] = n % 10 + '0';  
     } while ((n /= 10) > 0);     

     s[i] = '\0';
		 int  j=strlen(s);
     for (int z = 0; z<j; z++, j--) {
         c = s[z];
         s[z] = s[j];
         s[j] = c;
     }
} 

void sudoku_tiempo_total_partida(char mensaje_tiempo[]){
	char minutos[5];
	char segundos[5];
	tiempo_minutos= RTC_leer_minutos();
	tiempo_segundos = RTC_leer_segundos();
	sudoku_convertir(tiempo_minutos,minutos);
	sudoku_convertir(tiempo_segundos,segundos);
	//Envios de mensajes
	strcat(mensaje_tiempo,"Información de la partida: ");
	strcat(mensaje_tiempo,"\n Tiempo total de partida: ");
	strcat(mensaje_tiempo,minutos);
	strcat(mensaje_tiempo," minutos y  ");
	strcat(mensaje_tiempo,segundos);
	strcat(mensaje_tiempo,"segundos \n");
}                                              
	

void sudoku_evento_boton1(){
		if(estoy_en_comando == 0){
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
				celda_introducir_error(&cuadricula_C_C[i][j],valor);	//Si el valor introducido es erroneo se activa el bit de la celda que indica un valor erroneo
				
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
		}else{		//En este caso significa que estoy en comando
			gestor_IO_quitar_led();		//Se quita el led que hemos puesto antes
			estoy_en_comando = 0;	//He confirmado el comando pues ya no estoy en el
			gestor_alarmas_quitar_confirmar_jugada();		//Se quita la alarma que salta a los 3 segundos
			sudoku_mostrar_tablero();
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
    for (j=0;j<NUM_FILAS;j++){
			
			//if(celda_leer_valor(cuadricula[fila][j]) == 0){
				celda_eliminar_candidato(&cuadricula[fila][j],valor);
			//}
			//else{ // Celda con valor, no necesita candidatos
				//celda_eliminar_candidatos(&cuadricula[fila][j]);
			//}
		}
	//celda_eliminar_candidato(&cuadricula[fila][j],valor);

    /* recorrer columna descartando valor de listas candidatos */
    for (i=0;i<NUM_FILAS;i++){
			//if(celda_leer_valor(cuadricula[i][columna]) == 0){
				celda_eliminar_candidato(&cuadricula[i][columna],valor);			
			//else{ // Celda con valor, no necesita candidatos
				//celda_eliminar_candidatos(&cuadricula[i][columna]);
			//}
		}
	//celda_eliminar_candidato(&cuadricula[i][columna],valor);

    /* determinar fronteras región */
    init_i = init_region[fila];
    init_j = init_region[columna];
    end_i = init_i + 3;
    end_j = init_j + 3;

    /* recorrer region descartando valor de listas candidatos */
    for (i=init_i; i<end_i; i++) {
      for(j=init_j; j<end_j; j++) {
				//if(celda_leer_valor(cuadricula[i][j]) == 0){
					celda_eliminar_candidato(&cuadricula[i][j],valor);
				//}
				//else{ // Celda con valor, no necesita candidatos
					//celda_eliminar_candidatos(&cuadricula[i][j]);
				//}
			}
    }
}

int candidatos_actualizar_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS])
{
  int celdas_vacias = 0;
  uint8_t i;
	uint8_t j;
	
	//borrar todos los candidatos
	for ( i=0; i<NUM_FILAS;i++){
		for (j=0; j <NUM_FILAS;j++){
			celda_inicializar_candidatos(&cuadricula[i][j]);
		}
	}
	
	//recalcular candidatos de las celdas vacias calculando cuantas hay vacias
	for ( i=0; i<NUM_FILAS;i++){
		for (j=0; j <NUM_FILAS;j++){
			if (celda_leer_valor(cuadricula[i][j]) == 0){
				celdas_vacias ++;
			}
			else{
				candidatos_propagar_c(cuadricula,i,j);
			}
		}	
	}
	return celdas_vacias; //retornar el numero de celdas vacias
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
		if(estoy_en_comando == 0){
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
		}else{
			gestor_IO_quitar_led();		//Se quita el led que hemos puesto antes
			estoy_en_comando = 0;	//He confirmado el comando pues ya no estoy en el
			gestor_alarmas_quitar_confirmar_jugada();		//Se quita la alarma que salta a los 3 segundos
			//Propagar para quitar el valor
			celda_borrar_celda(&cuadricula_C_C[iComando][jComando]);		//Las variables que hemos guardado las uso para borrar la celda
			candidatos_actualizar_c(cuadricula_C_C);
			sudoku_mostrar_tablero();
		}
}


uint16_t sudoku_get_valor_candidatos(uint8_t fila,uint8_t columna){
	return cuadricula_C_C[fila][columna]>>7 ;
}

void sudoku_mostrar_tablero(){
	const int numFilas = 19;
	const int numColumnas=29;
	char tablero[numFilas][numColumnas];
	for(int i=0;i<numFilas;i=i+2){
		for(int j=0;j<numColumnas-1;j++){
			if(j%3==0){
				tablero[i][j] = '+';
			}else{
				tablero[i][j] = '-';
			}
		}
	}
	//Ahora hay que poner los valores y errores y si es pista
	//Ahora las celdas
	int iCuadricula=0;
	int jCuadricula=0;
	uint16_t celda;
	uint8_t pista;
	uint8_t error;
	uint8_t valor;
	uint16_t candidatos;
	for(int i=1;i<numFilas;i=i+2){
		for(int j=0;j<numColumnas-1;j++){
			if(j%3==0){
				tablero[i][j] = '|';
				//Aqui se aumentan los indices para recorrer las celdas
				//Obtener las cosas de esta celda
				if(j != numColumnas-2){		//parar que no tenga en cuenta el ultimo |
					celda = celda_leer_contenido(cuadricula_C_C[iCuadricula][jCuadricula]);
					pista = celda_leer_pista(celda); 
					error = celda_leer_error(celda);
					valor = celda_leer_valor(celda);
					jCuadricula=jCuadricula+1;
					if(jCuadricula==NUM_FILAS){		//j llega al final pues se pasa a la siguiente fila
						iCuadricula=iCuadricula+1;
						jCuadricula =0;
					}
				}
				
			}else{
				if(j%3==1){		//es la |. ahi pones el valor
					if(valor != 0){		//Si el valor no es 0 significa que se ha introducido algo
						tablero[i][j] = valor + '0';
					}else{
						tablero[i][j] =' ';
					}
				}else{
					if(j%3 == 2){		//es el caso |..
						if((pista&0x1) == 0x1){		//es pista
							tablero[i][j] ='P';
						}else{
							if((error&0x1)==1){//es error
								tablero[i][j] ='E';
							}else{
									if(tablero[i][j-1] != ' '){
											tablero[i][j]='V';
									}else{
											tablero[i][j]=' ';
									}
							}
						}
					}
				}
			}
		}
	}
	
	/*
	+--+--+--+
	|3P|3V|5V|		10+9*2+1
	*/
	
	for(int i=0;i<numFilas;i++){					//Poner fin de linea
		tablero[i][numColumnas-1] = '\n';
	}
	
	for(int i=0;i<numFilas;i++){
		for(int j=0;j<numColumnas;j++){
			mensajeFinal[i*numColumnas + j] = tablero[i][j];
		}
	}
	//Ahora hay que poner los candidatos
	int indiceFinal = numFilas*numColumnas;
	//int c1,c2,c3,c4,c5,c6,c7,c8,c9;	
	for(int i=0;i<NUM_FILAS;i++){		//Recorrer la matriz sacando los candidatos
		for(int j=0;j<NUM_FILAS;j++){
			celda = celda_leer_contenido(cuadricula_C_C[i][j]);
			candidatos= celda_leer_candidatos(celda);
			valor = celda_leer_valor(celda);
			if(valor == 0x0){		//Significa que no hay valor y por tanto candidatos
				mensajeFinal[indiceFinal]=i+'0';
				indiceFinal = indiceFinal +1;
				mensajeFinal[indiceFinal]=j+'0';
				indiceFinal = indiceFinal +1;
				mensajeFinal[indiceFinal]=' ';
				indiceFinal = indiceFinal +1;
				if((candidatos&0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal[indiceFinal]='1';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 1) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal[indiceFinal]='2';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 2) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal[indiceFinal]='3';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 3) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal[indiceFinal]='4';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 4) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal[indiceFinal]='5';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 5) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal[indiceFinal]='6';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 6) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal[indiceFinal]='7';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 7) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal[indiceFinal]='8';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 8) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal[indiceFinal]='9';
					indiceFinal = indiceFinal +1;
				}
				
				mensajeFinal[indiceFinal] = '\n';
				indiceFinal = indiceFinal +1;		
			}
		}
	}
	
	mensajeFinal[indiceFinal]='\0';
	gestor_serial_enviar_mensaje(mensajeFinal);
	
}



void sudoku_introducir_jugada(uint32_t aux){
		estoy_en_comando = 1;
		uint8_t i = aux >> 16;
		uint8_t j = aux >> 8;
		uint8_t valor = aux;
		iComando = i;
		jComando = j;
		valorComando = valor;
		
		uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
		//Estas las hacemos en celda para leer ya que el gestor no debe saber nada de que bits leer 
		uint8_t pista = celda_leer_pista(celda);  
		uint16_t candidatos_celda = celda_leer_candidatos(celda);
		
		if(se_puede_modificar(pista,valor) == 1){	//Si la celda no es una pista inicial y el valor a introducir esta entre 0 y 9 se modifica la celda
			
			
			celda_actualizar_celda(&cuadricula_C_C[i][j],valor);
			candidatos_propagar_c(cuadricula_C_C,i,j);	//Tras insertar el valor, se propaga al resto de celdas
			if(valor_en_candidatos(candidatos_celda,valor) == 1){		//Si el valor introducido es correcto se activa el led de validacion
				
				//gestor_IO_confirmar_escritura(); 	//Se quita porque ya no hace falta al introducir por linea serie
				
			}else{
				//celda_actualizar_celda(&cuadricula_C_C[i][j],0x0020);	//Si el valor introducido es erroneo se activa el bit de la celda que indica un valor erroneo
				//Esto a lo mejor hay que cambiarlo ^^
				//mejor ponerlo en plan celda poner error o algo asi
				celda_introducir_error(&cuadricula_C_C[i][j], valor);	//Si el valor introducido es erroneo se activa el bit de la celda que indica un valor erroneo
				
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
		
			sudoku_mostrar_tablero();
			//Se pone una alarma de 3 segundos para confirmar
			//el formato del aux sera	00000001 0 00000000000101110111000
			
			//gestor_IO_confirmar_escritura();	//lo activa durante un segundo pero yo lo desactivare cuando llegue el evento de confirmar escritura
			gestor_IO_escribir_led();
			//cola_guardar_eventos(Set_Alarm,0x01000BB8);
			cola_guardar_eventos(Set_Alarm,0x01008BB8);		//le meto un poco a la alarma para probarlo bien
}




void sudoku_mostrar_tablero_inicial(){
	//La idea es coger el array mensajeInicial y mensajeFinal y juntarlos
	const int numFilas = 19;
	const int numColumnas=29;
	char tablero[numFilas][numColumnas];
	for(int i=0;i<numFilas;i=i+2){
		for(int j=0;j<numColumnas-1;j++){
			if(j%3==0){
				tablero[i][j] = '+';
			}else{
				tablero[i][j] = '-';
			}
		}
	}
	//Ahora hay que poner los valores y errores y si es pista
	//Ahora las celdas
	int iCuadricula=0;
	int jCuadricula=0;
	uint16_t celda;
	uint8_t pista;
	uint8_t error;
	uint8_t valor;
	uint16_t candidatos;
	for(int i=1;i<numFilas;i=i+2){
		for(int j=0;j<numColumnas-1;j++){
			if(j%3==0){
				tablero[i][j] = '|';
				//Aqui se aumentan los indices para recorrer las celdas
				//Obtener las cosas de esta celda
				if(j != numColumnas-2){		//parar que no tenga en cuenta el ultimo |
					celda = celda_leer_contenido(cuadricula_C_C[iCuadricula][jCuadricula]);
					pista = celda_leer_pista(celda); 
					error = celda_leer_error(celda);
					valor = celda_leer_valor(celda);
					jCuadricula=jCuadricula+1;
					if(jCuadricula==NUM_FILAS){		//j llega al final pues se pasa a la siguiente fila
						iCuadricula=iCuadricula+1;
						jCuadricula =0;
					}
				}
				
			}else{
				if(j%3==1){		//es la |. ahi pones el valor
					if(valor != 0){		//Si el valor no es 0 significa que se ha introducido algo
						tablero[i][j] = valor + '0';
					}else{
						tablero[i][j] =' ';
					}
				}else{
					if(j%3 == 2){		//es el caso |..
						if((pista&0x1) == 0x1){		//es pista
							tablero[i][j] ='P';
						}else{
							if((error&0x1)==1){//es error
								tablero[i][j] ='E';
							}else{
									if(tablero[i][j-1] != ' '){
											tablero[i][j]='V';
									}else{
											tablero[i][j]=' ';
									}
							}
						}
					}
				}
			}
		}
	}
	
	for(int i=0;i<numFilas;i++){					//Poner fin de linea
		tablero[i][numColumnas-1] = '\n';
	}
	
	for(int i=0;i<numFilas;i++){
		for(int j=0;j<numColumnas;j++){
			mensajeFinal2[i*numColumnas + j] = tablero[i][j];
		}
	}
	//Ahora hay que poner los candidatos
	int indiceFinal = numFilas*numColumnas;
	//int c1,c2,c3,c4,c5,c6,c7,c8,c9;	
	for(int i=0;i<NUM_FILAS;i++){		//Recorrer la matriz sacando los candidatos
		for(int j=0;j<NUM_FILAS;j++){
			celda = celda_leer_contenido(cuadricula_C_C[i][j]);
			candidatos= celda_leer_candidatos(celda);
			valor = celda_leer_valor(celda);
			if(valor == 0x0){		//Significa que no hay valor y por tanto candidatos
				mensajeFinal2[indiceFinal]=i+'0';
				indiceFinal = indiceFinal +1;
				mensajeFinal2[indiceFinal]=j+'0';
				indiceFinal = indiceFinal +1;
				mensajeFinal2[indiceFinal]=' ';
				indiceFinal = indiceFinal +1;
				if((candidatos&0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal2[indiceFinal]='1';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 1) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal2[indiceFinal]='2';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 2) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal2[indiceFinal]='3';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 3) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal2[indiceFinal]='4';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 4) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal2[indiceFinal]='5';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 5) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal2[indiceFinal]='6';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 6) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal2[indiceFinal]='7';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 7) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal2[indiceFinal]='8';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 8) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
					mensajeFinal2[indiceFinal]='9';
					indiceFinal = indiceFinal +1;
				}
				mensajeFinal2[indiceFinal] = '\n';
				indiceFinal = indiceFinal +1;		
			}
		}
	}
	
	mensajeFinal2[indiceFinal]='\0';
	
	//Ahora se juntan 
	
	/* make space for the new string (should check the return value ...) */
	strcpy(mensajeInicial, informacionJuego); /* copy name into the new var */
	strcat(mensajeInicial, mensajeFinal2); /* add the extension */
	gestor_serial_enviar_mensaje(mensajeInicial);
}

//Significa que ha llegado la alarma de 3 segundos tras introducir la jugada
//Se muestra el tablero y se quita el led de confirmar escritura
void sudoku_confirmar_jugada(){
	sudoku_mostrar_tablero();	//Se muestra otra vez porque lo exigen y se quita el led 
	gestor_IO_quitar_led();		//Se quita el led que hemos puesto antes
	estoy_en_comando = 0;
}

