puzzle: puzzle.o ghostcontainer.o puzzlepiece.o
	g++ -o $@ $^ -ltools -lsimplecv -lsimplegl -lwidgets -lgestures -L ../../libtisch-1.1/build/lib/

%.o: %.cpp
	g++ -DTISCH_SHARED="" -DTISCH_PREFIX="" -c -ggdb -Wall -o $@ $< -I /usr/include/libtisch/

clean:
	-rm *.o puzzle 
