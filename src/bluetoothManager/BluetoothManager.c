#include "BluetoothManager.h"
// LOG_INF("nRF Connect SDK Fundamentals");
// LOG_DBG("A log message in debug level");
// LOG_WRN("A log message in warning level!");
// LOG_ERR("A log message in Error level!");
static struct k_mutex tracked_mutex;
uint64_t last_time;
LOG_MODULE_REGISTER(Bluetooth_Debug, LOG_LEVEL_DBG);
K_THREAD_STACK_DEFINE(range_stack, 1024);
static struct k_thread range_thread;
struct tracked_device trackedDevices[4];
static const uint8_t allowed_ids[NUM_ALLOWED_IDS] = {0x01, 0x02, 0x03, 0x04};
static bool id_is_allowed(uint8_t id) {
  for (int i = 0; i < NUM_ALLOWED_IDS; i++)
    if (id == allowed_ids[i]) {
      trackedDevices[i].device_tracked = true;
      return true;
    }
  return false;
}
static bool check_security_payload(const uint8_t *data) {
  uint16_t out_key = (data[1] << 8) | data[2];
  // Example: [0] = ID, [1] = key
  if (!id_is_allowed(data[0]) && out_key == SECURITY_KEY)
    return false;
  return true;
}

/* ---------- Periodic Tracker ---------- */
void range_monitor_thread(void) {
  uint8_t device_count = 0;
  k_mutex_lock(&tracked_mutex, K_FOREVER);

  while (1) {
    if (trackedDevices[0].device_tracked) {
      if (k_uptime_get() - trackedDevices[0].last_seen_ms >
          OUT_OF_RANGE_TIMEOUT_MS) {
        char addr_str[BT_ADDR_LE_STR_LEN];
        bt_addr_le_to_str(&trackedDevices[0].tracked_device, addr_str,
                          sizeof(addr_str));
        LOG_WRN("Device %s out of range!\n", addr_str);
        trackedDevices[0].device_tracked = false;
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
        trackedDevices[1].device_tracked = false;
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
        trackedDevices[2].device_tracked = false;
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
        trackedDevices[3].device_tracked = false;
        LOG_WRN("Device 4 out of range");
      }
    }
    k_mutex_unlock(&tracked_mutex);
  }
}

/* ---------- Scan Callback ---------- */
static void scan_cb(const bt_addr_le_t *addr, int8_t rssi, uint8_t adv_type,
                    struct net_buf_simple *buf) {
  // Parse AD elements
  if (rssi < -70) {
    return;
  }

  if (rssi > RSSI_Limit) {
    uint8_t *data = buf->data;
    if (buf->len > 1) {
      LOG_INF("data is present");
    } else {
      LOG_INF("data not present");
      return;
    }
    if (check_security_payload(data)) {
      LOG_INF("valid node present");
    } else {
      LOG_INF("invalid node present");
      return;
    }
    uint8_t id = data[0];

    if (id > ARRAY_SIZE(trackedDevices)) {
      LOG_INF("the id is %d", id);
      LOG_ERR("Unknown device trying to connect");
      return;
    }
    k_mutex_lock(&tracked_mutex, K_FOREVER);

    trackedDevices[id].device_tracked = true;
    trackedDevices[id].last_rssi = rssi;
    trackedDevices[id].last_seen_ms = k_uptime_get();
    trackedDevices[id].tracked_device = *addr;

    k_mutex_unlock(&tracked_mutex);
    // LOG_WRN("tracking info of device");
    // LOG_INF("Device [%d] tracked = %s | RSSI = %d dBm | Last seen = %lld ms",
    //         id, trackedDevices[id].device_tracked ? "true" : "false",
    //         trackedDevices[id].last_rssi, trackedDevices[id].last_seen_ms);
    // char addr_str[BT_ADDR_LE_STR_LEN];
    // bt_addr_le_to_str(addr, addr_str, sizeof(addr_str));
    // LOG_INF("Address: %s ", addr_str);
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