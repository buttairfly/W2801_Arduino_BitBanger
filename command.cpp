#include "command.hpp"

Command::Command(Adafruit_WS2801 *_strip) : strip(_strip) {
  initialized = false;
  quietMode = false;
  latchTime = millis() - LATCH_TIMEOUT;
  reset();
}

boolean Command::IsInitialized(void) { return initialized; }

void Command::ProcessCommand(const uint8_t s) {
  if (!quietMode) {
    Serial.write(s);
    Serial.flush();
  }
  setCharType(s);
  calcParity(s);
  if (!hasCommand) {
    if (charType != TYPE_COMMAND) {
      printErrorAndReset(ErrorNoCommand, s);
      return;
    }
    switch (s) {
      case VERSION:          // print version
      case LATCH_FRAME:      // latch buffered frame
        hasNumParam = true;  // version and latchFrame do not have numParam
      case QUIET_MODE:  // enables or disables return of received parameters
      case INIT:        // Show number of initialized leds
      case PIXEL:  // Colorize pixel (position = numParam, parameter = color) no
                   // latch
      case SHADE:  // Shade first numParam leds (length = numParam, parameter =
                   // color) and latch
      case RAW_FRAME:  // Frame input (length = numParam, colors) and latch
        initCommand(s);
        return;
      default:
        printErrorAndReset(ErrorNotDefinedCommand, s);
        return;
    }
  } else {  // process command
    if (charType != TYPE_HEX && charType != TYPE_RETURN) {
      printErrorAndReset(ErrorUnknownLetter, s, charType);
      ProcessCommand(s);  // maybe it is a new command starting
      return;
    }
    if (!hasNumParam) {
      if (charType == TYPE_RETURN) {
        printErrorAndReset(ErrorUnknownReturn, s);
        return;
      }
      processNumParam(s);
      if (paramPos >= NUM_PARAM_CHARS) {
        hasNumParam = true;
        paramPos = 0;
        if (initialized && command != QUIET_MODE &&
            numParam > strip->numPixels()) {
          printErrorAndReset(ErrorNumberParameterOverflow, s, numParam);
          return;
        }
      }
    } else {
      if (!hasParityByte) {
        if (charType != TYPE_HEX) {
          printErrorAndReset(ErrorNotHexNumberParameter, s, charType);
          return;
        }
        if (!checkParity(s)) {
          printErrorAndReset(ErrorWrongParity, s, calcHexParity());
          return;
        }
        hasParityByte = true;
        return;
      }
      switch (command) {
        case SHADE:
          processShade(s);
          return;
        case PIXEL:
          processPixel(s);
          return;
        case RAW_FRAME:
          processRawFrame(s);
          return;
        case QUIET_MODE:
          quiet(s);
          return;
        case INIT:
          init(s);
          return;
        case LATCH_FRAME:
          latch(s);
          return;
        case VERSION:
          printVersion(s);
          return;
        default:
          printErrorAndReset(ErrorUnknownCommand, s, command);
          return;
      }
    }
  }
}

void Command::printErrorAndReset(const String errorCode, const uint8_t s,
                                 const uint32_t param = 0xFFFFFFFF) {
  if (!quietMode) {
    Serial.print("\n");  // initial new line to highlight error
  }
  Serial.print(errorCode);
  if (param != 0xFFFFFFFF) {
    Serial.print(",");
    Serial.print("p");
    Serial.write(param);
  }
  if (s != 0x0) {
    Serial.print(",");
    Serial.print("s");
    Serial.write(s);
  }
  Serial.print("\n");
  Serial.flush();
  reset();
}

boolean Command::isReturnCharType(const uint8_t s) {
  if (charType != TYPE_RETURN) {
    printErrorAndReset(ErrorNoReturn, s);
    return false;
  }
  return true;
}

void Command::printVersion(const uint8_t s) {
  if (isReturnCharType(s)) {
    Serial.print(BUILD_PROGRAM);
    Serial.print(": ");
    Serial.print(BUILD_DATE);
    Serial.print(" - ");
    Serial.print(BUILD_VERSION);
    Serial.print("\n");
    Serial.flush();
    reset();
  }
}

void Command::init(const uint8_t s) {
  if (isReturnCharType(s)) {
    if (!initialized) {
      strip->updateLength(numParam);
    }
    if (strip->numPixels() != 0) {
      Serial.print("Init ");
      Serial.print(strip->numPixels(), HEX);
      if (numParam == strip->numPixels()) {
        initialized = true;
      } else

      {
        Serial.print(" should be ");
        Serial.print(numParam, HEX);
      }
      Serial.print('\n');
      Serial.flush();
      reset();
    } else {
      printErrorAndReset(ErrorNoInitialisationPossible, 0x0);
    }
  }
}

void Command::quiet(const uint8_t s) {
  if (isReturnCharType(s)) {
    numParam != 0 ? quietMode = true : quietMode = false;
  }
}

void Command::latch(const uint8_t s) {
  if (isReturnCharType(s)) {
    unsigned long now = millis();
    if (now - latchTime > LATCH_TIMEOUT) {
      strip->show();
      latchTime = now;
      reset();
    } else {
      printErrorAndReset(ErrorLatchTimeout, 0x0, LATCH_TIMEOUT);
    }
  }
}

void Command::reset(void) {
  colorParam = 0;
  numParam = 0;
  hasCommand = false;
  hasNumParam = false;
  hasParityByte = false;
  paramPos = 0;
  ledPos = 0;
  charType = TYPE_UNDEFINED;
  parity = PARITY_SEED;
  lastChar = 0;
}

void Command::initCommand(const uint8_t s) {
  if (initialized || s == INIT || s == VERSION | s == QUIET_MODE) {
    command = s;
    hasCommand = true;
  } else {
    printErrorAndReset(ErrorNotInitialized, s);
  }
}

void Command::calcParity(const uint8_t s) {
  if (charType != TYPE_RETURN) {
    parity ^= lastChar;
    lastChar = s;

    Serial.print("calcParity");
    Serial.println(parity, HEX);
  }
}

uint8_t Command::calcHexParity() {
  uint8_t highParity = (parity & 0xf0) >> 4;
  uint8_t lowParity = parity & 0xf;
  return highParity ^ lowParity;
}

boolean Command::checkParity(const uint8_t receivedParity) {
  Serial.print("\ncheckParity ");
  Serial.print(calcHexParity(), HEX);
  Serial.print(" ");
  Serial.println(receivedParity, HEX);
  return calcHexParity() == receivedParity;
}

void Command::processNumParam(const uint8_t s) {
  if (charType == TYPE_HEX) {
    numParam = hex2uint16(numParam, s, paramPos);
    paramPos++;
  } else {
    printErrorAndReset(ErrorNotHexNumberParameter, s);
    return;
  }
}

void Command::processColor(const uint8_t s) {
  if (charType == TYPE_HEX) {
    colorParam = hex2color(colorParam, s, paramPos);
    paramPos++;
    return;
  } else {
    printErrorAndReset(ErrorNotHexColorParameter, s);
    return;
  }
}

void Command::processShade(const uint8_t s) {
  if (charType == TYPE_RETURN) {
    if (paramPos >= HAS_NUM_SINGLE_COLOR) {
      for (uint16_t i = 0; i < numParam; i++) {
        strip->setPixelColor(i, colorParam);
      }
      reset();
    } else {
      printErrorAndReset(ErrorNotEnoughBytesColorParam, s, paramPos);
      return;
    }
    return;
  }
  processColor(s);
}

void Command::processPixel(const uint8_t s) {
  if (charType == TYPE_RETURN) {
    if (paramPos >= HAS_NUM_SINGLE_COLOR) {
      strip->setPixelColor(numParam, colorParam);
      reset();
    } else {
      printErrorAndReset(ErrorNotEnoughBytesColorParam, s, paramPos);
      return;
    }
    return;
  }
  // also test error on identity here
  if (numParam == strip->numPixels()) {
    printErrorAndReset(ErrorNumberParameterOverflowEqualsNumLeds, s, numParam);
    return;
  }
  processColor(s);
}

void Command::processRawFrame(const uint8_t s) {
  Serial.print("processRawFrame\n");
  Serial.flush();
  reset();
}

/**
 * hex2uint16
 * take a hex string and convert it to a 16bit number
 */
uint16_t Command::hex2uint16(uint16_t val, const uint8_t hex,
                             const uint8_t pos) {
  if (pos == 0) {
    val = 0;
  }
  if (pos == 2) {
    val <<= 8;
  }

  val = (val & 0xFF00) | (hex2uint8(val & 0xFF, hex));

  return val;
}

/**
 * hex2color
 * take a hex string and convert it to a 0xRRGGBB color uint32
 */
uint32_t Command::hex2color(uint32_t val, const uint8_t hex,
                            const uint8_t pos) {
  if (pos == 0) {
    val = 0;
  }
  if (pos == 2 || pos == 4) {
    val <<= 8;
  }

  val = (val & 0xFFFF00) | (hex2uint8(val & 0xFF, hex));

  return val;
}

uint8_t Command::hex2uint8(uint8_t val, const uint8_t hex) {
  uint8_t number = getHexVal(hex);
  // shift 4 to make space for new digit, and add the 4 bits of the new digit
  return (val << 4) | (number & 0xF);
}

uint8_t Command::getHexVal(const uint8_t hex) {
  // transform hex character to the 4bit equivalent number, using the ascii
  // table indexes
  if (hex >= '0' && hex <= '9')
    return hex - '0';
  else if (hex >= 'a' && hex <= 'f')
    return hex - 'a' + 10;
  else if (hex >= 'A' && hex <= 'F')
    return hex - 'A' + 10;
  return TYPE_UNKNOWN;
}

void Command::setCharType(const uint8_t s) {
  switch (s) {
    case QUIET_MODE:  // enables or disables return of received parameters
    case INIT:        // init length of ws2801 strip
    case PIXEL:  // Colorize pixel (position = numParam, parameter = color) no
                 // latch
    case SHADE:  // Shade first numParam leds (length = numParam, parameter =
                 // color) and latch
    case RAW_FRAME:    // Frame input (length = numParam, colors) and latch
    case LATCH_FRAME:  // latch buffered frame
    case VERSION:      // print version
      charType = TYPE_COMMAND;
      return;
    case '\n':
      charType = TYPE_RETURN;
      return;
    default:
      if (getHexVal(s) != TYPE_UNKNOWN) {
        charType = TYPE_HEX;
        return;
      }
      charType = TYPE_UNKNOWN;
      return;
  }
}
