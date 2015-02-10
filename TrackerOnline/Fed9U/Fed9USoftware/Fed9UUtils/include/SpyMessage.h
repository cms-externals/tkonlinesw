#ifndef _SpyMessage_h_
#define _SpyMessage_h_
#endif

#include "i2o/Method.h"
#include "i2o/utils/Dispatcher.h"
#include "i2o/Messenger.h" 
#include "i2o/Listener.h" 

#define I2O_SPY_MESSAGE_CODE 0xAF

typedef struct _I2O_SPY_MESSAGE_FRAME {
  I2O_PRIVATE_MESSAGE_FRAME PvtMessageFrame;
  U32 version;
  U32 nObjects;
  S32 data[];
} I2O_SPY_MESSAGE_FRAME;

