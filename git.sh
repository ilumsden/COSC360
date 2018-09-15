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

build_all=true
build_lab1=false
build_lab2=false

for CLA in $@; do
    case "${CLA}" in
        lab1) build_lab1=true; build_all=false
        ;;
        lab2) build_lab2=true; build_all=false
        ;;
        all) build_all=true
        ;;
        -h|--help) help; exit 0
        ;;
        *) ;;
    esac
done

echo ${build_all}
echo ${build_lab1}
echo ${build_lab2}

cd "$( dirname "${BASH_SOURCE[0]}" )"

if [ -d "./build" ]; then
    cd build
else
    mkdir build
    cd build
fi

cmake_flags=""
cmake_flags+="-DPERSONAL=ON "

if ${build_all}; then
    cmake_flags+="-DALL_LABS=ON "
elif ${build_lab1}; then
    cmake_flags+="-DLAB1=ON "
elif ${build_lab2}; then
    cmake_flags+="-DLAB2=ON "
fi

cmake ${cmake_flags[@]}..
make

cd ../$1
