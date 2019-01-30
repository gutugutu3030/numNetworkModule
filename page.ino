void handleRoot() {

}

void handleNum() {
  if (Server.method() == HTTP_POST) {
    String nums = Server.arg("num");
    int num=nums.toInt();
    String waits = Server.arg("wait");
    int wait=waits.toInt();
    setNumber(num,wait);
  }
}

