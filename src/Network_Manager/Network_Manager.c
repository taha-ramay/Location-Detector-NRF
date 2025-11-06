#include "Network_Manager.h"
int sock;
struct sockaddr_in server;
static struct k_thread network_thread;
K_THREAD_STACK_DEFINE(network_stack, 4096);
LOG_MODULE_REGISTER(Network_Debug);
uint64_t start_time;

void network_monitor_thread(void) {
  LOG_INF("network_monitor_thread setting");
  while (1) {
    if (k_uptime_get() - start_time > OUT_OF_RANGE_TIMEOUT_network) {
      LOG_INF("sneding sensor dat");
      uint8_t data[3];
      data[0] = 25;  // temperature (°C)
      data[1] = 101; // pressure (kPa)
      data[2] = 45;  // CO2 level (% or arbitrary units)
      send_data(&data, sizeof(data));
      start_time = k_uptime_get();
    }
  }
}
bool wait_for_ack(int sock, int timeout_ms) {
  uint8_t buf[64];
  int64_t start_time = k_uptime_get();
  int total_len = 0;

  memset(buf, 0, sizeof(buf));

  while ((k_uptime_get() - start_time) < timeout_ms) {
    int ret =
        recv(sock, buf + total_len, sizeof(buf) - total_len - 1, MSG_DONTWAIT);

    if (ret > 0) {
      total_len += ret;
      buf[total_len] = '\0'; // for string operations

      // Check if newline received
      char *newline = strchr((char *)buf, '\n');
      if (newline) {
        if (strncmp((char *)buf, "ACK", 3) == 0) {
          LOG_INF("ACK received from server");
          return true;
        } else {
          LOG_WRN("Unexpected reply: %s", buf);
          return false;
        }
      }
    } else if (ret == 0) {
      LOG_WRN("Server closed connection");
      return false;
    } else {
      k_msleep(10); // wait before next poll
    }
  }

  LOG_WRN("ACK timeout");
  return false;
}
void network_init() {
  LOG_INF("Starting network ");

  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);
  inet_pton(AF_INET, SERVER_IP, &server.sin_addr);

  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0) {
    LOG_ERR("Failed to create socket: %d", errno);
    return;
  }

  LOG_INF("Connecting to %s:%d", SERVER_IP, SERVER_PORT);
  if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
    LOG_ERR("Connect failed: %d", errno);
    close(sock);
    return;
  }
  LOG_INF("Connected!");
  k_thread_create(&network_thread, network_stack,
                  K_THREAD_STACK_SIZEOF(network_stack), network_monitor_thread,
                  NULL, NULL, NULL, 7, 0, K_NO_WAIT);
  start_time = k_uptime_get();
}
int send_data(uint8_t *data, uint16_t data_len) {
  LOG_INF("Sending data to the server");
  uint16_t len_be = htons(data_len);
  LOG_INF("Sending data to the server");

  int ret = send(sock, &len_be, sizeof(len_be), 0);
  LOG_INF("A");

  if (ret < 0) {
    LOG_ERR("Failed to send length: %d", errno);
    return 0;
  }
  LOG_INF("B");

  ret = send(sock, data, data_len, 0);
  if (ret < 0) {
    LOG_ERR("Failed to send message: %d", errno);
    return 0;
  }
  LOG_INF("Sent %d bytes (length prefix + message)", data_len + sizeof(len_be));
  if (wait_for_ack(sock, 3000)) {
    LOG_INF("ACK OK — transmission successful");
  } else {
    LOG_WRN("No ACK received");
  }
  return 1;
}