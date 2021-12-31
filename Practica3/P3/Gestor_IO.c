#include "Gestor_IO.h"
#include "GPIO.h"
#include <stddef.h>
#include "cola.h"
#include "timer.h"
#include "evento.h"
#include "Gestor_Alarmas.h"
#include <LPC210x.H> 
#include "SWI.h"

uint8_t estado_idle_Global=0;

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
int gestor_IO_leer_fila(){
			return GPIO_leer(16,4);
}

/************************
Funci�n que se va a encargar de leer la columna seleccionada del sudoku */
int gestor_IO_leer_columna(){
			return GPIO_leer(20,4);
}

/************************
Funci�n que se va a encargar de escribir valor en la columna seleccionada*/
void gestor_IO_escribir_columna(int valor){
			GPIO_escribir(20,4,valor);
}

/************************
Funci�n que se va a encargar de escribir valor en la fila seleccionada*/
void gestor_IO_escribir_fila(int valor){
			GPIO_escribir(16,4,valor);
}

/************************
Funci�n de leer el valor de la celda seleccionda */
int gestor_IO_leer_celda(){
			return GPIO_leer(0,4);
}

/************************
Funci�n que lee el valor que quiere introducir el usuario  */
int gestor_IO_leer_valor_introducir(){
	return GPIO_leer(24,4);
}
/************************
Funci�n de escribir el valor en la celda seleccionda */
void gestor_IO_escribir_celda(int valor){
			GPIO_escribir(0,4,valor);
}

/************************
Funci�n de lee los candidatos de la celda  */
int gestor_IO_leer_candidatos(){
	return GPIO_leer(4,9);
}

/************************
Funci�n que escribe los candidatos de la celda */
void gestor_IO_escribir_candidatos(int valor){
	GPIO_escribir(4,9,valor);
}

/************************
Funci�n que lee si hay un error en la celda */
int gestor_IO_leer_led(){
	return GPIO_leer(13,1);
}

/************************
Funci�n que escribe en la celda de fallo */
void gestor_IO_escribir_led(){
	GPIO_escribir(13,1,1);
}

void gestor_IO_quitar_led(){
	GPIO_escribir(13,1,0);
}

int gestor_IO_reiniciar(uint8_t i, uint8_t j, uint8_t valor){
	if((i==0)&&(j==0)&&(valor==0)){
		return 1;
	}else{
		return 0;
	}
}

/************************
Tras una escritura correcta se marca el led durante 1 segundo */
void gestor_IO_confirmar_escritura(){
	GPIO_escribir(13,1,1);
	//disable_isr();
	//disable_isr_fiq();
	cola_guardar_eventos(Set_Alarm, 0x070003e8);
	//enable_isr();
	//enable_isr_fiq();
}

/************************
Función que activará el boton de iddle en la GPIO */
void gestor_IO_activar_iddle(){
	GPIO_escribir(31,1,1);
}

/************************
Función que desactivará el boton de iddle en la GPIO */
void gestor_IO_desactivar_iddle(){
	GPIO_escribir(31,1,0);
}

uint32_t gestor_IO_leer_estado(){
	return GPIO_leer(0,32);
}

void gestor_IO_evento_idle(){
	//uint8_t estado_idle = GPIO_leer(31,1);
	if(estado_idle_Global == 1){
		gestor_IO_desactivar_iddle();
		estado_idle_Global = 0;
	}else{
		gestor_IO_activar_iddle();
		estado_idle_Global = 1;
	}
}

