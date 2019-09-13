// Do some fun switching so we can have both programs in the same file and still
// compile and flash with MPLABX (blasted NetBeans platform IDEs!)
#define TRANSMITTER

#ifdef TRANSMITTER
#include "transmitter.c"
#endif

#ifndef TRANSMITTER
#include "receiver.c"
#endif
