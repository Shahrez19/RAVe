#!/bin/bash
# install adafruit motorshield library to appropirate places
if ! id | grep -q root; then
	echo "Must be run as root; exiting." | tee -a .build_log
	exit
fi
if ! uname -o | grep -q Linux; then
	echo "This script should only be run on a linux machine; exiting."
	exit
fi
wget https://github.com/adafruit/Adafruit-Motor-Shield-library/archive/1.0.0.zip
unzip 1.0.0.zip
mv -f Adafruit-Motor-Shield-library-1.0.0/ AFMotor
mv -f AFMotor /usr/share/arduino/libraries/
rm -rf AFMotor
rm -f 1.0.0.zip
echo DONE
