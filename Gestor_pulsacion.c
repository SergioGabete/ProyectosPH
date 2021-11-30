#include "Gestor_pulsacion.h"
#include <LPC210x.H>
#include "Gestor_Alarmas.h"

static volatile unsigned int estado_pulsacion_0 = NO_PULSADO;
static volatile unsigned int estado_pulsacion_1 = NO_PULSADO;
//static unsigned int entrada = NO_PULSADO;

/************************
Esta función inicializa EINT1 Y EINT2 .*/
void gestor_pulsacion_eint_init(){
	eint1_init();
	eint2_init();
}

/************************
Esta función resetea el valor de eint_1 , es decir, pone el valor de eint1_pulsacion a 0  .*/
void gestor_pulsacion_clear_nueva_pulsacion_0(void){
	button_clear_nueva_pulsacion_1();
}

/************************
Dicha función devuelve un entero cuando ocurra una nueva pulsación en EINT1 .*/
unsigned int gestor_pulsacion_nueva_pulsacion_0(void){
	unsigned int new;
	new = button_nueva_pulsacion_1();
	button_clear_nueva_pulsacion_1(); // Las pulsaciones s?lo deben procesarse una vez. Por tanto se pone a 0 despu?s de leerlo
	return new;
}


/************************
Esta funcion actualiza el valor del estado de pulsación, es decir , si se pulsa EINT1, el estado seguirá en PULSADO, 
en cambio, sino esta pulsado pasará a NO PULSADO.*/
void gestor_pulsacion_actualizar_estado_0(void){
	EXTINT =  EXTINT | 2;        // clear interrupt flag de EINT1     
	if ((EXTINT & 2) == 2){ // si el boton esta pulsado, la instrucci?n de arriba no hara nada y EXTINT valdr? 1. Si el bot?n no est? pulsado valdr? 0
		estado_pulsacion_0 = PULSADO;
	}
	else{
		estado_pulsacion_0 = NO_PULSADO;
		// si no esta pulsado se habilitan las interrupciones (antes ya se ha limpiado el de EXTINT)
		VICIntEnable = VICIntEnable | 0x00008000; // Enable EXTINT0 Interrupt (la interrupcion del boton se deshabilita a si misma, al terminar la pulsaci?n hay ue volver a habilitarla)
	}
}


/************************
Esta función actualiza el estado y devuelve si el EINT1 esta pulsado o en caso contrario no lo esta  .*/
unsigned int gestor_pulsacion_leer_estado_0(void){
	gestor_pulsacion_actualizar_estado_0();
	return estado_pulsacion_0; 
}

//****************

/************************
Esta función resetea el valor de eint_2 , es decir, pone el valor de eint2_pulsacion a 0  .*/
void gestor_pulsacion_clear_nueva_pulsacion_1(void){
	button_clear_nueva_pulsacion_2();
}

/************************
Dicha función devuelve un entero cuando ocurra una nueva pulsación en EINT2 .*/
unsigned int gestor_pulsacion_nueva_pulsacion_1(void){
	unsigned int new;
	new = button_nueva_pulsacion_2();
	button_clear_nueva_pulsacion_2(); // Las pulsaciones s?lo deben procesarse una vez. Por tanto se pone a 0 despu?s de leerlo
	return new;
}

/************************
Esta funcion actualiza el valor del estado de pulsación, es decir , si se pulsa EINT2, el estado seguirá en PULSADO, 
en cambio, sino esta pulsado pasará a NO PULSADO.*/
void gestor_pulsacion_actualizar_estado_1(void){
	EXTINT =  EXTINT | 4;        // clear interrupt flag de EINT2     
	if ((EXTINT & 4) == 4){ // si el boton esta pulsado, la instrucci?n de arriba no hara nada y EXTINT valdra 1. Si el bot?n no est? pulsado valdr? 0
		estado_pulsacion_1 = PULSADO;
	}
	else{
		estado_pulsacion_1 = NO_PULSADO;
		// si no esta pulsado se habilitan las interrupciones (antes ya se ha limpiado el de EXTINT)
		VICIntEnable = VICIntEnable | 0x00010000; // Enable EXTINT0 Interrupt (la interrupci?n del bot?n se deshabilita a si misma, al terminar la pulsaci?n hay ue volver a habilitarla)
	}
}

/************************
Esta función actualiza el estado y devuelve si el EINT2 esta pulsado o en caso contrario no lo esta  .*/
unsigned int gestor_pulsacion_leer_estado_1(void){
	gestor_pulsacion_actualizar_estado_1();
	return estado_pulsacion_1; 
}

void gestor_pulsacion_boton1_pretado(){
	gestor_pulsacion_nueva_pulsacion_0();		//Meter estas 2 y el setalarm en el gestor pulsacion
	gestor_pulsacion_actualizar_estado_0();
	cola_guardar_eventos(Set_Alarm,0x04800064);
}

void gestor_pulsacion_boton2_pretado(){
	gestor_pulsacion_nueva_pulsacion_1();	//Son del gestor			//Meter estas 2 en el gestor y el evento setAlarm
		gestor_pulsacion_actualizar_estado_1();	//Son del gestor
		cola_guardar_eventos(Set_Alarm,0x05800064);	//Meter con esas 2 en el gestor	
}

void gestor_pulsacion_alarma_boton1(void){
	if(gestor_pulsacion_leer_estado_0()==PULSADO){
			//Si llega la alarma y el boton sigue pulsado no se hace nada
		}
		else{
			//En caso de que no este pulsado se actualiza el estado a no pulsado
			gestor_alarmas_quitar_alarma(evento_alarma_pulsaciones_1);
			gestor_pulsacion_clear_nueva_pulsacion_0();
			gestor_pulsacion_actualizar_estado_0();
		}
}


void gestor_pulsacion_alarma_boton2(void){
	if(gestor_pulsacion_leer_estado_1()==PULSADO){
			//Si llega la alarma y el boton sigue pulsado no se hace nada
		}
		else{
			//En caso de que no este pulsado se actualiza el estado a no pulsado
			gestor_pulsacion_actualizar_estado_1();
			gestor_pulsacion_clear_nueva_pulsacion_1();
			gestor_alarmas_quitar_alarma(evento_alarma_pulsaciones_2);
		}
}


