#We use pushdef here because we can't use shell variables before AC_INIT, but we want to use a variable with AC_INIT:
pushdef([MAJOR_VERSION],     [1])
pushdef([MINOR_VERSION],     [0])
pushdef([MICRO_VERSION],     [0])
pushdef([VERSION], MAJOR_VERSION.MINOR_VERSION.MICRO_VERSION)

AC_INIT([qtx], VERSION)
AC_CONFIG_SRCDIR([Widget.C])
AC_PREREQ(2.59)

#########################################################################
#  Version and initialization
#########################################################################
[MAJOR_VERSION]=MAJOR_VERSION
[MINOR_VERSION]=MINOR_VERSION
[MICRO_VERSION]=MICRO_VERSION
[VERSION]=VERSION
popdef([MAJOR_VERSION])
popdef([MINOR_VERSION])
popdef([MICRO_VERSION])
popdef([VERSION])
RELEASE=$MAJOR_VERSION.$MINOR_VERSION
AC_DEFINE_UNQUOTED(MAJOR_VERSION, $MAJOR_VERSION, [Major version of qtx])
AC_DEFINE_UNQUOTED(MINOR_VERSION, $MINOR_VERSION, [Minor version of qtx])
AC_DEFINE_UNQUOTED(MICRO_VERSION, $MICRO_VERSION, [Micro version of qtx])
AC_SUBST(MAJOR_VERSION)
AC_SUBST(MINOR_VERSION)
AC_SUBST(MICRO_VERSION)
AC_SUBST(VERSION)
AC_SUBST(RELEASE)

VERSION_INFO=`expr $MAJOR_VERSION + $MINOR_VERSION`:$MICRO_VERSION:$MINOR_VERSION
AC_SUBST(VERSION_INFO)

dnl AC_CONFIG_AUX_DIR(scripts)

# Initialize automake stuff
# tar-ustar asks it to use a sensible tar format that can handle long filenames.
AM_INIT_AUTOMAKE([1.9 tar-ustar])

dnl Specify a configuration file (no autoheader)
dnl AM_CONFIG_HEADER(config.h)
dnl AM_MAINTAINER_MODE
dnl AL_ACLOCAL_INCLUDE(scripts)


AC_PROG_CC
AC_PROG_CPP
AC_PROG_MAKE_SET
dnl AC_CANONICAL_BUILD
dnl AC_CANONICAL_HOST

AC_PROG_LIBTOOL

#########################################################################
#  Dependency checks
#########################################################################

dnl Check for libsigc++ stuff
PKG_CHECK_MODULES(SIGC, [sigc++-2.0])
AC_SUBST(SIGC_LIBS)
AC_SUBST(SIGC_CFLAGS)

dnl Check for Qt stuff
PKG_CHECK_MODULES(QT, [QtGui])
AC_SUBST(QT_LIBS)
AC_SUBST(QT_CFLAGS)

#########################################################################
#  C++ checks
#########################################################################
AC_PROG_CXX

AC_LANG_CPLUSPLUS



AC_CONFIG_FILES([
  Makefile
  QtX/Makefile
])


AC_OUTPUT()

