#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "../VortexConfig.h"

#if VORTEX_EMBEDDED == 1
#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLE2902.h>
#endif

#include "../Serial/ByteStream.h"

class Bluetooth {
public:
    static bool init();
    static void cleanup();

    static bool isConnected();
    static bool checkBluetooth();

    static void write(const char* msg, ...);
    static void write(ByteStream& byteStream);

    static void read(ByteStream& byteStream);
    static void readAmount(uint32_t amount, ByteStream& byteStream);

    static bool dataReady();

#if VORTEX_EMBEDDED == 1
    static BLEServer* pServer;
    static BLECharacteristic* writeChar;
    static BLECharacteristic* notifyChar;
#endif
    static bool m_bleConnected;
    static ByteStream receivedData;
};

#endif
