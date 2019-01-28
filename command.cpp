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
        command = s;
        hasCommand = true;
        numParam = 0;
        commandPos = 0;
      } else return;
    } else { // hasCommand
      processNumParam(s);
      if (commandPos >= INIT_LEN_CHAR) {
        initialized = true;
        strip->updateLength(numParam);
        Serial.print("numParam ");
        Serial.print(numParam, HEX);
        Serial.print('\n');
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
      case PIXEL: // Colorize pixel (length = numParam, parameter = color)
      case FRAME: // Frame input (length = numParam, colors)
        command = s;
        hasCommand = true;
        break;
      case SHOW: // show buffered frame
        strip->show();
        reset();
        break;
      default: break;
    }
  } else { // process command
    if(hasNumParam){
      switch(command) {
        case PIXEL:
          processPixel(s);
          break;
        case FRAME:
          processFrame(s);
          break;
        default:
          reset();
          break;
      }
    } else {
      processNumParam(s);
      if (commandPos >= HAS_NUM_LEN_CHAR) {
        hasNumParam = true;
      }
    }
  }
}

void Command::reset(void) {
  numParam = -1;
  hasCommand = false;
  hasNumParam = false;
  commandPos = 0;
}

void Command::processNumParam(const uint8_t s) {
    numParam = hex2uint16(numParam, s, commandPos);
    commandPos++;
}

void Command::processPixel(const uint8_t s) {
  Serial.print("processPixel\n");
}

void Command::processFrame(const uint8_t s) {
  Serial.print("processFrame\n");
}

/**
 * hex2int
 * take a hex string and convert it to a 8bit number
 */
uint16_t Command::hex2uint16(uint16_t val, uint8_t hex, uint32_t pos) {
  if(pos == 0) {
    val = 0;
  }
  if(pos == 2) {
    val = val << 8;
  }
  val = val | (hex2uint8(val, hex) & 0xFF);
  Serial.print(pos);
  Serial.print(':');
  Serial.print(val, HEX);
  Serial.print(';');
  return val;
}

uint8_t Command::hex2uint8(uint8_t val, uint8_t hex) {
  // transform hex character to the 4bit equivalent number, using the ascii table indexes
  if (hex >= '0' && hex <= '9') hex = hex - '0';
  else if (hex >= 'a' && hex <='f') hex = hex - 'a' + 10;
  else if (hex >= 'A' && hex <='F') hex = hex - 'A' + 10;
  // shift 4 to make space for new digit, and add the 4 bits of the new digit
  val = (val << 4) | (hex & 0xF);

  Serial.print('h');
  Serial.print(hex);
  Serial.print(':');
  Serial.print(val, HEX);
  Serial.print(';h');
  return val;
}
