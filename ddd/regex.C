// $Id$ -*- C++ -*-
//

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

// $Log$
// Revision 1.1  1995/05/01 15:47:55  zeller
// Initial revision
//
// Revision 1.1.1.5  1995/04/20  19:22:01  zeller
// Fix: memset() may be declared in <string.h>
//
// Revision 1.1.1.4  1995/04/20  18:00:36  zeller
// Fix: provided appropriate back-up for systems without memset()
//
// Revision 1.1.1.3  1995/04/10  20:05:20  zeller
// Fix: POSIX-like memset decl returns void *
//
// Revision 1.1.1.2  1995/04/03  14:59:03  zeller
// Fix: provide POSIX-like memset() decl
//
// Revision 1.1.1.1  1995/02/09  09:12:04  zeller
// ICE 0.5 distribution
//
// Revision 1.3  1995/02/08  16:01:17  zeller
// New: Compiler no more complains on unused RCS ID
//
// Revision 1.2  1995/01/14  14:48:27  zeller
// New license for ICE 0.5
//

static const char rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

/* 
  regex class implementation
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "config.h"
#include "regex.h"

#include <ctype.h>
#include <new.h>
#include <malloc.h>
#include <iostream.h>
#include <string.h>

#ifdef HAVE_MEMORY_H
#include <memory.h>
#endif

#if defined(HAVE_MEMSET) && !defined(HAVE_MEMSET_DECL)
extern "C" void *memset(void *s, int c, size_t n);
#endif

extern "C" {
#include <librx/rx.h>
}

regex::~regex()
{
    if (buf->buffer) 
	free(buf->buffer);
    if (buf->fastmap) 
	free(buf->fastmap);
    delete buf;
    delete reg;
}

regex::regex(const char* t, int fast, int bufsize, 
	     const char* transtable)
{
    int tlen = (t == 0)? 0 : strlen(t);
    buf = new re_pattern_buffer;
#ifdef HAVE_MEMSET
    memset (buf, 0, sizeof(re_pattern_buffer));
#else
    for (int p = 0; p < sizeof(re_pattern_buffer); p++)
	((char *)buf)[p] = '\0';
#endif
    reg = new re_registers;
    if (fast)
	buf->fastmap = (char*)malloc(256);
    else
	buf->fastmap = 0;
    buf->translate = (unsigned char*)transtable;
    if (tlen > bufsize)
	bufsize = tlen;
    buf->allocated = bufsize;
    buf->buffer = (char *)malloc(buf->allocated);
    const char* msg = re_compile_pattern((const char*)t, tlen, buf);
    if (msg != 0)
    {
	cerr << "regex: " << msg << "\n";
	abort();
    }
    else if (fast)
	re_compile_fastmap(buf);
}

int regex::match_info(int& start, int& length, int nth) const
{
    if ((unsigned)(nth) >= RE_NREGS)
	return 0;
    else
    {
	start = reg->start[nth];
	length = reg->end[nth] - start;
	return start >= 0 && length >= 0;
    }
}

int regex::search(const char* s, int len, int& matchlen, int startpos) const
{
    int matchpos, pos, range;
    if (startpos >= 0)
    {
	pos = startpos;
	range = len - startpos;
    }
    else
    {
	pos = len + startpos;
	range = -pos;
    }
    matchpos = re_search_2(buf, 0, 0, (char*)s, len, pos, range, reg, len);
    if (matchpos >= 0)
	matchlen = reg->end[0] - reg->start[0];
    else
	matchlen = 0;
    return matchpos;
}

int regex::match(const char*s, int len, int p) const
{
    if (p < 0)
    {
	p += len;
	if (p > len)
	    return -1;
	return re_match_2(buf, 0, 0, (char*)s, p, 0, reg, p);
    }
    else if (p > len)
	return -1;
    else
	return re_match_2(buf, 0, 0, (char*)s, len, p, reg, len);
}

int regex::OK() const
{
    // Can't verify much, since we've lost the original string
    int v = buf != 0;             // have a regex buf
    v &= buf->buffer != 0;        // with a pat
    if (!v)
        abort();
    return v;
}

/*
 some built-in Regular expressions
*/

const regex rxwhite("[ \n\t\r\v\f]+", 1);
const regex rxint("-?[0-9]+", 1);
const regex rxdouble("-?\\(\\([0-9]+\\.[0-9]*\\)\\|\\([0-9]+\\)\\|\\(\\.[0-9]+\\)\\)\\([eE][---+]?[0-9]+\\)?", 1, 200);
const regex rxalpha("[A-Za-z]+", 1);
const regex rxlowercase("[a-z]+", 1);
const regex rxuppercase("[A-Z]+", 1);
const regex rxalphanum("[0-9A-Za-z]+", 1);
const regex rxidentifier("[A-Za-z_][A-Za-z0-9_]*", 1);
