
/*

~/opt/bin/protoc --cpp_out=. mtype.proto 

clang++ -std=c++17 -I ~/opt/include -L ~/opt/lib \
typegen.cc mtype.pb.cc -o typegen \
-lprotobuf -lprotoc -lpthread

*/

#include <iostream>
#include <string>
#include <sstream>


#include "google/protobuf/compiler/plugin.h"
#include "google/protobuf/compiler/code_generator.h"
#include "google/protobuf/io/printer.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/descriptor.pb.h"
#include "google/protobuf/message.h"

#include "mtype.pb.h"


namespace proto = google::protobuf;

class TypeGenerator : public proto::compiler::CodeGenerator {
public:

  bool Generate(const proto::FileDescriptor* file,
                const std::string& parameter,
                proto::compiler::GeneratorContext* context,
                std::string* error) const
  {
    auto filename = file->name();
    filename.erase(filename.rfind(".proto"));
    filename += ".pb.h";

    for(int i = 0; i < file->message_type_count(); ++i)
    {
      auto mdes = file->message_type(i);    // DescriptorProto
      std::string scope("class_scope:"); 
      scope += mdes->name();

      auto mtype = mdes->options().GetExtension(msg_type);

      //proto::scoped_ptr<proto::io::ZeroCopyOutputStream>
      auto output(context->OpenForInsert(filename, scope));

      std::ostringstream ss;
      ss << "int32_t static msgType() { return " << mtype << "; }";

      proto::io::Printer printer(output, '$');
      printer.PrintRaw(ss.str());
      if (printer.failed()) {
        *error = "typegen detected write error.";
        return false;
      }
    }

    return true;
  }


};


int main(int argc, char* argv[])
{
  TypeGenerator generator;
  return google::protobuf::compiler::PluginMain(argc, argv, &generator);
}
