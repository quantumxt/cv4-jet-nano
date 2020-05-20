#!/bin/bash

#********************************************************************
#
# CUDA Sample compilation Script
# Version 1.0.2
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

# Check if script is running as root
if [[ $(/usr/bin/id -u) -ne 0 ]]; then
    echo "Please run script as root!"
    exit
fi
# Copy samples if dir not found
if [ -d "$HOME/cuda_samples" ] ; then
	echo 'cuda_samples dir found!'
else
	echo 'Copying samples to home dir...'
	cp -r /usr/local/cuda/samples ~/cuda_samples
	echo 'Updating dir permissions...'
	chmod 755 ~/cuda_samples
	echo 'Updating child permissions...'
	chmod -Rv 755 ~/cuda_samples
fi
cd ~/cuda_samples
echo 'Compiling samples...'
time make
echo 'Compile time ^'

#Add CUDA path to bashrc
echo 'Adding CUDA paths...'
echo '# Append CUDA tools path' >> ~/.bashrc
echo 'export PATH=/usr/local/cuda-10.2/bin${PATH:+:${PATH}}'>> ~/.bashrc
echo 'export LD_LIBRARY_PATH=/usr/local/cuda-10.2/lib64${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}'>> ~/.bashrc


