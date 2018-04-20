#ifndef __MYSTRING__
#define __MYSTRING__

#include <iostream.h>

class String
{
  public:
    String(const char* cstr = 0);
    // 拷贝构造
    String(const String& str);
    // 拷贝赋值
    String& operator = (const String& str);
    // 析构函数
    ~String();

    char* get_c_str() const { return m_data; }
  private:
    char* m_data;
}

inline
String::String(const char* cstr = 0)
{ // 动态分配
  if(cstr) {
    m_data = new char[strlen(cstr) + 1];
    strcpy(m_data, cstr);
  }
  else {
    m_data = new char[1];
    *m_data = '\0';
  }
}

inline
String::String(const String& str)
{ // 拷贝构造
  m_data = new char[ strlen(str.m_data) + 1 ];
  strcpy(m_data, str.m_data);
}

inline
String& String::operator = (const String& str)
{ // 拷贝赋值
  if(this == &str)
  { // 检测自我赋值,不检测的话会删除自己！！
    return *this;
  }
  delete[] m_data;
  m_data = new char[ strlen(str.m_data) + 1];
  strcpy(m_data, str.m_data);
  return *this;
}

inline
String::~String()
{ // new[] 内存回收 需要使用 delete[] 否则内存泄漏
  delete[] m_data;
}

ostream&
operator << (ostream& os, const String& str)
{
  os << str.get_c_str();
  return os;
}


#endif