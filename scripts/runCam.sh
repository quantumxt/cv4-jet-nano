#!/bin/bash

#********************************************************************
#
# Jetson Camera Test Script
# Version 1.0.3
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

#Default var
WIDTH=3280
HEIGHT=2464
FLIP=0
DUAL=0

#Usage
usage() { printf "\nUsage: $ $0 -w IMG_INP_WIDTH -h IMG_INP_HEIGHT -f FLIP_BY_90 -d \n\nFLAGS\n\t-w IMG_INP_WIDTH: \tSet width of Camera Input Image\n\t-h IMG_INP_HEIGHT: \tSet height of Camera Input Image\n\t-f FLIP_BY_90:\t\tFlip image by n*90 degrees, as specified in FLIP_BY_90\n\t-d:\t\t\tDisplay image from both cameras \n\n" $0; exit 1; }

#(w)idth, (h)eight, (f)lip, (d)ual camera
# colon in optstring means values required for flag
# Colon infront (:w:h..) silences unexpected flag
while getopts w:h:f:d option
do 
	case "${option}"
		in
		w) #Set Width of input img
		WIDTH=${OPTARG}
		echo "Setting image width: $WIDTH"
		;;
		h) #Set height of input img
		HEIGHT=${OPTARG}	
		echo "Setting image width: $HEIGHT"
		;;

		f) #Flip image by n degrees
		FLIP=${OPTARG}
		ANG=$(expr $FLIP \* 90)
		echo "Flipping image by $ANG deg"	
		;;
		d) #Display image feed from both camera
		DUAL=1
		echo "Configuring as Dual Camera"
		;;
		?) usage
		;;
	esac
done

echo "Opening 1st Camera..."
#Display Camera 1
gnome-terminal --tab -- gst-launch-1.0 nvarguscamerasrc sensor_id=0 ! "video/x-raw(memory:NVMM),width=$WIDTH, height=$HEIGHT, framerate=21/1, format=NV12" ! nvvidconv flip-method=$FLIP ! 'video/x-raw,width=720, height=480' ! nvvidconv ! nvegltransform ! nveglglessink -e
#Open 2nd camera if dual arg is specified -> $ runCam.sh dual
if [ $DUAL -eq 1 ] 
then
    echo "Opening 2nd Camera..."
    gnome-terminal --tab -- gst-launch-1.0 nvarguscamerasrc sensor_id=1 ! "video/x-raw(memory:NVMM),width=$WIDTH, height=$HEIGHT, framerate=21/1, format=NV12" ! nvvidconv flip-method=$FLIP ! 'video/x-raw,width=720, height=480' ! nvvidconv ! nvegltransform ! nveglglessink -e
fi

