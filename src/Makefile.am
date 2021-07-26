bin_PROGRAMS = majik
majik_SOURCES = \
mcConfig.cpp mcConfig.hpp mcDebug.cpp mcDebug.hpp mcDisplay.cpp mcDisplay.hpp \
mcError.cpp mcError.hpp mcInput.cpp mcInput.hpp mcLandscape.cpp \
mcLandscape.hpp mcObject.cpp mcObject.hpp mcProtocol.cpp mcProtocol.hpp \
mcSocket.cpp mcSocket.hpp mcPerlin.cpp mcPerlin.hpp mcMenu.cpp mcMenu.hpp \
mcScene.cpp mcMapquad.hpp mcMapquad.cpp mcScene.hpp mcOverlay.cpp \
mcOverlay.hpp mcSky.cpp mcSky.hpp mcTerrainBlock.cpp mcTerrainBlock.hpp \
mcTexGen.cpp mcTexGen.hpp mcTerrainGen.cpp mcTerrainGen.hpp \
mcCameraController.cpp mcCameraController.hpp mcController.cpp \
mcController.hpp mcPlayerController.cpp mcPlayerController.hpp \
mcPlayer.cpp mcPlayer.hpp mcTerrainHeightGen.cpp mcTerrainHeightGen.hpp \
mcCamera.cpp mcCamera.hpp version.hpp main.cpp
majik_CXXFLAGS = \
	$(X11_CFLAGS) \
	$(GL_CFLAGS) \
	$(GLU_CFLAGS) \
	$(GLUT_CFLAGS) \
	$(PLIB_CFLAGS)
majik_LDADD = \
	$(X11_LIBS) \
	$(GL_LIBS) \
	$(GLU_LIBS) \
	$(GLUT_LIBS) \
	$(PLIB_LIBS)
CLEANFILES = *.debug *~
toilet  :
	@-wc *.cpp *.hpp
autodoc	:
	doc++ -S *.cpp *.hpp
	rm -rf doc/*
	mv *.class *.html *.gif doc/
