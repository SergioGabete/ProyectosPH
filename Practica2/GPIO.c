#include <LPC210x.H> 
#include "GPIO.h"

/************************
Esta función permite iniciar y utilizar la GPIO, además debe ser inbvocada antes de
llamar a cualquier función de la GPIO, para que se proceda al inicio de ella.*/
void GPIO_iniciar(void){ 
		IOCLR = 0xFFFFFFFF;
		//IODIR = 0x00FF0000;
		IODIR = 0xFFFFFFFF;
}
/************************
bit_inicial indica el primer bit a leer.
num_bits indica el numero de bits a leer.
Esta función devuelve el valor de los bits que queremos leer empezando por el bit_inicial y continuenda num_bits 
a la izquierda.*/
int GPIO_leer(int bit_inicial, int num_bits){ 	
    int lectura=0;
		int bit_indicados=IOPIN>>bit_inicial;
    while(num_bits!=0){
        lectura=(lectura<<1) +1;
        num_bits--;
    }
		bit_indicados=bit_indicados & lectura;
    return bit_indicados;
}
/************************
bit_inicial indica el primer bit a leer.
num_bits indica el numero de bits a escribir.
Escribe en los bits indicados el valor (si valor no puede representarse en los bits indicados se escribirá 
los num_bits menos significativos a partir del inicial) .*/
void GPIO_escribir( int bit_inicial, int num_bits, int valor){

	unsigned long pines=valor<<bit_inicial;
	int bit,i;
	for(i=bit_inicial;i<bit_inicial+num_bits;i++){
		bit=pines & (1<<i);
		if(bit==0){
			IOCLR=(1<<i);
		}else {
			IOSET=(1<<i);
		}
	}
}

/************************
bit_inicial indica el primer bit a leer.
num_bits indica el numero de bits a marcar como entrada.
Esta funcion marcara como pines de entrada los bits indicados por el usuario .*/
void GPIO_marcar_entrada(int bit_inicial, int num_bits){
    int inicial=bit_inicial;
    while(num_bits!=0){
        IODIR &= (0<<inicial);//IODIR OR desplazamiento lógico a la izquierda de inicial para indicar en IODIR los pines de entrada
				num_bits--;
				inicial++;
    }
	
}

/************************
bit_inicial indica el primer bit a leer.
num_bits indica el numero de bits a marcar como salida.
Esta funcion marcara como pines de salida los bits indicados por el usuario .*/
void GPIO_marcar_salida(int bit_inicial, int num_bits){ 		
    int inicial=bit_inicial;	
    while(num_bits!=0){				
        IODIR |= (1<<inicial); //IODIR OR desplazamiento lógico a la izquierda de inicial para indicar en IODIR los pines de salida 
				num_bits--;//decrementamos el num_bits ya que ya hemos marcado un pin de salida
				inicial++;//incrementamos el bit_inicial hacia la izquierda
    }
}

