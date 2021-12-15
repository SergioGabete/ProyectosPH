#include <LPC210x.H>                       /* LPC210x definitions */
//#include "Serial_port.h" 

void WT_init(int segundos){
	// first, it checks for a previous watchdog time out  
	//WDTOF (bit 2)The Watchdog Time-Out Flag is set when the watchdog times out. This flag is cleared by software.
	if( WDMOD & 0x04 ) {					   /* Check for watchdog time out. El bit 2 se activa si se ha disparado el watchdog*/
		WDMOD &= ~0x04;						   /* Clear time out flag           */
  }
	// Time out: Pclk*WDTC*4			//Esto hay que mirar los valores para poner bien el timeout
	// valor m?nimo WTC= 256; valor m?ximo 2^32 (si escribes algo menos que FF, se escribe FF)
  WDTC  = 15000000*segundos;						   // Set watchdog time out value
  WDMOD = 0x03;                            /* Enable watchdog timer (bit 0) and reset (bit 1).  */   
	// se puede trabajar en dos modos: reset dando el valor 11 a los dos bits menos significativos; Int: dando 10 (en ese caso el WT genera una interrupci?n
	// Una vez se da un valor a estos bits ?ya no se pueden cambiar a no ser que haya un reset externo, o la cuenta del WT acabe
	// Watchdog Timer Value register (WDTV) The WDTV register is used to read the current value of watchdog timer.
	
//	WT_init(int sec)		Esto es lo de binhui
//	 WDTC  = 0xE4E1C0*sec;						   
//  WDMOD = 0x03;  
}


void feed_watchdog (void) {				   /* Reload the watchdog timer       */
// esta es la secuencia necesaria para que el watchdog se reinicialice. Si no se alimenta al WT antes de que termine la cuenta resetear? el sistema (si est? habilitado para ello)
// aunque se active el watchdog, este no har? nada hasta que no se le alimente por primera vez
//Important! Interrupts must be disabled during the feed sequence. An abort condition will occur if an interrupt happens during the feed sequence
	
	//Aqui hay que proteger que se desactiven interrupciones y luego se vuelvan a activar
	__disable_fiq();
	__disable_irq();
  WDFEED = 0xAA;						   
  WDFEED = 0x55;  
	__enable_fiq();
	__enable_irq();
}

