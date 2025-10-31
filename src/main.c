/* main.c - Application main entry point */

/*
 * Copyright (c) 2015-2016 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "BluetoothManager.h"
#include "Buzzer_Manager.h"
#include "DisplayManager.h"
#include "common.h"
#include <errno.h>
#include <stddef.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>

LOG_MODULE_REGISTER(Main_Debug, LOG_LEVEL_DBG);

int main(void) {
  LOG_INF("Booting");
  bluetooth_Init();
  return 0;
}
