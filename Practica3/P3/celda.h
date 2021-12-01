/* guarda para evitar inclusiones múltiples (include guard) */
#ifndef CELDA_H
#define CELDA_H

#include <inttypes.h>


/* Cada celda se codifica en 16 bits
 * bits [15,7]: los 9 bits más significativos representan el vector de candidatos,
 * si el bit 7 + valor - 1 está a 0, valor es candidato, 1 en caso contrario
 * bit 6: no empleado
 * bit 5: error
 * bit 4: pista
 * bits [3,0]: valor de la celda
 */

enum { BIT_CANDIDATOS = 7 };

typedef uint16_t CELDA;
/*
static int NUM_FILAS = 9;
static int NUM_COLUMNAS = 16;

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
};*/

/* *****************************************************************************
 * elimina el candidato del valor almacenado en la celda indicada */
__inline static void celda_eliminar_candidato(CELDA *celdaptr, uint8_t valor)
{
    *celdaptr = *celdaptr | (0x0001 << (BIT_CANDIDATOS + valor - 1));
}
/*
__inline static void celda_eliminar_candidato(CELDA celdaptr, uint8_t valor)
{
    celdaptr = celdaptr | (0x0001 << (BIT_CANDIDATOS + valor - 1));
}*/

/* *****************************************************************************
 * modifica el valor almacenado en la celda indicada */

__inline static void
celda_poner_valor(CELDA *celdaptr, uint8_t val)
{
    *celdaptr = (*celdaptr & 0xFFF0) | (val & 0x000F);
}


/* *****************************************************************************
 * extrae el valor almacenado en los 16 bits de una celda */
__inline static uint8_t
celda_leer_valor(CELDA celda)
{
    return (celda & 0x000F);
}

/* *****************************************************************************
 * extrae el valor almacenado en los 16 bits de una celda */
__inline static uint16_t
celda_leer_contenido(CELDA celda)
{
    return celda ;
}

/********************************************************************
Los 9 candidatos de cada celda se ponen a 0 para indicar
que cualquier numero puede ser candidato*/
__inline static void celda_inicializar_candidatos(CELDA *celdaptr){
	*celdaptr = (*celdaptr & 0x007F);
}
/***********************************************************
* sustituye el valor de los 16 bits de celda por valor*/
__inline static void celda_actualizar_celda(CELDA *celdaptr, uint16_t valor){
        *celdaptr = valor;
		
}

/***********************************************************
* sustituye el valor de los 16 bits de celda por 0*/
__inline static void celda_borrar_celda(CELDA *celdaptr){
		*celdaptr = 0;
}

/***********************************************************
* Lee el bit pista de la celda*/
__inline static uint8_t celda_leer_pista(CELDA celdaptr){
		return (celdaptr >> 4) & 0x01; 
}

/***********************************************************
* Lee los candidatos de la celda*/
__inline static uint16_t celda_leer_candidatos(CELDA celdaptr){
		return (celdaptr >> 7) & 0x01ff;
}

/***********************************************************
* introduce una celda erronea*/
__inline static void celda_introducir_error(CELDA *celdaptr){
		*celdaptr = 0x0020;
}

__inline static uint8_t celda_leer_error(CELDA celdaptr){
		return (celdaptr >> 5) & 0x01;
}
#endif // CELDA_H
