#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

unsigned long buttonSPressing = 0;
#define OLED_Address 0x3C
Adafruit_SSD1306 dis(1);

bool graphOn = false;
int x = 32;
int y = 128;
int longPressTime = 1000;
int delayInLongPress = 100;
char map1[] = {'1', '2', '3', '+',
         '4', '5', '6', '-',
         '7', '8', '9', '*',
         '=', '0', 'r', '/'};
String input = "";
int startP = 2;
//out
int endP = 5;
//in
int endP2 = 9;

int sum = 0;

void allPinsOn() {
  for (int i = startP; i <= endP; i++) {
    digitalWrite(i, HIGH);
  }
}

void allPinsOff() {
  for (int i = startP; i <= endP; i++) {
    digitalWrite(i, LOW);
  }
}

bool oneIsOn() {
  return digitalRead(6) || digitalRead(7) || digitalRead(8) || digitalRead(9);
}

int pinHig() {
  for (int i = endP + 1; i <= endP2; i++)
    if (digitalRead(i)) {
      return i - 6;
    }
}

int buttonWhatIsOn() {
  for (int i = startP; i <= endP; i++) {
    allPinsOff();
    digitalWrite(i, HIGH);
    if (oneIsOn()) {
      return (i - 2) * 4 + pinHig();
    }
  }
  return -1;
}

void setupWriting(int x, int y) {
  dis.setTextColor(WHITE);
  dis.setCursor(x, y);
}

bool buttonIsStillPressed(int button) {
  digitalWrite(button / 4 + 2, HIGH);
  return digitalRead(button%4 + 6);
}

void buildAGraph() {
  graphOn = true;
  dis.clearDisplay();
  dis.display();
  //for x^2
  for (int i = 0; i < x; i++) {
    i -= x / 2 + sum;
    int x1 = i * i / 4;
    int y1 = i + x / 2 + sum;
    int x2 = (i-1) * (i-1) / 4;
    int y2 = i + x / 2 + sum-1;
    dis.drawLine(x1, y1, x2, y2, WHITE);
    dis.display();
    i += x / 2 + sum;
  }
}

float countByStr() {
  bool firstWas = false;
  float first = 0;
  float second = 0;
  char dos = '\0';
  for (int i = 0; i < input.length(); i++) {
    if (!firstWas) {
      if ((int)input[i] > 47 && (int)input[i] < 72) {
        first *= 10;
        first += (int)input[i] - 48;
      }
      else {
        firstWas = true;
        dos = input[i];
      }
    }
    else {
      second *= 10;
      second += (int)input[i] - 48;
    }
  }
  if (dos == '+')
    return first + second;
  if (dos == '-')
    return first - second;
  if (dos == '*')
    return first * second;
  if (dos == '/')
    return first / second;
  if (dos == 'r')
    return sqrt(first);
}

void setup() {
  dis.begin(SSD1306_SWITCHCAPVCC, OLED_Address);
  dis.clearDisplay();
  dis.display();
  Serial.begin(9600);

  //pins def
  for (int i = startP; i <= endP; i++)
    pinMode(i, OUTPUT);
  for (int i = endP + 1; i < endP2; i++)
    pinMode(i, INPUT);
  allPinsOn();
  //end pins def

  setupWriting(0, 0);
  writingInSetup();
}

void buttonOnCLick() {
  allPinsOff();
  int button = buttonWhatIsOn();
  allPinsOn();
  afterClick(button);
}

void doWhileButtonPressed(int button) {
  if (millis() - buttonSPressing > longPressTime) {
    delay(400);
  }
}

void afterClick(int button) {
  if (input == "" && map1[button] == 'r') {
    buildAGraph();
  }
  else if (graphOn) {
    if (button == 7 || button == 11) {
      sum += 1;
      buildAGraph();
    }
    else if (button == 4 || button == 8) {
      sum -= 1;
      buildAGraph();
    }
    else if (button == 14) {
      graphOn = false;
    }
  }
  else if (map1[button] != '=') {
    input += map1[button];
    setupWriting(0, 0);
    dis.clearDisplay();
    dis.print(input);
    dis.display();
  }
  else {
    setupWriting(0, 0);
    dis.clearDisplay();
    dis.print(countByStr());
    dis.display();
    input = "";
  }

  allPinsOff();
  buttonSPressing = millis();
  while(buttonIsStillPressed(button)){
    allPinsOff();
    doWhileButtonPressed(button);
  }

  allPinsOn();
}

void writingInSetup() {
  dis.print(sum);
  dis.display();
}

void loop() {
  if (oneIsOn())
    buttonOnCLick();
}
