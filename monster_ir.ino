#include <WiFi.h>
#include <ArduinoOTA.h>
#include <IRremote.hpp>

#include "secrets.h"

decode_results results;

volatile int phase = 0;
int direction = 0;


void setup() {
  Serial.begin(115200);
  IrReceiver.begin(D2, ENABLE_LED_FEEDBACK);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  // put your setup code here, to run once:
  pinMode(D4, OUTPUT);
  pinMode(D5, OUTPUT);
  pinMode(D7, INPUT);

  pinMode(LED_BUILTIN, OUTPUT);


  // phase = digitalRead(D1);

  digitalWrite(D4, HIGH);
  digitalWrite(D5, HIGH);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.setHostname("speaker");
  ArduinoOTA.begin();

  digitalWrite(LED_BUILTIN, HIGH);

  phase = digitalRead(D7);
  Serial.printf("Startup. Phase=%d\n", phase);
}

void loop() {
  // put your main code here, to run repeatedly:
  ArduinoOTA.handle();

  if (IrReceiver.decode()) {
    // Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); // Print "old" raw data
    // USE NEW 3.x FUNCTIONS
    IrReceiver.printIRResultShort(&Serial);  // Print complete received data in one line
    IrReceiver.printIRSendUsage(&Serial);    // Print the statement required to send this data

    if (IrReceiver.decodedIRData.command == 0x1A) {
      //up
      if (phase == 0) {
        digitalWrite(D5, HIGH);
        delay(20);
        digitalWrite(D4, HIGH);
        delay(20);
      } else {
        digitalWrite(D5, LOW);
        delay(20);
        digitalWrite(D4, LOW);
        delay(20);
      }
    } else if (IrReceiver.decodedIRData.command == 0x1B) {
      // down
      if (phase == 0) {
        digitalWrite(D4, HIGH);
        delay(20);
        digitalWrite(D5, HIGH);
        delay(20);
      } else {
        digitalWrite(D4, LOW);
        delay(20);
        digitalWrite(D5, LOW);
        delay(20);
      }
    }

    IrReceiver.resume();  // Enable receiving of the next value
  }

  direction = Serial.read();

  if (direction == 'u') {
    Serial.println("up");

    if (phase == 0) {
      digitalWrite(D4, HIGH);
      delay(20);
      digitalWrite(D5, HIGH);
      delay(20);
    } else {
      digitalWrite(D4, LOW);
      delay(20);
      digitalWrite(D5, LOW);
      delay(20);
    }
    // D1 high
    // wait 20ms
    // D2 high
    // invert
  } else if (direction == 'd') {
    Serial.println("down");
    if (phase == 0) {
      digitalWrite(D4, HIGH);
      delay(20);
      digitalWrite(D5, HIGH);
      delay(20);
    } else {
      digitalWrite(D4, LOW);
      delay(20);
      digitalWrite(D5, LOW);
      delay(20);
    }
  } else if (direction == 't') {
  }

  int p = digitalRead(D7);
  if (p != phase) {
    Serial.printf("Phase change. Phase=%d\n", p);
    phase = p;
  }
}
