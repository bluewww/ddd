// $Id$  -*- C++ -*-
// This may look like C code, but it is really -*- C++ -*-

/* 
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef _ICE_regex_h
#define _ICE_regex_h

#ifdef __GNUG__
#pragma interface
#endif

struct re_pattern_buffer;       // defined elsewhere
struct re_registers;

class regex
{
private:

    regex(const regex&) {}	      // no X(X&)
    void operator = (const regex&) {} // no assignment

protected:
    re_pattern_buffer* buf;
    re_registers* reg;

public:
    regex(const char* t, 
	  int fast = 0, 
	  int bufsize = 40, 
	  const char* transtable = 0);

    ~regex();

    int match(const char* s, int len, int pos = 0) const;
    int search(const char* s, int len, 
	       int& matchlen, int startpos = 0) const;
    int match_info(int& start, int& length, int nth = 0) const;

    int OK() const;  // representation invariant

};

// some built-in regular expressions

extern const regex rxwhite;          // = "[ \n\t\r\v\f]+"
extern const regex rxint;            // = "-?[0-9]+"
extern const regex rxdouble;         // = "-?\\(\\([0-9]+\\.[0-9]*\\)\\|
                                     //    \\([0-9]+\\)\\|\\(\\.[0-9]+\\)\\)
                                     //    \\([eE][---+]?[0-9]+\\)?"
extern const regex rxalpha;          // = "[A-Za-z]+"
extern const regex rxlowercase;      // = "[a-z]+"
extern const regex rxuppercase;      // = "[A-Z]+"
extern const regex rxalphanum;       // = "[0-9A-Za-z]+"
extern const regex rxidentifier;     // = "[A-Za-z_][A-Za-z0-9_]*"

#endif // _ICE_regex_h
// DON'T ADD ANYTHING behind this #endif
