#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <analogWrite.h>
#include <L298N.h>

#include <Arduino_GFX_Library.h>

#include "BLEAddress.h"
#include "BLEDevice.h"
#include "LEGORemote.h"

const unsigned int MAX_SPEED = 255;
const unsigned int MIN_SPEED = 160;

// Pin definition
const unsigned int A_IN1 = 12;
const unsigned int A_IN2 = 14;
const unsigned int A_EN = 13;
const unsigned int B_IN1 = 27;
const unsigned int B_IN2 = 26;
const unsigned int B_EN = 25;

// Create one motor instance
L298N motor_a(A_EN, A_IN1, A_IN2);
L298N motor_b(B_EN, B_IN1, B_IN2);

Arduino_DataBus *bus = new Arduino_ESP32SPI(27 /* DC */, 5 /* CS */, 18 /* SCK */, 23 /* MOSI */, GFX_NOT_DEFINED /* MISO */);
Arduino_GFX *gfx = new Arduino_SSD1283A(bus, 2 /* RST */, 0 /* rotation */);

unsigned short resolveSpeed(int currentSpeed, int button, int state) {
  switch (state) {
    case (LEGORemote::UP):

      if (currentSpeed < MIN_SPEED) {
        currentSpeed = MIN_SPEED;
      }

      currentSpeed += 10;

      if (currentSpeed > MAX_SPEED) {
        currentSpeed = MAX_SPEED;
      }
      break;
    case (LEGORemote::DOWN):
      if (currentSpeed < MIN_SPEED) {
        currentSpeed = MIN_SPEED;
      }

      currentSpeed -= 10;

      if (currentSpeed < MIN_SPEED) {
        currentSpeed = MIN_SPEED;
      }
      break;
    case (LEGORemote::STOP):
      currentSpeed = 0;
      break;
  }

  return (unsigned short)currentSpeed;
}

void motorA(int button, int state) {
  int currentSpeed = motor_a.getSpeed();

  motor_a.setSpeed(resolveSpeed(currentSpeed, button, state));
}

void motorB(int button, int state) {
  int currentSpeed = motor_b.getSpeed();

  motor_b.setSpeed(resolveSpeed(currentSpeed, button, state));
}


LEGORemote myLEGORemote;

void setup() {
  // put your setup code here, to run once:
  // Used to display information
  Serial.begin(9600);

  // Wait for Serial Monitor to be opened
  while (!Serial) {
    //do nothing
  }

  // Set initial speed
  motor_a.setSpeed(MAX_SPEED);
  motor_b.setSpeed(MAX_SPEED);

  // Tell the motor to go forward (may depend by your wiring)
  motor_a.forward();
  motor_b.forward();

  BLEDevice::init("");

  BLEAddress controlA = BLEAddress("fc:a8:9b:56:e5:dc");
  BLEAddress controlB = BLEAddress("b0:10:a0:69:c5:50");
  
  myLEGORemote.AddController(BLEAddress("fc:a8:9b:56:e5:dc"), motorA, 0x04);
  myLEGORemote.AddController(BLEAddress("b0:10:a0:69:c5:50"), motorB, 0x02);
  myLEGORemote.Init();
  
  gfx->begin();
  gfx->fillScreen(WHITE);
  gfx->fillRect(0, 0, 130, 64, BLUE);
  gfx->drawLine(0, 65, 130, 65, BLACK);
  gfx->fillRect(0, 66, 130, 130, RED);
  gfx->setTextSize(3);
}

void loop() {
  delay(3000);
  motor_a.setSpeed(motor_a.getSpeed() - 10);
  gfx->fillRect(0, 0, 130, 64, BLUE);
  gfx->setCursor(10, 10);
  gfx->setTextColor(RED);
  gfx->printf("%d", motor_a.getSpeed());

  gfx->fillRect(0, 66, 130, 130, RED);
  gfx->setCursor(10, 75);
  gfx->setTextColor(BLUE);
  gfx->printf("%d", motor_b.getSpeed());

  //delay(500);
}
