#include "Gestor_IO.h"
#include "GPIO.h"
#include <stddef.h>
#include "cola.h"
#include "timer.h"
#include "evento.h"
#include "Gestor_Alarmas.h"
#include <LPC210x.H> 

/************************
Funci�n que inicializa el gestor IO que sirve de enlace entre el planificador y la GPIO y sus pines*/
void gestor_IO_iniciar(void){
	GPIO_iniciar();
	GPIO_marcar_entrada(14,14);	//Pones como entrada los bits eint1, eint2, fila, columna, nuevo valor a introducir
	//Se pone una alarma cada 200 milisegundos que sea periodica
}

/************************
Funci�n que devuelve el n�mero que vamos a leer con la GPIO*/
int gestor_IO_leer(int bit_inicial, int num_bits){
	return GPIO_leer(bit_inicial,num_bits);
}


/************************
Funci�n que devuelve el n�mero que vamos a escribir con la GPIO*/
void gestor_IO_escribir( int bit_inicial, int num_bits, int valor){
	GPIO_escribir(bit_inicial,num_bits,valor);
}


/************************
Funci�n que se va a encargar de marcar los pines de entrada de la GPIO*/
void gestor_IO_marcar_entrada(int bit_inicial, int num_bits){
	GPIO_marcar_entrada(bit_inicial,num_bits);
}


/************************
Funci�n que se va a encargar de marcar los pines de salida de la GPIO*/
void gestor_IO_marcar_salida(int bit_inicial, int num_bits){
	GPIO_marcar_salida(bit_inicial,num_bits);
}


//NUEVOOOO ****************************************

/************************
Funci�n que se va a encargar de leer la fila seleccionada del sudoku */
int leer_fila(){
			return GPIO_leer(16,4);
}

/************************
Funci�n que se va a encargar de leer la columna seleccionada del sudoku */
int leer_columna(){
			return GPIO_leer(20,4);
}

/************************
Funci�n que se va a encargar de escribir valor en la columna seleccionada*/
void escribir_columna(int valor){
			GPIO_escribir(20,4,valor);
}

/************************
Funci�n que se va a encargar de escribir valor en la fila seleccionada*/
void escribir_fila(int valor){
			GPIO_escribir(16,4,valor);
}

/************************
Funci�n de leer el valor de la celda seleccionda */
int leer_Celda(){
			return GPIO_leer(0,4);
}
/************************
Funci�n de escribir el valor en la celda seleccionda */
void escribir_Celda(int valor){
			GPIO_escribir(0,4,valor);
}

/************************
Funci�n de lee los candidatos de la celda  */
int leerCandidatos(){
	return GPIO_leer(4,9);
}

/************************
Funci�n que escribe los candidatos de la celda */
void escribirCandidatos(int valor){
	GPIO_escribir(4,9,valor);
}

/************************
Funci�n que lee si hay un error en la celda */
int leerFallo(){
	return GPIO_leer(13,1);
}

/************************
Funci�n que escribe en la celda de fallo */
void escribirFallo(int valor){
	GPIO_escribir(13,1,valor);
}

