
#include <iostream>
#include <algorithm>
#include <array>
#include <vector>

//===================================================================
template<typename T>
void print(const T& t)
{
  for (auto &x : t) { std::cout << x << ", "; };
  std::cout << std::endl;
}

//-------------------------------------------------------------------
template<typename T>
T partition(T lo, T hi)
{
  using std::swap;

  auto pivot = *lo;

  while (true)
  {
    while (*lo < pivot) { ++lo; };
    while (*hi > pivot) { --hi; };

    if (lo >= hi) return hi;

    swap(*lo, *hi);
  }
}

//-------------------------------------------------------------------
template<typename T>
void qsort(T lo, T hi)
{
  if ( lo < hi && (hi - lo) > 1) {
    auto p = partition(lo, hi - 1);
    qsort(lo, p);
    qsort(p + 1, hi);
  }
}

template<typename T> void qsort(T &t) { qsort(t.begin(), t.end()); }
    

//===================================================================
int main()
{
  //std::array<int, 10> a{5, 7, 4, 2, 8, 6, 1, 9, 0, 3};

  std::vector<int> a = { 5, 7, 4, 2, 8, 6, 1, 9, 0, 3};
  print(a);
  qsort(a);
  print(a);

  return 0;
}
