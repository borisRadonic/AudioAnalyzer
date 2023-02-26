#ifndef ANALOGSIGNAL_H
#define ANALOGSIGNAL_H

#include "SignalBuffer.h"


class AnalogSignal : public CSignalBuffer
{
public:
    AnalogSignal():CSignalBuffer()
    {
    }

    ~AnalogSignal(void)
    {
    }
};


#endif // ANALOGSIGNAL_H
