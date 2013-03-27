LDFLAGS=-lglut -ltools -loscpack -lsimplecv -lsimplegl -lwidgets -lgestures -L ../../libtisch-2.0/build/lib/

all: siedler dice

siedler: siedler.o 
	g++ -o $@ $< $(LDFLAGS)

dice: dice.o 
	g++ -o $@ $< $(LDFLAGS)

%.o: %.cc
	g++ -DTISCH_SHARED="" -DTISCH_PREFIX="" -c -ggdb -Wall -o $@ $< -I ../../libtisch-2.0/build/include/libtisch/

clean:
	-rm *.o graphedit 
