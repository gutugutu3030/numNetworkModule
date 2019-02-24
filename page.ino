void handleNotFound() {
  if (! handleFileRead(Server.uri())) {
    //  ファイルが見つかりません
    Serial.println("404 not found");
    Server.send(404, "text/plain", "File not found in Dongbeino...");
  }
}

//  MIMEタイプを推定
String getContentType(String filename) {
  if (filename.endsWith(".html") || filename.endsWith(".htm")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  else if (filename.endsWith(".jpg")) return "image/jpeg";
  else return "text/plain";
}

//  SPIFSS のファイルをクライアントに転送する
bool handleFileRead(String path) {
  Serial.println("handleFileRead: trying to read " + path);
  // パス指定されたファイルがあればクライアントに送信する
  bool isIndex = false;
  if (path.endsWith("/")) {
    path += "index.html";
    isIndex = true;
  }
  String contentType = getContentType(path);
  if (SPIFFS.exists(path)) {
    Serial.println("handleFileRead: sending " + path);
    File file = SPIFFS.open(path, "r");
    Server.streamFile(file, contentType);
    file.close();
    Serial.println("handleFileRead: sent " + path);
    return true;
  }
  else {
    Serial.println("handleFileRead: 404 not found");
    Server.send (404, "text/plain", "ESP: 404 not found");
    return false;
  }
}

void handleNum() {
  if (Server.method() == HTTP_POST) {
    String nums = Server.arg("num");
    int num = nums.toInt();

    String waits = Server.arg("wait");
    int wait = waits.toInt();

    String targets = Server.arg("self");
    int target = targets.toInt();

    String speeds = Server.arg("speed");
    float speed = speeds.toFloat();
    

    switch (target) {
      case 1:
        //self
        setNumber(num, wait, speed);
        break;
      case 0:
        //broadcast
        setNumber(num, wait, speed);
        broadcastNum(num, wait, speed);
        break;
    }
    Server.send(200, "application/json", "{}");
  }
}

void handleNums() {

}

void handleShake() {
  if (Server.method() == HTTP_POST) {
    String nums = Server.arg("num");
    int num = nums.toInt();
    String ids = Server.arg("id");
    int id = ids.toInt();
  }

}

