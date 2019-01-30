//pins:
// 1
//2 0
// 3
//6 4
// 5
const int pins[7] = {4,5,13,15,16,12,14};
//const int pins[7] = {14, 12, 13, 4, 5, 16, 15};
const int num[11][7] = {
  {LOW, LOW, LOW, HIGH, LOW, LOW, LOW},
  {LOW, HIGH, HIGH, HIGH, LOW, HIGH, HIGH},
  {LOW, LOW, HIGH, LOW, HIGH, LOW, LOW},
  {LOW, LOW, HIGH, LOW, LOW, LOW, HIGH},
  {LOW, HIGH, LOW, LOW, LOW, HIGH, HIGH},
  {HIGH, LOW, LOW, LOW, LOW, LOW, HIGH},
  {HIGH, LOW, LOW, LOW, LOW, LOW, LOW},
  {LOW, LOW, HIGH, HIGH, LOW, HIGH, HIGH},
  {LOW, LOW, LOW, LOW, LOW, LOW, LOW},
  {LOW, LOW, LOW, LOW, LOW, LOW, HIGH}
};
int showData=-2;

void init7(){
  for(int i=0;i<7;i++){
    pinMode(pins[i],OUTPUT);
  }
}

void seg7(int x) {
  if (0 <= x && x < 10) {
    if(showData==x){
      return;
    }
    showData=x;
    for (int i = 0; i < 7; i++) {
      digitalWrite(pins[i], num[x][i]);
    }
  } else {
    showData=x;
    for (int i = 0; i < 7; i++) {
      digitalWrite(pins[i], HIGH);
    }
  }
}

void showSeg(int x) {
  if(x<0){
    return;
  }
  int num=1,x2=x;
  while(x2>=10){
    x2/=10;
    num++;
  }
  for(int i=num-1;i>=0;i--){
    int x1=x;
    for(int j=0;j<i;j++){
      x1/=10;
    }
    seg7(x1 % 10);
    delay(700);
    seg7(-1);
    delay(300);
  }
}

