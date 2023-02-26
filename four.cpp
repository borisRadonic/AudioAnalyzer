#include "four.h"
#include <iostream>     // std::cout
#include <algorithm>    // std::swap
#include <vector>       // std::vector
#include "math.h"
#include <complex.h>

// derived from Numerical Recipes in C, Cambridge University Press

#define SWAP(a,b) tempr = (a); (a) = (b); (b) = tempr

// Input: nn is the number of points in the data and in the FFT,
//           nn must be a power of 2
// Input: data is sampled voltage v(0),0,v(1),0,v(2),...v(nn-1),0 versus time
// Output: data is complex FFT Re[V(0)],Im[V(0)], Re[V(1)],Im[V(1)],...
// data is an array of 2*nn elements
void FFTUtility::fft(double data[], unsigned long nn)
{
unsigned long n,mmax,m,j,istep,i;
double wtemp,wr,wpr,wpi,wi,theta;
double tempr,tempi;
    n = nn<<1;  // n is the size of data array (2*nn)
    j = 1;
    for(i=1; i<n; i+=2){
        if(j > i){				// bit reversal section
            SWAP(data[j-1],data[i-1]);
            SWAP(data[j],data[i]);
        }
        m = n>>1;
        while((m >= 2)&&(j > m)){
            j = j-m;
            m = m>>1;
        }
        j = j+m;
    }
    mmax = 2;             // Danielson-Lanczos section
    while( n > mmax){     // executed log2(nn) times
        istep = mmax<<1;
        theta = -6.283185307179586476925286766559/mmax;
        // the above line should be + for inverse FFT
        wtemp = sin(0.5*theta);
        wpr = -2.0*wtemp*wtemp;  // real part
        wpi = sin(theta);        // imaginary part
        wr = 1.0;
        wi = 0.0;
        for(m=1; m<mmax; m+=2){
            for(i=m; i<=n; i=i+istep){
                j = i+mmax;
                tempr     = wr*data[j-1]-wi*data[j]; // Danielson-Lanczos formula
                tempi     = wr*data[j]+wi*data[j-1];
                data[j-1] = data[i-1]-tempr;
                data[j]   = data[i]-tempi;
                data[i-1] = data[i-1]+tempr;
                data[i]   = data[i]+tempi;
            }
            wtemp = wr;
            wr = wr*wpr-wi*wpi+wr;
            wi = wi*wpr+wtemp*wpi+wi;
        }
        mmax = istep;
    }
}

//-----------------------------------------------------------
// Calculates the FFT magnitude at a given frequency index.
// Input: data is complex FFT Re[V(0)],Im[V(0)], Re[V(1)],Im[V(1)],...
// Input: nn is the number of points in the data and in the FFT,
//           nn must be a power of 2
// Input: k is frequency index 0 to nn/2-1
//        E.g., if nn=16384, then k can be 0 to 8191
// Output: Magnitude in volts at this frequency (volts)
// data is an array of 2*nn elements
// returns 0 if k >= nn/2
double FFTUtility::fftMagnitude(double data[], unsigned long nn, unsigned long k)
{
    double nr, realPart, imagPart;

    nr = (double) nn;
    if (k >= nn/2){
        return 0.0; // out of range
    }
    if (k == 0){
        return sqrt(data[0] * data[0] + data[1] * data[1]) / nr;
    }
    realPart = fabs(data[2*k])   + fabs(data[2*nn-2*k]);
    imagPart = fabs(data[2*k+1]) + fabs(data[2*nn-2*k+1]);
    return  sqrt(realPart * realPart + imagPart * imagPart) / nr;
}

//-----------------------------------------------------------
// Calculates the FFT magnitude in db full scale at a given frequency index.
// Input: data is complex FFT Re[V(0)],Im[V(0)], Re[V(1)],Im[V(1)],...
// Input: nn is the number of points in the data and in the FFT,
//           nn must be a power of 2
// Input: k is frequency index 0 to nn/2-1
//        E.g., if nn=16384, then k can be 0 to 8191
// Input: fullScale is the largest possible component in volts
// Output: Magnitude in db full scale at this frequency
// data is an array of 2*nn elements
// returns -200 if k >= nn/2
double FFTUtility::fftMagdB(double data[], unsigned long nn, unsigned long k, double fullScale)
{
    double magnitude = fftMagnitude(data, nn, k);
    if (magnitude<0.0000000001)
    { // less than 0.1 nV
        return -200; // out of range
    }
    return 20.0*log10(magnitude/fullScale);
}

//-----------------------------------------------------------
// Calculates the FFT phase at a given frequency index.
// Input: data is complex FFT Re[V(0)],Im[V(0)], Re[V(1)],Im[V(1)],...
// Input: nn is the number of points in the data and in the FFT,
//           nn must be a power of 2
// Input: k is frequency index 0 to nn/2-1
//        E.g., if nn=16384, then k can be 0 to 8191
// Output: Phase at this frequency
// data is an array of 2*nn elements
// returns 0 if k >= nn/2
double FFTUtility::fftPhase(double data[], unsigned long nn, unsigned long k)
{
    if (k >= nn/2)
    {
        return 0.0;     // out of range
    }
    if (data[2*k+1]==0.0)
    {
        return 0.0;     // imaginary part is zero
    }
    if (data[2*k]==0.0)
    { // real part is zero
        if (data[2*k+1]>0.0)
        { // real part is zero
            return 1.5707963267948966192313216916398;    // 90 degrees
        }
        else
        {
            return -1.5707963267948966192313216916398;   // -90 degrees
        }
    }
    return atan2 (data[2*k+1], data[2*k]);  // imaginary part over real part
}

//-----------------------------------------------------------
// Calculates equivalent frequency in Hz at a given frequency index.
// Input: fs is sampling rate in Hz
// Input: nn is the number of points in the data and in the FFT,
//           nn must be a power of 2
// Input: k is frequency index 0 to nn-1
//        E.g., if nn=16384, then k can be 0 to 16383
// Output: Equivalent frequency in Hz
// returns 0 if k >= nn
double FFTUtility::fftFrequency (unsigned long nn, unsigned long k, double fs)
{
    if (k >= nn)
    {
        return 0.0;     // out of range
    }

    if (k <= nn/2)
    {
        return fs * (double)k / (double)nn;
    }
    return -fs * (double)(nn-k)/ (double)nn;
}

double FFTUtility::goertzel(unsigned int numSamples, unsigned int frequency, unsigned int sampleRate, double* data)
{
    double q0(0.0);
    double q1(0.0);
    double q2(0.0);
    double samples = (double) numSamples;
    unsigned int k = (int) (0.5 + ((samples * frequency) / sampleRate));
    double omega = (2.0 * M_PI * k) / samples;
    double sine = sin(omega);
    double cosine = cos(omega);
    double coeff = 2.0 * cosine;
    int i(0);
    while( i < numSamples*2 )
    {
        q0 = coeff * q1 - q2 + data[i];
        q2 = q1;
        q1 = q0;
        i = i + 2;
    }
    double real = (q1 - q2 * cosine);
    double imag = (q2 * sine);
    return real*real + imag*imag;
}
