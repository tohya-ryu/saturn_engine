include config.mk
MSG = make 
NAME = demo
# linux
ifeq ($(filter linux,$(MAKECMDGOALS)),linux)
LIB = `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_mixer \
	  /home/$(USER)/bin/mruby-2.0.1/build/host/lib/libmruby.a -lm
INC = -I/home/$(USER)/bin/mruby-2.0.1/include
CC = gcc
RUN = -o $(NAME)
MSG += linux
endif

# windows 64
ifeq ($(filter win64,$(MAKECMDGOALS)),win64)
LIB = -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer \
	  -L/home/$(USER)/lib/SDL2-2.0.9/x86_64-w64-mingw32/lib \
	  -L/home/$(USER)/lib/SDL2_image-2.0.4/x86_64-w64-mingw32/lib \
	  -L/home/$(USER)/lib/SDL2_mixer-2.0.4/x86_64-w64-mingw32/lib \
	  /home/$(USER)/bin/mruby-2.0.1/build/win64/lib/libmruby.a -lm
INC = -I/home/$(USER)/lib/SDL2-2.0.9/x86_64-w64-mingw32/include/SDL2 \
	  -I/home/$(USER)/lib/SDL2_image-2.0.4/x86_64-w64-mingw32/include/SDL2 \
	  -I/home/$(USER)/lib/SDL2_mixer-2.0.4/x86_64-w64-mingw32/include/SDL2 \
      -I/home/$(USER)/bin/mruby-2.0.1/include -lws2_32
CC = x86_64-w64-mingw32-gcc
RUN = -o $(NAME).exe
SFLAG = -D_WIN32
MSG += win64
endif

# windows 32
ifeq ($(filter win32,$(MAKECMDGOALS)),win32)
LIB =
INC =
CC = i686-w64-mingw32-gcc
RUN = -o $(NAME).exe
SFLAG = -D_WIN32
MSG += win32
endif

# platform independent

OBJ = src/main.c src/scene_root.c src/scene_title.c src/scene_load.c
INC += -Iinclude
ifeq ($(filter dev,$(MAKECMDGOALS)),dev)
CFLAG = -g -Wall -Wno-switch -Wno-unused-label
SFLAG += -D_DEBUG
MSG += dev
endif

ifeq ($(filter macro,$(MAKECMDGOALS)),macro)
	#CFLAG += -E
	RUN = -E $(NAME)
endif

#linux: $(OBJ) // does not recompile when header files change
linux win64:
	@echo $(MSG)
	$(CC) $(CFLAG) $(RUN) $(SFLAG) $(OBJ)  $(LIB) $(INC)

dev:
	@echo Compiled in debug mode.

macro:
	@echo Wrote preprocessor output to stdout




#release:
#	gcc -Wall -o game $(SRC) `sdl2-config --cflags --libs` $(ADD_LIBS)
