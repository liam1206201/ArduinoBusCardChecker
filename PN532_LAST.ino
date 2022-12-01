#include <Arduino.h>
#include "Adafruit_PN532.h"
#include <LiquidCrystal_I2C.h>
Adafruit_PN532 nfc(10);
LiquidCrystal_I2C lcd(0x27, 16, 2);
long timer=1800000;
int state=0; //0: 대기상태, 1: 카운트다운상태
long minute, second;
uint32_t money;

void setup() {
lcd.begin();
Serial.begin(115200);
delay(50);
pinMode(LED_BUILTIN, OUTPUT);
digitalWrite(LED_BUILTIN, HIGH); 
Serial.println("Start NFC");

nfc.begin();
uint32_t versiondata = nfc.getFirmwareVersion();
if (! versiondata) {
Serial.print("Didn't find PN53x board");
while (1);
}
nfc.SAMConfig();
Serial.println("Waiting for an ISO14443A Card ...");
delay(2000);
}

void loop() {

Serial.println(state);

if (state==0) {
  uint8_t success;
uint8_t responseLength = 64;
success = nfc.inListPassiveTarget();
if (success) {
Serial.println("Found something!");
uint8_t cardInfo[responseLength];
uint8_t cardNumSize = 0;
uint8_t selectApdu[] = { 0x00, 0xA4, 0x00, 0x00, 0x02, 0x42, 0x00 };
success = nfc.inDataExchange(selectApdu, sizeof(selectApdu), cardInfo, &responseLength);
if (success) {nfc.PrintHexChar(cardInfo, responseLength);
if (responseLength >= 24) {}}

uint8_t balance[responseLength];
uint8_t balanceApdu[] = { 0x90, 0x4C, 0x00, 0x00, 0x04 } ;
success = nfc.inDataExchange(balanceApdu, sizeof(balanceApdu), balance, &responseLength);
if (success) {
nfc.PrintHexChar(balance, responseLength);
if (responseLength >= 4) {
char fpsbuf[32] = "";
money = balance[0] * 256 * 256 * 256 +  balance[1] * 256 * 256 +  balance[2] * 256 + balance[3];
dtostrf((float)money, 10, 0,   fpsbuf);
delay(2000);
Serial.print("money : "); Serial.println(money);
state=1;
lcd.setCursor(2, 0);
lcd.print("Money :");
lcd.setCursor(10,0);
lcd.print(money);
}
}
}
}

else if (state==1) {
minute=timer/60000;
second=(timer-minute*60000)/1000;
lcd.setCursor(0,1); lcd.print("Left Time");
lcd.setCursor(10,1); lcd.print(minute);
lcd.setCursor(12,1); lcd.print(":");
lcd.setCursor(13,1); lcd.print(second);

delay(1000); timer=timer-1000;
if (timer<0) {state=0; lcd.clear();}
}
}
