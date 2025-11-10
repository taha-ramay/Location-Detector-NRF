#include "NonBlockDelay.h"
unsigned long iTimeout;
bool TimingActive = 0;
bool Delay(unsigned long t) {
  if (TimingActive) {
    if ((k_uptime_get() > iTimeout)) {
      TimingActive = 0;
      return (1);
    }
    return (0);
  }
  iTimeout = k_uptime_get() + t;
  TimingActive = 1;
  return (0);
};

bool Bump(unsigned long t) {
  if (TimingActive) {
    iTimeout = iTimeout + t;
    return (1);
  } else {
    return (0);
  }
}

bool reset(unsigned long t) {
  if (TimingActive) {
    iTimeout = k_uptime_get() + t;
    return (1);
  } else {
    return (0);
  }
}

bool DoDelay(unsigned long t) {
  if (TimingActive) {
    if ((k_uptime_get() > iTimeout)) {
      TimingActive = 0;
      return (1);
    }
    return (0);
  }
  iTimeout = k_uptime_get() + t;
  TimingActive = 1;
  return (1);
};

// returns true if timer expired
bool Timeout(void) {
  if (TimingActive) {
    if ((k_uptime_get() > iTimeout)) {
      TimingActive = 0;
      iTimeout = 0;
      return (1);
    }
    return false;
  }
  return (true);
}

// Returns the time left in milliseconds
unsigned long Time(void) {
  if (TimingActive) {
    return (iTimeout);
  } else {
    return 0;
  }
}