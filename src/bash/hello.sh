#!/bin/bash

# Produces markdown with embedded commands or executes those cmds.

#==========================================================
shopt -s extglob
PROG=${0}

#----------------------------------------------------------
declare -i RUNIT=0

# This is Markdown code block tags
CODEBEGIN="\`\`\`"
CODEEND="\`\`\`"

#==========================================================
error() { echo -e "Error: ${*}"; }
errxit() { error ${*}; exit 1; }


#==========================================================
# cat the command with markdown or execute by sourcing it.
run()
{
  if ((${RUNIT}))
  then
    if [[ -t 0 ]]; then ${*}; else source ${*} <(cat); fi
  else
    echo -e "${CODEBEGIN}"
    if [[ -t 0 ]]; then echo "${*}"; else cat; fi
    echo -e "${CODEEND}"
  fi
}

#----------------------------------------------------------
doc()
{
  if ((!${RUNIT}))
  then
    if [[ -t 0 ]]; then echo "${*}"; else cat; fi
  fi
}


#----------------------------------------------------------
help() {
cat << eof
Produces a markdown document or executes the code-blocks.
Usage: ${PROG##*/} [options]
with options
    -r        Execute the commands.
    -h        Help
Examples:
  ${PROG##*/}
  ${PROG##*/} > hello.md
  ${PROG##*/} -r
eof
}


#==========================================================
check()
{
  doc " Run this command to see if the gcc compiler is available:  "
  run gcc -v || errxit "gcc not found"
}

#----------------------------------------------------------
create()
{
  run <<'eof'
cat > ./hello.c <<-EOF
#include <stdio.h>
int main()
{
  printf("Hello world\n");
  return 0;
}
EOF
eof
}


#----------------------------------------------------------
build()
{
  run gcc -o ./hello ./hello.c || errxit "build() failed"
}

#----------------------------------------------------------
dotest()
{
  doc "Testing the program"
  run ./hello || errxit "Failed to execute the binary."

  doc "Check the binary output"

  run <<eof
  diff -w <(./hello) <(echo "Hello world") \
	|| { error "Failed test - unexpected output."; return 1; }
eof
}


#==========================================================
while getopts "rh" opt
do
  case ${opt} in
    r) RUNIT=1; ;;
    h) help; exit 0; ;;
  esac
done

let n=${OPTIND}-1
shift ${n}

###########################################################
# Start here

doc << "eof"

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

eof


#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
check

doc <<"eof"
Create a file `hello.cc` using your favorite editor or execute the
command below:
eof

create

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
doc "Compile the executable"
build

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
dotest
