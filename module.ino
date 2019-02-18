#define NIXIE


#ifdef NIXIE
//pins:[16,4,12,5] C D A B
//
const int mpinNum = 4;
const int pins[mpinNum] = {12, 5, 16, 4};
const int num[10][mpinNum] = {
  {LOW, LOW, LOW, LOW},
  {HIGH, LOW, LOW, LOW},
  {LOW, HIGH, LOW, LOW},
  {HIGH, HIGH, LOW, LOW},
  {LOW, LOW, HIGH, LOW},
  {HIGH, LOW, HIGH, LOW},
  {LOW, HIGH, HIGH, LOW},
  {HIGH, HIGH, HIGH, LOW},
  {LOW, LOW, LOW, HIGH},
  {HIGH, LOW, LOW, HIGH},
};

#else
//pins:
// 1
//2 0
// 3
//6 4
// 5
const int mpinNum = 7;
const int pins[mpinNum] = {4, 5, 13, 15, 16, 12, 14};
const int num[11][mpinNum] = {
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
#endif

int showData = -2;

void init7() {
  for (int i = 0; i < mpinNum; i++) {
    pinMode(pins[i], OUTPUT);
  }
}

void seg7(int x) {
  if (0 <= x && x < 10) {
    if (showData == x) {
      return;
    }
    showData = x;
    for (int i = 0; i < mpinNum; i++) {
      digitalWrite(pins[i], num[x][i]);
    }
  } else {
    showData = x;
    for (int i = 0; i < mpinNum; i++) {
      digitalWrite(pins[i], HIGH);
    }
  }
}

void showSeg(int x) {
  if (x < 0) {
    return;
  }
  int num = 1, x2 = x;
  while (x2 >= 10) {
    x2 /= 10;
    num++;
  }
  for (int i = num - 1; i >= 0; i--) {
    int x1 = x;
    for (int j = 0; j < i; j++) {
      x1 /= 10;
    }
    seg7(x1 % 10);
    delay(700);
    seg7(-1);
    delay(300);
  }
}

