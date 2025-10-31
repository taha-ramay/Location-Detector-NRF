#ifndef BluetoothManager_H
#define BluetoothManager_H
#include "common.h"
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/logging/log.h>

struct tracked_device {
  bt_addr_le_t tracked_device;
  int8_t last_rssi;
  int64_t last_seen_ms;
  bool device_tracked;
};
void bluetooth_Init();

#endif
