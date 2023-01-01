#ifndef LEGO_REMOTE_H_INCLUDED
#define LEGO_REMOTE_H_INCLUDED

#include "BLEAddress.h"
#include "BLEDevice.h"

typedef std::function<void(int, int)> ControllerCallback;

struct LegoControllerIdx {
  ControllerCallback callBack;
  byte color;
};

/*
enum ControlerColor
{
  "BLACK" = 0,
  "PINK" = 1,
  "PURPLE" = 2,
  "BLUE" = 3,
  "LIGHTBLUE" = 4,
  "CYAN" = 5,
  "GREEN" = 6,
  "YELLOW" = 7,
  "ORANGE" = 8,
  "RED" = 9,
  "WHITE" = 10,
  "NONE" = 0xff
};
*/

class LEGORemote {
public:
  LEGORemote();
  virtual ~LEGORemote() {}

  void Init();
  void AddController(BLEAddress m_address, ControllerCallback m_callBack, byte color);

  enum {
    LEFT = 0x00,
    RIGHT = 0x01,
    PORT_VALUE_SINGLE = 0x45,
    PRESSED = 0x01,
    RELEASED = 0x00,
    UP = 0x01,
    DOWN = 0xff,
    STOP = 0x7f,
    COLOR = 0x34
  };

protected:
  std::map<BLEAddress, LegoControllerIdx> m_Callbacks;

  void Callback(BLERemoteCharacteristic* pBLERemoteCharacteristic, ControllerCallback m_callBack, uint8_t* pData, size_t length, bool isNotify);
  void WriteValue(BLERemoteCharacteristic* m_Char, byte command[], int size);
};


#endif  // LEGO_REMOTE_H_INCLUDED