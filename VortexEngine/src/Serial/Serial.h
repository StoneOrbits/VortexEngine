#ifndef SERIAL_H
#define SERIAL_H

#include <inttypes.h>
#include <stdbool.h>

typedef struct ByteStream ByteStream;

bool SerialComs_init();
void SerialComs_cleanup();
bool SerialComs_isConnected();
bool SerialComs_checkSerial();
void SerialComs_write(const char *msg, ...);
void SerialComs_writeStream(ByteStream *byteStream);
void SerialComs_read(ByteStream *byteStream);
bool SerialComs_dataReady();

#endif
