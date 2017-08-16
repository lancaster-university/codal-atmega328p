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
