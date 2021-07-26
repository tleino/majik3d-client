dnl Process this file with autoconf to produce a configure script.

AC_INIT([majik3d], [0.4.0])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CANONICAL_HOST
AC_DEFINE_UNQUOTED(CPU_VENDOR_OS, "$host")
AM_INIT_AUTOMAKE

dnl Checks for programs.
PKG_PROG_PKG_CONFIG

dnl Check dependencies
PKG_CHECK_MODULES([X11], [x11])
PKG_CHECK_MODULES([GL], [gl])
PKG_CHECK_MODULES([GLU], [glu])
PKG_CHECK_MODULES([GLUT], [freeglut],,
	[PKG_CHECK_MODULES([GLUT], [glut],,
			[PKG_CHECK_MODULES([GLUT], [libglut])]
		)
	]
)

dnl Check PLIB, which does not have pkg-config files
dnl You may need to tweak this manually...
AC_SUBST([PLIB_CFLAGS])
AC_SUBST([PLIB_LIBS])
PLIB_CFLAGS="-I/usr/local/include"
PLIB_LIBS="-I/usr/local/lib -lplibssg -lplibpu -lplibfnt -lplibsg -lplibul"

AC_PROG_CC
AC_PROG_CPP
AC_PROG_CXX
AC_PROG_CXXCPP
AC_PROG_INSTALL

dnl Checks for header files.
AC_CHECK_HEADER(windows.h, AC_DEFINE(WIN32))

if test "x$ac_cv_header_windows_h" != "xyes" ; then

  dnl Check for X11
  
  AC_PATH_XTRA
  
  x_suffix="$X_LIBS $X_PRE_LIBS -lX11 -lXi -lXext -lXmu $X_EXTRA_LIBS -lm"
  
    dnl Reasonable stuff non-windoze variants ... :-)
	
	AC_CHECK_LIB(GLcore, glNewList,,,$x_suffix)
	
	if test "x$ac_cv_lib_GLcore_glNewList" = "xno" ; then
	  dnl if no GLcore, check for GL
	  AC_CHECK_LIB(GL, glNewList,,,$x_suffix)
	  if test "x$ac_cv_lib_GL_glNewList" = "xno" ; then
	    dnl if no GL, check for MesaGL
	    AC_CHECK_LIB(MesaGL, glNewList,,,$x_suffix)
	  fi
	else
	  dnl if GLcore found, then also check for GL
	  AC_CHECK_LIB(GL, glXCreateContext,,,$x_suffix)
	fi
	
	dnl if using mesa, check for xmesa.h
	if test "x$ac_cv_lib_MesaGL_glNewList" = "xyes" ; then
	  AC_CHECK_HEADER(GL/xmesa.h)
	  AM_CONDITIONAL(ENABLE_XMESA_FX, \
	    test "x$ac_cv_header_GL_xmesa_h" = "xyes")
	else
	  dnl force a failed check
	  AM_CONDITIONAL(ENABLE_XMESA_FX, test "no" = "yes")
	fi
	
	AC_CHECK_LIB(GLU, gluLookAt,,,$x_suffix)
	if test "x$ac_cv_lib_GLU_gluLookAt" = "xno" ; then
	  dnl if no GLU, check for MesaGLU
	  AC_CHECK_LIB(MesaGLU, gluLookAt,,,$x_suffix)
	fi
	
	opengl_LIBS="$LIBS $x_suffix"
	
	dnl check for glut
	AC_CHECK_LIB(glut, glutGetModifiers,,,$opengl_LIBS)
	
	LIBS="$plib_suffix $LIBS $x_suffix"
	else
	dnl CygWin under Windoze.
	
	LIBS="$plib_suffix $LIBS -lm -lwinmm -lglut32 -lglu32 -lopengl32 -luser32 -lgdi32"
	fi

AC_CHECK_LIB(resolv, inet_aton)

if test "x$ac_cv_lib_resolv_inet_aton" = "xyes" ; then
  dnl This is probably a Solaris machine...
  LIBS="$LIBS -lresolv"
fi

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
m4_unquote(
  _m4_defn([_m4_wrap_text])_m4_popdef([_m4_wrap_text]))
