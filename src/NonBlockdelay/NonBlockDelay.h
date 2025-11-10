#ifndef NON_BLOCK_DELAY_H_
#define NON_BLOCK_DELAY_H_
#include <stdbool.h>
#include <string.h>
#include <zephyr/kernel.h>

bool Delay(unsigned long);
bool DoDelay(unsigned long);
bool Timeout(void);
bool Bump(unsigned long);
unsigned long Time(void);
bool reset(unsigned long);

#endif