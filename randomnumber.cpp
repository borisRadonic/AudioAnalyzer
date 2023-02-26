#include "randomnumber.h"

RandomNumber::RandomNumber():m_white(0)
{
}

void RandomNumber::reset()
{
    m_seed = time(NULL);
}

double RandomNumber::white( double scale )
{
    m_seed   = (m_seed * 196314165) + 907633515;
    m_white  = m_seed >> 9;
    m_white |= 0x40000000;
    return (double) ((*(float*)&m_white)-3.0)*scale;
}
