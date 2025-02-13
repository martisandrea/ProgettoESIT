#include "time_module.h"

TimeModule::TimeModule(long utcOffset)
  : timeClient(ntpUDP, "pool.ntp.org", utcOffset) {
}

void TimeModule::begin() {
  timeClient.begin();
}

void TimeModule::updateTime() {
  timeClient.update();
}

String TimeModule::getFormattedTime() {
  return timeClient.getFormattedTime();
}
