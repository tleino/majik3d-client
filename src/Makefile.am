bin_PROGRAMS = majik
majik_SOURCES = \
Config.cpp Config.hpp Debug.cpp Debug.hpp Detail.hpp Display.cpp Display.hpp \
Error.cpp Error.hpp Keyboard.cpp Keyboard.hpp Landscape.cpp Landscape.hpp \
Majik.hpp Matrix.cpp Matrix.hpp Object.cpp Object.hpp P3D.cpp P3D.hpp \
Protocol.cpp Protocol.hpp SDLMesa.cpp SDLMesa.hpp Socket.cpp Socket.hpp \
main.cpp
INCLUDES = -I. -I/usr/include/GL
CLEANFILES = *.debug
LDADD = -L/usr/X11R6/lib -L/usr/local/lib \
-lX11 -lXext -lXmu -lXt -lXi -lSDL -lMesaGL -lSDL -lICE -lglut -lMesaGLU \
-lpthread -ldl -lm
toilet  :
	@-wc *.cpp *.hpp
