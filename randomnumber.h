#ifndef RANDOMNUMBER_H
#define RANDOMNUMBER_H

#include <time.h>

class RandomNumber
{
public:

    RandomNumber();

    void reset();

    double white( double scale );

private:

  unsigned long  m_seed;
  unsigned long  m_white;
};





#endif // RANDOMNUMBER_H
