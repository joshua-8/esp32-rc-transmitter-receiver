#include <Arduino.h> // tested with esp32 boards version 3.2.1
#include <ESP32_easy_wifi_data.h> // tested with version 1.4.3
/*
   Connects to a wifi network
   Reads a joystick and sends 4 channels of information
*/

const int ledPin = 2;
const int buttonPin = 0;

const byte ch1Pin = 34; // forwards joystick axis
const byte ch2Pin = 35; // right joystick axis
const byte ch3Pin = 26; // stop switch, connect to ground to turn off
const byte ch4Pin = 27; // who has control switch, connect to ground to turn off override

#define CH1CONFIG 50, 1640, 3770, 100 // back, center, forwards, deadzone
#define CH2CONFIG 4080, 1973, 100, 100 // left, center, right, deadzone

int ch1Val;
int ch2Val;
int ch3Val;
int ch4Val;

// data receive callback
void WifiDataToParse()
{
  digitalWrite(ledPin, EWD::recvBl());
}
// data send callback
void WifiDataToSend()
{
  EWD::sendIn(ch1Val);
  EWD::sendIn(ch2Val);
  EWD::sendIn(ch3Val);
  EWD::sendIn(ch4Val);
}

void configWifi()
{
  EWD::mode = EWD::Mode::connectToNetwork;
  EWD::routerName = "gbgCarDemo";
  EWD::routerPassword = "2521sert"; // no one will ever guess this
  EWD::routerPort = 25001;
  EWD::communicateWithIP = "192.168.4.1"; // the address the receiver gives itself in AP mode
  EWD::debugPrint = true;
}

void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  configWifi();
  EWD::setupWifi(WifiDataToParse, WifiDataToSend);
  Serial.println("controller running");

  pinMode(ch3Pin, INPUT_PULLUP);
  pinMode(ch4Pin, INPUT_PULLUP);
}

void loop()
{
  float ch1 = InputReader_JoystickAxis(analogRead(ch1Pin), CH1CONFIG);
  float ch2 = InputReader_JoystickAxis(analogRead(ch2Pin), CH2CONFIG);
  float ch3 = digitalRead(ch3Pin) ? 1 : -1;
  float ch4 = digitalRead(ch4Pin) ? 1 : -1;

  ch1Val = ch1 * 500 + 1500;
  ch2Val = ch2 * 500 + 1500;
  ch3Val = ch3 * 500 + 1500;
  ch4Val = ch4 * 500 + 1500;

  //  Serial.print(analogRead(ch1Pin));
  //  Serial.print(" ");
  //  Serial.print(ch1Val);
  //  Serial.print(",    ");
  //  Serial.print(analogRead(ch2Pin));
  //  Serial.print(" ");
  //  Serial.print(ch2Val);
  //  Serial.print(",    ");
  //  Serial.print((ch3));
  //  Serial.print(" ");
  //  Serial.print(ch3Val);
  //  Serial.print(",    ");
  //  Serial.print((ch4));
  //  Serial.print(" ");
  //  Serial.print(ch4Val);
  //  Serial.println();

  EWD::runWifiCommunication();
  if (EWD::timedOut()) {
    digitalWrite(ledPin, LOW);
  }

  delay(10);
}

float InputReader_JoystickAxis(int reading, int neg, int cent, int pos, int deadzone) {
  deadzone = max(deadzone, 0);
  ////////// scale joystick values from adjustible range to -1 to 1
  float output = 0;
  if (neg < pos) {  //if the value for when the joystick is towards the left is less then the value for when the joystick is towards the right...
    if (reading < cent - deadzone && neg < cent - deadzone)
      output = floatMap(reading, neg, cent - deadzone, -1, 0);
    if (reading > cent + deadzone && pos > cent + deadzone)
      output = floatMap(reading, pos, cent + deadzone, 1, 0);
  }
  if (neg > pos) {  // joystick axis is backwards
    if (reading > cent + deadzone && neg > cent + deadzone)
      output = floatMap(reading, neg, cent + deadzone, -1, 0);
    if (reading < cent - deadzone && pos < cent - deadzone)
      output = floatMap(reading, pos, cent - deadzone, 1, 0);
  }
  return constrain(output, -1, 1);
}

float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
