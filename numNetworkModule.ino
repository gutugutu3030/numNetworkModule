#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <ESP8266WiFiMulti.h>


#define ID_ADDR 0
#define DATA_ADDR 1
#define EEPROMSIZE 10

ESP8266WiFiMulti wifiMulti;

ESP8266WebServer Server(80);         //  ウェブサーバ＠ポート番号（HTTP）
ESP8266HTTPUpdateServer Updater;     //  ウェブアップデート
WiFiUDP Udp;

const char *ssid = "numNetwork";
const char *pass = "numnumnum";      //  ８文字以上

int ipid = 100;
int data = -1;

void setup() {
  init7();
  Serial.begin(115200);
  EEPROM.begin(EEPROMSIZE);
  ipid = EEPROM.read(ID_ADDR);
  if (ipid == 1) {
    //APモード
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, pass);
    showSeg(ipid);
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
    wifiMulti.addAP("mogumogu",     "gu2gu2Network");
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
  }
  EEPROM.end();
  Server.on("/", handleRoot);
  Udp.begin(8888);

  Updater.setup(&Server, "gutu", "3030");
  Server.begin();
}

int cnt = 0;
unsigned long randomTime = 0;

void setNumber(OSCMessage &mes) {
  switch (mes.size()) {
    case 1:
      if (mes.isInt(0)) {
        data = mes.getInt(0);
        seg7(data);
      }
    case 2:
      if (mes.isInt(0) && mes.isInt(1)) {
        randomTime = mes.getInt(1) + millis();
        data = mes.getInt(0);
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

void loop() {
  //  クライアントからの要求を処理する
  Server.handleClient();
  //  main loop
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
    } else {
      Serial.println("error");
    }
  }
  if (randomTime > millis()) {
    if (millis() % 30 == 0) {
      int rnd = (int)random(9);
      seg7(rnd < data ? rnd : (rnd + 1));
    }
  } else {
    seg7(data);
  }
}
