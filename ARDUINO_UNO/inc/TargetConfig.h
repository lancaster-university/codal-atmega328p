#include "common_includes.h"
#include "device_types.h"

//
// Memory configuration
//
// Physical address of the top of SRAM.
#ifndef DEVICE_SRAM_END
#define DEVICE_SRAM_END                         RAMEND
#endif

// The start address of usable RAM memory.
#ifndef DEVICE_SRAM_BASE
#define DEVICE_SRAM_BASE                        RAMEND - 2048
#endif

// Physical address of the top of the system stack (on mbed-classic this is the top of SRAM)
#ifndef AVR_STACK_BASE
#define AVR_STACK_BASE                          DEVICE_SRAM_END
#endif

// Amount of memory reserved for the stack at the end of memory (bytes).
#ifndef DEVICE_STACK_SIZE
// this could potentially not work... :)
#define DEVICE_STACK_SIZE                       1048
#endif

// Physical address of the end of heap space.
#ifndef CODAL_HEAP_START
                                              extern char* __malloc_heap_start;
#define CODAL_HEAP_START                      (PROCESSOR_WORD_TYPE)(&__malloc_heap_start;)
#endif

// Physical address of the end of heap space.
#ifndef CODAL_HEAP_END
#define CODAL_HEAP_END                        (AVR_STACK_BASE - DEVICE_STACK_SIZE)
#endif

// Enable this to read 10 bits of data from the acclerometer.
// Otherwise, 8 bits are used.
// Set '1' to enable.
#ifndef USE_ACCEL_LSB
#define USE_ACCEL_LSB                           0
#endif

#define TARGET_DEBUG_CLASS                      NOT_IMPLEMENTED
