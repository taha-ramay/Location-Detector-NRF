#ifndef Buzzer_Manager_H
#define Buzzer_Manager_H
#include <zephyr/drivers/gpio.h>

#include <zephyr/dt-bindings/gpio/gpio.h>
#include <zephyr/logging/log.h>
void init_all_buzzers(void);
struct Buzzer {
  const struct gpio_dt_spec *pin; // Zephyr DT spec for pin
  bool is_active;
};
extern struct Buzzer buzzers[4];
void buzzer_init(struct Buzzer *b, const struct gpio_dt_spec *pin);
void buzzer_on(struct Buzzer *b);
void buzzer_off(struct Buzzer *b);
#endif