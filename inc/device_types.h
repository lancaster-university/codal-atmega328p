#ifndef DEVICE_SIZE_H
#define DEVICE_SIZE_H

#include <stdint.h>

#define PROCESSOR_WORD_TYPE     uint16_t

typedef enum {
    PullUp = 0,
    PullDown = 1,
    PullNone = 2,
    OpenDrain = 3,
    PullDefault = PullNone
} PinMode;

typedef uint8_t PinName;

#endif
