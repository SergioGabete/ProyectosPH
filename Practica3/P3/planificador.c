#include "planificador.h"
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
#include "Gestor_Serial.h"
#include "WT.h"
#include "SWI.h"
//#include "cuadricula.h"

//static int parar =0;
static char mensaje[100];
//static int reset =1;	//variable que sirve para avisar que hemos llegado a reset

void planificador_init(){
	struct evento evento_sin_tratar;
		//Prueba del watchdog
	while(1){
		cola_guardar_eventos(0,0);
		feed_watchdog();
	}
		while(1){
			if(cola_comprobar_nuevos_eventos() == 1){					//Si hay eventos nuevos sin tratar se desencola un evento
					feed_watchdog();
				//disable_isr();
				//disable_isr_fiq();
					evento_sin_tratar = cola_evento_sin_tratar();
				//enable_isr();
				//enable_isr_fiq();
					planificador_tratar_evento(evento_sin_tratar);
		 }else{
				//Si no hay eventos a tratar se pasa a modo idle
				//gestor_IO_activar_iddle();
				idle_procesador();					
			}
		}
	
}


/****************************************
*	La funcion realiza una accion dependiendo del evento que le llegue al planificador*/
void planificador_tratar_evento(struct evento evento_sin_tratar){
	//La cosa es que al resetear o al hacer new habra que limpiar la cola y el gestor de alarmas
	//porque habra alarmas o eventos introducidos pero no los queremos
	switch(evento_sin_tratar.ID_evento){
		case resta_Periodos:
			gestor_alarmas_restar_periodo();
			break;
		case Set_Alarm:
			gestor_alarmas_introducir_evento(evento_sin_tratar.auxData);
			break;
		case evento_boton1:
			//gestor_pulsacion_boton1_pretado();
			sudoku_evento_boton1();
			break;
		case evento_boton2:
			//gestor_pulsacion_boton2_pretado();
			sudoku_evento_boton2();
			break;
		case evento_alarma_pulsaciones_1:
			gestor_pulsacion_alarma_boton1();
			break;
		case evento_alarma_pulsaciones_2:
			gestor_pulsacion_alarma_boton2();
			break;
		case evento_visualizacion_GPIO:
			sudoku_evento_visualizacion_GPIO();
			break;
		case  evento_visualizar_led:
			gestor_IO_quitar_led();
			break;
		case evento_power_down:
			//Se para el modo idle
			gestor_alarmas_quitar_alarma(evento_idle);
			powerdown_procesador();
			gestor_alarmas_activar_alarma(evento_idle);
			break;
		case evento_idle:
			gestor_IO_evento_idle();
			break;
		case evento_rst:
			sudoku_evento_rst(mensaje);
			break;
		case evento_new:
			sudoku_evento_new(mensaje);
			break;
		case evento_jugada:
			sudoku_introducir_jugada(evento_sin_tratar.auxData);								//No se si se deberia meter esa funcion nueva de sudoku o usar cosas de la GPIO
			break;
		case evento_continuar_mensaje:
			gestor_serial_continuar_mensaje();
			break;
		case evento_reset_power_down:
			gestor_alarmas_resetear_power_down();
			break;
		case evento_confirmar_jugada:
			sudoku_confirmar_jugada();
			break;
		case evento_fin_partida:
			sudoku_evento_fin_partida(mensaje);
			break;
		case evento_letra_introducida:
			sudoku_evento_letra_introducida(evento_sin_tratar.auxData);
			break;
		default:
			;
			break;
	}
	
	//Poner alarma del idle
	//Llega una alarma de tipo latido que la vaya quitando
		
}


/*

@ __enable_isr
@ 				MSR cpsr_c, r8
@ 				BIC r8, r8, #I_Bit
@ 				MSR cpsr_c, r8				   ; Enable IRQ ;MRS cambiao
@                 LDMFD   SP!, {R8, R12}         ; Load R8, SPSR, Pop from a Full Descending Stack.
@                 MSR     SPSR_cxsf, R12         ; Set SPSR
@                 LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return

@ __disable_isr

@ 				MSR cpsr_c, r8
@ 				ORR r8, r8, #I_Bit
@ 				MSR cpsr_c, r8				   ; Disable IRQ
@                 LDMFD   SP!, {R8, R12}         ; Load R8, SPSR, Pop from a Full Descending Stack.
@                 MSR     SPSR_cxsf, R12         ; Set SPSR
@                 LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return
				
@ __enable_isr_fiq
@ 				MSR cpsr_c, r8
@ 				BIC r8, r8, #F_Bit
@ 				MSR cpsr_c, r8				   ; Enable FIQ
@                 LDMFD   SP!, {R8, R12}         ; Load R8, SPSR, Pop from a Full Descending Stack.
@                 MSR     SPSR_cxsf, R12         ; Set SPSR
@                 LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return

@ __disable_isr_fiq
@ 				MSR cpsr_c, r8
@ 				ORR r8, r8, #F_Bit
@ 				MSR cpsr_c, r8				   ; Disable FIQ
@                 LDMFD   SP!, {R8, R12}         ; Load R8, SPSR, Pop from a Full Descending Stack.
@                 MSR     SPSR_cxsf, R12         ; Set SPSR
@                 LDMFD   SP!, {R12, PC}^        ; Restore R12 and Return

*/

