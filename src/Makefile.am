bin_PROGRAMS = majik
majik_SOURCES = \
Config.cpp Config.hpp Debug.cpp Debug.hpp Display.cpp Display.hpp \
Error.cpp Error.hpp Input.cpp Input.hpp Landscape.cpp Landscape.hpp \
Object.cpp Object.hpp  Protocol.cpp Protocol.hpp Socket.cpp Socket.hpp \
Perlin.cpp Perlin.hpp Menu.cpp Menu.hpp Scene.cpp Mapquad.hpp Mapquad.cpp \
Scene.hpp Overlay.cpp Overlay.hpp main.cpp
INCLUDES=-I/usr/local/plib/include
CLEANFILES = *.debug *~
toilet  :
	@-wc *.cpp *.hpp
autodoc	:
	doc++ -S *.cpp *.hpp
	rm -rf doc/*
	mv *.class *.html *.gif doc/
