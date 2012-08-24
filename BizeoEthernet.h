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

#include <Ethernet.h>

// Used for determining which HTTP method to use
enum HTTP_METHOD {
    GET,
    POST,
    SOAP
};

class BizeoClass {
private:
    EthernetClient _client;
    unsigned int _debugLevel;
    String parseXML(String searchStr);

public:
    BizeoClass() { _debugLevel = 0; }
    int begin();
    int begin(uint8_t *mac_address);
    void setDebugLevel(int level);
    // Main usage functions. When no HTTP_METHOD is specified,
    // the default is to use GET
    int getStatus(String userGuid);
    int getStatus(HTTP_METHOD method, String userGuid);
    int updateKpi(String kpiGuid, int value);
    int updateKpi(String kpiGuid, String value);
    int updateKpi(HTTP_METHOD method, String kpiGuid, int value);
    int updateKpi(HTTP_METHOD method, String kpiGuid, String value);
};

extern BizeoClass Bizeo;  // For use as global object

#endif  // __BIZEO_H

/* (c) Copyright 2012 Direct Marketing Software Pty. Ltd. */