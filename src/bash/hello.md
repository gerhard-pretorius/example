
### Markdown with embedded commands

Tired of cut-n-pasting snippets of bash commands into a markdown document ?
Tired of executing those commands to ensure correctness.

This script is an example of using 'literate programming' - sort of -
it mixes code snippets and documentation.

When you execute the script it will produce either a markdown document
with code-blocks or execute those code-blocks instead. 

It is a simple way of ensuring that your markdown documents contains valid
code-blocks.

#### Build hello world

 Run this command to see if the gcc compiler is available:  
```
gcc -v
```
Create a file `hello.cc` using your favorite editor or execute the
command below:
```
cat > ./hello.c <<-EOF
#include <stdio.h>
int main()
{
  printf("Hello world\n");
  return 0;
}
EOF
```
Compile the executable
```
gcc -o ./hello ./hello.c
```
Testing the program
```
./hello
```
Check the binary output
```
  diff -w <(./hello) <(echo "Hello world") 	|| { error "Failed test - unexpected output."; return 1; }
```
