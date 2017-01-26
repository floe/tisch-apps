sudoku: sudoku.o 
	g++ -o $@ $< -loscpack -ltools -lsimplecv -lsimplegl -lwidgets -lgispl -lGL -lGLU -L ../../tisch-core.git/build/lib/

scantest: scantest.o 
	g++ -o $@ $< -ltools -lsimplecv -lsimplegl -lwidgets -lgispl -L ../../libtisch-1.1/build/lib/

%.o: %.cc
	g++ -DTISCH_SHARED="" -DTISCH_PREFIX="" -c -ggdb -Wall -o $@ $< -I ../../tisch-core.git/build/include/libtisch/

clean:
	-rm *.o sudoku
