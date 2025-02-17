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

String TimeModule::getFormattedDate(){
  time_t t = timeClient.getEpochTime();
  struct tm * timeInfo = localtime(&t);

  String day   = String(timeInfo->tm_mday);
  String month = String(timeInfo->tm_mon + 1);      // tm_mon va da 0 a 11
  String year  = String(timeInfo->tm_year + 1900);    // tm_year conta gli anni dal 1900

  return (day+"/"+month+"/"+year);
}

String TimeModule::getFormattedTime() {
  return timeClient.getFormattedTime();
}
