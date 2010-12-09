tischroaches: Tischroaches.o Roach.o EllipseHabitat.o SquareHabitat.o Habitat.o
	g++ -O2 -o $@ *.o -ltools -lsimplecv -lsimplegl -lwidgets -lgestures -L ../../libtisch-1.1/build/lib/

%.o: %.cpp
	g++ -O2 -DTISCH_SHARED="" -DTISCH_PREFIX="" -c -ggdb -Wall -o $@ $< -I /usr/include/libtisch/

clean:
	-rm *.o tischroaches
