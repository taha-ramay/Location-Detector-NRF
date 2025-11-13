#ifndef JSONPARSER_H
#define JSONPARSER_H
#include "waterlevel.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/data/json.h>
#include <zephyr/logging/log.h>

struct payload {
  struct {
    struct {
      const char *app_version;
      const char *modem_version;
      uint32_t uptime;
    } reported;
  } state;
};

int json_payload_construct(char *message, size_t size, struct payload *payload);
extern void EspNodeParser(const uint8_t *data, char *json_out,
                          size_t json_out_size);
#endif
