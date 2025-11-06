#ifndef Network_Manager_H
#define Network_Manager_H
#include "common.h"
#include <stdbool.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/net_mgmt.h>
#include <zephyr/net/socket.h>

void network_init();
int send_data(uint8_t *data, uint16_t data_len);
#endif
