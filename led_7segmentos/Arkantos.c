
/*
 * File:   Arkantos.c
 * Author: Aldah
 *
 * Created on 20 de noviembre de 2025, 07:50 pm
 */
//pwm_reusase


#include <xc.h>
#define _XTAL_FREQ 20000000
#include "PICF877AConfig_1.h"
#define DEBOUNCE_MS 20

// 7 segmentos 
const unsigned char DISPLAY[10] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

// RB2 antirrebote sencillo
unsigned char get_input_debounced(void) {
    unsigned char a = PORTBbits.RB2;
    __delay_ms(DEBOUNCE_MS);
    unsigned char b = PORTBbits.RB2;

    if (a == b) {
        return b;       // estable: 0 o 1
    } else {
        return 0xFF;    // inestable
    }
}


void display_write(unsigned char digit) {
    if (digit > 9) return;  

    unsigned char pattern_cc = DISPLAY[digit];      
    unsigned char pattern_ca = (~pattern_cc) & 0x7F; // invertir para ánodo común 

    // RA1 = a
    PORTAbits.RA1 = (pattern_ca >> 0) & 1;
    // RA2 = b
    PORTAbits.RA2 = (pattern_ca >> 1) & 1;
    // RA3 = c
    PORTAbits.RA3 = (pattern_ca >> 2) & 1;
    // RA4 = d
    PORTAbits.RA4 = (pattern_ca >> 3) & 1;
    // RA5 = e
    PORTAbits.RA5 = (pattern_ca >> 4) & 1;

    // RE0 = f
    PORTEbits.RE0 = (pattern_ca >> 5) & 1;
    // RE1 = g
    PORTEbits.RE1 = (pattern_ca >> 6) & 1;
}

void main(void) {
    //digital
    ADCON1 = 0x07;

    // RB2 como entrada (pulsador)
    TRISBbits.TRISB2 = 1;

    // Pines del display como salida
    TRISAbits.TRISA1 = 0;
    TRISAbits.TRISA2 = 0;
    TRISAbits.TRISA3 = 0;
    TRISAbits.TRISA4 = 0;
    TRISAbits.TRISA5 = 0;

    TRISEbits.TRISE0 = 0;
    TRISEbits.TRISE1 = 0;

    unsigned char counter = 0;
    unsigned char prev = 0;

    
    display_write(counter);

    while (1) {
        unsigned char stable = get_input_debounced(); // 0, 1 o 0xFF

        if (stable != 0xFF) {                
            if (prev == 0 && stable == 1) {   // flanco de subida en RB2
                counter++;
                if (counter == 10) {          // después de 9 vuelve a 0
                    counter = 0;
                }
                display_write(counter);
            }
            prev = stable;                    // actualizar estado 
        }
    }
}
