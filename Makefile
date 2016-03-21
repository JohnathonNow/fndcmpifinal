OBJECTS = final.o control.o gfx_john.o menu.o game.o john_lib.o levelparser.o
FLAGS = -lm -lX11 -Wall -g
final : $(OBJECTS)
	gcc $(OBJECTS) -o final $(FLAGS)
final.o : final.c constants.h
	gcc -c final.c $(FLAGS)
control.o : control.c constants.h
	gcc -c control.c $(FLAGS)
menu.o : menu.c constants.h
	gcc -c menu.c $(FLAGS)
game.o : game.c constants.h
	gcc -c game.c $(FLAGS)
gfx5.o : gfx_john.c constants.h
	gcc -c gfx_john.c $(FLAGS)
john_lib.o : john_lib.c constants.h
	gcc -c john_lib.c $(FLAGS)
levelparser.o : levelparser.c constants.h
	gcc -c levelparser.c $(FLAGS)
clean :
	rm *.o final
