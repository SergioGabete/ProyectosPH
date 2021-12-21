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
#include "SWI.h"

static int sudoku_estado=0;
static char informacionJuego[] ="Bienvenido al sudoku\nPara jugar puede introducir los siguientes comandos:\n$NEW para empezar una nueva partida\n$RST para detener la partida\n$FCVR donde F es la fila a introducir, C la columna y V el valor. R sera la suma de los 3 anteriores modulo 8\nDespues de introducir una jugada, durante 3 segundos se puede cancelar con el boton2 o confirmar con el boton1\nSi no se hace nada se confirma por defecto\nAl introducir una casilla invalida saldran errores en las que tengan el mismo valor\nSi en ese caso es una Pista saldra una X indicando que no se puede modificar\nPara iniciar una nueva partida introduzaca el comando #NEW!\n" ;
static int tiempo;
static int tiempo_computo=0;
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


static uint32_t estado_GPIO=0;


//static char mensajeFinal2[1000];
static char mensajeFinal[1000];
//static char mensajeInicial[1500];

//Variables para saber si estamos en jugada introducida por comandos o por GPIO
static uint8_t iComando;
static uint8_t jComando;
static uint8_t valorComando;
static int estoy_en_comando =0;		//Si es 0 significa que no esta en comando
static int conteos_introducir_jugada =0;

int sudoku_comprobar_tablero_lleno(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS]){
	//Se recorren filas y columnas y se ve si esta lleno
	int celdas_vacias=0;
	uint16_t celda;
	uint8_t valor_celda;
	uint8_t error_celda;
	for(int i=0;i<NUM_FILAS;i++){
		for(int j=0;j<NUM_FILAS;j++){
			celda = celda_leer_contenido(cuadricula_C_C[i][j]);
			valor_celda = celda_leer_valor(celda);
			error_celda = celda_leer_error(celda);
			if(valor_celda == 0 || error_celda == 1){
				celdas_vacias = celdas_vacias + 1;
			}
		}
	}
	return celdas_vacias;
}
/************************
Esta funcion devuelve el valor nuevo de los candidatos en cada celda en valorNuevo..*/
int valor_en_candidatos(uint16_t candidatos_celda, uint8_t valorNuevo){
	if( ((candidatos_celda >> (valorNuevo -1) ) & 0x1) == 0){
		return 1;
	}else{
		return 0;
	}
}

/************************
Esta funcion devuelve si el valor que le hemos pasado se pue modificadr, es decir si no es una pista y
su valor esta comprendido entre 0 y 9.*/
int se_puede_modificar(uint8_t pista, uint8_t valor){
	if(pista != 1 && (valor > 0 && valor <=9)){
		return 1;
	}else{
		return 0;
	}
}

/************************
Esta funcion inicializa el sudoku, es decir actualizamos el valor de la cuadricula, ademas calculamos 
el timpo en computo de dicha función*/
void sudoku_inicializar(){
		tiempo=timer1_temporizador_leer();
		candidatos_actualizar_c(cuadricula_C_C);	
		tiempo_computo=(timer1_temporizador_leer()-tiempo)+ tiempo_computo;
}

/************************
Esta funcion escribe el mensaje inicial de nuestro sudoku.*/
void sudoku_mensajeinicial(){
	gestor_serial_enviar_mensaje(informacionJuego);
}

/************************
Esta funcion reinicia el sudoku, es decir , resetea el valor de la cuadricula y vuelve a actualizarla.*/
void sudoku_reiniciar(){
	for(int i=0;i<NUM_FILAS;i++){
		for(int j=0;j<NUM_FILAS;j++){
			cuadricula_C_C[i][j] = cuadricula_C_C_Aux[i][j];
		}
	}
	tiempo=timer1_temporizador_leer();
	candidatos_actualizar_c(cuadricula_C_C);	
	tiempo_computo=(timer1_temporizador_leer()-tiempo)+ tiempo_computo;
}

/************************
Esta funcion ttransforma un entero num a una cadena de caracteres cadena.*/
void sudoku_mostrar_tiempo(int num,char cadena[]){
	char caracter[2];								//caracter donde se almacena el numero
	caracter[1]='\0';
	if (num==0){										//Numero es 0
		caracter[0]='0';
		strcat(cadena,caracter);			//Concatenamos el numero 0 a la cadena
	}else{
		while(num!=0){								//Mientra que el numero no sea 0
			caracter[0]=((num%10)+'0');	//numero = numero%10 + '0'
			strcat(cadena,caracter);		//Concatenamos un numero a la cadena
			num=num/10;									//Numero = numero /10
		}
	}
} 
/************************
Esta funcion envia un mensaje que determina porque se ha dejado de jugar .*/
void sudoku_fin_partida(char fin_partida[]){
	strcat(fin_partida, "\nEnhorabuena, has completado el sudoku con exito");
}
/************************
Esta funcion envia un mensaje que determina porque se ha dejado de jugar .*/
void sudoku_reset_partida(char reset_partida[]){
	strcat(reset_partida, "\nHas hecho un reset del sudoku\0");
}
/************************
Esta funcion envia un mensaje para avisar que se ha iniciado una nueva partida .*/
void sudoku_nueva_partida(char nueva_partida[]){
	strcat(nueva_partida, "\nHas iniciado una nueva partida\0");
}
/************************
Esta funcion nos informa del timpo total de la partida, el tiempo en computo de la función candaddatos_actualizar
y además nos pregunta si queremos volver a jugar una nueva partida.*/
void sudoku_tiempo_total_partida(char mensaje_tiempo[]){
	//Envios de mensajes
	strcat(mensaje_tiempo,"\nInformacion de la partida acabada: \0");
	strcat(mensaje_tiempo,"\nTiempo total de partida: \0");
	sudoku_mostrar_tiempo(RTC_leer_minutos(),mensaje_tiempo);
	strcat(mensaje_tiempo," minutos y  \0");
	sudoku_mostrar_tiempo(RTC_leer_segundos(),mensaje_tiempo);
	strcat(mensaje_tiempo,"  segundos \n\0");
	strcat(mensaje_tiempo, "El tiempo en computo de la funcion candidatos_actualizar es : \0");
	sudoku_mostrar_tiempo(tiempo_computo,mensaje_tiempo);
	strcat(mensaje_tiempo," microsegundos\0");
	strcat(mensaje_tiempo,"\nQuiere volver a jugar? Si es asi inicie una nueva partida con el comando #NEW!\n");
}                                             
	
/************************
Esta funcion realiza las acciones necesarias cuando presionamos el boton 1.*/
void sudoku_evento_boton1(){
		if(sudoku_estado != 0){
			gestor_pulsacion_boton1_pretado();	
			if(estoy_en_comando == 0){
				//Quitar alarma del idle
				double t0 = timer1_temporizador_leer();
				//gestor_pulsacion_boton1_pretado();			//ESTO estaba comentado
				uint8_t i = gestor_IO_leer_fila();
				uint8_t j = gestor_IO_leer_columna();
				uint8_t valor = gestor_IO_leer_valor_introducir();
				uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
				//Estas las hacemos en celda para leer ya que el gestor no debe saber nada de que bits leer 
				uint8_t pista = celda_leer_pista(celda); 
				uint8_t valor_celda = celda_leer_valor(celda);
				uint8_t error_celda = celda_leer_error(celda);
				uint16_t candidatos_celda = celda_leer_candidatos(celda);
				if(se_puede_modificar(pista,valor) == 1){	//Si la celda no es una pista inicial y el valor a introducir esta entre 0 y 9 se modifica la celda
					celda_actualizar_celda(&cuadricula_C_C[i][j],valor);
					if(valor != valor_celda && error_celda == 1){		//Si voy a introducir un valor y no habia error y el valor es distinto pues propago
						candidatos_actualizar_error_c(cuadricula_C_C,valor_celda);
					}
					candidatos_propagar_c(cuadricula_C_C,i,j);	//Tras insertar el valor, se propaga al resto de celdas
					if(valor_en_candidatos(candidatos_celda,valor) == 1){		//Si el valor introducido es correcto se activa el led de validacion
						gestor_IO_confirmar_escritura(); 
					}else{
						celda_modificar_bit_error(&cuadricula_C_C[i][j]);
						///////////////////////////////////////////////////////////////////////////////
					candidatos_actualizar_c(cuadricula_C_C);		//CAMBIADO//////////////////////////////////////////////////////////////////////////
					///////////////////////////////////////////////////////////////
						//celda_introducir_error(&cuadricula_C_C[i][j],valor);	//Si el valor introducido es erroneo se activa el bit de la celda que indica un valor erroneo
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
					tiempo=timer1_temporizador_leer();
					candidatos_actualizar_c(cuadricula_C_C);
					tiempo_computo=(timer1_temporizador_leer()-tiempo)+ tiempo_computo;
					}
				
					if(sudoku_comprobar_tablero_lleno(cuadricula_C_C) == 0){	//Si es 0 es que estan todas llenas
						disable_isr();
						disable_isr_fiq();
						cola_guardar_eventos(evento_rst,0);
						enable_isr();
						enable_isr_fiq();
					}
			}else{		//En este caso significa que estoy en comando
				//gestor_pulsacion_boton1_pretado();			//ESTO estaba comentado
				gestor_IO_quitar_led();		//Se quita el led que hemos puesto antes
				estoy_en_comando = 0;	//He confirmado el comando pues ya no estoy en el
				gestor_alarmas_quitar_confirmar_jugada();		//Se quita la alarma que salta a los 3 segundos
				//Como se ha confirmado pues se introduce y se propaga
				uint16_t celda = celda_leer_contenido(cuadricula_C_C[iComando][jComando]);
				uint8_t pista = celda_leer_pista(celda);
				uint8_t valor_celda = celda_leer_valor(celda);
				uint16_t candidatos_celda=celda_leer_candidatos(celda);
				uint8_t error_celda = celda_leer_error(celda);
				
				if(valorComando == 0){		//Se ha introducido un 0 entonces se borra el valor de la celda
					celda_introducir_celda(&cuadricula_C_C[iComando][jComando],0);	//Pones un 0 y se llama a actualizar
					if(valorComando != valor_celda && error_celda == 1){		//Si habia un error e introduzco un 0 pues quito el error de las demas
						candidatos_actualizar_error_c(cuadricula_C_C,valor_celda);
					}
					candidatos_actualizar_c(cuadricula_C_C);
				}else{
					if(se_puede_modificar(pista,valorComando) == 1 && (valorComando != valor_celda)){	//Si la celda no es una pista inicial y el valor a introducir esta entre 0 y 9 se modifica la celda
						//candidatos_actualizar_error_c(cuadricula_C_C,);
						celda_actualizar_celda(&cuadricula_C_C[iComando][jComando],valorComando);
						if(valorComando != valor_celda && error_celda == 1){		//Si la celda tenia un error se llama a esto
							candidatos_actualizar_error_c(cuadricula_C_C,valor_celda);
						}
						candidatos_propagar_c(cuadricula_C_C,iComando,jComando);	
						if(valor_en_candidatos(candidatos_celda,valorComando) == 1){		//Si el valor introducido es correcto se activa el led de validacion
						}else{
							celda_modificar_bit_error(&cuadricula_C_C[iComando][jComando]);
							///////////////////////////////////////////////////////////////////////////////
							candidatos_actualizar_c(cuadricula_C_C);		//CAMBIADO//////////////////////////////////////////////////////////////////////////
							///////////////////////////////////////////////////////////////
							//celda_introducir_error(&cuadricula_C_C[iComando][jComando], valorComando);	//Si el valor introducido es erroneo se activa el bit de la celda que indica un valor erroneo
						}
					}
				}
				sudoku_mostrar_tablero();
				if(sudoku_comprobar_tablero_lleno(cuadricula_C_C) == 0){	//Si es 0 es que estan todas llenas
					disable_isr();
					disable_isr_fiq();
					cola_guardar_eventos(evento_rst,0);
					enable_isr();
					enable_isr_fiq();
				}
			}
	}
}


/************************
Esta funcion sirve para visualizar las acciones en nuestra GPIO*/
void sudoku_evento_visualizacion_GPIO(){
		if(sudoku_estado != 0){
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
}



/************************
Función que propaga los datos de la cuadricula.*/
void candidatos_propagar_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],
	uint8_t fila, uint8_t columna)
{
		uint16_t celda;
		uint8_t valor_celda;
    uint8_t j, i , init_i, init_j, end_i, end_j;
    /* puede ayudar esta "look up table" a mejorar el rendimiento */
    const uint8_t init_region[NUM_FILAS] = {0, 0, 0, 3, 3, 3, 6, 6, 6};

    /* valor que se propaga */
    uint8_t valor = celda_leer_valor(cuadricula[fila][columna]);

    /* recorrer fila descartando valor de listas candidatos */
    for (j=0;j<NUM_FILAS;j++){
			//if(celda_leer_valor(cuadricula[fila][j]) == 0){
			celda_eliminar_candidato(&cuadricula[fila][j],valor);
			celda = celda_leer_contenido(cuadricula[fila][j]);
			valor_celda = celda_leer_valor(celda);
			if(valor == valor_celda && j!=columna){
				celda_modificar_bit_error(&cuadricula[fila][j]);
			}
			
		}
	//celda_eliminar_candidato(&cuadricula[fila][j],valor);

    /* recorrer columna descartando valor de listas candidatos */
    for (i=0;i<NUM_FILAS;i++){
			//if(celda_leer_valor(cuadricula[i][columna]) == 0){
			celda_eliminar_candidato(&cuadricula[i][columna],valor);			
			celda = celda_leer_contenido(cuadricula[i][columna]);
			valor_celda = celda_leer_valor(celda);
			if(valor == valor_celda && i!=fila){
				celda_modificar_bit_error(&cuadricula[i][columna]);
			}
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
				celda = celda_leer_contenido(cuadricula[i][j]);
				valor_celda = celda_leer_valor(celda);
				if(valor == valor_celda && j!=columna && i!=fila){
					celda_modificar_bit_error(&cuadricula[i][j]);
				}
			}
    }
}

/************************
Esta funcion actualiza la cuadricula .*/
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
	//Varible para sacar el error
	//uint16_t celda;
	//uint8_t error_celda;
	//recalcular candidatos de las celdas vacias calculando cuantas hay vacias
	for ( i=0; i<NUM_FILAS;i++){
		for (j=0; j <NUM_FILAS;j++){
			//celda=celda_leer_contenido(cuadricula[i][j]);
			//error_celda = celda_leer_error(celda);
			//Si esta vacia o hay error se suma 1 a celdas_vacias
			if (celda_leer_valor(cuadricula[i][j]) == 0){		//celda_leer_valor(cuadricula[i][j]) == 0
				celdas_vacias ++;
			}
			else{
				candidatos_propagar_c(cuadricula,i,j);
			}
		}	
	}
	return celdas_vacias; //retornar el numero de celdas vacias
}

/************************
Esta funcion realiza las acciones necesarias cuando presionamos el boton 2.*/
void sudoku_evento_boton2(){
		if(sudoku_estado != 0){
			gestor_pulsacion_boton2_pretado();			//ESTO estaba comentado
			if(estoy_en_comando == 0){
			uint8_t i = gestor_IO_leer_fila();
			uint8_t j = gestor_IO_leer_columna();
			uint8_t valor = gestor_IO_leer_valor_introducir();
			uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
			uint8_t pista = celda_leer_pista(celda); 
			uint16_t candidatos_celda = celda_leer_candidatos(celda);
				uint8_t valor_celda=celda_leer_valor(celda);
				uint8_t error_celda= celda_leer_error(celda);
			//sudoku_2021_borrar_valor(i,j);
			if(pista != 1){	//Si la celda no es una pista inicial se borra el valor
				celda_borrar_celda(&cuadricula_C_C[i][j]);
				if(error_celda == 1){		//En este caso el valor da igual porque solo borras
					candidatos_actualizar_error_c(cuadricula_C_C,valor_celda);
				}
				tiempo=timer1_temporizador_leer();
				candidatos_actualizar_c(cuadricula_C_C);	//Para evitar valores corruptos se vuelve a actualizar todo el valor
				tiempo_computo=(timer1_temporizador_leer()-tiempo)+ tiempo_computo;
				
			}
			//Si se introduce los valores fila=0, columna=0 y valor=0 acaba el programa
			if(gestor_IO_reiniciar(i,j,valor) == 1){
					sudoku_reiniciar();
				tiempo=timer1_temporizador_leer();
				candidatos_actualizar_c(cuadricula_C_C);	
				tiempo_computo=(timer1_temporizador_leer()-tiempo)+ tiempo_computo;
				}
			}else{
				gestor_IO_quitar_led();		//Se quita el led que hemos puesto antes
				estoy_en_comando = 0;	//He confirmado el comando pues ya no estoy en el
				gestor_alarmas_quitar_confirmar_jugada();		//Se quita la alarma que salta a los 3 segundos
				
				tiempo=timer1_temporizador_leer();
				candidatos_actualizar_c(cuadricula_C_C);	
				tiempo_computo=(timer1_temporizador_leer()-tiempo)+ tiempo_computo;
				sudoku_mostrar_tablero();
			}
	}
}

/************************
Esta funcion obtiene el valor de los candidatos.*/
uint16_t sudoku_get_valor_candidatos(uint8_t fila,uint8_t columna){
	return cuadricula_C_C[fila][columna]>>7 ;
}

/************************
Esta funcion nos mostrara el tablero en el cual realizaremos nuestro juego.*/
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
						if(((pista&0x1) == 0x1)&&((error&0x1)==1)){	//Si es pista y hay error se pone 0
							tablero[i][j] ='X';
						}else{
							
//							if(((error&0x1)==1)&&(valor!=0)){		//Significa que es valor introducido y he metido otra igual
//								tablero[i][j] ='X';
//							}else{
							if((pista&0x1) == 0x1){		//es pista
								tablero[i][j] ='P';
							}else{
								if((error&0x1)==1){//es error
									tablero[i][j] ='E';
								}else{
										if(tablero[i][j-1] != ' '){
												tablero[i][j]=' ';
										}else{
												tablero[i][j]=' ';
										}
								}
							}
						//}
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
			mensajeFinal[i*numColumnas + j] = tablero[i][j];
		}
	}
	//Ahora hay que poner los candidatos
	int indiceFinal = numFilas*numColumnas;
	uint8_t error_celda;
	uint8_t pista_celda;
	//int c1,c2,c3,c4,c5,c6,c7,c8,c9;	
	for(int i=0;i<NUM_FILAS;i++){		//Recorrer la matriz sacando los candidatos
		for(int j=0;j<NUM_FILAS;j++){
			celda = celda_leer_contenido(cuadricula_C_C[i][j]);
			candidatos= celda_leer_candidatos(celda);
			valor = celda_leer_valor(celda);
			error_celda = celda_leer_error(celda);
			pista_celda=celda_leer_pista(celda);
			if((valor == 0x0 || error_celda == 1) && pista_celda!=1){		//Significa que no hay valor y por tanto candidatos
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
				if(((candidatos >> 1) & 0x1) == 0){	
					mensajeFinal[indiceFinal]='2';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 2) & 0x1) == 0){	
					mensajeFinal[indiceFinal]='3';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 3) & 0x1) == 0){	
					mensajeFinal[indiceFinal]='4';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 4) & 0x1) == 0){	
					mensajeFinal[indiceFinal]='5';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 5) & 0x1) == 0){	
					mensajeFinal[indiceFinal]='6';
					indiceFinal = indiceFinal +1;
				}
				if(((candidatos >> 6) & 0x1) == 0){	
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
/************************
Esta funcion nos permite introducir una jugada en nuestro sudoku.*/
void sudoku_introducir_jugada(uint32_t aux){
		if(sudoku_estado != 0){
			estoy_en_comando = 1;
			uint8_t i = aux >> 16;
			uint8_t j = aux >> 8;
			uint8_t valor = aux;
			iComando = i;
			jComando = j;
			valorComando = valor;
			
		//Idea: guardar el estado de la celda, mostrar tablero y cuando acabe pues lo vuelves a meter
		
			uint16_t celda = celda_leer_contenido(cuadricula_C_C[i][j]);
			//Estas las hacemos en celda para leer ya que el gestor no debe saber nada de que bits leer 
			uint8_t pista = celda_leer_pista(celda);  
			uint16_t candidatos_celda = celda_leer_candidatos(celda);
			uint8_t valor_celda = celda_leer_valor(celda);
			uint8_t error_celda = celda_leer_error(celda);
			if(valorComando == 0){		//Se ha introducido un 0 entonces se borra el valor de la celda
				celda_introducir_celda(&cuadricula_C_C[iComando][jComando],0);	//Pones un 0 y se llama a actualizar
				if(valorComando != valor_celda && error_celda == 1){		//Si habia un error e introduzco un 0 pues quito el error de las demas
					candidatos_actualizar_error_c(cuadricula_C_C,valor_celda);
				}
				candidatos_actualizar_c(cuadricula_C_C);
			}else{
			if(se_puede_modificar(pista,valor) == 1 && (valor != valor_celda)){	//Si la celda no es una pista inicial y el valor a introducir esta entre 0 y 9 se modifica la celda	
				celda_actualizar_celda(&cuadricula_C_C[i][j],valor);
				//candidatos_propagar_c(cuadricula_C_C,i,j);	//Si quita el propagar porque a lo mejor luego se quita
				if(valor_en_candidatos(candidatos_celda,valor) == 1){		//Si el valor introducido es correcto se activa el led de validacion
				}else{
					//celda_introducir_error(&cuadricula_C_C[i][j], valor);	//Si el valor introducido es erroneo se activa el bit de la celda que indica un valor erroneo
					celda_modificar_bit_error(&cuadricula_C_C[i][j]);
				}
			}
		}	
			//Si se introduce los valores fila=0, columna=0 y valor=0 acaba el programa
		if(gestor_IO_reiniciar(i,j,valor) == 1){	//Esto a lo mejor hay que cambiarlo, no se si es enn el gestorIO
			//	parar = 1;
			//Reiniciar
			sudoku_reiniciar();
			tiempo=timer1_temporizador_leer();
			candidatos_actualizar_c(cuadricula_C_C);	//Creo que esta linea hay que quitarla porque ya lo hace la funcion reiniciar
			tiempo_computo=(timer1_temporizador_leer()-tiempo)+ tiempo_computo;
			}
		
			sudoku_mostrar_tablero();
			//Se pone una alarma de 3 segundos para confirmar
			//el formato del aux sera	00000001 0 00000000000101110111000
			
			//gestor_IO_confirmar_escritura();	//lo activa durante un segundo pero yo lo desactivare cuando llegue el evento de confirmar escritura
			gestor_IO_escribir_led();
			
			celda_introducir_celda(&cuadricula_C_C[i][j],celda);	//Vuelvo a escribir la celda porque la he cambiado y no se ha confirmado
			//cola_guardar_eventos(Set_Alarm,0x01000BB8);		//le meto un poco a la alarma para probarlo bien
			//cola_guardar_eventos(Set_Alarm,0x01008BB8);
			conteos_introducir_jugada = 0;
			disable_isr();
			disable_isr_fiq();
			cola_guardar_eventos(Set_Alarm,0x010001F4);				//Se pone a 500 para que parpadee
			enable_isr();
			enable_isr_fiq();
	}
}
/************************
Esta funcion nos muestra la informacion inicial al empezar una partida.*/
void sudoku_mostrar_tablero_inicial(){
gestor_serial_enviar_mensaje(informacionJuego);
}

/************************
Esta funcion nos confirma que nuestra jugada ha sido correcta .*/
//Significa que ha llegado la alarma de 3 segundos tras introducir la jugada
//Se muestra el tablero y se quita el led de confirmar escritura
void sudoku_confirmar_jugada(){
	if(conteos_introducir_jugada != 6){
		conteos_introducir_jugada = conteos_introducir_jugada +1;
		if(conteos_introducir_jugada%2==0){
			gestor_IO_escribir_led();
		}else{
			gestor_IO_quitar_led();
		}
		gestor_alarmas_quitar_alarma(evento_confirmar_jugada);
		disable_isr();
		disable_isr_fiq();
		cola_guardar_eventos(Set_Alarm,0x010001F4);	//Ponemos otra vez la alarma
		enable_isr();
		enable_isr_fiq();
	}else{
		estoy_en_comando = 0;
		gestor_IO_quitar_led();		//Se quita el led que hemos puesto antes
		uint16_t celda = celda_leer_contenido(cuadricula_C_C[iComando][jComando]);
				uint8_t pista = celda_leer_pista(celda);
				uint8_t valor_celda = celda_leer_valor(celda);
				uint16_t candidatos_celda=celda_leer_candidatos(celda);
				uint16_t error = celda_leer_error(celda);
		if(valorComando == 0){		//Se ha introducido un 0 entonces se borra el valor de la celda
			celda_introducir_celda(&cuadricula_C_C[iComando][jComando],0);	//Pones un 0 y se llama a actualizar
			if(valorComando != valor_celda && error == 1){		//Si habia un error e introduzco un 0 pues quito el error de las demas
				candidatos_actualizar_error_c(cuadricula_C_C,valor_celda);
			}
			candidatos_actualizar_c(cuadricula_C_C);
		}else{
			if(se_puede_modificar(pista,valorComando) == 1 && (valorComando != valor_celda)){	//Si la celda no es una pista inicial y el valor a introducir esta entre 0 y 9 se modifica la celda
				celda_actualizar_celda(&cuadricula_C_C[iComando][jComando],valorComando);
				if(valorComando != valor_celda && error == 1){		//Significa que no he vuelto a introducir el mismo valor y habia un error
					candidatos_actualizar_error_c(cuadricula_C_C,valor_celda);
				}
				if(valor_en_candidatos(candidatos_celda,valorComando) == 1){		//Si el valor introducido es correcto se activa el led de validacion
				}else{
					celda_modificar_bit_error(&cuadricula_C_C[iComando][jComando]);
					//En caso de introducir un error se deben mantener los candidatos para facilitar la resolucion al usuario
					candidatos_actualizar_c(cuadricula_C_C);
				}
				candidatos_propagar_c(cuadricula_C_C,iComando,jComando);	//Si quita el propagar porque a lo mejor luego se quita
			}
		}
				sudoku_mostrar_tablero();
		
				//Mirar si se han llenado todas las celdas para acabar
			if(sudoku_comprobar_tablero_lleno(cuadricula_C_C) == 0){	//Si es 0 es que estan todas llenas
				disable_isr();
				disable_isr_fiq();
				cola_guardar_eventos(evento_rst,0);
				enable_isr();
				enable_isr_fiq();
			}
	}
}

/************************
La funcion recorre todas las celdas para propagar aquellas celdas que ya no tienen error*/
void candidatos_actualizar_error_c(CELDA cuadricula[NUM_FILAS][NUM_COLUMNAS],uint8_t valor_introducido){
	 //int celdas_vacias = 0;
  uint8_t i;
	uint8_t j;
	//recalcular candidatos de las celdas vacias calculando cuantas hay vacias
	for ( i=0; i<NUM_FILAS;i++){
		for (j=0; j <NUM_FILAS;j++){
			candidatos_propagar_error_c(valor_introducido,i,j);
		}	
	}
}

/********************************************
La funcion recorre la fila, columna y región y dtecta si debe quitar error de las celdas con valor igual al parametro valor.*/
void
candidatos_propagar_error_c(uint8_t valor,uint8_t fila, uint8_t columna){
		uint16_t celda;
		uint8_t valor_celda;
		
    uint8_t j, i , init_i, init_j, end_i, end_j;
    /* puede ayudar esta "look up table" a mejorar el rendimiento */
    const uint8_t init_region[NUM_FILAS] = {0, 0, 0, 3, 3, 3, 6, 6, 6};

		int varios_detectados =0;
    
    for (j=0;j<NUM_FILAS;j++){
			celda = celda_leer_contenido(cuadricula_C_C[fila][j]);
			//Extraer el valor y ver si es el mismo
			valor_celda=celda_leer_valor(celda);
			if(valor_celda == valor){
				varios_detectados = varios_detectados +1;
			}	
		}
		
		for (i=0;i<NUM_FILAS;i++){
			celda = celda_leer_contenido(cuadricula_C_C[i][columna]);
			//Extraer el valor y ver si es el mismo
			valor_celda=celda_leer_valor(celda);
			if(valor_celda == valor && i!=fila){		//Para que no lea otra vez la celda desde la que se mira (ya se ha tenido en cuenta al recorrer la fila)
				varios_detectados = varios_detectados +1;
			}	
		}
		
		
		init_i = init_region[fila];
    init_j = init_region[columna];
    end_i = init_i + 3;
    end_j = init_j + 3;

    /* recorrer region descartando valor de listas candidatos */
    for (i=init_i; i<end_i; i++) {
      for(j=init_j; j<end_j; j++) {
				celda = celda_leer_contenido(cuadricula_C_C[i][j]);
				if(i != fila && j!=columna){
			//Extraer el valor y ver si es el mismo
			valor_celda=celda_leer_valor(celda);
			if(valor_celda == valor){
				varios_detectados = varios_detectados +1;
			
			}
		}
			}
    }
		//En el caso de dtectar solo a 1 se debe eliminar el error
		if(varios_detectados == 1){
			for (j=0;j<NUM_FILAS;j++){
				celda = celda_leer_contenido(cuadricula_C_C[fila][j]);
				//Extraer el valor y ver si es el mismo
				valor_celda=celda_leer_valor(celda);
				if(valor_celda == valor){				//Vuelves a recorrer y si el valor es el mismo lo quitas
					celda_quitar_bit_error(&cuadricula_C_C[fila][j]);
				}	
			}
		}

		if(varios_detectados == 1){
			for (i=0;i<NUM_FILAS;i++){
				celda = celda_leer_contenido(cuadricula_C_C[i][columna]);
				//Extraer el valor y ver si es el mismo
				valor_celda=celda_leer_valor(celda);
				if(valor_celda == valor){
					celda_quitar_bit_error(&cuadricula_C_C[i][columna]);
				}	
			}
		}
	
		if(varios_detectados == 1){
			for (i=init_i; i<end_i; i++) {
				for(j=init_j; j<end_j; j++) {
					celda = celda_leer_contenido(cuadricula_C_C[i][j]);
				//Extraer el valor y ver si es el mismo
					valor_celda=celda_leer_valor(celda);
					if(valor_celda == valor){
						celda_quitar_bit_error(&cuadricula_C_C[i][j]);
					}
				}
			}
		}
}
/************************
Esta funcion realiza las acciones necesarias para resetear la partida.*/
void sudoku_evento_rst(char mensaje[]){
		if(sudoku_estado != 0){
		sudoku_reset_partida(mensaje);
		gestor_serial_enviar_mensaje(mensaje);
		sudoku_tiempo_total_partida(mensaje);
		gestor_serial_enviar_mensaje(mensaje);
		//Se cambia el estado al inicial
		sudoku_cambiar_estado(0);
	}
}
/************************
Esta funcion realiza las acciones necesarias para empezar una nueva partida.*/
void sudoku_evento_new(char mensaje[]){
		sudoku_reiniciar();
		//sudoku_nueva_partida(mensaje);
		//gestor_serial_enviar_mensaje(mensaje);
		sudoku_mostrar_tablero();
		sudoku_cambiar_estado(2);
}
/************************
Esta funcion realiza las acciones necesarias para finalizar la partida.*/
void sudoku_evento_fin_partida(char mensaje[]){
	sudoku_fin_partida(mensaje);
	gestor_serial_enviar_mensaje(mensaje);
	sudoku_tiempo_total_partida(mensaje);
	gestor_serial_enviar_mensaje(mensaje);
}

/************************
La funcion actualiza el estado del sudoku*/
void sudoku_cambiar_estado(int nuevo_estado){
	sudoku_estado = nuevo_estado;
}

int sudoku_get_estado(){
	return sudoku_estado;
}


//0x0015, 0x0003, 0x0004, 0x0006, 0x0007, 0x0008, 0x0009, 0x0001, 0x0002, 0, 0, 0, 0, 0, 0, 0,
//0x0006, 0x0007, 0x0002, 0x0001, 0x0019, 0x0005, 0x0003, 0x0004, 0x0018, 0, 0, 0, 0, 0, 0, 0,
//0x0001, 0x0019, 0x0018, 0x0013, 0x0004, 0x0012, 0x0005, 0x0016, 0x0007, 0, 0, 0, 0, 0, 0, 0,
//0x0008, 0x0015, 0x0009, 0x0017, 0x0006, 0x0001, 0x0014, 0x0002, 0x0003, 0, 0, 0, 0, 0, 0, 0,
//0x0004, 0x0002, 0x0016, 0x0018, 0x0015, 0x0013, 0x0017, 0x0009, 0x0001, 0, 0, 0, 0, 0, 0, 0,
//0x0007, 0x0001, 0x0013, 0x0019, 0x0002, 0x0014, 0x0008, 0x0015, 0x0006, 0, 0, 0, 0, 0, 0, 0,
//0x0009, 0x0016, 0x0001, 0x0015, 0x0003, 0x0017, 0x0012, 0x0018, 0x0004, 0, 0, 0, 0, 0, 0, 0,
//0x0012, 0x0008, 0x0007, 0x0004, 0x0011, 0x0009, 0x0006, 0x0003, 0x0005, 0, 0, 0, 0, 0, 0, 0,
//0x0003, 0x0004, 0x0005, 0x0002, 0x0008, 0x0016, 0x0001, 0x0007, 0x0019, 0, 0, 0, 0, 0, 0, 0


//{
//0x0015, 0x0000, 0x0000, 0x0013, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
//0x0000, 0x0000, 0x0000, 0x0000, 0x0019, 0x0000, 0x0000, 0x0000, 0x0015, 0, 0, 0, 0, 0, 0, 0,
//0x0000, 0x0019, 0x0016, 0x0017, 0x0000, 0x0015, 0x0000, 0x0013, 0x0000, 0, 0, 0, 0, 0, 0, 0,
//0x0000, 0x0018, 0x0000, 0x0019, 0x0000, 0x0000, 0x0016, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
//0x0000, 0x0000, 0x0015, 0x0018, 0x0016, 0x0011, 0x0014, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
//0x0000, 0x0000, 0x0014, 0x0012, 0x0000, 0x0013, 0x0000, 0x0017, 0x0000, 0, 0, 0, 0, 0, 0, 0,
//0x0000, 0x0017, 0x0000, 0x0015, 0x0000, 0x0019, 0x0012, 0x0016, 0x0000, 0, 0, 0, 0, 0, 0, 0,
//0x0016, 0x0000, 0x0000, 0x0000, 0x0018, 0x0000, 0x0000, 0x0000, 0x0000, 0, 0, 0, 0, 0, 0, 0,
//0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0012, 0x0000, 0x0000, 0x0011, 0, 0, 0, 0, 0, 0, 0
//};



	//La idea es coger el array mensajeInicial y mensajeFinal y juntarlos
//	const int numFilas = 19;
//	const int numColumnas=29;
//	char tablero[numFilas][numColumnas];
//	for(int i=0;i<numFilas;i=i+2){
//		for(int j=0;j<numColumnas-1;j++){
//			if(j%3==0){
//				tablero[i][j] = '+';
//			}else{
//				tablero[i][j] = '-';
//			}
//		}
//	}
//	
//	int iCuadricula=0;
//	int jCuadricula=0;
//	uint16_t celda;
//	uint8_t pista;
//	uint8_t error;
//	uint8_t valor;
//	uint16_t candidatos;
//	for(int i=1;i<numFilas;i=i+2){
//		for(int j=0;j<numColumnas-1;j++){
//			if(j%3==0){
//				tablero[i][j] = '|';
//				if(j != numColumnas-2){		//parar que no tenga en cuenta el ultimo |
//					celda = celda_leer_contenido(cuadricula_C_C[iCuadricula][jCuadricula]);
//					pista = celda_leer_pista(celda); 
//					error = celda_leer_error(celda);
//					valor = celda_leer_valor(celda);
//					jCuadricula=jCuadricula+1;
//					if(jCuadricula==NUM_FILAS){		//j llega al final pues se pasa a la siguiente fila
//						iCuadricula=iCuadricula+1;
//						jCuadricula =0;
//					}
//				}
//				
//			}else{
//				if(j%3==1){		//es la |. ahi pones el valor
//					if(valor != 0){		//Si el valor no es 0 significa que se ha introducido algo
//						tablero[i][j] = valor + '0';
//					}else{
//						tablero[i][j] =' ';
//					}
//				}else{
//					if(j%3 == 2){		//es el caso |..
//						if((pista&0x1) == 0x1){		//es pista
//							tablero[i][j] ='P';
//						}else{
//							if((error&0x1)==1){//es error
//								tablero[i][j] ='E';
//							}else{
//									if(tablero[i][j-1] != ' '){
//											tablero[i][j]=' ';
//									}else{
//											tablero[i][j]=' ';
//									}
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//	
//	for(int i=0;i<numFilas;i++){					//Poner fin de linea
//		tablero[i][numColumnas-1] = '\n';
//	}
//	
//	for(int i=0;i<numFilas;i++){
//		for(int j=0;j<numColumnas;j++){
//			mensajeFinal2[i*numColumnas + j] = tablero[i][j];
//		}
//	}
//	//Ahora hay que poner los candidatos
//	int indiceFinal = numFilas*numColumnas;
//	//int c1,c2,c3,c4,c5,c6,c7,c8,c9;	
//	for(int i=0;i<NUM_FILAS;i++){		//Recorrer la matriz sacando los candidatos
//		for(int j=0;j<NUM_FILAS;j++){
//			celda = celda_leer_contenido(cuadricula_C_C[i][j]);
//			candidatos= celda_leer_candidatos(celda);
//			valor = celda_leer_valor(celda);
//			if(valor == 0x0){		//Significa que no hay valor y por tanto candidatos
//				mensajeFinal2[indiceFinal]=i+'0';
//				indiceFinal = indiceFinal +1;
//				mensajeFinal2[indiceFinal]=j+'0';
//				indiceFinal = indiceFinal +1;
//				mensajeFinal2[indiceFinal]=' ';
//				indiceFinal = indiceFinal +1;
//				if((candidatos&0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
//					mensajeFinal2[indiceFinal]='1';
//					indiceFinal = indiceFinal +1;
//				}
//				if(((candidatos >> 1) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
//					mensajeFinal2[indiceFinal]='2';
//					indiceFinal = indiceFinal +1;
//				}
//				if(((candidatos >> 2) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
//					mensajeFinal2[indiceFinal]='3';
//					indiceFinal = indiceFinal +1;
//				}
//				if(((candidatos >> 3) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
//					mensajeFinal2[indiceFinal]='4';
//					indiceFinal = indiceFinal +1;
//				}
//				if(((candidatos >> 4) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
//					mensajeFinal2[indiceFinal]='5';
//					indiceFinal = indiceFinal +1;
//				}
//				if(((candidatos >> 5) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
//					mensajeFinal2[indiceFinal]='6';
//					indiceFinal = indiceFinal +1;
//				}
//				if(((candidatos >> 6) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
//					mensajeFinal2[indiceFinal]='7';
//					indiceFinal = indiceFinal +1;
//				}
//				if(((candidatos >> 7) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
//					mensajeFinal2[indiceFinal]='8';
//					indiceFinal = indiceFinal +1;
//				}
//				if(((candidatos >> 8) & 0x1) == 0){	//Si el c1 es 1 significa que ahi hay un candidato
//					mensajeFinal2[indiceFinal]='9';
//					indiceFinal = indiceFinal +1;
//				}
//				mensajeFinal2[indiceFinal] = '\n';
//				
//				indiceFinal = indiceFinal +1;		
//			}
//		}
//	}
	
	//mensajeFinal2[indiceFinal]='\0';
//	strcpy(mensajeInicial, informacionJuego); /* copy name into the new var */
//	strcat(mensajeInicial, mensajeFinal2); /* add the extension */
//	gestor_serial_enviar_mensaje(mensajeInicial);


