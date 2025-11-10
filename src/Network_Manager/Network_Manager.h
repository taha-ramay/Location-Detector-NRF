#ifndef Network_Manager_H
#define Network_Manager_H
#include "DisplayManager.h"
#include "NonBlockDelay.h"
#include "common.h"
#include <stdbool.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/net_if.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/socket.h>
#include <zephyr/sys/fdtable.h>

void network_init();
void enqueue_data(uint8_t *data, uint16_t data_len);
#endif
