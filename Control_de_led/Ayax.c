
/*
 * File:   Arkantos.c
 * Author: Aldah
 *
 * Created on 20 de noviembre de 2025, 05.37 pm
 */
//pwm_reusase



#define _XTAL_FREQ 2000000 
#include <xc.h>
#include "PICF877AConfig_1.h"

#define DEBOUNCE_MS 20

// Lectura  antirrebote en RB1
unsigned char get_input_debounced(void) {
    unsigned char a = PORTBbits.RB1;
    __delay_ms(DEBOUNCE_MS);
    unsigned char b = PORTBbits.RB1;

    if (a == b)
        return b;      
    else
        return 0xFF;    // inestable
}

void main(void) 
{
    TRISBbits.TRISB1 = 1;   // RB1 como entrada
    TRISCbits.TRISC7 = 0;   // RC7 como salida

    PORTCbits.RC7 = 0;      // LED inicialmente apagado

    while (1)
    {
        unsigned char stable = get_input_debounced();

        if (stable != 0xFF) {   

            if (stable == 1) {
                PORTCbits.RC7 = 1;  
            } else { // stable == 0
                PORTCbits.RC7 = 0;   
            }
        }
    }
}
