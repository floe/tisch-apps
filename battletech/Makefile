battletech: menu.o damageResolutionMenu.o engine.o selectioncircle.o gamefield.o gametile.o infoMenu.o mainMenu.o menu.o menubutton.o player.o playerMenu.o textures.o unit.o unitDataContainer.o unitListMenu.o unitSelectionMenu.o unittile.o victoryMenu.o warningMenu.o xmlParser/xmlParser.o battletech.o
	g++ -o $@ $^ -ltools -lsimplegl -lsimplecv -lwidgets -lgispl -L ../../libtisch-3.0/build/lib/

%.o: %.cpp
	g++ -DTISCH_SHARED="" -DTISCH_PREFIX="" -c -ggdb -Wall -o $@ $< -I ../../libtisch-3.0/build/include/libtisch/

clean:
	-rm *.o battletech
