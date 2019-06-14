CPPFLAGS=$(shell sdl-config --cflags)
LIBS=$(shell sdl-config --libs) -lSDL_image
CPP=gcc
OBJS=main.o world.o plyr.o nmy.o particles.o image.o
NAME=teh_ninja


$(NAME): $(OBJS) 
	$(CPP) -Wall $(OBJS) $(CPPFLAGS) $(LIBS) -o $(NAME)


.PHONY: clean dist

clean:
	rm -f *.o *~ $(NAME)

dist:
	rm -f *.o *~
