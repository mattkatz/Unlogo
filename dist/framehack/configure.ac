AC_INIT([Framehack], [0.1], [jeff@crouse.cc], [framehack], [http://jeffcrouse.info])
AC_PREREQ([2.59])
AM_INIT_AUTOMAKE([1.10 no-define])

PKG_CHECK_MODULES([OPENCV], [opencv-2.1])
AC_SUBST(OPENCV_CFLAGS)
AC_SUBST(OPENCV_LIBS)

AC_CONFIG_HEADERS([config.h])
AC_PROG_CXX
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
