#include "Network_Manager.h"
int sock;
struct sockaddr_in server;
static struct k_thread network_thread;
K_THREAD_STACK_DEFINE(network_stack, 4096);
LOG_MODULE_REGISTER(Network_Debug);
typedef struct {
  uint64_t connectionStatusCheck;
  bool data_queued;
  bool senddata;
  bool receivingdata;
  bool ackdata;
  bool ConnectionStatus;
} sockComm;
sockComm commFlags = {0};
uint8_t buf[64];
uint8_t total_len = 0;
bool wait_for_ack(int sock);
int send_data(uint8_t *data, uint16_t data_len);
uint8_t *serverdata = 0;
uint16_t serverdata_len;
void network_monitor_thread(void *p1, void *p2, void *p3) {
  LOG_INF("network_monitor_thread setting");
  commFlags.connectionStatusCheck = k_uptime_get();
  int err = 0;
  socklen_t len = sizeof(err);

  memset(buf, 0, sizeof(buf));
  while (1) {
    // LOG_INF("i am inside thread");

    if (commFlags.data_queued) {
      if (commFlags.senddata) {

        if (send_data(serverdata, serverdata_len)) {
          free(serverdata);
          commFlags.senddata = false;
          commFlags.receivingdata = true;
        } else {
          commFlags.data_queued = false;
        }
      } else if (commFlags.receivingdata) {
        if (wait_for_ack(sock)) {
          LOG_INF("ACK received");
          commFlags.receivingdata = false;
          commFlags.data_queued = false;

        } else {
          LOG_WRN("ACK not recevied yet");
          if (Delay(3000)) {
            LOG_WRN("ACk receive time limit reached");
            commFlags.receivingdata = false;
            commFlags.data_queued = false;
            // break;
          } else {
            LOG_WRN("ACk receive time in process");
          }
          // break;
        }
      }
    }

    if (k_uptime_get() - commFlags.connectionStatusCheck >
        Connection_check_Time) {
      if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len) < 0) {
        LOG_ERR("getsockopt failed: %d", errno);
      } else if (err != 0) {
        LOG_ERR("Socket not connected or error: %d", err);
        LOG_ERR("Thread aborted");
        close(sock);
        commFlags.ConnectionStatus = false;
        k_msleep(3000); // Wait for LCD to power up
        k_thread_abort(k_current_get());

        // return;
      } else {
        LOG_INF("Socket is still connected and OK");
        commFlags.connectionStatusCheck = k_uptime_get();
      }
    }
    k_msleep(100); // Wait for LCD to power up
  }
}
bool wait_for_ack(int sock) {
  // LOG_WRN("waiting for ack");
  int ret =
      recv(sock, buf + total_len, sizeof(buf) - total_len - 1, MSG_DONTWAIT);
  // LOG_WRN("after receving ack");
  // LOG_INF("The value of ret is %d", ret);
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
    LOG_WRN("ACK not received yet");
    return false;
  }
}
// bool wait_for_ack(int sock, int timeout_ms) {
//   uint8_t buf[64];
//   int64_t start_time = k_uptime_get();
//   int total_len = 0;

//   memset(buf, 0, sizeof(buf));

//   while ((k_uptime_get() - start_time) < timeout_ms) {
//     int ret =
//         recv(sock, buf + total_len, sizeof(buf) - total_len - 1,
//         MSG_DONTWAIT);

//     if (ret > 0) {
//       total_len += ret;
//       buf[total_len] = '\0'; // for string operations

//       // Check if newline received
//       char *newline = strchr((char *)buf, '\n');
//       if (newline) {
//         if (strncmp((char *)buf, "ACK", 3) == 0) {
//           LOG_INF("ACK received from server");
//           return true;
//         } else {
//           LOG_WRN("Unexpected reply: %s", buf);
//           return false;
//         }
//       }
//     } else if (ret == 0) {
//       LOG_WRN("Server closed connection");
//       return false;
//     } else {
//       k_msleep(10); // wait before next poll
//     }
//   }

//   LOG_WRN("ACK timeout");
//   return false;
// }
static void wait_for_network_up(void) {
  struct net_if *iface = net_if_get_default();
  LOG_INF("Waiting for interface to be UP...");

  while (!net_if_is_up(iface)) {
    k_sleep(K_MSEC(100));
  }

  LOG_INF("Interface is UP!");
}
bool wait_for_ack21(int sock, int timeout_ms) {
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
  // Get default network interface
  struct net_if *iface = net_if_get_default();
  if (!iface) {
    LOG_ERR("No default network interface found!");
    return;
  }

  // Wait until interface is up
  while (!net_if_is_up(iface)) {
    LOG_INF("Waiting for Ethernet interface to come up...");
    k_sleep(K_SECONDS(1));
  }
  LOG_INF("after net_if_is_up .");

  server.sin_family = AF_INET;
  server.sin_port = htons(SERVER_PORT);
  inet_pton(AF_INET, SERVER_IP, &server.sin_addr);
  sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0) {
    LOG_ERR("Failed to create socket: %d", errno);
    return;
  }

  int err = 0;
  socklen_t len = sizeof(err);
  if (getsockopt(sock, SOL_SOCKET, SO_ERROR, &err, &len) < 0) {
    LOG_ERR("getsockopt failed: %d", errno);
  } else if (err != 0) {
    LOG_ERR("Socket not connected or error: %d", err);
  } else {
    LOG_INF("Socket is still connected and OK");
  }
  LOG_INF("Connecting to %s:%d", SERVER_IP, SERVER_PORT);
  if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
    LOG_ERR("Connect failed: %d", errno);
    close(sock);

    return;
  }
  LOG_INF("Connected!");
  commFlags.ConnectionStatus = true;
  k_thread_create(&network_thread, network_stack,
                  K_THREAD_STACK_SIZEOF(network_stack), network_monitor_thread,
                  NULL, NULL, NULL, 7, 0, K_NO_WAIT);
}
void enqueue_data(uint8_t *data, uint16_t data_len) {
  LOG_INF("inside enqueue data");

  if (commFlags.ConnectionStatus) {
    LOG_INF("Available");
  } else {
    LOG_INF("Socket server not available");

    return;
  }

  if (!commFlags.data_queued) {
    LOG_INF("Sending data to the server");
    serverdata = malloc(data_len);
    if (serverdata == NULL) {
      LOG_ERR("malloc failed");
      return;
    }
    memcpy(serverdata, data, data_len);

    serverdata_len = data_len;
    commFlags.senddata = true;
    commFlags.data_queued = true;
  } else {
    LOG_INF("Previous data still in sending phase");
  }
}

int send_data(uint8_t *data, uint16_t data_len) {
  LOG_INF("inside send data");

  uint16_t len_be = htons(data_len);

  int ret = send(sock, &len_be, sizeof(len_be), 0);

  if (ret < 0) {
    LOG_ERR("Failed to send length: %d", errno);
    return 0;
  }
  k_msleep(25); // Wait for LCD to power up

  ret = send(sock, data, data_len, 0);
  if (ret < 0) {
    LOG_ERR("Failed to send message: %d", errno);
    return 0;
  }
  LOG_INF("Sent %d bytes (length prefix + message)", data_len + sizeof(len_be));

  return 1;
}