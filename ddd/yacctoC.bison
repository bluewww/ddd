# $Id$
# Adapt BISON output for C++ usage

# Copyright (C) 1995-1999 Technische Universitaet Braunschweig, Germany.
# Written by Andreas Zeller <zeller@gnu.org>.
# 
# This file is part of DDD.
# 
# DDD is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 3 of the License, or (at your option) any later version.
# 
# DDD is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public
# License along with DDD -- see the file COPYING.
# If not, see <http://www.gnu.org/licenses/>.
# 
# DDD is the data display debugger.
# For details, see the DDD World-Wide-Web page, 
# `http://www.gnu.org/software/ddd/',
# or send a mail to the DDD developers <ddd@gnu.org>.

# Declare functions
1i\
// This file was modified for -*- C++ -*-\
// using $RCSfile$ $Revision$\
extern void yyerror(const char *s);\
extern int yylex();\

# Make YYSTYPE a struct such that we can store class objects
# (use anonymous unions to save space!)
s!^typedef union *[a-zA-Z_]*\(.*\)$!typedef struct _YYSTYPE \1!

# We cannot use alloca with class objects.
# Thus, we use new/delete instead (rather ugly).

# Make sure all is deleted when we exit
\!^#define YYERRCODE.*!a\
\
// This deletes the "kill" array upon destruction.\
// Will be used as automatic variable in yyparse().\
// Added by $RCSfile$\
struct YYMEMHANDLER {\
    YYSTYPE *kill;\
\
    YYMEMHANDLER()\
	: kill(0)\
    {}\
\
    ~YYMEMHANDLER()\
    {\
	if (kill != 0)\
    	    delete[] kill;\
        kill = 0;\
    }\
\
private:\
    YYMEMHANDLER(const YYMEMHANDLER&);\
    YYMEMHANDLER& operator=(const YYMEMHANDLER&);\
};\

\!.int yystacksize!a\
  YYMEMHANDLER yymem; // Added by $RCSfile$

# stack reallocation -> own code
\!.*yyvs = (YYSTYPE \*!,\!unsigned int!c\
\
      // Added by $RCSfile$. \
      YYSTYPE *new_yyvs = new YYSTYPE [yystacksize];\
      if (new_yyvs == 0)\
      {\
          yyerror("parser stack overflow");\
          return 2;\
      }\
      for (int yycopy = 0; yycopy < yystacksize; yycopy++)\
      {\
          new_yyvs[yycopy] = yyvs[yycopy];\
      }\
      delete[] yyvs; yyvs = new_yyvs;\
      yymem.kill = yyvs; // make sure yyvs is deleted upon return \


# malloc/free -> new/delete
s!(\([^)]*\) *\*) *malloc *(\([^)]*\))!new \1[\2]!g
s!free *(\([^)]*\))!delete [] \1!g

# malloc.h -> stdlib.h
s!<malloc\.h>!<stdlib.h>!

# Fix a warning on g++-4.1.2 (and probably other versions too).
# This jump to the next statement will probably be eliminated
# by peephole optimisation.
/^yyerrlab1:$/ i\
  goto yyerrlab1; /* fix a g++-4.1.2 warning: unused label */

# bison-1.875 generates code the doesn't compile.  Fix it.
/Suppress GCC warning that yyerrlab1 is unused when no action/,/#endif/d

