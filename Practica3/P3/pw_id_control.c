#include "pw_id_control.h"


/************************
Funcion que pone al sistema en modo power down*/
void powerdown_procesador (void)  {
  EXTWAKE |= 6; // EXTINT0 will awake the processor
	PCON |= 0x02;
	Switch_to_PLL();
}

/************************
Funcion que pone al sistema en modo idle*/
void idle_procesador(void){				
  EXTWAKE |= 6;
	PCON |= 0x01;
}
