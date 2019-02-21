#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <ESP8266WiFiMulti.h>
#include <FS.h>
#include <WebSocketsServer.h>
#include "hardware.h"

#define ID_ADDR 0
#define DATA_ADDR 1
#define EEPROMSIZE 10
#define RSSI_SEARCH_INTERVAL 100

ESP8266WiFiMulti wifiMulti;

ESP8266WebServer Server(80);         //  ウェブサーバ＠ポート番号（HTTP）
ESP8266HTTPUpdateServer Updater;     //  ウェブアップデート
WiFiUDP Udp;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);

WebSocketsServer webSocket = WebSocketsServer(443);

const char *ssid = "numNetwork";
const char *pass = "numnumnum";      //  ８文字以上

int ipid = 100;
int data = -1;

boolean isClient = false;
unsigned long lastRssiSerchTime = 0;
int distance = 0;

#ifdef NIXIE
#define DEPTH_LEN 18
const int numberDepth[DEPTH_LEN] = {1, 0, 2, 9, 3, 8, 4, 7, 5, 6, 5, 7, 4, 8, 3, 9, 2, 0};
int randomDir = 1;
int randomCurrentIndex = 0, randomTargetIndex = 0;
int randomWaitTime = 1;
#endif

void setup() {
  init7();
  SPIFFS.begin();
  Serial.begin(115200);
  EEPROM.begin(EEPROMSIZE);
  ipid = EEPROM.read(ID_ADDR);
  if (ipid == 1) {
    //APモード
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, pass);
    showSeg(ipid);
    isClient = false;
  } else {
    //クライアントモード
    if (ipid <= 0 || ipid > 255) {
      //ipが設定されてなければ100に固定
      ipid = 100;
      EEPROM.write(ID_ADDR, ipid);
      EEPROM.commit();
    }
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP("gutugutuBlade", "gu2gu2Network");
    wifiMulti.addAP(ssid, pass);
    while (wifiMulti.run() != WL_CONNECTED) {
      delay(1000);
    }
    IPAddress nowIP = WiFi.localIP();
    IPAddress gateway(nowIP[0], nowIP[1], nowIP[2], 1);
    IPAddress ip(nowIP[0], nowIP[1], nowIP[2], ipid);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.config(ip, gateway, subnet);
    showSeg(ipid);
    isClient = true;
    postToOne();
  }
  EEPROM.end();
  Server.on("/num", handleNum);
  Server.on("/nums", handleNum);
  Server.onNotFound(handleNotFound);
  Udp.begin(8888);

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  Updater.setup(&Server, "gutu", "3030");
  Server.begin();
}

int cnt = 0;
unsigned long randomTime = 0;
unsigned long startTime = 0;

void postToOne() {
  OSCMessage mes("/shake");
  mes.add(ipid);
  mes.add(data);
  IPAddress nowIP = WiFi.localIP();
  IPAddress gateway(nowIP[0], nowIP[1], nowIP[2], 1);
  Udp.beginPacket(gateway, 8888);
  mes.send(Udp);
  Udp.endPacket();
  mes.empty();
}

void setNumber(int x, int wait = 0, float speed = 0) {
  if (speed == 0) {
    startTime = millis();
  } else {
    startTime = millis() + (int)(distance / speed);
  }
  randomTime = wait + startTime;
#ifdef NIXIE
  randomWaitTime = wait;
  for (int i = 0; i < DEPTH_LEN; i++) {
    if (numberDepth[i] == x) {
      randomTargetIndex = i;
      if(wait==0){
        randomCurrentIndex=i;
      }
      if (i < randomCurrentIndex) {
        randomDir = 1;
      } else {
        randomDir = -1;
      }
      break;
    }
  }
#endif
  data = x;
  if (wait == 0) {
    seg7(data);
  }
}

void setNumber(OSCMessage &mes) {
  switch (mes.size()) {
    case 1:
      if (mes.isInt(0)) {
        setNumber(mes.getInt(0));
      }
    case 2:
      if (mes.isInt(0) && mes.isInt(1)) {
        setNumber(mes.getInt(0), mes.getInt(1));
      }
    case 3:
      if (mes.isInt(0) && mes.isInt(1) && (mes.getFloat(2))) {
        setNumber(mes.getInt(0), mes.getInt(1), mes.getFloat(2));
      }
      else if (mes.isInt(0) && mes.isInt(1) && (mes.isDouble(2))) {
        setNumber(mes.getInt(0), mes.getInt(1), (float)mes.getDouble(2));
      }
  }
}

void saveIP(OSCMessage &mes) {
  if (mes.size() == 1 && mes.isInt(0)) {
    ipid = mes.getInt(0);
    EEPROM.begin(EEPROMSIZE);
    EEPROM.write(ID_ADDR, ipid);
    EEPROM.commit();
    EEPROM.end();
  }
}

void addClient(OSCMessage &mes) {
  if (mes.size() == 2 && mes.isInt(0) && mes.isInt(1)) {
  }
}

void loop() {
  //  クライアントからの要求を処理する
  Server.handleClient();
  //  main loop
  webSocket.loop();
  OSCMessage  mes;
  int packetCount = Udp.parsePacket();
  if (packetCount > 0) {
    Serial.println("receive");
    while (packetCount--) {
      mes.fill(Udp.read());
    }
    if (!mes.hasError()) {
      mes.dispatch("/num", setNumber);
      mes.dispatch("/id", saveIP);
      mes.dispatch("/shake", addClient);
    } else {
      Serial.println("error");
    }
  }
  if (startTime >millis()){
    
  }
#ifdef NIXIE
  else if (randomTime <= millis()) {
    if (randomWaitTime != 0 && randomTargetIndex != randomCurrentIndex) {
      randomTime += randomWaitTime;
      randomCurrentIndex = (randomCurrentIndex + randomDir + DEPTH_LEN) % DEPTH_LEN;
      seg7(numberDepth[randomCurrentIndex]);
    } else {
      seg7(data);
    }
  }

#else
  else if (randomTime > millis()) {
    if (millis() % 30 == 0) {
      int rnd = (int)random(9);
      seg7(rnd < data ? rnd : (rnd + 1));
    }
  } else {
    seg7(data);
  }
#endif
  if (isClient && lastRssiSerchTime + RSSI_SEARCH_INTERVAL < millis()) {
    long rssi = WiFi.RSSI();
    double d = pow(10.0f , (float)((20.5f - WiFi.RSSI()) / 20.0f));
    distance = (int)d / 10;
    lastRssiSerchTime = millis();
  }
}
