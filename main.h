#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "cube.h"

// Define USART stuff
#define FOSC 14745600
#define BAUD 38400
#define MYUBRR (((((FOSC * 10) / (16L * BAUD)) + 5) / 10) - 1)

#define DATA_BUS PORTA
#define LAYER_SELECT PORTC
#define LATCH_ADDR PORTB
#define LATCH_MASK 0x07
#define LATCH_MASK_INV 0xf8
#define OE_PORT PORTB
#define OE_MASK 0x08

// Red led on D2
#define LED_RED 0x04
// Green led D3
#define LED_GREEN 0x08
// Program led on D4
#define LED_PGM 0x10;
// Leds connected to port D
#define LED_PORT PORTD
// Rs232 button on D5
#define RS232_BTN 0x20
// Main button on B4
#define MAIN_BTN 0x10 

void ioinit (void);
void bootmsg (void);

volatile unsigned char current_layer;
volatile unsigned char pgm_mode;
void rs232(void);
unsigned int bootwait (void);
#endif

