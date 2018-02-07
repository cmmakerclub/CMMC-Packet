#ifndef CMMC_Packet_H
#define CMMC_Packet_H

#include <Arduino.h>

#define DEBUG_BUFFER_SIZE 500
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
  uint8_t reserved[4]= {0xff,0xff,0xff,0xff};
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
      CMMC_Packet(uint8_t project, uint8_t version, uint8_t header[2], uint8_t footer[2]) { 
        this->_packet.header[0] = header[0];
        this->_packet.header[1] = header[1];
        this->_packet.tail[0] = footer[0];
        this->_packet.tail[1] = footer[1];

        this->_packet.ms = millis();
        // auto blank = [](const char* message) {}; 
      } 
      ~CMMC_Packet() {} 
      
      void init();
      const CMMC_PACKET_T* getPacketPtr() {
        return &this->_packet;
      }
      uint32_t checksum(uint8_t* data, size_t len) {
        uint32_t sum = 0;
          while(len--) {
            sum += *(data++);
          }
          return sum;
        } 

      void dump() {
        this->dump((u8*)&this->_packet, sizeof(this->_packet));
      }

      void dump(const u8* data, size_t size) {
          for (size_t i = 0; i < size; i++) {
            USER_DEBUG_PRINTF("%02x ", data[i]);
          }
          USER_DEBUG_PRINTF("\n");
      }

      bool setSensorName(const char name[16]) {
        int len = strlen(name);
        if (len > 15) {
          return false; 
        }
        else {
          strcpy(this->_packet.data.sensorName, name);
          this->_packet.data.nameLen = strlen(name); 
        } 
        return true;
      }

      bool setName(const char name[16]) {
        int len = strlen(name);
        if (len > 15) {
          return false; 
        }
        else {
          strcpy(this->_packet.myName, name);
          this->_packet.nameLen = strlen(name); 
        } 
        return true;
      }
      void debug(cmmc_debug_cb_t); 
    private:
      char debug_buffer[DEBUG_BUFFER_SIZE];
      CMMC_SENSOR_T *_sensorPtr;
      CMMC_PACKET_T _packet;
      cmmc_debug_cb_t _user_debug_cb;
};

#endif //CMMC_Packet_H
