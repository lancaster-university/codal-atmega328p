/*
The MIT License (MIT)

Copyright (c) 2016 British Broadcasting Corporation.
This software is provided by Lancaster University by arrangement with the BBC.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

/**
  * Definitions for the Device system timer.
  *
  * This module provides:
  *
  * 1) a concept of global system time since power up
  * 2) a simple periodic multiplexing API for the underlying mbed implementation.
  *
  * The latter is useful to avoid costs associated with multiple mbed Ticker instances
  * in codal components, as each incurs a significant additional RAM overhead (circa 80 bytes).
  */

#include "CodalCompat.h"
#include "ATMegaTimer.h"
#include "ATMegaSerial.h"
#include "ErrorNo.h"
#include <avr/io.h>
#include <avr/interrupt.h>

#define MINIMUM_PERIOD 100

#define LED PB5
#define output_low(port,pin) port &= ~(1<<pin)
#define output_high(port,pin) port |= (1<<pin)
#define set_input(portdir,pin) portdir &= ~(1<<pin)
#define set_output(portdir,pin) portdir |= (1<<pin)

using namespace codal;

static ATMegaTimer *instance = NULL;

extern ATMegaSerial *SERIAL_DEBUG;

ISR(TIMER1_COMPA_vect)
{
    if (instance)
    {
        instance->syncRequest();

        OCR1A = instance->period;
        TCNT1 = 0;
        instance->sigma = 0;

        instance->trigger();
    }
}


/**
 * Constructor for a generic system clock interface.
 */
ATMegaTimer::ATMegaTimer() : Timer()
{
    running = 0;

    // Set default periof of every 10000 us (nice number).
    period = 20000;
    sigma = 0;

	// Set up timer 1 for 0.5us precision.
    TCCR1B = 0x02;

    // Reset counter.
    TCNT1 = 0;

    // Configure for compare match on channelA.
    TIMSK1 = 0x02;

	// Configure initial trigger event period.
    OCR1A = period;

    // Enable interrupts
    sei();

    // record a handle on this object for our ISR(s) to use.
    instance = this;
}

/**
* request to the physical timer implementation code to provide a trigger callback at the given time.
* note: it is perfectly legitimate for the implementation to trigger before this time if convenient.
* @param t Indication that t time units (typically microsends) have elapsed.
*/
void ATMegaTimer::triggerIn(CODAL_TIMESTAMP t)
{
	// Configure hardware timer to interrupt in at most 1 microseconds (overriding any previous setting)
    // Care to clamp within ranhge of the 16 bit timer though.
    
    if (!running)
        return;

    if (t < MINIMUM_PERIOD)
        t = MINIMUM_PERIOD;

    period = (t << 1) < (CODAL_TIMESTAMP)0xffff ? (t << 1) : 0xffff;
    sigma = 0;

    //SERIAL_DEBUG->send("REQUEST_TRIGGER_IN:");
    //uint16_t a = (t & (0xffff0000)) >> 16;
    //uint16_t b = t & 0xffff;

    //SERIAL_DEBUG->send(a);
    //SERIAL_DEBUG->send(b);
    //SERIAL_DEBUG->send("\n");

    //SERIAL_DEBUG->send("TRIGGER_IN:");
    //SERIAL_DEBUG->send(period);
    //SERIAL_DEBUG->send("\n");

    TCNT1 = 0;
    OCR1A = period;
}

/**
* request to the physical timer implementation code to trigger immediately.
*/
void ATMegaTimer::syncRequest()
{
    if (!running)
        return;

	// Snapshot timer
    uint16_t snapshot = TCNT1;
    uint16_t elapsed = (snapshot - sigma) >> 1;
    sigma = snapshot;

    //SERIAL_DEBUG->send("ELAPSED:");
    //SERIAL_DEBUG->send(elapsed);
    //SERIAL_DEBUG->send("\n");

    sync(elapsed);
}

/**
 * Start the timer running
 */
void ATMegaTimer::start()
{
    running = 1;
}

/**
 * Destructor for this Timer instance
 */
ATMegaTimer::~ATMegaTimer()
{
}


