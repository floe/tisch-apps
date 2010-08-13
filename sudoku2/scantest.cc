#include <algorithm>
#include <sstream>
#include <deque>
#include <map>
#include <set>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <Scanner.h>

Scanner* scan;

int main() {

	scan = new Scanner( "/org/bluez/hci0", -100 );
	scan->start();

	while (1) {
		sleep(1);
		std::cout << "current device list: " << std::endl;
		DevMap* map = scan->getDevices();
		for (DevMap::iterator it = map->begin(); it != map->end(); it++) {
			std::cout << it->first << ": " << it->second.rssi << std::endl;
		}
		delete map;
	}
}

