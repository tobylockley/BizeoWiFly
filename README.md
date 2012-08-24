# BizeoWiFly Library
##### Interfaces to the [Bizeo](http://bizeo.com.au/) web services via simple HTTP calls. Requires an arduino, the arduino IDE (version 1.0 or later), and a WiFly shield from Sparkfun (or Little Bird Electronics).

This library takes away all the hassle of working with the Bizeo web services with an arduino by tacking on to the WiFly library. Feel free to browse the source code to see how they are called and parsed. An overview of all the available web services can be found [here](http://bizeocloudws.cloudapp.net/PublicWS.asmx).

This library implements all available HTTP methods (GET, POST and SOAP). If the web service functions are called without specifying which method to use, the GET method is used. This is because it sends and receives the least amount of data, so can speed up web interaction.

You cannot use this library and the equivalent [BizeoEthernet](https://github.com/tobylockley/BizeoEthernet) library at the same time.

## Installation
Download the library as a zip (button above). Copy the folder in the zip to your arduino libraries folder *<strong>"...ArduinoSketchbook/libraries/"</strong>* and rename to *<strong>BizeoWiFly</strong>*.
You may have to create the libraries folder. Sketchbook location can be found in the arduino IDE preferences. Examples:

* Windows: "My Documents\Arduino\libraries\BizeoWiFly\*\<source_files\>*"
* Mac: "~/Documents/Arduino/libraries/BizeoWiFly/*\<source_files\>*"

## Usage
First, you must include all relevant library files:

````
#include <SPI.h>
#include <WiFly.h>
#include <BizeoWiFly.h>
````

Then, call Bizeo.begin in the setup function: 
**NOTE: If the WiFly does not associate with the specified network, it will enter an endless retry loop. Diagnose by setting debug level before calling begin.**

````c
void setup()
{
    String mySSID = "ssid_name";
    String myPASSPHRASE = "secret_passphrase";
    Bizeo.begin(mySSID, myPASSPHRASE));
}
````

Then, use the Bizeo web calls any time you need them:

````c
int status = Bizeo.getStatus(user_guid);
// Do something with status

int update = Bizeo.updateKpi(kpi_guid, new_value);
// Check the output to make sure it worked
````

Occasionally, the internet requests may fail. This can be due to an unplugged wire, an ISP malfunction, or even a server error. It is a good idea to always check the output of the web service functions to make sure they succeeded, and handle the errors when they don't (retry, give a warning, etc).

Full examples can be seen in **File > Examples > BizeoEthernet**.

## Overview of Functions

###### Note: Some return values differ from what is returned by the web service. This is to provide consistency between the functions and provide a more meaningful return value within the arduino environment.

--------------------------------

````c
int begin(String ssid, String passphrase)
````

Initializes the Bizeo library and WiFly shield. Must provide a valid SSID and passphrase.

##### *Input*
* ``ssid``  --  String representing the name of the wireless network
* ``passphrase``  --  secret passphrase to use

##### *Output*
* ``1``  --  Succeeded to initialize ethernet shield and obtain IP address
* ``0``  --  Failed to initialize ethernet
<br><br>

--------------------------------

````c
void setDebugLevel(unsigned int level)
````

Sets the level of verbosity desired. Currently only values 0, 1, and 2 are implemented, but larger values will not cause errors, simply allow debug levels up to and including the given value.

##### *Input*
* ``level`` -- Desired verbosity level
    * ``0``  --  No debug output
    * ``1``  --  Light debug messages
    * ``2``  --  Detailed (verbose) debug messages

##### *Output*
* ``void``  --  No output
<br><br>

--------------------------------

````c
int getStatus(String userGuid)
int getStatus(HTTP_METHOD method, String userGuid)
````

When a valid user GUID is supplied, will return the current Bizeo status for that user. The optional ``method`` parameter is a custom enum (defined in the header file) representing the HTTP request method to use. When no method is given, the default of HTTP GET is used. This option is provided in case GET requests are ever disabled in the future and an alternate method needs to be used.

##### Input
* ``userGuid``  --  Bizeo user GUID, which can be extracted from the url when browsing to the Bizeo status page in a browser
* ``method``  --  HTTP request method to use for this web service call. Valid arguments are:
    * ``GET``
    * ``POST``
    * ``SOAP``

##### Output
* &nbsp;``0``  --  Green
* &nbsp;``1``  --  Yellow
* &nbsp;``2``  --  Red
* ``-1``  --  Connection error
* ``-2``  --  Invalid GUID
* ``-3``  --  Unknown server response
<br><br>

--------------------------------

````c
int updateKpi(String kpiGuid, int value)
int updateKpi(HTTP_METHOD method, String kpiGuid, int value)
````

When a valid KPI GUID is supplied, a request is sent to the Bizeo server, which will attempt to update the KPI with the new value. Like the ``getStatus()`` function, there is an optional ``method`` parameter. When no method is given, the default of HTTP GET is used.

##### Input
* ``kpiGuid``  --  Bizeo external KPI GUID, which can be found by browsing to an external KPI in your browser, and clicking on settings
* ``value``  --  New KPI value
* ``method``  --  HTTP request method to use for this web service call. Valid arguments are:
    * ``GET``
    * ``POST``
    * ``SOAP``

##### Output
* &nbsp;``0``  --  Success
* ``-1``  --  Connection error
* ``-2``  --  Invalid GUID
* ``-3``  --  Unknown server response
<br><br>

--------------------------------

*&copy; 2012 Direct Marketing Software Pty Ltd. All rights reserved.*

