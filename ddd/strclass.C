// $Id$ -*- C++ -*-
// The libg++ String class, but named "string" for coexistence with Xt

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

char strclass_rcsid[] = 
    "$Id$";

/* 
  string class implementation
 */

#ifdef __GNUG__
#pragma implementation
#endif

#ifndef MALLOC_DEBUG
#define MALLOC_DEBUG 0
#endif

#if MALLOC_DEBUG
extern "C" int malloc_verify();
#include "assert.h"
#endif

#include "strclass.h"
#include "config.h"
#include "return.h"
#include <ctype.h>
#include <limits.h>
#include <new.h>

#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

void string::error(const char* msg) const
{
    cerr << "string: " << msg << "\n";
    abort();
}

string::operator const char*() const
{ 
    return (const char *)chars();
}

//  globals

strRep  _nilstrRep = { 0, 1, { 0 } }; // nil strings point here
string _nilstring;               // nil subStrings point here




/*
 the following inline fcts are specially designed to work
 in support of string classes, and are not meant as generic replacements
 for libc "str" functions.

 inline copy fcts -  I like left-to-right from->to arguments.
 all versions assume that `to' argument is non-null

 These are worth doing inline, rather than through calls because,
 via procedural integration, adjacent copy calls can be smushed
 together by the optimizer.
*/

// copy n bytes
inline static void ncopy(const char* from, char* to, int n)
{
    if (from != to) while (--n >= 0) *to++ = *from++;
}

// copy n bytes, null-terminate
inline static void ncopy0(const char* from, char* to, int n)
{
    if (from != to) 
    {
	while (--n >= 0) *to++ = *from++;
	*to = 0;
    }
    else
	to[n] = 0;
}

// copy until null
inline static void scopy(const char* from, char* to)
{
    if (from != 0) while((*to++ = *from++) != 0);
}

// copy right-to-left
inline static void revcopy(const char* from, char* to, int n)
{
    if (from != 0) while (--n >= 0) *to-- = *from--;
}


inline static int slen(const char* t) // inline  strlen
{
    if (t == 0)
	return 0;
    else
    {
	const char* a = t;
	while (*a++ != 0);
	return a - 1 - t;
    }
}

// minimum & maximum representable rep size

#define MAXstrRep_SIZE   ((1 << (sizeof(int) * CHAR_BIT - 1)) - 1)
#define MINstrRep_SIZE   16

#ifndef MALLOC_MIN_OVERHEAD
#define MALLOC_MIN_OVERHEAD  4
#endif

// The basic allocation primitive:
// Always round request to something close to a power of two.
// This ensures a bit of padding, which often means that
// concatenations don't have to realloc. Plus it tends to
// be faster when lots of Strings are created and discarded,
// since just about any version of malloc (op new()) will
// be faster when it can reuse identically-sized chunks

inline static strRep* string_Snew(int newsiz)
{
    unsigned int siz = sizeof(strRep) + newsiz + MALLOC_MIN_OVERHEAD;
    unsigned int allocsiz = MINstrRep_SIZE;
    while (allocsiz < siz) allocsiz <<= 1;
    allocsiz -= MALLOC_MIN_OVERHEAD;
    if (allocsiz >= MAXstrRep_SIZE)
    {
	cerr << "string: requested length out of range\n";
	abort();
    }
    
    strRep* rep = (strRep *) new char[allocsiz];
    rep->sz = allocsiz - sizeof(strRep);
    return rep;
}

// Do-something-while-allocating routines.

// We live with two ways to signify empty Sreps: either the
// null pointer (0) or a pointer to the nilstrRep.

// We always signify unknown source lengths (usually when fed a char*)
// via len == -1, in which case it is computed.

// allocate, copying src if nonull

strRep* string_Salloc(strRep* old, const char* src, int srclen, int newlen)
{
#if MALLOC_DEBUG
    assert(malloc_verify());
#endif

    if (old == &_nilstrRep) old = 0;
    if (srclen < 0) srclen = slen(src);
    if (newlen < srclen) newlen = srclen;
    strRep* rep;
    if (old == 0 || unsigned(newlen) > old->sz)
	rep = string_Snew(newlen);
    else
	rep = old;

    rep->len = newlen;
    ncopy0(src, rep->s, srclen);

    if (old != rep && old != 0) delete[] (char *)old;

    return rep;
}

// reallocate: Given the initial allocation scheme, it will
// generally be faster in the long run to get new space & copy
// than to call realloc

static strRep *string_Sresize(strRep* old, int newlen)
{
#if MALLOC_DEBUG
    assert(malloc_verify());
#endif

    if (old == &_nilstrRep) old = 0;
    strRep* rep;
    if (old == 0)
	rep = string_Snew(newlen);
    else if (unsigned(newlen) > old->sz)
    {
	rep = string_Snew(newlen);
	ncopy0(old->s, rep->s, old->len);
	delete[] (char *)old;
    }
    else
	rep = old;

    rep->len = newlen;

    return rep;
}

void string::alloc (int newsize)
{
    unsigned short old_len = rep->len;
    rep = string_Sresize(rep, newsize);
    rep->len = old_len;
}


// like allocate, but we know that src is a strRep

strRep* string_Scopy(strRep* old, strRep* s)
{
#if MALLOC_DEBUG
    assert(malloc_verify());
#endif

    if (old == &_nilstrRep) old = 0;
    if (s == &_nilstrRep) s = 0;
    if (old == s) 
	return (old == 0)? &_nilstrRep : old;
    else if (s == 0)
    {
	old->s[0] = 0;
	old->len = 0;
	return old;
    }
    else 
    {
	strRep* rep;
	unsigned newlen = s->len;
	if (old == 0 || newlen > old->sz)
	{
	    if (old != 0) delete[] (char *)old;
	    rep = string_Snew(newlen);
	}
	else
	    rep = old;
	rep->len = newlen;
	ncopy0(s->s, rep->s, newlen);
	return rep;
    }
}

// allocate & concatenate

strRep* string_Scat(strRep* old, 
		    const char* s, 
		    int srclen, 
		    const char* t, 
		    int tlen)
{
#if MALLOC_DEBUG
    assert(malloc_verify());
#endif

    if (old == &_nilstrRep) old = 0;
    if (srclen < 0) srclen = slen(s);
    if (tlen < 0) tlen = slen(t);
    unsigned newlen = srclen + tlen;
    strRep* rep;

    if (old == 0 || newlen > old->sz || 
	(t >= old->s && t < &(old->s[old->len]))) // beware of aliasing
	rep = string_Snew(newlen);
    else
	rep = old;

    rep->len = newlen;

    ncopy(s, rep->s, srclen);
    ncopy0(t, &(rep->s[srclen]), tlen);

    if (old != rep && old != 0) delete[] (char *)old;

    return rep;
}

// double-concatenate

strRep* string_Scat(strRep* old, const char* s, int srclen, 
		    const char* t, int tlen,
		    const char* u, int ulen)
{
#if MALLOC_DEBUG
    assert(malloc_verify());
#endif

    if (old == &_nilstrRep) old = 0;
    if (srclen < 0) srclen = slen(s);
    if (tlen < 0) tlen = slen(t);
    if (ulen < 0) ulen = slen(u);
    unsigned newlen = srclen + tlen + ulen;
    strRep* rep;
    if (old == 0 || newlen > old->sz || 
	(t >= old->s && t < &(old->s[old->len])) ||
	(u >= old->s && u < &(old->s[old->len])))
	rep = string_Snew(newlen);
    else
	rep = old;

    rep->len = newlen;

    ncopy(s, rep->s, srclen);
    ncopy(t, &(rep->s[srclen]), tlen);
    ncopy0(u, &(rep->s[srclen+tlen]), ulen);

    if (old != rep && old != 0) delete[] (char *)old;

    return rep;
}

// like cat, but we know that new stuff goes in the front of existing rep

strRep* string_Sprepend(strRep* old, const char* t, int tlen)
{
#if MALLOC_DEBUG
    assert(malloc_verify());
#endif

    char* s;
    int srclen;
    if (old == &_nilstrRep || old == 0)
    {
	s = 0; old = 0; srclen = 0;
    }
    else
    {
	s = old->s; srclen = old->len;
    }
    if (tlen < 0) tlen = slen(t);
    unsigned newlen = srclen + tlen;
    strRep* rep;
    if (old == 0 || newlen > old->sz || 
	(t >= old->s && t < &(old->s[old->len])))
	rep = string_Snew(newlen);
    else
	rep = old;

    rep->len = newlen;

    revcopy(&(s[srclen]), &(rep->s[newlen]), srclen+1);
    ncopy(t, rep->s, tlen);

    if (old != rep && old != 0) delete[] (char *)old;

    return rep;
}


// string compare: first argument is known to be non-null

inline static int scmp(const char* a, const char* b)
{
    if (b == 0)
	return *a != 0;
    else
    {
	signed char diff = 0;
	while ((diff = *a - *b++) == 0 && *a++ != 0);
	return diff;
    }
}


inline static int ncmp(const char* a, int al, const char* b, int bl)
{
    int n = (al <= bl)? al : bl;
    signed char diff;
    while (n-- > 0) if ((diff = *a++ - *b++) != 0) return diff;
    return al - bl;
}

int fcompare(const string& x, const string& y)
{
    const char* a = x.chars();
    const char* b = y.chars();
    int al = x.length();
    int bl = y.length();
    int n = (al <= bl)? al : bl;
    signed char diff = 0;
    while (n-- > 0)
    {
	char ac = *a++;
	char bc = *b++;
	if ((diff = ac - bc) != 0)
	{
	    if (ac >= 'a' && ac <= 'z')
		ac = ac - 'a' + 'A';
	    if (bc >= 'a' && bc <= 'z')
		bc = bc - 'a' + 'A';
	    if ((diff = ac - bc) != 0)
		return diff;
	}
    }
    return al - bl;
}

// these are not inline, but pull in the above inlines, so are 
// pretty fast

int compare(const string& x, const char* b)
{
    return scmp(x.chars(), b);
}

int compare(const string& x, const string& y)
{
    return scmp(x.chars(), y.chars());
}

int compare(const string& x, const subString& y)
{
    return ncmp(x.chars(), x.length(), y.chars(), y.length());
}

int compare(const subString& x, const string& y)
{
    return ncmp(x.chars(), x.length(), y.chars(), y.length());
}

int compare(const subString& x, const subString& y)
{
    return ncmp(x.chars(), x.length(), y.chars(), y.length());
}

int compare(const subString& x, const char* b)
{
    if (b == 0)
	return x.length();
    else
    {
	const char* a = x.chars();
	int n = x.length();
	signed char diff;
	while (n-- > 0) if ((diff = *a++ - *b++) != 0) return diff;
	return (*b == 0) ? 0 : -1;
    }
}

/*
 index fcts
*/

int string::search(int start, int sl, char c) const
{
    const char* s = chars();
    if (sl > 0)
    {
	if (start >= 0)
	{
	    const char* a = &(s[start]);
	    const char* lasta = &(s[sl]);
	    while (a < lasta) if (*a++ == c) return --a - s;
	}
	else
	{
	    const char* a = &(s[sl + start + 1]);
	    while (--a >= s) if (*a == c) return a - s;
	}
    }
    return -1;
}

int string::search(int start, int sl, const char* t, int tl) const
{
    const char* s = chars();
    if (tl < 0) tl = slen(t);
    if (sl > 0 && tl > 0)
    {
	if (start >= 0)
	{
	    const char* lasts = &(s[sl - tl]);
	    const char* lastt = &(t[tl]);
	    const char* p = &(s[start]);

	    while (p <= lasts)
	    {
		const char* x = p++;
		const char* y = t;
		while (*x++ == *y++) if (y >= lastt) return --p - s;
	    }
	}
	else
	{
	    const char* firsts = &(s[tl - 1]);
	    const char* lastt =  &(t[tl - 1]);
	    const char* p = &(s[sl + start + 1]); 

	    while (--p >= firsts)
	    {
		const char* x = p;
		const char* y = lastt;
		while (*x-- == *y--) if (y < t) return ++x - s;
	    }
	}
    }
    return -1;
}

int string::match(int start, int sl, int exact, const char* t, int tl) const
{
    if (tl < 0) tl = slen(t);

    if (start < 0)
    {
	start = sl + start - tl + 1;
	if (start < 0 || (exact && start != 0))
	    return -1;
    }
    else if (exact && sl - start != tl)
	return -1;

    if (sl == 0 || tl == 0 || sl - start < tl || start >= sl)
	return -1;

    int n = tl;
    const char* s = &(rep->s[start]);
    while (--n >= 0) if (*s++ != *t++) return -1;
    return tl;
}

void subString::assign(strRep* ysrc, const char* ys, int ylen)
{
    if (&S == &_nilstring) return;

    if (ylen < 0) ylen = slen(ys);
    strRep* targ = S.rep;
    unsigned sl = targ->len - len + ylen;

    if (ysrc == targ || sl >= targ->sz)
    {
	strRep* oldtarg = targ;
	targ = string_Sresize(0, sl);
	ncopy(oldtarg->s, targ->s, pos);
	ncopy(ys, &(targ->s[pos]), ylen);
	scopy(&(oldtarg->s[pos + len]), &(targ->s[pos + ylen]));
	delete[] (char *)oldtarg;
    }
    else if (len == ylen)
	ncopy(ys, &(targ->s[pos]), len);
    else if (unsigned(ylen) < len)
    {
	ncopy(ys, &(targ->s[pos]), ylen);
	scopy(&(targ->s[pos + len]), &(targ->s[pos + ylen]));
    }
    else
    {
	revcopy(&(targ->s[targ->len]), &(targ->s[sl]), targ->len-pos-len +1);
	ncopy(ys, &(targ->s[pos]), ylen);
    }
    targ->len = sl;
    S.rep = targ;
}



/*
 * substitution
 */


int string::_gsub(const char* pat, int pl, const char* r, int rl)
{
    int nmatches = 0;
    if (pl < 0) pl = slen(pat);
    if (rl < 0) rl = slen(r);
    int sl = length();
    if (sl <= 0 || pl <= 0 || sl < pl)
	return nmatches;
  
    const char* s = chars();

    // prepare to make new rep
    strRep* nrep = 0;
    int nsz = 0;
    char* x = 0;

    int si = 0;
    int xi = 0;
    int remaining = sl;

    while (remaining >= pl)
    {
	int pos = search(si, sl, pat, pl);
	if (pos < 0)
	    break;
	else
	{
	    ++nmatches;
	    int mustfit = xi + remaining + rl - pl;
	    if (mustfit >= nsz)
	    {
		if (nrep != 0) nrep->len = xi;
		nrep = string_Sresize(nrep, mustfit);
		nsz = nrep->sz;
		x = nrep->s;
	    }
	    pos -= si;
	    ncopy(&(s[si]), &(x[xi]), pos);
	    ncopy(r, &(x[xi + pos]), rl);
	    si += pos + pl;
	    remaining -= pos + pl;
	    xi += pos + rl;
	}
    }

    if (nrep == 0)
    {
	if (nmatches == 0)
	    return nmatches;
	else
	    nrep = string_Sresize(nrep, xi+remaining);
    }

    ncopy0(&(s[si]), &(x[xi]), remaining);
    nrep->len = xi + remaining;

    if (nrep->len <= rep->sz)   // fit back in if possible
    {
	rep->len = nrep->len;
	ncopy0(nrep->s, rep->s, rep->len);
	delete[] (char *)nrep;
    }
    else
    {
	delete[] (char *)rep;
	rep = nrep;
    }
    return nmatches;
}

int string::_gsub(const regex& pat, const char* r, int rl)
{
    int nmatches = 0;
    int sl = length();
    if (sl <= 0)
	return nmatches;

    if (rl < 0) rl = slen(r);

    const char* s = chars();

    strRep* nrep = 0;
    int nsz = 0;

    char* x = 0;

    int si = 0;
    int xi = 0;
    int remaining = sl;
    int  pos, pl = 0;		// how long is a regular expression?

    while (remaining > 0)
    {
	// unlike string search, the pos returned here is absolute
	pos = pat.search(s, sl, pl, si);
	if (pos < 0 || pl <= 0)
	    break;
	else
	{
	    ++nmatches;
	    int mustfit = xi + remaining + rl - pl;
	    if (mustfit >= nsz)
	    {
		if (nrep != 0) nrep->len = xi;
		nrep = string_Sresize(nrep, mustfit);
		x = nrep->s;
		nsz = nrep->sz;
	    }
	    pos -= si;
	    ncopy(&(s[si]), &(x[xi]), pos);
	    ncopy(r, &(x[xi + pos]), rl);
	    si += pos + pl;
	    remaining -= pos + pl;
	    xi += pos + rl;
	}
    }

    if (nrep == 0)
    {
	if (nmatches == 0)
	    return nmatches;
	else
	    nrep = string_Sresize(nrep, xi+remaining);
    }

    ncopy0(&(s[si]), &(x[xi]), remaining);
    nrep->len = xi + remaining;

    if (nrep->len <= rep->sz)   // fit back in if possible
    {
	rep->len = nrep->len;
	ncopy0(nrep->s, rep->s, rep->len);
	delete[] (char *)nrep;
    }
    else
    {
	delete[] (char *)rep;
	rep = nrep;
    }
    return nmatches;
}


/*
 * deletion
 */

void string::del(int pos, int len)
{
    if (pos < 0 || len <= 0 || (unsigned)(pos + len) > length()) return;
    int nlen = length() - len;
    int first = pos + len;
    ncopy0(&(rep->s[first]), &(rep->s[pos]), length() - first);
    rep->len = nlen;
}

void string::del(const regex& r, int startpos)
{
    int mlen;
    int first = r.search(chars(), length(), mlen, startpos);
    del(first, mlen);
}

void string::del(const char* t, int startpos)
{
    int tlen = slen(t);
    int p = search(startpos, length(), t, tlen);
    del(p, tlen);
}

void string::del(const string& y, int startpos)
{
    del(search(startpos, length(), y.chars(), y.length()), y.length());
}

void string::del(const subString& y, int startpos)
{
    del(search(startpos, length(), y.chars(), y.length()), y.length());
}

void string::del(char c, int startpos)
{
    del(search(startpos, length(), c), 1);
}

/*
 * subString extraction
 */


subString string::at(int first, int len)
{
    return _substr(first, len);
}

subString string::operator() (int first, int len)
{
    return _substr(first, len);
}

subString string::before(int pos)
{
    return _substr(0, pos);
}

subString string::through(int pos)
{
    return _substr(0, pos+1);
}

subString string::after(int pos)
{
    return _substr(pos + 1, length() - (pos + 1));
}

subString string::from(int pos)
{
    return _substr(pos, length() - pos);
}

subString string::at(const string& y, int startpos)
{
    int first = search(startpos, length(), y.chars(), y.length());
    return _substr(first,  y.length());
}

subString string::at(const subString& y, int startpos)
{
    int first = search(startpos, length(), y.chars(), y.length());
    return _substr(first, y.length());
}

subString string::at(const regex& r, int startpos)
{
    int mlen;
    int first = r.search(chars(), length(), mlen, startpos);
    return _substr(first, mlen);
}

subString string::at(const char* t, int startpos)
{
    int tlen = slen(t);
    int first = search(startpos, length(), t, tlen);
    return _substr(first, tlen);
}

subString string::at(char c, int startpos)
{
    int first = search(startpos, length(), c);
    return _substr(first, 1);
}

subString string::before(const string& y, int startpos)
{
    int last = search(startpos, length(), y.chars(), y.length());
    return _substr(0, last);
}

subString string::before(const subString& y, int startpos)
{
    int last = search(startpos, length(), y.chars(), y.length());
    return _substr(0, last);
}

subString string::before(const regex& r, int startpos)
{
    int mlen;
    int first = r.search(chars(), length(), mlen, startpos);
    return _substr(0, first);
}

subString string::before(char c, int startpos)
{
    int last = search(startpos, length(), c);
    return _substr(0, last);
}

subString string::before(const char* t, int startpos)
{
    int tlen = slen(t);
    int last = search(startpos, length(), t, tlen);
    return _substr(0, last);
}

subString string::through(const string& y, int startpos)
{
    int last = search(startpos, length(), y.chars(), y.length());
    if (last >= 0) last += y.length();
    return _substr(0, last);
}

subString string::through(const subString& y, int startpos)
{
    int last = search(startpos, length(), y.chars(), y.length());
    if (last >= 0) last += y.length();
    return _substr(0, last);
}

subString string::through(const regex& r, int startpos)
{
    int mlen;
    int first = r.search(chars(), length(), mlen, startpos);
    if (first >= 0) first += mlen;
    return _substr(0, first);
}

subString string::through(char c, int startpos)
{
    int last = search(startpos, length(), c);
    if (last >= 0) last += 1;
    return _substr(0, last);
}

subString string::through(const char* t, int startpos)
{
    int tlen = slen(t);
    int last = search(startpos, length(), t, tlen);
    if (last >= 0) last += tlen;
    return _substr(0, last);
}

subString string::after(const string& y, int startpos)
{
    int first = search(startpos, length(), y.chars(), y.length());
    if (first >= 0) first += y.length();
    return _substr(first, length() - first);
}

subString string::after(const subString& y, int startpos)
{
    int first = search(startpos, length(), y.chars(), y.length());
    if (first >= 0) first += y.length();
    return _substr(first, length() - first);
}

subString string::after(char c, int startpos)
{
    int first = search(startpos, length(), c);
    if (first >= 0) first += 1;
    return _substr(first, length() - first);
}

subString string::after(const regex& r, int startpos)
{
    int mlen;
    int first = r.search(chars(), length(), mlen, startpos);
    if (first >= 0) first += mlen;
    return _substr(first, length() - first);
}

subString string::after(const char* t, int startpos)
{
    int tlen = slen(t);
    int first = search(startpos, length(), t, tlen);
    if (first >= 0) first += tlen;
    return _substr(first, length() - first);
}

subString string::from(const string& y, int startpos)
{
    int first = search(startpos, length(), y.chars(), y.length());
    return _substr(first, length() - first);
}

subString string::from(const subString& y, int startpos)
{
    int first = search(startpos, length(), y.chars(), y.length());
    return _substr(first, length() - first);
}

subString string::from(const regex& r, int startpos)
{
    int mlen;
    int first = r.search(chars(), length(), mlen, startpos);
    return _substr(first, length() - first);
}

subString string::from(char c, int startpos)
{
    int first = search(startpos, length(), c);
    return _substr(first, length() - first);
}

subString string::from(const char* t, int startpos)
{
    int tlen = slen(t);
    int first = search(startpos, length(), t, tlen);
    return _substr(first, length() - first);
}



/*
 * split/join
 */


int split(const string& src, string results[], int n, const string& sep)
{
    string x = src;
    const char* s = x.chars();
    int sl = x.length();
    int i = 0;
    int pos = 0;
    while (i < n && pos < sl)
    {
	int p = x.search(pos, sl, sep.chars(), sep.length());
	if (p < 0)
	    p = sl;
	results[i].rep = string_Salloc(results[i].rep, &(s[pos]), p - pos, p - pos);
	i++;
	pos = p + sep.length();
    }
    return i;
}

int split(const string& src, string results[], int n, const regex& r)
{
    string x = src;
    const char* s = x.chars();
    int sl = x.length();
    int i = 0;
    int pos = 0;
    int p, matchlen;
    while (i < n && pos < sl)
    {
	p = r.search(s, sl, matchlen, pos);
	if (p < 0)
	    p = sl;
	results[i].rep = string_Salloc(results[i].rep, &(s[pos]), p - pos, p - pos);
	i++;
	pos = p + matchlen;
    }
    return i;
}

string join(string src[], int n, const string& separator) RETURNS(x)
{
    RETURN_OBJECT(string, x);
    string sep = separator;
    int xlen = 0;
    for (int i = 0; i < n; ++i)
	xlen += src[i].length();
    xlen += (n - 1) * sep.length();

    x.alloc(xlen);

    int j = 0;
  
    for (i = 0; i < n - 1; ++i)
    {
	ncopy(src[i].chars(), &(x.rep->s[j]), src[i].length());
	j += src[i].length();
	ncopy(sep.chars(), &(x.rep->s[j]), sep.length());
	j += sep.length();
    }
    ncopy0(src[i].chars(), &(x.rep->s[j]), src[i].length());
    RETURN(x);
}

  
/*
 misc
*/

    
strRep* string_Sreverse(strRep* src, strRep* dest)
{
    int n = src->len;
    if (src != dest)
	dest = string_Salloc(dest, src->s, n, n);
    if (n > 0)
    {
	char* a = dest->s;
	char* b = &(a[n - 1]);
	while (a < b)
	{
	    char t = *a;
	    *a++ = *b;
	    *b-- = t;
	}
    }
    return dest;
}


strRep* string_Supcase(strRep* src, strRep* dest)
{
    int n = src->len;
    if (src != dest) dest = string_Salloc(dest, src->s, n, n);
    char* p = dest->s;
    char* e = &(p[n]);
    for (; p < e; ++p) if (islower(*p)) *p = toupper(*p);
    return dest;
}

strRep* string_Sdowncase(strRep* src, strRep* dest)
{
    int n = src->len;
    if (src != dest) dest = string_Salloc(dest, src->s, n, n);
    char* p = dest->s;
    char* e = &(p[n]);
    for (; p < e; ++p) if (isupper(*p)) *p = tolower(*p);
    return dest;
}

strRep* string_Scapitalize(strRep* src, strRep* dest)
{
    int n = src->len;
    if (src != dest) dest = string_Salloc(dest, src->s, n, n);

    char* p = dest->s;
    char* e = &(p[n]);
    for (; p < e; ++p)
    {
	int at_word;
	if (at_word = islower(*p))
	    *p = toupper(*p);
	else 
	    at_word = isupper(*p) || isdigit(*p);

	if (at_word)
	{
	    while (++p < e)
	    {
		if (isupper(*p))
		    *p = tolower(*p);
		else if (!islower(*p) && !isdigit(*p))
		    break;
	    }
	}
    }
    return dest;
}

#if defined(HAVE_NAMED_RETURN_VALUES)

#if 0				// already defined in -lg++
string replicate(char c, int n) return w;
{
    w.rep = Sresize(w.rep, n);
    char* p = w.rep->s;
    while (n-- > 0) *p++ = c;
    *p = 0;
}
#endif

string replicate(const string& y, int n) return w
{
    int len = y.length();
    w.rep = string_Sresize(w.rep, n * len);
    char* p = w.rep->s;
    while (n-- > 0)
    {
	ncopy(y.chars(), p, len);
	p += len;
    }
    *p = 0;
}

string common_prefix(const string& x, const string& y, int startpos) return r;
{
    const char* xchars = x.chars();
    const char* ychars = y.chars();
    const char* xs = &(xchars[startpos]);
    const char* ss = xs;
    const char* topx = &(xchars[x.length()]);
    const char* ys = &(ychars[startpos]);
    const char* topy = &(ychars[y.length()]);
    for (int l = 0; xs < topx && ys < topy && *xs++ == *ys++; ++l);
    r.rep = string_Salloc(r.rep, ss, l, l);
}

string common_suffix(const string& x, const string& y, int startpos) return r;
{
    const char* xchars = x.chars();
    const char* ychars = y.chars();
    const char* xs = &(xchars[x.length() + startpos]);
    const char* botx = xchars;
    const char* ys = &(ychars[y.length() + startpos]);
    const char* boty = ychars;
    for (int l = 0; xs >= botx && ys >= boty && *xs == *ys ; --xs, --ys, ++l);
    r.rep = string_Salloc(r.rep, ++xs, l, l);
}

#else // !defined(HAVE_NAMED_RETURN_VALUES)

#if 0
string replicate(char c, int n)
{
    string w;
    w.rep = Sresize(w.rep, n);
    char* p = w.rep->s;
    while (n-- > 0) *p++ = c;
    *p = 0;
    return w;
}
#endif

string replicate(const string& y, int n)
{
    string w;
    int len = y.length();
    w.rep = Sresize(w.rep, n * len);
    char* p = w.rep->s;
    while (n-- > 0)
    {
	ncopy(y.chars(), p, len);
	p += len;
    }
    *p = 0;
    return w;
}

string common_prefix(const string& x, const string& y, int startpos)
{
    string r;
    const char* xchars = x.chars();
    const char* ychars = y.chars();
    const char* xs = &(xchars[startpos]);
    const char* ss = xs;
    const char* topx = &(xchars[x.length()]);
    const char* ys = &(ychars[startpos]);
    const char* topy = &(ychars[y.length()]);
    for (int l = 0; xs < topx && ys < topy && *xs++ == *ys++; ++l);
    r.rep = Salloc(r.rep, ss, l, l);
    return r;
}

string common_suffix(const string& x, const string& y, int startpos) 
{
    string r;
    const char* xchars = x.chars();
    const char* ychars = y.chars();
    const char* xs = &(xchars[x.length() + startpos]);
    const char* botx = xchars;
    const char* ys = &(ychars[y.length() + startpos]);
    const char* boty = ychars;
    for (int l = 0; xs >= botx && ys >= boty && *xs == *ys ; --xs, --ys, ++l);
    r.rep = Salloc(r.rep, ++xs, l, l);
    return r;
}

#endif // !defined(HAVE_NAMED_RETURN_VALUES)

// IO

istream& operator>>(istream& s, string& x)
{
#ifdef _OLD_STREAMS
    if (!s.good())
    {
	return s;
    }
    s >> ws;
    if (!s.good())
    {
	return s;
    }
#else
    if (!s.ipfx(0) || (!(s.flags() & ios::skipws) && !ws(s)))
    {
	s.clear(ios::failbit|s.rdstate()); // Redundant if using GNU iostreams.
	return s;
    }
#endif
    int ch;
    unsigned i = 0;
    x.rep = string_Sresize(x.rep, 20);
    register streambuf *sb = s.rdbuf();
    while ((ch = sb->sbumpc()) != EOF)
    {
	if (isspace(ch))
	    break;
	if (i >= x.rep->sz - 1)
	    x.rep = string_Sresize(x.rep, i+1);
	x.rep->s[i++] = ch;
    }
    x.rep->s[i] = 0;
    x.rep->len = i;
    int new_state = s.rdstate();
    if (i == 0) new_state |= ios::failbit;
    if (ch == EOF) new_state |= ios::eofbit;
    s.clear(new_state);
    return s;
}

int readline(istream& s, string& x, char terminator, int discard)
{
#ifdef _OLD_STREAMS
    if (!s.good())
#else
	if (!s.ipfx(0))
#endif
	{
	    return 0;
	}
    int ch;
    unsigned i = 0;
    x.rep = string_Sresize(x.rep, 80);
    register streambuf *sb = s.rdbuf();
    while ((ch = sb->sbumpc()) != EOF)
    {
	if (ch != terminator || !discard)
	{
	    if (i >= x.rep->sz - 1)
		x.rep = string_Sresize(x.rep, i+1);
	    x.rep->s[i++] = ch;
	}
	if (ch == terminator)
	    break;
    }
    x.rep->s[i] = 0;
    x.rep->len = i;
    if (ch == EOF) s.set(ios::eofbit);
    return i;
}




// from John.Willis@FAS.RI.CMU.EDU

int string::freq(const subString& y) const
{
    int found = 0;
    for (unsigned int i = 0; i < length(); i++) 
	if (match(i,length(),0,y.chars(), y.length())>= 0) found++;
    return(found);
}

int string::freq(const string& y) const
{
    int found = 0;
    for (unsigned int i = 0; i < length(); i++) 
	if (match(i,length(),0,y.chars(),y.length()) >= 0) found++;
    return(found);
}

int string::freq(const char* t) const
{
    int found = 0;
    for (unsigned int i = 0; i < length(); i++) 
	if (match(i,length(),0,t) >= 0) found++;
    return(found);
}

int string::freq(char c) const
{
    int found = 0;
    for (unsigned int i = 0; i < length(); i++) 
	if (match(i,length(),0,&c,1) >= 0) found++;
    return(found);
}


int string::OK() const
{
    if (rep == 0		    // don't have a rep
	|| rep->len > rep->sz	    // string outside bounds
	|| rep->s[rep->len] != 0)   // not null-terminated
	error("invariant failure");
    return 1;
}

int subString::OK() const
{
    int v = S != 0;              // have a string;
    v &= S.OK();                 // that is legal
    v &= pos + len >= S.rep->len;// pos and len within bounds
    if (!v) S.error("subString invariant failure");
    return v;
}

