/*

~/opt/bin/protoc --cpp_out=. mtype4.proto header4.proto

clang++ -std=c++17 -I ~/opt/include -L ~/opt/lib \
typegen4.cc mtype4.pb.cc -o typegen \
-lprotobuf -lprotoc -lpthread

~/opt/bin/protoc --plugin=protoc-gen-typegen=./typegen --cpp_out=. \
--typegen_out=. msg4.proto 

clang++ -std=c++17 -I ~/opt/include -L ~/opt/lib \
mtype4.pb.cc header4.pb.cc msg4.pb.cc msg4.cc \
-lprotobuf -lpthread

*/

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdint>

#include "header4.pb.h"
#include "msg4.pb.h"


template <typename T>
auto makeHeaderV1(const T& msg, int size)
{
  std::int64_t t((size & 0x7fffffL) << 40);
  t += ((T::msgType() & 0x7fff) << 24);
  //t+= (1L << 16);
  t += 0x01090a;

  aprot::Header h;
  h.set_magic(t);
  return h;
}

using namespace std;
int main(int argc, char *argv[])
{
  msg::Foo foo;
  foo.set_value(42);

  msg::Bar bar;
  bar.set_name("bar");

  auto fs = foo.SerializeAsString();
  auto h1 = makeHeaderV1(foo, fs.size());


  cout << h1.SerializeAsString();

  return 0;
}
