/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
                                                                   bedbath.ino 
                                             Copyright © 2019, Zigfred & Nik.S
19.04.2019 v1
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*****************************************************************************\
Сервер bedbath выдает данные: 
  аналоговые: 
    датчики трансформаторы тока  
/*****************************************************************************/
#include <Ethernet2.h>
#include <EmonLib.h>
#define DEVICE_ID "bedbath";
#define VERSION 1

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
EthernetServer httpServer(40250);

EnergyMonitor emon1;
EnergyMonitor emon2;
EnergyMonitor emon3;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            setup
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void setup() {
  Serial.begin(9600);
  while (!Serial) continue;

if (!Ethernet.begin(mac)) {
    Serial.println(F("Failed to initialize Ethernet library"));
    return;
  }
  httpServer.begin();
  Serial.println(F("Server is ready."));
  Serial.print(F("Please connect to http://"));
  Serial.println(Ethernet.localIP()); 
  
  pinMode( A1, INPUT );
  pinMode( A2, INPUT );
  pinMode( A3, INPUT );
  emon1.current(1, 9);
  emon2.current(2, 9);
  emon3.current(3, 9);
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            loop
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void loop() 
{
  
  realTimeService();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            my functions
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            realTimeService
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void realTimeService()
{

  EthernetClient reqClient = httpServer.available();
  if (!reqClient)
    return;

  while (reqClient.available())
    reqClient.read();

  String data = createDataString();

  reqClient.println(F("HTTP/1.1 200 OK"));
  reqClient.println(F("Content-Type: application/json"));
  reqClient.print(F("Content-Length: "));
  reqClient.println(data.length());
  reqClient.println();
  reqClient.print(data);

  reqClient.stop();
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            createDataString
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
String createDataString()
{
  String resultData;
  resultData.concat(F("{"));
  resultData.concat(F("\n\"deviceId\":"));
  //  resultData.concat(String(DEVICE_ID));
  resultData.concat(F("\"bedbath\""));
  resultData.concat(F(","));
  resultData.concat(F("\n\"version\":"));
  resultData.concat((int)VERSION);
  resultData.concat(F(","));
  resultData.concat(F("\n\"data\": {"));
    resultData.concat(F("\n\"bb-trans-1\":"));
    resultData.concat(String(emon1.calcIrms(1480), 1));
    resultData.concat(F(","));
    resultData.concat(F("\n\"bb-trans-2\":"));
    resultData.concat(String(emon2.calcIrms(1480), 1));
    resultData.concat(F(","));
    resultData.concat(F("\n\"bb-trans-3\":"));
    resultData.concat(String(emon3.calcIrms(1480), 1));
    resultData.concat(F("\n}"));
  resultData.concat(F(","));
  resultData.concat(F("\n\"freeRam\":"));
  resultData.concat(freeRam());
  resultData.concat(F(",\n\"upTime\":\""));
  resultData.concat(upTime(millis()));
  resultData.concat(F("\"\n}"));

  return resultData;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            Время работы после старта или рестарта
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
String upTime(uint32_t lasttime)
{
  lasttime /= 1000;
  String lastStartTime;
  
  if (lasttime > 86400) {
    uint8_t lasthour = lasttime/86400;
    lastStartTime.concat(lasthour);
    lastStartTime.concat(F("d "));
    lasttime = (lasttime-(86400*lasthour));
  }
  if (lasttime > 3600) {
    if (lasttime/3600<10) { lastStartTime.concat(F("0")); }
  lastStartTime.concat(lasttime/3600);
  lastStartTime.concat(F(":"));
  }
  if (lasttime/60%60<10) { lastStartTime.concat(F("0")); }
lastStartTime.concat((lasttime/60)%60);
lastStartTime.concat(F(":"));
  if (lasttime%60<10) { lastStartTime.concat(F("0")); }
lastStartTime.concat(lasttime%60);
//lastStartTime.concat(F("s"));

return lastStartTime;
}

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            Количество свободной памяти
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
int freeRam()
{
  extern int __heap_start, *__brkval;
  int v;
  return (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            end
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
 
