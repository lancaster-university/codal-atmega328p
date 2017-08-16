#ifndef DEVICE_FIBER_IMP_H
#define DEVICE_FIBER_IMP_H

#include "device_types.h"
#include "CodalDevice.h"
#include <avr/interrupt.h>

/**
  *  Thread Context for an ARM Cortex M0 core.
  *
  * This is probably overkill, but the ARMCC compiler uses a lot register optimisation
  * in its calling conventions, so better safe than sorry!
  */
struct PROCESSOR_TCB
{
    uint8_t SPLO;
    uint8_t SPHI;

    uint16_t lr;

    uint8_t R20;
    uint8_t R21;
    uint8_t R22;
    uint8_t R23;
    uint8_t R24;
    uint8_t R25;
};

inline PROCESSOR_WORD_TYPE fiber_initial_stack_base()
{
    return RAMEND - 0x02;
}

/**
  * Configures the link register of the given tcb to have the value function.
  *
  * @param tcb The tcb to modify
  * @param function the function the link register should point to.
  */
inline void tcb_configure_lr(PROCESSOR_TCB* tcb, PROCESSOR_WORD_TYPE function)
{
    tcb->lr = function;
}

/**
  * Configures the link register of the given tcb to have the value function.
  *
  * @param tcb The tcb to modify
  * @param function the function the link register should point to.
  */
inline void tcb_configure_sp(PROCESSOR_TCB* tcb, PROCESSOR_WORD_TYPE sp)
{
    tcb->SPHI = codal::high(sp);
    tcb->SPLO = codal::low(sp);
}

inline void tcb_configure_stack_base(PROCESSOR_TCB* tcb, PROCESSOR_WORD_TYPE stack_base)
{
    //tcb->SPHI = codal::high(stack_base);
    //tcb->SPLO = codal::low(stack_base);
}

inline PROCESSOR_WORD_TYPE tcb_get_stack_base(PROCESSOR_TCB* tcb)
{
    //return tcb->SPHI << 8 | tcb->SPLO;
    return fiber_initial_stack_base();

}

inline PROCESSOR_WORD_TYPE get_current_sp()
{
    return SPH << 8 | SPL;;
}

inline PROCESSOR_WORD_TYPE tcb_get_sp(PROCESSOR_TCB* tcb)
{
    return tcb->SPHI << 8 | tcb->SPLO;
}

inline void tcb_configure_args(PROCESSOR_TCB* tcb, PROCESSOR_WORD_TYPE ep, PROCESSOR_WORD_TYPE cp, PROCESSOR_WORD_TYPE pm)
{
    // arg 3
    tcb->R20 = codal::low(pm);
    tcb->R21 = codal::high(pm);
    // arg 2
    tcb->R22 = codal::low(cp);
    tcb->R23 = codal::high(cp);
    // arg 1
    tcb->R24 = codal::low(ep);
    tcb->R25 = codal::high(ep);
}

#endif
