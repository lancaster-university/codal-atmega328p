/*
The MIT License (MIT)

Copyright (c) 2017 Lancaster University.

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
  * Class definition for Pin.
  *
  * Commonly represents an I/O pin on the edge connector.
  */
#include "ATMegaSerial.h"
#include "ErrorNo.h"
#include "Event.h"
#include <avr/io.h>

using namespace codal;

/**
  * Constructor.
  */
ATMegaSerial::ATMegaSerial()
{
    // Set for 115200 baud 8N1 communication.
    UCSR0A = 0x02;
    UCSR0B = 0x08;
    UCSR0C = 0x06;
    UBRR0H = 0;
    UBRR0L = 16;
}

/**
 *
 * Send the given byte on this serial port.
 * This function will wait for any previous communication to complete before sending.
 *
 * @param c The character to send
 * @return DEVICE_OK on success.
 */
int ATMegaSerial::sendChar(char c)
{
    // Wait for empty transmit buffer
    while (!( UCSR0A & 0x20));

    // Put data into buffer, sends the data
    UDR0 = c;

    return DEVICE_OK;
}

/**
 *
 * Send the given string on this serial port.
 * This function will wait for any previous communication to complete before sending.
 *
 * @param c The character to send
 * @return DEVICE_OK on success.
 */
int ATMegaSerial::send(const char *s)
{
    while (*s != 0)
    {
        sendChar(*s);
        s++;
    }

    return DEVICE_OK;
}

/**
 *
 * Send the given number on this serial port.
 * This function will wait for any previous communication to complete before sending.
 *
 * @param n The number to send
 * @return DEVICE_OK on success.
 */
int ATMegaSerial::send(const uint16_t n)
{
    int sh = 12;

    send("0x");
    while (sh >= 0)
    {
        int d = (n >> sh) & 0xf;
        sendChar(d > 9 ? 'A' + d - 10 : '0' + d);

        sh -= 4;
    }

    return DEVICE_OK;
}

/**
 * Configures this serial port for the givn board rate.
 *
 * @param baud the new baud rate of this serial port, in bits per second.
 */
int ATMegaSerial::setBaud(int baud)
{
    //TODO.
    return DEVICE_OK;
}
