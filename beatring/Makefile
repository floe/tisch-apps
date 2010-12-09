beatring: BeatSensor.o MIDIOutput.o beatring.o 
	g++ -o $@ $^ -ltools -lsimplecv -lsimplegl -lwidgets -lgestures -lasound

%.o: %.cc
	g++ -DTISCH_SHARED="" -DTISCH_PREFIX="" -c -ggdb -Wall -o $@ $< -I/usr/include/libtisch/

clean:
	-rm *.o beatring
