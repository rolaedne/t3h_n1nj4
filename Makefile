CPPFLAGS=$(shell sdl2-config --cflags)
LIBS=$(shell sdl2-config --libs) -lSDL2_image
CPP=gcc
OBJS=main.o world.o plyr.o nmy.o particles.o image.o utils.o controls.o special.o
NAME=teh_ninja


$(NAME): $(OBJS) 
	$(CPP) -Wall $(OBJS) $(CPPFLAGS) $(LIBS) -o $(NAME)


.PHONY: clean dist

clean:
	rm -f *.o *~ $(NAME)

dist:
	rm -f *.o *~
