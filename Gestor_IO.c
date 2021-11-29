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


//#include "Gestor_IO.h"
//#include "GPIO.h"
//#include <stddef.h>
//#include "cola.h"
//#include "timer.h"
//#include "evento.h"
//#include "Gestor_Alarmas.h"
//#include <LPC210x.H> 

//void gestor_IO_iniciar(void){
//	GPIO_iniciar();
//	GPIO_marcar_entrada(14,14);	//Pones como entrada los bits eint1, eint2, fila, columna, nuevo valor a introducir
//	//Se pone una alarma cada 200 milisegundos que sea periodica
//}

//int gestor_IO_leer(int bit_inicial, int num_bits){
//	return GPIO_leer(bit_inicial,num_bits);
//}

//void gestor_IO_escribir( int bit_inicial, int num_bits, int valor){
//	GPIO_escribir(bit_inicial,num_bits,valor);
//}

//void gestor_IO_marcar_entrada(int bit_inicial, int num_bits){
//	GPIO_marcar_entrada(bit_inicial,num_bits);
//}

//void gestor_IO_marcar_salida(int bit_inicial, int num_bits){
//	GPIO_marcar_salida(bit_inicial,num_bits);
//}

