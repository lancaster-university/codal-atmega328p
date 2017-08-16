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

#ifndef ATMEGA_PIN_H
#define ATMEGA_PIN_H

#include "CodalConfig.h"
#include "Pin.h"

#define IO_STATUS_DIGITAL_IN                0x01        // Pin is configured as a digital input, with no pull up.
#define IO_STATUS_DIGITAL_OUT               0x02        // Pin is configured as a digital output
#define IO_STATUS_ANALOG_IN                 0x04        // Pin is Analog in
#define IO_STATUS_ANALOG_OUT                0x08        // Pin is Analog out
#define IO_STATUS_TOUCH_IN                  0x10        // Pin is a makey-makey style touch sensor
#define IO_STATUS_EVENT_ON_EDGE             0x20        // Pin will generate events on pin change
#define IO_STATUS_EVENT_PULSE_ON_EDGE       0x40        // Pin will generate events on pin change

/**
  * Class definition for Pin.
  *
  * Commonly represents an I/O pin on the edge connector.
  */
namespace codal
{
    class ATMegaPin : public Pin
    {
        private:

            void IOREG_SET(volatile uint8_t* const* REG);
            void IOREG_CLR(volatile uint8_t* const* REG);
            int  IOREG_IS_SET(volatile uint8_t* const* REG);

            /**
             * Disconnect any attached mBed IO from this pin.
             *
             * Used only when pin changes mode (i.e. Input/Output/Analog/Digital)
             */
            virtual void disconnect();

        public:

            /**
             * Constructor.
             * Create a DevicePin instance, generally used to represent a pin on the edge connector.
             *
             * @param id the unique EventModel id of this component.
             *
             * @param name the mbed PinName for this DevicePin instance.
             *
             * @param capability the capabilities this DevicePin instance should have.
             *                   (PIN_CAPABILITY_DIGITAL, PIN_CAPABILITY_ANALOG, PIN_CAPABILITY_AD, PIN_CAPABILITY_ALL)
             *
             * @code
             * DevicePin P0(DEVICE_ID_IO_P0, DEVICE_PIN_P0, PIN_CAPABILITY_ALL);
             * @endcode
             */
            ATMegaPin(PinName name, PinCapability capability);

            /**
             * Configures this IO pin as a digital output (if necessary) and sets the pin to 'value'.
             *
             * @param value 0 (LO) or 1 (HI)
             *
             * @return DEVICE_OK on success, DEVICE_INVALID_PARAMETER if value is out of range, or DEVICE_NOT_SUPPORTED
             *         if the given pin does not have digital capability.
             *
             * @code
             * DevicePin P0(DEVICE_ID_IO_P0, DEVICE_PIN_P0, PIN_CAPABILITY_BOTH);
             * P0.setDigitalValue(1); // P0 is now HI
             * @endcode
             */
            virtual int setDigitalValue(int value);

            /**
             * Configures this IO pin as a digital input (if necessary) and tests its current value.
             *
             *
             * @return 1 if this input is high, 0 if input is LO, or DEVICE_NOT_SUPPORTED
             *         if the given pin does not have digital capability.
             *
             * @code
             * DevicePin P0(DEVICE_ID_IO_P0, DEVICE_PIN_P0, PIN_CAPABILITY_BOTH);
             * P0.getDigitalValue(); // P0 is either 0 or 1;
             * @endcode
             */
            virtual int getDigitalValue();

            /**
             * Configures this IO pin as an analog/pwm output, and change the output value to the given level.
             *
             * @param value the level to set on the output pin, in the range 0 - 1024
             *
             * @return DEVICE_OK on success, DEVICE_INVALID_PARAMETER if value is out of range, or DEVICE_NOT_SUPPORTED
             *         if the given pin does not have analog capability.
             */
            virtual int setAnalogValue(int value);


            /**
             * Configures this IO pin as an analogue input (if necessary), and samples the Pin for its analog value.
             *
             * @return the current analogue level on the pin, in the range 0 - 1024, or
             *         DEVICE_NOT_SUPPORTED if the given pin does not have analog capability.
             *
             * @code
             * DevicePin P0(DEVICE_ID_IO_P0, DEVICE_PIN_P0, PIN_CAPABILITY_BOTH);
             * P0.getAnalogValue(); // P0 is a value in the range of 0 - 1024
             * @endcode
             */
            virtual int getAnalogValue();

            /**
             * Configures the PWM period of the analog output to the given value.
             *
             * @param period The new period for the analog output in microseconds.
             *
             * @return DEVICE_OK on success, or DEVICE_NOT_SUPPORTED if the
             *         given pin is not configured as an analog output.
             */
            virtual int setAnalogPeriodUs(uint32_t period);

            /**
             * Obtains the PWM period of the analog output in microseconds.
             *
             * @return the period on success, or DEVICE_NOT_SUPPORTED if the
             *         given pin is not configured as an analog output.
             */
            virtual uint32_t getAnalogPeriodUs();

            /**
             * Configures the pull of this pin.
             *
             * @param pull one of the mbed pull configurations: PullUp, PullDown, PullNone
             *
             * @return DEVICE_NOT_SUPPORTED if the current pin configuration is anything other
             *         than a digital input, otherwise DEVICE_OK.
             */
            virtual int setPull(PinMode pull);
    };
}


#endif
