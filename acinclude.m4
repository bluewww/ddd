dnl $Id$ -*- autoconf -*-
dnl ICE and DDD autoconf macros
dnl 
dnl Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
dnl Copyright (C) 1999-2001 Universitaet Passau, Germany.
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
dnl or send a mail to the DDD developers at `ddd@gnu.org'.
dnl
dnl
dnl ICE_PROG_CC and ICE_PROG_CXX
dnl ----------------------------
dnl 
dnl Extended compiler checks.  Check not only for a compiler,
dnl but also determine whether it compiles a simple "hello, world" 
dnl program.
dnl
AC_DEFUN(ICE_PROG_CC,
[
AC_REQUIRE([AC_ISC_POSIX])
AC_REQUIRE([AC_PROG_CC])
AC_MSG_CHECKING(whether the C compiler (${CC}) compiles and links a simple C program)
AC_CACHE_VAL(ice_cv_prog_cc,
[
AC_LANG_SAVE
AC_LANG_C
AC_TRY_LINK([#include <stdio.h>], [printf("hello, world!");],
ice_cv_prog_cc=yes,
ice_cv_prog_cc=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_prog_cc)
if test "$ice_cv_prog_cc" = no; then
AC_MSG_ERROR(You must set the environment variable CC to a working
                  C compiler.  Also check the CFLAGS settings.
                  See the file 'config.log' for further diagnostics.)
fi
])dnl
dnl
AC_DEFUN(ICE_PROG_CXX,
[
AC_REQUIRE([AC_ISC_POSIX])
AC_REQUIRE([AC_PROG_CXX])
dnl
ice_save_LIBS="$LIBS"
if test "$CXX" = gcc; then
dnl
dnl Using gcc as C++ compiler requires linkage with -lstdc++ or -lg++
dnl
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_CHECK_LIB(m, sin, LIBS="-lm $LIBS")
AC_CHECK_LIB(stdc++, cout, LIBS="-lstdc++ $LIBS")
case "$LIBS" in
*-lstdc++*)
dnl -lstdc++ found - proceed
;;
*)
dnl -lstdc++ not found - try -lg++ instead
AC_CHECK_LIB(g++, cout, LIBS="-lg++ $LIBS")
;;
esac
AC_LANG_RESTORE
fi
dnl
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) compiles a simple program)
AC_CACHE_VAL(ice_cv_prog_cxx,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_LINK([#include <iostream>], [std::cout << "hello, world!";],
ice_cv_prog_cxx=yes,
ice_cv_prog_cxx=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_prog_cxx)
if test "$ice_cv_prog_cxx" = no; then
AC_MSG_ERROR(You must set the environment variable CXX to a working 
                  C++ compiler.  Also check the CXXFLAGS settings.
                  See the file 'config.log' for further diagnostics.)
fi
ice_need_cxxlibs=no
case "$LIBS" in
  *-lstdc++*)
	      case "$CXXLIBS" in
		*-lstdc++*)
			    ;;
		*)
		   ice_need_cxxlibs=yes
		   ;;
	      esac
	      ;;
  *-lg++*)
	   case "$CXXLIBS" in
	     *-lg++*)
		      ;;
	     *)
		ice_need_cxxlibs=yes
		;;
	     esac
	     ;;
esac
if test "$ice_need_cxxlibs" = yes
then
dnl These libraries are required for all C++ programs.
CXXLIBS="$CXXLIBS $LIBS"
fi
AC_SUBST(CXXLIBS)dnl
LIBS="$ice_save_LIBS"
])dnl
dnl
dnl ICE_EXTERNAL_TEMPLATES
dnl ----------------------
dnl
dnl If the C++ compiler accepts the `-fexternal-templates' flag,
dnl set output variable `EXTERNAL_TEMPLATES' to `-fexternal-templates',
dnl empty otherwise.
dnl
AC_DEFUN(ICE_EXTERNAL_TEMPLATES,
[
AC_REQUIRE([AC_PROG_CXX])
if test "$GXX" = yes; then
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) accepts -fexternal-templates)
AC_CACHE_VAL(ice_cv_external_templates,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_save_cxxflags="$CXXFLAGS"
dnl Don't use if deprecated
CXXFLAGS="-Werror -fexternal-templates"
AC_TRY_COMPILE(,[int a;],
ice_cv_external_templates=yes, ice_cv_external_templates=no)
CXXFLAGS="$ice_save_cxxflags"
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_external_templates)
if test "$ice_cv_external_templates" = yes; then
EXTERNAL_TEMPLATES=-fexternal-templates
fi
fi
AC_SUBST(EXTERNAL_TEMPLATES)
])dnl
dnl
dnl
dnl
dnl
dnl ICE_PERMISSIVE
dnl --------------
dnl
dnl If the C++ compiler accepts the `-fpermissive' flag,
dnl set output variable `PERMISSIVE' to `-fpermissive',
dnl empty otherwise.
dnl
AC_DEFUN(ICE_PERMISSIVE,
[
AC_REQUIRE([AC_PROG_CXX])
if test "$GXX" = yes; then
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) accepts -fpermissive)
AC_CACHE_VAL(ice_cv_permissive,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_save_cxxflags="$CXXFLAGS"
CXXFLAGS="-fpermissive"
AC_TRY_COMPILE(,[int a;],
ice_cv_permissive=yes, ice_cv_permissive=no)
CXXFLAGS="$ice_save_cxxflags"
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_permissive)
if test "$ice_cv_permissive" = yes; then
PERMISSIVE=-fpermissive
fi
fi
AC_SUBST(PERMISSIVE)
])dnl
dnl
dnl
dnl ICE_X_PERMISSIVE
dnl ----------------
dnl
dnl If the C++ compiler wants `-fpermissive' to compile X headers,
dnl set output variable `X_PERMISSIVE' to `-fpermissive',
dnl empty otherwise.
dnl
AC_DEFUN(ICE_X_PERMISSIVE,
[
AC_REQUIRE([ICE_PERMISSIVE])
if test "$GXX" = yes; then
if test "$PERMISSIVE" != ""; then
AC_MSG_CHECKING(whether compiling X headers requires $PERMISSIVE)
AC_CACHE_VAL(ice_cv_x_permissive,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([
#include <Xm/Xm.h>
],[int a;],
ice_cv_x_permissive=no, ice_cv_x_permissive=maybe)
if test "$ice_cv_x_permissive" = maybe; then
ice_save_cxxflags="$CXXFLAGS"
CXXFLAGS="$CXXFLAGS $PERMISSIVE"
AC_TRY_COMPILE([
#include <Xm/Xm.h>
],[int a;],
ice_cv_x_permissive=yes, ice_cv_x_permissive=no)
CXXFLAGS="$ice_save_cxxflags"
fi
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_x_permissive)
if test "$ice_cv_x_permissive" = yes; then
X_PERMISSIVE=$PERMISSIVE
fi
fi
fi
AC_SUBST(X_PERMISSIVE)
])dnl
dnl
dnl
dnl ICE_NO_IMPLICIT_TEMPLATES
dnl -------------------------
dnl
dnl If the C++ compiler accepts the `-fno-implicit-templates' flag,
dnl set output variable `NO_IMPLICIT_TEMPLATES' to `-fno-implicit-templates',
dnl empty otherwise.
dnl
AC_DEFUN(ICE_NO_IMPLICIT_TEMPLATES,
[
AC_REQUIRE([AC_PROG_CXX])
if test "$GXX" = yes; then
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) accepts -fno-implicit-templates)
AC_CACHE_VAL(ice_cv_no_implicit_templates,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_save_cxxflags="$CXXFLAGS"
CXXFLAGS="-fno-implicit-templates"
AC_TRY_COMPILE(,[int a;],
ice_cv_no_implicit_templates=yes, ice_cv_no_implicit_templates=no)
CXXFLAGS="$ice_save_cxxflags"
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_no_implicit_templates)
if test "$ice_cv_no_implicit_templates" = yes; then
NO_IMPLICIT_TEMPLATES=-fno-implicit-templates
fi
fi
AC_SUBST(NO_IMPLICIT_TEMPLATES)
])dnl
dnl
dnl
dnl ICE_ELIDE_CONSTRUCTORS
dnl ----------------------
dnl
dnl If the C++ compiler accepts the `-felide-constructors' flag,
dnl set output variable `ELIDE_CONSTRUCTORS' to `-felide-constructors',
dnl empty otherwise.
dnl
AC_DEFUN(ICE_ELIDE_CONSTRUCTORS,
[
AC_REQUIRE([AC_PROG_CXX])
if test "$GXX" = yes; then
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) accepts -felide-constructors)
AC_CACHE_VAL(ice_cv_elide_constructors,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_save_cxxflags="$CXXFLAGS"
CXXFLAGS="-felide-constructors"
AC_TRY_COMPILE(,[int a;],
ice_cv_elide_constructors=yes, ice_cv_elide_constructors=no)
CXXFLAGS="$ice_save_cxxflags"
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_elide_constructors)
if test "$ice_cv_elide_constructors" = yes; then
ELIDE_CONSTRUCTORS=-felide-constructors
fi
fi
AC_SUBST(ELIDE_CONSTRUCTORS)
])dnl
dnl
dnl ICE_CONSERVE_SPACE
dnl ------------------
dnl
dnl If the C++ compiler accepts the `-fconserve-space' flag,
dnl set output variable `CONSERVE_SPACE' to `-fconserve-space',
dnl empty otherwise.
dnl
AC_DEFUN(ICE_CONSERVE_SPACE,
[
AC_REQUIRE([AC_PROG_CXX])
if test "$GXX" = yes; then
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) accepts -fconserve-space)
AC_CACHE_VAL(ice_cv_conserve_space,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_save_cxxflags="$CXXFLAGS"
CXXFLAGS="-fconserve-space"
AC_TRY_COMPILE(,[int a;],
ice_cv_conserve_space=yes, ice_cv_conserve_space=no)
CXXFLAGS="$ice_save_cxxflags"
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_conserve_space)
if test "$ice_cv_conserve_space" = yes; then
CONSERVE_SPACE=-fconserve-space
fi
fi
AC_SUBST(CONSERVE_SPACE)
])dnl
dnl
dnl
dnl
dnl ICE_WARN_EFFECTIVE_CXX
dnl ----------------------
dnl
dnl If the C++ compiler accepts the `-Weffc++' flag,
dnl set output variable `WARN_EFFECTIVE_CXX' to `-Weffc++' and
dnl `WARN_NO_EFFECTIVE_CXX' to `-Wno-effc++'.  Otherwise,
dnl leave both empty.
dnl
AC_DEFUN(ICE_WARN_EFFECTIVE_CXX,
[
AC_REQUIRE([AC_PROG_CXX])
if test "$GXX" = yes; then
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) accepts -Weffc++)
AC_CACHE_VAL(ice_cv_warn_effective_cxx,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_save_cxxflags="$CXXFLAGS"
CXXFLAGS="-Weffc++"
AC_TRY_COMPILE(,[int a;],
ice_cv_warn_effective_cxx=yes, ice_cv_warn_effective_cxx=no)
CXXFLAGS="$ice_save_cxxflags"
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_warn_effective_cxx)
if test "$ice_cv_warn_effective_cxx" = yes; then
WARN_EFFECTIVE_CXX=-Weffc++
WARN_NO_EFFECTIVE_CXX=-Wno-effc++
fi
fi
AC_SUBST(WARN_EFFECTIVE_CXX)
AC_SUBST(WARN_NO_EFFECTIVE_CXX)
])dnl
dnl
dnl ICE_TRIGRAPHS
dnl -------------
dnl
dnl If the C++ compiler accepts the `-trigraphs' flag,
dnl set output variable `TRIGRAPHS' to `-trigraphs'.  Otherwise,
dnl leave it empty.
dnl
dnl Note: I'm not fond of trigraphs, but enabling `-trigraphs' allows us
dnl to detect incompatibilities with other C++ compilers
dnl
AC_DEFUN(ICE_TRIGRAPHS,
[
AC_REQUIRE([AC_PROG_CXX])
if test "$GXX" = yes; then
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) accepts -trigraphs)
AC_CACHE_VAL(ice_cv_trigraphs,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_save_cxxflags="$CXXFLAGS"
CXXFLAGS="-trigraphs"
AC_TRY_COMPILE(,[int a;],
ice_cv_trigraphs=yes, ice_cv_trigraphs=no)
CXXFLAGS="$ice_save_cxxflags"
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_trigraphs)
if test "$ice_cv_trigraphs" = yes; then
TRIGRAPHS=-trigraphs
fi
fi
AC_SUBST(TRIGRAPHS)
])dnl
dnl
dnl ICE_BIG_TOC
dnl -----------
dnl
dnl If the C++ compiler supports `-Wl,-bbigtoc' (as required for large
dnl programs on RS/6000 and PowerPC) set BIG_TOC to `-Wl,-bbigtoc'.
dnl An alternative to this is using `-mminimal-toc' when compiling.
dnl
AC_DEFUN(ICE_BIG_TOC,
[
AC_REQUIRE([AC_PROG_CXX])
if test "$GXX" = yes; then
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) accepts [-Wl,-bbigtoc])
AC_CACHE_VAL(ice_cv_big_toc,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_save_ldflags="$LDFLAGS"
LDFLAGS="-Wl,-bbigtoc"
AC_TRY_LINK(,[int a;],
ice_cv_big_toc=yes, ice_cv_big_toc=no)
LDFLAGS="$ice_save_ldflags"
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_big_toc)
if test "$ice_cv_big_toc" = yes; then
BIG_TOC="-Wl,-bbigtoc"
fi
fi
AC_SUBST(BIG_TOC)
])dnl
dnl
dnl
dnl ICE_MINIMAL_TOC
dnl ---------------
dnl
dnl If the C++ compiler `-mminimal-toc' (as required for large
dnl programs on RS/6000 and PowerPC) set MINIMAL_TOC to `-mminimal-toc'.
dnl An alternative to this is using `-Wl,-bbigtoc' when linking.
dnl
AC_DEFUN(ICE_MINIMAL_TOC,
[
AC_REQUIRE([AC_PROG_CXX])
if test "$GXX" = yes; then
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) accepts [-mminimal-toc])
AC_CACHE_VAL(ice_cv_minimal_toc,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_save_cxxflags="$CXXFLAGS"
CXXFLAGS="-mminimal-toc"
AC_TRY_LINK(,[int a;],
ice_cv_minimal_toc=yes, ice_cv_minimal_toc=no)
CXXFLAGS="$ice_save_cxxflags"
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_minimal_toc)
if test "$ice_cv_minimal_toc" = yes; then
MINIMAL_TOC="-mminimal-toc"
fi
fi
AC_SUBST(MINIMAL_TOC)
])dnl
dnl
dnl
dnl ICE_RPATH
dnl ---------
dnl
dnl If the C++ compiler supports `-Wl,-rpath,PATH', set ice_rpath to `yes'.
dnl
AC_DEFUN(ICE_RPATH,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) accepts [-Wl,-rpath,PATH])
AC_CACHE_VAL(ice_cv_rpath,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_save_ldflags="$LDFLAGS"
LDFLAGS="-Wl,-rpath,/usr/lib"
AC_TRY_LINK(,[int a;],
ice_cv_rpath=yes, ice_cv_rpath=no)
LDFLAGS="$ice_save_ldflags"
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_rpath)
ice_rpath="$ice_cv_rpath"
])dnl
dnl
dnl
dnl ICE_WARN_UNINITIALIZED
dnl ----------------------
dnl
dnl If the C++ compiler accepts the `-Wuninitialized' flag,
dnl set output variable `WARN_UNINITIALIZED' to `-Wuninitialized'
dnl and `WARN_NO_UNINITIALIZED' to `-Wno-uninitialized'.  Otherwise,
dnl leave both empty.
dnl
AC_DEFUN(ICE_WARN_UNINITIALIZED,
[
AC_REQUIRE([AC_PROG_CXX])
if test "$GXX" = yes; then
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) accepts -Wuninitialized)
AC_CACHE_VAL(ice_cv_warn_uninitialized,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_save_cxxflags="$CXXFLAGS"
CXXFLAGS="-Wuninitialized"
AC_TRY_COMPILE(,[int a;],
ice_cv_warn_uninitialized=yes, ice_cv_warn_uninitialized=no)
CXXFLAGS="$ice_save_cxxflags"
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_warn_uninitialized)
if test "$ice_cv_warn_uninitialized" = yes; then
WARN_UNINITIALIZED=-Wuninitialized
WARN_NO_UNINITIALIZED=-Wno-uninitialized
fi
fi
AC_SUBST(WARN_UNINITIALIZED)
AC_SUBST(WARN_NO_UNINITIALIZED)
])dnl
dnl
dnl
dnl
dnl ICE_XS_DEBUG_INFO
dnl -----------------
dnl
dnl If the C++ compiler accepts the `-xs' flag (as Sun CC)
dnl set output variable `XS_DEBUG_INFO' to `-xs'.  Otherwise, 
dnl leave it empty.
dnl
AC_DEFUN(ICE_XS_DEBUG_INFO,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) accepts -xs)
AC_CACHE_VAL(ice_cv_xs_debug_info,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_save_cxxflags="$CXXFLAGS"
CXXFLAGS="-xs"
AC_TRY_COMPILE(,[int a;],
ice_cv_xs_debug_info=yes, ice_cv_xs_debug_info=no)
CXXFLAGS="$ice_save_cxxflags"
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_xs_debug_info)
if test "$ice_cv_xs_debug_info" = yes; then
XS_DEBUG_INFO=-xs
fi
AC_SUBST(XS_DEBUG_INFO)
])dnl
dnl
dnl
dnl
dnl ICE_CXX_ISYSTEM
dnl ---------------
dnl
dnl If the C++ compiler accepts the `-isystem PATH' flag,
dnl set output variable `ISYSTEM' to `-isystem ', `-I' otherwise.
dnl
AC_DEFUN(ICE_CXX_ISYSTEM,
[
AC_REQUIRE([AC_PROG_CXX])
if test "$GXX" = yes; then
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) accepts -isystem)
AC_CACHE_VAL(ice_cv_cxx_isystem,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_save_cxxflags="$CXXFLAGS"
CXXFLAGS="-isystem ."
AC_TRY_COMPILE(,[int a;],
ice_cv_cxx_isystem=yes, ice_cv_cxx_isystem=no)
CXXFLAGS="$ice_save_cxxflags"
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_cxx_isystem)
if test "$ice_cv_cxx_isystem" = yes; then
ISYSTEM="-isystem "
else
ISYSTEM="-I"
fi
else
dnl No GNU compiler
ISYSTEM="-I"
fi
AC_SUBST(ISYSTEM)
])dnl
dnl
dnl
dnl ICE_CXX_NEW_CASTS
dnl ----------------
dnl
dnl If the C++ compiler accepts the c++ style casts, define `HAVE_NEW_CASTS'.
dnl
AC_DEFUN(ICE_CXX_NEW_CASTS,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) supports new style casts)
AC_CACHE_VAL(ice_cv_have_new_casts,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE(,[double *x = 0; char *s = reinterpret_cast<char*>(x);],
ice_cv_have_new_casts=yes,
ice_cv_have_new_casts=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_new_casts)
if test "$ice_cv_have_new_casts" = yes; then
AC_DEFINE(HAVE_NEW_CASTS)
fi
])dnl
dnl
dnl
dnl
dnl
dnl
dnl ICE_CXX_PROBLEMATIC_VERSION
dnl ---------------------------
dnl
dnl If this is GNU C++ earlier than 2.5, issue a warning.
dnl
AC_DEFUN(ICE_CXX_PROBLEMATIC_VERSION,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(if this is a problematic ${CXX} version)
AC_CACHE_VAL(ice_cv_cxx_problematic_version,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_CPP([
#ifdef __GNUC__
#if __GNUC__ < 2
#error ICE works best with GCC version 2.5 or higher
#endif /* __GNUC__ < 2 */
#ifdef __GNUC_MINOR__
#if __GNUC__ == 2 && __GNUC_MINOR__ < 5
#error ICE works best with GCC version 2.5 or higher
#endif /* __GNUC_MINOR__ < 5 */
#endif /* defined(__GNUC_MINOR__) */
#endif /* defined(__GNUC__) */
],
ice_cv_cxx_problematic_version=no, ice_cv_cxx_problematic_version=yes)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_cxx_problematic_version)
if test "$ice_cv_cxx_problematic_version" = yes; then
AC_MSG_WARN(*** This package works best with ${CXX} version 2.5 or higher ***)
fi
])dnl
dnl
dnl
dnl
dnl ICE_CXX_BOOL
dnl ------------
dnl
dnl If the C++ compiler accepts the `bool' keyword, define `HAVE_BOOL'.
dnl
AC_DEFUN(ICE_CXX_BOOL,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) supports bool types)
AC_CACHE_VAL(ice_cv_have_bool,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE(,[bool b = true;],
ice_cv_have_bool=yes,
ice_cv_have_bool=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_bool)
if test "$ice_cv_have_bool" = yes; then
AC_DEFINE(HAVE_BOOL)
fi
])dnl
dnl
dnl
dnl
dnl ICE_CXX_EXPLICIT
dnl ----------------
dnl
dnl If the C++ compiler accepts the `explicit' keyword, define `HAVE_EXPLICIT'.
dnl
AC_DEFUN(ICE_CXX_EXPLICIT,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) supports explicit constructors)
AC_CACHE_VAL(ice_cv_have_explicit,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE(,[class A { public: explicit A(int) {} };],
ice_cv_have_explicit=yes,
ice_cv_have_explicit=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_explicit)
if test "$ice_cv_have_explicit" = yes; then
AC_DEFINE(HAVE_EXPLICIT)
fi
])dnl
dnl
dnl
dnl
dnl ICE_CXX_MUTABLE
dnl ----------------
dnl
dnl If the C++ compiler accepts the `mutable' keyword, define `HAVE_MUTABLE'.
dnl
AC_DEFUN(ICE_CXX_MUTABLE,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) supports mutable members)
AC_CACHE_VAL(ice_cv_have_mutable,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE(,[struct A { mutable int x; };],
ice_cv_have_mutable=yes,
ice_cv_have_mutable=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_mutable)
if test "$ice_cv_have_mutable" = yes; then
AC_DEFINE(HAVE_MUTABLE)
fi
])dnl
dnl
dnl
dnl
dnl ICE_CXX_NAMESPACE
dnl -----------------
dnl
dnl If the C++ compiler supports namespaces, define `HAVE_NAMESPACE'.
dnl
AC_DEFUN(ICE_CXX_NAMESPACE,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) supports namespaces)
AC_CACHE_VAL(ice_cv_have_namespace,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([
namespace one { 
    extern int f(); 
};

int one::f() { return 1; };

using namespace one;
],[f()],
ice_cv_have_namespace=yes,
ice_cv_have_namespace=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_namespace)
if test "$ice_cv_have_namespace" = yes; then
AC_DEFINE(HAVE_NAMESPACE)
fi
])dnl
dnl
dnl
dnl ICE_CXX_EXPLICIT_TEMPLATE_INSTANTIATION
dnl ---------------------------------------
dnl
dnl If the C++ compiler supports explicit template instantiation, 
dnl define `HAVE_EXPLICIT_TEMPLATE_INSTANTIATION'.
dnl
AC_DEFUN(ICE_CXX_EXPLICIT_TEMPLATE_INSTANTIATION,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) supports explicit template instantiation)
AC_CACHE_VAL(ice_cv_have_explicit_template_instantiation,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([
template <class T> class Pointer { public: T *value; };
template class Pointer<char>;
], [/* empty */],
ice_cv_have_explicit_template_instantiation=yes,
ice_cv_have_explicit_template_instantiation=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_explicit_template_instantiation)
if test "$ice_cv_have_explicit_template_instantiation" = yes; then
AC_DEFINE(HAVE_EXPLICIT_TEMPLATE_INSTANTIATION)
fi
])dnl
dnl
dnl
dnl ICE_CXX_PRETTY_FUNCTION
dnl -----------------------
dnl
dnl If the C++ compiler supports the __PRETTY_FUNCTION__ macro,
dnl define `HAVE_PRETTY_FUNCTION'.
dnl
AC_DEFUN(ICE_CXX_PRETTY_FUNCTION,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) supports __PRETTY_FUNCTION__)
AC_CACHE_VAL(ice_cv_have_pretty_function,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
# Testing compilation alone does not suffice --
# some GCC versions have trouble in linking.
AC_TRY_LINK([#include <stdio.h>],
[puts(__PRETTY_FUNCTION__);],
ice_cv_have_pretty_function=yes,
ice_cv_have_pretty_function=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_pretty_function)
if test "$ice_cv_have_pretty_function" = yes; then
AC_DEFINE(HAVE_PRETTY_FUNCTION)
fi
])dnl
dnl
dnl
dnl ICE_CXX_ARRAY_OPERATOR_NEW
dnl --------------------------
dnl
dnl If the C++ compiler supports overloading operator new[],
dnl define `HAVE_ARRAY_OPERATOR_NEW'.
dnl
AC_DEFUN(ICE_CXX_ARRAY_OPERATOR_NEW,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING([whether the C++ compiler (${CXX}) supports overloaded new])
AC_CACHE_VAL(ice_cv_have_array_operator_new,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([#include <sys/types.h>
void *operator new[](size_t nbytes);],
/* empty */,
ice_cv_have_array_operator_new=yes,
ice_cv_have_array_operator_new=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_array_operator_new)
if test "$ice_cv_have_array_operator_new" = yes; then
AC_DEFINE(HAVE_ARRAY_OPERATOR_NEW)
fi
])dnl
dnl
dnl ICE_STREAMPOS
dnl -------------
dnl
dnl If <iostream.h> or <fstream.h> define a `streampos' type 
dnl (as type of os.tellp(), etc.), define `HAVE_STREAMPOS'.
dnl In DDD, see `streampos.h' for details.
dnl
AC_DEFUN(ICE_STREAMPOS,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING([for streampos])
AC_CACHE_VAL(ice_cv_have_streampos,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([#include <iostream>
#include <fstream>],
[streampos p;],
ice_cv_have_streampos=yes,
ice_cv_have_streampos=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_streampos)
if test "$ice_cv_have_streampos" = yes; then
AC_DEFINE(HAVE_STREAMPOS)
fi
])dnl
dnl
dnl
dnl If <iostream.h> or <fstream.h> define a `std::streampos' type 
dnl (as type of os.tellp(), etc.), define `HAVE_STD_STREAMPOS'.
dnl (Background: In GCC 3.0, `streampos' does not work, but 
dnl `std::streampos' does...)
dnl In DDD, see `streampos.h' for details.
dnl
AC_DEFUN(ICE_STD_STREAMPOS,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING([for std::streampos])
AC_CACHE_VAL(ice_cv_have_std_streampos,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([#include <iostream>
#include <fstream>],
[std::streampos p;],
ice_cv_have_std_streampos=yes,
ice_cv_have_std_streampos=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_std_streampos)
if test "$ice_cv_have_std_streampos" = yes; then
AC_DEFINE(HAVE_STD_STREAMPOS)
fi
])dnl
dnl
dnl ICE_IOSTATE
dnl -----------
dnl
dnl If <iostream.h> defines an `ios::iostate' type (as type of os.rdstate(), 
dnl os.clear(), etc.), define `HAVE_IOSTATE'.
dnl
AC_DEFUN(ICE_IOSTATE,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING([for ios::iostate])
AC_CACHE_VAL(ice_cv_have_iostate,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([#include <iostream>],
[ios::iostate new_state;],
ice_cv_have_iostate=yes,
ice_cv_have_iostate=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_iostate)
if test "$ice_cv_have_iostate" = yes; then
AC_DEFINE(HAVE_IOSTATE)
fi
])dnl
dnl
dnl
dnl
dnl
dnl ICE_CXX_PLACEMENT_NEW
dnl ---------------------
dnl
dnl If the C++ compiler supports placement new,
dnl define `HAVE_PLACEMENT_NEW'.
dnl
AC_DEFUN(ICE_CXX_PLACEMENT_NEW,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING([whether the C++ compiler (${CXX}) supports placement new])
AC_CACHE_VAL(ice_cv_have_placement_new,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([#include <new>],
[int *pi = new (operator new (sizeof(int))) int;],
ice_cv_have_placement_new=yes,
ice_cv_have_placement_new=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_placement_new)
if test "$ice_cv_have_placement_new" = yes; then
AC_DEFINE(HAVE_PLACEMENT_NEW)
fi
])dnl
dnl
dnl
dnl ICE_CXX_LIFETIME_OF_TEMPORARIES
dnl -------------------------------
dnl
dnl If the C++ compiler realizes ANSI C++ working paper conformant
dnl lifetime of temporaries, define `HAVE_ANSI_LIFETIME_OF_TEMPORARIES'.
dnl
AC_DEFUN(ICE_CXX_LIFETIME_OF_TEMPORARIES,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) supports ANSI lifetime of temporaries)
AC_CACHE_VAL(ice_cv_have_ansi_lifetime_of_temporaries,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_RUN(
[
// This program returns 0 if compiled with ANSI C++ compliant 
// lifetime of temporaries; 1, otherwise.
static int destructor_called = 0;
struct X {
    int dummy;
    X(int)         {}
    X(const X&)    {}
    ~X()           { destructor_called = 1; }
};
int f(const X&)    { return destructor_called; }
X g(const X& x)    { return x; }
X h()              { return 1; }
int main()         { return f(g(h())); }
], 
ice_cv_have_ansi_lifetime_of_temporaries=yes, 
ice_cv_have_ansi_lifetime_of_temporaries=no,
[
if test "$GXX" = yes; then
# Cross-compiling with GNU C++: Guess on the version number
AC_TRY_CPP([
/* Check for GCC versions lower than 2.6 */
#ifdef __GNUC__
#if __GNUC__ < 2
#error
#endif
#if !defined(__GNUC_MINOR__)
#error
#endif
#if __GNUC_MINOR__ < 6
#error
#endif
#endif
],
ice_cv_have_ansi_lifetime_of_temporaries=yes,
ice_cv_have_ansi_lifetime_of_temporaries=no)
else
# Cross-compiling and not GNU C++: Play it safe.
ice_cv_have_ansi_lifetime_of_temporaries=no
fi
])
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_ansi_lifetime_of_temporaries)
if test "$ice_cv_have_ansi_lifetime_of_temporaries" = yes; then
AC_DEFINE(HAVE_ANSI_LIFETIME_OF_TEMPORARIES)
fi
])dnl
##!dnl
##!dnl ICE_OSTRSTREAM_PCOUNT_BROKEN
##!dnl ----------------------------
##!dnl
##!dnl If the C++ ostrstream::pcount() is increased by one after calling
##!dnl ostrstream::str() (as in the SGI C++ I/O library), 
##!dnl define `OSTRSTREAM_PCOUNT_BROKEN'.
##!dnl
##!AC_DEFUN(ICE_OSTRSTREAM_PCOUNT_BROKEN,
##![
##!AC_REQUIRE([AC_PROG_CXX])
##!AC_MSG_CHECKING(whether ostrstream::pcount() is broken)
##!AC_CACHE_VAL(ice_cv_ostrstream_pcount_broken,
##![
##!AC_LANG_SAVE
##!AC_LANG_CPLUSPLUS
##!AC_TRY_RUN(
##![
##!// Returns 0 if ostrstream::pcount() is broken; 1, otherwise.
##!#include <iostream>
##!#include <strstream>
##!
##!int main() 
##!{
##!    std::ostrstream os;
##!    os << 'a';           // os.pcount() == 1.
##!    char *s = os.str();  // In the SGI C++ I/O library, os.pcount() is now 2!
##!    return (os.pcount() == 2) ? 0 : 1;
##!}
##!], 
##!ice_cv_ostrstream_pcount_broken=yes,
##!ice_cv_ostrstream_pcount_broken=no,
##!ice_cv_ostrstream_pcount_broken=no
##!)
##!AC_LANG_RESTORE
##!])
##!AC_MSG_RESULT($ice_cv_ostrstream_pcount_broken)
##!if test "$ice_cv_ostrstream_pcount_broken" = yes; then
##!AC_DEFINE(OSTRSTREAM_PCOUNT_BROKEN)
##!fi
##!])dnl
dnl
dnl
dnl ICE_CXX_LONG_LONG
dnl -----------------
dnl
dnl If the C++ compiler supports `long long' types,  define `HAVE_LONG_LONG'.
dnl
AC_DEFUN(ICE_CXX_LONG_LONG,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) supports long long types)
AC_CACHE_VAL(ice_cv_have_long_long,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE(,[long long a;], 
ice_cv_have_long_long=yes, 
ice_cv_have_long_long=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_long_long)
if test "$ice_cv_have_long_long" = yes; then
AC_DEFINE(HAVE_LONG_LONG)
fi
])dnl
dnl
dnl
dnl ICE_CXX_EXCEPTIONS
dnl ------------------
dnl
dnl If the C++ compiler handles exceptions, define `HAVE_EXCEPTIONS'.
dnl
AC_DEFUN(ICE_CXX_EXCEPTIONS,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) supports exception handling)
AC_CACHE_VAL(ice_cv_have_exceptions,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE(,[try { throw 1; } catch(int) { }],
ice_cv_have_exceptions=yes,
ice_cv_have_exceptions=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_exceptions)
if test "$ice_cv_have_exceptions" = yes; then
AC_DEFINE(HAVE_EXCEPTIONS)
fi
])dnl
dnl
dnl
dnl ICE_CXX_STD_EXCEPTIONS
dnl ----------------------------
dnl
dnl If the C++ compiler handles ISO C++ std exceptions, 
dnl define `HAVE_STD_EXCEPTIONS'.
dnl
AC_DEFUN(ICE_CXX_STD_EXCEPTIONS,
[
AC_REQUIRE([ICE_CXX_EXCEPTIONS])
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) supports standard exceptions)
AC_CACHE_VAL(ice_cv_have_std_exceptions,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
dnl On SGI, EGCS <exception> included by <stdexcept> conflicts with <math.h>.
dnl Check for this.
AC_TRY_COMPILE([
#include <stdexcept>
#include <math.h>
],
[try { throw std::runtime_error("too many fingers on keyboard"); }
 catch(const std::exception& e) { const char *s = e.what(); }],
ice_cv_have_std_exceptions=yes,
ice_cv_have_std_exceptions=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_std_exceptions)
if test "$ice_cv_have_std_exceptions" = yes; then
AC_DEFINE(HAVE_STD_EXCEPTIONS)
fi
])dnl
dnl
dnl
dnl ICE_CXX_TYPEINFO
dnl ----------------
dnl
dnl If the C++ compiler supports run-time type info, define `HAVE_TYPEINFO'.
dnl
AC_DEFUN(ICE_CXX_TYPEINFO,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether the C++ compiler (${CXX}) supports run-time type info)
AC_CACHE_VAL(ice_cv_have_typeinfo,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
dnl On SGI, EGCS <exception> included by <typeinfo> conflicts with <math.h>.
dnl Check for this.
AC_TRY_COMPILE([
#include <typeinfo>
#include <math.h>
],
[int x; const char *s = typeid(x).name();],
ice_cv_have_typeinfo=yes,
ice_cv_have_typeinfo=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_typeinfo)
if test "$ice_cv_have_typeinfo" = yes; then
AC_DEFINE(HAVE_TYPEINFO)
fi
])dnl
dnl
dnl
dnl ICE_PROG_CPP_TRADITIONAL
dnl ------------------------
dnl
dnl Set output variable `CPP_TRADITIONAL' to a command that runs a 
dnl "traditional" C preprocessor (that is, pre-ANSI-C).
dnl Try each one of `$CPP', `$CC -E', `/lib/cpp' either without flags
dnl or with `-traditional-cpp' or with `-traditional'.
dnl
AC_DEFUN(ICE_PROG_CPP_TRADITIONAL,
[
AC_REQUIRE([AC_PROG_CPP])
AC_MSG_CHECKING(for a traditional C preprocessor)
AC_CACHE_VAL(ice_cv_traditional_cpp,
[
cat > conftest.c << EOF
#if 1
"this is an unterminated string
#endif
EOF
ice_cv_traditional_cpp=
ice_save_cpp="$CPP"
ice_save_cppflags="$CPPFLAGS"
for ice_cpp in "$CPP" "$CC -E" "/lib/cpp"; do
for ice_cppflags in '' ' -traditional-cpp' ' -traditional'; do
CPP="$ice_cpp"
CPPFLAGS="$ice_cppflags"
AC_TRY_CPP([
#if 1
"this is an unterminated string
#endif
], ice_cv_traditional_cpp="${CPP}${CPPFLAGS}")
if test "$ice_cv_traditional_cpp" != ""; then
break 2
fi
done
done
CPP="$ice_save_cpp"
CPPFLAGS="$ice_save_cppflags"
])
AC_MSG_RESULT($ice_cv_traditional_cpp)
CPP_TRADITIONAL="$ice_cv_traditional_cpp"
AC_SUBST(CPP_TRADITIONAL)
])dnl
dnl
dnl
dnl ICE_CHECK_PATH_MAX
dnl ------------------
dnl
dnl Define `HAVE_PATH_MAX' if PATH_MAX is defined in <limits.h>
dnl
AC_DEFUN(ICE_CHECK_PATH_MAX,
[
AC_MSG_CHECKING(for PATH_MAX definition in limits.h)
AC_CACHE_VAL(ice_cv_have_path_max,
[
AC_TRY_COMPILE([#include <limits.h>], [int n = PATH_MAX;],
ice_cv_have_path_max=yes,
ice_cv_have_path_max=no)
])
AC_MSG_RESULT($ice_cv_have_path_max)
if test "$ice_cv_have_path_max" = yes; then
AC_DEFINE(HAVE_PATH_MAX)
fi
])dnl
dnl
dnl
dnl ICE_CHECK_POSIX_PATH_MAX
dnl ------------------------
dnl
dnl Define `HAVE_POSIX_PATH_MAX' if _POSIX_PATH_MAX is defined in <limits.h>
dnl
AC_DEFUN(ICE_CHECK_POSIX_PATH_MAX,
[
AC_MSG_CHECKING(for _POSIX_PATH_MAX definition in limits.h)
AC_CACHE_VAL(ice_cv_have_posix_path_max,
[
AC_TRY_COMPILE([#include <limits.h>], [int n = _POSIX_PATH_MAX],
ice_cv_have_posix_path_max=yes,
ice_cv_have_posix_path_max=no)
])
AC_MSG_RESULT($ice_cv_have_posix_path_max)
if test "$ice_cv_have_posix_path_max" = yes; then
AC_DEFINE(HAVE_POSIX_PATH_MAX)
fi
])dnl
dnl
dnl
dnl ICE_CHECK_MAXPATHLEN
dnl --------------------
dnl
dnl Define `HAVE_MAXPATHLEN' if MAXPATHLEN is defined in <limits.h>
dnl
AC_DEFUN(ICE_CHECK_MAXPATHLEN,
[
AC_MSG_CHECKING(for MAXPATHLEN definition in sys/param.h)
AC_CACHE_VAL(ice_cv_have_maxpathlen,
[
AC_TRY_COMPILE([#include <sys/param.h>], [int n = MAXPATHLEN;],
ice_cv_have_maxpathlen=yes,
ice_cv_have_maxpathlen=no)
])
AC_MSG_RESULT($ice_cv_have_maxpathlen)
if test "$ice_cv_have_maxpathlen" = yes; then
AC_DEFINE(HAVE_MAXPATHLEN)
fi
])dnl
dnl
dnl
dnl ICE_CHECK_DECL (FUNCTION, HEADER-FILE...)
dnl -----------------------------------------
dnl
dnl If FUNCTION is available, define `HAVE_FUNCTION'.  If it is declared
dnl in one of the headers named in the whitespace-separated list 
dnl HEADER_FILE, define `HAVE_FUNCTION_DECL` (in all capitals).
dnl
AC_DEFUN(ICE_CHECK_DECL,
[
changequote(,)dnl
ice_tr=`echo $1 | tr '[a-z]' '[A-Z]'`
changequote([,])dnl
ice_have_tr=HAVE_$ice_tr
ice_have_decl_tr=${ice_have_tr}_DECL
ice_have_$1=no
dnl Have to switch to C for this, or autoconf will include <stdlib.h>
dnl in the test which breaks lots of things.
AC_LANG_SAVE
AC_LANG_C
AC_CHECK_FUNCS($1, ice_have_$1=yes)
AC_LANG_RESTORE
if test "${ice_have_$1}" = yes; then
AC_MSG_CHECKING(for $1 declaration in $2)
AC_CACHE_VAL(ice_cv_have_$1_decl,
[
ice_cv_have_$1_decl=no
changequote(,)dnl
ice_re_params='[a-zA-Z_][a-zA-Z0-9_]*'
ice_re_word='(^|[^a-zA-Z_0-9_])'
changequote([,])dnl
for header in $2; do
# Check for ordinary declaration
AC_EGREP_HEADER([${ice_re_word}$1 *\(], $header, 
	ice_cv_have_$1_decl=yes)
if test "$ice_cv_have_$1_decl" = yes; then
	break
fi
# Check for "fixed" declaration like "getpid _PARAMS((int))"
AC_EGREP_HEADER([${ice_re_word}$1 *$ice_re_params\(\(], $header, 
	ice_cv_have_$1_decl=yes)
if test "$ice_cv_have_$1_decl" = yes; then
	break
fi
done
])
AC_MSG_RESULT($ice_cv_have_$1_decl)
if test "$ice_cv_have_$1_decl" = yes; then
AC_DEFINE_UNQUOTED(${ice_have_decl_tr})
fi
fi
])dnl
dnl
dnl
dnl ICE_CHECK_LESSTIF
dnl -----------------
dnl
dnl Define `HAVE_LESSTIF' if the Motif library is actually a LessTif library
dnl
AC_DEFUN(ICE_CHECK_LESSTIF,
[
AC_MSG_CHECKING(whether the Motif library is actually a LessTif library)
AC_CACHE_VAL(ice_cv_have_lesstif,
AC_EGREP_CPP(yes,
[#include <Xm/Xm.h>
#ifdef LesstifVersion
yes
#endif
], ice_cv_have_lesstif=yes, ice_cv_have_lesstif=no))
AC_MSG_RESULT($ice_cv_have_lesstif)
if test "$ice_cv_have_lesstif" = yes; then
AC_DEFINE(HAVE_LESSTIF)
fi
])dnl
dnl
dnl
dnl
dnl ICE_CHECK_PTRACE_DUMPCORE
dnl -------------------------
dnl
dnl Set `HAVE_PTRACE_DUMPCORE' if PTRACE_DUMPCORE is defined in <sys/ptrace.h>.
dnl
AC_DEFUN(ICE_CHECK_PTRACE_DUMPCORE,
[
AC_MSG_CHECKING(for PTRACE_DUMPCORE definition in <sys/ptrace.h>)
AC_CACHE_VAL(ice_cv_have_ptrace_dumpcore,
[
AC_TRY_COMPILE([#include <sys/ptrace.h>], [int request = PTRACE_DUMPCORE;],
ice_cv_have_ptrace_dumpcore=yes,
ice_cv_have_ptrace_dumpcore=no)
])
AC_MSG_RESULT($ice_cv_have_ptrace_dumpcore)
if test "$ice_cv_have_ptrace_dumpcore" = yes; then
AC_DEFINE(HAVE_PTRACE_DUMPCORE)
fi
])dnl
dnl
dnl
dnl
dnl ICE_CHECK_CORE_MAGIC
dnl --------------------
dnl
dnl Set `HAVE_CORE_MAGIC' if CORE_MAGIC is defined in <sys/core.h>.
dnl
AC_DEFUN(ICE_CHECK_CORE_MAGIC,
[
AC_MSG_CHECKING(for CORE_MAGIC definition in <sys/core.h>)
AC_CACHE_VAL(ice_cv_have_core_magic,
AC_EGREP_CPP(yes,
[#include <sys/core.h>
#ifdef CORE_MAGIC
yes
#endif
], ice_cv_have_core_magic=yes, ice_cv_have_core_magic=no))
AC_MSG_RESULT($ice_cv_have_core_magic)
if test "$ice_cv_have_core_magic" = yes; then
AC_DEFINE(HAVE_CORE_MAGIC)
fi
])dnl
dnl
dnl
dnl
dnl ICE_CXX_INCLUDE_DIR
dnl -------------------
dnl
dnl Set output variable CXX_INCLUDE_DIR to the name of a directory
dnl where the C++ compiler looks for C++ include files.
dnl
AC_DEFUN(ICE_CXX_INCLUDE_DIR,
[
AC_MSG_CHECKING(for directory to install c++ include files)
AC_CACHE_VAL(ice_cv_cxx_include_dir,
[
cat > conftest.cc << EOF
#include <iostream>
EOF
$CXXCPP $DEFS conftest.cc > conftest.ii 2>&5
if test $? != 0; then
AC_MSG_ERROR(${CXXCPP} could not find iostream.h)
else
ice_file=`grep '^# 1' conftest.ii | grep iostream.h | \
	head -1 | sed 's/^.*"\(.*\)".*$/\1/'`
changequote(,)dnl
ice_cv_cxx_include_dir=`echo $ice_file | sed 's%/[^/][^/]*$%%'`
changequote([,])dnl
fi
if test "$ice_cv_cxx_include_dir" = ""; then
ice_cv_cxx_include_dir="$prefix/include"
for pfx in "$prefix" "$exec_prefix"; do
for dir in "$pfx/lib/g++-include" "$pfx/include/CC" \
    "$pfx/include" /usr/include; do
if test -d "$dir"; then
ice_cv_cxx_include_dir="$dir"
break
fi
done
done
fi
rm -f conftest.cc conftest.ii
])
CXX_INCLUDE_DIR=$ice_cv_cxx_include_dir
AC_MSG_RESULT(${CXX_INCLUDE_DIR})
AC_SUBST(CXX_INCLUDE_DIR)
])dnl
dnl
dnl
dnl ICE_CXX_LIB_DIR
dnl ---------------
dnl
dnl Set output variable CXX_LIB_DIR to the name of a directory
dnl where the C++ compiler looks for C++ libraries.
dnl
AC_DEFUN(ICE_CXX_LIB_DIR,
[
AC_MSG_CHECKING(for directory to install c++ libraries)
AC_CACHE_VAL(ice_cv_cxx_lib_dir,
[
changequote(,)dnl
ice_cv_cxx_lib_dir=`$CXX $DEFS -print-libgcc-file-name 2>&5 | \
	sed 's%/[^/][^/]*$%%'`
changequote([,])dnl
if test "$ice_cv_cxx_lib_dir" = ""; then
for p in $exec_prefix /usr/local /usr; do
if test -d "$p/lib"; then
ice_cv_cxx_lib_dir="$p/lib"
break
fi
done
fi
])
CXX_LIB_DIR=$ice_cv_cxx_lib_dir
AC_MSG_RESULT(${CXX_LIB_DIR})
AC_SUBST(CXX_LIB_DIR)
])dnl
dnl
dnl
dnl ICE_CXX_OPTIONS
dnl ---------------
dnl
dnl Setup C++ compile options.  Specific to DDD and ICE.
dnl
AC_DEFUN(ICE_CXX_OPTIONS,
[
if test "$GXX" = yes; then
  # GCC options.

  # Check warnings
  ICE_WARN_EFFECTIVE_CXX
  ICE_WARN_UNINITIALIZED

  # Check TOC options
  ICE_MINIMAL_TOC
  if test "$ice_cv_minimal_toc" = yes; then
    # Only check for `-Wl,-bbig-toc' if `-mminimal-toc' is supported.
    ICE_BIG_TOC
    if test "$ice_cv_big_toc" = yes; then
      # Prefer `-Wl,-bbig-toc' on `-mminimal-toc'.
      MINIMAL_TOC=
    fi
  fi

  # Setup options
  CXXOPT="-DNDEBUG"
  CXXDEBUG=
  CXXWARNINGS="-W -Wall"
  CXXSTATIC_BINDING="-Bstatic"
  CXXDYNAMIC_BINDING="-Bdynamic"
  CXXSTUFF="${MINIMAL_TOC}"

  for flag in $CXXFLAGS; do
    case $flag in
      -O*) CXXOPT="$CXXOPT $flag";;
      -g*) CXXDEBUG="$flag";;
      -W*) CXXWARNINGS="$CXXWARNINGS $flag";;
      *)   CXXSTUFF="$CXXSTUFF $flag";;
    esac
  done
else
  # Other options.

  case "$CXX" in
    *CC)
      case "$host_os" in
	*solaris*)
	  # We're using CC on Solaris.  Debugging with GDB requires -xs.
	  ICE_XS_DEBUG_INFO
	  ;;
	*)
	  ;;
      esac
      ;;
    *)
      ;;
  esac

  CXXOPT="-DNDEBUG"
  CXXDEBUG="${XS_DEBUG_INFO}"
  CXXWARNINGS=
  CXXSTATIC_BINDING="-static"
  CXXDYNAMIC_BINDING=
  CXXSTUFF=

  for flag in $CXXFLAGS; do
    case $flag in
      -O)  CXXOPT="$CXXOPT $flag";;
      -O[0123456789]*)  CXXOPT="$CXXOPT $flag";;
      -g*) CXXDEBUG="$CXXDEBUG $flag";;
      -W*) CXXWARNINGS="$CXXWARNINGS $flag";;
      *)   CXXSTUFF="$CXXSTUFF $flag";;
    esac
  done
fi
AC_MSG_CHECKING(for C++ compiler (${CXX}) warning options)
AC_MSG_RESULT(${CXXWARNINGS})
AC_MSG_CHECKING(for C++ compiler (${CXX}) optimizing options)
AC_MSG_RESULT(${CXXOPT})
AC_MSG_CHECKING(for C++ compiler (${CXX}) debugging options)
AC_MSG_RESULT(${CXXDEBUG})
AC_MSG_CHECKING(for C++ compiler (${CXX}) extra libraries)
AC_MSG_RESULT(${CXXLIBS})
AC_MSG_CHECKING(for C++ compiler (${CXX}) static binding options)
AC_MSG_RESULT(${CXXSTATIC_BINDING})
AC_MSG_CHECKING(for C++ compiler (${CXX}) dynamic binding options)
AC_MSG_RESULT(${CXXDYNAMIC_BINDING})
AC_SUBST(CXXWARNINGS)dnl
AC_SUBST(CXXDEBUG)dnl
AC_SUBST(CXXOPT)dnl
AC_SUBST(CXXLIBS)dnl
AC_SUBST(CXXSTATIC_BINDING)dnl
AC_SUBST(CXXDYNAMIC_BINDING)dnl
dnl
if test "$GXX" = yes; then
ICE_EXTERNAL_TEMPLATES
ICE_PERMISSIVE
dnl ICE_NO_IMPLICIT_TEMPLATES
ICE_ELIDE_CONSTRUCTORS
ICE_CONSERVE_SPACE
ICE_TRIGRAPHS
fi
CXXSTUFF="$CXXSTUFF $EXTERNAL_TEMPLATES $TRIGRAPHS"
AC_SUBST(CXXSTUFF)dnl
])dnl
dnl
dnl
dnl
dnl ICE_PROG_EMACS
dnl --------------
dnl
dnl Look for emacs; put its name in output variable `EMACS'.
dnl
dnl
AC_DEFUN(ICE_PROG_EMACS, 
[
AC_CHECK_PROGS(EMACS, emacs temacs xemacs lemacs)
])dnl
dnl
dnl
dnl ICE_PATH_INFO
dnl --------------
dnl
dnl Look for info path; put it in output variable `infodir'.
dnl
AC_DEFUN(ICE_PATH_INFO,
[
AC_MSG_CHECKING(for info directory)
AC_CACHE_VAL(ice_cv_infodir,
[
AC_REQUIRE([AC_PREFIX_PROGRAM])
AC_REQUIRE([ICE_PROG_EMACS])
ice_cv_infodir=$prefix/info
if test "$EMACS" != ""; then
cat > conftest.el << EOF

(defun print-list (l)
  (if (not (null l))
      (list
       (princ (car l) t)
       (princ "
" t)
       (print-list (cdr l)))))

(defun print-info-dirs ()
  (cond
   ((boundp 'Info-directory-list) 
    (print-list Info-directory-list))
   ((boundp 'Info-default-directory-list) 
    (print-list Info-default-directory-list))
   ((boundp 'Info-directory) 
    (print-list (list Info-directory)))
))
EOF
ice_info_dirs=`$EMACS -batch -l conftest.el -f print-info-dirs 2>&5`
rm -f conftest.el
for ice_info_dir in $ice_info_dirs; do
    ice_info_dir=`echo $ice_info_dir | sed 's,/*$,,'`
    if test -f "$ice_info_dir/dir"; then
	ice_cv_infodir="$ice_info_dir"
        break
    fi
done
fi
])
infodir=$ice_cv_infodir
AC_MSG_RESULT($infodir)
AC_SUBST(infodir)
])dnl
dnl
dnl
dnl ICE_TYPE_SIGNAL
dnl ---------------
dnl
dnl ICE_TYPE_SIGNAL: like AC_TYPE_SIGNAL, but use C++ for checks.
dnl
AC_DEFUN(ICE_TYPE_SIGNAL,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING([return type of signal handlers])
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_CACHE_VAL(ice_cv_type_signal,
[
AC_TRY_COMPILE(
[
#include <sys/types.h>
#include <signal.h>
void handler(int sg);],
[signal(1, handler);], ice_cv_type_signal=void, ice_cv_type_signal=int)])dnl
AC_LANG_RESTORE
AC_MSG_RESULT($ice_cv_type_signal)
AC_DEFINE_UNQUOTED(RETSIGTYPE, $ice_cv_type_signal)
])dnl
dnl
dnl
dnl
dnl ICE_TYPE_SIGNALPROC
dnl -------------------
dnl
dnl ICE_TYPE_SIGNALPROC: check params of signal handler.  Use as:
dnl
dnl #include "config.h"
dnl
dnl typedef void (*SignalProc)(SIGHANDLERARGS);
dnl
dnl
AC_DEFUN(ICE_TYPE_SIG_HANDLER_ARGS,
[
AC_REQUIRE([AC_TYPE_SIGNAL])
AC_MSG_CHECKING([parameter type of signal handlers])
AC_CACHE_VAL(ice_cv_type_sig_handler_args,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_cv_type_sig_handler_args=""
# Try "..."
if test "$ice_cv_type_sig_handler_args" = ""; then
AC_TRY_COMPILE(
[
#include <sys/types.h>
#include <signal.h>
RETSIGTYPE handler(...);],
[signal(1, handler);], 
ice_cv_type_sig_handler_args="...")
fi
# Try "int"
if test "$ice_cv_type_sig_handler_args" = ""; then
AC_TRY_COMPILE(
[
#include <sys/types.h>
#include <signal.h>
RETSIGTYPE handler(int);],
[signal(1, handler);], 
ice_cv_type_sig_handler_args="int")
fi
# Try "int, ..."
if test "$ice_cv_type_sig_handler_args" = ""; then
AC_TRY_COMPILE(
[
#include <sys/types.h>
#include <signal.h>
RETSIGTYPE handler(int ...);],
[signal(1, handler);], 
ice_cv_type_sig_handler_args="int ...")
fi
AC_LANG_RESTORE
]
)dnl
AC_MSG_RESULT($ice_cv_type_sig_handler_args)
if test "$ice_cv_type_sig_handler_args" = ""; then
AC_MSG_WARN([[Please #define SIGHANDLERARGS in config.h]])
fi
AC_DEFINE_UNQUOTED(SIGHANDLERARGS, $ice_cv_type_sig_handler_args)
])dnl
##!dnl
##!dnl
##!dnl
##!dnl ICE_CHECK_FROZEN_OSTRSTREAM
##!dnl ---------------------------
##!dnl
##!dnl If the C++ library has a ostrstream::frozen() function,
##!dnl define HAVE_FROZEN_OSTRSTREAM.
##!dnl
##!AC_DEFUN(ICE_CHECK_FROZEN_OSTRSTREAM,
##![
##!AC_REQUIRE([AC_PROG_CXX])
##!AC_MSG_CHECKING([for ostrstream::frozen()])
##!AC_CACHE_VAL(ice_cv_frozen_ostrstream,
##![
##!AC_LANG_SAVE
##!AC_LANG_CPLUSPLUS
##!AC_TRY_COMPILE([#include <strstream>],
##![std::ostrstream os; int frozen = os.frozen();],
##!ice_cv_frozen_ostrstream=yes, ice_cv_frozen_ostrstream=no)
##!AC_LANG_RESTORE
##!])
##!AC_MSG_RESULT($ice_cv_frozen_ostrstream)
##!if test "$ice_cv_frozen_ostrstream" = yes; then
##!AC_DEFINE(HAVE_FROZEN_OSTRSTREAM)
##!fi
##!])dnl
##!dnl
##!dnl ICE_CHECK_FREEZE_OSTRSTREAM
##!dnl ---------------------------
##!dnl
##!dnl If the C++ library has a ostrstream::freeze() function,
##!dnl define HAVE_FREEZE_OSTRSTREAM.
##!dnl
##!AC_DEFUN(ICE_CHECK_FREEZE_OSTRSTREAM,
##![
##!AC_REQUIRE([AC_PROG_CXX])
##!AC_MSG_CHECKING([for ostrstream::freeze()])
##!AC_CACHE_VAL(ice_cv_freeze_ostrstream,
##![
##!AC_LANG_SAVE
##!AC_LANG_CPLUSPLUS
##!AC_TRY_COMPILE([#include <strstream>],
##![std::ostrstream os; os.freeze(0);],
##!ice_cv_freeze_ostrstream=yes, ice_cv_freeze_ostrstream=no)
##!AC_LANG_RESTORE
##!])
##!AC_MSG_RESULT($ice_cv_freeze_ostrstream)
##!if test "$ice_cv_freeze_ostrstream" = yes; then
##!AC_DEFINE(HAVE_FREEZE_OSTRSTREAM)
##!fi
##!])dnl
##!dnl
##!dnl ICE_CHECK_FROZEN_OSTRSTREAMBUF
##!dnl ------------------------------
##!dnl
##!dnl If the C++ library has a ostrstreambuf::frozen() function,
##!dnl define HAVE_FROZEN_OSTRSTREAMBUF.
##!dnl
##!AC_DEFUN(ICE_CHECK_FROZEN_OSTRSTREAMBUF,
##![
##!AC_REQUIRE([AC_PROG_CXX])
##!AC_MSG_CHECKING([for ostrstreambuf::frozen()])
##!AC_CACHE_VAL(ice_cv_frozen_ostrstreambuf,
##![
##!AC_LANG_SAVE
##!AC_LANG_CPLUSPLUS
##!AC_TRY_COMPILE([#include <strstream>],
##![std::ostrstream os; int frozen = os.rdbuf()->frozen();],
##!ice_cv_frozen_ostrstreambuf=yes, ice_cv_frozen_ostrstreambuf=no)
##!AC_LANG_RESTORE
##!])
##!AC_MSG_RESULT($ice_cv_frozen_ostrstreambuf)
##!if test "$ice_cv_frozen_ostrstreambuf" = yes; then
##!AC_DEFINE(HAVE_FROZEN_OSTRSTREAMBUF)
##!fi
##!])dnl
##!dnl
##!dnl ICE_CHECK_FREEZE_OSTRSTREAMBUF
##!dnl ------------------------------
##!dnl
##!dnl If the C++ library has a ostrstreambuf::freeze() function,
##!dnl define HAVE_FREEZE_OSTRSTREAMBUF.
##!dnl
##!AC_DEFUN(ICE_CHECK_FREEZE_OSTRSTREAMBUF,
##![
##!AC_REQUIRE([AC_PROG_CXX])
##!AC_MSG_CHECKING([for ostrstreambuf::freeze()])
##!AC_CACHE_VAL(ice_cv_freeze_ostrstreambuf,
##![
##!AC_LANG_SAVE
##!AC_LANG_CPLUSPLUS
##!AC_TRY_COMPILE([#include <strstream>],
##![std::ostrstream os; os.rdbuf()->freeze(0);],
##!ice_cv_freeze_ostrstreambuf=yes, ice_cv_freeze_ostrstreambuf=no)
##!AC_LANG_RESTORE
##!])
##!AC_MSG_RESULT($ice_cv_freeze_ostrstreambuf)
##!if test "$ice_cv_freeze_ostrstreambuf" = yes; then
##!AC_DEFINE(HAVE_FREEZE_OSTRSTREAMBUF)
##!fi
##!])dnl
dnl
dnl
dnl ICE_TYPE_REGEX_T
dnl ----------------
dnl
dnl ICE_TYPE_REGEX_T: find members of POSIX.2 `regex_t' type
dnl - HAVE_REGEX_T_RE_NSUB:   `regex_t' has a `re_nsub' member
dnl - HAVE_REGEX_T_N_SUBEXPS: `regex_t' has a `n_subexps' member
dnl
AC_DEFUN(ICE_TYPE_REGEX_T,
[
AC_REQUIRE([AC_PROG_CXX])
AC_CHECK_HEADERS(regex.h rx.h rxposix.h)
ICE_CHECK_DECL(regcomp, regex.h rx.h rxposix.h)
ICE_CHECK_DECL(regexec, regex.h rx.h rxposix.h)
ice_save_cppflags="$CPPFLAGS"
CPPFLAGS="-I$srcdir/.. $CPPFLAGS"
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_MSG_CHECKING([re_nsub member of POSIX.2 regex_t type])
AC_CACHE_VAL(ice_cv_have_regex_t_re_nsub,
[
AC_TRY_COMPILE(
[
extern "C" {
#include <sys/types.h>

// Avoid conflicts with C regex() function
#define regex c_regex

// Don't include old libg++ <regex.h> contents
#define __REGEXP_LIBRARY

#ifndef __STDC__
#define __STDC__ 1              // Reguired for KCC when using GNU includes
#endif

// Some old versions of libg++ contain a <regex.h> file.  Avoid this.
#if !defined(REG_EXTENDED) && HAVE_REGEX_H
#include <regex.h>		// POSIX.2 interface
#endif

// Try hard-wired path to get native <regex.h>.
#if !defined(REG_EXTENDED) && HAVE_REGEX_H
#include </usr/include/regex.h>	// POSIX.2 interface
#endif

// Some more GNU headers.
#if !defined(REG_EXTENDED) && HAVE_RX_H
#include <rx.h>	 	        // Header from GNU rx 0.07
#endif

#if !defined(REG_EXTENDED) && HAVE_RXPOSIX_H
#include <rxposix.h>		// Header from GNU rx 1.0 and later
#endif
}
],
[regex_t rx; int a = rx.re_nsub;], 
ice_cv_have_regex_t_re_nsub=yes, ice_cv_have_regex_t_re_nsub=no)])dnl
AC_MSG_RESULT($ice_cv_have_regex_t_re_nsub)
if test "$ice_cv_have_regex_t_re_nsub" = yes; then
AC_DEFINE(HAVE_REGEX_T_RE_NSUB)
fi
dnl
dnl
AC_MSG_CHECKING([n_subexps member of GNU RX 1.0 regex_t type])
AC_CACHE_VAL(ice_cv_have_regex_t_n_subexps,
[
AC_TRY_COMPILE(
[
extern "C" {
#include <sys/types.h>

// Avoid conflicts with C regex() function
#define regex c_regex

// Don't include old libg++ <regex.h> contents
#define __REGEXP_LIBRARY

#ifndef __STDC__
#define __STDC__ 1              // Reguired for KCC when using GNU includes
#endif

// Some old versions of libg++ contain a <regex.h> file.  Avoid this.
#if !defined(REG_EXTENDED) && HAVE_REGEX_H
#include <regex.h>		// POSIX.2 interface
#endif

// Try hard-wired path to get native <regex.h>.
#if !defined(REG_EXTENDED) && HAVE_REGEX_H
#include </usr/include/regex.h>	// POSIX.2 interface
#endif

// Some more GNU headers.
#if !defined(REG_EXTENDED) && HAVE_RX_H
#include <rx.h>	 	        // Header from GNU rx 0.07
#endif

#if !defined(REG_EXTENDED) && HAVE_RXPOSIX_H
#include <rxposix.h>		// Header from GNU rx 1.0 and later
#endif
}
],
[regex_t rx; int a = rx.n_subexps;],
ice_cv_have_regex_t_n_subexps=yes, ice_cv_have_regex_t_n_subexps=no)])dnl
AC_MSG_RESULT($ice_cv_have_regex_t_n_subexps)
if test "$ice_cv_have_regex_t_n_subexps" = yes; then
AC_DEFINE(HAVE_REGEX_T_N_SUBEXPS)
fi
dnl
dnl
AC_LANG_RESTORE
CPPFLAGS="$ice_save_cppflags"
])dnl
dnl
dnl
dnl
dnl ICE_REGCOMP_BROKEN
dnl ------------------
dnl
dnl #define REGCOMP_BROKEN if regcomp() always returns -1.
dnl This happens on Solaris 2.4.
dnl
AC_DEFUN(ICE_REGCOMP_BROKEN,
[
AC_REQUIRE([ICE_TYPE_REGEX_T])
if  test "$ice_have_regcomp" = yes && test "$ice_have_regexec" = yes; then
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING([whether regcomp() is broken])
ice_save_cppflags="$CPPFLAGS"
CPPFLAGS="-I$srcdir/.. $CPPFLAGS"
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_CACHE_VAL(ice_cv_regcomp_broken,
[
AC_TRY_RUN(
[
extern "C" {
#include <sys/types.h>

// Avoid conflicts with C regex() function
#define regex c_regex

// Don't include old libg++ <regex.h> contents
#define __REGEXP_LIBRARY

#ifndef __STDC__
#define __STDC__ 1              // Reguired for KCC when using GNU includes
#endif

// Some old versions of libg++ contain a <regex.h> file.  Avoid this.
#if !defined(REG_EXTENDED) && HAVE_REGEX_H
#include <regex.h>		// POSIX.2 interface
#endif

// Try hard-wired path to get native <regex.h>.
#if !defined(REG_EXTENDED) && HAVE_REGEX_H
#include </usr/include/regex.h>	// POSIX.2 interface
#endif

// Some more GNU headers.
#if !defined(REG_EXTENDED) && HAVE_RX_H
#include <rx.h>	 	        // Header from GNU rx 0.07
#endif

#if !defined(REG_EXTENDED) && HAVE_RXPOSIX_H
#include <rxposix.h>		// Header from GNU rx 1.0 and later
#endif
}

int main()
{
    char rxdouble[] = 
	"-?(([0-9]+\\\\.[0-9]*)|([0-9]+)|(\\\\.[0-9]+))"
	"([eE][---+]?[0-9]+)?";
    regex_t compiled;
    int errcode = regcomp(&compiled, rxdouble, REG_EXTENDED);
    if (errcode)
	return 1;
    
    char somedouble[] = "3.141529e+0";
    return regexec(&compiled, somedouble, 0, 0, 0);
}
], ice_cv_regcomp_broken=no, ice_cv_regcomp_broken=yes,
ice_cv_regcomp_broken=yes)])dnl
AC_MSG_RESULT($ice_cv_regcomp_broken)
if test "$ice_cv_regcomp_broken" = yes; then
AC_DEFINE(REGCOMP_BROKEN)
fi
dnl
dnl
AC_LANG_RESTORE
CPPFLAGS="$ice_save_cppflags"
]
fi)dnl
dnl
dnl
dnl ICE_DELETE_CR
dnl -------------
dnl
dnl If running on CYGWIN, set output variable DELETE_CR to 
dnl `tr -d '\r' | gzip | gunzip' (relying on gunzip to set its 
dnl output to binary mode) and to `cat' otherwise.
dnl 
dnl
AC_DEFUN(ICE_DELETE_CR,
[
AC_REQUIRE([AC_CYGWIN])
if test "$CYGWIN" = yes; then
    DELETE_CR="tr -d '\r' | gzip | gunzip"
else
    DELETE_CR="cat"
fi
AC_SUBST(DELETE_CR)
])dnl
dnl
dnl
dnl
dnl
dnl ICE_FIND_MOTIF
dnl --------------
dnl
dnl Find Motif libraries and headers
dnl Put Motif include directory in motif_includes,
dnl put Motif library directory in motif_libraries,
dnl and add appropriate flags to X_CFLAGS and X_LIBS.
dnl
dnl
AC_DEFUN(ICE_FIND_MOTIF,
[
AC_REQUIRE([AC_PATH_XTRA])
AC_REQUIRE([ICE_CXX_ISYSTEM])
motif_includes=
motif_libraries=
AC_ARG_WITH(motif,
[  --without-motif         do not use Motif widgets])
dnl Treat --without-motif like
dnl --without-motif-includes --without-motif-libraries.
if test "$with_motif" = "no"
then
motif_includes=no
motif_libraries=no
fi
AC_ARG_WITH(motif-includes,
[  --with-motif-includes=DIR
                          Motif include files are in DIR],
motif_includes="$withval")
AC_ARG_WITH(motif-libraries,
[  --with-motif-libraries=DIR
                          Motif libraries are in DIR],
motif_libraries="$withval")
AC_MSG_CHECKING(for Motif)
#
#
# Search the include files.
#
if test "$motif_includes" = ""; then
AC_CACHE_VAL(ice_cv_motif_includes,
[
ice_motif_save_LIBS="$LIBS"
ice_motif_save_CFLAGS="$CFLAGS"
ice_motif_save_CPPFLAGS="$CPPFLAGS"
ice_motif_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_COMPILE([#include <Xm/Xm.h>],[int a;],
[
# Xm/Xm.h is in the standard search path.
ice_cv_motif_includes=
],
[
# Xm/Xm.h is not in the standard search path.
# Locate it and put its directory in `motif_includes'
#
# /usr/include/Motif* are used on HP-UX (Motif).
# /usr/include/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
           /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
           /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
           /usr/dt/include /usr/openwin/include \
           /usr/dt/*/include /opt/*/include /usr/include/Motif* \
           /usr/*/include/X11R6 /usr/*/include/X11R5 /usr/*/include/X11R4 \
	   "${prefix}"/*/include /usr/*/include /usr/local/*/include \
	   "${prefix}"/include/* /usr/include/* /usr/local/include/*; do
if test -f "$dir/Xm/Xm.h"; then
ice_cv_motif_includes="$dir"
break
fi
done
if test "$ice_cv_motif_includes" = "/usr/include"; then
ice_cv_motif_includes=
fi
])
#
LIBS="$ice_motif_save_LIBS"
CFLAGS="$ice_motif_save_CFLAGS"
CPPFLAGS="$ice_motif_save_CPPFLAGS"
LDFLAGS="$ice_motif_save_LDFLAGS"
])
motif_includes="$ice_cv_motif_includes"
fi
#
#
# Now for the libraries.
#
if test "$motif_libraries" = ""; then
AC_CACHE_VAL(ice_cv_motif_libraries,
[
ice_motif_save_LIBS="$LIBS"
ice_motif_save_CFLAGS="$CFLAGS"
ice_motif_save_CPPFLAGS="$CPPFLAGS"
ice_motif_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
# We use XtToolkitInitialize() here since it takes no arguments
# and thus also works with a C++ compiler.
AC_TRY_LINK([
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
],[XtToolkitInitialize();],
[
# libXm.a is in the standard search path.
ice_cv_motif_libraries=
],
[
# libXm.a is not in the standard search path.
# Locate it and put its directory in `motif_libraries'
#
# /usr/lib/Motif* are used on HP-UX (Motif).
# /usr/lib/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/lesstif is used on Linux (Lesstif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
	   /usr/lib/Motif2.0 /usr/lib/Motif1.2 /usr/lib/Motif1.1 \
	   /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
           /usr/dt/lib /usr/openwin/lib \
	   /usr/dt/*/lib /opt/*/lib /usr/lib/Motif* \
           /usr/lesstif*/lib /usr/lib/Lesstif* \
	   /usr/*/lib/X11R6 /usr/*/lib/X11R5 /usr/*/lib/X11R4 /usr/*/lib/X11 \
	   "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
	   "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
if test -d "$dir" && test "`ls $dir/libXm.* 2> /dev/null`" != ""; then
ice_cv_motif_libraries="$dir"
break
fi
done
if test "$ice_cv_motif_libraries" = ""; then
ice_cv_motif_libraries=no
fi
])
#
LIBS="$ice_motif_save_LIBS"
CFLAGS="$ice_motif_save_CFLAGS"
CPPFLAGS="$ice_motif_save_CPPFLAGS"
LDFLAGS="$ice_motif_save_LDFLAGS"
])
#
motif_libraries="$ice_cv_motif_libraries"
fi
# Add Motif definitions to X flags
#
if test "$motif_includes" != "" && test "$motif_includes" != "$x_includes" && test "$motif_includes" != "no"
then
X_CFLAGS="$ISYSTEM$motif_includes $X_CFLAGS"
fi
if test "$motif_libraries" != "" && test "$motif_libraries" != "$x_libraries" && test "$motif_libraries" != "no"
then
case "$X_LIBS" in
  *-R\ *) X_LIBS="-L$motif_libraries -R $motif_libraries $X_LIBS";;
  *-R*)   X_LIBS="-L$motif_libraries -R$motif_libraries $X_LIBS";;
  *)      X_LIBS="-L$motif_libraries $X_LIBS";;
esac
fi
#
#
motif_libraries_result="$motif_libraries"
motif_includes_result="$motif_includes"
test "$motif_libraries_result" = "" && 
  motif_libraries_result="in default path"
test "$motif_includes_result" = "" && 
  motif_includes_result="in default path"
test "$motif_libraries_result" = "no" && 
  motif_libraries_result="(none)"
test "$motif_includes_result" = "no" && 
  motif_includes_result="(none)"
AC_MSG_RESULT(
  [libraries $motif_libraries_result, headers $motif_includes_result])
])dnl
dnl
dnl
dnl ICE_FIND_ATHENA
dnl ---------------
dnl
dnl Find Athena libraries and headers.
dnl Put Athena include directory in athena_includes,
dnl put Athena library directory in athena_libraries,
dnl and add appropriate flags to X_CFLAGS and X_LIBS.
dnl
dnl
AC_DEFUN(ICE_FIND_ATHENA,
[
AC_REQUIRE([AC_PATH_XTRA])
AC_REQUIRE([ICE_CXX_ISYSTEM])
athena_includes=
athena_libraries=
AC_ARG_WITH(athena,
[  --without-athena        do not use Athena widgets])
dnl Treat --without-athena like
dnl --without-athena-includes --without-athena-libraries.
if test "$with_athena" = "no"
then
athena_includes=no
athena_libraries=no
fi
AC_ARG_WITH(athena-includes,
[  --with-athena-includes=DIR
                          Athena include files are in DIR],
athena_includes="$withval")
AC_ARG_WITH(athena-libraries,
[  --with-athena-libraries=DIR
                          Athena libraries are in DIR],
athena_libraries="$withval")
AC_MSG_CHECKING(for Athena)
#
#
# Search the include files.
#
if test "$athena_includes" = ""; then
AC_CACHE_VAL(ice_cv_athena_includes,
[
ice_athena_save_LIBS="$LIBS"
ice_athena_save_CFLAGS="$CFLAGS"
ice_athena_save_CPPFLAGS="$CPPFLAGS"
ice_athena_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXaw -lXmu -lXext -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_COMPILE([
#include <X11/Intrinsic.h>
#include <X11/Xaw/Text.h>
],[int a;],
[
# X11/Xaw/Text.h is in the standard search path.
ice_cv_athena_includes=
],
[
# X11/Xaw/Text.h is not in the standard search path.
# Locate it and put its directory in `athena_includes'
#
# /usr/include/Motif* are used on HP-UX (Motif).
# /usr/include/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
           /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
           /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
           /usr/dt/include /usr/openwin/include \
           /usr/dt/*/include /opt/*/include /usr/include/Motif* \
           /usr/*/include/X11R6 /usr/*/include/X11R5 /usr/*/include/X11R4 \
	   "${prefix}"/*/include /usr/*/include /usr/local/*/include \
	   "${prefix}"/include/* /usr/include/* /usr/local/include/*; do
if test -f "$dir/X11/Xaw/Text.h"; then
ice_cv_athena_includes="$dir"
break
fi
done
if test "$ice_cv_athena_includes" = "/usr/include"; then
ice_cv_athena_includes=
fi
])
#
LIBS="$ice_athena_save_LIBS"
CFLAGS="$ice_athena_save_CFLAGS"
CPPFLAGS="$ice_athena_save_CPPFLAGS"
LDFLAGS="$ice_athena_save_LDFLAGS"
])
athena_includes="$ice_cv_athena_includes"
fi
#
#
# Now for the libraries.
#
if test "$athena_libraries" = ""; then
AC_CACHE_VAL(ice_cv_athena_libraries,
[
ice_athena_save_LIBS="$LIBS"
ice_athena_save_CFLAGS="$CFLAGS"
ice_athena_save_CPPFLAGS="$CPPFLAGS"
ice_athena_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXaw -lXmu -lXext -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
# We use XtToolkitInitialize() here since it takes no arguments
# and thus also works with a C++ compiler.
AC_TRY_LINK([
#include <X11/Intrinsic.h>
#include <X11/Xaw/Text.h>
],[XtToolkitInitialize();],
[
# libXaw.a is in the standard search path.
ice_cv_athena_libraries=
],
[
# libXaw.a is not in the standard search path.
# Locate it and put its directory in `athena_libraries'
#
#
# /usr/lib/Motif* are used on HP-UX (Motif).
# /usr/lib/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
	   /usr/lib/Motif2.0 /usr/lib/Motif1.2 /usr/lib/Motif1.1 \
	   /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
           /usr/dt/lib /usr/openwin/lib \
	   /usr/dt/*/lib /opt/*/lib /usr/lib/Motif* \
	   /usr/*/lib/X11R6 /usr/*/lib/X11R5 /usr/*/lib/X11R4 /usr/*/lib/X11 \
	   "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
	   "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
if test -d "$dir" && test "`ls $dir/libXaw.* 2> /dev/null`" != ""; then
ice_cv_athena_libraries="$dir"
break
fi
done
])
#
LIBS="$ice_athena_save_LIBS"
CFLAGS="$ice_athena_save_CFLAGS"
CPPFLAGS="$ice_athena_save_CPPFLAGS"
LDFLAGS="$ice_athena_save_LDFLAGS"
])
#
athena_libraries="$ice_cv_athena_libraries"
fi
# Add Athena definitions to X flags
#
if test "$athena_includes" != "" && test "$athena_includes" != "$x_includes" && test "$athena_includes" != "no"
then
X_CFLAGS="$ISYSTEM$athena_includes $X_CFLAGS"
fi
if test "$athena_libraries" != "" && test "$athena_libraries" != "$x_libraries" && test "$athena_libraries" != "no"
then
case "$X_LIBS" in
  *-R\ *) X_LIBS="-L$athena_libraries -R $athena_libraries $X_LIBS";;
  *-R*)   X_LIBS="-L$athena_libraries -R$athena_libraries $X_LIBS";;
  *)      X_LIBS="-L$athena_libraries $X_LIBS";;
esac
fi
#
#
athena_libraries_result="$athena_libraries"
athena_includes_result="$athena_includes"
test "$athena_libraries_result" = "" && 
  athena_libraries_result="in default path"
test "$athena_includes_result" = "" && 
  athena_includes_result="in default path"
test "$athena_libraries_result" = "no" && 
  athena_libraries_result="(none)"
test "$athena_includes_result" = "no" && 
  athena_includes_result="(none)"
AC_MSG_RESULT(
  [libraries $athena_libraries_result, headers $athena_includes_result])
])dnl
dnl
dnl
dnl ICE_FIND_XPM
dnl ---------------
dnl
dnl Find Xpm libraries and headers.
dnl Put Xpm include directory in xpm_includes,
dnl put Xpm library directory in xpm_libraries,
dnl and add appropriate flags to X_CFLAGS and X_LIBS.
dnl
dnl
AC_DEFUN(ICE_FIND_XPM,
[
AC_REQUIRE([AC_PATH_XTRA])
AC_REQUIRE([ICE_CXX_ISYSTEM])
xpm_includes=
xpm_libraries=
AC_ARG_WITH(xpm,
[  --without-xpm           do not use the Xpm library])
dnl Treat --without-xpm like
dnl --without-xpm-includes --without-xpm-libraries.
if test "$with_xpm" = "no"
then
xpm_includes=no
xpm_libraries=no
fi
AC_ARG_WITH(xpm-includes,
[  --with-xpm-includes=DIR Xpm include files are in DIR],
xpm_includes="$withval")
AC_ARG_WITH(xpm-libraries,
[  --with-xpm-libraries=DIR
                          Xpm libraries are in DIR],
xpm_libraries="$withval")
AC_MSG_CHECKING(for Xpm)
#
#
# Search the include files.  Note that XPM can come in <X11/xpm.h> (as
# in X11R6) or in <xpm.h> if installed locally.
#
if test "$xpm_includes" = ""; then
AC_CACHE_VAL(ice_cv_xpm_includes,
[
ice_xpm_save_LIBS="$LIBS"
ice_xpm_save_CFLAGS="$CFLAGS"
ice_xpm_save_CPPFLAGS="$CPPFLAGS"
ice_xpm_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXpm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_COMPILE([
#include <X11/xpm.h>
],[int a;],
[
# X11/xpm.h is in the standard search path.
ice_cv_xpm_includes=
],
[
# X11/xpm.h is not in the standard search path.
# Locate it and put its directory in `xpm_includes'
#
# /usr/include/Motif* are used on HP-UX (Motif).
# /usr/include/X11* are used on HP-UX (X and Xaw).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Xaw).
# Other directories are just guesses.
for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
           /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
           /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
           /usr/dt/include /usr/openwin/include \
           /usr/dt/*/include /opt/*/include /usr/include/Motif* \
           /usr/*/include/X11R6 /usr/*/include/X11R5 /usr/*/include/X11R4 \
	   "${prefix}"/*/include /usr/*/include /usr/local/*/include \
	   "${prefix}"/include/* /usr/include/* /usr/local/include/*; do
if test -f "$dir/X11/xpm.h" || test -f "$dir/xpm.h"; then
ice_cv_xpm_includes="$dir"
break
fi
done
if test "$ice_cv_xpm_includes" = "/usr/include"; then
ice_cv_xpm_includes=
fi
])
#
LIBS="$ice_xpm_save_LIBS"
CFLAGS="$ice_xpm_save_CFLAGS"
CPPFLAGS="$ice_xpm_save_CPPFLAGS"
LDFLAGS="$ice_xpm_save_LDFLAGS"
])
xpm_includes="$ice_cv_xpm_includes"
fi
#
#
# Now for the libraries.
#
if test "$xpm_libraries" = ""; then
AC_CACHE_VAL(ice_cv_xpm_libraries,
[
ice_xpm_save_LIBS="$LIBS"
ice_xpm_save_CFLAGS="$CFLAGS"
ice_xpm_save_CPPFLAGS="$CPPFLAGS"
ice_xpm_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXpm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
#
# We use XtToolkitInitialize() here since it takes no arguments
# and thus also works with a C++ compiler.
AC_TRY_LINK([
#include <X11/Intrinsic.h>
#include <X11/xpm.h>
],[XtToolkitInitialize();],
[
# libxpm.a is in the standard search path.
ice_cv_xpm_libraries=
],
[
# libXpm.a is not in the standard search path.
# Locate it and put its directory in `xpm_libraries'
#
#
# /usr/lib/Motif* are used on HP-UX (Motif).
# /usr/lib/X11* are used on HP-UX (X and Xpm).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Xpm).
# Other directories are just guesses.
for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
	   /usr/lib/Motif2.0 /usr/lib/Motif1.2 /usr/lib/Motif1.1 \
	   /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
           /usr/dt/lib /usr/openwin/lib \
	   /usr/dt/*/lib /opt/*/lib /usr/lib/Motif* \
	   /usr/*/lib/X11R6 /usr/*/lib/X11R5 /usr/*/lib/X11R4 /usr/*/lib/X11 \
	   "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
	   "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
if test -d "$dir" && test "`ls $dir/libXpm.* 2> /dev/null`" != ""; then
ice_cv_xpm_libraries="$dir"
break
fi
done
])
#
LIBS="$ice_xpm_save_LIBS"
CFLAGS="$ice_xpm_save_CFLAGS"
CPPFLAGS="$ice_xpm_save_CPPFLAGS"
LDFLAGS="$ice_xpm_save_LDFLAGS"
])
#
xpm_libraries="$ice_cv_xpm_libraries"
fi
#
# Add Xpm definitions to X flags
#
if test "$xpm_includes" != "" && test "$xpm_includes" != "$x_includes" && test "$xpm_includes" != "no"
then
X_CFLAGS="$ISYSTEM$xpm_includes $X_CFLAGS"
fi
if test "$xpm_libraries" != "" && test "$xpm_libraries" != "$x_libraries" && test "$xpm_libraries" != "no"
then
case "$X_LIBS" in
  *-R\ *) X_LIBS="-L$xpm_libraries -R $xpm_libraries $X_LIBS";;
  *-R*)   X_LIBS="-L$xpm_libraries -R$xpm_libraries $X_LIBS";;
  *)      X_LIBS="-L$xpm_libraries $X_LIBS";;
esac
fi
#
#
xpm_libraries_result="$xpm_libraries"
xpm_includes_result="$xpm_includes"
test "$xpm_libraries_result" = "" && 
  xpm_libraries_result="in default path"
test "$xpm_includes_result" = "" && 
  xpm_includes_result="in default path"
test "$xpm_libraries_result" = "no" && 
  xpm_libraries_result="(none)"
test "$xpm_includes_result" = "no" && 
  xpm_includes_result="(none)"
AC_MSG_RESULT(
  [libraries $xpm_libraries_result, headers $xpm_includes_result])
])dnl
dnl
dnl
dnl
dnl ICE_TRANSLATION_RESOURCE
dnl ------------------------
dnl 
dnl If Xt supports base translations, set @TRANSLATIONS@ to `baseTranslations',
dnl otherwise, to `translations'.
dnl
AC_DEFUN(ICE_TRANSLATION_RESOURCE,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(for the name of the translation resource)
AC_CACHE_VAL(ice_cv_translations,
[
AC_TRY_COMPILE([#include <X11/Intrinsic.h>],[
#if XtSpecificationRelease < 5
#error baseTranslation resource only in X11R5 and later
#endif
],
ice_cv_translations=baseTranslations, 
ice_cv_translations=translations)
])
AC_MSG_RESULT($ice_cv_translations)
TRANSLATIONS=$ice_cv_translations
AC_SUBST(TRANSLATIONS)
])dnl
dnl
dnl
dnl ICE_SETUP_RPATH
dnl ---------------
dnl
dnl For each `-L PATH' option in LDFLAGS, add a `-Wl,-rpath,PATH' option
dnl if the linker supports it.  This is important on Linux, because
dnl the path set by `-L PATH' is ignored at run-time.
dnl
AC_DEFUN(ICE_SETUP_RPATH,
[
AC_REQUIRE([ICE_RPATH])
if test "$ice_rpath" = yes; then
changequote(,)dnl
LDFLAGS=`echo $LDFLAGS | sed 's/-L *\([^ ][^ ]*\)/& -Wl,-rpath,\1/g'`
 X_LIBS=`echo $X_LIBS  | sed 's/-L *\([^ ][^ ]*\)/& -Wl,-rpath,\1/g'`
changequote([,])dnl
fi
])dnl
dnl
dnl
dnl ICE_MOTIF_DIALOGS_OWN_SELECTION_LIST
dnl ----------------------------------
dnl
AC_DEFUN(ICE_MOTIF_DIALOGS_OWN_SELECTION_LIST,
[
AC_MSG_CHECKING(whether Motif dialogs own the string table)
AC_CACHE_VAL(ice_cv_motif_dialogs_own_selection_list,
[
AC_REQUIRE([ICE_FIND_MOTIF])
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_RUN(
[
#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef XmFONTLIST_DEFAULT_TAG
#define DEFAULT_CHARSET XmFONTLIST_DEFAULT_TAG
#else
#define DEFAULT_CHARSET XmSTRING_DEFAULT_CHARSET
#endif

extern "C" void free(void *p)
{
    static void *freed[100000];
    static int freed_count = 0;

    for (int i = 0; i < freed_count; i++)
	if (freed[i] == p)
	{
	    fprintf(stderr, "%p already freed\n", p);
	    exit(1);
	}

    freed[freed_count++] = p;
}

int main(int argc, char *argv[])
{
    XtAppContext app_context;
    Widget toplevel = 
	XtAppInitialize(&app_context, "Config",
			XrmOptionDescList(0), 0, 
			&argc, argv, (String *)0, ArgList(0), 0);

    Widget dialog = 
	XmCreateSelectionDialog(toplevel, "dialog", ArgList(0), 0);

    int length = 10;

    XmStringTable xmlist = 
	XmStringTable(XtMalloc(length * sizeof(XmString)));
    int i;
    for (i = 0; i < length; i++)
	xmlist[i] = XmStringCreateLtoR("abc", DEFAULT_CHARSET);

    XtVaSetValues (dialog,
		   XmNlistItems,     xmlist,
		   XmNlistItemCount, length,
		   XtPointer(0));

    for (i = 0; i < length; i++)
	XmStringFree(xmlist[i]);

    XtFree((char *)xmlist);

    XtDestroyWidget(toplevel);

    exit(0);
}
], 
ice_cv_motif_dialogs_own_selection_list=no, 
ice_cv_motif_dialogs_own_selection_list=yes,
ice_cv_motif_dialogs_own_selection_list=yes)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_motif_dialogs_own_selection_list)
if test "$ice_cv_motif_dialogs_own_selection_list" = yes; then
AC_DEFINE(MOTIF_DIALOGS_OWN_SELECTION_LIST)
fi
])dnl
dnl
dnl DDD_VERSION
dnl -----------
dnl
dnl Define DDD version.
dnl *This* is the place where name and version numbers are defined.
AC_DEFUN(DDD_VERSION,
[AC_MSG_CHECKING(for product name)
dnl
dnl The product name, in various capitalizations.
ddd=ddd
Ddd=Ddd
DDD=DDD
PACKAGE=${ddd}
dnl
dnl The product version, in the form `MAJOR-NUMBER.MINOR-NUMBER[.PATCHLEVEL]'
dnl A version in the form `YYYY-MM-DD' is a release-of-the-day, i.e. a
dnl snapshot of the current development tree.
VERSION=3.3.8
dnl
dnl The version nickname.
NICKNAME="Cinnamon"
dnl 
dnl The product expiration date, in ISO 8601 YYYY-MM-DD format.
dnl After this date, users are requested to update DDD.
dnl An empty expiration date means no expiration.
EXPIRES=
dnl
dnl
AC_SUBST(PACKAGE)dnl
AC_SUBST(ddd)dnl
AC_SUBST(Ddd)dnl
AC_SUBST(DDD)dnl
AC_SUBST(VERSION)dnl
AC_SUBST(NICKNAME)dnl
AC_MSG_RESULT(${DDD} ${VERSION} \"${NICKNAME}\")
dnl
dnl
AC_MSG_CHECKING(for expiration date)
dnl
AC_SUBST(EXPIRES)dnl
if test "$EXPIRES" = ""; then
AC_MSG_RESULT(no)
else
AC_MSG_RESULT(${EXPIRES})
fi
])
