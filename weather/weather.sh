#!/bin/bash

export DISPLAY=:0.0
export LD_LIBRARY_PATH=$(pwd)/../../tisch-core.git/build/lib/

cd ../../tisch-core.git/build/bin/
./touchd -V -c ../../../tisch-apps.git/weather/kinect.xml > /dev/null & disown
sleep 10
./calibd & disown

cd ../../../tisch-apps.git/weather
./weather weather2.png weather3.png weather1.png 
