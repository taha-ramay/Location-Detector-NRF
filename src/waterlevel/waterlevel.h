#ifndef WATERLEVEL_H
#define WATERLEVEL_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/logging/log.h>

int getWaterLevel();
int setWaterLevel();
int incrementWaterLevel();
int DecrementWaterLevel();
void motor_on();
void motor_off();
#endif // WATERLEVEL_H
