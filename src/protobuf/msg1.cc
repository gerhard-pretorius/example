/* Example Google Protocolbuffers TextFormat

~/opt/bin/protoc --cpp_out=. msg1.proto
clang++ -std=c++17 -I ~/opt/include msg1.pb.cc msg1.cc -L ~/opt/lib -lprotobuf -lpthread

./a.out | xxd -p
082a


./a.out | ~/opt/bin/protoc --decode=Foo msg1.proto

value: 42

*/

#include <iostream>
#include <iomanip>
#include <string>

#include "msg1.pb.h"

using namespace std;

int main(int argc, char *argv[])
{
  Foo foo;
  foo.set_value(42);

  cout << foo.SerializeAsString();

  return 0;
}
