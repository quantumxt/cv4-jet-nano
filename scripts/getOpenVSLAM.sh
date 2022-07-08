#!/bin/bash

#********************************************************************
#
# OpenVSLAM Installation Script
# Version 1.0.0
#
# Licensed under the MIT License.
#  __ _  _   ___ ______ ____                    _                   
# /_ | || | / _ \____  / __ \                  | |                  
#  | | || || (_) |  / / |  | |_   _  __ _ _ __ | |_ _   _ _ __ ___  
#  | |__   _> _ <  / /| |  | | | | |/ _` | '_ \| __| | | | '_ ` _ \ 
#  | |  | || (_) |/ / | |__| | |_| | (_| | | | | |_| |_| | | | | | |
#  |_|  |_| \___//_/   \___\_\\__,_|\__,_|_| |_|\__|\__,_|_| |_| |_|
#                              
# Copyright(c) 1487Quantum
#
#********************************************************************

VERSION="1.0.0"
INSTALL_DIR="$pwd"		#Would install in current directory
PNUM=3				#Using 3 CPU to compile

makeSrc () {
time make -j$PNUM
if [ $1 == 2 ]; then
printf "\nInstalling..."
sudo make install
fi
cd ../..
echo "\n>> Entering $(pwd) <<\n"
}

makeDirEnter () {
mkdir -p $1 && cd $1
}

pmsg () {
printf "\n"
printf '=%.0s' $( eval echo {1..${#1}} )
printf "\n$1\n"
printf '=%.0s' $( eval echo {1..${#1}} )
printf "\n"
}

menuSelect () {
local CHR=-1
while [ $CHR -eq -1 ]; do
clear
pmsg "OpenVSLAM Installation Script\nVersion $VERSION"
printf "\n1) Install OpenVSLAM only\n2) Install OpenVSLAM + Pangolin Viewer (Recommended) \n3) Install OpenVSLAM + WebSocket Viewer\n4) Install All (OpenVSLAM + Pangolin Viewer + WebSocket Viewer)\nE) Exit installation\n\n"
read -p "Select option: " CHOICE
case $CHOICE in
	1)
	CHR=1;;
	2)
	CHR=2;;
	3)
	CHR=3;;
	4)
	CHR=4;;
	[Ee]* )
	pmsg "Exiting installation..."
	exit 1;;
	*)
	CHR=
	echo "Invalid option"
esac
done
return $CHR
}


menuSelect		#Show Menu
TMP_MCHR=$?
printf "\n"
MCHR=-1
while [ $MCHR -eq -1 ]; do
read -p "Selected Option ($TMP_MCHR), proceed (Y/N)? " PM1
case $PM1 in
	[Yy]* )
	pmsg "Updating repo..."
	break;; 
	[Nn]* )
	echo "Returning to menu..."
	menuSelect
	TMP_MCHR=$?
	;;
	*)
	MCHR=-1
	printf "Invalid option\n";;
esac
done

sudo apt update -y
sudo apt upgrade -y --no-install-recommends

pmsg "DEPENDENCIES\n\nInstalling main dependencies..."
# basic dependencies
sudo apt install -y build-essential pkg-config cmake git wget curl unzip
# g2o dependencies
sudo apt install -y libatlas-base-dev libsuitesparse-dev
# OpenCV dependencies
sudo apt install -y libgtk-3-dev ffmpeg libavcodec-dev libavformat-dev libavutil-dev libswscale-dev libavresample-dev
# eigen dependencies
sudo apt install -y gfortran
# other dependencies
sudo apt install -y libyaml-cpp-dev libgoogle-glog-dev libgflags-dev

if [ $TMP_MCHR -eq 2 ]  || [ $TMP_MCHR -eq 4 ]; then
pmsg "Installing Pangolin Viewer dependencies..."
# Pangolin dependencies
sudo apt install -y libglew-dev
fi

if [ $TMP_MCHR -eq 3 ]  || [ $TMP_MCHR -eq 4 ]; then
pmsg "Installing Socket Viewer dependencies..."
#  SocketViewer
# Protobuf dependencies
sudo apt install -y autogen autoconf libtool
# Node.js
curl -sL https://deb.nodesource.com/setup_6.x | sudo -E bash -
sudo apt install -y nodejs
fi

makeDirEnter ovslam
pmsg "Getting Eigen Lib..."
wget -q http://bitbucket.org/eigen/eigen/get/3.3.4.tar.bz2
tar xf 3.3.4.tar.bz2
rm -rf 3.3.4.tar.bz2
cd eigen-eigen-5a0156e40feb
makeDirEnter build
pmsg "Making & Compiling Eigen Lib..."
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    ..
makeSrc 2

pmsg "Getting Custom DBoW2..."
git clone https://github.com/shinsumicco/DBoW2.git
cd DBoW2
makeDirEnter build
pmsg "Making & Compiling Custom DBoW2..."
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    ..
makeSrc 2

pmsg "Getting g2o..."
git clone https://github.com/RainerKuemmerle/g2o.git
cd g2o
git checkout 9b41a4ea5ade8e1250b9c1b279f3a9c098811b5a
makeDirEnter build
pmsg "Making & Compiling g2o..."
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DCMAKE_CXX_FLAGS=-std=c++11 \
    -DBUILD_SHARED_LIBS=ON \
    -DBUILD_UNITTESTS=OFF \
    -DBUILD_WITH_MARCH_NATIVE=ON \
    -DG2O_USE_CHOLMOD=OFF \
    -DG2O_USE_CSPARSE=ON \
    -DG2O_USE_OPENGL=OFF \
    -DG2O_USE_OPENMP=ON \
    ..
makeSrc 2

if [ $TMP_MCHR -eq 2 ]  || [ $TMP_MCHR -eq 4 ]; then
pmsg "Getting Pangolin Viewer..."
git clone https://github.com/stevenlovegrove/Pangolin.git
cd Pangolin
git checkout ad8b5f83222291c51b4800d5a5873b0e90a0cf81
makeDirEnter build
pmsg "Making & Compiling Pangolin Viewer..."
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    ..
makeSrc 2
fi

if [ $TMP_MCHR -eq 3 ]  || [ $TMP_MCHR -eq 4 ]; then
pmsg "Getting Protobuf..."
wget -q https://github.com/google/protobuf/archive/v3.6.1.tar.gz
tar xf v3.6.1.tar.gz
cd protobuf-3.6.1
pmsg "Making & Compiling Protobuf..."
./autogen.sh
./configure \
    --prefix=/usr/local \
    --enable-static=no
makeSrc 2

pmsg "Getting SocketViewer..."
git clone https://github.com/shinsumicco/socket.io-client-cpp
cd socket.io-client-cpp
git submodule init
git submodule update
makeDirEnter build
pmsg "Making & Compiling SocketViewer..."
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr/local \
    -DBUILD_UNIT_TESTS=OFF \
    ..
makeSrc 2
fi

# Set VARS
if [ $TMP_MCHR -eq 2 ] || [ $TMP_MCHR -eq 4 ]; then
PV=ON
fi

if [ $TMP_MCHR -eq 3 ] || [ $TMP_MCHR -eq 4 ]; then
SV=ON
fi

pmsg "MAIN INSTALLATION\nCloning OpenVSLAM..."
git clone https://github.com/1487quantum/openvslam
cd openvslam
makeDirEnter build
pmsg "Making & Compiling OpenVSLAM..."
cmake \
    -DBUILD_WITH_MARCH_NATIVE=ON \
    -DUSE_PANGOLIN_VIEWER=$PV \
    -DUSE_SOCKET_PUBLISHER=$SV \
    -DUSE_STACK_TRACE_LOGGER=ON \
    -DBOW_FRAMEWORK=DBoW2 \
    -DBUILD_TESTS=ON \
    ..
makeSrc 0
pmsg "Checking installation..."
cd openvslam/build
./run_kitti_slam -h

pmsg "OpenVSLAM Installation complete!"


