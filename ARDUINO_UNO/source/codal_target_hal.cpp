#include "codal_target_hal.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <util/delay.h>
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
    _delay_ms(milliseconds);
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

struct PROCESSOR_TCB
{
    uint8_t SPLO;
    uint8_t SPHI;

    uint16_t LR;

    uint8_t R20;
    uint8_t R21;
    uint8_t R22;
    uint8_t R23;
    uint8_t R24;
    uint8_t R25;
};

PROCESSOR_WORD_TYPE fiber_initial_stack_base()
{
    return RAMEND - 0x02;;
}

void* tcb_allocate()
{
    return (void *)malloc(sizeof(PROCESSOR_TCB));
}

/**
  * Configures the link register of the given tcb to have the value function.
  *
  * @param tcb The tcb to modify
  * @param function the function the link register should point to.
  */
void tcb_configure_lr(void* tcb, PROCESSOR_WORD_TYPE function)
{
    PROCESSOR_TCB* tcbPointer = (PROCESSOR_TCB *)tcb;
    tcbPointer-LR = function;
}

/**
  * Configures the link register of the given tcb to have the value function.
  *
  * @param tcb The tcb to modify
  * @param function the function the link register should point to.
  */
void tcb_configure_sp(void* tcb, PROCESSOR_WORD_TYPE sp)
{
    PROCESSOR_TCB* tcbPointer = (PROCESSOR_TCB *)tcb;
    tcb->SPHI = codal::high(sp);
    tcb->SPLO = codal::low(sp);
}

void tcb_configure_stack_base(void* tcb, PROCESSOR_WORD_TYPE stack_base)
{
    PROCESSOR_TCB* tcbPointer = (PROCESSOR_TCB *)tcb;
    tcbPointer->stack_base = stack_base;
}

PROCESSOR_WORD_TYPE tcb_get_stack_base(void* tcb)
{
    return fiber_initial_stack_base();
}

PROCESSOR_WORD_TYPE get_current_sp()
{
    return SPH << 8 | SPL;;
}

PROCESSOR_WORD_TYPE tcb_get_sp(void* tcb)
{
    return tcb->SPHI << 8 | tcb->SPLO;
}

void tcb_configure_args(void* tcb, PROCESSOR_WORD_TYPE ep, PROCESSOR_WORD_TYPE cp, PROCESSOR_WORD_TYPE pm)
{
    PROCESSOR_TCB* tcbPointer = (PROCESSOR_TCB *)tcb;
    // arg 3
    tcbPointer->R20 = codal::low(pm);
    tcbPointer->R21 = codal::high(pm);
    // arg 2
    tcbPointer->R22 = codal::low(cp);
    tcbPointer->R23 = codal::high(cp);
    // arg 1
    tcbPointer->R24 = codal::low(ep);
    tcbPointer->R25 = codal::high(ep);
}


