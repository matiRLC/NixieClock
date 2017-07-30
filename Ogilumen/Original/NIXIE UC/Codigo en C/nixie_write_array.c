// Programa converdito de Arduino a C
/*
 * nixie_write_array.c
 *
 * Paso en C de programa en .PDE (Arduino)
 * Informacion original:
 * nixie_write_array - sample code using the Nixie library 
                      for controlling the Ogi Lumen nixie tube driver.
   Created by Lionel Haims, July 25, 2008.
   Released into the public domain.
 
 */ 
 
// Revisar bien que tiene este header
// #include <Nixie.h>

//headers para trabajar con Atmega
#include <avr/io.h>
#include <avr/boot.h>
#include <avr/interrupt.h>

// pins conectados al uC

#define dataPin 2
#define clockPin 3
#define latchPin 4

//en realidad queremos usar 10 pero primero convertiremos
//el codigo para solo 4
#define numDigits 4

//definiciones simples

#define shortDelay 100
#define longDelay 5000

//Funciones utilizadas en todo el codigo
void ClearDigits(void);
void Loop(void);




//Pasar los numeros en el orden correcto
void Loop()
{
     int i = 0
     
     //creamos un array para guardar los digitos
     int arrayNums[numDigits];
     for(i=0;i<numDigits;i++)
     {
         arrayNums[1]=random(10)                        
     }
}



void ClearDigits(numDigits)
{
}
