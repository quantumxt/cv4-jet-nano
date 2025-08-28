#!/bin/bash
# ./compile.sh [proj_dir_name]
MPATH="$(pwd)/$1"
BPATH="$MPATH/build"
printf "Build path: $BPATH\n\n"
if [ ! -f $MPATH/CMakeLists.txt ]; then
	echo "CMakeLists.txt not found! Exiting..."
	exit 0
fi
mkdir -p $BPATH
cd $BPATH
cmake ..
make -j$(nproc)
cd ..
