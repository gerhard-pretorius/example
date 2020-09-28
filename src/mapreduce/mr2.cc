
/*
clang++ -std=c++20 mr2.cc -lpthread
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
  int i = 1;
  if (std::ifstream f{fname}; f.good())
  {
    std::string s;
    f >> s;
    while (f.good()) {
      m.emplace(s,1);
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
int main()
{
  auto f1 = std::async(std::launch::async, &map, "./rep1.txt");
  auto f2 = std::async(std::launch::async, &map, "./rep2.txt");

  auto m1 = f1.get();
  auto m2 = f2.get();

  auto f3 = std::async(std::launch::async, &reduce, m1);
  auto f4 = std::async(std::launch::async, &reduce, m2);

  auto r1 = f3.get();
  auto r2 = f4.get();

  auto j1 = combine(r1, r2);
  auto r3 = reduce(j1);
  std::cout << r3 << std::endl;

  return 0;
}
