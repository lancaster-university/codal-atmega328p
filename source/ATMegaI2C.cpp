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

#include "ATMegaI2C.h"
#include "ErrorNo.h"
#include "avr/io.h"

#define TWSR_MASK 0xFC
#define TWSR_START 0x08
#define TWSR_REPEATED_START 0x10
#define TWSR_ADDR_ACK 0x18
#define TWSR_ADDR_NACK 0x20

#define TWI_DONE (TWCR & (1 << TWINT))

#define TWSR_START_COMPLETE ((TWSR & TWSR_MASK) == TWSR_START)

namespace codal
{
/**
  * Constructor.
  */
ATMegaI2C::ATMegaI2C(ATMegaPin &sda, ATMegaPin &scl) : I2C(sda, scl)
{
    setFrequency(100000);
}

/** Set the frequency of the I2C interface
  *
  * @param frequency The bus frequency in hertz
  */
int ATMegaI2C::setFrequency(uint32_t frequency)
{
    uint32_t b = (8000000 / frequency) - 8;
    TWBR = (uint8_t)b;
    TWSR = 0x00;

    return DEVICE_OK;
}

/**
 * Issues a START condition on the I2C bus
 */
int ATMegaI2C::start()
{
    // Initiate a start condition
    TWCR = ((1 << TWINT) | (1 << TWSTA) | (1 << TWEN));

    // Wait for start condition to be transmitted.
    while (!TWI_DONE);

    if (!TWSR_START_COMPLETE)
        return DEVICE_I2C_ERROR;

    return DEVICE_OK;
}

/**
 * Issues a STOP condition on the I2C bus
 */
int ATMegaI2C::stop()
{
    // Initiate a stop condition
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);

    // Wait for start condition to be transmitted.
    // while (!TWI_DONE);
    return DEVICE_OK;
}

/**
* Writes the given byte to the I2C bus.
*
* The CPU will busy wait until the transmission is complete.
*
* @param data The byte to write.
* @return DEVICE_OK on success, DEVICE_I2C_ERROR if the the write request failed.
*/
int ATMegaI2C::write(uint8_t data)
{
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!TWI_DONE);

    return DEVICE_OK;
}

/**
* Reads a single byte from the I2C bus.
* The CPU will busy wait until the transmission is complete.
*
* @return the byte read from the I2C bus, or DEVICE_I2C_ERROR if the the write request failed.
*/
int ATMegaI2C::read(AcknowledgeType ack)
{
    if (ack == ACK)
        TWCR = (1 << TWEA) | (1 << TWINT) | (1 << TWEN);
    else
        TWCR = (1 << TWINT) | (1 << TWEN);

    while (!TWI_DONE);

    return TWDR;
}

}
