/*
 *  Code to controll an 8x8x8 ledcube using arduino
 */
#include "main.h"
#include "effect.h"
#include "launch_effect.h"
#include "draw.h"

void toggle_layer_clock(void) {
    // quickly toggle the pin (very quickly)
    PORTC |= _BV(PC4);
    PORTC &= ~_BV(PC4);
}

void reset_layer_counter(void) { // non-inverting
    // quickly toggle the pin (very quickly)
    PORTC |= _BV(PC5);
    PORTC &= ~_BV(PC5);
}


void output_data(uint8_t addr, uint8_t data) {
    PORTC = addr;

    // send 8 bits on the data bus
    PORTD = (data << 2);
    PORTB = (data >> 6);

    // toggle the address bus
    if (addr == 7)
        PORTC = 0x00;
    else
        PORTC = addr+1;
}

void output_enable(uint8_t state) {
    if (state == 0)
        PORTC &= ~_BV(PC3);
    if (state == 1)
        PORTC |= _BV(PC3);
}

void reset_data_buffers(void) {
    for(int i=0; i<8; i++) {
        output_data(0x00, i);
    }
}

/* initialize timer interrupt */
void init_isr_timers(void) {
   /* OC1A/OC1B disconnected */
   TCCR1A = 0;
   /* CTC mode, top in OCR1A / Timer clock = CLK/1024 */
   TCCR1B = (1<<WGM12)|(1<<CS12)|(1<<CS10);
   /* set OCR1A top value for ~ 480 Hz (~ 60 Hz for whole cube) */
   /*   32 for CPU @ 16 MHz */
   TCNT1 = 0x00; // start counting at 0
   OCR1A = 32;
   /* enable Timer/Counter 1 interrupt */
   TIMSK1 = (1<<OCIE1A);
}


/* timer/counter 1 interrupt handler */
ISR(TIMER1_COMPA_vect) {
    if (current_layer == 0) {
        reset_layer_counter();
    }
    for(int k=0; k<8; k++) {
        output_data(k, cube[current_layer][k]);
    }
    toggle_layer_clock();
    current_layer++;
    if (current_layer == 8) {
        // on the next refresh will also reset the layer counter
        current_layer = 0;
    }
}


void setup_ports(void) {
    DDRC = 0x3F;        // 0b00111111 HCF Demux output(3) + OE + RST + CLK
    DDRD = 0xFC;        // 0b11111100 Data bus
    DDRB = 0x03;        // 0b00000011 Data bus

    PORTC = ~0x3F;      // PORTC = 0b00000000;
    PORTC |= _BV(PC3);  // OE on - inverting
}

void setup_usart(void) {
    UCSR0B |= (1<<RXEN0) | (1<<TXEN0); // enable USART
    UBRR0H = UBRRH_VALUE;           // These are calculated by setbaud.h
    UBRR0L = UBRRL_VALUE;           // from the value of BAUD in main.h
}


void setup(void) {
    setup_ports();
    setup_usart();
    reset_layer_counter();
    init_isr_timers();
    current_layer = 0;
    sei();
}

int main (void) {
    setup();
    while (1) {
        // Show the effects in a predefined order
        for (int i=0; i<EFFECTS_TOTAL; i++)
            launch_effect(i);
        // Show the effects in a random order.
        // Comment the two lines above and uncomment this
        // if you want the effects in a random order.
        //launch_effect(rand()%EFFECTS_TOTAL);
    }
}


