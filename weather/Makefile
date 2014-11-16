weather: weather.o 
	g++ -o $@ $< -ltools -loscpack -lsimplecv -lsimplegl -lwidgets -lgispl -lGL -lGLU -L ../../libtisch-3.0/build/lib/

%.o: %.cc
	g++ -DTISCH_SHARED="" -DTISCH_PREFIX="" -c -ggdb -Wall -o $@ $< -I ../../libtisch-3.0/build/include/libtisch/

clean:
	-rm *.o weather
