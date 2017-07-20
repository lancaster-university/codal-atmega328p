#include "codal_target_hal.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>

extern char* __malloc_heap_start;

extern "C"
{
    uint16_t codal_heap_start = &__malloc_heap_start;
}

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
 //TODO:
}

void target_panic(int statusCode)
{
    target_disable_irq();
}
