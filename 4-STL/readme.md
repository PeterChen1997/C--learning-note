# STL

standard library 包括 standard template library

## STL六大组件

- 容器 Containers
- 分配器 Allocators
- 算法 Algorithms
- 迭代器 Iterators
- 适配器 Adapters
- 仿函式 Functors

```c++
#include <vector>
#include <algoithm>
#include <functional>
#include <iostream>

using namespace std;

int main()
{
  int ia[6] = {27, 210, 12, 47, 109};
  vector<int, allocator<int>> vi(ia, ia+6);

  cout << count_if(vi.begin(), vi.end(), not1(bind2nd(less<int>(), 40)));
}
```

### 容器类型

- 序列容器(Array Vector Deque List Forward-List)
- 关系容器(Set/Multiset Map/Multimap)
- 无序容器(Unordered Set/Multiset Unordered map/Multimap)