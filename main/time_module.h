#ifndef TIME_MODULE_H
#define TIME_MODULE_H

#include <NTPClient.h>
#include <time.h>
#include <WiFiUdp.h>
#include <Arduino.h>

class TimeModule {
  public:
    // timetable (ex. 3600 per UTC+1)
    TimeModule(long utcOffset);
    void begin();
    void updateTime();
    String getFormattedTime();
    String getFormattedDate();

  private:
    WiFiUDP ntpUDP;
    NTPClient timeClient;
};

#endif
