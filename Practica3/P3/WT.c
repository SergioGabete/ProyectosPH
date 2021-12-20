#include <LPC210x.H>                       /* LPC210x definitions */
//#include "Serial_port.h" 

void wt_rsi(void) __irq;

/************************
Esta funcion : inicializa el watchdog para que resetee el procesador dentro de
 segundos si no se le “alimenta”.*/
void WT_init(int segundos){
	// first, it checks for a previous watchdog time out  
	//WDTOF (bit 2)The Watchdog Time-Out Flag is set when the watchdog times out. This flag is cleared by software.
	//Limpia el flag si había disparado el watchdog en la partida anterior
	if( WDMOD & 0x04 ) {					   /* Check for watchdog time out. El bit 2 se activa si se ha disparado el watchdog*/
		WDMOD &= ~0x04;						   /* Clear time out flag           */
  }
	//Ajusta el WDTC para contar los sec segundos
  WDTC  = 1500000*segundos;						   // Set watchdog time out value
	//Activa el bit de enable y reset
  WDMOD = 0x03;                            /* Enable watchdog timer (bit 0) and reset (bit 1).  */   
	// se puede trabajar en dos modos: reset dando el valor 11 a los dos bits menos significativos; Int: dando 10 (en ese caso el WT genera una interrupci?n
	// Una vez se da un valor a estos bits ?ya no se pueden cambiar a no ser que haya un reset externo, o la cuenta del WT acabe
	// Watchdog Timer Value register (WDTV) The WDTV register is used to read the current value of watchdog timer.
}

/************************
Esta funcion alimenta al watchdog timer, escribiendo 0xAA y 0x55 en WDFEED; se
deshabilita las interrupciones para garantizar escrituras consecutivas.*/
void feed_watchdog (void) {				   /* Reload the watchdog timer       */
	
	//Aqui hay que proteger que se desactiven interrupciones y luego se vuelvan a activar
	__disable_fiq();
	__disable_irq();
  WDFEED = 0xAA;						   
  WDFEED = 0x55;  
	__enable_fiq();
	__enable_irq();
}

