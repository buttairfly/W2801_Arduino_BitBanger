// command.hpp
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Arduino.h"
#include <Adafruit_WS2801.h>
#include "version.hpp"
#include "device.serial.arduino.error.hpp"

#define NUM_PARAM_CHARS      4
#define HAS_NUM_SINGLE_COLOR 6

#define LATCH_TIMEOUT  10
#define PARITY_SEED    0xa5

#define TYPE_UNDEFINED 'D'
#define TYPE_COMMAND   'C'
#define TYPE_HEX       'H'
#define TYPE_RETURN    'R'
#define TYPE_UNKNOWN   'U'

#define VERSION       'V'
#define INIT          'I'
#define SHADE         'S'
#define PIXEL         'P'
#define RAW_FRAME     'R'
#define LATCH_FRAME   'L'
#define QUIET_MODE    'Q'

class Command {
   public:
              Command(Adafruit_WS2801* _strip);
     void     ProcessCommand(const uint8_t c);
     boolean  IsInitialized(void);

   private:
     void     reset(void);
     void     printErrorAndReset(const String errorCode, const uint8_t inChar, const uint32_t param = 0xFFFFFFFF);
     void     printVersion(const uint8_t s);
     void     init(const uint8_t s);
     void     latch(const uint8_t s);
     void     initCommand(const uint8_t s);
     void     quiet(const uint8_t s);

     void     calcParity(const uint8_t s);
     boolean  checkParity();

     void     processNumParam(const uint8_t c);
     void     processColor(const uint8_t s);

     void     processShade(const uint8_t s);
     void     processPixel(const uint8_t c);
     void     processRawFrame(const uint8_t c);

     uint32_t hex2color(uint32_t val, const uint8_t hex, const uint8_t pos);
     uint16_t hex2uint16(uint16_t val, const uint8_t hex, const uint8_t pos);
     uint8_t  hex2uint8(uint8_t val, const uint8_t hex);

     uint8_t  getCharType(const uint8_t s);
     uint8_t  getHexVal(const uint8_t hex);
     void     setCharType(const uint8_t s);
     boolean  isReturnCharType(const uint8_t s);

     boolean         hasCommand;
     boolean         hasNumParam;
     boolean         initialized;
     boolean         quietMode;
     uint8_t         charType;
     uint8_t         command;
     uint8_t         paramPos;
     uint8_t         parity;
     uint8_t         lastChar;
     uint16_t        ledPos;
     uint16_t        numParam;
     uint32_t        colorParam;
     unsigned long   latchTime;

     Adafruit_WS2801 *strip;
};

#endif
