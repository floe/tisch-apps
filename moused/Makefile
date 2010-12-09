moused: moused.o
	g++ -o $@ $^ -ltools -lsimplecv -lgestures -L ../../libtisch-1.1/build/lib/ -lX11 -lXi -lXtst 

%.o: %.cc
	g++ -DTISCH_SHARED="" -DTISCH_PREFIX="" -c -ggdb -Wall -o $@ $< -I /usr/include/libtisch/

clean:
	-rm *.o moused
