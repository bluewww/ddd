// $Id$
// A string class (based on `String' from GNU libg++-2.3)

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
Foundation, 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

// Differences to GNU String class:
// - class name is `string' instead of `String'
// - string length is `unsigned int' instead of `short'
// - extra constructor string::string(ostrstream&)
// - extra assignment string::operator=(ostrstream&)
// - output to stream issues *all* characters, including '\0'
// - extra functions for `char *' provided (besides `const char *')
// - `cat' with 4 arguments is not supported


// Here's the libg++ documentation of Doug Lea, adapted for this class:

// The string class
// ****************
// 
//    The `string' class is designed to extend GNU C++ to support string
// processing capabilities similar to those in languages like Awk.  The
// class provides facilities that ought to be convenient and efficient
// enough to be useful replacements for `char*' based processing via the C
// string library (i.e., `strcpy, strcmp,' etc.) in many applications.
// Many details about string representations are described in the
// Representation section.
// 
//    A separate `subString' class supports substring extraction and
// modification operations. This is implemented in a way that user
// programs never directly construct or represent subStrings, which are
// only used indirectly via string operations.
// 
//    Another separate class, `regex' is also used indirectly via string
// operations in support of regular expression searching, matching, and the
// like.  The regex class is based entirely on the GNU Emacs regex
// functions.  *See Syntax of Regular Expressions: (emacs.info)regexps,
// for a full explanation of regular expression syntax.  (For
// implementation details, see the internal documentation in files
// `rxclass.h' and `rxclass.C'.)
// 
// Constructors
// ============
// 
//    Strings are initialized and assigned as in the following examples:
// 
// `string x;  string y = 0; string z = "";'
//      Set x, y, and z to the nil string. Note that either 0 or "" may
//      always be used to refer to the nil string.
// 
// `string x = "Hello"; string y("Hello");'
//      Set x and y to a copy of the string "Hello".
// 
// `string x = 'A'; string y('A');'
//      Set x and y to the string value "A"
// 
// `string u = x; string v(x);'
//      Set u and v to the same string as string x
// 
// `string u = x.at(1,4); string v(x.at(1,4));'
//      Set u and v to the length 4 substring of x starting at position 1
//      (counting indexes from 0).
// 
// `string x("abc", 2);'
//      Sets x to "ab", i.e., the first 2 characters of "abc".
// 
// `string x = dec(20);'
//      Sets x to "20". As here, strings may be initialized or assigned
//      the results of any `char*' function.
// 
//    There are no directly accessible forms for declaring subString
// variables.
// 
//    The declaration `regex r("[a-zA-Z_][a-zA-Z0-9_]*");' creates a
// compiled regular expression suitable for use in string operations
// described below. (In this case, one that matches any C++ identifier).
// The first argument may also be a string.  Be careful in distinguishing
// the role of backslashes in quoted GNU C++ char* constants versus those
// in regexes. For example, a regex that matches either one or more tabs
// or all strings beginning with "ba" and ending with any number of
// occurrences of "na" could be declared as `regex r =
// "\\(\t+\\)\\|\\(ba\\(na\\)*\\)"' Note that only one backslash is needed
// to signify the tab, but two are needed for the parenthesization and
// virgule, since the GNU C++ lexical analyzer decodes and strips
// backslashes before they are seen by regex.
//
//    As a convenience, several regexes are predefined and usable in any
// program. Here are their declarations from `rxclass.h'.
// 
//      extern regex rxwhite;      // = "[ \n\t]+"
//      extern regex rxint;        // = "-?[0-9]+"
//      extern regex rxdouble;     // = "-?\\(\\([0-9]+\\.[0-9]*\\)\\|
//                                 //    \\([0-9]+\\)\\|
//                                 //    \\(\\.[0-9]+\\)\\)
//                                 //    \\([eE][---+]?[0-9]+\\)?"
//      extern regex rxalpha;      // = "[A-Za-z]+"
//      extern regex rxlowercase;  // = "[a-z]+"
//      extern regex rxuppercase;  // = "[A-Z]+"
//      extern regex rxalphanum;   // = "[0-9A-Za-z]+"
//      extern regex rxidentifier; // = "[A-Za-z_][A-Za-z0-9_]*"
// 
// Examples
// ========
// 
//    Most `string' class capabilities are best shown via example.  The
// examples below use the following declarations.
// 
//          string x = "Hello";
//          string y = "world";
//          string n = "123";
//          string z;
//          char*  s = ",";
//          string lft, mid, rgt;
//          regex  r = "e[a-z]*o";
//          regex  r2("/[a-z]*/");
//          char   c;
//          int    i, pos, len;
//          double f;
//          string words[10];
//          words[0] = "a";
//          words[1] = "b";
//          words[2] = "c";
// 
// Comparing, Searching and Matching
// =================================
// 
//    The usual lexicographic relational operators (`==, !=, <, <=, >, >=')
// are defined. A functional form `compare(string, string)' is also
// provided, as is `fcompare(string, string)', which compares strings
// without regard for upper vs. lower case.
// 
//    All other matching and searching operations are based on some form
// of the (non-public) `match' and `search' functions.  `match' and
// `search' differ in that `match' attempts to match only at the given
// starting position, while `search' starts at the position, and then
// proceeds left or right looking for a match.  As seen in the following
// examples, the second optional `startpos' argument to functions using
// `match' and `search' specifies the starting position of the search: If
// non-negative, it results in a left-to-right search starting at position
// `startpos', and if negative, a right-to-left search starting at
// position `x.length() + startpos'. In all cases, the index returned is
// that of the beginning of the match, or -1 if there is no match.
// 
//    Three string functions serve as front ends to `search' and `match'.
// `index' performs a search, returning the index, `matches' performs a
// match, returning nonzero (actually, the length of the match) on success,
// and `contains' is a boolean function performing either a search or
// match, depending on whether an index argument is provided:
// 
// `x.index("lo")'
//      returns the zero-based index of the leftmost occurrence of
//      substring "lo" (3, in this case).  The argument may be a string,
//      subString, char, char*, or regex.
// 
// `x.index("l", 2)'
//      returns the index of the first of the leftmost occurrence of "l"
//      found starting the search at position x[2], or 2 in this case.
// 
// `x.index("l", -1)'
//      returns the index of the rightmost occurrence of "l", or 3 here.
// 
// `x.index("l", -3)'
//      returns the index of the rightmost occurrence of "l" found by
//      starting the search at the 3rd to the last position of x,
//      returning 2 in this case.
// 
// `pos = r.search("leo", 3, len, 0)'
//      returns the index of r in the `char*' string of length 3, starting
//      at position 0, also placing the  length of the match in reference
//      parameter len.
// 
// `x.contains("He")'
//      returns nonzero if the string x contains the substring "He". The
//      argument may be a string, subString, char, char*, or regex.
// 
// `x.contains("el", 1)'
//      returns nonzero if x contains the substring "el" at position 1.
//      As in this example, the second argument to `contains', if present,
//      means to match the substring only at that position, and not to
//      search elsewhere in the string.
// 
// `x.contains(rxwhite);'
//      returns nonzero if x contains any whitespace (space, tab, or
//      newline). Recall that `rxwhite' is a global whitespace regex.
// 
// `x.matches("lo", 3)'
//      returns nonzero if x starting at position 3 exactly matches "lo",
//      with no trailing characters (as it does in this example).
// 
// `x.matches(r)'
//      returns nonzero if string x as a whole matches regex r.
// 
// `int f = x.freq("l")'
//      returns the number of distinct, nonoverlapping matches to the
//      argument (2 in this case).
// 
// Substring extraction
// ====================
// 
//    Substrings may be extracted via the `at', `before', `through',
// `from', and `after' functions.  These behave as either lvalues or
// rvalues.
// 
// `z = x.at(2, 3)'
//      sets string z to be equal to the length 3 substring of string x
//      starting at zero-based position 2, setting z to "llo" in this
//      case. A nil string is returned if the arguments don't make sense.
// 
// `x.at(2, 2) = "r"'
//      Sets what was in positions 2 to 3 of x to "r", setting x to "Hero"
//      in this case. As indicated here, substring assignments may be of
//      different lengths.
// 
// `x.at("He") = "je";'
//      x("He") is the substring of x that matches the first occurrence of
//      it's argument. The substitution sets x to "jello". If "He" did not
//      occur, the substring would be nil, and the assignment would have
//      no effect.
// 
// `x.at("l", -1) = "i";'
//      replaces the rightmost occurrence of "l" with "i", setting x to
//      "Helio".
// 
// `z = x.at(r)'
//      sets string z to the first match in x of regex r, or "ello" in this
//      case. A nil string is returned if there is no match.
// 
// `z = x.before("o")'
//      sets z to the part of x to the left of the first occurrence of
//      "o", or "Hell" in this case. The argument may also be a string,
//      subString, or regex.  (If there is no match, z is set to "".)
// 
// `x.before("ll") = "Bri";'
//      sets the part of x to the left of "ll" to "Bri", setting x to
//      "Brillo".
// 
// `z = x.before(2)'
//      sets z to the part of x to the left of x[2], or "He" in this case.
// 
// `z = x.after("Hel")'
//      sets z to the part of x to the right of "Hel", or "lo" in this
//      case.
// 
// `z = x.through("el")'
//      sets z to the part of x up and including "el", or "Hel" in this
//      case.
// 
// `z = x.from("el")'
//      sets z to the part of x from "el" to the end, or "ello" in this
//      case.
// 
// `x.after("Hel") = "p";'
//      sets x to "Help";
// 
// `z = x.after(3)'
//      sets z to the part of x to the right of x[3] or "o" in this case.
// 
// `z = "  ab c"; z = z.after(rxwhite)'
//      sets z to the part of its old string to the right of the first
//      group of whitespace, setting z to "ab c"; Use gsub(below) to strip
//      out multiple occurrences of whitespace or any pattern.
// 
// `x[0] = 'J';'
//      sets the first element of x to 'J'. x[i] returns a reference to
//      the ith element of x, or triggers an error if i is out of range.
// 
// `common_prefix(x, "Help")'
//      returns the string containing the common prefix of the two strings
//      or "Hel" in this case.
// 
// `common_suffix(x, "to")'
//      returns the string containing the common suffix of the two strings
//      or "o" in this case.
// 
// Concatenation
// =============
// 
// `z = x + s + ' ' + y.at("w") + y.after("w") + ".";'
//      sets z to "Hello, world."
// 
// `x += y;'
//      sets x to "Helloworld"
// 
// `cat(x, y, z)'
//      A faster way to say z = x + y.
// 
// `y.prepend(x);'
//      A faster way to say y = x + y.
// 
// `z = replicate(x, 3);'
//      sets z to "HelloHelloHello".
// 
// `z = join(words, 3, "/")'
//      sets z to the concatenation of the first 3 strings in string array
//      words, each separated by "/", setting z to "a/b/c" in this case.
//      The last argument may be "" or 0, indicating no separation.
// 
// Other manipulations
// ===================
// 
// `z = "this string has five words"; i = split(z, words, 10, rxwhite);'
//      sets up to 10 elements of string array words to the parts of z
//      separated by whitespace, and returns the number of parts actually
//      encountered (5 in this case). Here, words[0] = "this", words[1] =
//      "string", etc.  The last argument may be any of the usual.  If
//      there is no match, all of z ends up in words[0]. The words array
//      is *not* dynamically created by split.
// 
// `int nmatches x.gsub("l","ll")'
//      substitutes all original occurrences of "l" with "ll", setting x
//      to "Hellllo". The first argument may be any of the usual,
//      including regex.  If the second argument is "" or 0, all
//      occurrences are deleted. gsub returns the number of matches that
//      were replaced.
// 
// `z = x + y;  z.del("loworl");'
//      deletes the leftmost occurrence of "loworl" in z, setting z to
//      "Held".
// 
// `z = reverse(x)'
//      sets z to the reverse of x, or "olleH".
// 
// `z = upcase(x)'
//      sets z to x, with all letters set to uppercase, setting z to
//      "HELLO"
// 
// `z = downcase(x)'
//      sets z to x, with all letters set to lowercase, setting z to
//      "hello"
// 
// `z = capitalize(x)'
//      sets z to x, with the first letter of each word set to uppercase,
//      and all others to lowercase, setting z to "Hello"
// 
// `x.reverse(), x.upcase(), x.downcase(), x.capitalize()'
//      in-place, self-modifying versions of the above.
// 
// Reading, Writing and Conversion
// ===============================
// 
// `cout << x'
//      writes out x.
// 
// `cout << x.at(2, 3)'
//      writes out the substring "llo".
// 
// `cin >> x'
//      reads a whitespace-bounded string into x.
// 
// `x.length()'
//      returns the length of string x (5, in this case).
// 
// `s = (const char*)x'
// `s = x.chars()'
//      can be used to extract the `char*' char array. This coercion is
//      useful for sending a string as an argument to any function
//      expecting a `const char*' argument (like `atoi', and
//      `File::open'). This operator must be used with care, since the
//      conversion returns a pointer to `string' internals without copying
//      the characters: The resulting `(char *)' is only valid until the
//      next string operation,  and you must not modify it.  (The
//      conversion is defined to return a const value so that GNU C++ will
//      produce warning and/or error messages if changes are attempted.)


#ifndef _ICE_strclass_h
#define _ICE_strclass_h

#ifdef __GNUG__
#pragma interface
#endif

#include <iostream.h>
#include <strstream.h>
#include "rxclass.h"
#include "config.h"
#include "bool.h"

// Internal string representations
struct strRep
{
    unsigned len;		// String length 
    unsigned sz;		// Allocated space
    char s[1];			// The string starts here 
				// (at least 1 char for trailing null)
				// Allocated & expanded via non-public fcts
};

// Primitive ops on strReps -- nearly all string fns go through these.

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

    string& S;			// The string I'm a subString of
    unsigned int pos;		// Starting position in S's rep
    unsigned int len;		// Length of subString

    void assign(strRep*, const char*, int = -1);
    subString(string& x, int p, int l);
    subString(const string& x, int p, int l);

public:
    // Note there are no public constructors. subStrings are always
    // created via string operations
    
    // This one should be protected, but KCC keeps complaining about this
    subString(const subString& x);
    ~subString();

    subString& operator = (const string& y);
    subString& operator = (const subString& y);
    subString& operator = (const char* t);
    subString& operator = (char* t);
    subString& operator = (char c);

    // Return true iff target appears anywhere in subString
    bool contains(char c) const;
    bool contains(const string& y) const;
    bool contains(const subString& y) const;
    bool contains(const char* t) const;
    bool contains(char* t) const;
    bool contains(const regex& r) const;

    // Return true iff target matches entire subString
    bool matches(const regex& r) const;

    // I/O 
    friend inline ostream& operator<<(ostream& s, const subString& x);

    // Status
    unsigned int length() const;
    int empty() const;
    const char* chars() const;

    bool OK() const; 
};


class string
{
    friend class subString;

protected:
    strRep* rep;   // Strings are pointers to their representations

    // Some helper functions
    int search(int, int, const char*, int = -1) const;
    int search(int, int, char) const;
    int match(int, int, int, const char*, int = -1) const;
    int _gsub(const char*, int, const char* ,int);
    int _gsub(const regex&, const char*, int);
    subString _substr(int, int);
    const subString _substr(int, int) const;

private:
    // Don't get constructed or assigned from int
    string(int) : rep(0)      { error("init from int"); }
    string& operator = (int)  { error("int assign"); return *this; }

public:
    // Constructors and assignment
    string();
    string(const string& x);
    string(const subString&  x);
    string(const char* t);
    string(const char* t, int len);
    string(char c);
    string(ostrstream& os); // should be const

    ~string();

    string& operator = (const string& y);
    string& operator = (const char* y);
    string& operator = (char* y);
    string& operator = (char c);
    string& operator = (const subString& y);
    string& operator = (ostrstream& os);

    // Concatenation
    string& operator += (const string& y); 
    string& operator += (const subString& y);
    string& operator += (const char* t);
    string& operator += (char* t);
    string& operator += (char c);

    string& prepend(const string& y); 
    string& prepend(const subString& y);
    string& prepend(const char* t);
    string& prepend(char* t);
    string& prepend(char c);


    // Procedural versions:
    // Concatenate first 2 args, store result in last arg
    friend inline void cat(const string&, const string&, string&);
    friend inline void cat(const string&, const subString&, string&);
    friend inline void cat(const string&, const char*, string&);
    friend inline void cat(const string&, char*, string&);
    friend inline void cat(const string&, char, string&);

    friend inline void cat(const subString&, const string&, string&);
    friend inline void cat(const subString&, const subString&, string&);
    friend inline void cat(const subString&, const char*, string&);
    friend inline void cat(const subString&, char*, string&);
    friend inline void cat(const subString&, char, string&);

    friend inline void cat(const char*, const string&, string&);
    friend inline void cat(const char*, const subString&, string&);
    friend inline void cat(const char*, const char*, string&);
    friend inline void cat(const char*, char*, string&);
    friend inline void cat(const char*, char, string&);

    friend inline void cat(char*, const string&, string&);
    friend inline void cat(char*, const subString&, string&);
    friend inline void cat(char*, const char*, string&);
    friend inline void cat(char*, char*, string&);
    friend inline void cat(char*, char, string&);

    friend inline void cat(char, const string&, string&);
    friend inline void cat(char, const subString&, string&);
    friend inline void cat(char, const char*, string&);
    friend inline void cat(char, char*, string&);
    friend inline void cat(char, char, string&);


    // Searching & matching

    // Return position of target in string or -1 for failure
    int index(char c, int startpos = 0) const;      
    int index(const string& y, int startpos = 0) const;      
    int index(const subString&  y, int startpos = 0) const;      
    int index(const char* t, int startpos = 0) const;  
    int index(char* t, int startpos = 0) const;  
    int index(const regex& r, int startpos = 0) const;       

    // Return 1 if target appears anyhere in string; else 0
    bool contains(char c) const;
    bool contains(const string& y) const;
    bool contains(const subString& y) const;
    bool contains(const char* t) const;
    bool contains(char* t) const;
    bool contains(const regex& r) const;

    // Return 1 if target appears anywhere after position pos 
    // (or before, if pos is negative) in string; else 0
    bool contains(char c, int pos) const;
    bool contains(const string& y, int pos) const;
    bool contains(const subString& y, int pos) const;
    bool contains(const char* t, int pos) const;
    bool contains(char* t, int pos) const;
    bool contains(const regex& r, int pos) const;

    // Return 1 if target appears at position pos in string; else 0
    bool matches(char c, int pos = 0) const;
    bool matches(const string& y, int pos = 0) const;
    bool matches(const subString& y, int pos = 0) const;
    bool matches(const char* t, int pos = 0) const;
    bool matches(char* t, int pos = 0) const;
    bool matches(const regex& r, int pos = 0) const;

    // Return number of occurences of target in string
    int freq(char c) const; 
    int freq(const string& y) const;
    int freq(const subString& y) const;
    int freq(const char* t) const;
    int freq(char* t) const;

    // subString extraction

    // Note that you can't take a subString of a const string, since
    // this leaves open the possiblility of indirectly modifying the
    // string through the subString
    subString at(int pos, int len);
    subString operator() (int pos, int len); // synonym for at

    subString at(const string& x, int startpos = 0); 
    subString at(const subString&  x, int startpos = 0); 
    subString at(const char* t, int startpos = 0);
    subString at(char* t, int startpos = 0);
    subString at(char c, int startpos = 0);
    subString at(const regex& r, int startpos = 0); 

    subString before(int pos);
    subString before(const string& x, int startpos = 0);
    subString before(const subString&   x, int startpos = 0);
    subString before(const char* t, int startpos = 0);
    subString before(char* t, int startpos = 0);
    subString before(char c, int startpos = 0);
    subString before(const regex& r, int startpos = 0);

    subString through(int pos);
    subString through(const string& x, int startpos = 0);
    subString through(const subString& x, int startpos = 0);
    subString through(const char* t, int startpos = 0);
    subString through(char* t, int startpos = 0);
    subString through(char c, int startpos = 0);
    subString through(const regex& r, int startpos = 0);

    subString from(int pos);
    subString from(const string& x, int startpos = 0);
    subString from(const subString& x, int startpos = 0);
    subString from(const char* t, int startpos = 0);
    subString from(char* t, int startpos = 0);
    subString from(char c, int startpos = 0);
    subString from(const regex& r, int startpos = 0);

    subString after(int pos);
    subString after(const string& x, int startpos = 0);
    subString after(const subString& x, int startpos = 0);
    subString after(const char* t, int startpos = 0);
    subString after(char* t, int startpos = 0);
    subString after(char c, int startpos = 0);
    subString after(const regex& r, int startpos = 0);

    // Const versions
    const subString at(int pos, int len) const;
    const subString operator() (int pos, int len) const; // synonym for at

    const subString at(const string& x, int startpos = 0) const;
    const subString at(const subString&  x, int startpos = 0) const;
    const subString at(const char* t, int startpos = 0) const;
    const subString at(char* t, int startpos = 0) const;
    const subString at(char c, int startpos = 0) const;
    const subString at(const regex& r, int startpos = 0) const;

    const subString before(int pos) const;
    const subString before(const string& x, int startpos = 0) const;
    const subString before(const subString&   x, int startpos = 0) const;
    const subString before(const char* t, int startpos = 0) const;
    const subString before(char* t, int startpos = 0) const;
    const subString before(char c, int startpos = 0) const;
    const subString before(const regex& r, int startpos = 0) const;

    const subString through(int pos) const;
    const subString through(const string& x, int startpos = 0) const;
    const subString through(const subString& x, int startpos = 0) const;
    const subString through(const char* t, int startpos = 0) const;
    const subString through(char* t, int startpos = 0) const;
    const subString through(char c, int startpos = 0) const;
    const subString through(const regex& r, int startpos = 0) const;

    const subString from(int pos) const;
    const subString from(const string& x, int startpos = 0) const;
    const subString from(const subString& x, int startpos = 0) const;
    const subString from(const char* t, int startpos = 0) const;
    const subString from(char* t, int startpos = 0) const;
    const subString from(char c, int startpos = 0) const;
    const subString from(const regex& r, int startpos = 0) const;

    const subString after(int pos) const;
    const subString after(const string& x, int startpos = 0) const;
    const subString after(const subString& x, int startpos = 0) const;
    const subString after(const char* t, int startpos = 0) const;
    const subString after(char* t, int startpos = 0) const;
    const subString after(char c, int startpos = 0) const;
    const subString after(const regex& r, int startpos = 0) const;


    // Deletion

    // Delete len chars starting at pos
    void del(int pos, int len);

    // Delete the first occurrence of target after startpos
    void del(const string& y, int startpos = 0);
    void del(const subString& y, int startpos = 0);
    void del(const char* t, int startpos = 0);
    void del(char* t, int startpos = 0);
    void del(char c, int startpos = 0);
    void del(const regex& r, int startpos = 0);

    // Global substitution: substitute all occurrences of PAT with REPL
    int gsub(const string& pat, const string& repl);
    int gsub(const subString& pat, const string& repl);
    int gsub(const char* pat, const string& repl);
    int gsub(const char* pat, const char* repl);
    int gsub(const char* pat, char* repl);
    int gsub(char* pat, const string& repl);
    int gsub(char* pat, const char* repl);
    int gsub(char* pat, char* repl);
    int gsub(const regex& pat, const string& repl);

    // Friends & utilities
    // Split string into array RES at SEPARATORS; return number of elements
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

    // Simple builtin transformations
    friend inline string reverse(const string& x);
    friend inline string upcase(const string& x);
    friend inline string downcase(const string& x);
    friend inline string capitalize(const string& x);

    // In-place versions of above
    void reverse();
    void upcase();
    void downcase();
    void capitalize();

    // Element extraction
    // Some C++ compilers cannot properly disambiguate here,
    // so we supply prototypes for all integral types.

    char& operator [] (char i);
    char& operator [] (short i);
    char& operator [] (unsigned short i);
    char& operator [] (int i);
    char& operator [] (unsigned int i);
    char& operator [] (long i);
    char& operator [] (unsigned long i);

    char operator [] (char i) const;
    char operator [] (unsigned short i) const;
    char operator [] (short i) const;
    char operator [] (int i) const;
    char operator [] (unsigned int i) const;
    char operator [] (long i) const;
    char operator [] (unsigned long i) const;

    char elem(int i) const;
    char firstchar() const;
    char lastchar() const;

    // Conversion
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

    // Status
    unsigned int length() const;
    int empty() const;

    // Preallocate some space for string
    void alloc(int newsize);

    // Report current allocation (not length!)
    int allocation() const;

    void error(const char* msg) const;

    bool OK() const;
};

#if 0
typedef string strTmp; // for backward compatibility
#endif

// Other externs

int compare(const string& x,    const string& y);
int compare(const string& x,    const subString& y);
int compare(const string& x,    const char* y);
int compare(const subString& x, const string& y);
int compare(const subString& x, const subString& y);
int compare(const subString& x, const char* y);
int compare(const char *x,      const string& y);
int compare(const char *x,      const subString& y);

int fcompare(const string& x, const string& y); // ignore case

extern strRep  _nilstrRep;
extern string _nilstring;

// Status reports, needed before defining other things

inline unsigned int string::length() const {  return rep->len; }
inline int          string::empty() const { return rep->len == 0; }
inline const char*  string::chars() const { return &(rep->s[0]); }
inline int          string::allocation() const { return rep->sz; }

inline unsigned int subString::length() const { return len; }
inline int          subString::empty() const { return len == 0; }
inline const char*  subString::chars() const { return &(S.rep->s[pos]); }

// Constructors
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

// For HAVE_PLACEMENT_NEW, if using placement new, use operator
// delete instead of vector delete.
// 
// According to Robert Wiegand <wiegand@kong.gsfc.nasa.gov>, Purify
// was flagging this and it does result in undefined behavior.
inline void string_DeleteRep(strRep *rep)
{
#if HAVE_PLACEMENT_NEW
    operator delete(rep);
#else
    delete[] (char *) rep;
#endif
}

inline string::~string()
{
    if (rep != &_nilstrRep) string_DeleteRep(rep);
}

inline subString::subString(const subString& x)
  :S(x.S), pos(x.pos), len(x.len) {}
inline subString::subString(string& x, int first, int l)
  :S(x), pos(first), len(l) {}
inline subString::subString(const string& x, int first, int l)
  :S((string &)x), pos(first), len(l) {}

inline subString::~subString() {}

// Assignment
inline string& string::operator = (const string& y)
{ 
    rep = string_Scopy(rep, y.rep); return *this;
}

inline string& string::operator = (const char* t)
{
    rep = string_Salloc(rep, t, -1, -1); return *this;
}

inline string& string::operator = (char* t)
{
    rep = string_Salloc(rep, t, -1, -1); return *this;
}

inline string& string::operator = (const subString&  y)
{
    rep = string_Salloc(rep, y.chars(), y.length(), y.length()); return *this;
}

inline string& string::operator = (char c)
{
    rep = string_Salloc(rep, &c, 1, 1); return *this;
}

inline string& string::operator = (ostrstream& os)
{
#if HAVE_FROZEN_OSTRSTREAM
    // No need to freeze the stream, since the string is copied right away
    int frozen = os.frozen();
#endif

    const char *str = os.str();
    rep = string_Salloc(rep, str, os.pcount(), os.pcount());

#if HAVE_FROZEN_OSTRSTREAM
    os.freeze(frozen);
#endif
    return *this;
}

inline string::string(ostrstream& os)
    : rep(&_nilstrRep) 
{
    operator=(os);
}

inline subString& subString::operator = (const char* ys)
{
    assign(0, ys); return *this;
}

inline subString& subString::operator = (char* ys)
{
    assign(0, ys); return *this;
}

inline subString& subString::operator = (char ch)
{
    assign(0, &ch, 1); return *this;
}

inline subString& subString::operator = (const string& y)
{
    assign(y.rep, y.chars(), y.length()); return *this;
}

inline subString& subString::operator = (const subString& y)
{
    assign(y.S.rep, y.chars(), y.length()); return *this;
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

inline void cat(const string& x, char* y, string& r)
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

inline void cat(const subString& x, char* y, string& r)
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

inline void cat(const char* x, char* y, string& r)
{
    r.rep = string_Scat(r.rep, x, -1, y, -1);
}

inline void cat(const char* x, char y, string& r)
{
    r.rep = string_Scat(r.rep, x, -1, &y, 1);
}

inline void cat(char* x, const string& y, string& r)
{
    r.rep = string_Scat(r.rep, x, -1, y.chars(), y.length());
}

inline void cat(char* x, const subString& y, string& r)
{
    r.rep = string_Scat(r.rep, x, -1, y.chars(), y.length());
}

inline void cat(char* x, const char* y, string& r)
{
    r.rep = string_Scat(r.rep, x, -1, y, -1);
}

inline void cat(char* x, char* y, string& r)
{
    r.rep = string_Scat(r.rep, x, -1, y, -1);
}

inline void cat(char* x, char y, string& r)
{
    r.rep = string_Scat(r.rep, x, -1, &y, 1);
}

inline void cat(char x, const string& y, string& r)
{
    r.rep = string_Scat(r.rep, &x, 1, y.chars(), y.length());
}

inline void cat(char x, const subString& y, string& r)
{
    r.rep = string_Scat(r.rep, &x, 1, y.chars(), y.length());
}

inline void cat(char x, const char* y, string& r)
{
    r.rep = string_Scat(r.rep, &x, 1, y, -1);
}

inline void cat(char x, char* y, string& r)
{
    r.rep = string_Scat(r.rep, &x, 1, y, -1);
}

inline void cat(char x, char y, string& r)
{
    r.rep = string_Scat(r.rep, &x, 1, &y, 1);
}


// Operator versions
inline string& string::operator += (const string& y)
{
    cat(*this, y, *this); return *this;
}

inline string& string::operator += (const subString& y)
{
    cat(*this, y, *this); return *this;
}

inline string& string::operator += (const char* y)
{
    cat(*this, y, *this); return *this;
}

inline string& string::operator += (char* y)
{
    cat(*this, y, *this); return *this;
}

inline string& string:: operator +=(char y)
{
    cat(*this, y, *this); return *this;
}

// Constructive concatenation
#if HAVE_NAMED_RETURN_VALUES

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

inline string operator + (const string& x, char* y) return r;
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

inline string operator + (const subString& x, char* y) return r;
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

inline string operator + (char* x, const string& y) return r;
{
    cat(x, y, r);
}

inline string operator + (char* x, const subString& y) return r;
{
    cat(x, y, r);
}

inline string operator + (char x, const string& y) return r;
{
    cat(x, y, r);
}

inline string operator + (char x, const subString& y) return r;
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

inline string operator + (const string& x, char* y) 
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

inline string operator + (const subString& x, char* y) 
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

inline string operator + (char* x, const string& y) 
{
    string r; cat(x, y, r); return r;
}

inline string operator + (char* x, const subString& y) 
{
    string r; cat(x, y, r); return r;
}

inline string operator + (char x, const string& y) 
{
    string r; cat(x, y, r); return r;
}

inline string operator + (char x, const subString& y) 
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

#endif /* ! defined(NAMED_RETURN_VALUES) */

// prepend

inline string& string::prepend(const string& y)
{
    rep = string_Sprepend(rep, y.chars(), y.length()); return *this;
}

inline string& string::prepend(const char* y)
{
    rep = string_Sprepend(rep, y, -1); return *this;
}

inline string& string::prepend(char* y)
{
    rep = string_Sprepend(rep, y, -1); return *this;
}

inline string& string::prepend(char y)
{
    rep = string_Sprepend(rep, &y, 1); return *this;
}

inline string& string::prepend(const subString& y)
{
    rep = string_Sprepend(rep, y.chars(), y.length());return *this;
}


// Misc transformations

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


// Element extraction

inline char&  string::operator [] (unsigned int i) 
{
#ifndef NDEBUG
    if (i >= length()) error("invalid index");
#endif
    return rep->s[i];
}

inline char string::operator [] (unsigned int i) const
{ 
#ifndef NDEBUG
    if (i >= length()) error("invalid index");
#endif
    return rep->s[i];
}

inline char& string::operator [] (int i) 
{
    return string::operator [] ((unsigned int) i);
}

inline char string::operator [] (int i) const
{
    return string::operator [] ((unsigned int) i);
}

inline char& string::operator [] (char i) 
{
    return string::operator [] ((unsigned int) i);
}

inline char string::operator [] (char i) const
{
    return string::operator [] ((unsigned int) i);
}

inline char& string::operator [] (short i) 
{
    return string::operator [] ((unsigned int) i);
}

inline char string::operator [] (short i) const
{
    return string::operator [] ((unsigned int) i);
}

inline char& string::operator [] (unsigned short i) 
{
    return string::operator [] ((unsigned int) i);
}

inline char string::operator [] (unsigned short i) const
{
    return string::operator [] ((unsigned int) i);
}

inline char& string::operator [] (long i) 
{
    return string::operator [] ((unsigned int) i);
}

inline char string::operator [] (long i) const
{
    return string::operator [] ((unsigned int) i);
}

inline char& string::operator [] (unsigned long i) 
{
    return string::operator [] ((unsigned int) i);
}

inline char string::operator [] (unsigned long i) const
{
    return string::operator [] ((unsigned int) i);
}

inline char string::elem (int i) const
{ 
#ifndef NDEBUG
    if (((unsigned)i) >= length()) error("invalid index");
#endif
    return rep->s[i];
}

inline char string::firstchar() const
{ 
    return elem(0);
}

inline char string::lastchar() const
{ 
    return elem(length() - 1);
}

// Searching

inline int string::index(char c, int startpos) const
{
    return search(startpos, length(), c);
}

inline int string::index(const char* t, int startpos) const
{   
    return search(startpos, length(), t);
}

inline int string::index(char* t, int startpos) const
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

inline bool string::contains(char c) const
{
    return search(0, length(), c) >= 0;
}

inline bool string::contains(const char* t) const
{   
    return search(0, length(), t) >= 0;
}

inline bool string::contains(char* t) const
{   
    return search(0, length(), t) >= 0;
}

inline bool string::contains(const string& y) const
{   
    return search(0, length(), y.chars(), y.length()) >= 0;
}

inline bool string::contains(const subString& y) const
{   
    return search(0, length(), y.chars(), y.length()) >= 0;
}

inline bool string::contains(char c, int p) const
{
    return match(p, length(), 0, &c, 1) >= 0;
}

inline bool string::contains(const char* t, int p) const
{
    return match(p, length(), 0, t) >= 0;
}

inline bool string::contains(char* t, int p) const
{
    return match(p, length(), 0, t) >= 0;
}

inline bool string::contains(const string& y, int p) const
{
    return match(p, length(), 0, y.chars(), y.length()) >= 0;
}

inline bool string::contains(const subString& y, int p) const
{
    return match(p, length(), 0, y.chars(), y.length()) >= 0;
}

inline bool string::contains(const regex& r) const
{
    int unused;  return r.search(chars(), length(), unused, 0) >= 0;
}

inline bool string::contains(const regex& r, int p) const
{
    return r.match(chars(), length(), p) >= 0;
}


inline bool string::matches(const subString& y, int p) const
{
    return match(p, length(), 1, y.chars(), y.length()) >= 0;
}

inline bool string::matches(const string& y, int p) const
{
    return match(p, length(), 1, y.chars(), y.length()) >= 0;
}

inline bool string::matches(const char* t, int p) const
{
    return match(p, length(), 1, t) >= 0;
}

inline bool string::matches(char* t, int p) const
{
    return match(p, length(), 1, t) >= 0;
}

inline bool string::matches(char c, int p) const
{
    return match(p, length(), 1, &c, 1) >= 0;
}

inline bool string::matches(const regex& r, int p) const
{
    int l = (p < 0)? -p : length() - p;
    return r.match(chars(), length(), p) == l;
}


inline bool subString::contains(const char* t) const
{   
    return S.search(pos, pos+len, t) >= 0;
}

inline bool subString::contains(char* t) const
{   
    return S.search(pos, pos+len, t) >= 0;
}

inline bool subString::contains(const string& y) const
{   
    return S.search(pos, pos+len, y.chars(), y.length()) >= 0;
}

inline bool subString::contains(const subString&  y) const
{   
    return S.search(pos, pos+len, y.chars(), y.length()) >= 0;
}

inline bool subString::contains(char c) const
{
    return S.search(pos, pos+len, 0, c) >= 0;
}

inline bool subString::contains(const regex& r) const
{
    int unused;  return r.search(chars(), len, unused, 0) >= 0;
}

inline bool subString::matches(const regex& r) const
{
    return unsigned(r.match(chars(), len, 0)) == len;
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

inline int string::gsub(char* pat, const string& r)
{
    return _gsub(pat, -1, r.chars(), r.length());
}

inline int string::gsub(const char* pat, const char* r)
{
    return _gsub(pat, -1, r, -1);
}

inline int string::gsub(const char* pat, char* r)
{
    return _gsub(pat, -1, r, -1);
}

inline int string::gsub(char* pat, const char* r)
{
    return _gsub(pat, -1, r, -1);
}

inline int string::gsub(char* pat, char* r)
{
    return _gsub(pat, -1, r, -1);
}


// `char *' => `const char *' wrappers

inline subString string::after(char* t, int startpos)
{
    return after((const char *)t, startpos);
}

inline subString string::at(char* t, int startpos)
{
    return at((const char *)t, startpos);
}

inline subString string::before(char* t, int startpos)
{
    return before((const char *)t, startpos);
}

inline void string::del(char* t, int startpos)
{
    del((const char *)t, startpos);
}

inline int string::freq(char* t) const
{
    return freq((const char *)t);
}

inline subString string::from(char* t, int startpos)
{
    return from((const char *)t, startpos);
}

inline subString string::through(char* t, int startpos)
{
    return through((const char *)t, startpos);
}

inline int compare(const subString& x, char* y)
{
    return compare(x, (const char*)y);
}

inline int compare(const string& x, char* y)
{
    return compare(x, (const char*)y);
}

inline int compare(char *x, const subString& y)
{
    return compare((const char*)x, y);
}

inline int compare(char *x, const string& y)
{
    return compare((const char*)x, y);
}

inline int compare(const string& x, char y)
{
    return x.length() == 1 ? *x.chars() - y : x.length() - 1;
}

inline int compare(char x, const string& y)
{
    return -compare(y, x);
}

inline int compare(const subString& x, char y)
{
    return x.length() == 1 ? *x.chars() - y : x.length() - 1;
}

inline int compare(char x, const subString& y)
{
    return -compare(y, x);
}

// Const wrappers
inline const subString string::at(int pos, int len) const
{
    return ((string *)this)->at(pos, len);
}

inline const subString string::operator() (int pos, int len) const
{
    return ((string *)this)->operator()(pos, len);
}

inline const subString string::at(const string& x, int startpos) const
{
    return ((string *)this)->at(x, startpos);
}

inline const subString string::at(const subString&  x, int startpos) const
{
    return ((string *)this)->at(x, startpos);
}

inline const subString string::at(const char* t, int startpos) const
{
    return ((string *)this)->at(t, startpos);
}

inline const subString string::at(char* t, int startpos) const
{
    return ((string *)this)->at(t, startpos);
}

inline const subString string::at(char c, int startpos) const
{
    return ((string *)this)->at(c, startpos);
}

inline const subString string::at(const regex& r, int startpos) const
{
    return ((string *)this)->at(r, startpos);
}


inline const subString string::before(int pos) const
{
    return ((string *)this)->before(pos);
}

inline const subString string::before(const string& x, int startpos) const
{
    return ((string *)this)->before(x, startpos);
}

inline const subString string::before(const subString& x, int startpos) 
    const
{
    return ((string *)this)->before(x, startpos);
}

inline const subString string::before(const char* t, int startpos) const
{
    return ((string *)this)->before(t, startpos);
}

inline const subString string::before(char* t, int startpos) const
{
    return ((string *)this)->before(t, startpos);
}

inline const subString string::before(char c, int startpos) const
{
    return ((string *)this)->before(c, startpos);
}

inline const subString string::before(const regex& r, int startpos) const
{
    return ((string *)this)->before(r, startpos);
}


inline const subString string::through(int pos) const
{
    return ((string *)this)->through(pos);
}

inline const subString string::through(const string& x, int startpos) const
{
    return ((string *)this)->through(x, startpos);
}

inline const subString string::through(const subString& x, int startpos) 
    const
{
    return ((string *)this)->through(x, startpos);
}

inline const subString string::through(const char* t, int startpos) const
{
    return ((string *)this)->through(t, startpos);
}

inline const subString string::through(char* t, int startpos) const
{
    return ((string *)this)->through(t, startpos);
}

inline const subString string::through(char c, int startpos) const
{
    return ((string *)this)->through(c, startpos);
}

inline const subString string::through(const regex& r, int startpos) const
{
    return ((string *)this)->through(r, startpos);
}


inline const subString string::from(int pos) const
{
    return ((string *)this)->from(pos);
}

inline const subString string::from(const string& x, int startpos) const
{
    return ((string *)this)->from(x, startpos);
}

inline const subString string::from(const subString& x, int startpos) const
{
    return ((string *)this)->from(x, startpos);
}

inline const subString string::from(const char* t, int startpos) const
{
    return ((string *)this)->from(t, startpos);
}

inline const subString string::from(char* t, int startpos) const
{
    return ((string *)this)->from(t, startpos);
}

inline const subString string::from(char c, int startpos) const
{
    return ((string *)this)->from(c, startpos);
}

inline const subString string::from(const regex& r, int startpos) const
{
    return ((string *)this)->from(r, startpos);
}


inline const subString string::after(int pos) const
{
    return ((string *)this)->after(pos);
}

inline const subString string::after(const string& x, int startpos) const
{
    return ((string *)this)->after(x, startpos);
}

inline const subString string::after(const subString& x, int startpos) 
    const
{
    return ((string *)this)->after(x, startpos);
}

inline const subString string::after(const char* t, int startpos) const
{
    return ((string *)this)->after(t, startpos);
}

inline const subString string::after(char* t, int startpos) const
{
    return ((string *)this)->after(t, startpos);
}

inline const subString string::after(char c, int startpos) const
{
    return ((string *)this)->after(c, startpos);
}

inline const subString string::after(const regex& r, int startpos) const
{
    return ((string *)this)->after(r, startpos);
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

// A zillion comparison operators - for every combination of char,
// char *, const char *, string, and subString.
#define string_COMPARE(op, t1, t2) \
inline int operator op(t1 x, t2 y) \
{ \
    return compare(x, y) op 0; \
}

#define string_COMPARE_ALL(t1, t2) \
string_COMPARE(==, t1, t2) \
string_COMPARE(!=, t1, t2) \
string_COMPARE(<,  t1, t2) \
string_COMPARE(>,  t1, t2) \
string_COMPARE(<=, t1, t2) \
string_COMPARE(>=, t1, t2)

string_COMPARE_ALL(const string&, const string&)
string_COMPARE_ALL(const string&, const subString&)
string_COMPARE_ALL(const string&, const char *)
string_COMPARE_ALL(const string&, char *)
string_COMPARE_ALL(const string&, char)

string_COMPARE_ALL(const subString&, const string&)
string_COMPARE_ALL(const subString&, const subString&)
string_COMPARE_ALL(const subString&, const char *)
string_COMPARE_ALL(const subString&, char *)
string_COMPARE_ALL(const subString&, char)

string_COMPARE_ALL(const char *, const string&)
string_COMPARE_ALL(const char *, const subString&)

string_COMPARE_ALL(char *, const string&)
string_COMPARE_ALL(char *, const subString&)

string_COMPARE_ALL(char, const string&)
string_COMPARE_ALL(char, const subString&)

#undef string_COMPARE
#undef string_COMPARE_ALL

// A helper needed by at, before, etc.
inline subString string::_substr(int first, int l)
{
    if (first < 0 || (unsigned)(first + l) > length())
	return subString(_nilstring, 0, 0);
    else 
	return subString(*this, first, l);
}

inline const subString string::_substr(int first, int l) const
{
    if (first < 0 || (unsigned)(first + l) > length())
	return subString(_nilstring, 0, 0);
    else 
	return subString(*this, first, l);
}

#endif // _ICE_strclass_h
// DON'T ADD ANYTHING BEHIND THIS #endif
