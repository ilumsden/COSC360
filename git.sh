#!/bin/bash

help()
{
    echo 'This script compiles on non-UT computers using git submodules.'
    echo
    echo 'Usage:'
    echo
    echo './git.sh [Sub-Directory] [Options]'
    echo '    Sub-Directory: specifies which makefile to use.'
    echo '    Options (Not Required):'
    echo '        * -h | --help: prints the usage info'
    echo
}

for CLA in $@; do
    if [ "${CLA}" = "-h" ] || [ "${CLA}" = "--help" ]; then
        help
        exit 0
    fi
done

cd "$( dirname "${BASH_SOURCE[0]}" )"

include="-I${PWD}/libfdr"
linkdir="-L${PWD}"

cd libfdr
make
mv libfdr.a ..
make clean
cd ..

cd $1

make INCLUDE=${include} LINKDIR=${linkdir}
