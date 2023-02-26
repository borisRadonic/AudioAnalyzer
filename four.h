#ifndef FOUR_H
#define FOUR_H

class FFTUtility
{
public:

    static void fft( double data[], unsigned long nn );
    static double fftMagnitude( double data[], unsigned long nn, unsigned long k );
    static double fftMagdB( double data[], unsigned long nn, unsigned long k, double fullScale );
    static double fftPhase( double data[], unsigned long nn, unsigned long k );
    static double fftFrequency( unsigned long nn, unsigned long k, double fs );
    static double goertzel( unsigned int numSamples, unsigned int frequency, unsigned int sampleRate, double* data );

private:

    FFTUtility()
    {
    }
};

#endif // FOUR_H
