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

int gestor_IO_leer_columna(void);

int gestor_IO_leer_fila(void);

int gestor_IO_leer_valor_introducir(void);

void gestor_IO_escribir_columna(int valor);

void gestor_IO_escribir_fila(int valor);

int gestor_IO_leer_celda(void);

void gestor_IO_escribir_celda(int valor);

int gestor_IO_leer_candidatos(void);

void gestor_IO_escribir_candidatos(int valor);

int gestor_IO_leer_led(void);

void gestor_IO_escribir_led(void);

int gestor_IO_reiniciar(uint8_t i, uint8_t j, uint8_t valor);

#endif 




