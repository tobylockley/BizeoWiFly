/* Bizeo-Ethernet library for Arduino
 * Author: Toby Lockley
 * (c) Copyright 2012 Direct Marketing Software Pty. Ltd.
 * All rights reserved.
 */
 
#ifndef __BIZEO_H
#define __BIZEO_H

#if (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <WiFly.h>

class BizeoClass {
private:
    WiFlyClient _client;
    unsigned int _debugLevel;
    int scanResponse(String searchStr);
    String getLineFromClient();

public:
    BizeoClass() { _debugLevel = 0; }
    void setDebugLevel(int level);
    int begin(const char* ssid, const char* passphrase);
    int getStatus(String userGuid);
    int updateKpi(String kpiGuid, int value);
    int updateKpi(String kpiGuid, String value);
};

extern BizeoClass Bizeo;  // For use as global object

#endif  // __BIZEO_H

/* (c) Copyright 2012 Direct Marketing Software Pty. Ltd. */