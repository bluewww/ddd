// $Id$ 
// The VSL Library

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_VSLLib_h
#define _DDD_VSLLib_h

#ifdef __GNUG__
#pragma interface
#endif


#include <iostream.h>
#include "strclass.h"

#include "bool.h"
#include "VSEFlags.h"
#include "TypeInfo.h"
#include "StringBox.h"
#include "ListBox.h"
#include "TagBox.h"
#include "PrimitiveB.h"

class VSLDef;
class VSLDefList;
class VSLNode;

class VSLLib;

// Custom eval() arguments
// This is just a wrapper around a Box pointer with a few fancy conversions.
class VSLArg {
private:
    Box *_box;

public:
    Box *box() const { return _box; }

    VSLArg()
	:_box(0)
    {}

    VSLArg(char *s)
	:_box(new StringBox(s))
    {}
    
    VSLArg(const string& s)
	:_box(new StringBox(s))
    {}
    
    VSLArg(int n)
	:_box(new SquareBox(n))
    {}

    VSLArg(unsigned n)
	:_box(new SquareBox(n))
    {}

    // When we are assigned a box, we take control over it
    VSLArg(Box *box)
	:_box(box)
    {}

    // When we are assigned a VSLArg, we establish another link...
    VSLArg(const VSLArg& arg)
	:_box(arg.box() ? arg.box()->link() : 0)
    {}

    // ...since it will be destroyed when we leave.
    ~VSLArg()
    {
	if (_box)
	    _box->unlink();
    }

    // Assignment: just the same.
    void operator = (Box *box)
    {
	if (box != _box)
	{
	    if (_box)
		_box->unlink();
	    _box = box;
	}
    }

    void operator = (const VSLArg& arg)
    {
	if (arg.box() != _box)
	{
	    if (_box)
		_box->unlink();

	    _box = arg.box() ? arg.box()->link() : 0;
	}
    }
};


// Custom tagging function using VSLArgs
inline VSLArg tag(const VSLArg& arg, Data *data = 0, DataLink *link = 0)
{
    // Prevent double unlink: from Box::tag() and ~VSLArg()
    arg.box()->link();

    // Tag the box
    return arg.box()->tag(data, link);
}



// Size of hash table (should be prime)
const int hashSize = 4001;

// Flags for optimizing (used internally)
const unsigned _ResolveDefs             = (1 << 4);
const unsigned _ResolveSynonyms         = (1 << 5);
const unsigned _FoldOps                 = (1 << 6);
const unsigned _FoldConsts              = (1 << 7);
const unsigned _InlineFuncs             = (1 << 8);
const unsigned _CountSelfReferences     = (1 << 9);
const unsigned _Cleanup                 = (1 << 10);

// Public flags for optimizing
const unsigned ResolveDefs          = _ResolveDefs;
const unsigned ResolveSynonyms      = _ResolveDefs | _ResolveSynonyms;
const unsigned FoldOps              = _ResolveSynonyms | _FoldOps;
const unsigned FoldConsts           = _FoldConsts;
const unsigned InlineFuncs          = _ResolveDefs | _InlineFuncs;
const unsigned CountSelfReferences  = _CountSelfReferences;
const unsigned Cleanup              = _CountSelfReferences | _Cleanup;

// Mask for building #iterations
const unsigned loopMask         = (1 << 0 | 1 << 1 | 1 << 2 | 1 << 3);
const unsigned allOpts          = ((unsigned)-1) & ~loopMask;
const unsigned stdOpt           = allOpts | 2;


// The VSL library
class VSLLib {
    friend class VSLDefList;  // for VSLDefList::replace()

public:
    DECLARE_TYPE_INFO

private:
    string _lib_name;               // library name
    VSLDefList *defs[hashSize];     // hash table containing definitions
    VSLDef *_first;                 // linked list over definitions
    VSLDef *_last;                  // last def in list

    void initHash();

    // Optimizing and post-processing
    int bind();                     // bind internal functions to references
    int resolveNames();             // bind names to variables
    int compilePatterns();          // compile call patterns
    int resolveDefs();              // resolve unambiguous function calls
    int resolveSynonyms();          // resolve synonyms
    int foldOps();                  // fold associative operators
    int foldConsts();               // evaluate constants
    int inlineFuncs();              // perform function inlining
    int countSelfReferences();      // count references internal to functions
    int cleanup();                  // remove unreferenced functions

    // Front-end for optimizing functions
    void process(unsigned mode = stdOpt);

    // Build function call with arglist as argument
    VSLNode *_call(const string& func_name, VSLNode *arglist);

    // Build function call with given args
    VSLNode *call(const string& func_name);
    VSLNode *call(const string& func_name, 
		  VSLNode *arg1);
    VSLNode *call(const string& func_name, 
		  VSLNode *arg1, 
		  VSLNode *arg2);
    VSLNode *call(const string& func_name, 
		  VSLNode *arg1, 
		  VSLNode *arg2, 
		  VSLNode *arg3);

    // same, but with char *
    VSLNode *call(char *func_name) 
    { 
	return call(string(func_name)); 
    }
    VSLNode *call(char *func_name, 
		  VSLNode *arg1)
    { 
	return call(string(func_name), arg1); 
    }
    VSLNode *call(char *func_name, 
		  VSLNode *arg1,
		  VSLNode *arg2)
    { 
	return call(string(func_name), arg1, arg2);
    }
    VSLNode *call(char *func_name, 
		  VSLNode *arg1,
		  VSLNode *arg2,
		  VSLNode *arg3)
    { 
	return call(string(func_name), arg1, arg2, arg3);
    }

    // Add a new function
    VSLDef *add(const string& func_name, 
		VSLNode *pattern, 
		VSLNode *expr = 0,
		bool global = false, 
		const string& filename = "builtin", 
		int lineno = 0);

    // Rename function
    int override(const string& func_name);

    // Remove definition
    int replace(const string& func_name);

    // Find function
    VSLDefList *deflist(const string& func_name) const;
    VSLDef *def(const string& func_name, Box *arg) const;

    // The parse function generated by YACC -- this cannot be a C++ name
    friend int VSLLib_parse();

public:
    // Build
    VSLLib();
    VSLLib(const string& lib_name, unsigned optimize = stdOpt);
    VSLLib(istream& s, unsigned optimize = stdOpt);

    // Read
    VSLLib& read(const string& lib_name, unsigned optimize = stdOpt);
    VSLLib& read(istream& s, unsigned optimize = stdOpt);
    static int parse();

    // Check if function is present
    bool has(const string& func_name)
    {
	return deflist(func_name) != 0;
    }

    // Evaluate function with given argument list
    const Box *eval(const string& func_name, ListBox *arg) const;
    
    // Custom functions
    const Box *eval(const string& func_name, VSLArg args[]) const;
    const Box *eval(const string& func_name, 
		    VSLArg arg0 = (Box *)0,
		    VSLArg arg1 = (Box *)0,
		    VSLArg arg2 = (Box *)0,
		    VSLArg arg3 = (Box *)0,
		    VSLArg arg4 = (Box *)0,
		    VSLArg arg5 = (Box *)0,
		    VSLArg arg6 = (Box *)0,
		    VSLArg arg7 = (Box *)0,
		    VSLArg arg8 = (Box *)0,
		    VSLArg arg9 = (Box *)0) const;

    // Perform __output() function on arg if present
    void output(Box *&arg);

    // Destroy
    virtual ~VSLLib();

    // Message handling
    static void echo(const string& s);

    // Parsing messages
    static void parse_echo(const string& s);
    static void parse_error(const string& s);
    static void parse_warning(const string& s);

    // Evaluation messages
    static void eval_echo(const string& s, const VSLDef *def = 0);
    static void eval_error(const string& s, const VSLDef *def = 0);
    static void eval_warning(const string& s, const VSLDef *def = 0);

    // Debugging
    friend ostream& operator << (ostream& s, const VSLLib& lib);
    void dumpTree(ostream& s) const;
    VSLDef *lastdef()  { return _last; }
    VSLDef *firstdef() { return _first; }

    // Background processing
    static void (*background)();

    // Representation invariant
    virtual bool OK() const;
};

inline int VSLLib::parse()
{
    return VSLLib_parse();
}

#endif
