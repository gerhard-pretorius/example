#!/bin/bash
#
# An example of handling options and arguments that's too
# complicated for (only) using getopt.

#==========================================================
shopt -s extglob
PROG=${0}

declare -A CMD_OPTIONS
declare -a CMD_ARGS
declare -A CMD_FUNC
declare -A CMD_HELP

#----------------------------------------------------------
error() { echo -e "Error: ${*}"; }

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
noargs()
{
  echo "Error: Requires at least one none option argument"
  echo "Try ${PROG##*/} help"
}


#----------------------------------------------------------
add_option()
{
  local key=$1 val=$2 tmp
  if [[ ${CMD_OPTIONS[$key]} ]]; then
    tmp=$val
    val=${CMD_OPTIONS[$key]}
    val+=","
    val+=$tmp
  fi
  CMD_OPTIONS+=([$key]=$val)
}

#----------------------------------------------------------
# $1 = optstring to parse short options using getopt
# $2 ... commandline args
parse_options()
{
  local opts=${1}; shift
  local i tmp key val

  while (($#))
  do
    OPTIND=0; i=1
    case $1 in
      --*)  tmp=${1#--}; key=${tmp/=*/}; val=${tmp/$key=/}
            add_option $key $val
      ;;

      -*) if getopts "$opts" key ${*}; then
            add_option $key $OPTARG
            let i=${OPTIND}-1
          fi
      ;;

      *)  CMD_ARGS+=(${1}); ;;
    esac
    shift $i
  done
}

#==========================================================

# example variables set via global options
declare -i NOTHING=0      # -n
SOMETHING=""              # -s

#----------------------------------------------------------
help()
{
  local cmd=$2

  if [[ $# == 2 ]]; then
    if [[ ${CMD_HELP[$cmd]} ]]; then
      ${CMD_HELP[$cmd]}
      exit 1
    fi
  fi

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cat << eof
Usage: ${PROG##*/} [options] <command> [args ...] [options] [args ...]
An example of using options and arguments.
Executes the specified command.

Commands:
  foo
  bar
  help

Try:
  ${PROG##*/} help <command>

Options
  -n
  -s        whatever

Examples: 
  ${PROG##*/} help foo
  ${PROG##*/} foo 1 2 3 -s one -n -s two
  ${PROG##*/} -n bar 1 2 3 --src=tmp --dst=log -e one -e two 4 5 6

eof
}


#----------------------------------------------------------
# example handling global options (by setting a global variables)
global_options()
{
  local key
  for key in ${!CMD_OPTIONS[@]}
  do
    case $key in
      n) NOTHING=1; ;;
      s) SOMETHING=${CMD_OPTIONS[s]}; ;;

    # add other global options here
    esac
  done
}


#----------------------------------------------------------
# example command and matching helper
foo()
{
  echo "$*"
  echo "global options: NOTHING=$NOTHING SOMETHING=$SOMETHING"
}

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
foo_help()
{
cat << eof
Usage: ${PROG##*/} foo [args ...]
foo prints it's arguments and global options.

Examples: 
  ${PROG##*/} foo 1 2 3 -s one -n -s two
eof
}


#----------------------------------------------------------
# example command and matching helper
bar()
{
  local key

  # required
  if [[ ! ${CMD_OPTIONS[src]} ]]; then
    error "Requires --src"
    help
    exit 1
  fi

  echo "src=${CMD_OPTIONS[src]}"

  # optional
  for key in ${!CMD_OPTIONS[@]}
  do
    case $key in
      dst) echo "dst=${CMD_OPTIONS[dst]}"; ;;
        e) echo "e=${CMD_OPTIONS[e]}";  ;;
    esac
  done

  echo "$*"
}

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bar_help()
{
cat << eof
Usage: ${PROG##*/} bar --src=source [dst=dest] [-e x] [args ...]
bar prints it's options and arguments.

Options:
  --src=path/to/source [required]
  --dst=path/to/destination [optional]
  -e whatever [optional]

Examples: 
  ${PROG##*/} -n bar 1 2 3 --src=tmp --dst=log -e one -e two 4 5 6
eof
}


###########################################################
# main starts here

# configure the commands and helpers
CMD_FUNC=( [foo]=foo [bar]=bar [help]=help )
CMD_HELP=( [foo]=foo_help [bar]=bar_help )

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
parse_options "ne:s:" ${*}

if [[ ${#CMD_ARGS[*]} == 0 ]]; then
  noargs
  exit 1
fi

if [[ ! ${CMD_FUNC[${CMD_ARGS[0]}]} ]]; then
  error "Unknown command (${CMD_ARGS[0]})"
  help
  exit 1
fi

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
global_options
${CMD_FUNC[${CMD_ARGS[0]}]} "${CMD_ARGS[@]}"
