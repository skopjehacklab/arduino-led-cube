#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "cube.h"

// Define USART stuff
#define BAUD 38400
#include <util/setbaud.h>

volatile unsigned char current_layer;
#endif
