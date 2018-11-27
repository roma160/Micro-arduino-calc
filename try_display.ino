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
int maxDigitsStringToDouble = 3;
char map1[] = {'1', '2', '3', '*',
         '4', '5', '/', '.',
         '7', '(', ')', '-',
         '=', '0', 'K', 's'};
String input = "";
String pinput = "";
char twoOp[] = { '+', '-','*', '/', '%' };
char oneOp[] = {'s', 'c', 'd', 't', 'S', 'C', 'T', 'R', 'r', '!'};
int startP = 2;
//out
int endP = 5;
//in
int endP2 = 9;

bool flip = false;
int sum = 0;
int up = 0;
double m = 1;

double toD(String da) {
  da.replace("н", "-");
  return da.toDouble();
}

String toS(double D){
 String ret = "";
 ret+=D;
    return ret;
  }

String split(String a, char b, int n) {
  String ret = "";
  for (int i = 0; i < a.length(); i++) {
    if (n == 0) {
      while (a[i] != b && i < a.length()) {
        ret += a[i];
        i++;
      }
      return ret;
    }
    else
      if (a[i] == b)
        n--;
  }
  return ret;
}
int factorial(int a) {
  if (a == 0)
    return 1;
  if (a == 1)
    return 1;
  else
    return factorial(a - 1)*a;
}
double operation(double a, double b, char n) {
  if (n == '+')
    return a + b;
  if (n == '-')
    return a - b;
  if (n == '*')
    return a * b;
  if (n == '/')
    return a / b;
  if (n == '%')
    return a / b * 100;
  else
    return pow(a, b);
}
double operation(double a, char n) {
  if (n == 's')
    return sin(a);
  if (n == 'c')
    return cos(a);
  if (n == 'd')
    return a * PI / 180;
  if (n == 'R')
    return a * 180 / PI;
  if (n == 'r')
    return sqrt(a);
  if (n == 't')
    return tan(a);
  if (n == 'T')
    return atan(a);
  if (n == 'S')
    return asin(a);
  if (n == 'C')
    return acos(a);
  if (n == '!')
    return factorial(a);
}
bool contains(String a, String b) {
  if (a.indexOf(b) >= 0)
    return true;
  else
    return false;
}
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
  for (double i = -(flip ? y - up : x - sum)/m; i < (flip ? y - up : x - sum)/m; i+=(double)1/m*2) {
    String eqa = pinput;
    String repl = "(";
    repl += i;
    repl += ")";
    eqa.replace("x", repl);
    eqa.replace("-", "+-");
    double res = countByStr(eqa);
    double x1 = (flip? (i)*m + sum :(res)*m + up);
    double y1 = (flip? -(res)*m + up :(i)*m + sum);
    i-= (double)1/m*2;
    eqa = pinput;
    repl = "(";
    repl += i;
    repl += ")";
    eqa.replace("x", repl);
    eqa.replace("-", "+-");
    res = countByStr(eqa);
      double x2 = (flip? (i)*m + sum:(res)*m + up);
    double y2 = (flip ? -(res)*m + up : (i)*m + sum);
    dis.drawLine((int)x1, (int)y1, (int)x2, (int)y2, WHITE);
    dis.display();
    i+=(double)1/m*2;
  }
}
double countByStr(String inp) {
  if (contains(inp, "(")) {
    int first = -1;
    int second = -1;
    int n = 0;
    for (int i = 0; i < inp.length(); i++) {
      if (inp[i] == '(') {
        n++;
        first = i + 1;
      }
      else if (inp[i] == ')')
        n--;
      if (n == 0 && first != -1) {
        second = i;
        double res = countByStr(inp.substring(first, second));
        String repl = toS(res);
        repl.replace("-", "н");
        inp.replace(inp.substring(first - 1, second + 1), repl);
        second = -1;
        first =-1;
        i = inp.indexOf('(')-1;
      }
    }
  }
  for(int i = 0; i < sizeof(twoOp); i++)
    if (contains(inp, (String)twoOp[i])) {
      String doWith = split(inp, twoOp[i], 0);
      double first = countByStr(doWith);
      doWith = inp.substring(inp.indexOf(twoOp[i]) + 1);
      double second = countByStr(doWith);
      return operation(first, second, twoOp[i]);
    }
  for(int i = 0; i < sizeof(oneOp); i++)
    if (contains(inp, (String)oneOp[i])) {
      String doWith = split(inp, oneOp[i], 1);
      double first = countByStr(doWith);
      return operation(first, oneOp[i]);
    }
  return toD(inp);
}
void printMap() {
  for (int i = 0; i < 4; i++) {
    dis.setTextColor(WHITE);
    dis.setCursor(128 - 24, 8 * i);
      for (int j = 0; j < 4; j++) {
        dis.print(map1[i * 4 + j]);
        //dis.print(' ');
      }
      dis.print('\n');
  }
  dis.display();
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
    if (map1[button] == 'K' && !graphOn) {
      sum = flip ? y / 2 : x / 2;
      up = flip ? x / 2 : y / 2;
      buildAGraph();
      delay(400);
    }
  }
  if(graphOn)
    if (button == 14 || button == 13) {
      if (flip)
        up += 4;
      else
        up -= 4;
      buildAGraph();
    }
    else if (button == 1 || button == 2) {
      if (flip)
        up -= 4;
      else
        up += 4;
      buildAGraph();
    }
    else if (button == 7 || button == 11) {
      sum += 4;
      buildAGraph();
    }
    else if (button == 4 || button == 8) {
      sum -= 4;
      buildAGraph();
    }
    else if (button == 0) {
      flip = !flip;
      sum = flip ? y / 2 : x / 2;
      up = flip ? x / 2 : y / 2;
      buildAGraph();
    }
    else if (button == 5) {
      m/=2;
      buildAGraph();
    }
    else if (button == 6) {
      m*=2;
      buildAGraph();
    }
}

void printMoreDigit(double inp) {
  dis.print(inp, maxDigitsStringToDouble);
}

void afterClick(int button) {
  if (graphOn) {
    if (button == 15) {
      graphOn = false;
      dis.clearDisplay();
      setupWriting(0, 0);
      dis.print(input + "0");
      printMap();
      dis.display();
    }
  }
  else if (map1[button] == 'K') {
    pinput = input;
    input = "";
    dis.clearDisplay();
    setupWriting(0, 0);
    dis.print(input + "0");
    dis.display();
  }
  else if (map1[button] != '=') {
    input += map1[button];
    dis.clearDisplay();
    setupWriting(0, 0);
    dis.print(input);
    dis.display();
  }
  else {
    dis.clearDisplay();
    setupWriting(0, 0);
    input.replace("-", "+-");
    double res = countByStr(input);
    printMoreDigit(res);
    dis.display();
    input = "";
    input += toS(res);
  }
  if(!graphOn)
  printMap();
  allPinsOff();
  buttonSPressing = millis();
  while(buttonIsStillPressed(button)){
    allPinsOff();
    doWhileButtonPressed(button);
  }

  dis.stopscroll();
  delay(100);
  allPinsOn();
}

void writingInSetup() {
  dis.print(sum);
  dis.display();
  printMap();
}

void loop() {
  if (oneIsOn())
    buttonOnCLick();
}
