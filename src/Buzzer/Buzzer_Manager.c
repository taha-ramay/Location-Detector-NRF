#include "Buzzer_Manager.h"
struct Buzzer buzzers[4];
LOG_MODULE_REGISTER(Buzzer_Debug, LOG_LEVEL_DBG);
static const struct gpio_dt_spec buzzer1_pin =
    GPIO_DT_SPEC_GET(DT_NODELABEL(buzzer1), gpios);
static const struct gpio_dt_spec buzzer2_pin =
    GPIO_DT_SPEC_GET(DT_NODELABEL(buzzer2), gpios);
static const struct gpio_dt_spec buzzer3_pin =
    GPIO_DT_SPEC_GET(DT_NODELABEL(buzzer3), gpios);
static const struct gpio_dt_spec buzzer4_pin =
    GPIO_DT_SPEC_GET(DT_NODELABEL(buzzer4), gpios);

void init_all_buzzers(void) {
  buzzer_init(&buzzers[0], &buzzer1_pin);
  buzzer_init(&buzzers[1], &buzzer2_pin);
  buzzer_init(&buzzers[2], &buzzer3_pin);
  buzzer_init(&buzzers[3], &buzzer4_pin);
}

void buzzer_init(struct Buzzer *b, const struct gpio_dt_spec *pin) {
  b->pin = pin;
  b->is_active = false;

  if (!device_is_ready(pin->port))
    return;

  gpio_pin_configure_dt(pin, GPIO_OUTPUT_INACTIVE);
}
void buzzer_on(struct Buzzer *b) {
  if (!b->pin)
    return;
  gpio_pin_set_dt(b->pin, 1);
  b->is_active = true;
}
void buzzer_off(struct Buzzer *b) {
  if (!b->pin)
    return;
  gpio_pin_set_dt(b->pin, 0);
  b->is_active = false;
}
