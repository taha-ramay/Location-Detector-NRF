#include "JsonParser.h"
LOG_MODULE_REGISTER(Json_Parser);
struct __attribute__((packed, aligned(1))) payloaddata {
  int Temp;
  int Pres;
  int CO2;
  int Water;
};
// static const struct json_obj_descr BLE_descr[] = {
//     JSON_OBJ_DESCR_PRIM_NAMED(struct payloaddata, "Temp", Temp,
//                               JSON_TOK_NUMBER),
//     JSON_OBJ_DESCR_PRIM_NAMED(struct payloaddata, "Pres", Pres,
//                               JSON_TOK_NUMBER),
//     JSON_OBJ_DESCR_PRIM_NAMED(struct payloaddata, "CO2", CO2,
//     JSON_TOK_NUMBER),

// };
static const struct json_obj_descr BLE_descr[] = {
    JSON_OBJ_DESCR_PRIM(struct payloaddata, Temp, JSON_TOK_NUMBER),
    JSON_OBJ_DESCR_PRIM(struct payloaddata, Pres, JSON_TOK_NUMBER),
    JSON_OBJ_DESCR_PRIM(struct payloaddata, CO2, JSON_TOK_NUMBER),
    JSON_OBJ_DESCR_PRIM(struct payloaddata, Water, JSON_TOK_NUMBER),

};
// static const struct json_obj_descr BLE_descr[] = {
//     JSON_OBJ_DESCR_PRIM(struct payloaddata, Temp, JSON_TOK_NUMBER),
//     JSON_OBJ_DESCR_PRIM(struct payloaddata, Pres, JSON_TOK_NUMBER),
//     JSON_OBJ_DESCR_PRIM(struct payloaddata, CO2, JSON_TOK_NUMBER),
// };

void EspNodeParser(const uint8_t *data, char *json_out, size_t json_out_size) {
  LOG_INF("filling data of ESP32 node temp,pressure and CO2");
  if (data == NULL || json_out == NULL || json_out_size == 0) {
    LOG_ERR("data null");
    return;
  }
  struct payloaddata payload_data;

  payload_data.Temp = data[0];
  payload_data.Pres = data[1];
  payload_data.CO2 = data[2];
  payload_data.Water = getWaterLevel();
  incrementWaterLevel();
  //   payload_data.Temp = 0;
  //   payload_data.Pres = 0;
  //   payload_data.CO2 = 0;
  LOG_INF("the value of temp is %d", payload_data.Temp);
  LOG_INF("the value of pressure is %d", payload_data.Pres);
  LOG_INF("the value of co2 is %d", payload_data.CO2);
  //   const struct json_obj_descr reported[] = {
  //       JSON_OBJ_DESCR_OBJECT_NAMED(struct payloaddata, "bledata", Temp,
  //                                   parameters),
  //   };

  int err = json_obj_encode_buf(BLE_descr, ARRAY_SIZE(BLE_descr), &payload_data,
                                json_out, json_out_size);
  if (err) {
    LOG_ERR("json_obj_encode_buf, error: %d", err);
    return;
  }
  //   struct payload Payload;
  //   Payload.state.reported.app_version = "1.0.0";
  //   Payload.state.reported.modem_version = "modem";
  //   Payload.state.reported.uptime = 40;
  //   LOG_ERR("before json_payload_construct");

  //   json_payload_construct(json_out, 40, &Payload);
  //   // Format JSON into output buffer
  //   // Using snprintf for safety (avoids buffer overflow)
  //   snprintf(json_out, json_out_size, "{\"temp\":%u,\"pressure\":%u,\"co2\":
  //   %u}",
  //            temp, pressure, co2);
}

// **
//  * @brief Helper macro to declare a descriptor for an object value
//  *
//  * @param struct_ Struct packing the values
//  * @param field_name_ Field name in the struct
//  * @param sub_descr_ Array of json_obj_descr describing the subobject
//  *
//  * Here's an example of use:
//  *
//  *      struct nested {
//   *int32_t foo;
//   *struct {
//     *int32_t baz;
//     *
//   } bar;
//   *
// };
// **struct json_obj_descr nested_bar[] = {*{... declare bar.baz descriptor...},
//                                         *};
// *struct json_obj_descr nested[] = {
//     *{... declare foo descriptor...},
//     *JSON_OBJ_DESCR_OBJECT(struct nested, bar, nested_bar), *};
// */
// #define JSON_OBJ_DESCR_OBJECT(struct_, field_name_, sub_descr_) \
//   { \
//       .field_name = (#field_name_), \
//       .align_shift = Z_ALIGN_SHIFT(struct_), \
//       .field_name_len = (sizeof(#field_name_) - 1), \
//       .type = JSON_TOK_OBJECT_START, \
//       .offset = offsetof(struct_, field_name_), \
//       .object = \
//           { \
//               .sub_descr = sub_descr_, \
//               .sub_descr_len = ARRAY_SIZE(sub_descr_), \
//           }, \
//   }

/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

int json_payload_construct(char *message, size_t size,
                           struct payload *payload) {
  int err;
  LOG_ERR("2");
  if (!payload || !message || size == 0) {
    LOG_ERR("Payload or buffer invalid");
    return -1;
  }
  const struct json_obj_descr parameters[] = {
      JSON_OBJ_DESCR_PRIM_NAMED(struct payload, "uptime", state.reported.uptime,
                                JSON_TOK_NUMBER),
      JSON_OBJ_DESCR_PRIM_NAMED(struct payload, "app_version",
                                state.reported.app_version, JSON_TOK_STRING),

      JSON_OBJ_DESCR_PRIM_NAMED(struct payload, "modem_version",
                                state.reported.modem_version, JSON_TOK_STRING)

  };
  LOG_ERR("3");

  const struct json_obj_descr reported[] = {
      JSON_OBJ_DESCR_OBJECT_NAMED(struct payload, "reported", state.reported,
                                  parameters),
  };
  LOG_ERR("4");

  const struct json_obj_descr root[] = {
      JSON_OBJ_DESCR_OBJECT(struct payload, state, reported),
  };
  LOG_ERR("5");

  err = json_obj_encode_buf(root, ARRAY_SIZE(root), payload, message, 40);
  if (err) {
    LOG_ERR("json_obj_encode_buf, error: %d", err);
    return err;
  }
  LOG_ERR("6");

  return 0;
}
