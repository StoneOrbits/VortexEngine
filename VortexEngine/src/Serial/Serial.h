#ifndef SERIAL_H
#define SERIAL_H

#include <stdbool.h>
#include <inttypes.h>

typedef struct ByteStream ByteStream;

// init serial
bool SerialComs_init();
void SerialComs_cleanup();

// whether serial is initialized
bool SerialComs_isConnected();

// check for any serial connection or messages
bool SerialComs_checkSerial();

// write a formatted message to serial
void SerialComs_write(const char *msg, ...);

// write raw data
void SerialComs_writeStream(ByteStream *byteStream);

// read a message from serial
void SerialComs_read(ByteStream *byteStream);

// data in the socket ready to read
bool SerialComs_dataReady();

#endif