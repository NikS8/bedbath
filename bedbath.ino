/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
                                                                   bedbath.ino 
                                             Copyright © 2019, Zigfred & Nik.S
19.04.2019 v1
10.11.2019 v2 add DHT
13.11.2019 v3 переход на статические IP
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*****************************************************************************\
Сервер bedbath выдает данные: 
  аналоговые: 
    датчики трансформаторы тока
  цифровые:
    датчик температуры и влажности DHT  
/*****************************************************************************/

#include <Ethernet2.h>  // библиотека для работы с Ethernet Shield
#include <EmonLib.h>    // библиотека для работы с трансформаторами тока
#include <DHT.h>        // библиотека для работы с DHT22 

#define DEVICE_ID "bedbath";
#define VERSION 2

#define DHT1TYPE DHT22 
#define DHT1PIND 9      // PIN подключения датчика DTH22
DHT dht1(DHT1PIND, DHT1TYPE);
float sensorDhtTemp;    // температура от датчика DHT22 
int sensorDhtHum;       // влажность от датчика DHT22

byte mac[] = {0xCA, 0x74, 0xBE, 0xBA, 0xFF, 0x01};
IPAddress ip(192, 168, 1, 161);
EthernetServer httpServer(40161); // Ethernet server

EnergyMonitor emon1;
EnergyMonitor emon2;
EnergyMonitor emon3;

/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*\
            setup
\*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void setup() {
  Serial.begin(9600);
  Serial.println("Serial.begin(9600)"); 

  Ethernet.begin(mac,ip);
  
  Serial.println(F("Server is ready."));
  Serial.print(F("Please connect to http://"));
  Serial.println(Ethernet.localIP()); 
   
  httpServer.begin();
  
  pinMode( A1, INPUT );
  pinMode( A2, INPUT );
  pinMode( A3, INPUT );
  emon1.current(1, 9);
  emon2.current(2, 9);
  emon3.current(3, 9);

  dht1.begin();
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

  sensorDhtHum = dht1.readHumidity();
  sensorDhtTemp = dht1.readTemperature();    

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
  
    resultData.concat(F("\n\"bed-trans-1\":"));
    resultData.concat(String(emon1.calcIrms(1480), 1));
    resultData.concat(F(","));
    resultData.concat(F("\n\"bed-trans-2\":"));
    resultData.concat(String(emon2.calcIrms(1480), 1));
    resultData.concat(F(","));
    resultData.concat(F("\n\"bed-trans-3\":"));
    resultData.concat(String(emon3.calcIrms(1480), 1));
    resultData.concat(F(","));

    resultData.concat(F("\n\"bed-Hum %\":"));
    resultData.concat(sensorDhtHum);
    resultData.concat(F(","));
    resultData.concat(F("\n\"bed-Temp °C\":"));
    resultData.concat(sensorDhtTemp);
        
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
