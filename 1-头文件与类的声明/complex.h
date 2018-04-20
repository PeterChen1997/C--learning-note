#ifndef __COMPLEX__
#define __COMPLEX__

// 前置声明
#include <iostream.h>

// 类-声明
class complex
{
  public:
    // 构造函数
    complex (double r = 0, double i = 0)
      : re (r), im (i)
    { }
    // 支持的函数(成员函数)(inline-function)
    complex& operator += (const complex&);
    double real() const { return re; }
    double imag() const { return im; }

  private:
    double re, im;

    friend  complex& __doapl (complex*, const complex&);
}


// 类-定义
inline complex&
__doapl(complex* ths, const complex& r)
{
  ths -> re += r.re;
  ths -> im += r.im;
  return *ths;
}

inline complex&
complex::operator += (const complex& r)
{
  return __doapl(this, r);
}

inline complex
operator + (const complex& x, const complex& y)
{
  // 返回创建的保存结果的临时对象
  return complex( real(x) + real(y), imag(x) + imag(y);)
}

inline complex
operator + (const complex& x, double y)
{
  return complex( real(x) + y, imag(x);)
}

inline complex
operator + (double x, const complex& y)
{
  return complex( x + real(y), imag(y);)
}

// 操作符重载
ostream&
operator << (ostream& os, const complex& x)
{
  return os << '(' << real(x) << imag(x) << ')';
}

#endif