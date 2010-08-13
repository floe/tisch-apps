beatring: BeatSensor.o MIDIOutput.o beatring.o 
	g++ -o $@ $^ -ltools -lsimplecv -lsimplegl -lwidgets -lgestures -L../../libtisch-1.1/build/lib/ -lasound

%.o: %.cc
	g++ -DTISCH_SHARED="" -DTISCH_PREFIX="" -c -ggdb -Wall -o $@ $< -I../../libtisch-1.1/build/include/libtisch/

clean:
	-rm *.o beatring
