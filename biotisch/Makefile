biotisch: biotisch.o 
	g++ -o $@ $< -loscpack -ltools -lsimplecv -lsimplegl -lwidgets -lgestures -L ../../libtisch-2.0/build/lib/

%.o: %.cc
	g++ -DTISCH_SHARED="" -DTISCH_PREFIX="" -c -ggdb -Wall -o $@ $< -I ../../libtisch-2.0/build/include/libtisch/

clean:
	-rm *.o biotisch 
