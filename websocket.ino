void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
      /* 応答処理を書く */
      Serial.printf("[%u] get Text: %s\n", num, payload);
      break;
  }
}
