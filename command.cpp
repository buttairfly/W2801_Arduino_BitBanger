#include "command.hpp"

Command::Command(Adafruit_WS2801* _strip)
: strip(_strip){
  initialized = false;
  latchTime = millis() - LATCH_TIMEOUT;
  reset();
}

boolean Command::IsInitialized(void) {
  return initialized;
}

void Command::ProcessCommand(const uint8_t s) {
  setCharType(s);
  if(!hasCommand) {
    if(charType != TYPE_COMMAND) {
        reset();
        return;
    }
    switch(s) {
      case VERSION: // print version
      case LATCH_FRAME: // latch buffered frame
        hasNumParam = true; // version and latchFrame do not have numParam
      case INIT: // Show number of initialized leds
      case PIXEL: // Colorize pixel (position = numParam, parameter = color) no latch
      case SHADE: // Shade first numParam leds (length = numParam, parameter = color) and latch
      case RAW_FRAME: // Frame input (length = numParam, colors) and latch
        initCommand(s);
        return;
      default:
        Serial.print("eucd:"); // error unknown defined
        Serial.print(command, HEX);
        Serial.print("\n");
        Serial.flush();
        reset();
        return;
    }
  } else { // process command
    if (charType != TYPE_HEX || charType != TYPE_RETURN) {
      Serial.print("eul:"); // error unknown letter
      Serial.print(s, HEX);
      Serial.print("\n");
      Serial.flush();
      reset();
      ProcessCommand(s); //maybe it is a new command starting
      return;
    }
    if(hasNumParam){
      switch(command) {
        case SHADE:
          processShade(s);
          return;
        case PIXEL:
          processPixel(s);
          return;
        case RAW_FRAME:
          processRawFrame(s);
          return;
        case INIT:
          init();
          return;
        case LATCH_FRAME:
          latch();
          return;
        case VERSION:
          printVersion();
          return;
        default:
          Serial.print("euc:"); // error unknown command
          Serial.print(command, HEX);
          Serial.print("\n");
          Serial.flush();
          reset();
          return;
      }
    } else {
      if (charType == TYPE_RETURN) {
        Serial.print("euret:"); // error unknown return
        Serial.print(s, HEX);
        Serial.print("\n");
        Serial.flush();
        reset();
        return;
      }
      processNumParam(s);
      if (paramPos >= NUM_PARAM_CHARS) {
        hasNumParam = true;
        paramPos = 0;
        if(initialized && numParam > strip->numPixels()) {
          Serial.print("enpo:"); // error num param overflow
          Serial.print(numParam, HEX);
          Serial.print(",c:"); // command
          Serial.print(command, HEX);
          Serial.print("\n");
          Serial.flush();
          reset();
          return;
        }
      }
    }
  }
}

boolean Command::isReturnCharType(void) {
  if (charType != TYPE_RETURN) {
    Serial.print("enret\n"); // error no return
    Serial.flush();
    reset();
    return false;
  }
  return true;
}

void Command::printVersion(void) {
  if (isReturnCharType()) {
    Serial.print(BUILD_PROGRAM);
    Serial.print(": ");
    Serial.print(BUILD_DATE);
    Serial.print(" ");
    Serial.print(BUILD_VERSION);
    Serial.print("\n");
    Serial.flush();
    reset();
  }
}

void Command::init(void) {
  if (isReturnCharType()) {
    if(!initialized) {
      strip->updateLength(numParam);
    }
    if(strip->numPixels() != 0) {
      Serial.print("Init ");
      Serial.print(strip->numPixels(), HEX);
      if (initialized && numParam != strip->numPixels()) {
        Serial.print(" could not set ");
        Serial.print(numParam, HEX);
      }
      Serial.print('\n');
      Serial.flush();
      initialized = true;
    } else {
      Serial.print("Init failed\n");
      Serial.flush();
    }
    reset();
  }
}

void Command::latch(void) {
  if (isReturnCharType()) {
    unsigned long now = millis();
    if (now - latchTime > LATCH_TIMEOUT) {
      strip->show();
      latchTime = now;
      reset();
    } else {
      Serial.print("elt\n"); // error latch timeout
      Serial.flush();
      reset();
    }
  }
}

void Command::reset(void) {
  colorParam = 0;
  numParam = 0;
  hasCommand = false;
  hasNumParam = false;
  paramPos = 0;
  ledPos = 0;
  charType = TYPE_UNDEFINED;
}

void Command::initCommand(const uint8_t s) {
  if (initialized || s == INIT || s == VERSION) {
    command = s;
    hasCommand = true;
  } else {
    Serial.print("eni:"); // error not initialized
    Serial.print(s);
    Serial.print("\n");
    Serial.flush();
    reset();
  }
}

void Command::processNumParam(const uint8_t s) {
  if (charType == TYPE_HEX) {
    numParam = hex2uint16(numParam, s, paramPos);
    paramPos++;
  } else {
    Serial.print("enh:"); // error not hex
    Serial.print(s);
    Serial.print("\n");
    Serial.flush();
    reset();
  }
}

void Command::processColor(const uint8_t s) {
  if (charType == TYPE_HEX) {
    colorParam = hex2color(colorParam, s, paramPos);
    paramPos++;
    return;
  } else {
    Serial.print("enh:"); // error not hex
    Serial.print(s);
    Serial.print("\n");
    Serial.flush();
    reset();
  }
}

void Command::processShade(const uint8_t s) {
  if (charType == TYPE_RETURN) {
    if (paramPos >= HAS_NUM_SINGLE_COLOR) {
      for(uint16_t i = 0; i < numParam; i++) {
        strip->setPixelColor(i, colorParam);
      }
      latch();
      reset();
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
    }
    return;
  }
  // also test error on identity here
  if(numParam == strip->numPixels()) {
    Serial.print("enp=:"); // error num param equals
    Serial.print(numParam);
    Serial.print("\n");
    Serial.flush();
    reset();
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
uint16_t Command::hex2uint16(uint16_t val, const uint8_t hex, const uint8_t pos) {
  if(pos == 0) {
    val = 0;
  }
  if(pos == 2) {
    val <<= 8;
  }

  val = (val & 0xFF00) | (hex2uint8(val & 0xFF, hex));

  return val;
}

/**
 * hex2color
 * take a hex string and convert it to a 0xRRGGBB color uint32
 */
uint32_t Command::hex2color(uint32_t val, const uint8_t hex, const uint8_t pos) {
  if(pos == 0) {
    val = 0;
  }
  if(pos == 2 || pos == 4) {
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
  // transform hex character to the 4bit equivalent number, using the ascii table indexes
  if      (hex >= '0' && hex <= '9') return hex - '0';
  else if (hex >= 'a' && hex <= 'f') return - 'a' + 10;
  else if (hex >= 'A' && hex <= 'F') return - 'A' + 10;
  return  TYPE_UNKNOWN;
}

void Command::setCharType(const uint8_t s) {
  switch(s) {
    case INIT:        // init length of ws2801 strip
    case PIXEL:       // Colorize pixel (position = numParam, parameter = color) no latch
    case SHADE:       // Shade first numParam leds (length = numParam, parameter = color) and latch
    case RAW_FRAME:   // Frame input (length = numParam, colors) and latch
    case LATCH_FRAME: // latch buffered frame
    case VERSION:     // print version
      charType = TYPE_COMMAND;
      break;
    case '\n':
      charType = TYPE_RETURN;
      break;
    default:
      if (getHexVal(s) != TYPE_UNKNOWN) {
        charType = TYPE_HEX;
        return;
      }
      charType = TYPE_UNKNOWN;
      return;
  }
}
