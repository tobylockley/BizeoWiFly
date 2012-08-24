/* Bizeo-Ethernet library for Arduino
 * Author: Toby Lockley
 * (c) Copyright 2012 Direct Marketing Software Pty. Ltd.
 * All rights reserved.
 */

#include "BizeoEthernet.h"

#define BIZEO_WS_DOMAIN   "bizeocloudws.cloudapp.net"
#define BIZEO_WS_URI      "/PublicWS.asmx"
#define BIZEO_TIMEOUT     1000 * 2  // Timeout (ms) for an active connection

int BizeoClass::begin()
{
    uint8_t defaultMac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x00 };
    return begin(defaultMac);
}

int BizeoClass::begin(uint8_t *macAddress)
{
    bool test = Ethernet.begin(macAddress);
    if (test == 1) {
        if (_debugLevel >= 1) {
            Serial.print(F("Bizeo successfully initialized ethernet with IP: "));
            Serial.println(Ethernet.localIP());
        }
    }
    else {
        if (_debugLevel >= 1) {
            Serial.println(F("Bizeo failed to attach ethernet shield."));
        }
    }
    return test;
}

void BizeoClass::setDebugLevel(int level)
{
    _debugLevel = level;
}

int BizeoClass::getStatus(String guid)
{
    return getStatus(GET, guid);  // Use GET when no argument supplied
}

int BizeoClass::getStatus(HTTP_METHOD method, String guid)
{
    /* Invokes Bizeo web service to retrieve current Bizeo status.
     * HTTP_METHOD = GET/POST/SOAP
     */

    unsigned int length = guid.length();  // Used for content length of POST requests
    String response;  // Store the response from the web server, ready for analysis
    int returnVal = -10;  // This function should never return -10

    if (_debugLevel >= 2) Serial.println();  // Make more readable in verbose mode
    if (_debugLevel >= 1) Serial.print(F("Connecting..."));

    // Attempt to connect
    if (_client.connect(BIZEO_WS_DOMAIN, 80)) {
        if (_debugLevel >= 1) Serial.print(F("Success..."));

        // Branch off depending on method argument
        switch (method) {
        case GET:
            if (_debugLevel >= 2) Serial.print(F("Using GET..."));

            _client.print(F("GET /PublicWS.asmx/GetMonitorStatus?MasterTaskID="));
            _client.print(guid);
            _client.print(F(" HTTP/1.1\nHost: bizeocloudws.cloudapp.net\n\n"));

            response = parseXML("int");
            break;

        case POST:
            if (_debugLevel >= 2) Serial.print(F("Using POST..."));

            length += 13;  // "MasterTaskID="

            _client.print(F("POST "));
            _client.print(F(BIZEO_WS_URI));
            _client.print(F("/GetMonitorStatus HTTP/1.1\n"));
            _client.print(F("Content-Type: application/x-www-form-urlencoded\n"));
            _client.print(F("Content-Length: "));
            _client.print(length);
            _client.print(F("\nHost: bizeocloudws.cloudapp.net\n\n"));

            _client.print(F("MasterTaskID="));
            _client.print(guid);

            response = parseXML("int");
            break;

        case SOAP:
            if (_debugLevel >= 2) Serial.print(F("Using SOAP..."));

            length += 348;  // All characters of XML data (found using notepad++)

            _client.print(F("POST "));
            _client.print(F(BIZEO_WS_URI));
            _client.print(F(" HTTP/1.1\n"));
            _client.print(F("Host: bizeocloudws.cloudapp.net\n"));
            _client.print(F("Content-Length: "));
            _client.print(length);
            _client.print(F("\nConnection: close\n"));
            _client.print(F("Content-Type: application/soap+xml; charset=utf-8\n\n"));

            _client.print(F("<?xml version=\"1.0\" encoding=\"utf-8\"?>"));
            _client.print(F("<soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "));
            _client.print(F("xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "));
            _client.print(F("xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\">"));
            _client.print(F("<soap12:Body>"));
            _client.print(F("<GetMonitorStatus xmlns=\"http://tempuri.org/\"><MasterTaskID>"));
            _client.print(guid);
            _client.print(F("</MasterTaskID></GetMonitorStatus></soap12:Body></soap12:Envelope>"));
        
            response = parseXML("GetMonitorStatusResult");
            break;

        default:
            // This shouldn't happen
            if (_debugLevel >= 1) Serial.println(F("Unexpected HTTP_METHOD."));
            returnVal = -3;  // General error code
        }

        // Translate response to a usable int value
        if (response == "0") {
            // Everything's OK
            if (_debugLevel >= 1) Serial.println(F("Status: GREEN"));
            returnVal = 0;
        }
        else if (response == "1")
        {
            // Warning
            if (_debugLevel >= 1) Serial.println(F("Status: YELLOW"));
            returnVal = 1;
        }
        else if (response == "2")
        {
            // Something's wrong!
            if (_debugLevel >= 1) Serial.println(F("Status: RED"));
            returnVal = 2;
        }
        else if (response == "-2")
        {
            // Invalid GUID
            if (_debugLevel >= 1) Serial.println(F("Error: Invalid GUID"));
            returnVal = -2;
        }
        else
        {
            // Unkown error
            if (_debugLevel >= 1) Serial.println(F("Error: Unexpected server response"));
            returnVal = -3;
        }
    }
    else {
        // Something wrong with internet connection
        if (_debugLevel >= 1) Serial.println(F("Failed to make a connection"));
        returnVal = -1;
    }

    _client.stop();
    return returnVal;
}

int BizeoClass::updateKpi(String guid, int value)
{
    return updateKpi(GET, guid, String(value));
}

int BizeoClass::updateKpi(String guid, String value)
{
    return updateKpi(GET, guid, value);
}

int BizeoClass::updateKpi(HTTP_METHOD method, String guid, int value)
{
    return updateKpi(method, guid, String(value));
}

int BizeoClass::updateKpi(HTTP_METHOD method, String guid, String value)
{
    /* Invokes Bizeo web service to update an external KPI.
     */

    unsigned int length = guid.length() + value.length();  // Used for content length of POST requests
    String response;  // Store the response from the web server, ready for analysis
    int returnVal = -10;  // This function should never return -10

    if (_debugLevel >= 2) Serial.println();  // Make more readable in verbose mode
    if (_debugLevel >= 1) Serial.print(F("Connecting..."));

    // Attempt to connect
    if (_client.connect(BIZEO_WS_DOMAIN, 80)) {
        if (_debugLevel >= 1) Serial.print(F("Success..."));

        // Branch off depending on method argument
        switch (method) {
        case GET:
            if (_debugLevel >= 2) Serial.print(F("Using GET..."));
            
            _client.print(F("GET "));
            _client.print(F(BIZEO_WS_URI));
            _client.print(F("/UpdateExternalKPI?taskId="));
            _client.print(guid);
            _client.print(F("&result="));
            _client.print(value);
            _client.print(F(" HTTP/1.1\nHost: bizeocloudws.cloudapp.net\n\n"));

            response = parseXML("int");
            break;

        case POST:
            if (_debugLevel >= 2) Serial.print(F("Using POST..."));

            length += 15;  // "taskId=&result="

            _client.print(F("POST "));
            _client.print(F(BIZEO_WS_URI));
            _client.print(F("/UpdateExternalKPI HTTP/1.1\n"));
            _client.print(F("Content-Type: application/x-www-form-urlencoded\n"));
            _client.print(F("Content-Length: "));
            _client.print(length);
            _client.print(F("\nHost: bizeocloudws.cloudapp.net\n\n"));

            _client.print(F("taskId="));
            _client.print(guid);
            _client.print(F("&result="));
            _client.print(value);

            response = parseXML("int");
            break;

        case SOAP:
            if (_debugLevel >= 2) Serial.print(F("Using SOAP..."));

            length += 355;  // All characters of XML data (found using notepad++)

            _client.print(F("POST "));
            _client.print(F(BIZEO_WS_URI));
            _client.print(F(" HTTP/1.1\n"));
            _client.print(F("Host: bizeocloudws.cloudapp.net\n"));
            _client.print(F("Content-Length: "));
            _client.print(length);
            _client.print(F("\nConnection: close\n"));
            _client.print(F("Content-Type: application/soap+xml; charset=utf-8\n\n"));

            _client.print(F("<?xml version=\"1.0\" encoding=\"utf-8\"?>"));
            _client.print(F("<soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" "));
            _client.print(F("xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" "));
            _client.print(F("xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\">"));
            _client.print(F("<soap12:Body>"));
            _client.print(F("<UpdateExternalKPI xmlns=\"http://tempuri.org/\"><taskId>"));
            _client.print(guid);
            _client.print(F("</taskId><result>"));
            _client.print(value);
            _client.print(F("</result></UpdateExternalKPI></soap12:Body></soap12:Envelope>"));
            
            response = parseXML("UpdateExternalKPIResult");
            break;

        default:
            // This shouldn't happen
            if (_debugLevel >= 1) Serial.println(F("Unexpected HTTP_METHOD."));
            returnVal = -3;  // General error code
        }

        // Translate response to a usable int value
        if (response == "0") {
            // Success
            if (_debugLevel >= 1) Serial.println(F("KPI updated"));
            returnVal = 0;
        }
        else if (response == "1")
        {
            // Fail
            if (_debugLevel >= 1) Serial.println(F("Failed, check KPI GUID"));
            returnVal = -2;
        }
        else
        {
            // Unknown error
            if (_debugLevel >= 1) Serial.println(F("Error: Unexpected server response"));
            returnVal = -3;
        }
    }
    else {
        // Something wrong with internet connection
        if (_debugLevel >= 1) Serial.println(F("Failed to make a connection"));
        returnVal = -1;
    }

    _client.stop();
    return returnVal;
}


/***** PRIVATE METHODS *****/

String BizeoClass::parseXML(String searchStr)
{
    /* 
     * Parse data in the form of XML from web client stream.
     * Looks for an XML tag named <searchStr> and returns it's value.
     * Only works for lowest level tags, e.g  <myData> myValue </myData>
     * Also works if tag has paramters, e.g. <myData param=var>
     */

    boolean tagFlag = false;
    boolean dataFlag = false;

    String tagStr;
    String dataStr;

    unsigned long bytetimer = millis();  // Initialise timer
    char inChar;

    if (_debugLevel >= 2) Serial.println(F("\r\n------------------------------------------------------------"));
    
    while (true) {
        // Endless loop, but returns when either tag is found, or timed out.
        // i.e. If the tag is not found, it will parse the whole stream and then timeout

        if (_client.available()) {
            // Read a character from the stream
            inChar = _client.read();
            if (_debugLevel >= 2) Serial.print(inChar);  // Print http stream for debugging

            switch (inChar) {
            case '<':
                if (dataFlag) {
                    // End of data section, return found data
                    if (_debugLevel >= 2) Serial.println(F("\r\n------------------------------------------------------------"));
                    return dataStr;
                }
                // Start of new tag, set flags and clear strings
                tagFlag = true;
                dataFlag = false;
                tagStr = "";
                dataStr = "";
                break;

            case '>':
                if (tagStr == searchStr) dataFlag = true;  // Only set data flag when correct tag found
                tagFlag = false;
                break;

            default:
                if (tagFlag) {
                    if (inChar == ' ') {
                        // We want to catch the special case when we are reading a tag
                        // and the next character is a space
                        tagFlag = false;  // Clear tagFlag, but leave tagStr populated
                    }
                    else if (tagStr.length() > searchStr.length()) {
                        // Tag is longer than our search string, stop processing it
                        tagFlag = false;
                    }
                    else {
                        tagStr += inChar;  // Add character to tag buffer
                    }
                }
                if (dataFlag) {
                    dataStr += inChar;  // Add character to data buffer
                    if (dataStr.length() > 128) dataFlag = false;  // Defensive driving, discard long strings
                }
            }

            bytetimer = millis();  // Reset timer
        }
        else if ((millis() - bytetimer > BIZEO_TIMEOUT) || !_client.connected()) {
            if (_debugLevel >= 2) Serial.println(F("\r\n------------------------------------------------------------"));
            return "";  // Return an empty string
        }
    }
}

BizeoClass Bizeo;  // For use as global object

/* (c) Copyright 2012 Direct Marketing Software Pty. Ltd. */