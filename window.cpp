#include "window.h"
#include <QDebug>

#include <QtMath>
#include <stdio.h>

#define C_PI 3.14159

double BaseWindow::w( int i )
{
    if( i < m_w.size() )
    {
        return m_w[i];
    }
    return 0;
}

BaseWindow* WindowFactory::create( BaseWindow::Window window, int size, double attenuation )
{
    switch(window)
    {
    case BaseWindow::Rectangular:
    {
        return new WindowRectangular( size );
    }
    case BaseWindow::Chebyshev:
    {
        return new WindowChebyshev( size,attenuation );
    }
    case BaseWindow::FlatTop:
    {
        return new WindowFlatTop( size );
    }
    case BaseWindow::Hamming:
    {
        return new WindowHamming( size );
    }
    case BaseWindow::Hann:
    {
        return new WindowHann( size );
    }
    case BaseWindow::Kaiser:
    {
        return new WindowKaiser( size,attenuation );
    }
    case BaseWindow::BartlettHann:
    {
        return new WindowBartlettHann( size );
    }
    case BaseWindow::Blackman:
    {
        return new WindowBlackman( size );
    }
    case BaseWindow::BlackmanHarris:
    {
        return new WindowBlackmanHarris( size );
    }
    case BaseWindow::Nuttall:
    {
        return new WindowNuttall( size );
    }
    case BaseWindow::BlackmanNuttall:
    {
        return new WindowBlackmanNuttall( size );
    }
    case BaseWindow::Parzen:
    {
        return new WindowParzen( size );
    }
    case BaseWindow::Triangular:
    {
        return new WindowTriangular( size );
    }
    case BaseWindow::Tukey:
    {
        return new WindowTukey( size );
    }
    case BaseWindow::Welch:
    {
        return new WindowWelch( size );
    }
    case BaseWindow::Lanczos:
    {
        return new WindowLanczos( size );
    }
    }
    return NULL;
}

WindowRectangular::WindowRectangular( int size ):BaseWindow( size, 0.0 )
{
    for( int i = 0; i < size; i++ )
    {
        m_w.push_back(1.0);
    }
}


double acosh( double x )
{
    return 2 * log (sqrt((x+1) / 2.0) + sqrt( (x-1)/2.0) );
}

float beta(int n, float alpha)
{
  return cosh (acosh(pow(10,alpha))/(n-1));
}


double T(double n, double x){
  if(fabs(x)<=1)
  {
    return cos(n*acos(x));
  }
  else
  {
    return cosh(n*acosh(x));
  }
}
WindowChebyshev::WindowChebyshev( int size, double attenuation ):BaseWindow( size, 0.0 )
{
    //todo

    attenuation = attenuation < 0 ? -attenuation : attenuation;

    int k;
    float a = attenuation/20;
    int M=size/2;
    int N=M*2;
    double b = beta(N,a);

    for(int i=0;i < size;i++ )
    {
        double sum=0;
        for(k=0;k<M;k++)
        {
            sum += (k&1?-1:1)*T(N,b*cos(M_PI*k/N)) * cos (2*i*k*M_PI/N);
        }
        sum /= T(N,b);
        sum-=.5;
        m_w.push_back(sum);
        //qDebug() << sum << i;
    }
    qDebug() << "WindowChebyshev created";
 }

WindowFlatTop::WindowFlatTop( int size ):BaseWindow( size, 0.0 )
{
    double w(0.0);
    for( int i = 0; i < size; i++ )
    {
        w = (   1.000
              - 1.930 * cos (2.0*C_PI*(double)i/(double)(size-1))
              + 1.290 * cos (4.0*C_PI*(double)i/(double)(size-1))
              - 0.388 * cos (6.0*C_PI*(double)i/(double)(size-1))
              + 0.028 * cos (8.0*C_PI*(double)i/(double)(size-1)) );
        //qDebug() << w;
        m_w.push_back(w);
    }
}

WindowHamming::WindowHamming( int size ):BaseWindow( size, 0.0 )
{
    double w(0.0);
    for( int i = 0; i < size; i++ )
    {
        w = ( 0.54 - 0.46 * cos (2.0*C_PI*(double)i/(double)(size-1)) );
        //qDebug() << w;
        m_w.push_back(w);
    }
}

WindowHann::WindowHann( int size ):BaseWindow( size, 0.0 )
{
    double w(0.0);
    for( int i = 0; i < size; i++ )
    {
        w = ( 0.5 * (1.0 - cos (2.0*C_PI*(double)i/(double)(size-1))) );
        //qDebug() << w;
        m_w.push_back(w);
    }
}

WindowKaiser::WindowKaiser( int size, double attenuation  ):BaseWindow( size, 0.0 )
{
}

WindowBartlettHann::WindowBartlettHann( int size ):BaseWindow( size, 0.0 )
{
    double w(0.0);
    for( int i = 0; i < size; i++ )
    {
        w = (   0.62
              - 0.48 * abs( ((double)i/(double)(size-1)) - 0.5 )
              - 0.38 * cos (2.0*C_PI*(double)i/(double)(size-1)));

        //qDebug() << w;
        m_w.push_back(w);
    }
}

WindowBlackman::WindowBlackman( int size ):BaseWindow( size, 0.0 )
{
    double w(0.0);
    for( int i = 0; i < size; i++ )
    {
        w = ( 0.42 - 0.5 * cos (2.0*C_PI*(double)i/(double)(size-1))
            + 0.08 * cos (4.0*C_PI*(double)i/(double)(size-1)) );
        //qDebug() << w;
        m_w.push_back(w);
    }
}

WindowBlackmanHarris::WindowBlackmanHarris( int size ):BaseWindow( size, 0.0 )
{
    double w(0.0);
    for( int i = 0; i < size; i++ )
    {
        w = (   0.358750
              - 0.488290 * cos (2.0*C_PI*(double)i/(double)(size-1))
              + 0.141280 * cos (4.0*C_PI*(double)i/(double)(size-1))
              - 0.001168 * cos (6.0*C_PI*(double)i/(double)(size-1)) );
        //qDebug() << w;
        m_w.push_back(w);
    }
}

WindowNuttall::WindowNuttall( int size ):BaseWindow( size, 0.0 )
{
    double w(0.0);
    for( int i = 0; i < size; i++ )
    {
        w = (   0.355768
              - 0.487396 * cos (2.0*C_PI*(double)i/(double)(size-1))
              + 0.144232 * cos (4.0*C_PI*(double)i/(double)(size-1))
              - 0.012604 * cos (6.0*C_PI*(double)i/(double)(size-1)) );
        //qDebug() << w;
        m_w.push_back(w);
    }
}

WindowBlackmanNuttall::WindowBlackmanNuttall( int size ):BaseWindow( size, 0.0 )
{
    double w(0.0);
    for( int i = 0; i < size; i++ )
    {
        w = (   0.3635819
              - 0.4891775 * cos (2.0*C_PI*(double)i/(double)(size-1))
              + 0.1365995 * cos (4.0*C_PI*(double)i/(double)(size-1))
              - 0.0106411 * cos (6.0*C_PI*(double)i/(double)(size-1)) );
        //qDebug() << w;
        m_w.push_back(w);
    }
}

WindowParzen::WindowParzen( int size ):BaseWindow( size, 0.0 )
{
    double w(0.0);
    for( int i = 0; i < size; i++ )
    {
        w = (1.0 - fabs (((double)i-0.5*(double)(size-1)) /(0.5*(double)(size+1))));
        //qDebug() << w;
        m_w.push_back(w);
    }
}

WindowTriangular::WindowTriangular( int size ):BaseWindow( size, 0.0 )
{
    double w(0.0);
    double nominator(0.0);
    double denominator(0.0);
    for( int i = 0; i < size; i++ )
    {
        nominator = (double) i- (double) (size-1) *0.5;
        denominator = (double) size * 0.5;
        w = 1 - abs( nominator / denominator);
        m_w.push_back(w);
        //qDebug() << w;
    }
}

WindowTukey::WindowTukey( int size ):BaseWindow( size, 0.0 )
{

}

WindowWelch::WindowWelch( int size ):BaseWindow( size, 0.0 )
{
    double w(0.0);
    for( int i = 0; i < size; i++ )
    {
        w = (1.0-(((double)i-0.5*(double)(size-1))/ (0.5*(double)(size+1)))
             *(((double)i-0.5*(double)(size-1)) / (0.5*(double)(size+1))));
        m_w.push_back(w);
        //qDebug() << w;
    }
}

WindowLanczos::WindowLanczos( int size ):BaseWindow( size, 0.0 )
{

}
