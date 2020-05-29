#!/bin/bash
# ./compile.sh [proj_dir_name]
BPATH="$(pwd)/$1/build"
echo $BPATH
mkdir -p $BPATH
cd $BPATH
cmake ..
make -j4
cd ..
