// command.hpp
#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <Adafruit_WS2801.h>

#include "Arduino.h"
#include "device.ledpanel.arduino.error.hpp"
#include "version.hpp"

const uint8_t NUM_PARAM_CHARS = 4;
const uint8_t HAS_NUM_SINGLE_COLOR = 6;

const uint8_t LATCH_TIMEOUT = 10;
const uint8_t PARITY_SEED = 0xa5;

const uint8_t TYPE_UNDEFINED = 'D';
const uint8_t TYPE_COMMAND = 'C';
const uint8_t TYPE_HEX = 'H';
const uint8_t TYPE_RETURN = 'R';
const uint8_t TYPE_UNKNOWN = 'U';

const uint8_t VERSION = 'V';
const uint8_t LATCH_FRAME = 'L';
const uint8_t INIT = 'I';
const uint8_t QUIET_MODE = 'Q';
const uint8_t SHADE = 'S';
const uint8_t PIXEL = 'P';
const uint8_t RAW_FRAME = 'W';

/*
const String CMD_TYPE_VERSION = "CHR";
const String CMD_TYPE_LATCH = "CHR";
const String CMD_TYPE_INIT = "CHHHHHR";
const String CMD_TYPE_QUIET_MODE = "CHHHHHR";
const String CMD_TYPE_RAW_FRAME = "RHHHHHHHHHHHHHHHHHHHHHHHHHHHH...HR";

const String CMD_PARAM_VERSION = "VPR";
const String CMD_PARAM_LATCH = "LPR";
const String CMD_PARAM_INIT = "I1111PR";
const String CMD_PARAM_QUIET_MODE = "Q1111PR";
const String CMD_PARAM_RAW_FRAME = "W1111222222333333222222333333...PR";
*/

class Command {
 public:
  Command(Adafruit_WS2801 *_strip);
  void ProcessCommand(const uint8_t c);
  boolean IsInitialized(void);

 private:
  void reset(void);
  void printErrorAndReset(const String errorCode, const uint8_t inChar,
                          const uint32_t param = 0xFFFFFFFF);
  void printVersion(const uint8_t s);
  void init(const uint8_t s);
  void latch(const uint8_t s);
  void initCommand(const uint8_t s);
  void quiet(const uint8_t s);

  void calcParity(const uint8_t s);
  boolean checkParity(const uint8_t receivedParity);
  uint8_t calcHexParity();

  void processNumParam(const uint8_t c);
  void processColor(const uint8_t s);

  void processShade(const uint8_t s);
  void processPixel(const uint8_t c);
  void processRawFrame(const uint8_t c);

  uint32_t hex2color(uint32_t val, const uint8_t hex, const uint8_t pos);
  uint16_t hex2uint16(uint16_t val, const uint8_t hex, const uint8_t pos);
  uint8_t hex2uint8(uint8_t val, const uint8_t hex);

  uint8_t getCharType(const uint8_t s);
  uint8_t getHexVal(const uint8_t hex);
  void setCharType(const uint8_t s);
  boolean isReturnCharType(const uint8_t s);

  boolean hasCommand;
  boolean hasNumParam;
  boolean moreParams;
  boolean hasParityByte;
  boolean initialized;
  boolean quietMode;
  boolean noCmd;
  uint8_t charType;
  uint8_t command;
  uint8_t paramPos;
  uint8_t parity;
  uint8_t lastChar;
  uint16_t ledPos;
  uint16_t numParam;
  uint32_t colorParam;
  unsigned long latchTime;

  Adafruit_WS2801 *strip;
};

#endif
