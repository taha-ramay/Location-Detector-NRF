#include "waterlevel.h"
LOG_MODULE_REGISTER(WaterLevelMonitor);
uint8_t waterlevel = 0;
bool motoroff = false;
bool motoron = true;
int getWaterLevel() {
  LOG_INF("getWaterLevel");
  //   if (motoron) {
  //     return waterlevel + 1;
  //   }
  return waterlevel;
}
int setWaterLevel() {
  LOG_INF("setWaterLevel");
  return waterlevel + 1;
}
int incrementWaterLevel() {
  LOG_INF("incrementWaterLevel");
  if (motoron) {
    waterlevel += 10;
  } else {
    waterlevel -= 10;
  }

  if (waterlevel > 100) {
    motoron = false;
  } else if (waterlevel <= 0) {
    motoron = true;
  }
}
int DecrementWaterLevel() {
  LOG_INF("DecrementWaterLevel");

  return 0;
}
void motor_on() {
  LOG_INF("motor_on");
  if (waterlevel > 100) {
    LOG_INF("cannot turn on motor");
    motoron = false;
  }
  motoron = true;
}
void motor_off() {
  LOG_INF("motor_off");
  if (waterlevel <= 0) {
    LOG_INF("cannot turn off motor");
    return;
  }
  motoron = false;
}