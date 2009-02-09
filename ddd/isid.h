// $Id$ -*- C++ -*-
// isid() character tester

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2005 Free Software Foundation, Inc.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_isid_h
#define _DDD_isid_h

#include "bool.h"
#include <ctype.h>

// True if C is an identifier character
inline bool isid(char c)
{
    return isalnum((unsigned char)c) || c == '_' || c == '$';
}

// True if C can be a Bash "special" variable
inline bool is_bash_special(char c)
{
  switch(c) {
  case '*': case '@': case '#': case '?': case '-': case '$':
  case '!': case '_':
    return true;
  default:
    return false;
  }
}

// True if C can be a GNU Make "automatic" variable
inline bool is_make_automatic(char c)
{
  switch(c) {
  case '@': case '%': case '*': case '+': case '|': case '<':
  case '^': case '?':
    return true;
  default:
    return false;
  }
}

// True if C is a Perl prefix character
inline bool is_perl_prefix(char c)
{
    return c == '@' || c == '%' || c == '$';
}

#endif // _DDD_isid_h
// DON'T ADD ANYTHING BEHIND THIS #endif
