sudoku: sudoku.o 
	g++ -o $@ $< -ltools -lsimplecv -lsimplegl -lwidgets -lgestures -L ../../libtisch-1.1/build/lib/

scantest: scantest.o 
	g++ -o $@ $< -ltools -lsimplecv -lsimplegl -lwidgets -lgestures -L ../../libtisch-1.1/build/lib/

%.o: %.cc
	g++ -DTISCH_SHARED="" -DTISCH_PREFIX="" -c -ggdb -Wall -o $@ $< -I ../../libtisch-1.1/build/include/libtisch/

clean:
	-rm *.o sudoku
