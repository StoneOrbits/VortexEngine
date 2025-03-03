#include "Bluetooth.h"
#include <stdarg.h>

#include "../Time/TimeControl.h"

#if VORTEX_EMBEDDED == 1
#include <BLEUtils.h>
#include <BLE2902.h>
#endif

#define SERVICE_UUID       "12345678-1234-1234-1234-123456789abc"
#define WRITE_CHAR_UUID    "12345678-1234-1234-1234-123456789abd"
#define NOTIFY_CHAR_UUID   "12345678-1234-1234-1234-123456789abe"

bool Bluetooth::m_bleConnected = false;
ByteStream Bluetooth::receivedData;

#if VORTEX_EMBEDDED == 1
BLEServer *Bluetooth::pServer = nullptr;
BLECharacteristic *Bluetooth::writeChar = nullptr;
BLECharacteristic *Bluetooth::notifyChar = nullptr;

class BluetoothCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer) override { Bluetooth::m_bleConnected = true; }
  void onDisconnect(BLEServer *pServer) override { Bluetooth::m_bleConnected = false; }
};

class WriteCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *characteristic) override
  {
    String value = characteristic->getValue();
    for (char c : value) {
      Bluetooth::receivedData.serialize8(c);
    }
  }
};
#endif

bool Bluetooth::init()
{
#if VORTEX_EMBEDDED == 1
  BLEDevice::init("ESP32-C3 BLE");
  BLEDevice::setMTU(512);

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BluetoothCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  writeChar = pService->createCharacteristic(
    WRITE_CHAR_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  writeChar->setCallbacks(new WriteCallback());

  notifyChar = pService->createCharacteristic(
    NOTIFY_CHAR_UUID,
    BLECharacteristic::PROPERTY_NOTIFY
  );

  BLE2902 *desc = new BLE2902();
  desc->setNotifications(true);
  desc->setIndications(true);
  //desc->setValue("10"); // Send notifications **every 10ms**
  notifyChar->addDescriptor(desc);

  pService->start();

  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_NO_BOND);
  pSecurity->setCapability(ESP_IO_CAP_NONE);
  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  // for stable connection
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();

#endif
  return true;
}

void Bluetooth::cleanup()
{
#if VORTEX_EMBEDDED == 1
  if (pServer) {
    BLEDevice::deinit(true);
  }
#endif
}

bool Bluetooth::isConnected()
{
  return m_bleConnected;
}

bool Bluetooth::checkBluetooth()
{
  return isConnected();
}

void Bluetooth::write(const char *msg, ...)
{
#if VORTEX_EMBEDDED == 1
  if (!isConnected()) return;

  char buffer[512];
  va_list args;
  va_start(args, msg);
  int len = vsnprintf(buffer, sizeof(buffer), msg, args);
  va_end(args);

  //Serial.print("Sending via BLE: ");
  //Serial.println(buffer);
  // Ensure message is within bounds
  if (len < 0 || len >= sizeof(buffer)) {
    // abort?
    return;
  }
  notifyChar->setValue(buffer);
  notifyChar->notify();
#endif
}

void Bluetooth::write(ByteStream &byteStream)
{
#if VORTEX_EMBEDDED == 1
  if (!isConnected()) return;

  byteStream.recalcCRC();
  ByteStream buf(byteStream.rawSize() + sizeof(uint32_t));
  buf.serialize32(byteStream.rawSize());
  ByteStream secondary(byteStream.rawSize(), (uint8_t *)byteStream.rawData());
  buf.append(secondary);
  notifyChar->setValue((uint8_t *)buf.data(), buf.size());
  notifyChar->notify();
#endif
}

void Bluetooth::read(ByteStream &byteStream)
{
  if (receivedData.size() == 0) return;

  uint8_t byte;
  while (receivedData.consume8(&byte)) {
    byteStream.serialize8(byte);
  }
}

void Bluetooth::readAmount(uint32_t amount, ByteStream &byteStream)
{
  if (receivedData.size() == 0) return;

  for (uint32_t i = 0; i < amount; i++) {
    uint8_t byte;
    if (!receivedData.consume8(&byte)) {
      break;
    }
    byteStream.serialize8(byte);
  }
}

bool Bluetooth::dataReady()
{
  return receivedData.size() > 0;
}
