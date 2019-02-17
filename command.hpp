// command.hpp
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Arduino.h"
#include <Adafruit_WS2801.h>
#include "version.hpp"

#define NUM_PARAM_CHARS      4
#define HAS_NUM_SINGLE_COLOR 6

#define LATCH_TIMEOUT  10

#define TYPE_UNDEFINED 0x01
#define TYPE_COMMAND   0x01
#define TYPE_HEX       0x02
#define TYPE_RETURN    0x03
#define TYPE_UNKNOWN   0xFF

#define VERSION       'V'
#define INIT          'I'
#define SHADE         'S'
#define PIXEL         'P'
#define RAW_FRAME     'R'
#define LATCH_FRAME   'L'

class Command {
   public:
              Command(Adafruit_WS2801* _strip);
     void     ProcessCommand(const uint8_t c);
     boolean  IsInitialized(void);

   private:
     void     printVersion(void);
     void     init(void);
     void     latch(void);
     void     reset(void);
     void     initCommand(const uint8_t s);
     uint8_t  getCharType(const uint8_t s);

     void     processNumParam(const uint8_t c);
     void     processColor(const uint8_t s);

     void     processShade(const uint8_t s);
     void     processPixel(const uint8_t c);
     void     processRawFrame(const uint8_t c);

     uint32_t hex2color(uint32_t val, const uint8_t hex, const uint8_t pos);
     uint16_t hex2uint16(uint16_t val, const uint8_t hex, const uint8_t pos);
     uint8_t  hex2uint8(uint8_t val, const uint8_t hex);

     uint8_t  getHexVal(const uint8_t hex);
     void     setCharType(const uint8_t s);
     boolean  isReturnCharType(void);

     boolean         hasCommand;
     boolean         hasNumParam;
     boolean         initialized;
     uint8_t         charType;
     uint8_t         command;
     uint8_t         paramPos;
     uint16_t        ledPos;
     uint16_t        numParam;
     uint32_t        colorParam;
     unsigned long   latchTime;

     Adafruit_WS2801 *strip;
};

#endif
