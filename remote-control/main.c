// Do some fun switching so we can have both programs in the same file and still
// compile and flash with MPLABX (blasted NetBeans platform IDEs!)
#define TRANSMITTER

// Code definitions
// Only the last 3 bits are used
//                 XXXXX---
#define CODE_NOP 0b00000000 // No-op
#define CODE_VUP 0b00000001 // Volume up
#define CODE_VDN 0b00000010 // Volume down
#define CODE_VMT 0b00000011 // Mute/unmute
#define CODE_CUP 0b00000100 // Channel up
#define CODE_CDN 0b00000101 // Channel down
#define CODE_CRS 0b00000110 // Channel reset

#ifdef TRANSMITTER
#include "transmitter.c"
#endif

#ifndef TRANSMITTER
#include "receiver.c"
#endif
