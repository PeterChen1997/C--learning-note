# note

## 数量不定的模板参数

```c++
#include <iostream>
#include <stdlib.h>
using namespace std;

void print()
{
    
}
template <typename T, typename... Types>
void print(const T& firstArg, const Types&... args)
{
    cout << firstArg << endl;
    print(args...);
};

int main() {
    print(7.5, "hello", 11, 42);
    return 0;
}
```