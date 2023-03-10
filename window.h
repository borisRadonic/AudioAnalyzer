#ifndef WINDOW_H
#define WINDOW_H

#include<QVector>

class BaseWindow
{
public:

    enum Window
    {
        Rectangular     = 0,        
        FlatTop         = 1,
        Hamming         = 2,
        Hann            = 3,
        BartlettHann    = 4,
        Blackman        = 5,
        BlackmanHarris  = 6,
        Nuttall         = 7,
        BlackmanNuttall = 8,
        Parzen          = 9,
        Triangular      = 10,
        Tukey           = 11,
        Welch           = 12,
        Lanczos         = 13,
        Chebyshev       = 14,
        Kaiser          = 15,
    };

    BaseWindow( int , double ) {}

    virtual ~BaseWindow() {}

    inline int length()
    {
        return m_w.length();
    }

    double w( int i );

protected:
    QVector<double> m_w;
private:
    BaseWindow(){}
};

class WindowFactory
{
public:
    static BaseWindow* create( BaseWindow::Window window, int size, double attenuation = 0.0 );
private:
    WindowFactory() {}
};

class WindowRectangular : public BaseWindow
{
public:
    WindowRectangular( int size );
    virtual ~WindowRectangular() {}
};

class WindowChebyshev : public BaseWindow
{
public:
    WindowChebyshev( int size, double attenuation );
    virtual ~WindowChebyshev() {}
};

class WindowFlatTop : public BaseWindow
{
public:
    WindowFlatTop( int size );
    virtual ~WindowFlatTop() {}
};

class WindowHamming : public BaseWindow
{
public:
    WindowHamming( int size );
    virtual ~WindowHamming() {}
};

class WindowHann : public BaseWindow
{
public:
    WindowHann( int size );
    virtual ~WindowHann() {}
};

class WindowKaiser : public BaseWindow
{
public:
    WindowKaiser( int size, double attenuation );
    virtual ~WindowKaiser() {}
};

class WindowBartlettHann : public BaseWindow
{
public:
    WindowBartlettHann( int size );
    virtual ~WindowBartlettHann() {}
};

class WindowBlackman : public BaseWindow
{
public:
    WindowBlackman( int size );
    virtual ~WindowBlackman() {}
};

class WindowBlackmanHarris : public BaseWindow
{
public:
    WindowBlackmanHarris( int size );
    virtual ~WindowBlackmanHarris() {}
};

class WindowNuttall : public BaseWindow
{
public:
    WindowNuttall( int size );
    virtual ~WindowNuttall() {}
};

class WindowBlackmanNuttall : public BaseWindow
{
public:
    WindowBlackmanNuttall( int size );
    virtual ~WindowBlackmanNuttall() {}
};

class WindowParzen : public BaseWindow
{
public:
    WindowParzen( int size );
    virtual ~WindowParzen() {}
};

class WindowTriangular : public BaseWindow
{
public:
    WindowTriangular( int size );
    virtual ~WindowTriangular() {}
};

class WindowTukey : public BaseWindow
{
public:
    WindowTukey( int size );
    virtual ~WindowTukey() {}
};

class WindowWelch : public BaseWindow
{
public:
    WindowWelch( int size );
    virtual ~WindowWelch() {}
};

class WindowLanczos : public BaseWindow
{
public:
    WindowLanczos( int size );
    virtual ~WindowLanczos() {}
};




#endif // WINDOW_H
