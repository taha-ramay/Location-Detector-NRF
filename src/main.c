

#include "BluetoothManager.h"
#include "Buzzer_Manager.h"
#include "DisplayManager.h"
#include "Network_Manager.h"
#include "common.h"
#include <errno.h>
#include <stddef.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>

LOG_MODULE_REGISTER(Main_Debug, LOG_LEVEL_DBG);
int main(void) {
  LOG_INF("Booting");
  // LCD_init();
  // bluetooth_Init();
  network_init();
  return 0;
}
