#ifndef CMMC_Packet_H
#define CMMC_Packet_H

#include <Arduino.h>

#ifndef DEBUG_BUFFER_SIZE
  #define DEBUG_BUFFER_SIZE 200
#endif

#ifndef HEX_STRING_BUFFER_SIZE 
  #define HEX_STRING_BUFFER_SIZE 1025
#endif


#define USER_DEBUG_PRINTF(fmt, args...) { \
    sprintf(this->debug_buffer, fmt, ## args); \
    _user_debug_cb(this->debug_buffer); \
  }

typedef struct __attribute((__packed__)) {
  uint8_t from[6];
  uint8_t to[6];
  uint8_t type = 0;
  uint32_t battery = 0x00;
  uint32_t field1 = 0x00; //distance cm
  uint32_t field2 = 0x00; // retries
  uint32_t field3 = 0x00;
  uint32_t field4 = 0x00;
  uint32_t field5 = 0x00; // controller working time
  uint32_t field6 = 0x00; // DEFAULT_SLEEP_TIME_M
  uint8_t nameLen = 0x00;
  char sensorName[16];
  uint32_t ms = 0;
  uint32_t sent_ms = 0;
  uint32_t sum = 0;
} CMMC_SENSOR_T;

typedef struct __attribute((__packed__)) {
  uint8_t header[2] = {0x00, 0x00};
  uint8_t version = 1;
  uint8_t project = 1;
  uint8_t reserved[4] = {0xff, 0xff, 0xff, 0xff};
  uint8_t nameLen = 0x00;
  char myName[16];
  uint32_t sleepTime;
  uint32_t ms;
  CMMC_SENSOR_T data;
  uint32_t sum;
  uint8_t tail[2] = {0x00, 0x00};
} CMMC_PACKET_T;

#ifndef CMMC_NO_ALIAS
#define CMMC_Packet CMMC_Packet
#endif

// #ifdef ESP8266
//   extern "C" {
//     #include "user_interface.h"
//   }
//   #include "ESP8266WiFi.h"
//   #include <functional>
// #endif

typedef void (*cmmc_debug_cb_t)(const char* message);

class CMMC_Packet
{
  public:
    // constructor
    CMMC_Packet(uint8_t project, uint8_t version, uint8_t header[2], uint8_t footer[2]);
    ~CMMC_Packet(); 

    void init();
    const char* getHexString(); 
    void toHexString(const u8 array[], size_t len, char buffer[]); 
    const CMMC_PACKET_T* getPacketPtr(); 
    uint32_t checksum(uint8_t* data, size_t len); 
    void dump(); 
    void updatePacketSum(); 
    void dump(const u8* data, size_t size);

    bool setSensorName(const char name[16]);
    bool setName(const char name[16]);
    void debug(cmmc_debug_cb_t);
  private:
    char debug_buffer[DEBUG_BUFFER_SIZE];
    CMMC_SENSOR_T *_sensorPtr;
    CMMC_PACKET_T _packet;
    cmmc_debug_cb_t _user_debug_cb;
    char _hexStringBuffer[HEX_STRING_BUFFER_SIZE];
};

#endif //CMMC_Packet_H
