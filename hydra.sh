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

if [ -d "./build" ]; then
    cd build
    #if [ -d "./lab1Input" ]; then
    #    rm -r lab1Input
    #fi
else
    mkdir build
    cd build
fi

cmake ..
make

mv famtree ../lab1

#cd ../$1

#if [ -d "./input" ]; then
#    cp -r ./input ../build
#    cd ../build
#    mv input lab1Input
#fi
