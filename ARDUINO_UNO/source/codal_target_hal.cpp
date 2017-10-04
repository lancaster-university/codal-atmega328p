#include "codal_target_hal.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/wdt.h>
#include "ATMegaSerial.h"

void target_enable_irq()
{
    sei();
}

void target_disable_irq()
{
    cli();
}

void target_wait_for_event()
{
    target_disable_irq();
    sleep_enable();
    target_enable_irq();
    sleep_cpu();
    sleep_disable();
}

void target_wait(uint32_t milliseconds)
{
}

void target_reset()
{
    target_disable_irq();
    wdt_enable(WDTO_15MS);
    while (1) {}
}

void target_panic(int statusCode)
{
    target_disable_irq();
    // the methods we use are actually more or less static
    codal::ATMegaSerial *serial = nullptr;
    for (int i = 0; i < 10; ++i) {
        serial->send(" !!! ");
        serial->send(statusCode);
    }
    while (1) {}
}
