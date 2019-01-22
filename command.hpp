// railsensor.hpp
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Arduino.h"
#include <Adafruit_WS2801.h>

#define INIT_LEN_CHAR 4
#define HAS_NUM_LEN_CHAR 4

#define INIT 'I'
#define PIXEL 'P'
#define FRAME 'F'
#define SHOW 'S'

class Command {
   public:
     Command(Adafruit_WS2801* _strip);
     void Init(const uint8_t c);
     void ProcessCommand(const uint8_t c);
     boolean IsInitialized(void);
   private:
     void reset(void);
     void processNumParam(const uint8_t c);
     void processPixel(const uint8_t c);
     void processFrame(const uint8_t c);

     uint16_t hex2uint16(uint16_t val, uint8_t hex, uint32_t pos);
     uint8_t hex2uint8(uint8_t val, uint8_t hex);

     boolean hasCommand;
     boolean hasNumParam;
     boolean initialized;
     uint8_t command;
     uint32_t commandPos;
     uint16_t numParam;

     Adafruit_WS2801 *strip;
};

#endif
