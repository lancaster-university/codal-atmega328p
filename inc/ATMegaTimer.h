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

#ifndef ATMEGA_TIMER_H
#define ATMEGA_TIMER_H

#include "Timer.h"
#include "ErrorNo.h"

namespace codal
{
	class ATMegaTimer : public Timer
	{
	public:

		/**
		  * Constructor for a generic system clock interface.
		  */
		ATMegaTimer();

		/**
		  * Destructor for this Timer instance
		  */
		~ATMegaTimer();

		/**
		 * request to the physical timer implementation code to provide a trigger callback at the given time.
		 * note: it is perfectly legitimate for the implementation to trigger before this time if convenient.
		 * @param t Indication that t time units (typically microsends) have elapsed.
		 */
		virtual void triggerIn(CODAL_TIMESTAMP t);

		/**
		 * request to the physical timer implementation code to trigger immediately.
		 */
		virtual void syncRequest();

        void start();

        uint16_t    period;
        uint16_t    sigma;
        uint16_t    running;

	};
}

#endif
