#ifndef MODE_H_
#define MODE_H_

#include <stdbool.h>

typedef enum {
    MODE_ALL_CHANNELS=10, MODE_RGB, MODE_INDIVIDUAL_CHANNELS
} mode_e;


/**
 * Initializes jumpers C1 and C2 and sets the device's operating mode
 * 
 * @return information on whether the mode has changed since the last startup
 */
bool mode_init();

// Returns the mode in which the LED controller is configured
mode_e mode_get();

#endif //MODE_H_