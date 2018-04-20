# Note

## 类的声明

1. 防卫式声明
1. 构造函数使用初值列，初始化私有变量
1. 函数前加const，代表不改动数据

## 堆内存与栈内存

- 栈内存：存在于某作用域的一款内存空间(离开作用域会自动清理)
- 堆内存：由操作符new提供的一块global空间，动态分配获得，需要delete

> static object生命在作用域结束后仍然存在，直到整个程序结束

## new操作

先分配memory,在调用ctor

```c++
Complex* pc = new Complex(1, 2);

// 转化为
Complex *pc;

// 新建内存空间 （malloc）
void* mem = operator new(sizeof(Complex));
// 转型
pc = static_cast<Complex*>(mem);
// 构造函数
pc->Complex::Complex(1, 2);
```

## delete操作

先调用dtor,在释放memory

```c++
String* ps = new String("hello");
delete ps;

// 转换为
// 析构函数
String::~String(ps);
// 释放内存(free(ps))清除指针
operator delete(ps);
```