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

/*
clique semiclique , aleatorios, onef , crea ciudad aleatoria y la conecta a todos, tialon es crear una matriz de todo 0
Si vemos algun caso cuando estas ahciendo el algoritmo pues lo metes y se lo dices en la memoria

La cosa es que el setcover esto son las ciudades
Oscar dice que puede sernlogn

*/
