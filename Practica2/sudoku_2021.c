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

Añadir mas de 8 tipos de alarmas	

Hacer lo del latido idle				HECHO


El sudoku puede llamar a gestor_IO y demas
El unico que conoce que es una fila o columna es el sudoku
El planificador simplemente recibe un evento y lo envia
Poner la cuadricula en sudoku
El idle simplemente es del planificador, sera una alarma que despertara al procesador y ya.
Al estar procesando quitas la alarma y cuando acabas la vuelves a poner (esta alarma quita el tic y lo vuelve a poner)

*/

