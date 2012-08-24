
#include <SPI.h>
#include <Ethernet.h>
#include <BizeoEthernet.h>

// This can be retrieved from your browser's URL when viewing Bizeo
const char USER_GUID[]  = "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX";

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 };

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  delay(1000);

  Serial.println();
  Serial.println(F("# BIZEO EXAMPLE"));
  Serial.println(F("# Connection: Ethernet"));
  Serial.println(F("# Function: getStatus()"));
  Serial.println(F("#"));
  Serial.println(F("# Expected results:"));
  Serial.println(F("#  0 = Green"));
  Serial.println(F("#  1 = Yellow"));
  Serial.println(F("#  2 = Red"));
  Serial.println(F("# -1 = Connection error"));
  Serial.println(F("# -2 = Invalid GUID"));
  Serial.println(F("# -3 = Unknown error"));
  Serial.println();

  Serial.println(F("Initializing ethernet..."));
  if (!Bizeo.begin(mac)) {
    Serial.println("Bizeo failed to initialize, halting.");
    for (;;)
      ;  // Halt
  }

  //Bizeo.setDebugLevel(2);  // Uncomment (or set to 0/1) to adjust verbose output

  int bizeostatus;

  // Bizeo.getStatus(method, guid)
  // The default GET method is the one you should use (i.e. no argument for method).
  // The others are included in case GET does not work,
  // or is disabled in future server revisions

  Serial.print(F("\r\nCalling Bizeo.getStatus() with GET method..."));
  bizeostatus = Bizeo.getStatus(USER_GUID);
  Serial.print(F("returned: "));
  Serial.println(bizeostatus);

  Serial.print(F("\r\nCalling Bizeo.getStatus() with POST method..."));
  bizeostatus = Bizeo.getStatus(POST, USER_GUID);
  Serial.print(F("returned: "));
  Serial.println(bizeostatus);

  Serial.print("\r\nCalling Bizeo.getStatus() with SOAP method..."));
  bizeostatus = Bizeo.getStatus(SOAP, USER_GUID);
  Serial.print("returned: "));
  Serial.println(bizeostatus);
  
  Serial.println(F("\r\n### END ###"));
}


void loop()
{
}
