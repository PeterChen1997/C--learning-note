# OOP OOD

Object oriented programming/design

## 复合 Composition

has-a

### 复合下的构造与析构

- 构造由内而外
- 析构由外而内

```c++
// 构造
Container::Container()
  : Component() {}

// 析构
Container::~Container()
{
  ...
  ~Component();
}
```

## 委托 Delegation

Composition by reference

## 继承 Inheritance

### 虚函数

希望派生类重新定义它

> 纯虚函数：派生类必须重新定义 virtual void func = 0