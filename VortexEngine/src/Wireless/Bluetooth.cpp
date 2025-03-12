#include "Bluetooth.h"
#include <stdarg.h>

#include "../Time/TimeControl.h"

#define SERVICE_UUID       "12345678-1234-1234-1234-123456789abc"
#define WRITE_CHAR_UUID    "12345678-1234-1234-1234-123456789abd"
#define NOTIFY_CHAR_UUID   "12345678-1234-1234-1234-123456789abe"

// Static members
bool Bluetooth::m_bleConnected = false;
ByteStream Bluetooth::receivedData;

// We'll track whether an indication is currently in progress:
#if BLUETOOTH_ENABLE == 1
static volatile bool m_isIndicationInProgress = false;

// Forward-declare the BLE objects
BLEServer *Bluetooth::pServer = nullptr;
BLECharacteristic *Bluetooth::writeChar = nullptr;
BLECharacteristic *Bluetooth::notifyChar = nullptr;

// Custom callbacks for the BLE server connection/disconnection.
class BluetoothCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer) override
  {
    Bluetooth::m_bleConnected = true;
  }

  void onDisconnect(BLEServer *pServer) override
  {
    Bluetooth::m_bleConnected = false;
  }
};

// onWrite callback for the Write characteristic:
// store incoming bytes (including nulls) into ByteStream.
class WriteCallback : public BLECharacteristicCallbacks
{
  void onWrite(BLECharacteristic *characteristic) override
  {
    size_t length = characteristic->getLength();
    const uint8_t* data = characteristic->getData();

    for (size_t i = 0; i < length; i++) {
      Bluetooth::receivedData.serialize8(data[i]);
    }
  }
};

// onStatus callback to detect when an indication is confirmed
// by the client. Once we see SUCCESS_INDICATE, we know the
// client has received the data.
class IndicationConfirmCallbacks : public BLECharacteristicCallbacks
{
  void onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code) override
  {
    // Check if the status indicates a successful Indicate
    if (s == BLECharacteristicCallbacks::Status::SUCCESS_INDICATE) {
      // Mark that the indication has been acknowledged
      m_isIndicationInProgress = false;
    }
  }
};
#endif // VORTEX_EMBEDDED

bool Bluetooth::init()
{
#if BLUETOOTH_ENABLE == 1
  BLEDevice::init(BLUETOOTH_BROADCAST_NAME);
  // Request a higher MTU, but actual negotiation may differ
  BLEDevice::setMTU(1024);

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new BluetoothCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  writeChar = pService->createCharacteristic(
    WRITE_CHAR_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );
  writeChar->setCallbacks(new WriteCallback());

  // Indication characteristic
  notifyChar = pService->createCharacteristic(
    NOTIFY_CHAR_UUID,
    BLECharacteristic::PROPERTY_INDICATE
  );

  // Attach our IndicationConfirmCallbacks so we know when an Indicate is done
  notifyChar->setCallbacks(new IndicationConfirmCallbacks());

  // 0x2902 descriptor: enable Indications
  BLE2902 *desc = new BLE2902();
  desc->setNotifications(false);
  desc->setIndications(true);
  notifyChar->addDescriptor(desc);

  pService->start();

  BLESecurity *pSecurity = new BLESecurity();
  pSecurity->setAuthenticationMode(ESP_LE_AUTH_NO_BOND);
  pSecurity->setCapability(ESP_IO_CAP_NONE);
  pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->start();
#endif
  return true;
}

void Bluetooth::cleanup()
{
#if BLUETOOTH_ENABLE == 1
  if (pServer != nullptr) {
    BLEDevice::deinit(true);
    pServer = nullptr;
    writeChar = nullptr;
    notifyChar = nullptr;
    m_bleConnected = false;
  }
#endif
}

bool Bluetooth::isInitialized()
{
#if BLUETOOTH_ENABLE == 1
  if (pServer != nullptr) {
    return true;
  }
#endif
  return false;
}

bool Bluetooth::isConnected()
{
  return m_bleConnected;
}

bool Bluetooth::checkBluetooth()
{
  return isConnected();
}

// Write a formatted string with Indicate, waiting for confirmation.
void Bluetooth::write(const char *msg, ...)
{
#if BLUETOOTH_ENABLE == 1
  if (!isConnected()) return;

  char buffer[512];
  va_list args;
  va_start(args, msg);
  int len = vsnprintf(buffer, sizeof(buffer), msg, args);
  va_end(args);

  if (len < 0 || len >= (int)sizeof(buffer)) {
    // If there's an error or overflow, bail out
    return;
  }

  while (m_isIndicationInProgress) {
    Time::delayMilliseconds(1);
  }
  // Mark an indication as in progress
  m_isIndicationInProgress = true;
  // Set the characteristic's value and indicate
  notifyChar->setValue((uint8_t*)buffer, len);
  notifyChar->indicate();
#endif
}

// Write a ByteStream with Indicate, waiting for confirmation.
void Bluetooth::write(ByteStream &byteStream)
{
#if BLUETOOTH_ENABLE == 1
  if (!isConnected()) return;

  byteStream.recalcCRC();
  uint32_t size = byteStream.rawSize();

  // We'll prepend 'size' as a 4-byte header
  uint8_t *buf = new uint8_t[size + sizeof(size)];
  if (!buf) {
    return;
  }
  memcpy(buf, &size, sizeof(size));
  memcpy(buf + sizeof(size), byteStream.rawData(), size);

  while (m_isIndicationInProgress) {
    Time::delayMilliseconds(1);
  }
  // Mark an indication as in progress
  m_isIndicationInProgress = true;
  // Set the characteristic's value and indicate
  notifyChar->setValue(buf, size + sizeof(size));
  notifyChar->indicate();
  delete[] buf;
#endif
}

// Read any received bytes and place them into 'byteStream'.
void Bluetooth::read(ByteStream &byteStream)
{
  if (receivedData.size() == 0) return;

  uint8_t byte;
  while (receivedData.consume8(&byte)) {
    byteStream.serialize8(byte);
  }
}

// Read a specific 'amount' of bytes into 'byteStream' if available.
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

// Check if we have data ready to be consumed.
bool Bluetooth::dataReady()
{
  return receivedData.size() > 0;
}
