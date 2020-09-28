
/*
clang++ -std=c++20 mr3.cc -lpthread
*/


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <future>
#include <thread>
#include <stdexcept>


//=========================================================
using WMap = std::multimap<std::string, int>;

std::ostream &operator<<(std::ostream &os, const WMap &m)
{
  for (const auto &s: m)
    os << s.first << '=' << s.second << ',';
  return os;
}


//=========================================================
auto map(const char *fname)
{
  WMap m;
  if (std::ifstream f{fname}; f.good())
  {
    std::string s;
    f >> s;

    while (f.good())
    {
      if (auto found = m.find(s); found != m.end()) {
        ++found->second;
      }
      else {
        m.emplace(s, 1);
      }
      f >> s;
    }
  }
  else {
    throw std::runtime_error("file not found");
  }

  return m;
}


//=========================================================
auto reduce(const WMap &m)
{
  WMap c;
  for_each(m.begin(), m.end(),
    [&](const auto &kv) {
      if (auto found = c.find(kv.first); found != c.end()) {
        found->second += kv.second;
      }
      else {
        c.emplace(kv.first, kv.second);
      }
    }
  );
  return c;
}


//=========================================================
auto combine(const WMap &m1, const WMap &m2)
{
  WMap c(m1);
  c.insert(m2.begin(), m2.end());

  return c;
}

//=========================================================
template <class F, class... A>
auto task(F&& f, A&&... a)
{
  return std::async(std::launch::async, f, std::forward<A>(a)...);
}


//=========================================================
int main()
{
  auto mapreduce = [](const char *n1, const char *n2)
  {
    auto f1 = task(&map, n1);
    auto f2 = task(&map, n2);
    return reduce(combine(f1.get(), f2.get()));
  };

  auto f1 = task(mapreduce, "./rep1.txt", "./rep2.txt");
  auto f2 = task(mapreduce, "./rep3.txt", "./rep4.txt");

  auto r3 = reduce(combine(f1.get(), f2.get()));
  std::cout << r3 << std::endl;

  return 0;
}
