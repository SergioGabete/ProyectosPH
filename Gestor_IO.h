#ifndef GESTOR_IO_H
#define GESTOR_IO_H

#include "pulsacion.h"
#include "evento.h"



//int gestor_pulsacion(int entrada);
void gestor_IO_iniciar(void);

int gestor_IO_leer(int bit_inicial, int num_bits);

void gestor_IO_escribir( int bit_inicial, int num_bits, int valor);

void gestor_IO_marcar_entrada(int bit_inicial, int num_bits);

void gestor_IO_marcar_salida(int bit_inicial, int num_bits);

int leer_columna(void);

int leer_fila(void);

void escribir_columna(int valor);

void escribir_fila(int valor);

int leer_Celda(void);

void escribir_Celda(int valor);

int leerCandidatos(void);

void escribirCandidatos(int valor);

int leerFallo(void);

void escribirFallo(int valor);

#endif 




