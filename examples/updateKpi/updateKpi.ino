
#include <SPI.h>
#include <Ethernet.h>
#include <BizeoEthernet.h>

// This can be retrieved from going to the settings on an external KPI
const char KPI_GUID[]  = "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX";

byte mac[] = { 
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0x01 };

void setup()
{
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  delay(1000);
        
  Serial.println(F(""));
  Serial.println(F("# BIZEO EXAMPLE"));
  Serial.println(F("# Connection: Ethernet"));
  Serial.println(F("# Function: updateKpi()"));
  Serial.println(F("#"));
  Serial.println(F("# Expected results:"));
  Serial.println(F("#  0 = Update successful"));
  Serial.println(F("# -1 = Connection error"));
  Serial.println(F("# -2 = Invalid GUID"));
  Serial.println(F("# -3 = Unknown error"));
  Serial.println(F(""));

  Serial.println(F("Initializing ethernet..."));
  if (!Bizeo.begin(mac)) {
    Serial.println("Bizeo failed to initialize, halting.");
    for (;;)
      ;  // Halt
  }

  //Bizeo.setDebugLevel(2);  // Uncomment (or set to 0/1) to adjust verbose output

  int updatestatus;

  // Bizeo.updateKpi(method, guid, value)
  // The default GET method is the one you should use (i.e. no argument for method).
  // The others are included in case GET does not work,
  // or is disabled in future server revisions

  Serial.print("\r\nCalling Bizeo.getStatus() with GET method...");
  updatestatus = Bizeo.updateKpi(KPI_GUID, 100);
  Serial.print("returned: ");
  Serial.println(updatestatus);

  Serial.print("\r\nCalling Bizeo.getStatus() with POST method...");
  updatestatus = Bizeo.getStatus(POST, KPI_GUID, 200);
  Serial.print("returned: ");
  Serial.println(updatestatus);

  Serial.print("\r\nCalling Bizeo.getStatus() with SOAP method...");
  updatestatus = Bizeo.getStatus(SOAP, KPI_GUID, 300);
  Serial.print("returned: ");
  Serial.println(updatestatus);
  
  Serial.println(F("\r\n### END ###"));
}


void loop()
{
}
