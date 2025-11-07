#include "BluetoothManager.h"
// LOG_INF("nRF Connect SDK Fundamentals");
// LOG_DBG("A log message in debug level");
// LOG_WRN("A log message in warning level!");
// LOG_ERR("A log message in Error level!");
static struct k_mutex tracked_mutex;
char addr_str[] = "24:0A:C4:45:88:56";

uint64_t last_time;
LOG_MODULE_REGISTER(Bluetooth_Debug, LOG_LEVEL_DBG);
K_THREAD_STACK_DEFINE(range_stack, 1024);
static struct k_thread range_thread;
// struct tracked_device trackedDevices[4];
struct tracked_device trackedDevices[4] = {0};
uint8_t helo = 3;
int a = 4;
int c = 5;

static const uint8_t allowed_ids[NUM_ALLOWED_IDS] = {0x01, 0x02, 0x03, 0x04};
static bool id_is_allowed(uint8_t id) {
  for (uint8_t i = 0; i < NUM_ALLOWED_IDS; i++)
    if (id == allowed_ids[i]) {
      if (trackedDevices[id - 1].device_tracked == true) {
        return false;
      }
      trackedDevices[i].device_tracked = true;
      return true;
    }
  return false;
}
static bool check_security_payload(const uint8_t *data) {
  uint8_t id = data[2];
  uint16_t out_key = (data[3] << 8) | data[4];

  if (!id_is_allowed(id) && out_key == SECURITY_KEY) {
    return false;
  }

  return true;
}

/* ---------- Periodic Tracker ---------- */
void range_monitor_thread(void *p1, void *p2, void *p3) {

  while (1) {
    if (trackedDevices[0].device_tracked == true) {
      if (k_uptime_get() - trackedDevices[0].last_seen_ms >
          OUT_OF_RANGE_TIMEOUT_MS) {
        // char addr_str[BT_ADDR_LE_STR_LEN];
        // bt_addr_le_to_str(&trackedDevices[0].tracked_device, addr_str,
        //                   sizeof(addr_str));
        // LOG_WRN("Device %s out of range!", addr_str);
        k_mutex_lock(&tracked_mutex, K_FOREVER);
        trackedDevices[0].device_tracked = false;

        k_mutex_unlock(&tracked_mutex);
        // LOG_WRN("time of check %d", trackedDevices[0].last_seen_ms / 1000);
        LOG_WRN("Last RSSI is %d", trackedDevices[0].last_rssi);
        LCD_SendString("device 1 out of range");

        LOG_WRN("Device 1 out of range");
      }
    }
    if (trackedDevices[1].device_tracked) {
      if (k_uptime_get() - trackedDevices[1].last_seen_ms >
          OUT_OF_RANGE_TIMEOUT_MS) {
        char addr_str[BT_ADDR_LE_STR_LEN];
        bt_addr_le_to_str(&trackedDevices[1].tracked_device, addr_str,
                          sizeof(addr_str));
        LOG_WRN("Device %s out of range!\n", addr_str);
        k_mutex_lock(&tracked_mutex, K_FOREVER);

        trackedDevices[1].device_tracked = false;
        k_mutex_unlock(&tracked_mutex);
        LOG_WRN("Last RSSI is %d", trackedDevices[0].last_rssi);
        LCD_SendString("device 2 out of range");
        LOG_WRN("Device 2 out of range");
      }
    }
    if (trackedDevices[2].device_tracked) {
      if (k_uptime_get() - trackedDevices[2].last_seen_ms >
          OUT_OF_RANGE_TIMEOUT_MS) {
        char addr_str[BT_ADDR_LE_STR_LEN];
        bt_addr_le_to_str(&trackedDevices[2].tracked_device, addr_str,
                          sizeof(addr_str));
        LOG_WRN("Device %s out of range!\n", addr_str);
        k_mutex_lock(&tracked_mutex, K_FOREVER);

        trackedDevices[2].device_tracked = false;
        k_mutex_unlock(&tracked_mutex);
        LCD_SendString("device 3 out of range");

        LOG_WRN("Device 3 out of range");
      }
    }
    if (trackedDevices[3].device_tracked) {
      if (k_uptime_get() - trackedDevices[3].last_seen_ms >
          OUT_OF_RANGE_TIMEOUT_MS) {
        char addr_str[BT_ADDR_LE_STR_LEN];
        bt_addr_le_to_str(&trackedDevices[3].tracked_device, addr_str,
                          sizeof(addr_str));
        LOG_WRN("Device %s out of range!\n", addr_str);
        k_mutex_lock(&tracked_mutex, K_FOREVER);

        trackedDevices[3].device_tracked = false;
        k_mutex_unlock(&tracked_mutex);
        LCD_SendString("device 4 out of range");

        LOG_WRN("Device 4 out of range");
      }
    }
    k_sleep(K_SECONDS(1));
  }
}

/* ---------- Scan Callback ---------- */
static void scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type,
                    struct net_buf_simple *buf) {
  if (adv_type != BT_GAP_ADV_TYPE_ADV_NONCONN_IND) {
    return;
  }
  bt_addr_t required_addr;
  bt_addr_from_str("24:0A:C4:45:88:56", &required_addr);
  if (bt_addr_cmp(&addr->a, &required_addr) == 0) {
    LOG_INF("in range our required address");
  } else {
    return;
  }
  // uint8_t *data = buf->data;

  // uint16_t len = buf->len; // length of data (if available)

  // LOG_INF("Advertisement data (len=%d):", len);

  // for (int i = 0; i < len; i++) {
  //   LOG_INF("data[%d] = 0x%02X", i, data[i]);
  // }
  if (rssi > RSSI_Limit) {
    uint8_t *data = &(buf->data[2]);
    unsigned char bledata[16];
    memset(bledata, 0x00, sizeof(bledata));
    if (!aes_cfb128(data, 16, bledata)) {
      LOG_INF("decryption failed, unknown device");
      return;
    }
    for (int i = 0; i < 3; i++) {
      LOG_INF("%d ", bledata[i]);
    }
    char addr_str[BT_ADDR_LE_STR_LEN];
    bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    LOG_INF("Address: %s ", addr_str);
    k_msleep(5000); // Wait for LCD to power up

    // if (buf->len > 1) {
    // } else {
    //   LOG_INF("data not present");
    //   return;
    // }
    // if (check_security_payload(data)) {
    //   // LOG_INF("valid node present");
    // } else {
    //   LOG_INF("invalid node present");
    //   return;
    // }
    // uint8_t id = data[2] - 1;
    // k_mutex_lock(&tracked_mutex, K_FOREVER);

    // trackedDevices[id].device_tracked = true;
    // trackedDevices[id].last_rssi = rssi;
    // trackedDevices[id].last_seen_ms = k_uptime_get();
    // trackedDevices[id].tracked_device = *addr;
    // k_mutex_unlock(&tracked_mutex);
    // LCD_SendString("Device 1 in range");

    // LOG_INF("Device [%d] tracked = %s | RSSI = %d dBm | Last seen = %lld ms",
    //         id, trackedDevices[id].device_tracked ? "true" : "false",
    //         trackedDevices[id].last_rssi, trackedDevices[id].last_seen_ms);

    return;
  } else {
    if (k_uptime_get() - last_time > OUT_OF_RANGE_TIMEOUT_MS) {
      LOG_INF("Not in range");
      uint8_t *data = buf->data;
      uint8_t id = data[0];
      LOG_INF("Device ID: %u | RSSI: %d dBm", id, rssi);

      char addr_str[BT_ADDR_LE_STR_LEN];
      bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
      LOG_INF("Address: %s ", addr_str);
      last_time = k_uptime_get();
    }
  }
}

const struct bt_le_scan_param scan_param = {
    .type = BT_LE_SCAN_TYPE_PASSIVE,
    .options = BT_LE_SCAN_OPT_NONE,
    .interval = 0x0010,
    .window = 0x0010,
};

void bluetooth_Init() {
  LOG_INF("Bluetooth begin");
  int err;
  k_mutex_init(&tracked_mutex);

  err = bt_enable(NULL);
  if (err) {
    LOG_ERR("Bluetooth init failed (err %d)\n", err);
    return;
  }

  err = bt_le_scan_start(&scan_param, scan_cb);
  if (err) {
    LOG_ERR("Scan start failed (%d)", err);
    return;
  }
  k_thread_create(&range_thread, range_stack,
                  K_THREAD_STACK_SIZEOF(range_stack), range_monitor_thread,
                  NULL, NULL, NULL, 7, 0, K_NO_WAIT);
  LOG_INF("Scanning successfully started\n");
}