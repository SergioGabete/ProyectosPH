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
#include "sudoku_2021.h"
#include "planificador.h"
#include "UART0.h"
//#include "cuadricula.h"


int main (void) {
	//planificador_init();
		//Se inicializan todos los perifericos del sistema
		gestor_pulsacion_eint_init();
		timer1_temporizador_iniciar();
		timer1_temporizador_empezar();
		timer0_temporizador_iniciar();
		timer0_temporizador_empezar();
		timer0_temporizador_periodico(1);
		gestor_alarmas_init();
		gestor_IO_iniciar();
		sudoku_inicializar();
		//Poner alarma para la visualizacion constante de la GPIO
		cola_guardar_eventos(Set_Alarm,0x068000C8);
		//Introducir una periodica que sea el idle
	//00000000 1 00000000000000000010100
	cola_guardar_eventos(Set_Alarm,0x00800014);
		///candidatos_actualizar_c(cuadricula_C_C);	//Esta funcion estara en el sudoku y habra que llamar a eso 　　　　　　　　　
		cola_guardar_eventos(Set_Alarm,0x02003A98);
		//Se llama alplanificador
		uart0_init();
//		uart0_sendchar(65);
		planificador_init();
		
		//Si ha habido un reset de la partida se actualiza el tablero a sus valores iniciales
		//sudoku_reiniciar();
		//Se reinician los timers
		
		//Aque se eberia poner lo de parar a 0
		//sudoku_retomar_ejecucion()
	
}

