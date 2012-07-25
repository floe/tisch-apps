#!/bin/bash

export DISPLAY=:0.0
export LD_LIBRARY_PATH=$(pwd)/../../libtisch-2.0/build/lib/

cd ../../libtisch-2.0/build/bin/
./touchd -V -c ../../../applets/weather/kinect.xml > /dev/null & disown
sleep 10
./calibd & disown

cd ../../../applets/weather
./weather weather?.png
