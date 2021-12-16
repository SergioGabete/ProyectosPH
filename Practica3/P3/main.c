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
#include "WT.h"
#include "RTC.h"
//#include "cuadricula.h"

//void PAbt_Handler_function(void) {}
//void DAbt_Handler_function(void) {}


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
		RTC_init();
		WT_init(20);
		//Poner alarma para la visualizacion constante de la GPIO
	__disable_irq();
		cola_guardar_eventos(Set_Alarm,0x068000C8);
	__enable_irq();
		//Introducir una periodica que sea el idle
	//00000000 1 00000000000000000010100
	__disable_irq();
	cola_guardar_eventos(Set_Alarm,0x00800014);
	__enable_irq();
		///candidatos_actualizar_c(cuadricula_C_C);	//Esta funcion estara en el sudoku y habra que llamar a eso 　　　　　　　　　
		__disable_irq();
		cola_guardar_eventos(Set_Alarm,0x02003A98);
		__enable_irq();
		//Se llama alplanificador
		uart0_init();
		//sudoku_mensajeinicial();
		sudoku_mostrar_tablero_inicial();
		uart0_sendchar('B');
		planificador_init();
		
		//Si ha habido un reset de la partida se actualiza el tablero a sus valores iniciales
		//sudoku_reiniciar();
		//Se reinician los timers
		
		//Aque se eberia poner lo de parar a 0
		//sudoku_retomar_ejecucion()
	
}

