#ifndef _xdaqtkbeam_h
#define _xdaqtkbeam_h

#include "i2o.h"

/*
 *   TKBEAM BUILDER PART 
 */


#define I2O_TKBEAM_CMD      0x41
#define I2O_TKBEAM_ACK      0x42

#define TKBEAM_MAXFED         180
#define TKBEAM_MAXCHANNEL    (96* TKBEAM_MAXFED)
#define TKBEAM_CHANNEL_SIZE  280

typedef struct _I2O_TKBEAM_MESSAGE_FRAME {
#if defined(LITTLE_ENDIAN__)
  I2O_PRIVATE_MESSAGE_FRAME   	PvtMessageFrame;
  U32 run;
  U32 spill;
  U32 event;
  U32 bx;
  U8  type;
  U32 dbversion;
  U8  latency;
  U8  calchan;
  U8  calsel;
  U8 multimode;
  U16  nchannel;
  U32  channel_id[TKBEAM_MAXCHANNEL];
  U32  channel_size[TKBEAM_MAXCHANNEL];
  U8 pllcoarseshift;
  U8 pllfineshift;
  U8 tscfedshift;
  U8 optogain;
  U8 optobias;
  U8 aligndelay;
  U32 idkeyon;
  U32 process;
  U32 vpsp;
  U32 kmux;
  U32 idinstance;
  U32 idip;
  U16			      data[1]; 
#elif defined(BIG_ENDIAN__)
  I2O_PRIVATE_MESSAGE_FRAME   PvtMessageFrame;
  U32 run;
  U32 spill;
  U32 event;
  U32 bx;
  U8  type;    
  U32 dbversion;
  U8  latency;
  U8  calchan;
  U8  calsel;
  U8 multimode;
  U16  nchannel;
  U32  channel_id[TKBEAM_MAXCHANNEL];
  U32  channel_size[TKBEAM_MAXCHANNEL];
  U8 pllcoarseshift;
  U8 pllfineshift;
  U8 tscfedshift;
  U8 optogain;
  U8 optobias;
  U8 aligndelay;
  U32 idkeyon;
  U32 process;
  U32 vpsp;
  U32 kmux;
  U32 idinstance;
  U32 idip;
  U16			      data[1]; 
#endif
} I2O_TKBEAM_MESSAGE_FRAME, *PI2O_TKBEAM_MESSAGE_FRAME;

typedef struct _I2O_TKBEAM_ACK_MESSAGE_FRAME {
#if defined(LITTLE_ENDIAN__)
   I2O_PRIVATE_MESSAGE_FRAME   	PvtMessageFrame;
   U32                        error;
   U32			      reply; 
#elif defined(BIG_ENDIAN__)
   I2O_PRIVATE_MESSAGE_FRAME   PvtMessageFrame;
   U32                        error;
   U32			      reply;
#endif
} I2O_TKBEAM_ACK_MESSAGE_FRAME, *PI2O_TKBEAM_ACK_MESSAGE_FRAME;

#endif
