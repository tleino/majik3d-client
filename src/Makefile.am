bin_PROGRAMS = majik
majik_SOURCES = \
Config.cpp Config.hpp Debug.cpp Debug.hpp Detail.hpp Display.cpp Display.hpp \
Error.cpp Error.hpp Input.cpp Input.hpp Landscape.cpp Landscape.hpp \
Matrix.cpp Matrix.hpp Object.cpp Object.hpp P3D.cpp P3D.hpp \
Protocol.cpp Protocol.hpp Socket.cpp Socket.hpp Perlin.cpp Perlin.hpp \
Menu.cpp Menu.hpp Material.cpp Material.hpp main.cpp
INCLUDES = -I/usr/include/GL -I/usr/local/include -I/usr/local/plib/include
CLEANFILES = *.debug
LDADD = -L/usr/X11R6/lib -L/usr/local/lib -L/usr/local/plib/lib \
-lX11 -lXext -lXmu -lXt -lXi -lICE -lglut -lMesaGLU -lMesaGL -lssg -lpu \
-lfnt -lsg -lm
toilet  :
	@-wc *.cpp *.hpp
