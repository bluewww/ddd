dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl Copyright (C) 1996-1998 Technische Universitaet Braunschweig, Germany.
dnl Copyright (C) 2001-2003 Free Software Foundation, USA.
dnl Written by Andreas Zeller <zeller@gnu.org>.
dnl 
dnl This file is part of DDD.
dnl 
dnl DDD is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU General Public
dnl License as published by the Free Software Foundation; either
dnl version 2 of the License, or (at your option) any later version.
dnl 
dnl DDD is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
dnl See the GNU General Public License for more details.
dnl 
dnl You should have received a copy of the GNU General Public
dnl License along with DDD -- see the file COPYING.
dnl If not, write to the Free Software Foundation, Inc.,
dnl 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
dnl 
dnl DDD is the data display debugger.
dnl For details, see the DDD World-Wide-Web page, 
dnl `http://www.gnu.org/software/ddd/',
dnl or send a mail to the DDD developers <ddd@gnu.org>.
dnl
dnl
dnl
dnl
dnl
dnl Revision Control.
dnl
AC_PREREQ(2.53)
AC_REVISION([$Id$])
dnl
dnl Zdenek Sekera <zs@cray.com> says that if CDPATH is set, it may break
dnl this script.
unset CDPATH
dnl
dnl
dnl Initialization.
dnl
AC_INIT([NEWS])
AM_CONFIG_HEADER([ddd/config.h])
dnl
dnl Some simple checks.
dnl 
AC_CANONICAL_TARGET
AC_ARG_PROGRAM
DDD_VERSION
AM_INIT_AUTOMAKE(${PACKAGE}, ${VERSION})
AC_PROG_MAKE_SET
dnl
dnl
dnl Check for POSIX stuff (required by AC_FIND_X)
dnl
AC_ISC_POSIX
dnl
dnl
dnl
dnl
dnl Locate programs
dnl
dnl Fortran.  Required to build test program.
dnl Zdenek Sekera <zs@sgi.com> says: Don't use AC_PROG_F77, 
dnl because this aborts construction if f77 does not work.
dnl Lutz Jaenicke <Lutz.Jaenicke@aet.TU-Cottbus.DE> adds that on HP-UX,
dnl f77 will fail with the "-L" linker switch on HP-UX. To get the
dnl BSD compatible f77 options on HP-UX you must call "fort77" instead.
dnl
AC_CHECK_PROGS(F77, g77 fort77 f77 f2c, "g77")
FFLAGS=-g
AC_SUBST(FFLAGS)
dnl
dnl Various programs.
dnl
AC_PROG_LN_S
AC_PATH_PROGS(SH, sh)
AC_CHECK_PROGS(TAR, gtar tar)
AC_CHECK_PROGS(M4, gm4 m4)
AC_CHECK_PROGS(SED, gsed sed)
AC_CHECK_PROGS(DIFF, gdiff diff)
AC_CHECK_PROGS(DVIPS, dvips dvi2ps)
AC_CHECK_PROGS(COMPRESS, gzip compress)
COMPRESS="$COMPRESS -c"
case "$COMPRESS" in
*gzip*) COMPRESS="$COMPRESS -9";;
esac
AC_CHECK_PROGS(UNCOMPRESS, gzip gunzip gzcat zcat uncompress)
case "$UNCOMPRESS" in
*gzip*)       UNCOMPRESS="$UNCOMPRESS -d";;
esac
case "$UNCOMPRESS" in
*zip*)        UNCOMPRESS="$UNCOMPRESS -c";;
*uncompress*) UNCOMPRESS="$UNCOMPRESS -c";;
esac
AC_PATH_PROGS(RSH, remsh rsh on)
AC_CHECK_PROGS(LPR, lp lpr)
if test "$LPR" = "lpr"; then
PRINT_COMMAND="lpr"
elif test "$LPR" = "lp"; then
PRINT_COMMAND="lp -c"
else
PRINT_COMMAND="lp"
fi
AC_SUBST(PRINT_COMMAND)dnl
AC_CHECK_PROGS(GCORE, gcore)
if test "$GCORE" = "gcore"; then
GCORE_COMMAND="gcore -o @FILE@ @PID@"
else
GCORE_COMMAND=
fi
AC_SUBST(GCORE_COMMAND)dnl
dnl
AC_CHECK_PROGS(COMPFACE, compface)
if test "$COMPFACE" = "compface"; then
  AC_CACHE_CHECK([whether compface can input xbm files], ddd_cv_compface_xbm,
	[if $COMPFACE ${srcdir}/icons/ddd.xbm > /dev/null 2>&1; then
	 ddd_cv_compface_xbm=yes; else ddd_cv_compface_xbm=no; fi])
  if test "$ddd_cv_compface_xbm" = "yes"; then
    XBM2XFACE="$COMPFACE"
  fi
fi
if test -z "$XBM2XFACE"; then
  AC_CHECK_PROGS(XBM2XFACE, xbm2xface.pl xbm2xface, false)
fi
dnl
dnl The debugger.
dnl
AC_CHECK_PROGS(DEBUGGER, gdb wdb dbx ladebug xdb jdb)dnl
case "$DEBUGGER" in
*gdb*) DEBUGGER_OPTION="";;
*wdb*) DEBUGGER_OPTION="--wdb";;
*dbx*) DEBUGGER_OPTION="--dbx";;
*ladebug*) DEBUGGER_OPTION="--ladebug";;
*xdb*) DEBUGGER_OPTION="--xdb";;
*jdb*) DEBUGGER_OPTION="--jdb";;
esac
case "$DEBUGGER" in
gdb) ;;
wdb) ;;
dbx) ;;
ladebug) ;;
xdb) ;;
jdb) ;;
*)   DEBUGGER_OPTION="$DEBUGGER_OPTION --debugger $DEBUGGER";
esac
AC_SUBST(DEBUGGER_OPTION)dnl
dnl
dnl Various helpers.
dnl
AC_CHECK_PROGS(XTERM, xterm kterm dtterm hpterm, xterm)dnl
AC_CHECK_PROGS(XFONTSEL, xfontsel, xfontsel)dnl
AC_PROG_INSTALL
dnl
dnl Yacc.
dnl
AC_PROG_YACC
AC_MSG_CHECKING(for ${YACC} flags)
YFLAGS=""
AC_MSG_RESULT(${YFLAGS})
AC_SUBST(YFLAGS)dnl
dnl
if test "$YACC" = "bison -y"; then
dnl
dnl Check Bison version.  DDD wants bison 1.28 or later.
dnl Note: Bison is only needed if a .Y file was modified.
dnl 
AC_MSG_CHECKING(${YACC} version)
YACC_VERSION=`${YACC} --version | ${SED} 's/.*version *//'`
if test `expr "$YACC_VERSION" '>=' 1.28` = 1; then
AC_MSG_RESULT(${YACC_VERSION} [(ok)])
else
AC_MSG_RESULT(${YACC_VERSION} [(too old)])
YACC="$missing_dir/missing $(YACC)"
AC_MSG_WARN([If you modify a .Y file or build from CVS,])
AC_MSG_WARN([you will need Bison 1.28 or later.])
fi
fi
dnl
dnl Lex.
dnl
AM_PROG_LEX
AC_MSG_CHECKING(for ${LEX} flags)
LFLAGS=""
if test "$LEX" = "flex"; then
LFLAGS="-8"
fi
AC_MSG_RESULT(${LFLAGS})
AC_SUBST(LFLAGS)dnl
AC_PROG_AWK
dnl
dnl
dnl Check for linker flags
dnl
rpath=
RPATH=
AC_ARG_ENABLE(rpath,
AC_HELP_STRING([--enable-rpath=PATH],[search run-time libraries in PATH]),
[rpath="$enableval"],[rpath=])
if test "$rpath" != ""; then
RPATH="-R`echo $rpath | sed 's/:/ -R/g'`"
fi
AC_SUBST(RPATH)dnl
dnl
dnl
dnl Find and check C/C++ compilers
dnl
ICE_PROG_CC
ICE_PROG_CXX
dnl
dnl
dnl Find and check C/C++/F77 preprocessors
dnl
AC_PROG_CPP
AC_PROG_CXXCPP
dnl
dnl
dnl Check for compiler characteristics
dnl
dnl ICE_CXX_PROBLEMATIC_VERSION
dnl ICE_CXX_INCLUDE_DIR
dnl ICE_CXX_LIB_DIR
ICE_CXX_OPTIONS
ICE_CXX_BOOL
ICE_CXX_EXPLICIT
ICE_CXX_MUTABLE
ICE_CXX_EXCEPTIONS
ICE_CXX_STD_EXCEPTIONS
ICE_CXX_NAMESPACE
ICE_CXX_TYPEINFO
ICE_CXX_LONG_LONG
ICE_CXX_PRETTY_FUNCTION
ICE_CXX_ARRAY_OPERATOR_NEW
ICE_CXX_PLACEMENT_NEW
ICE_CXX_LIFETIME_OF_TEMPORARIES
ICE_CXX_ISYSTEM
ICE_CXX_NEW_CASTS
ICE_CXX_ATTRIBUTE_NORETURN
ICE_PROG_CPP_TRADITIONAL
dnl
dnl
dnl Check for `.exe' and `.obj' extensions (for Windows/Cygwin)
dnl
AC_EXEEXT
AC_OBJEXT
ICE_DELETE_CR
dnl
dnl
dnl The C++ compiler is set up.  Use it for further checks.
dnl 
AC_LANG(C++)
dnl
dnl
dnl Check for libtool.
AM_PROG_LIBTOOL
dnl
dnl
dnl Locate Libraries
dnl
dnl Check for networking functions in C++ runtime library.
dnl Zdenek Sekera <zs@cray.com> reports that SGI IRIX 6.x has these in -lc.
ddd_have_connect_in_libc=no
AC_CHECK_FUNC(connect, ddd_have_connect_in_libc=yes)
ddd_have_gethostbyname_in_libc=no
AC_CHECK_FUNC(gethostbyname, ddd_have_gethostbyname_in_libc=yes)
ddd_have_regcmp_in_libc=no
AC_CHECK_FUNC(regcmp, ddd_have_regcmp_in_libc=yes)
dnl
dnl
if test "$ddd_have_connect_in_libc" != yes; then
dnl Solaris 2.6 wants an explicit `-lsocket' before `-lnsl'.
dnl Reported by Emmanuel Engel <Emmanuel.Engel@lri.fr>
dnl 
dnl Andreas Buschmann <Andreas.Buschmann@ks.sel.alcatel.de> says that
dnl libsocket may _not_ go into LIBS, because otherwise the X11
dnl configuration on Solaris will not find -lsocket -lnsl anymore.
AC_CHECK_LIB(socket, connect, LIBSOCKET="-lsocket")
AC_SUBST(LIBSOCKET)
fi
dnl
if test "$ddd_have_gethostbyname_in_libc" != yes; then
AC_CHECK_LIB(nsl, gethostbyname, LIBNSL="-lnsl")
fi
AC_SUBST(LIBNSL)
dnl
if test "$ddd_have_regcmp_in_libc" != yes; then
AC_CHECK_LIB(gen, regcmp, LIBGEN="-lgen")
fi
AC_SUBST(LIBGEN)
dnl
dnl
dnl Check for -ly, -lm, and -lc libraries.  These should always be present.
AC_CHECK_LIB(y, yyerror, LIBY="-ly")
AC_SUBST(LIBY)
dnl
AC_CHECK_LIB(m, sin, LIBM="-lm")
AC_SUBST(LIBM)
dnl
AC_CHECK_LIB(c, open, LIBC="-lc")
AC_SUBST(LIBC)
dnl
dnl
dnl Check for -lelf library.  For vxworks.
AC_CHECK_LIB(elf, elf_version, LIBELF="-lelf")
AC_SUBST(LIBELF)
dnl
dnl
dnl Marc Lepage <mlepage@kingston.hummingbird.com> reports that
dnl the Digital UNIX native compiler wants -lcxxinst to get
dnl template instantiations.  Do this only if not GCC.
dnl
if test "$GXX" != yes; then
AC_CHECK_LIB(cxxinst, main, CXXLIBS="-lcxxinst $CXXLIBS")
fi
dnl
dnl
dnl If we already have -liberty, -ltermcap, etc., 
dnl there is no need to build them.
ddd_have_termcap=false
ddd_have_libiberty=false
ddd_have_readline=false
dnl
dnl check for ncurses
ncurses_includes=
ncurses_libraries=
AC_ARG_WITH(ncurses,
AC_HELP_STRING([--without-ncurses],[do not use the Ncurses library]),[
  if test "$withval" = "no"; then
    ncurses_includes=no
    ncurses_libraries=no
  fi
])
AC_ARG_WITH(ncurses-libraries,
AC_HELP_STRING([--with-ncurses-libraries=DIR],
               [Ncurses libraries are in DIR]),[
  ncurses_libraries="$withval"
  case "$ncurses_libraries" in
  no) ;;
  yes)
    AC_CHECK_LIB(ncurses, tgetent, [LIBTERMCAP="-lncurses";
      ddd_have_termcap=true])
    ;;
  *)
    _ldflags="${LDFLAGS}"
    LDFLAGS="${LDFLAGS} -L$ncurses_libraries"
    AC_CHECK_LIB(ncurses, tgetent,
      [LIBTERMCAP="-L$ncurses_libraries -lncurses";
      ddd_have_termcap=true])
    LDFLAGS="${_ldflags}"
    ;;
  esac
])  
dnl
dnl Look for the tgetent() function - either in libtermcap, libcurses,
dnl or use the GNU termcap library as shipped with DDD.
dnl
dnl On FreeBSD systems, `-lmytinfo' is preferred to `-lncurses'.
dnl Reported by Vincenzo Capuano <vcapuano@esoc.esa.de>
dnl
dnl On Linux ELF systems, `-lncurses' is preferred to `-ltermcap'.
dnl Reported by jdassen@wi.leidenuniv.nl (J.H.M. Dassen)
dnl
if test "x${ddd_have_termcap}" = "xfalse"; then
  if test "x$ncurses_libraries" = "xno"; then
    _termlib="mytinfo curses termcap terminfo termlib"
  else
    _termlib="mytinfo ncurses curses termcap terminfo termlib"
  fi
  for termlib in ${_termlib}; do
     AC_CHECK_LIB(${termlib}, tgetent, [LIBTERMCAP="-l${termlib}";
       ddd_have_termcap=true; break])
  done
fi
dnl
dnl Check for -lreadline (optional)
readline_support=no
AC_ARG_WITH(readline,
AC_HELP_STRING([--with-readline],[do use the readline library default=no]),[
  if test "$withval" = "no"; then
    readline_support=no
  else
    readline_support=yes
  fi
])
LIBREADLINE=""
if test "x${readline_support}" != "xno"; then
  AC_ARG_WITH(readline-includes,
  AC_HELP_STRING([--with-readline-includes=DIR],
                 [Readline libraries are in DIR]),[
    readline_includes="$withval"
    case "$readline_includes" in
    no) ;;
    yes)
      AC_CHECK_HEADER(readline/readline.h)
      ;;
    *)
      _cppflags="${CPPFLAGS}"
      CPPFLAGS="-I$readline_includes $CPPFLAGS"
      AC_CHECK_HEADER(readline/readline.h, , CPPFLAGS="${_cppflags}")
      ;;
    esac
  ])
  AC_ARG_WITH(readline-libraries,
  AC_HELP_STRING([--with-readline-libraries=DIR],
                 [Readline libraries are in DIR]),[
    readline_libraries="$withval"
    case "$readline_libraries" in
    no) ;;
    yes)
      AC_CHECK_LIB(readline, readline,
                   [ddd_have_readline=true
                   LIBREADLINE="-lreadline"], ,
                   $LIBTERMCAP)
      ;;
    *)
      _ldflags="${LDFLAGS}"
      LDFLAGS="-L$readline_libraries $LDFLAGS"
      AC_CHECK_LIB(readline, readline,
                  [ddd_have_readline=true
		  LIBREADLINE="-L$readline_libraries -lreadline"], ,
                  $LIBTERMCAP)
      LDFLAGS="${_ldflags}"
      ;;
    esac])
dnl readline has not been found, test if readline is available on the system
  if test "x${ddd_have_readline}" != "true"; then
    AC_CHECK_LIB(readline, readline,
                [ddd_have_readline=true
                LIBREADLINE="-lreadline"], ,
                $LIBTERMCAP)
  fi
  if test "${ddd_have_readline}" != true; then
    LIBREADLINE="../readline/readline.a"
  fi
  AC_DEFINE([WITH_READLINE],1,[Define if you want readline support in DDD.])
else
dnl do not build built-in readline
  ddd_have_readline=true
fi
AC_SUBST(LIBREADLINE)
dnl
dnl
dnl The `-liberty' library per definition contains only functions
dnl missing in system libraries.
LIBIBERTY="../libiberty/libiberty.a"
dnl
dnl If we have an installed library, prefer it over the one shipped with DDD.
AC_CHECK_LIB(iberty, xstrerror,
	     [LIBIBERTY="-liberty"
	      ddd_have_libiberty=true])
dnl
AC_SUBST(LIBIBERTY)
dnl
dnl
if test "$ddd_have_readline" != true; then
    AC_CONFIG_SUBDIRS([readline])
fi
if test "$ddd_have_libiberty" != true; then
    AC_CONFIG_SUBDIRS([libiberty])
fi
if test "$ddd_have_termcap" != true; then
    AC_CONFIG_SUBDIRS([termcap])
    LIBTERMCAP="../termcap/libtermcap.a"
fi
AC_SUBST(LIBTERMCAP)
dnl
dnl
dnl `streampos' and `iostate' types
ICE_STREAMPOS
ICE_STD_STREAMPOS
dnl ICE_IOSTATE
dnl
dnl
dnl
dnl
dnl Check for X stuff
dnl
dnl
dnl Check X options
dnl
if test "$with_x" = no; then
AC_MSG_ERROR([$DDD requires the X window system to compile and run.
                  Please do not use the configure option '--without-x'.])
fi
if test "$with_motif" = no; then
AC_MSG_ERROR([$DDD requires Motif to compile and run.
                  Please do not use the configure option '--without-motif'.])
fi
dnl
dnl
dnl Check for X libraries
dnl
if test "$with_x" != no; then
ice_save_LIBS="$LIBS"
ice_save_CFLAGS="$CFLAGS"
ice_save_CXXFLAGS="$CXXFLAGS"
ice_save_CPPFLAGS="$CPPFLAGS"
ice_save_LDFLAGS="$LDFLAGS"
dnl
ICE_PERMISSIVE
CXXFLAGS="$CXXFLAGS $PERMISSIVE"
dnl
AC_PATH_XTRA
dnl
dnl Autoconf bug fix: Cygwin and others want -lSM -lICE in X_EXTRA_LIBS instead
dnl of X_PRE_LIBS (that is, linking order should be `-lX11 -lSM -lICE')
loop=true
while $loop; do
  case "$X_PRE_LIBS" in
    *-lICE*) X_EXTRA_LIBS="-lICE $X_EXTRA_LIBS";
	    X_PRE_LIBS=`echo $X_PRE_LIBS | $SED 's,-lICE,,g'`
	;;
    *-lSM*) X_EXTRA_LIBS="-lSM $X_EXTRA_LIBS";
	    X_PRE_LIBS=`echo $X_PRE_LIBS | $SED 's,-lSM,,g'`
	;;
    *) loop=false;;
   esac
done
dnl
dnl Autoconf bug fix: GCC 2.95 and later want -isystem for including X files
X_CFLAGS=`echo $X_CFLAGS | $SED "s,-I,$ISYSTEM,g"`
dnl
ICE_FIND_MOTIF
ICE_FIND_ATHENA
ICE_FIND_XPM
ICE_X_PERMISSIVE
CXXFLAGS="$CXXFLAGS $X_PERMISSIVE"
dnl
dnl Autoconf bug fix: Sun CC 3.0 requires `-R lib', g++ requires `-Rlib'.
changequote(,)dnl
case "$CXX" in
  *CC) X_LIBS=`echo $X_LIBS | $SED 's, -R\([^ ]\), -R \1,g'`
esac
changequote([,])dnl
dnl
dnl
dnl
LIBS="$LIBS $X_EXTRA_LIBS"
CFLAGS="$CFLAGS $X_CFLAGS"
CPPFLAGS="$CPPFLAGS $X_CFLAGS"
LDFLAGS="$LDFLAGS $X_LIBS"
dnl
dnl Check for X library
dnl
X11_LIBS=""
AC_CHECK_LIB(X11, XOpenDisplay, X11_LIBS="-lX11",,${X_PRE_LIBS} ${X_EXTRA_LIBS})
if test "$X11_LIBS" = ""; then
dnl Not having X is fatal.  Let the user fix this.
AC_MSG_ERROR([The X11 library '-lX11' could not be found.
                  Please use the configure options '--x-includes=DIR'
                  and '--x-libraries=DIR' to specify the X location.
                  See the files 'config.log' and 'ddd/config.log'
                  for further diagnostics.])
fi
AC_SUBST(X_LIBS)
AC_SUBST(X11_LIBS)
AC_SUBST(X_PRE_LIBS)
dnl
dnl
dnl Check for -lXintl library.  Lutz Kittler <kittler@sse-erfurt.de> says
dnl that DLD-Linux with CDE wants `-lXintl' to refine `Xlcmbcurmax'.
dnl Guenther Roehrich <guenther@studbox.uni-stuttgart.de> says that
dnl we must check for `_Xlcmbcurmax' instead of `Xlcmbcurmax'.
dnl
AC_CHECK_LIB(Xintl, _Xlcmbcurmax, X11_LIBS="${X11_LIBS} -lXintl",,
	${X_PRE_LIBS} ${X11_LIBS} ${X_EXTRA_LIBS})
dnl
dnl
dnl Check for -lipc library.  SCO unix is said to want that.
dnl
AC_CHECK_LIB(ipc, shmap, X11_LIBS="${X11_LIBS} -lipc",,
	${X_PRE_LIBS} ${X11_LIBS} ${X_EXTRA_LIBS})
dnl
dnl
dnl Check for X toolkit libraries
dnl
XT_LIBS=""
AC_CHECK_LIB(Xt, XtToolkitInitialize, XT_LIBS="-lXt",,
	${X_PRE_LIBS} ${X11_LIBS} ${X_EXTRA_LIBS})
if test "$XT_LIBS" = ""; then
dnl Not having Xt is fatal.  Let the user fix this.
AC_MSG_ERROR([The X toolkit library '-lXt' could not be found.
                  Please use the configure options '--x-includes=DIR'
                  and '--x-libraries=DIR' to specify the X location.
                  See the files 'config.log' and 'ddd/config.log'
                  for further diagnostics.])
fi
AC_SUBST(XT_LIBS)
dnl
dnl
dnl Check for X extension libraries
dnl
dnl
XEXT_LIBS=""
AC_CHECK_LIB(Xext, XShapeQueryVersion, XEXT_LIBS="-lXext",,
	${X_PRE_LIBS} ${X11_LIBS} ${X_EXTRA_LIBS})
AC_SUBST(XEXT_LIBS)
dnl
dnl
dnl
dnl Check for Xpm library and headers
dnl
XPM_LIBS=""
if test "$xpm_includes" != "no" && test "$xpm_libraries" != "no"
then
AC_CHECK_LIB(Xpm, XpmCreatePixmapFromXpmImage,
	XPM_LIBS="-lXpm"
	AC_DEFINE(HAVE_XPM,1,[Define if you have the XPM library.]),,
        ${X_PRE_LIBS} ${X11_LIBS} ${X_EXTRA_LIBS})
if test "$XPM_LIBS" != ""; then
AC_CHECK_HEADERS(X11/xpm.h xpm.h)
fi
fi
AC_SUBST(XPM_LIBS)
dnl
dnl
dnl Check for Athena widget libraries
dnl
dnl
XAW_LIBS=""
if test "$athena_includes" != "no" && test "$athena_libraries" != "no"
then
AC_CHECK_LIB(Xmu, XmuCvtStringToOrientation,
	XAW_LIBS="-lXmu ${XAW_LIBS}",,
	${X_PRE_LIBS} ${XAW_LIBS} ${XT_LIBS} ${XPM_LIBS} ${XEXT_LIBS} ${X11_LIBS} ${X_EXTRA_LIBS})
AC_CHECK_LIB(Xaw, XawTextSetInsertionPoint, 
	XAW_LIBS="-lXaw ${XAW_LIBS}"
	AC_DEFINE(HAVE_ATHENA,1,[Define if you have the X Athena widgets.]),,
	${X_PRE_LIBS} ${XAW_LIBS} ${XT_LIBS} ${XPM_LIBS} ${XEXT_LIBS} ${X11_LIBS} ${X_EXTRA_LIBS})
if test "$XAW_LIBS" = ""; then
dnl A warning suffices here -- DDD also works without Athena.
AC_MSG_WARN([The Athena widgets library '-lXaw' could not be found.
                    If you have the athena library, please use the
                    configure options '--with-athena-includes=DIR' and 
                    '--with-athena-libraries=DIR' to specify the Xaw location.
                    If you do not have the athena library, use the configure
                    option '--without-athena' to suppress this message.
                    See the files 'config.log' and 'ddd/config.log'
                    for further diagnostics.])
fi
fi
if test "$XAW_LIBS" != ""; then
AC_CHECK_HEADERS(X11/Xaw/Form.h X11/Xaw/Panner.h X11/Xaw/Porthole.h)
AC_CHECK_HEADERS(X11/Xmu/Editres.h,[],[],
[[#include <X11/Intrinsic.h>
]])
fi
AC_SUBST(XAW_LIBS)
dnl
dnl
dnl Check for Motif widget libraries
dnl
dnl
if test "$motif_includes" = "no"
then
dnl Without Xm/Xm.h, we won't get far.  Let the user fix this.
AC_MSG_ERROR([The Motif include file 'Xm/Xm.h' could not be found.
                  Please use the configure options '--with-motif-includes=DIR'
                  and '--with-motif-libraries=DIR' to specify the Xm location.
                  See the files 'config.log' and 'ddd/config.log'
                  for further diagnostics.])
fi
XM_LIBS=""
XP_LIBS=""
if test "$motif_includes" != "no" && test "$motif_libraries" != "no"
then
dnl Motif 2.1 wants `-lXp' (X11R6.3 print server)
AC_CHECK_LIB(Xp, XpSelectInput,
	XP_LIBS="-lXp"
	AC_DEFINE(HAVE_XP,1,[Define if you have the XP library (X11R6.3 print server).]),,
	${X_PRE_LIBS} ${XPM_LIBS} ${XEXT_LIBS} ${XT_LIBS} ${XEXT_LIBS} ${X11_LIBS} ${X_EXTRA_LIBS} ${LIBGEN})
dnl
AC_CHECK_LIB(Xm, XmCreateOptionMenu, 
	XM_LIBS="-lXm"
	AC_DEFINE(HAVE_MOTIF,1,[Define if you have Motif widgets.]),,
	${X_PRE_LIBS} ${XP_LIBS} ${XPM_LIBS} ${XEXT_LIBS} ${XT_LIBS} ${XEXT_LIBS} ${X11_LIBS} ${X_EXTRA_LIBS} ${LIBGEN})
dnl
AC_CHECK_LIB(Xm, xmUseVersion, 
	AC_DEFINE(HAVE_XMUSEVERSION,1,[Define if the Motif library defines `xmUseVersion'.]),,
	${X_PRE_LIBS} ${XP_LIBS} ${XPM_LIBS} ${XEXT_LIBS} ${XT_LIBS} ${XEXT_LIBS} ${X11_LIBS} ${X_EXTRA_LIBS} ${LIBGEN})
dnl
AC_CHECK_LIB(Xm, XmInstallImage,
	AC_DEFINE(HAVE_XMINSTALLIMAGE,1,[Define if the Motif library defines `XmInstallImage'.]),,
	${X_PRE_LIBS} ${XP_LIBS} ${XPM_LIBS} ${XEXT_LIBS} ${XT_LIBS} ${XEXT_LIBS} ${X11_LIBS} ${X_EXTRA_LIBS} ${LIBGEN})
dnl
AC_CHECK_LIB(Xm, Xm21InstallImage,
	AC_DEFINE(HAVE_XM21INSTALLIMAGE,1,[Define if the Motif library defines `Xm21InstallImage'.]),,
	${X_PRE_LIBS} ${XP_LIBS} ${XPM_LIBS} ${XEXT_LIBS} ${XT_LIBS} ${XEXT_LIBS} ${X11_LIBS} ${X_EXTRA_LIBS} ${LIBGEN})
fi
if test "$XM_LIBS" = ""; then
dnl Without Motif, we won't get far.  Let the user fix this.
AC_MSG_ERROR([The Motif library '-lXm' could not be found.
                  Please use the configure options '--with-motif-includes=DIR'
                  and '--with-motif-libraries=DIR' to specify the Xm location.
                  See the files 'config.log' and 'ddd/config.log'
                  for further diagnostics.])
fi
dnl Tweak: HPUX-10.20 requires that Motif be statically linked.
dnl Linking dynamically will appear to succeed, but DDD will core on startup.
dnl (I haven't tried other HPUX versions, so I'd be glad for any feedback.)
if test "$host_vendor" = "hp" && test "$host_os" = "hpux10.20"; then
   XM_LIBS="/usr/lib/Motif1.2_R6/libXm.a"
fi
AC_SUBST(XP_LIBS)
AC_SUBST(XM_LIBS)
dnl
dnl Check for `baseTranslations' resource
ICE_TRANSLATION_RESOURCE
dnl
dnl Check for Motif properties
LIBS="$LIBS ${XM_LIBS} ${X_PRE_LIBS} ${XP_LIBS} ${XPM_LIBS} ${XEXT_LIBS} ${XT_LIBS} ${XEXT_LIBS} ${X11_LIBS} ${X_EXTRA_LIBS} ${LIBGEN}"
ICE_MOTIF_DIALOGS_OWN_SELECTION_LIST
dnl
dnl
dnl
dnl
LIBS="$ice_save_LIBS"
CFLAGS="$ice_save_CFLAGS"
CXXFLAGS="$ice_save_CXXFLAGS"
CPPFLAGS="$ice_save_CPPFLAGS"
LDFLAGS="$ice_save_LDFLAGS"
fi
dnl
dnl
dnl
dnl
dnl Setup library loading flags
dnl
dnl FIXME: Don't use this right now.  Some systems/environments rely
dnl on $LD_RUN_PATH specifying library locations, and using -rpath
dnl overrides these settings.  If you need -rpath, it might be better
dnl to set LD_RUN_PATH instead.
dnl
dnl ICE_SETUP_RPATH
dnl
dnl
dnl Check for Make characteristics
dnl
AC_PROG_MAKE_SET
dnl
dnl
dnl Find types
dnl 
dnl The getgroups() check fails with SGI CC.  Use the C compiler instead.
AC_LANG_PUSH(C)
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UID_T
AC_TYPE_GETGROUPS
AC_TYPE_SIGNAL
AC_LANG_POP(C)
dnl
ICE_TYPE_SIG_HANDLER_ARGS
dnl
dnl
dnl Find libraries and functions
dnl 
AC_CHECK_FUNC(getpgrp)
AC_CHECK_FUNC(setpgrp)
dnl
dnl The getprgp() check fails with SGI CC.  Use the C compiler instead.
AC_LANG_PUSH(C)
AC_FUNC_GETPGRP
AC_FUNC_SETPGRP
AC_LANG_POP(C)
dnl
dnl
dnl
AC_HEADER_STDC
AC_CHECK_HEADERS(alloca.h fcntl.h limits.h locale.h malloc.h netdb.h pwd.h)
AC_CHECK_HEADERS(libelf.h elf.h sys/elf.h stdlib.h)
AC_CHECK_HEADERS(string.h strings.h stropts.h termio.h termios.h unistd.h)
AC_CHECK_HEADERS(sys/ioctl.h sys/int_types.h sys/param.h sys/resource.h)
AC_CHECK_HEADERS(sys/stat.h sys/stropts.h sys/vty.h sys/sysmacros.h sys/core.h)
AC_CHECK_HEADERS(sys/socket.h sys/termios.h sys/time.h sys/timeb.h sys/types.h)
AC_CHECK_HEADERS(sys/utsname.h sys/user.h sys/ptrace.h)
AC_CHECK_HEADERS(exception stdexcept)
AC_HEADER_TIME
AC_HEADER_SYS_WAIT
AC_HEADER_DIRENT
AC_HEADER_STAT
dnl
dnl These checks may require `-lstdc++' to link and run properly
ice_save_LIBS="$LIBS"
LIBS="$CXXLIBS $LIBS"
ICE_CHECK_CORE_MAGIC
ICE_CHECK_PTRACE_DUMPCORE
dnl ICE_CHECK_FREEZE_OSTRSTREAMBUF
dnl ICE_CHECK_FROZEN_OSTRSTREAMBUF
dnl ICE_CHECK_FREEZE_OSTRSTREAM
dnl ICE_CHECK_FROZEN_OSTRSTREAM
dnl ICE_OSTRSTREAM_PCOUNT_BROKEN
dnl
LIBS="$ice_save_LIBS"
AC_CHECK_LIB(m, hypot)
dnl
ICE_CHECK_DECL(_getpty, unistd.h)
AH_TEMPLATE([HAVE__GETPTY_DECL],[Define if _getpty is declared in unistd.h.])
ICE_CHECK_DECL(alloca, alloca.h stdlib.h)
AH_TEMPLATE([HAVE_ALLOCA_DECL],[Define if alloca is declared in alloca.h stdlib.h.])
ICE_CHECK_DECL(asctime, time.h)
AH_TEMPLATE([HAVE_ASCTIME_DECL],[Define if asctime is declared in time.h.])
ICE_CHECK_DECL(clock, time.h)
AH_TEMPLATE([HAVE_CLOCK_DECL],[Define if clock is declared in time.h.])
ICE_CHECK_DECL(difftime, time.h)
AH_TEMPLATE([HAVE_DIFFTIME_DECL],[Define if difftime is declared in time.h.])
ICE_CHECK_DECL(dup2, unistd.h)
AH_TEMPLATE([HAVE_DUP2_DECL],[Define if dup2 is declared in unistd.h.])
ICE_CHECK_DECL(endpwent, pwd.h)
AH_TEMPLATE([HAVE_ENDPWENT_DECL],[Define if endpwent is declared in pwd.h.])
ICE_CHECK_DECL(fcntl, fcntl.h)
AH_TEMPLATE([HAVE_FCNTL_DECL],[Define if fcntl is declared in fcntl.h.])
ICE_CHECK_DECL(ftime, sys/timeb.h)
AH_TEMPLATE([HAVE_FTIME_DECL],[Define if ftime is declared in sys/timeb.h.])
ICE_CHECK_DECL(gethostbyname, netdb.h)
AH_TEMPLATE([HAVE_GETHOSTBYNAME_DECL],[Define if gethostbyname is declared in netdb.h.])
ICE_CHECK_DECL(gethostname, unistd.h)
AH_TEMPLATE([HAVE_GETHOSTNAME_DECL],[Define if gethostname is declared in unistd.h.])
ICE_CHECK_DECL(getpt, stdlib.h)
AH_TEMPLATE([HAVE_GETPT_DECL],[Define if getpt is declared in stdlib.h.])
ICE_CHECK_DECL(getpty, unistd.h)
AH_TEMPLATE([HAVE_GETPTY_DECL],[Define if getpty is declared in unistd.h.])
ICE_CHECK_DECL(getrlimit, sys/resource.h)
AH_TEMPLATE([HAVE_GETRLIMIT_DECL],[Define if getrlimit is declared in sys/resource.h.])
ICE_CHECK_DECL(getrusage, sys/resource.h)
AH_TEMPLATE([HAVE_GETRUSAGE_DECL],[Define if getrusage is declared in sys/resource.h.])
ICE_CHECK_DECL(getwd, unistd.h)
AH_TEMPLATE([HAVE_GETWD_DECL],[Define if getwd is declared in unistd.h.])
ICE_CHECK_DECL(grantpt, stdlib.h)
AH_TEMPLATE([HAVE_GRANTPT_DECL],[Define if grantpt is declared in stdlib.h.])
ICE_CHECK_DECL(hypot, math.h)
AH_TEMPLATE([HAVE_HYPOT_DECL],[Define if hypot is declared in math.h.])
ICE_CHECK_DECL(ioctl, unistd.h sys/ioctl.h)
AH_TEMPLATE([HAVE_IOCTL_DECL],[Define if ioctl is declared in unistd.h sys/ioctl.h.])
ICE_CHECK_DECL(link, unistd.h)
AH_TEMPLATE([HAVE_LINK_DECL],[Define if link is declared in unistd.h.])
ICE_CHECK_DECL(lstat, sys/stat.h)
AH_TEMPLATE([HAVE_LSTAT_DECL],[Define if lstat is declared in sys/stat.h.])
ICE_CHECK_DECL(malloc, stdlib.h)
AH_TEMPLATE([HAVE_MALLOC_DECL],[Define if malloc is declared in stdlib.h.])
ICE_CHECK_DECL(memcpy, string.h)
AH_TEMPLATE([HAVE_MEMCPY_DECL],[Define if memcpy is declared in string.h.])
ICE_CHECK_DECL(memset, string.h)
AH_TEMPLATE([HAVE_MEMSET_DECL],[Define if memset is declared in string.h.])
ICE_CHECK_DECL(mkstemp, stdlib.h)
AH_TEMPLATE([HAVE_MKSTEMP_DECL],[Define if mkstemp is declared in stdlib.h.])
ICE_CHECK_DECL(pclose, stdio.h)
AH_TEMPLATE([HAVE_PCLOSE_DECL],[Define if pclose is declared in stdio.h.])
ICE_CHECK_DECL(popen, stdio.h)
AH_TEMPLATE([HAVE_POPEN_DECL],[Define if popen is declared in stdio.h.])
ICE_CHECK_DECL(ptrace, unistd.h sys/ptrace.h)
AH_TEMPLATE([HAVE_PTRACE_DECL],[Define if ptrace is declared in unistd.h sys/ptrace.h.])
ICE_CHECK_DECL(ptsname, stdlib.h)
AH_TEMPLATE([HAVE_PTSNAME_DECL],[Define if ptsname is declared in stdlib.h.])
ICE_CHECK_DECL(raise, signal.h)
AH_TEMPLATE([HAVE_RAISE_DECL],[Define if raise is declared in signal.h.])
ICE_CHECK_DECL(rand, stdlib.h)
AH_TEMPLATE([HAVE_RAND_DECL],[Define if rand is declared in stdlib.h.])
ICE_CHECK_DECL(random, stdlib.h)
AH_TEMPLATE([HAVE_RANDOM_DECL],[Define if random is declared in stdlib.h.])
ICE_CHECK_DECL(readlink, unistd.h)
AH_TEMPLATE([HAVE_READLINK_DECL],[Define if readlink is declared in unistd.h.])
ICE_CHECK_DECL(setbuf, stdio.h)
AH_TEMPLATE([HAVE_SETBUF_DECL],[Define if setbuf is declared in stdio.h.])
ICE_CHECK_DECL(setlocale, locale.h)
AH_TEMPLATE([HAVE_SETLOCALE_DECL],[Define if setlocale is declared in locale.h.])
ICE_CHECK_DECL(setpgid, unistd.h)
AH_TEMPLATE([HAVE_SETPGID_DECL],[Define if setpgid is declared in unistd.h.])
ICE_CHECK_DECL(setpgrp, unistd.h)
AH_TEMPLATE([HAVE_SETPGRP_DECL],[Define if setpgrp is declared in unistd.h.])
ICE_CHECK_DECL(setpgrp2, unistd.h)
AH_TEMPLATE([HAVE_SETPGRP2_DECL],[Define if setpgrp2 is declared in unistd.h.])
ICE_CHECK_DECL(setpwent, pwd.h)
AH_TEMPLATE([HAVE_SETPWENT_DECL],[Define if setpwent is declared in pwd.h.])
ICE_CHECK_DECL(setrlimit, sys/resource.h)
AH_TEMPLATE([HAVE_SETRLIMIT_DECL],[Define if setrlimit is declared in sys/resource.h.])
ICE_CHECK_DECL(setsid, unistd.h)
AH_TEMPLATE([HAVE_SETSID_DECL],[Define if setsid is declared in unistd.h.])
ICE_CHECK_DECL(setvbuf, stdio.h)
AH_TEMPLATE([HAVE_SETVBUF_DECL],[Define if setvbuf is declared in stdio.h.])
ICE_CHECK_DECL(sigblock, signal.h)
AH_TEMPLATE([HAVE_SIGBLOCK_DECL],[Define if sigblock is declared in signal.h.])
ICE_CHECK_DECL(sigprocmask, signal.h)
AH_TEMPLATE([HAVE_SIGPROCMASK_DECL],[Define if sigprocmask is declared in signal.h.])
ICE_CHECK_DECL(sigsetmask, signal.h)
AH_TEMPLATE([HAVE_SIGSETMASK_DECL],[Define if sigsetmask is declared in signal.h.])
ICE_CHECK_DECL(sigsuspend, signal.h)
AH_TEMPLATE([HAVE_SIGSUSPEND_DECL],[Define if sigsuspend is declared in signal.h.])
ICE_CHECK_DECL(srand, stdlib.h)
AH_TEMPLATE([HAVE_SRAND_DECL],[Define if srand is declared in stdlib.h.])
ICE_CHECK_DECL(srandom, stdlib.h math.h)
AH_TEMPLATE([HAVE_SRANDOM_DECL],[Define if srandom is declared in stdlib.h math.h.])
ICE_CHECK_DECL(stat, sys/stat.h)
AH_TEMPLATE([HAVE_STAT_DECL],[Define if stat is declared in sys/stat.h.])
ICE_CHECK_DECL(strerror, string.h)
AH_TEMPLATE([HAVE_STRERROR_DECL],[Define if strerror is declared in string.h.])
ICE_CHECK_DECL(strftime, time.h)
AH_TEMPLATE([HAVE_STRFTIME_DECL],[Define if strftime is declared in time.h.])
ICE_CHECK_DECL(strsignal, string.h unistd.h)
AH_TEMPLATE([HAVE_STRSIGNAL_DECL],[Define if strsignal is declared in string.h unistd.h.])
ICE_CHECK_DECL(strtod, stdlib.h)
AH_TEMPLATE([HAVE_STRTOD_DECL],[Define if strtod is declared in stdlib.h.])
ICE_CHECK_DECL(strtol, stdlib.h)
AH_TEMPLATE([HAVE_STRTOL_DECL],[Define if strtol is declared in stdlib.h.])
ICE_CHECK_DECL(strtoul, stdlib.h)
AH_TEMPLATE([HAVE_STRTOUL_DECL],[Define if strtoul is declared in stdlib.h.])
ICE_CHECK_DECL(symlink, unistd.h)
AH_TEMPLATE([HAVE_SYMLINK_DECL],[Define if symlink is declared in unistd.h.])
ICE_CHECK_DECL(tcdrain, termios.h)
AH_TEMPLATE([HAVE_TCDRAIN_DECL],[Define if tcdrain is declared in termios.h.])
ICE_CHECK_DECL(tcflow, termios.h)
AH_TEMPLATE([HAVE_TCFLOW_DECL],[Define if tcflow is declared in termios.h.])
ICE_CHECK_DECL(tcflush, termios.h)
AH_TEMPLATE([HAVE_TCFLUSH_DECL],[Define if tcflush is declared in termios.h.])
ICE_CHECK_DECL(tcgetattr, termios.h)
AH_TEMPLATE([HAVE_TCGETATTR_DECL],[Define if tcgetattr is declared in termios.h.])
ICE_CHECK_DECL(tcgetpgrp, unistd.h)
AH_TEMPLATE([HAVE_TCGETPGRP_DECL],[Define if tcgetpgrp is declared in unistd.h.])
ICE_CHECK_DECL(tcgetsid, sys/termios.h)
AH_TEMPLATE([HAVE_TCGETSID_DECL],[Define if tcgetsid is declared in sys/termios.h.])
ICE_CHECK_DECL(tcsendbreak, termios.h)
AH_TEMPLATE([HAVE_TCSENDBREAK_DECL],[Define if tcsendbreak is declared in termios.h.])
ICE_CHECK_DECL(tcsetattr, termios.h)
AH_TEMPLATE([HAVE_TCSETATTR_DECL],[Define if tcsetattr is declared in termios.h.])
ICE_CHECK_DECL(tcsetpgrp, unistd.h)
AH_TEMPLATE([HAVE_TCSETPGRP_DECL],[Define if tcsetpgrp is declared in unistd.h.])
ICE_CHECK_DECL(times, sys/times.h)
AH_TEMPLATE([HAVE_TIMES_DECL],[Define if times is declared in sys/times.h.])
ICE_CHECK_DECL(tmpfile, stdio.h)
AH_TEMPLATE([HAVE_TMPFILE_DECL],[Define if tmpfile is declared in stdio.h.])
ICE_CHECK_DECL(uname, sys/utsname.h)
AH_TEMPLATE([HAVE_UNAME_DECL],[Define if uname is declared in sys/utsname.h.])
ICE_CHECK_DECL(unlink, unistd.h)
AH_TEMPLATE([HAVE_UNLINK_DECL],[Define if unlink is declared in unistd.h.])
ICE_CHECK_DECL(unlockpt, stdlib.h)
AH_TEMPLATE([HAVE_UNLOCKPT_DECL],[Define if unlockpt is declared in stdlib.h.])
ICE_CHECK_DECL(waitpid, sys/wait.h)
AH_TEMPLATE([HAVE_WAITPID_DECL],[Define if waitpid is declared in sys/wait.h.])
dnl
dnl These checks may require -lstdc++ to link and tun properly
ice_save_LIBS="$LIBS"
LIBS="$CXXLIBS $LIBS"
ICE_CHECK_PATH_MAX
ICE_CHECK_POSIX_PATH_MAX
ICE_CHECK_MAXPATHLEN
LIBS="$ice_save_LIBS"
dnl
dnl
dnl
dnl
dnl As of DDD 2.2, we don't need all this RX stuff, but use
dnl a LEX-generated scanner for regexps.
dnl
dnl ICE_TYPE_REGEX_T
dnl dnl The regexec() check fails with SGI CC.  Use the C compiler instead.
dnl AC_LANG_PUSH(C)
dnl ICE_REGCOMP_BROKEN
dnl AC_LANG_POP(C)
dnl
dnl Check for a POSIX.2 regcomp()/regexec() library
dnl if test "$ice_have_regcomp" != yes || test "$ice_have_regexec" != yes || test "$ice_cv_regcomp_broken" != no; then
dnl dnl No need to use the GNU regcomp() library - there's one in the system
dnl   AC_MSG_ERROR([$DDD requires a regcomp()/regexec() library.])
dnl fi
dnl
LIBRX=""
AC_SUBST(LIBRX)
dnl
AC_DEFINE(WITH_RUNTIME_REGEX, 0,
[Define if you want support for runtime-compiled regexps. Required
in ICE; not needed in DDD.])
AC_DEFINE(RUNTIME_REGEX, 0,
[Define if you want support for runtime-compiled regexps. Default in
DDD 2.1 and earlier.  Requires ~30% more memory; slightly slower,
too. Requires WITH_RUNTIME_REGEX to be set. Required in ICE.])
dnl
dnl
dnl User-defined options
dnl
AC_ARG_ENABLE(builtin-manual,
AC_HELP_STRING([--enable-builtin-manual],[compile manual into DDD]),
	builtin_manual="$enableval", builtin_manual=no)
AC_ARG_ENABLE(builtin-app-defaults,
AC_HELP_STRING([--enable-builtin-app-defaults],
               [compile app-defaults file into DDD]),
	builtin_app_defaults="$enableval", builtin_app_defaults=no)
AC_ARG_ENABLE(builtin-news,
AC_HELP_STRING([--enable-builtin-news],[compile NEWS file into DDD]),
	builtin_news="$enableval", builtin_news=no)
AC_ARG_ENABLE(builtin-license,
AC_HELP_STRING([--enable-builtin-license],
               [compile COPYING file into DDD]),
	builtin_license="$enableval", builtin_license=no)
AC_ARG_ENABLE(builtin-vsllib,
AC_HELP_STRING([--enable-builtin-vsllib],[compile VSL library into DDD]),
	builtin_vsllib="$enableval", builtin_vsllib=no)
dnl
if test "$builtin_manual" != no; then
AC_DEFINE(WITH_BUILTIN_MANUAL,1,
[Define if you want the manual to be compiled into the executable.])
fi
if test "$builtin_app_defaults" != no; then
AC_DEFINE(WITH_BUILTIN_APP_DEFAULTS,1,
[Define if you want the app-defaults file to be compiled into the
executable.])
fi
if test "$builtin_news" != no; then
AC_DEFINE(WITH_BUILTIN_NEWS,1,
[Define if you want the DDD news to be compiled into the executable.])
fi
if test "$builtin_license" != no; then
AC_DEFINE(WITH_BUILTIN_LICENSE,1,
[Define if you want the DDD license to be compiled into the executable.])
fi
if test "$builtin_vsllib" != no; then
AC_DEFINE(WITH_BUILTIN_VSLLIB,1,
[Define if you want the DDD VSL library to be compiled into DDD.])
fi
dnl
dnl Check for risky LessTif versions.
dnl
risky_lesstif=false
ice_save_CPPFLAGS="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS $X_CFLAGS"
AC_TRY_CPP([#include <Xm/Xm.h>
#ifdef LESSTIF_VERSION
#if LESSTIF_VERSION == 0 && LESSTIF_REVISION <= 86
#error LessTif 0.86 or earlier
#endif
#endif],[risky_lesstif=false],[risky_lesstif=true])
CPPFLAGS="$ice_save_CPPFLAGS"
dnl
dnl
dnl Check for risky CC versions.
dnl
risky_cc=false
# Warn about old SGI CC versions.
case "$CXX" in
*CC)
  if test "$host_vendor" = "sgi" && test "$ice_cv_have_bool" != yes; then
    risky_cc=true
  fi
  ;;
*)
  ;;
esac
dnl
# Warn about old xlC versions.
case "$CXX" in
*xlC)
  if test "$ice_cv_have_bool" != yes; then
    risky_cc=true
  fi
  ;;
*)
  ;;
esac
dnl
dnl
dnl That's all, folks!
dnl
AC_CONFIG_FILES([Makefile ddd/Makefile ddd/OldMakefile ddd/vr.h ddd/bld.h
		 ddd/Ddd ddd/ddd.man ddd/cfg.texi ddd/.gdbinit ddd.spec
		 themes/Makefile vsllib/Makefile])
AC_CONFIG_COMMANDS([ddd/stamp-h], [date > ddd/stamp-h])
AC_OUTPUT
dnl
dnl 
dnl Give some warnings
dnl
if test "$silent" != yes; then
dnl
if $risky_cc; then
echo
echo "configure: This $CXX version may not generate safe $DDD code."
echo "Use GCC instead or continue at own risk."
fi
dnl
if $risky_lesstif; then
echo
echo "configure: This $DDD release requires LessTif 0.87 or later."
echo "Upgrade to a recent LessTif version or continue at own risk."
fi
dnl
fi
dnl
dnl Give an indication of what to do next.
dnl
if test "$silent" != yes; then
echo
echo "configure: configuration is done.  Type \`make' to make $DDD."
echo "In case of trouble, please check the settings in the files generated"
echo "by \`configure' - that is, \`ddd/config.h' and \`ddd/Makefile'."
echo
fi
