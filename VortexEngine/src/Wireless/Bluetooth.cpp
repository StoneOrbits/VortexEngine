#include "Bluetooth.h"
#include <BLEUtils.h>
#include <BLE2902.h>
#include <stdarg.h>

#define SERVICE_UUID       "12345678-1234-1234-1234-123456789abc"
#define WRITE_CHAR_UUID    "12345678-1234-1234-1234-123456789abd"
#define NOTIFY_CHAR_UUID   "12345678-1234-1234-1234-123456789abe"

BLEServer* Bluetooth::pServer = nullptr;
BLECharacteristic* Bluetooth::writeChar = nullptr;
BLECharacteristic* Bluetooth::notifyChar = nullptr;
bool Bluetooth::m_bleConnected = false;
ByteStream Bluetooth::receivedData;

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) override {
        Bluetooth::m_bleConnected = true;
        //Serial.println("Bluetooth Device Connected.");
    }

    void onDisconnect(BLEServer* pServer) override {
        Bluetooth::m_bleConnected = false;
        //Serial.println("Bluetooth Device Disconnected.");
    }
};

class WriteCallback : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* characteristic) override {
        String value = characteristic->getValue();
        if (!value.isEmpty()) {
            for (char c : value) {
                Bluetooth::receivedData.serialize8(c); // Store in ByteStream
            }
            //Serial.print("Received from BLE: ");
            //Serial.println(value.c_str());
        }
    }
};

bool Bluetooth::init() {
    //Serial.println("Initializing BLE...");
    BLEDevice::init("ESP32-C3 BLE");
    
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService* pService = pServer->createService(SERVICE_UUID);

    writeChar = pService->createCharacteristic(
        WRITE_CHAR_UUID,
        BLECharacteristic::PROPERTY_WRITE
    );
    writeChar->setCallbacks(new WriteCallback());

    notifyChar = pService->createCharacteristic(
        NOTIFY_CHAR_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );
    notifyChar->addDescriptor(new BLE2902());

    pService->start();
    
    BLESecurity* pSecurity = new BLESecurity();
    pSecurity->setAuthenticationMode(ESP_LE_AUTH_NO_BOND);
    pSecurity->setCapability(ESP_IO_CAP_NONE);
    pSecurity->setInitEncryptionKey(ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->start();

    //Serial.println("BLE Server Ready.");
    return true;
}

void Bluetooth::cleanup() {
    if (pServer) {
        BLEDevice::deinit(true);
        //Serial.println("Bluetooth cleaned up.");
    }
}

bool Bluetooth::isConnected() {
    return m_bleConnected;
}

bool Bluetooth::checkBluetooth() {
    return isConnected();
}

void Bluetooth::write(const char* msg, ...) {
    if (!isConnected()) return;
    
    char buffer[512];
    va_list args;
    va_start(args, msg);
    vsnprintf(buffer, sizeof(buffer), msg, args);
    va_end(args);

    //Serial.print("Sending via BLE: ");
    //Serial.println(buffer);

    notifyChar->setValue(buffer);
    notifyChar->notify();
}

void Bluetooth::write(ByteStream& byteStream) {
    if (!isConnected()) return;

    uint32_t size = byteStream.rawSize();
    notifyChar->setValue((uint8_t*)byteStream.rawData(), size);
    notifyChar->notify();
}

void Bluetooth::read(ByteStream& byteStream) {
    if (receivedData.size() == 0) return;

    uint8_t byte;
    while (receivedData.unserialize8(&byte)) {
        byteStream.serialize8(byte);
    }
}

void Bluetooth::readAmount(uint32_t amount, ByteStream& byteStream) {
    if (receivedData.size() == 0) return;

    for (uint32_t i = 0; i < amount; i++) {
        uint8_t byte;
        if (!receivedData.consume8(&byte)) break;
        byteStream.serialize8(byte);
    }
}

bool Bluetooth::dataReady() {
    return receivedData.size() > 0;
}
