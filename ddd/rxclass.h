// $Id$  -*- C++ -*-
// Regular expression class, based on POSIX regcomp()/regexec() interface

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
#include "bool.h"

extern "C" {
#include <sys/types.h>

// Avoid conflicts with `regex()' function
#define regex c_regex

// Don't include old libg++ `regex.h' contents
#define __REGEXP_LIBRARY

#ifndef __STDC__
#define __STDC__ 1              // Reguired for KCC when using GNU includes
#endif

#if defined(HAVE_REGCOMP) && defined(HAVE_REGEXEC) && !defined(REGCOMP_BROKEN)

// Some old versions of libg++ contain a `regex.h' file.  Avoid this.
#if !defined(REG_EXTENDED) && defined(HAVE_REGEX_H)
#include <regex.h>		// POSIX.2 interface
#endif

// Try hard-wired path to get native <regex.h>.
#if !defined(REG_EXTENDED) && defined(HAVE_REGEX_H)
#include </usr/include/regex.h>	// POSIX.2 interface
#endif

// Some more GNU headers.
#if !defined(REG_EXTENDED) && defined(HAVE_RX_H)
#include <rx.h>	 	        // Header from GNU rx 0.07
#endif

#if !defined(REG_EXTENDED) && defined(HAVE_RXPOSIX_H)
#include <rxposix.h>		// Header from GNU rx 1.0 and later
#endif

#endif // !defined(REGCOMP_BROKEN)

// Use GNU rx as shipped with DDD.
#if !defined(REG_EXTENDED)
#include <librx/rx.h>		// Header from GNU rx 0.07, as shipped with DDD
#define GNU_LIBRX_USED
#endif

#undef regex

#ifdef RE_DUP_MAX
#undef RE_DUP_MAX		// Avoid trouble with later redefinitions
#endif
}


// The interface is similar to Doug Lea's libg++ `Regex' class, 
// except that we use extended regexps and rely on POSIX.2 functions.

class regex
{
private:
    regex(const regex&) {}	      // no X(X&)
    void operator = (const regex&) {} // no assignment

protected:
    regex_t compiled;		// "^" + regexp
    char prefix[32];		// constant prefix (for faster search)

    regmatch_t *exprs;		// Matched expressions
    size_t nexprs() const;	// Number of expressions

    // Fatal error ERRCODE with regexp source SRC
    void fatal(int errcode, const char *src = 0);

    // Create a prefix from T and FLAGS
    static char get_prefix(const char *& t, int flags);

public:
    // Create and compile an (extended) regular expression in T.
    regex(const char* t, int flags = REG_EXTENDED);
    ~regex();

    // Iff T matches S at POS, return length of matched string;
    // -1, otherwise.  LEN is the length of S.
    int match(const char* s, int len, int pos = 0) const;

    // Search T in S; return position of first occurrence.
    // If STARTPOS is positive, start search from that position.
    // If STARTPOS is negative, perform reverse search from that position 
    // and return last occurrence.
    // LEN is the length of S.
    // MATCHLEN contains the length of the matched expression.
    // If T is not found, return -1.
    int search(const char* s, int len, 
	       int& matchlen, int startpos = 0) const;

    // Return matching info for NTH expression in START and LENGTH
    // Expression 0 is the entire regexp T; expression 1 and later are
    // the subexpressions of T.  Returns true iff successful.
    bool match_info(int& start, int& length, int nth = 0) const;

    bool OK() const;  // representation invariant
};

// Return number of expressions
inline size_t regex::nexprs() const
{
#if defined(HAVE_REGEX_T_RE_NSUB)
    return compiled.re_nsub + 1;
#elif defined(HAVE_REGEX_T_N_SUBEXPS)
    return compiled.n_subexps + 1;
#else
    return 32;  // Should be sufficient
#endif
}

// Some built-in regular expressions

extern const regex rxwhite;          // = "[ \n\t\r\v\f]+"
extern const regex rxint;            // = "-?[0-9]+"
extern const regex rxdouble;         // = "-?(([0-9]+.[0-9]*)|
                                     //    ([0-9]+)|(.[0-9]+))
                                     //    ([eE][---+]?[0-9]+)?"
extern const regex rxalpha;          // = "[A-Za-z]+"
extern const regex rxlowercase;      // = "[a-z]+"
extern const regex rxuppercase;      // = "[A-Z]+"
extern const regex rxalphanum;       // = "[0-9A-Za-z]+"
extern const regex rxidentifier;     // = "[A-Za-z_][A-Za-z0-9_]*"

#endif // _ICE_rxclass_h
// DON'T ADD ANYTHING behind this #endif
