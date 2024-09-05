#ifndef BUTTON_H_
#define BUTTON_H_

#include <stdbool.h>

#define BUTTON_GPIO CONFIG_GPIO_BUTTON_BUILTIN
#define BUTTON_ACTIVE_STATE CONFIG_GPIO_BUTTON_BUILTIN_ACTIVE_STATE

void button_init();
bool button_is_pressed();

#endif //BUTTON_H_