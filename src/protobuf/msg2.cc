/* Example Google Protocolbuffers TextFormat

~/opt/bin/protoc --cpp_out=. msg2.proto
clang++ -std=c++17 -I ~/opt/include msg2.pb.cc msg2.cc -L ~/opt/lib -lprotobuf -lpthread

./a.out | xxd -p
./a.out | ~/opt/bin/protoc --decode=Foo msg2.proto

*/

#include <iostream>
#include <iomanip>
#include <string>

#include "google/protobuf/message.h"

#include "msg2.pb.h"


namespace proto = google::protobuf;

int mytypeid(const proto::Message &msg)
{
  return msg.GetDescriptor()->field(0)->options().GetExtension(my_field);
  //return msg.GetDescriptor()->options().GetExtension(my_type);
}


template<typename T>
int mytypeid2(const T &msg)
{
  return msg.GetDescriptor()->options().GetExtension(my_type);
}

using namespace std;
int main(int argc, char *argv[])
{
  auto footype = Foo::descriptor()->options().GetExtension(my_type);
  auto bartype = Bar::descriptor()->options().GetExtension(my_type);

  Foo foo;
  foo.set_value(42);
  //cout << foo.SerializeAsString();

  Bar bar;
  //cout << bar.SerializeAsString();

  cout << mytypeid2(foo) << endl;
  cout << mytypeid2(bar) << endl;

  return 0;
}
