#include "Arduino.h"
#include "LEGORemote.h"
using namespace std::placeholders;

const std::string LEGO_REMOTE_SERVICE_UUID = "00001623-1212-efde-1623-785feabcd123";
const std::string LEGO_REMOTE_CHARACTERISTICS_UUID = "00001624-1212-efde-1623-785feabcd123";

LEGORemote::LEGORemote() {
}

void LEGORemote::AddController(BLEAddress m_address, ControllerCallback m_callBack, byte color) {
  LegoControllerIdx deviceInfo = { m_callBack, color };
  m_Callbacks.insert({ m_address, deviceInfo });
}

void LEGORemote::Init() {
  BLEScan *pScan = BLEDevice::getScan();
  BLEScanResults results = pScan->start(2);

  BLEUUID LEGOServiceUuid(LEGO_REMOTE_SERVICE_UUID);

  for (int i = 0; i < results.getCount(); i++) {
    BLEAdvertisedDevice device = results.getDevice(i);
    if (device.isAdvertisingService(LEGOServiceUuid)) {
      log_i("found device: %s", device.toString().c_str());
      BLEClient *m_Client = BLEDevice::createClient();
      log_i("found LEGO Remote %s", device.getName().c_str());

      BLEAddress m_address = device.getAddress();

      if (m_Client->connect(&device)) {
        BLERemoteService *pService = m_Client->getService(LEGOServiceUuid);
        if (pService) {

          BLERemoteCharacteristic *m_Char = pService->getCharacteristic(LEGO_REMOTE_CHARACTERISTICS_UUID);
          if (m_Char && m_Char->canNotify() && m_Callbacks.count(m_address) > 0) {
            LegoControllerIdx deviceInfo = m_Callbacks[m_address];

            m_Char->registerForNotify(std::bind(&LEGORemote::Callback, this, _1, deviceInfo.callBack, _2, _3, _4));
            byte activatePortDeviceMessage[8] = { 0x41, LEGORemote::LEFT, 0x0, 0x01, 0x00, 0x00, 0x00, 0x01 };
            WriteValue(m_Char, activatePortDeviceMessage, 8);
            activatePortDeviceMessage[1] = LEGORemote::RIGHT;
            WriteValue(m_Char, activatePortDeviceMessage, 8);

            byte setColorMode[8] = { 0x41, LEGORemote::COLOR, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00 };
            WriteValue(m_Char, setColorMode, 8);
            byte setColor[6] = { 0x81, LEGORemote::COLOR, 0x11, 0x51, 0x00, deviceInfo.color };
            WriteValue(m_Char, setColor, 6);
          }
        }
      }
    }
  }
}

void LEGORemote::Callback(BLERemoteCharacteristic *pBLERemoteCharacteristic, ControllerCallback m_callBack, uint8_t *pData, size_t length, bool isNotify) {

  if (pData[2] != PORT_VALUE_SINGLE) {
    return;
  }

  int button = pData[3];
  int state = pData[4];

  m_callBack(button, state);
}


void LEGORemote::WriteValue(BLERemoteCharacteristic *m_Char, byte command[], int size) {
  if (m_Char) {
    byte cmd[size + 2] = { (byte)(size + 2), 0x00 };
    memcpy(cmd + 2, command, size);
    m_Char->writeValue(cmd, sizeof(cmd), false);
  }
}