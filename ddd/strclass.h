// $Id$
// A customized GNU String class (from libg++-2.3)

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

// Differences to GNU String class:
// - class name is `string' instead of `String'
// - string length is `unsigned int' instead of `short'
// - extra constructor string::string(ostrstream&)
// - extra assignment string::operator=(ostrstream&)
// - output to stream issues *all* characters, including '\0'


#ifndef _ICE_strclass_h
#define _ICE_strclass_h

#ifdef __GNUG__
#pragma interface
#endif

#include <iostream.h>
#include <strstream.h>
#include "regex.h"
#include "constref.h"

struct strRep			// internal string representations
{
    unsigned len;		// string length 
    unsigned sz;		// allocated space
    char s[1];			// the string starts here 
				// (at least 1 char for trailing null)
				// allocated & expanded via non-public fcts
};

// primitive ops on strReps -- nearly all string fns go through these.

strRep* string_Salloc(strRep*, const char*, int, int);
strRep* string_Scopy(strRep*, strRep*);
strRep* string_Scat(strRep*, const char*, int, const char*, int);
strRep* string_Scat(strRep*, const char*, int,
		    const char*,int, const char*,int);
strRep* string_Sprepend(strRep*, const char*, int);
strRep* string_Sreverse(strRep*, strRep*);
strRep* string_Supcase(strRep*, strRep*);
strRep* string_Sdowncase(strRep*, strRep*);
strRep* string_Scapitalize(strRep*, strRep*);

// These classes need to be defined in the order given

class string;
class subString;

class subString
{
    friend class string;
protected:

    string&  S;			// The string I'm a subString of
    unsigned pos;		// starting position in S's rep
    unsigned len;		// length of subString

    void assign(strRep*, const char*, int = -1);
    subString(string& x, int p, int l);
    subString(const subString& x);

public:
// Note there are no public constructors. subStrings are always
// created via string operations

    ~subString();

    void operator =  (const string&     y);
    void operator =  (const subString&  y);
    void operator =  (const char* t);
    void operator =  (char        c);

// return 1 if target appears anywhere in subString; else 0

    int contains(char c) const;
    int contains(const string& y) const;
    int contains(const subString& y) const;
    int contains(const char* t) const;
    int contains(const regex& r) const;

// return 1 if target matches entire subString

    int matches(const regex& r) const;

// IO 

    friend inline ostream& operator<<(ostream& s, const subString& x);

// status

    unsigned int length() const;
    int empty() const;
    const char* chars() const;

    int OK() const; 
};


class string
{
    friend class subString;

protected:
    strRep* rep;   // Strings are pointers to their representations

// some helper functions

    int search(int, int, const char*, int = -1) const;
    int search(int, int, char) const;
    int match(int, int, int, const char*, int = -1) const;
    int _gsub(const char*, int, const char* ,int);
    int _gsub(const regex&, const char*, int);
    subString _substr(int, int);

public:

// constructors & assignment

    string();
    string(const string& x);
    string(const subString&  x);
    string(const char* t);
    string(const char* t, int len);
    string(char c);
    string(ostrstream& os); // should be const

    ~string();

    void operator =  (const string&     y);
    void operator =  (const char* y);
    void operator =  (char        c);
    void operator =  (const subString&  y);
    void operator =  (ostrstream& os);

// concatenation

    void operator += (const string&     y); 
    void operator += (const subString&  y);
    void operator += (const char* t);
    void operator += (char c);

    void prepend(const string&     y); 
    void prepend(const subString&  y);
    void prepend(const char* t);
    void prepend(char c);


// procedural versions:
// concatenate first 2 args, store result in last arg

    friend inline void cat(const string&, const string&, string&);
    friend inline void cat(const string&, const subString&, string&);
    friend inline void cat(const string&, const char*, string&);
    friend inline void cat(const string&, char, string&);

    friend inline void cat(const subString&, const string&, string&);
    friend inline void cat(const subString&, const subString&, string&);
    friend inline void cat(const subString&, const char*, string&);
    friend inline void cat(const subString&, char, string&);

    friend inline void cat(const char*, const string&, string&);
    friend inline void cat(const char*, const subString&, string&);
    friend inline void cat(const char*, const char*, string&);
    friend inline void cat(const char*, char, string&);

// double concatenation, by request. (yes, there are too many versions, 
// but if one is supported, then the others should be too...)
// Concatenate first 3 args, store in last arg

    friend inline void cat(const string&, const string&, 
			   const string&, string&);
    friend inline void cat(const string&, const string&,
			   const subString&, string&);
    friend inline void cat(const string&, const string&, 
			   const char*, string&);
    friend inline void cat(const string&, const string&, 
			   char, string&);
    friend inline void cat(const string&, const subString&,
			   const string&,string&);
    friend inline void cat(const string&, const subString&,
			   const subString&,string&);

    friend inline void cat(const string&, const subString&, 
			   const char*, string&);
    friend inline void cat(const string&, const subString&, 
			   char, string&);
    friend inline void cat(const string&, const char*, 
			   const string&, string&);
    friend inline void cat(const string&, const char*, 
			   const subString&, string&);
    friend inline void cat(const string&, const char*, 
			   const char*, string&);
    friend inline void cat(const string&, const char*,
			   char, string&);

    friend inline void cat(const char*, const string&,
			   const string&,string&);
    friend inline void cat(const char*, const string&,
			   const subString&,string&);
    friend inline void cat(const char*, const string&,
			   const char*, string&);
    friend inline void cat(const char*, const string&,
			   char, string&);
    friend inline void cat(const char*, const subString&,
			   const string&,string&);
    friend inline void cat(const char*, const subString&,
			   const subString&, string&);

    friend inline void cat(const char*, const subString&,
			   const char*, string&);
    friend inline void cat(const char*, const subString&,
			   char, string&);
    friend inline void cat(const char*, const char*,
			   const string&,    string&);
    friend inline void cat(const char*, const char*,
			   const subString&, string&);
    friend inline void cat(const char*, const char*,
			   const char*, string&);
    friend inline void cat(const char*, const char*,
			   char, string&);


// searching & matching

// return position of target in string or -1 for failure

    int index(char c, int startpos = 0) const;      
    int index(const string& y, int startpos = 0) const;      
    int index(const subString&  y, int startpos = 0) const;      
    int index(const char* t, int startpos = 0) const;  
    int index(const regex& r, int startpos = 0) const;       

// return 1 if target appears anyhere in string; else 0

    int contains(char c) const;
    int contains(const string& y) const;
    int contains(const subString& y) const;
    int contains(const char* t) const;
    int contains(const regex& r) const;

// return 1 if target appears anywhere after position pos 
// (or before, if pos is negative) in string; else 0

    int contains(char c, int pos) const;
    int contains(const string& y, int pos) const;
    int contains(const subString& y, int pos) const;
    int contains(const char* t, int pos) const;
    int contains(const regex& r, int pos) const;

// return 1 if target appears at position pos in string; else 0

    int matches(char c, int pos = 0) const;
    int matches(const string& y, int pos = 0) const;
    int matches(const subString& y, int pos = 0) const;
    int matches(const char* t, int pos = 0) const;
    int matches(const regex& r, int pos = 0) const;

//  return number of occurences of target in string

    int freq(char c) const; 
    int freq(const string& y) const;
    int freq(const subString& y) const;
    int freq(const char* t) const;

// subString extraction

// Note that you can't take a subString of a const string, since
// this leaves open the possiblility of indirectly modifying the
// string through the subString

    subString at(int pos, int len);
    subString operator () (int pos, int len); // synonym for at

    subString at(const string& x, int startpos = 0); 
    subString at(const subString&  x, int startpos = 0); 
    subString at(const char* t, int startpos = 0);
    subString at(char c, int startpos = 0);
    subString at(const regex& r, int startpos = 0); 

    subString before(int pos);
    subString before(const string& x, int startpos = 0);
    subString before(const subString&   x, int startpos = 0);
    subString before(const char* t, int startpos = 0);
    subString before(char c, int startpos = 0);
    subString before(const regex& r, int startpos = 0);

    subString through(int pos);
    subString through(const string& x, int startpos = 0);
    subString through(const subString& x, int startpos = 0);
    subString through(const char* t, int startpos = 0);
    subString through(char c, int startpos = 0);
    subString through(const regex& r, int startpos = 0);

    subString from(int pos);
    subString from(const string& x, int startpos = 0);
    subString from(const subString& x, int startpos = 0);
    subString from(const char* t, int startpos = 0);
    subString from(char c, int startpos = 0);
    subString from(const regex& r, int startpos = 0);

    subString after(int pos);
    subString after(const string& x, int startpos = 0);
    subString after(const subString& x, int startpos = 0);
    subString after(const char* t, int startpos = 0);
    subString after(char c, int startpos = 0);
    subString after(const regex& r, int startpos = 0);


// deletion

// delete len chars starting at pos
    void del(int pos, int len);

// delete the first occurrence of target after startpos

    void del(const string& y, int startpos = 0);
    void del(const subString& y, int startpos = 0);
    void del(const char* t, int startpos = 0);
    void del(char c, int startpos = 0);
    void del(const regex& r, int startpos = 0);

// global substitution: substitute all occurrences of pat with repl

    int gsub(const string& pat, const string& repl);
    int gsub(const subString& pat, const string& repl);
    int gsub(const char* pat, const string& repl);
    int gsub(const char* pat, const char* repl);
    int gsub(const regex& pat, const string& repl);

// friends & utilities

// split string into array res at separators; return number of elements

    friend int split(const string& x, string res[], int maxn, 
		     const string& sep);
    friend int split(const string& x, string res[], int maxn, 
		     const regex& sep);

    friend string common_prefix(const string& x, const string& y, 
				int startpos = 0);
    friend string common_suffix(const string& x, const string& y, 
				int startpos = -1);
    // friend string replicate(char c, int n);
    friend string replicate(const string& y, int n);
    friend string join(string src[], int n, const string& sep);

// simple builtin transformations

    friend inline string reverse(const string& x);
    friend inline string upcase(const string& x);
    friend inline string downcase(const string& x);
    friend inline string capitalize(const string& x);

// in-place versions of above

    void reverse();
    void upcase();
    void downcase();
    void capitalize();

// element extraction

    char& operator [] (int i);
    char operator [] (int i) const;
    char elem(int i) const;
    char firstchar() const;
    char lastchar() const;

// conversion

    operator const char*() const;
    operator char*() const;
    const char* chars() const;


// I/O

    friend inline ostream& operator<<(ostream& s, const string& x);
    friend inline ostream& operator<<(ostream& s, const subString& x);
    friend istream& operator>>(istream& s, string& x);

    friend int readline(istream& s, string& x, 
			char terminator = '\n',
			int discard_terminator = 1);

// status

    unsigned int length() const;
    int empty() const;

// preallocate some space for string
    void alloc(int newsize);

// report current allocation (not length!)

    int allocation() const;


    void error(const char* msg) const;

    int OK() const;
};

typedef string strTmp; // for backward compatibility

// other externs

int compare(const string& x, const string& y);
int compare(const string& x, const subString& y);
int compare(const string& x, const char* y);
int compare(const subString& x, const string& y);
int compare(const subString& x, const subString& y);
int compare(const subString& x, const char* y);
int fcompare(const string& x, const string& y); // ignore case

extern strRep  _nilstrRep;
extern string _nilstring;

// status reports, needed before defining other things

inline unsigned int string::length() const {  return rep->len; }
inline int          string::empty() const { return rep->len == 0; }
inline const char*  string::chars() const { return &(rep->s[0]); }
inline int          string::allocation() const { return rep->sz; }

inline unsigned int subString::length() const { return len; }
inline int          subString::empty() const { return len == 0; }
inline const char*  subString::chars() const { return &(S.rep->s[pos]); }

// constructors

inline string::string() 
  : rep(&_nilstrRep) {}
inline string::string(const string& x) 
  : rep(string_Scopy(0, x.rep)) {}
inline string::string(const char* t) 
  : rep(string_Salloc(0, t, -1, -1)) {}
inline string::string(const char* t, int tlen)
  : rep(string_Salloc(0, t, tlen, tlen)) {}
inline string::string(const subString& y)
  : rep(string_Salloc(0, y.chars(), y.length(), y.length())) {}
inline string::string(char c) 
  : rep(string_Salloc(0, &c, 1, 1)) {}

inline string::~string() { if (rep != &_nilstrRep) delete[] (char *)rep; }

inline subString::subString(const subString& x)
  :S(x.S), pos(x.pos), len(x.len) {}
inline subString::subString(string& x, int first, int l)
  :S(x), pos(first), len(l) {}

inline subString::~subString() {}

// assignment

inline void string::operator =  (const string& y)
{ 
    rep = string_Scopy(rep, y.rep);
}

inline void string::operator=(const char* t)
{
    rep = string_Salloc(rep, t, -1, -1); 
}

inline void string::operator=(const subString&  y)
{
    rep = string_Salloc(rep, y.chars(), y.length(), y.length());
}

inline void string::operator=(char c)
{
    rep = string_Salloc(rep, &c, 1, 1); 
}

inline void string::operator=(ostrstream& os)
{
#ifdef HAVE_FROZEN_OSTRSTREAM
    // No need to freeze the stream, since the string is copied right away
    int frozen = os.frozen();
#endif

    const char *str = os.str();
    rep = string_Salloc(rep, str, os.pcount(), os.pcount());

#ifdef HAVE_FROZEN_OSTRSTREAM
    os.freeze(frozen);
#endif
}

inline string::string(ostrstream& os)
    : rep(&_nilstrRep) 
{
    operator=(os);
}

inline void subString::operator = (const char* ys)
{
    assign(0, ys);
}

inline void subString::operator = (char ch)
{
    assign(0, &ch, 1);
}

inline void subString::operator = (const string& y)
{
    assign(y.rep, y.chars(), y.length());
}

inline void subString::operator = (const subString& y)
{
    assign(y.S.rep, y.chars(), y.length());
}

// Zillions of cats...

inline void cat(const string& x, const string& y, string& r)
{
    r.rep = string_Scat(r.rep, x.chars(), x.length(), y.chars(), y.length());
}

inline void cat(const string& x, const subString& y, string& r)
{
    r.rep = string_Scat(r.rep, x.chars(), x.length(), y.chars(), y.length());
}

inline void cat(const string& x, const char* y, string& r)
{
    r.rep = string_Scat(r.rep, x.chars(), x.length(), y, -1);
}

inline void cat(const string& x, char y, string& r)
{
    r.rep = string_Scat(r.rep, x.chars(), x.length(), &y, 1);
}

inline void cat(const subString& x, const string& y, string& r)
{
    r.rep = string_Scat(r.rep, x.chars(), x.length(), y.chars(), y.length());
}

inline void cat(const subString& x, const subString& y, string& r)
{
    r.rep = string_Scat(r.rep, x.chars(), x.length(), y.chars(), y.length());
}

inline void cat(const subString& x, const char* y, string& r)
{
    r.rep = string_Scat(r.rep, x.chars(), x.length(), y, -1);
}

inline void cat(const subString& x, char y, string& r)
{
    r.rep = string_Scat(r.rep, x.chars(), x.length(), &y, 1);
}

inline void cat(const char* x, const string& y, string& r)
{
    r.rep = string_Scat(r.rep, x, -1, y.chars(), y.length());
}

inline void cat(const char* x, const subString& y, string& r)
{
    r.rep = string_Scat(r.rep, x, -1, y.chars(), y.length());
}

inline void cat(const char* x, const char* y, string& r)
{
    r.rep = string_Scat(r.rep, x, -1, y, -1);
}

inline void cat(const char* x, char y, string& r)
{
    r.rep = string_Scat(r.rep, x, -1, &y, 1);
}

inline void cat(const string& a, const string& x, const string& y, string& r)
{
    r.rep = string_Scat(r.rep, a.chars(), a.length(), x.chars(), 
			x.length(), y.chars(), y.length());
}

inline void cat(const string& a, const string& x, const subString& y, string& r)
{
    r.rep = string_Scat(r.rep, a.chars(), a.length(), x.chars(), 
			x.length(), y.chars(), y.length());
}

inline void cat(const string& a, const string& x, const char* y, string& r)
{
    r.rep = string_Scat(r.rep, a.chars(), a.length(), x.chars(), 
			x.length(), y, -1);
}

inline void cat(const string& a, const string& x, char y, string& r)
{
    r.rep = string_Scat(r.rep, a.chars(), a.length(), x.chars(), 
			x.length(), &y, 1);
}

inline void cat(const string& a, const subString& x, const string& y, string& r)
{
    r.rep = string_Scat(r.rep, a.chars(), a.length(), x.chars(), 
			x.length(), y.chars(), y.length());
}

inline void cat(const string& a, const subString& x, const subString& y, string& r)
{
    r.rep = string_Scat(r.rep, a.chars(), a.length(), x.chars(), 
			x.length(), y.chars(), y.length());
}

inline void cat(const string& a, const subString& x, const char* y, string& r)
{
    r.rep = string_Scat(r.rep, a.chars(), a.length(), x.chars(), 
			x.length(), y, -1);
}

inline void cat(const string& a, const subString& x, char y, string& r)
{
    r.rep = string_Scat(r.rep, a.chars(), a.length(), x.chars(), 
			x.length(), &y, 1);
}

inline void cat(const string& a, const char* x, const string& y, string& r)
{
    r.rep = string_Scat(r.rep, a.chars(), a.length(), x, 
			-1, y.chars(), y.length());
}

inline void cat(const string& a, const char* x, const subString& y, string& r)
{
    r.rep = string_Scat(r.rep, a.chars(), a.length(), x, 
			-1, y.chars(), y.length());
}

inline void cat(const string& a, const char* x, const char* y, string& r)
{
    r.rep = string_Scat(r.rep, a.chars(), a.length(), x, -1, y, -1);
}

inline void cat(const string& a, const char* x, char y, string& r)
{
    r.rep = string_Scat(r.rep, a.chars(), a.length(), x, -1, &y, 1);
}


inline void cat(const char* a, const string& x, const string& y, string& r)
{
    r.rep = string_Scat(r.rep, a, -1, x.chars(), x.length(), 
			y.chars(), y.length());
}

inline void cat(const char* a, const string& x, const subString& y, string& r)
{
    r.rep = string_Scat(r.rep, a, -1, x.chars(), x.length(), 
			y.chars(), y.length());
}

inline void cat(const char* a, const string& x, const char* y, string& r)
{
    r.rep = string_Scat(r.rep, a, -1, x.chars(), x.length(), y, -1);
}

inline void cat(const char* a, const string& x, char y, string& r)
{
    r.rep = string_Scat(r.rep, a, -1, x.chars(), x.length(), &y, 1);
}

inline void cat(const char* a, const subString& x, const string& y, string& r)
{
    r.rep = string_Scat(r.rep, a, -1, x.chars(), x.length(), 
			y.chars(), y.length());
}

inline void cat(const char* a, const subString& x, 
		const subString& y, string& r)
{
    r.rep = string_Scat(r.rep, a, -1, x.chars(), x.length(), 
			y.chars(), y.length());
}

inline void cat(const char* a, const subString& x, const char* y, string& r)
{
    r.rep = string_Scat(r.rep, a, -1, x.chars(), x.length(), y, -1);
}

inline void cat(const char* a, const subString& x, char y, string& r)
{
    r.rep = string_Scat(r.rep, a, -1, x.chars(), x.length(), &y, 1);
}

inline void cat(const char* a, const char* x, const string& y, string& r)
{
    r.rep = string_Scat(r.rep, a, -1, x, -1, y.chars(), y.length());
}

inline void cat(const char* a, const char* x, const subString& y, string& r)
{
    r.rep = string_Scat(r.rep, a, -1, x, -1, y.chars(), y.length());
}

inline void cat(const char* a, const char* x, const char* y, string& r)
{
    r.rep = string_Scat(r.rep, a, -1, x, -1, y, -1);
}

inline void cat(const char* a, const char* x, char y, string& r)
{
    r.rep = string_Scat(r.rep, a, -1, x, -1, &y, 1);
}


// operator versions

inline void string::operator +=(const string& y)
{
    cat(*this, y, *this);
}

inline void string::operator +=(const subString& y)
{
    cat(*this, y, *this);
}

inline void string::operator += (const char* y)
{
    cat(*this, y, *this);
}

inline void string:: operator +=(char y)
{
    cat(*this, y, *this);
}

// constructive concatenation

#ifdef HAVE_NAMED_RETURN_VALUES

inline string operator + (const string& x, const string& y) return r;
{
    cat(x, y, r);
}

inline string operator + (const string& x, const subString& y) return r;
{
    cat(x, y, r);
}

inline string operator + (const string& x, const char* y) return r;
{
    cat(x, y, r);
}

inline string operator + (const string& x, char y) return r;
{
    cat(x, y, r);
}

inline string operator + (const subString& x, const string& y) return r;
{
    cat(x, y, r);
}

inline string operator + (const subString& x, const subString& y) return r;
{
    cat(x, y, r);
}

inline string operator + (const subString& x, const char* y) return r;
{
    cat(x, y, r);
}

inline string operator + (const subString& x, char y) return r;
{
    cat(x, y, r);
}

inline string operator + (const char* x, const string& y) return r;
{
    cat(x, y, r);
}

inline string operator + (const char* x, const subString& y) return r;
{
    cat(x, y, r);
}

inline string reverse(const string& x) return r;
{
    r.rep = string_Sreverse(x.rep, r.rep);
}

inline string upcase(const string& x) return r;
{
    r.rep = string_Supcase(x.rep, r.rep);
}

inline string downcase(const string& x) return r;
{
    r.rep = string_Sdowncase(x.rep, r.rep);
}

inline string capitalize(const string& x) return r;
{
    r.rep = string_Scapitalize(x.rep, r.rep);
}

#else /* ! defined(NAMED_RETURN_VALUES) */

inline string operator + (const string& x, const string& y)
{
    string r;  cat(x, y, r);  return r;
}

inline string operator + (const string& x, const subString& y) 
{
    string r; cat(x, y, r); return r;
}

inline string operator + (const string& x, const char* y) 
{
    string r; cat(x, y, r); return r;
}

inline string operator + (const string& x, char y) 
{
    string r; cat(x, y, r); return r;
}

inline string operator + (const subString& x, const string& y) 
{
    string r; cat(x, y, r); return r;
}

inline string operator + (const subString& x, const subString& y) 
{
    string r; cat(x, y, r); return r;
}

inline string operator + (const subString& x, const char* y) 
{
    string r; cat(x, y, r); return r;
}

inline string operator + (const subString& x, char y) 
{
    string r; cat(x, y, r); return r;
}

inline string operator + (const char* x, const string& y) 
{
    string r; cat(x, y, r); return r;
}

inline string operator + (const char* x, const subString& y) 
{
    string r; cat(x, y, r); return r;
}

inline string reverse(const string& x) 
{
    string r; r.rep = string_Sreverse(x.rep, r.rep); return r;
}

inline string upcase(const string& x) 
{
    string r; r.rep = string_Supcase(x.rep, r.rep); return r;
}

inline string downcase(const string& x) 
{
    string r; r.rep = string_Sdowncase(x.rep, r.rep); return r;
}

inline string capitalize(const string& x) 
{
    string r; r.rep = string_Scapitalize(x.rep, r.rep); return r;
}

#endif

// prepend

inline void string::prepend(const string& y)
{
    rep = string_Sprepend(rep, y.chars(), y.length());
}

inline void string::prepend(const char* y)
{
    rep = string_Sprepend(rep, y, -1); 
}

inline void string::prepend(char y)
{
    rep = string_Sprepend(rep, &y, 1); 
}

inline void string::prepend(const subString& y)
{
    rep = string_Sprepend(rep, y.chars(), y.length());
}

// misc transformations


inline void string::reverse()
{
    rep = string_Sreverse(rep, rep);
}


inline void string::upcase()
{
    rep = string_Supcase(rep, rep);
}


inline void string::downcase()
{
    rep = string_Sdowncase(rep, rep);
}


inline void string::capitalize()
{
    rep = string_Scapitalize(rep, rep);
}

// element extraction

inline char&  string::operator [] (int i) 
{ 
    if (((unsigned)i) >= length()) error("invalid index");
    return rep->s[i];
}

inline char string::operator [] (int i) const
{ 
    if (((unsigned)i) >= length()) error("invalid index");
    return rep->s[i];
}

inline char  string::elem (int i) const
{ 
    if (((unsigned)i) >= length()) error("invalid index");
    return rep->s[i];
}

inline char  string::firstchar() const
{ 
    return elem(0);
}

inline char  string::lastchar() const
{ 
    return elem(length() - 1);
}

// searching

inline int string::index(char c, int startpos) const
{
    return search(startpos, length(), c);
}

inline int string::index(const char* t, int startpos) const
{   
    return search(startpos, length(), t);
}

inline int string::index(const string& y, int startpos) const
{   
    return search(startpos, length(), y.chars(), y.length());
}

inline int string::index(const subString& y, int startpos) const
{   
    return search(startpos, length(), y.chars(), y.length());
}

inline int string::index(const regex& r, int startpos) const
{
    int unused;  return r.search(chars(), length(), unused, startpos);
}

inline int string::contains(char c) const
{
    return search(0, length(), c) >= 0;
}

inline int string::contains(const char* t) const
{   
    return search(0, length(), t) >= 0;
}

inline int string::contains(const string& y) const
{   
    return search(0, length(), y.chars(), y.length()) >= 0;
}

inline int string::contains(const subString& y) const
{   
    return search(0, length(), y.chars(), y.length()) >= 0;
}

inline int string::contains(char c, int p) const
{
    return match(p, length(), 0, &c, 1) >= 0;
}

inline int string::contains(const char* t, int p) const
{
    return match(p, length(), 0, t) >= 0;
}

inline int string::contains(const string& y, int p) const
{
    return match(p, length(), 0, y.chars(), y.length()) >= 0;
}

inline int string::contains(const subString& y, int p) const
{
    return match(p, length(), 0, y.chars(), y.length()) >= 0;
}

inline int string::contains(const regex& r) const
{
    int unused;  return r.search(chars(), length(), unused, 0) >= 0;
}

inline int string::contains(const regex& r, int p) const
{
    return r.match(chars(), length(), p) >= 0;
}


inline int string::matches(const subString& y, int p) const
{
    return match(p, length(), 1, y.chars(), y.length()) >= 0;
}

inline int string::matches(const string& y, int p) const
{
    return match(p, length(), 1, y.chars(), y.length()) >= 0;
}

inline int string::matches(const char* t, int p) const
{
    return match(p, length(), 1, t) >= 0;
}

inline int string::matches(char c, int p) const
{
    return match(p, length(), 1, &c, 1) >= 0;
}

inline int string::matches(const regex& r, int p) const
{
    int l = (p < 0)? -p : length() - p;
    return r.match(chars(), length(), p) == l;
}


inline int subString::contains(const char* t) const
{   
    return S.search(pos, pos+len, t) >= 0;
}

inline int subString::contains(const string& y) const
{   
    return S.search(pos, pos+len, y.chars(), y.length()) >= 0;
}

inline int subString::contains(const subString&  y) const
{   
    return S.search(pos, pos+len, y.chars(), y.length()) >= 0;
}

inline int subString::contains(char c) const
{
    return S.search(pos, pos+len, 0, c) >= 0;
}

inline int subString::contains(const regex& r) const
{
    int unused;  return r.search(chars(), len, unused, 0) >= 0;
}

inline int subString::matches(const regex& r) const
{
    return r.match(chars(), len, 0) == len;
}


inline int string::gsub(const string& pat, const string& r)
{
    return _gsub(pat.chars(), pat.length(), r.chars(), r.length());
}

inline int string::gsub(const subString&  pat, const string& r)
{
    return _gsub(pat.chars(), pat.length(), r.chars(), r.length());
}

inline int string::gsub(const regex& pat, const string& r)
{
    return _gsub(pat, r.chars(), r.length());
}

inline int string::gsub(const char* pat, const string& r)
{
    return _gsub(pat, -1, r.chars(), r.length());
}

inline int string::gsub(const char* pat, const char* r)
{
    return _gsub(pat, -1, r, -1);
}

// I/O

inline ostream& operator<<(ostream& s, const string& x)
{
    s.write(x.chars(), x.length()); return s;
}

inline ostream& operator<<(ostream& s, const subString& x)
{ 
    s.write(x.chars(), x.length()); return s;
}

// a zillion comparison operators

inline int operator==(const string& x, const string& y) 
{
    return compare(x, y) == 0; 
}

inline int operator!=(const string& x, const string& y)
{
    return compare(x, y) != 0; 
}

inline int operator>(const string& x, const string& y)
{
    return compare(x, y) > 0; 
}

inline int operator>=(const string& x, const string& y)
{
    return compare(x, y) >= 0; 
}

inline int operator<(const string& x, const string& y)
{
    return compare(x, y) < 0; 
}

inline int operator<=(const string& x, const string& y)
{
    return compare(x, y) <= 0; 
}

inline int operator==(const string& x, const subString&  y) 
{
    return compare(x, y) == 0; 
}

inline int operator!=(const string& x, const subString&  y)
{
    return compare(x, y) != 0; 
}

inline int operator>(const string& x, const subString&  y)      
{
    return compare(x, y) > 0; 
}

inline int operator>=(const string& x, const subString&  y)
{
    return compare(x, y) >= 0; 
}

inline int operator<(const string& x, const subString&  y) 
{
    return compare(x, y) < 0; 
}

inline int operator<=(const string& x, const subString&  y)
{
    return compare(x, y) <= 0; 
}

inline int operator==(const string& x, const char* t) 
{
    return compare(x, t) == 0; 
}

inline int operator!=(const string& x, const char* t) 
{
    return compare(x, t) != 0; 
}

inline int operator>(const string& x, const char* t)  
{
    return compare(x, t) > 0; 
}

inline int operator>=(const string& x, const char* t) 
{
    return compare(x, t) >= 0; 
}

inline int operator<(const string& x, const char* t)  
{
    return compare(x, t) < 0; 
}

inline int operator<=(const string& x, const char* t) 
{
    return compare(x, t) <= 0; 
}

inline int operator==(const subString& x, const string& y) 
{
    return compare(y, x) == 0; 
}

inline int operator!=(const subString& x, const string& y)
{
    return compare(y, x) != 0;
}

inline int operator>(const subString& x, const string& y)      
{
    return compare(y, x) < 0;
}

inline int operator>=(const subString& x, const string& y)     
{
    return compare(y, x) <= 0;
}

inline int operator<(const subString& x, const string& y)      
{
    return compare(y, x) > 0;
}

inline int operator<=(const subString& x, const string& y)     
{
    return compare(y, x) >= 0;
}

inline int operator==(const subString& x, const subString&  y) 
{
    return compare(x, y) == 0; 
}

inline int operator!=(const subString& x, const subString&  y)
{
    return compare(x, y) != 0;
}

inline int operator>(const subString& x, const subString&  y)      
{
    return compare(x, y) > 0;
}

inline int operator>=(const subString& x, const subString&  y)
{
    return compare(x, y) >= 0;
}

inline int operator<(const subString& x, const subString&  y) 
{
    return compare(x, y) < 0;
}

inline int operator<=(const subString& x, const subString&  y)
{
    return compare(x, y) <= 0;
}

inline int operator==(const subString& x, const char* t) 
{
    return compare(x, t) == 0; 
}

inline int operator!=(const subString& x, const char* t) 
{
    return compare(x, t) != 0;
}

inline int operator>(const subString& x, const char* t)  
{
    return compare(x, t) > 0; 
}

inline int operator>=(const subString& x, const char* t) 
{
    return compare(x, t) >= 0; 
}

inline int operator<(const subString& x, const char* t)  
{
    return compare(x, t) < 0; 
}

inline int operator<=(const subString& x, const char* t) 
{
    return compare(x, t) <= 0; 
}


// a helper needed by at, before, etc.

inline subString string::_substr(int first, int l)
{
    if (first < 0 || (unsigned)(first + l) > length() )
	return subString(_nilstring, 0, 0) ;
    else 
	return subString(*this, first, l);
}

#endif // _ICE_strclass_h
// DON'T ADD ANYTHING BEHIND THIS #endif
