dnl $Id$
dnl Local autoconf macros
dnl 
dnl Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
dnl Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
dnl 
dnl This file is part of the ICE Library.
dnl 
dnl The ICE Library is free software; you can redistribute it and/or
dnl modify it under the terms of the GNU Library General Public
dnl License as published by the Free Software Foundation; either
dnl version 2 of the License, or (at your option) any later version.
dnl 
dnl The ICE Library is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
dnl See the GNU Library General Public License for more details.
dnl 
dnl You should have received a copy of the GNU Library General Public
dnl License along with the ICE Library -- see the file COPYING.LIB.
dnl If not, write to the Free Software Foundation, Inc.,
dnl 675 Mass Ave, Cambridge, MA 02139, USA.
dnl 
dnl ICE is the incremental configuration engine.
dnl Contact ice@ips.cs.tu-bs.de for details.
dnl
dnl
dnl
dnl
dnl If the C++ compiler accepts the `-fexternal-templates' flag,
dnl set output variable `EXTERNAL_TEMPLATES' to `-fexternal-templates',
dnl empty otherwise.
dnl
AC_DEFUN(ICE_EXTERNAL_TEMPLATES,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether ${CXX} accepts the -fexternal-templates flag)
AC_CACHE_VAL(ice_cv_external_templates,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_save_cxxflags="$CXXFLAGS"
CXXFLAGS=-fexternal-templates
AC_TRY_COMPILE(,[int a;],
ice_cv_external_templates=yes, ice_cv_external_templates=no)
CXXFLAGS="$ice_save_cxxflags"
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_external_templates)
if test "$ice_cv_external_templates" = yes; then
EXTERNAL_TEMPLATES=-fexternal-templates
fi
AC_SUBST(EXTERNAL_TEMPLATES)
])dnl
dnl
dnl
dnl If the C++ compiler accepts the `-felide-constructors' flag,
dnl set output variable `ELIDE_CONSTRUCTORS' to `-felide-constructors',
dnl empty otherwise.
dnl
AC_DEFUN(ICE_ELIDE_CONSTRUCTORS,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether ${CXX} accepts the -felide-constructors flag)
AC_CACHE_VAL(ice_cv_elide_constructors,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
ice_save_cxxflags="$CXXFLAGS"
CXXFLAGS=-felide-constructors
AC_TRY_COMPILE(,[int a;],
ice_cv_elide_constructors=yes, ice_cv_elide_constructors=no)
CXXFLAGS="$ice_save_cxxflags"
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_elide_constructors)
if test "$ice_cv_elide_constructors" = yes; then
ELIDE_CONSTRUCTORS=-felide-constructors
fi
AC_SUBST(ELIDE_CONSTRUCTORS)
])dnl
dnl
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
AC_MSG_WARN(*** ${ICE} works best with ${CXX} version 2.5 or higher ***)
fi
])dnl
dnl
dnl
dnl If the C++ compiler accepts the `bool' keyword, define `HAVE_BOOL'.
dnl
AC_DEFUN(ICE_CXX_BOOL,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether ${CXX} supports bool types)
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
dnl If the C++ compiler supports named return values, 
dnl define `HAVE_NAMED_RETURN_VALUES'.
dnl
AC_DEFUN(ICE_CXX_NAMED_RETURN_VALUES,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether ${CXX} supports named return values)
AC_CACHE_VAL(ice_cv_have_named_return_values,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([
struct X {
    int f();
};

int X::f() return i;
{
    i = 42;
}
], [/* empty */],
ice_cv_have_named_return_values=yes,
ice_cv_have_named_return_values=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_have_named_return_values)
if test "$ice_cv_have_named_return_values" = yes; then
AC_DEFINE(HAVE_NAMED_RETURN_VALUES)
fi
])dnl
dnl
dnl
dnl If the C++ compiler supports the __PRETTY_FUNCTION__ macro,
dnl define `HAVE_PRETTY_FUNCTION'.
dnl
AC_DEFUN(ICE_CXX_PRETTY_FUNCTION,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether ${CXX} supports the __PRETTY_FUNCTION__ macro)
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
dnl If the C++ compiler supports overloading operator new[],
dnl define `HAVE_ARRAY_OPERATOR_NEW'.
dnl
AC_DEFUN(ICE_CXX_ARRAY_OPERATOR_NEW,
[
AC_REQUIRE([AC_PROG_CXX])
changequote(,)dnl
AC_MSG_CHECKING(whether ${CXX} supports overloaded operator new[])
changequote([,])dnl
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
dnl
dnl If the C++ compiler realizes ANSI C++ working paper conformant
dnl lifetime of temporaries, define `HAVE_ANSI_LIFETIME_OF_TEMPORARIES'.
dnl
AC_DEFUN(ICE_CXX_LIFETIME_OF_TEMPORARIES,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING(whether ${CXX} supports ANSI C++ lifetime of temporaries)
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
dnl
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
AC_CHECK_FUNCS($1, ice_have_$1=yes)
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
dnl
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
#include <iostream.h>
EOF
changequote(,)dnl
$CXXCPP $DEFS conftest.cc > conftest.ii 2>&5
if test $? != 0; then
AC_MSG_ERROR(${CXXCPP} could not find iostream.h)
else
ice_file=`grep '^# 1' conftest.ii | grep iostream.h | \
	head -1 | sed 's/^.*"\(.*\)".*$/\1/'`
ice_cv_cxx_include_dir=`echo $ice_file | sed 's%/[^/][^/]*$%%'`
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
changequote([,])dnl
rm -f conftest.cc conftest.ii
])
CXX_INCLUDE_DIR=$ice_cv_cxx_include_dir
AC_MSG_RESULT(${CXX_INCLUDE_DIR})
AC_SUBST(CXX_INCLUDE_DIR)
])dnl
dnl
dnl
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
dnl Setup C++ compile options.  Specific to ICE.
dnl
AC_DEFUN(ICE_CXX_OPTIONS,
[
CXXLIBS=
if test "$GXX" = yes; then
  CXXOPT="-DNDEBUG"
  CXXDEBUG=
  CXXWARNINGS="-Wall"
  CXXSTATIC_BINDING="-Bstatic"
  CXXDYNAMIC_BINDING="-Bdynamic"
  CXXSTUFF=
  for flag in $CXXFLAGS; do
    case $flag in
      -O)  CXXOPT="$CXXOPT -O2";;
      -O*) CXXOPT="$CXXOPT $flag";;
      -g*) CXXDEBUG="$flag";;
      -W*) CXXWARNINGS="$CXXWARNINGS $flag";;
      *)   CXXSTUFF="$CXXSTUFF $flag";;
    esac
  done
else
  CXXOPT="-DNDEBUG"
  CXXDEBUG=
  CXXWARNINGS=
  CXXSTATIC_BINDING="-static"
  CXXDYNAMIC_BINDING=
  CXXSTUFF=
  for flag in $CXXFLAGS; do
    case $flag in
      -O*) CXXOPT="$CXXOPT $flag";;
      -g*) CXXDEBUG="$flag";;
      -W*) CXXWARNINGS="$CXXWARNINGS $flag";;
      *)   CXXSTUFF="$CXXSTUFF $flag";;
    esac
  done
fi
AC_MSG_CHECKING(for ${CXX} compiler warning options)
AC_MSG_RESULT(${CXXWARNINGS})
AC_MSG_CHECKING(for ${CXX} compiler optimizing options)
AC_MSG_RESULT(${CXXOPT})
AC_MSG_CHECKING(for ${CXX} compiler extra libraries)
AC_MSG_RESULT(${CXXLIBS})
AC_MSG_CHECKING(for ${CXX} compiler static binding options)
AC_MSG_RESULT(${CXXSTATIC_BINDING})
AC_MSG_CHECKING(for ${CXX} compiler dynamic binding options)
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
ICE_ELIDE_CONSTRUCTORS
fi
CXXSTUFF="$EXTERNAL_TEMPLATES $ELIDE_CONSTRUCTORS"
AC_SUBST(CXXSTUFF)dnl
])dnl
dnl
dnl
dnl
dnl Look for emacs; put it in output variable `EMACS'.
dnl
dnl
AC_DEFUN(ICE_PROG_EMACS, 
[
AC_CHECK_PROGS(EMACS, emacs temacs xemacs lemacs)
])dnl
dnl
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
dnl ICE_TYPE_SIGNAL: like AC_TYPE_SIGNAL, but use C++ for checks.
dnl
AC_DEFUN(ICE_TYPE_SIGNAL,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING([return type of C++ signal handlers])
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
dnl If the C++ library has a ostrstream::frozen() function,
dnl define HAVE_FROZEN_OSTRSTREAM.
dnl
AC_DEFUN(ICE_CHECK_FROZEN_OSTRSTREAM,
[
AC_REQUIRE([AC_PROG_CXX])
AC_MSG_CHECKING([for ostrstream::frozen()])
AC_CACHE_VAL(ice_cv_frozen_ostrstream,
[
AC_LANG_SAVE
AC_LANG_CPLUSPLUS
AC_TRY_COMPILE([#include <strstream.h>],
[ostrstream os; int frozen = os.frozen();],
ice_cv_frozen_ostrstream=yes, ice_cv_frozen_ostrstream=no)
AC_LANG_RESTORE
])
AC_MSG_RESULT($ice_cv_frozen_ostrstream)
if test "$ice_cv_frozen_ostrstream" = yes; then
AC_DEFINE(HAVE_FROZEN_OSTRSTREAM)
fi
])dnl
dnl
dnl
dnl ICE_FIND_MOTIF: find OSF/Motif libraries and headers
dnl put Motif include directory in motif_includes
dnl put Motif library directory in motif_libraries
dnl and add appropriate flags to X_CFLAGS and X_LIBS
dnl
dnl
AC_DEFUN(ICE_FIND_MOTIF,
[
AC_REQUIRE([AC_PATH_XTRA])
AC_ARG_WITH(motif-includes,
[  --with-motif-includes=DIR   Motif include files are in DIR],
motif_includes="$withval", motif_includes=)
AC_ARG_WITH(motif-libraries,
[  --with-motif-libraries=DIR  Motif libraries are in DIR],
motif_libraries="$withval", motif_libraries=)
AC_MSG_CHECKING(for OSF/Motif)
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
	   "${prefix}"/*/include /usr/*/include /usr/local/*/include \
	   "${prefix}"/include/* /usr/include/* /usr/local/include/*; do
if test -f "$dir/Xm/Xm.h"; then
ice_cv_motif_includes="$dir"
break
fi
done
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
AC_TRY_LINK([#include <Xm/Xm.h>],[int a;],
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
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
	   /usr/lib/Motif2.0 /usr/lib/Motif1.2 /usr/lib/Motif1.1 \
	   /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
           /usr/dt/lib /usr/openwin/lib \
	   /usr/dt/*/lib /opt/*/lib /usr/lib/Motif* \
	   "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
	   "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
if test -d "$dir" && test "`ls $dir/libXm.* 2> /dev/null`" != ""; then
ice_cv_motif_libraries="$dir"
break
fi
done
])
#
ice_motif_save_LIBS="$LIBS"
ice_motif_save_CFLAGS="$CFLAGS"
ice_motif_save_CPPFLAGS="$CPPFLAGS"
ice_motif_save_LDFLAGS="$LDFLAGS"
])
#
motif_libraries="$ice_cv_motif_libraries"
fi
# Add Motif definitions to X flags
#
if test "$motif_includes" != "" && test "$motif_includes" != "$x_includes"
then
X_CFLAGS="-I$motif_includes $X_CFLAGS"
fi
if test "$motif_libraries" != "" && test "$motif_libraries" != "$x_libraries"
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
AC_MSG_RESULT(
  [libraries $motif_libraries_result, headers $motif_includes_result])
])dnl
dnl
dnl
dnl ICE_FIND_ATHENA: find OSF/Athena libraries and headers
dnl put Athena include directory in athena_includes
dnl put Athena library directory in athena_libraries
dnl and add appropriate flags to X_CFLAGS and X_LIBS
dnl
dnl
AC_DEFUN(ICE_FIND_ATHENA,
[
AC_REQUIRE([AC_PATH_XTRA])
AC_ARG_WITH(athena-includes,
[  --with-athena-includes=DIR   Athena include files are in DIR],
athena_includes="$withval", athena_includes=)
AC_ARG_WITH(athena-libraries,
[  --with-athena-libraries=DIR  Athena libraries are in DIR],
athena_libraries="$withval", athena_libraries=)
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
	   "${prefix}"/*/include /usr/*/include /usr/local/*/include \
	   "${prefix}"/include/* /usr/include/* /usr/local/include/*; do
if test -f "$dir/X11/Xaw/Text.h"; then
ice_cv_athena_includes="$dir"
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
AC_TRY_LINK([
#include <X11/Intrinsic.h>
#include <X11/Xaw/Text.h>
],[int a;],
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
	   "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
	   "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
if test -d "$dir" && test "`ls $dir/libXaw.* 2> /dev/null`" != ""; then
ice_cv_athena_libraries="$dir"
break
fi
done
])
#
ice_athena_save_LIBS="$LIBS"
ice_athena_save_CFLAGS="$CFLAGS"
ice_athena_save_CPPFLAGS="$CPPFLAGS"
ice_athena_save_LDFLAGS="$LDFLAGS"
])
#
athena_libraries="$ice_cv_athena_libraries"
fi
# Add Athena definitions to X flags
#
if test "$athena_includes" != "" && test "$athena_includes" != "$x_includes"
then
X_CFLAGS="-I$athena_includes $X_CFLAGS"
fi
if test "$athena_libraries" != "" && test "$athena_libraries" != "$x_libraries"
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
AC_MSG_RESULT(
  [libraries $athena_libraries_result, headers $athena_includes_result])
])dnl
