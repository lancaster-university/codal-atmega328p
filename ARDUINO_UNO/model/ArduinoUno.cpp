/*
The MIT License (MIT)

Copyright (c) 2016 Lancaster University, UK.

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

#include "CodalConfig.h"
#include "ArduinoUno.h"

using namespace codal;

/**
  * Constructor.
  *
  * Create a representation of a GenuinoZero device, which includes member variables
  * that represent various device drivers used to control aspects of the micro:bit.
  */
ArduinoUno::ArduinoUno() :
    timer(),
    serial(),
    io(),
    messageBus()
{
    // Clear our status
    status = 0;

    // Start our system timer running.
    timer.start();

    // Bring up fiber scheduler.
    scheduler_init(messageBus);

    messageBus.listen(DEVICE_ID_MESSAGE_BUS_LISTENER, DEVICE_EVT_ANY, this, &ArduinoUno::onListenerRegisteredEvent);

    for(int i = 0; i < DEVICE_COMPONENT_COUNT; i++)
    {
        if(CodalComponent::components[i])
            CodalComponent::components[i]->init();
    }
}

/**
  * A listener to perform actions as a result of Message Bus reflection.
  *
  * In some cases we want to perform lazy instantiation of components, such as
  * the compass and the accelerometer, where we only want to add them to the idle
  * fiber when someone has the intention of using these components.
  */
void ArduinoUno::onListenerRegisteredEvent(Event evt)
{
}
