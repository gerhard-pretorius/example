/*

~/opt/bin/protoc --cpp_out=. mtype.proto header.proto

clang++ -std=c++17 -I ~/opt/include -L ~/opt/lib \
typegen.cc mtype.pb.cc -o typegen \
-lprotobuf -lprotoc -lpthread

~/opt/bin/protoc --plugin=protoc-gen-typegen=./typegen --cpp_out=. \
--typegen_out=. msg3.proto 

clang++ -std=c++17 -I ~/opt/include -L ~/opt/lib \
mtype.pb.cc header.pb.cc msg3.pb.cc msg3.cc \
-lprotobuf -lpthread

*/

#include <iostream>
#include <iomanip>
#include <string>

#include "header.pb.h"
#include "msg3.pb.h"


template <typename T>
auto makeHeader(const T& msg, int size)
{
  Header h;
  h.set_magic(0xaaba00f0);        // magic cookie
  h.set_length(size);
  h.set_mtype(T::msgType());
  h.set_filler(1);

  return h;
}

using namespace std;
int main(int argc, char *argv[])
{
  Foo foo;
  foo.set_value(42);

  Bar bar;
  bar.set_name("bar");

  auto fs = foo.SerializeAsString();
  auto h1 = makeHeader(foo, fs.size());

  cout << h1.SerializeAsString();

  return 0;
}
