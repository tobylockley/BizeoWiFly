/* Bizeo-Ethernet library for Arduino
 * Author: Toby Lockley
 * (c) Copyright 2012 Direct Marketing Software Pty. Ltd.
 * All rights reserved.
 */

#include "BizeoWiFly.h"

#define BIZEO_WS_DOMAIN   "bizeo.com.au"
#define BIZEO_TIMEOUT     (1000 * 15)  // Timeout (ms) for an active connection

int BizeoClass::begin(const char* ssid, const char* passphrase)
{
    int returnVal;
    WiFly.begin();
    
    if (WiFly.join(ssid, passphrase)) {
        if (_debugLevel >= 2) {
            Serial.print(F("Bizeo successfully initialized WiFly shield."));
        }
        returnVal = 1;
    }
    else {
        if (_debugLevel >= 2) {
            Serial.println(F("Wifi association failed (check wifi details)."));
        }
        returnVal = 0;
    }
    return returnVal;
}

void BizeoClass::setDebugLevel(int level)
{
    _debugLevel = level;
}

int BizeoClass::getStatus(String guid)
{
    /* Invokes Bizeo web service to retrieve current Bizeo status.
     */

    String response;  // Store the response from the web server, ready for analysis
    int returnVal;
    char inChar;

    if (_debugLevel >= 2) Serial.println();  // Make more readable in verbose mode
    if (_debugLevel >= 1) Serial.print(F("Connecting..."));

    // Attempt to connect
    if (_client.connect(BIZEO_WS_DOMAIN, 80)) {
        if (_debugLevel >= 1) Serial.print(F("Success..."));

        _client.flush();
        _client.print(F("GET /Monitor/MasterGuid/"));
        _client.print(guid);
        _client.print(F(" HTTP/1.0\n\n"));

        // Parse GET response to retrieve content
        if (_debugLevel >= 2) Serial.println(F("\r\n---------------------------------------"));

        if (scanResponse(" 200 OK")) {
            // GET request was successful
            if (scanResponse("\r\n\r\n")) {
                // Beginning of content
                response = getLineFromClient();
                if (_debugLevel >= 2) Serial.println(F("\r\n---------------------------------------"));

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
                else
                {
                    // Unkown error
                    if (_debugLevel >= 1) Serial.println(F("Error: Unexpected server response"));
                    returnVal = -3;
                }
            }
            else {
                // Timeout or no content
                if (_debugLevel >= 1) Serial.println(F("Connection timed out"));
                returnVal = -1;
            }
        }
        else {
            // Unexpected response, server error or incorrect GUID
            if (_debugLevel >= 2) Serial.println(F("\r\n---------------------------------------"));
            if (_debugLevel >= 1) Serial.println(F("Error: Incorrect GUID or Internal Server Error"));
            returnVal = -2;
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
    return updateKpi(guid, String(value));
}

int BizeoClass::updateKpi(String guid, String value)
{
    /* Invokes Bizeo web service to update an external KPI.
     */

    unsigned int length = value.length();  // Used for content length of POST requests
    String response;  // Store the response from the web server, ready for analysis
    int returnVal;

    if (_debugLevel >= 2) Serial.println();  // Make more readable in verbose mode
    if (_debugLevel >= 1) Serial.print(F("Connecting..."));

    // Attempt to connect
    if (_client.connect(BIZEO_WS_DOMAIN, 80)) {
        if (_debugLevel >= 1) Serial.print(F("Success..."));

        length += 42;  // Length of:    jsonText={"Success": true,"Results": [""]}

        _client.flush();
        _client.print(F("POST /Task/"));
        _client.print(guid);
        _client.print(F("/PushLog HTTP/1.0\n"));
        _client.print(F("Content-Length: "));
        _client.println(length);
        _client.print(F("Content-Type: application/x-www-form-urlencoded\n\n"));

        _client.print(F("jsonText={\"Success\": true,\"Results\": [\""));
        _client.print(value);
        _client.print(F("\"]}"));

        ////////
        // TODO: Implement error detection when it is supported on the server
        ////////
        
        if (scanResponse(" 200 OK")) {
            // GET request was successful
            if (scanResponse("\r\n\r\n")) {
                if (_debugLevel >= 1) Serial.println(F("KPI Updated."));
                returnVal = 0;
            }
            else {
                // Timeout or no content
                if (_debugLevel >= 1) Serial.println(F("Connection timed out."));
                returnVal = -1;
            }
        }
        else {
            // Unexpected response, server error or incorrect GUID
            if (_debugLevel >= 1) Serial.println(F("Error: Incorrect GUID or Internal Server Error"));
            returnVal = -2;
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

int BizeoClass::scanResponse(String searchStr)
{
    /* Private helper function. Scans incoming data for "searchStr".
     * Returns 1 if found, 0 if not found or timeout.
     */

    unsigned long bytetimer = millis();  // Initialise timer
    char inChar;
    char searchChar;
    int searchCharIndex;
    int charsMatched;
    
    if (searchStr.length() == 0) {
        return 0;  // Just in case an empty string in passed
    }
    
    charsMatched = 0;
    searchCharIndex = 0;
    searchChar = searchStr.charAt(0);
    while (true) {
        // Endless loop, but returns when either "searchStr" found, end of content, or timed out.

        if (_client.available()) {
            // Read a character from the stream
            inChar = _client.read();
            if (_debugLevel >= 2) Serial.print(inChar);  // Print http stream for debugging

            if (inChar == searchChar) {
                charsMatched++;
                if (charsMatched == searchStr.length()) {
                    return 1;  // Whole string has been matched
                }
                searchCharIndex++;
                searchChar = searchStr.charAt(searchCharIndex);
            }
            else {
                // Reset search variables
                charsMatched = 0;
                searchCharIndex = 0;
                searchChar = searchStr.charAt(0);
            }

            bytetimer = millis();  // Reset timer
        }
        else if ((millis() - bytetimer > BIZEO_TIMEOUT) || !_client.connected()) {
            return 0;  // Timeout or string not found
        }
    }
}

String BizeoClass::getLineFromClient()
{
    /* Private helper function. Returns a whole line of text from the web client
     */

    unsigned long bytetimer = millis();  // Initialise timer
    char inChar;
    String returnString = "";

    while (true) {
        // Endless loop, but returns when either EOL reached or timed out.

        if (_client.available()) {
            // Read a character from the stream
            inChar = _client.read();
            if (_debugLevel >= 2) Serial.print(inChar);  // Print http stream for debugging
            if (inChar == '\n' || inChar == '\r') {
                break;
            }
            returnString += inChar;
            bytetimer = millis();  // Reset timer
        }
        else if ((millis() - bytetimer > BIZEO_TIMEOUT) || !_client.connected()) {
            break;  // Timeout
        }
    }
    return returnString;
}

BizeoClass Bizeo;  // For use as global object

/* (c) Copyright 2012 Direct Marketing Software Pty. Ltd. */