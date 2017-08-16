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

#ifndef ATMEGA_SREIAL_H
#define ATMEGA_SREIAL_H

#include "CodalConfig.h"
#include "CodalComponent.h"
#include "Pin.h"


/**
  * Class definition for an ATMEGA USART Serial Port
  */
namespace codal
{
    class ATMegaSerial : public CodalComponent
    {
        private:

        public:

            /**
             * Constructor.
             */
            ATMegaSerial();

            /**
             *
             * Send the given byte on this serial port.
             * This function will wait for any previous communication to complete before sending.
             *
             * @param c The character to send
             * @return DEVICE_OK on success.
             */
            int sendChar(char c);

            /**
             *
             * Send the given string on this serial port.
             * This function will wait for any previous communication to complete before sending.
             *
             * @param c The character to send
             * @return DEVICE_OK on success.
             */
            int send(const char *s);

            /**
             *
             * Send the given number on this serial port.
             * This function will wait for any previous communication to complete before sending.
             *
             * @param n The number to send
             * @return DEVICE_OK on success.
             */
            int send(const uint16_t n);

            /**
             * Configures this serial port for the givn board rate.
             *
             * @param baud the new baud rate of this serial port, in bits per second.
             */
            int setBaud(int baud);
    };
}


#endif
