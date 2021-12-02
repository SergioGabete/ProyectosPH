#include <LPC210X.H> 
#include "Gestor_Interrupciones.h"

void gestor_interrupciones_activar_interrupciones(){
	__enable_fiq();
	__enable_irq();
}

void gestor_interrupciones_desactivar_interrupciones(){
	__disable_fiq();
	__disable_irq();
}

