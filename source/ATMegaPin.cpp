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
  * Class definition for Pin.
  *
  * Commonly represents an I/O pin on the edge connector.
  */
#include "ATMegaPin.h"
#include "Button.h"
#include "Timer.h"
#include "ErrorNo.h"
#include "Event.h"
#include <avr/io.h>

static volatile uint8_t* const DD_REG[] = {&DDRB, &DDRC, &DDRD};
static volatile uint8_t* const PORT_REG[] = {&PORTB, &PORTC, &PORTD};
static volatile uint8_t* const PIN_REG[] = {&PINB, &PINC, &PIND};

using namespace codal;

void ATMegaPin::IOREG_SET(volatile uint8_t* const* REG)
{
    *(REG[name >> 8]) |= (1 << (name & 0x7));
}

void ATMegaPin::IOREG_CLR(volatile uint8_t* const* REG)
{
    *(REG[name >> 8]) &= ~(1 << (name & 0x7));
}

int ATMegaPin::IOREG_IS_SET(volatile uint8_t* const* REG)
{
    return ((*(REG[name >> 8])) & (1 << (name & 0x7))) ? 1 : 0;
}

/**
  * Constructor.
  *
  * @param name the mbed PinName for this Pin instance.
  * @param capability the capabilities this Pin instance should have.
  *                   (PIN_CAPABILITY_DIGITAL, PIN_CAPABILITY_ANALOG, PIN_CAPABILITY_AD, PIN_CAPABILITY_ALL)
  *
  * @code
  * Pin P0(DEVICE_ID_IO_P0, DEVICE_PIN_P0, PIN_CAPABILITY_ALL);
  * @endcode
  */
ATMegaPin::ATMegaPin(PinNumber name, PinCapability capability) : Pin(DEVICE_ID_IO_P0 + name, name, capability)
{
    static int portsInitialized = 0;

    if (!portsInitialized)
    {
        // Configure for a 125khz ADC clock, usung Vcc as a reference and free running mode.
        ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
        ADCSRB = 0;

        portsInitialized = 1;
    }
}

/**
  * Disconnect any attached peripherals from this pin.
  * Used only when pin changes mode (i.e. Input/Output/Analog/Digital)
  */
void ATMegaPin::disconnect()
{
}

/**
  * Configures this IO pin as a digital output (if necessary) and sets the pin to 'value'.
  *
  * @param value 0 (LO) or 1 (HI)
  *
  * @return DEVICE_OK on success, DEVICE_INVALID_PARAMETER if value is out of range, or DEVICE_NOT_SUPPORTED
  *         if the given pin does not have digital capability.
  *
  * @code
  * Pin P0(DEVICE_ID_IO_P0, DEVICE_PIN_P0, PIN_CAPABILITY_BOTH);
  * P0.setDigitalValue(1); // P0 is now HI
  * @endcode
  */
int ATMegaPin::setDigitalValue(int value)
{
    // Check if this pin has a digital mode...
    if(!(PIN_CAPABILITY_DIGITAL & capability))
        return DEVICE_NOT_SUPPORTED;

    // Ensure we have a valid value.
    if (value < 0)
        return DEVICE_INVALID_PARAMETER;

    // Move into a Digital output state if necessary.
    if (!(status & IO_STATUS_DIGITAL_OUT)){
        disconnect();
        IOREG_SET(DD_REG);
        status |= IO_STATUS_DIGITAL_OUT;
    }

    // Write the value.
    if (value)
        IOREG_SET(PORT_REG);
    else
        IOREG_CLR(PORT_REG);

    return DEVICE_OK;
}

/**
  * Configures the pull of this pin.
  *
  * @param pull one of the mbed pull configurations: PullUp, PullDown, PullNone
  *
  * @return DEVICE_NOT_SUPPORTED if the current pin configuration is anything other
  *         than a digital input, otherwise DEVICE_OK.
  */
int ATMegaPin::setPull(PullMode pull)
{
    pullMode = pull;

    // If we're already configured as a digital input, then reconfigure hardware.
    // otherwise, this will happen automatically when the pin next comes up.
    if (status & IO_STATUS_DIGITAL_IN)
    {
        if (pullMode == PullMode::Up)
            IOREG_SET(PORT_REG);
        else
            IOREG_CLR(PORT_REG);
    }

    return DEVICE_OK;
}

/**
  * Configures this IO pin as a digital input (if necessary) and tests its current value.
  *
  *
  * @return 1 if this input is high, 0 if input is LO, or DEVICE_NOT_SUPPORTED
  *         if the given pin does not have digital capability.
  *
  * @code
  * Pin P0(DEVICE_ID_IO_P0, DEVICE_PIN_P0, PIN_CAPABILITY_BOTH);
  * P0.getDigitalValue(); // P0 is either 0 or 1;
  * @endcode
  */
int ATMegaPin::getDigitalValue()
{
    //check if this pin has a digital mode...
    if(!(PIN_CAPABILITY_DIGITAL & capability))
        return DEVICE_NOT_SUPPORTED;

    // Move into a Digital input state if necessary.
    if (!(status & IO_STATUS_DIGITAL_IN)){
        disconnect();
        IOREG_CLR(DD_REG);

        // If a pullmode has been configured, apply it now.
        if (pullMode == PullMode::Up)
            IOREG_SET(PORT_REG);
        else
            IOREG_CLR(PORT_REG);

        status |= IO_STATUS_DIGITAL_IN;
    }

    // Read the value.
    return IOREG_IS_SET(PIN_REG);
}

/**
  * Configures this IO pin as an analogue input (if necessary), and samples the Pin for its analog value.
  *
  * @return the current analogue level on the pin, in the range 0 - 1024, or
  *         DEVICE_NOT_SUPPORTED if the given pin does not have analog capability.
  *
  * @code
  * Pin P0(DEVICE_ID_IO_P0, DEVICE_PIN_P0, PIN_CAPABILITY_BOTH);
  * P0.getAnalogValue(); // P0 is a value in the range of 0 - 1024
  * @endcode
  */
int ATMegaPin::getAnalogValue()
{
    // Check if this pin has an analogue mode...
    if(!(PIN_CAPABILITY_ANALOG & capability))
        return DEVICE_NOT_SUPPORTED;

    // Move into an analogue input state if necessary.
    if (!(status & IO_STATUS_ANALOG_IN)){
        disconnect();
        ADMUX = (1 << REFS0) | (name & 0x07);
        ADCSRA |= ((1 << ADEN) | (1 << ADSC));
        status |= IO_STATUS_ANALOG_IN;
    }

    // Perform a blocking read.
    while((ADCSRA & (1 << ADIF)) == 0);
    ADCSRA |= (1 << ADIF);

    return (uint16_t)ADCH << 8 | ADCL;
}

/**
  * Configures this IO pin as an analog/pwm output, and change the output value to the given level.
  *
  * @param value the level to set on the output pin, in the range 0 - 1024
  *
  * @return DEVICE_OK on success, DEVICE_INVALID_PARAMETER if value is out of range, or DEVICE_NOT_SUPPORTED
  *         if the given pin does not have analog capability.
  */
int ATMegaPin::setAnalogValue(int value)
{
    //check if this pin has an analogue mode...
    if(!(PIN_CAPABILITY_DIGITAL & capability))
        return DEVICE_NOT_SUPPORTED;

    //sanitise the level value
    if(value < 0 || value > DEVICE_PIN_MAX_OUTPUT)
        return DEVICE_INVALID_PARAMETER;

    // TODO: Analogout
    return DEVICE_NOT_SUPPORTED;
}

/**
  * Configures the PWM period of the analog output to the given value.
  *
  * @param period The new period for the analog output in microseconds.
  *
  * @return DEVICE_OK on success.
  */
int ATMegaPin::setAnalogPeriodUs(uint32_t period)
{
    int ret;

    if (!(status & IO_STATUS_ANALOG_OUT))
    {
        // Drop this pin into PWM mode, but with a LOW value.
        ret = setAnalogValue(0);
        if (ret != DEVICE_OK)
            return ret;
    }

    //TODO
    return DEVICE_NOT_SUPPORTED;
}

/**
  * Obtains the PWM period of the analog output in microseconds.
  *
  * @return the period on success, or DEVICE_NOT_SUPPORTED if the
  *         given pin is not configured as an analog output.
  */
uint32_t ATMegaPin::getAnalogPeriodUs()
{
    if (!(status & IO_STATUS_ANALOG_OUT))
        return DEVICE_NOT_SUPPORTED;

    // TODO
    return DEVICE_NOT_SUPPORTED;
}

