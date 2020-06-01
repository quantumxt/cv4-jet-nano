#!/bin/bash
DSTR=$(lsb_release -sc)
printf "OS: $(lsb_release -sd)\nSetting up source...\n"
sudo sh -c 'printf "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
echo "Setup keys..."
sudo apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key C1CF6E31E6BADE8868B172B4F42ED6FBAB17C654
sudo apt update
echo "Installing ROS..."
sudo apt install ros-melodic-desktop -y
printf "\nUpdating .bashrc..."
echo "source /opt/ros/melodic/setup.bash" >> ~/.bashrc
source ~/.bashrc

