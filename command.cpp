#include "command.hpp"

Command::Command(Adafruit_WS2801* _strip)
: strip(_strip){
  initialized = false;
  reset();
}

void Command::Init(const uint8_t s){
  if(!initialized) {
    if (!hasCommand) {
      if (s == INIT) {  // Init input (length = numParam, colors)
        initCommand(s);
      } else return;
    } else { // hasCommand
      processNumParam(s);
      if (commandPos >= INIT_LEN_CHAR) {
        strip->updateLength(numParam);
        if(strip->numPixels() != 0) {
          initialized = true;

          Serial.print("Init ");
          Serial.print(strip.numPixels(), HEX);
          Serial.print('\n');
          Serial.flush();
        } else {
          Serial.print("Init failed\n");
          Serial.flush();
        }
        reset();
      }
    }
  }
}


boolean Command::IsInitialized(void) {
  return initialized;
}

void Command::ProcessCommand(const uint8_t s){
  if(!hasCommand) {
    switch(s) {
      case PIXEL: // Colorize pixel (position = numParam, parameter = color) no latch
      case SHADE: // Shade first numParam leds (length = numParam, parameter = color) and latch
      case RAW_FRAME: // Frame input (length = numParam, colors) and latch
        initCommand(s);
        break;
      case LATCH_FRAME: // latch buffered frame
        strip->show();
        reset();
        break;
      default: break;
    }
  } else { // process command
    if(hasNumParam){
      switch(command) {
        case SHADE:
          processShade(s);
          break;
        case PIXEL:
          processPixel(s);
          break;
        case RAW_FRAME:
          processRawFrame(s);
          break;
        case INIT:
          Serial.print("Already initialized ");
          Serial.print(strip.numPixels(), HEX);
          Serial.print('\n');
          Serial.flush();
          reset();
          break;
        default:
          Serial.print("Unknown command:");
          Serial.print(command);
          Serial.print("\n");
          Serial.flush();
          reset();
          break;
      }
    } else {
      processNumParam(s);
      if (commandPos >= HAS_NUM_LEN_CHAR) {
        hasNumParam = true;
        paramPos = 0;
        if(numParam > strip->numPixels()) {
          Serial.print("NumParam error:");
          Serial.print(numParam);
          Serial.print("\n");
          Serial.flush();
          reset();
        }
      }
    }
  }
}

void Command::reset(void) {
  colorParam = -1;
  numParam = -1;
  hasCommand = false;
  hasNumParam = false;
  paramPos = 0;
  ledPos = 0;
}

void Command::initCommand(const uint8_t s) {
  command = s;
  hasCommand = true;
}

void Command::processNumParam(const uint8_t s) {
    numParam = hex2uint16(numParam, s, paramPos);
    paramPos++;
}

void Command::processColor(const uint8_t s) {
    colorParam = hex2uint16(colorParam, s, paramPos);
    paramPos++;
}

void Command::processShade(const uint8_t s) {
  processColor(s);
  if (commandPos >= HAS_NUM_SINGLE_COLOR) {
    for(uint16_t i = 0; i < numParam; i++) {
      strip->setPixelColor(i, colorParam);
    }
    strip.show();
    reset();
  }
}

void Command::processPixel(const uint8_t s) {
  // also test error on identity here
  if(numParam = strip->numPixels()) {
    Serial.print("NumParam processPixel error:");
    Serial.print(numParam);
    Serial.print("\n");
    Serial.flush();
    reset();
  }
  processColor(s);
  if (commandPos >= HAS_NUM_SINGLE_COLOR) {
    strip->setPixelColor(numParam, colorParam);
    reset();
  }
}

void Command::processRawFrame(const uint8_t s) {
  Serial.print("processRawFrame\n");
}

/**
 * hex2uint16
 * take a hex string and convert it to a 16bit number
 */
uint16_t Command::hex2uint16(uint16_t val, const uint8_t hex, const uint32_t pos) {
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
uint32_t Command::hex2color(uint32_t val, const uint8_t hex, const uint32_t pos) {
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
  // transform hex character to the 4bit equivalent number, using the ascii table indexes
  if (hex >= '0' && hex <= '9') hex = hex - '0';
  else if (hex >= 'a' && hex <='f') hex = hex - 'a' + 10;
  else if (hex >= 'A' && hex <='F') hex = hex - 'A' + 10;
  // shift 4 to make space for new digit, and add the 4 bits of the new digit
  val = (val << 4) | (hex & 0xF);

  return val;
}
