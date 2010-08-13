#!/bin/bash
pgrep -x moused > /dev/null && exit
export LD_LIBRARY_PATH=/home/fardemo/tisch/framework/build/lib/

cd /home/fardemo/tisch/applets/moused
exec ./moused $(xinput list --short | grep "Virtual core pointer" | cut -f2 | cut -d= -f2)

