// $Id$ 
// The VSL Library

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000 Universitaet Passau, Germany.
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

#ifndef _DDD_VSLLib_h
#define _DDD_VSLLib_h

#include <iostream>
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

    VSLArg(const char *s)
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
    VSLArg& operator = (Box *box)
    {
	if (box != _box)
	{
	    if (_box)
		_box->unlink();
	    _box = box;
	}
	return *this;
    }

    VSLArg& operator = (const VSLArg& arg)
    {
      if (this != &arg){
	if (arg.box() != _box)
	{
	    if (_box)
		_box->unlink();

	    _box = arg.box() ? arg.box()->link() : 0;
	}
      }
      return *this;
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
const unsigned _Basics                  = (1 << 11);

// Public flags for optimizing
const unsigned ResolveDefs          = _Basics | _ResolveDefs;
const unsigned ResolveSynonyms      = ResolveDefs | _ResolveSynonyms;
const unsigned FoldOps              = ResolveSynonyms | _FoldOps;
const unsigned FoldConsts           = _Basics | _FoldConsts;
const unsigned InlineFuncs          = ResolveDefs | _InlineFuncs;
const unsigned CountSelfReferences  = _Basics | _CountSelfReferences;
const unsigned Cleanup              = CountSelfReferences | _Cleanup;

// Mask for building #iterations
const unsigned loopMask         = (1 << 0 | 1 << 1 | 1 << 2 | 1 << 3);
const unsigned allOpts          = ((unsigned)-1) & ~loopMask;
const unsigned stdOpt           = allOpts | 2;

int VSLLib_parse(void);

// The VSL library
class VSLLib {
    friend class VSLDefList;	    // VSLDefList::replace() needs this
    friend class DefCallNode;	    // DefCallNode::rebind() needs this

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
    VSLNode *call(const char *func_name) 
    { 
	return call(string(func_name)); 
    }
    VSLNode *call(const char *func_name, 
		  VSLNode *arg1)
    { 
	return call(string(func_name), arg1); 
    }
    VSLNode *call(const char *func_name, 
		  VSLNode *arg1,
		  VSLNode *arg2)
    { 
	return call(string(func_name), arg1, arg2);
    }
    VSLNode *call(const char *func_name, 
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

protected:
    void init_from(const VSLLib& lib);
    void clear();

    VSLLib(const VSLLib& lib);
    VSLLib& operator = (const VSLLib& lib);

    static void default_echo(const string& s);

public:
    // Build
    VSLLib();
    VSLLib(const string& lib_name, unsigned optimize = stdOpt);
    VSLLib(std::istream& s, unsigned optimize = stdOpt);

    // Read
    virtual void update(const string& lib_name);
    virtual void update(std::istream& is);
    static int parse();

    // Optimize
    virtual void optimize(unsigned mode = stdOpt);

    // Clone
    virtual VSLLib *dup() const;

    // Check if function is present
    bool has(const string& func_name) const
    {
	return deflist(func_name) != 0;
    }

    // Evaluate function with given argument list
    const Box *eval(const string& func_name, ListBox *arg) const;
    
    // Custom functions
    const Box *eval(const string& func_name, VSLArg args[]) const;
    const Box *eval(const string& func_name, 
		    const VSLArg& arg0 = (Box *)0,
		    const VSLArg& arg1 = (Box *)0,
		    const VSLArg& arg2 = (Box *)0,
		    const VSLArg& arg3 = (Box *)0,
		    const VSLArg& arg4 = (Box *)0,
		    const VSLArg& arg5 = (Box *)0,
		    const VSLArg& arg6 = (Box *)0,
		    const VSLArg& arg7 = (Box *)0,
		    const VSLArg& arg8 = (Box *)0,
		    const VSLArg& arg9 = (Box *)0) const;

    // Perform __output() function on arg if present
    void output(Box *&arg);

    // Destroy
    virtual ~VSLLib();

    // Message handling
    static void (*echo)(const string& s);

    // Parsing messages
    static void parse_echo(const string& s);
    static void parse_error(const string& s);
    static void parse_warning(const string& s);

    // Evaluation messages
    static void eval_echo(const string& s, const VSLDef *def = 0);
    static void eval_error(const string& s, const VSLDef *def = 0);
    static void eval_warning(const string& s, const VSLDef *def = 0);

    // Debugging
    friend std::ostream& operator << (std::ostream& s, const VSLLib& lib);
    void dumpTree(std::ostream& s) const;
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
