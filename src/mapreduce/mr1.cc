
/*
clang++ -std=c++20 mr1.cc
*/


#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>


//=========================================================
using Word = std::vector<std::string>;

std::ostream &operator<<(std::ostream &os, const Word &w)
{
  for (const auto &s: w)
    os << s << ',';
  return os;
}

//=========================================================
using WMap = std::multimap<std::string, int>;

std::ostream &operator<<(std::ostream &os, const WMap &m)
{
  for (const auto &s: m)
    os << s.first << '=' << s.second << ',';
  return os;
}

//=========================================================
auto map(const Word &w)
{
  WMap m;
  for_each(w.begin(), w.end(),
    [&](const auto &s) { m.emplace(s,1); }
  );
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
  Word w1{"hello", "world", "bye", "world"},
       w2{"hello", "hadoop", "goodbye", "hadoop"};


  std::cout << w1 << std::endl;
  std::cout << w2 << std::endl;

  auto m1 = map(w1); 
  auto m2 = map(w2); 
  std::cout << m1 << std::endl << m2 << std::endl;

  auto r1 = reduce(m1);
  auto r2 = reduce(m2);
  std::cout << r1 << std::endl << r2 << std::endl;

  auto c1 = combine(m1, m2);
  std::cout << c1 << std::endl;

  auto r3 = reduce(c1);
  std::cout << r3 << std::endl;

  return 0;
}
