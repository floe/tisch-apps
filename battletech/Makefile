battletech: menu.o damageResolutionMenu.o engine.o selectioncircle.o gamefield.o gametile.o infoMenu.o mainMenu.o menu.o menubutton.o player.o playerMenu.o textures.o unit.o unitDataContainer.o unitListMenu.o unitSelectionMenu.o unittile.o victoryMenu.o warningMenu.o xmlParser/xmlParser.o battletech.o
	g++ -o $@ $^ -ltools -lsimplegl -lsimplecv -lwidgets -lgestures -L ../../libtisch-1.1/build/lib/

%.o: %.cpp
	g++ -DTISCH_SHARED="" -DTISCH_PREFIX="" -c -ggdb -Wall -o $@ $< -I ../../libtisch-1.1/build/include/libtisch/ -I ../../libtisch-1.1/

clean:
	-rm *.o battletech
