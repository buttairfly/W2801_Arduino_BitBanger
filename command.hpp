// command.hpp
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Arduino.h"
#include <Adafruit_WS2801.h>

#define INIT_LEN_CHAR 4
#define HAS_NUM_LEN_CHAR 4
#define HAS_NUM_SINGLE_COLOR 6

#define LATCH_TIMEOUT 10

#define INIT 'I'
#define SHADE 'S'
#define PIXEL 'P'
#define RAW_FRAME 'R'
#define LATCH_FRAME 'L'

class Command {
   public:
     Command(Adafruit_WS2801* _strip);
     void Init(const uint8_t c);
     void ProcessCommand(const uint8_t c);
     boolean IsInitialized(void);
   private:
     void latch(void);
     void reset(void);
     void initCommand(const uint8_t s);

     void processNumParam(const uint8_t c);
     void processColor(const uint8_t s);

     void processShade(const uint8_t s);
     void processPixel(const uint8_t c);
     void processRawFrame(const uint8_t c);

     uint32_t hex2color(uint32_t val, const uint8_t hex, const uint8_t pos);
     uint16_t hex2uint16(uint16_t val, const uint8_t hex, const uint8_t pos);
     uint8_t hex2uint8(uint8_t val, const uint8_t hex);

     boolean hasCommand;
     boolean hasNumParam;
     boolean initialized;
     uint8_t command;
     uint8_t paramPos;
     uint16_t ledPos;
     uint16_t numParam;
     uint32_t colorParam;
     unsigned long latchTime;

     Adafruit_WS2801 *strip;
};

#endif
