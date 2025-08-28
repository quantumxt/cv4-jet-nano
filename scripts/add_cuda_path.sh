#!/bin/bash

#Add CUDA path to bashrc
CUDA_PATH_COMMENT='# === CUDA path ==='
CUDA_VERSION=$(nvidia-smi | grep "CUDA Version" | awk '{print $9}')
CUDA_PATH="/usr/local/cuda-$CUDA_VERSION"

if grep -q "$CUDA_PATH_COMMENT" ~/.bashrc; then
    if grep -q "$CUDA_PATH/bin" ~/.bashrc; then
        echo "CUDA Path has already been added!"
	exit 1
    fi
fi

echo -e "<< Adding CUDA paths to .bashrc: $CUDA_PATH"
echo -e "\n$CUDA_PATH_COMMENT" >> ~/.bashrc
echo -e "export PATH=$CUDA_PATH/bin\${PATH:+:\$PATH}" >> ~/.bashrc
echo -e "export LD_LIBRARY_PATH=$CUDA_PATH/lib64\${LD_LIBRARY_PATH:+:\$LD_LIBRARY_PATH}" >> ~/.bashrc
echo '<< Checking if nvcc could be found'
source ~/.bashrc
if command -v nvcc &> /dev/null; then
    echo -e ">> nvcc found!\n"
    nvcc --version
else
    echo ">> nvcc not found..."
fi
