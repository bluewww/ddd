// $Id$  -*- C++ -*-
// Regular expression class, based on POSIX regcomp()/regexec() interface

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _ICE_rxclass_h
#define _ICE_rxclass_h

#ifdef __GNUG__
#pragma interface
#endif

#include "config.h"

extern "C" {
#if defined(HAVE_REGCOMP) && defined(HAVE_REGEXEC) && defined(HAVE_REGEX_H)
#include <regex.h>		// POSIX.2 interface
#elif defined(HAVE_REGCOMP) && defined(HAVE_REGEXEC) && defined(HAVE_RX_H)
#include <rx.h>	 	        // Header from GNU g++-include
#else
#include <rx/rxposix.h>		// Header from GNU rx 1.0 and later
#endif
}

class regex
{
private:
    regex(const regex&) {}	      // no X(X&)
    void operator = (const regex&) {} // no assignment

protected:
    regex_t compiled;		// The compiled regular expression
    regmatch_t *matches;	// Matched sub-expressions

    size_t nmatches() const;	// Number of sub-expressions

public:
    regex(const char* t, int flags = REG_EXTENDED | REG_NEWLINE);
    ~regex();

    int match(const char* s, int len, int pos = 0) const;
    int search(const char* s, int len, 
	       int& matchlen, int startpos = 0) const;
    int match_info(regoff_t& start, regoff_t& length, int nth = 0) const;

    int OK() const;  // representation invariant
};

// Return number of subexpressions
inline size_t regex::nmatches() const
{
#if defined(HAVE_REGEX_T_RE_NSUB)
    return compiled.re_nsub;
#elif defined(HAVE_REGEX_T_N_SUBEXPS)
    return compiled.n_subexps;
#else
    return 32;  // Should be sufficient
#endif
}

// Some built-in regular expressions

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

#endif // _ICE_rxclass_h
// DON'T ADD ANYTHING behind this #endif
