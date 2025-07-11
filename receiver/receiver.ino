#include <Arduino.h> // tested with esp32 boards version 3.2.1
#include <ESP32_easy_wifi_data.h> // tested with version 1.4.3
/*
   Creates wifi network
   Outputs 4 servo signals
*/

const byte ch1Pin = 32; // turn_rc_pin
const byte ch2Pin = 33; // speed_rc_pin
const byte ch3Pin = 25; // ctrl_rc_pin
const byte ch4Pin = 26; // stop_rc_pin

int ch1Val;
int ch2Val;
int ch3Val;
int ch4Val;

const int ledPin = 2;

// data receive callback
void WifiDataToParse()
{
  ch1Val = EWD::recvIn();
  ch2Val = EWD::recvIn();
  ch3Val = EWD::recvIn();
  ch4Val = EWD::recvIn();
}
// data send callback
void WifiDataToSend()
{
  EWD::sendBl(digitalRead(0)); // I think it needs something to be sent
}

void configWifi()
{
  EWD::mode = EWD::Mode::createAP;
  EWD::APPort = 25001;
  EWD::APName = "gbgCarDemo";
  EWD::APPassword = "2521sert"; // no one will ever guess this
}

void setup()
{
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200);
  configWifi();
  EWD::setupWifi(WifiDataToParse, WifiDataToSend);
  Serial.println("receiver running");

  ledcAttach(ch1Pin, 50, 14);
  ledcAttach(ch2Pin, 50, 14);
  ledcAttach(ch3Pin, 50, 14);
  ledcAttach(ch4Pin, 50, 14);
}
void loop()
{
  EWD::runWifiCommunication();
  if (EWD::newData()) {
    digitalWrite(ledPin, HIGH);

    //    Serial.print(ch1Val);
    //    Serial.print(", ");
    //    Serial.print(ch2Val);
    //    Serial.print(", ");
    //    Serial.print(ch3Val);
    //    Serial.print(", ");
    //    Serial.print(ch4Val);
    //    Serial.println();

    servoWriteMicroseconds(ch1Pin, ch1Val);
    servoWriteMicroseconds(ch2Pin, ch2Val);
    servoWriteMicroseconds(ch3Pin, ch3Val);
    servoWriteMicroseconds(ch4Pin, ch4Val);
  }
  if (EWD::timedOut()) { // connection lost
    digitalWrite(ledPin, LOW);
    // set pins low to indicate no connection
    ledcWrite(ch1Pin, 0);
    ledcWrite(ch2Pin, 0);
    ledcWrite(ch3Pin, 0);
    ledcWrite(ch4Pin, 0);
  }
  delay(10);
}

bool servoWriteMicroseconds(uint8_t pin, int ms) {
  ms = constrain(ms, 1000, 2000);
  return ledcWrite(pin, ms * 512 / 624);
}
