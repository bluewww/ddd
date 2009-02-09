// $Id$
// The VSL Library

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000 Universitaet Passau, Germany.
// Copyright (C) 2003, 2004 Free Software Foundation, Inc.
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

char VSLLib_rcsid[] = 
    "$Id$";

#include "assert.h"
#include "hash.h"

#include "VSEFlags.h"

#include "VSLLib.h"
#include "VSLDef.h"
#include "VSLDefList.h"
#include "VSLBuiltin.h"

#include "VSLNode.h"
#include "DefCallN.h"
#include "BuiltinCN.h"
#include "ArgNode.h"
#include "ListNode.h"
#include "TrueNode.h"
#include "StringBox.h"

DEFINE_TYPE_INFO_0(VSLLib)

// VSLLib

// Data

void (*VSLLib::background)() = 0;   // Background proc

#define BACKGROUND() do { if (background) background(); } while (false)
#define ASSERT_OK() \
do { \
    if (VSEFlags::assert_library_ok) \
    { \
        bool ok = OK(); \
        assert(ok); \
	if (!ok) \
	    abort(); \
    } \
} while (false)


void (*VSLLib::echo)(const string& s) = VSLLib::default_echo;

// Initialization

// Init hash table
void VSLLib::initHash()
{
    for (int i = 0; i < hashSize; i++)
	defs[i] = 0;
}

// Init VSL library
VSLLib::VSLLib()
    : _lib_name(""), _first(0), _last(0)
{
    initHash();
}


// Init VSL library and read from file
VSLLib::VSLLib(const string& lib_name, unsigned optimizeMode)
    : _lib_name(lib_name), _first(0), _last(0)
{
    initHash();
    update(lib_name);
    optimize(optimizeMode);
}


// Init VSL library and read from stream
VSLLib::VSLLib(std::istream& i, unsigned optimizeMode)
    : _lib_name(""), _first(0), _last(0)
{
    initHash();
    update(i);
    optimize(optimizeMode);
}


// Return list of defs for FUNC_NAME; 0 if not found
VSLDefList* VSLLib::deflist(const string& func_name) const
{
    unsigned hashcode = hashpjw(func_name.chars()) % hashSize;

    VSLDefList *d;
    for (d = defs[hashcode];
	 d != 0 && d->func_name() != func_name; d = d->next())
	;

    return d;
}


// Add definition to library
VSLDef *VSLLib::add(const string& func_name,
    VSLNode *pattern, VSLNode *expr, 
    bool global, const string& filename, int lineno)
{
    BACKGROUND();

    // Find DefList for function name
    VSLDefList *d = deflist(func_name);
    
    if (d == 0)
    {
	// Not found?  Create a new DefList...
	unsigned hashcode = hashpjw(func_name.chars()) % hashSize;

	d = defs[hashcode];
	defs[hashcode] = new VSLDefList(this, hashcode, func_name, global);
	defs[hashcode]->next() = d;

	// ...and return it
	d = defs[hashcode];
    }

    // Append new def to DefList
    bool newFlag;
    VSLDef* newdef = d->add(newFlag, pattern, expr, filename, lineno);

    if (newFlag)
    {
	// Insert def into global list
	if (_last == 0)
	    _first = newdef;
	else
	{
	    _last->libnext() = newdef;
	    newdef->libprev() = _last;
	}
	_last = newdef;
    }

    // Setup scope: Scope is global if any def is global.
    d->global() = d->global() || global;

    return newdef;
}


// Rename function name
int VSLLib::override(const string& func_name)
{
    VSLDefList *d = deflist(func_name);

    if (d == 0)
	return -1;

    d->override();
    return 0;
}

// Delete function def
int VSLLib::replace(const string& func_name)
{
    VSLDefList *d = deflist(func_name);

    if (d == 0)
	return -1;

    d->replace();
    return 0;
}

    



// Return definition for given function name and argument
VSLDef *VSLLib::def(const string& func_name, Box *a) const
{
    VSLDefList *d = deflist(func_name);
    return d ? d->def(a) : 0;
}


// Evaluate call
const Box *VSLLib::eval(const string& func_name, ListBox *a) const
{
    VSLDefList *d = deflist(func_name);
    if (d == 0)
	eval_error("'" + func_name + "(...)' undefined");

    const Box *result = d ? d->eval(a) : 0;

    if (VSEFlags::show_huge_eval)
    {
	std::clog << func_name << *a;
	if (result)
	    std::clog << " = " << *result;
	else
	    std::clog << " FAILS";

	std::clog << "\n";
    }

    return result;
}

// Custom functions
const Box *VSLLib::eval(const string& func_name, VSLArg args[]) const
{
    ListBox *arg = new ListBox;
    for (int i = 0; args[i].box() != 0; i++)
	*arg += args[i].box();
    
    const Box *result = eval(func_name, arg);
    arg->unlink();

    return result;
}

const Box *VSLLib::eval(const string& func_name,
			const VSLArg& arg0,
			const VSLArg& arg1,
			const VSLArg& arg2,
			const VSLArg& arg3,
			const VSLArg& arg4,
			const VSLArg& arg5,
			const VSLArg& arg6,
			const VSLArg& arg7,
			const VSLArg& arg8,
			const VSLArg& arg9) const
{
    VSLArg args[11];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;
    args[4] = arg4;
    args[5] = arg5;
    args[6] = arg6;
    args[7] = arg7;
    args[8] = arg8;
    args[9] = arg9;
    args[10] = (Box *)0;

    return eval(func_name, args);
}

// Prepare expression for output
void VSLLib::output(Box *&a)
{
    if (a == 0)
	return;

    VSLDefList *d = deflist("__output");
    if (d == 0)
	return;

    ListBox *args = new ListBox(a, new ListBox);
    a->unlink();
    a = (Box *)d->eval(args);
    args->unlink();
}
    

// Destructor
void VSLLib::clear()
{
    for (int i = 0; i < hashSize; i++)
	if (defs[i] != 0)
	{
	    defs[i]->destroy();
	    defs[i] = 0;
	}

    _first = 0;
    _last  = 0;
}

VSLLib::~VSLLib()
{
    clear();
}


// Duplication
VSLLib::VSLLib(const VSLLib& lib)
    : _lib_name(),
      _first(0),
      _last(0)
{
    initHash();
    init_from(lib);
}

void VSLLib::init_from(const VSLLib& lib)
{
    _lib_name = lib._lib_name;
    _first = 0;
    _last  = 0;

    const VSLDef *d;
    for (d = lib._first; d != 0; d = d->libnext())
    {
	// std::clog << "Adding: " << d->func_name() << "\n";

	add(d->deflist->func_name(),
	    d->node_pattern() ? d->node_pattern()->dup() : 0,
	    d->expr()         ? d->expr()->dup()         : 0,
	    d->deflist->global(),
	    d->filename(),
	    d->lineno());
    }

    assert(OK());

    // Make sure all pointers point to the new defs within this library
    for (d = _first; d != 0; d = d->libnext())
    {
	d->node_pattern()->rebind(this);
	if (d->expr())
	    d->expr()->rebind(this);
    }

    assert(OK());
}

VSLLib *VSLLib::dup() const
{
    return new VSLLib(*this);
}

// Assignment
VSLLib& VSLLib::operator = (const VSLLib& lib)
{
    if (this != &lib) {
      clear();
      assert(OK());
      init_from(lib);
    }

    return *this;
}



// Binder
// Add definitions of builtin functions;
// assign default value to still undefined functions
int VSLLib::bind()
{
    int changes = 0;
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
	if (cdef->expr() == 0)
	{
	    BACKGROUND();

	    // Deklaration without definition? Find suitable builtin function
	    int idx = VSLBuiltin::resolve(cdef->deflist->func_name());

	    if (idx < 0)
	    {
		eval_error("function declared, but not defined", cdef);
		
		// Replace missing def by "__undef()" call
		cdef->expr() = call("__undef");
		if (cdef->expr() == 0)
		    cdef->expr() = new ConstNode(new StringBox("?" "?" "?"));
	    }
	    else
	    {
		// All args passed to the declaration must be passed
		// to the builtin function as well
		cdef->expr() = new BuiltinCallNode(idx, 
		    cdef->node_pattern()->dup());
	    }

	    changes++;
	}

    return changes;
}


// Bind names to variables
int VSLLib::resolveNames()
{
    int changes = 0;
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	BACKGROUND();

	changes += cdef->resolveNames();
	ASSERT_OK();
    }

    return changes;
}


// (Re)create all function patterns
int VSLLib::compilePatterns()
{
    int changes = 0;

    // First, delete all patterns
    VSLDef *cdef;
    for (cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	BACKGROUND();

	// Function pattern
	cdef->uncompilePattern();
	ASSERT_OK();

	// LET and WHERE patterns
	cdef->expr()->uncompilePatterns(cdef);
	ASSERT_OK();
    }

    // Now recreate all patterns
    for (cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	BACKGROUND();

	// Function pattern
	cdef->compilePattern();
	ASSERT_OK();

	// LET and WHERE patterns
	cdef->expr()->compilePatterns(cdef);
	ASSERT_OK();

	changes++;
    }

    return changes;
}


// Make function calls unambiguous
int VSLLib::resolveDefs()
{
    int changes = 0;
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	BACKGROUND();

	changes += cdef->expr()->resolveDefs(cdef);
	ASSERT_OK();
    }

    return changes;
}

// Resolve synonyms
int VSLLib::resolveSynonyms()
{
    int changes = 0;
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	BACKGROUND();

	changes += cdef->expr()->resolveSynonyms(cdef, &cdef->expr());
	ASSERT_OK();
    }

    return changes;
}

// Make big operands
int VSLLib::foldOps()
{
    int changes = 0;
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	BACKGROUND();

	changes += cdef->expr()->foldOps(cdef, &cdef->expr());
	ASSERT_OK();
    }

    return changes;
}


// Build constants
int VSLLib::foldConsts()
{
    int changes = 0;
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
	if (VSEFlags::optimize_globals || !cdef->deflist->global())
	{
	    BACKGROUND();

	    changes += cdef->expr()->foldConsts(cdef, &cdef->expr());
	    ASSERT_OK();
	}

    return changes;
}


// Perform function inlining
int VSLLib::inlineFuncs()
{
    int changes = 0;
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	BACKGROUND();

	changes += cdef->expr()->inlineFuncs(cdef, &cdef->expr());
	ASSERT_OK();
    }

    return changes;
}

// Detect self references
int VSLLib::countSelfReferences()
{
    int changes = 0;

    // Set all self_references to zero
    VSLDef *cdef;
    for (cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	VSLDefList *dflist = cdef->deflist;
	dflist->self_references = 0;
    }

    // Now go and count the self_references
    for (cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	BACKGROUND();

	changes += cdef->expr()->countSelfReferences(cdef, cdef->deflist);
	ASSERT_OK();
    }

    return changes;
}

// Delete unused local defs
int VSLLib::cleanup()
{
    int changes = 0;

    VSLDef *d = _last;
    while (d != 0)
    {
	BACKGROUND();

	VSLDefList *dflist = d->deflist;
	if (!dflist->global() && 
	    (dflist->references() == 0 || 
	     dflist->references() == dflist->self_references))
	{   
	    // Function is unused except its def: delete it

	    // move D until it is beyond DFLIST
	    while (d != 0 && d->deflist == dflist)
		d = d->libprev();

	    if (VSEFlags::show_optimize)
	    {
		for (VSLDef *e = dflist->first(); e != 0; e = e->listnext())
		    std::cout << "\n" << e->longname() << ": removed";
		std::cout.flush();
	    }

	    // Find pointer from predecessor and have it point to the successor
	    if (defs[dflist->hashcode] == dflist)
		defs[dflist->hashcode] = dflist->next();
	    else
	    {
		VSLDefList *prev;
		for (prev = defs[dflist->hashcode];
		    prev != 0 && prev->next() != dflist; prev = prev->next())
		    ;
		assert (prev->next() == dflist);
		prev->next() = dflist->next();
	    }

	    // Delete VSLDefs
	    dflist->replace();

	    // Delete VSLDeflist
	    dflist->next() = 0; dflist->destroy();
	    changes++;
	}
	else
	{
	    // Check next definition
	    d = d->libprev();
	}

	ASSERT_OK();
    }

    return changes;
}


// Optimization

// Main entry point
void VSLLib::optimize(unsigned mode)
{
    /* 
	The sequence of optimizations is:

	1. resolveDefs
	2. resolveSynonyms
	3. foldOps     <--------+
	4. foldConsts           | (until fixpoint is reached)
	5. inlineFuncs ---------+
	6. countSelfReferences

	Between two optimization steps, unused functions are deleted
	(cleanup).  This speeds up optimization.

	In general, the sequence of optimization steps is arbitrary.
	In practice, we have the following constraints:

	* resolveDefs is precondition for inlineFuncs and resolveSynonyms.

	* resolveSynonyms does a subset of inlineFuncs, but much
	  faster and less resource-intensive.  Thus, it should be
	  called before inlineFuncs.

	* foldOps und foldConsts work best on large, complex
	  expressions, as resulting from inlineFuncs.

	* foldConsts is quite time-consuming and should profit
	  from all earlier optimizations.

	These constraints determine the sequence as shown above.

	If only single optimizations are chosen: resolveSynonyms and
	foldOps require the least time, but also have the least
	effects; the library shrinks slightly and evaluation speeds up
	slightly.

	foldConsts requires much time; the library may become much
	smaller (constants are shared); evaluation time shrinks a lot.

	inlineFuncs also requires much time; the library
	becomes much larger and evaluation speeds up slightly.
    */

    if (mode & _Basics)
    {
	bind();
	resolveNames();
	compilePatterns();
    }

    if (mode & _Cleanup)
	cleanup();

    if (mode & _ResolveDefs)
	if (resolveDefs() > 0)
	    if (mode & _Cleanup)
		cleanup();

    if (mode & _ResolveSynonyms)
	if (resolveSynonyms() > 0)
	    if (mode & _Cleanup)
		cleanup();
	
    for (unsigned loop = 0; loop < (mode & loopMask); loop++)
    {
	int sum = 0;
	int changes;

	if (mode & _FoldOps)
	    if ((changes = foldOps()) > 0)
	    {
		sum += changes;
		compilePatterns();
	    }

	if (mode & _FoldConsts)
	    if ((changes = foldConsts()) > 0)
	    {
		sum += changes;
		if (mode & _Cleanup)
		    cleanup();
	    }

	if (mode & _InlineFuncs)
	    if ((changes = inlineFuncs()) > 0)
	    {
		sum += changes;
		if (mode & _Cleanup)
		    cleanup();
	    }

	if (sum == 0)
	    break;      // No more changes
    }

    if (mode & _CountSelfReferences)
	if (countSelfReferences() > 0)
	    if (mode & _Cleanup)
		cleanup();
}




// Debugging

// Dump
std::ostream& operator << (std::ostream& s, const VSLLib& lib)
{
    s << "// " << lib._lib_name << "\n\n";

    VSLDef *d = lib._first;
    if (VSEFlags::dump_last)
	d = lib._last;

    for (; d != 0; d = d->libnext())
    {
	// Dump comment
	s << "// " << d->longname() << " (hashcode: "
	    << d->deflist->hashcode << ") ";
	    
	if (d->deflist->references() == 0)
	    s << "(unused) ";
	else
	    s << "(used " << d->deflist->references() << " times) ";

	if (d->straight())
	    s << "(straight)\n";
	else
	    s << "(not straight)\n";

	// Function name (with args)
	s << d->f_name();

	if (d->expr() != 0)
	{
	    if (d->deflist->global())
		s << " -> ";
	    else
		s << " = ";
	    s.flush();

	    // Definition
	    s << *(d->expr());
	}

	s << ";\n\n"; 
	s.flush();
    }

    return s;
}


// ...as tree
void VSLLib::dumpTree(std::ostream& s) const
{
    bool old = StringBox::quoted;
    StringBox::quoted = true;

    s << "#!vsl\n\n// " << _lib_name << "\n\n";
    s << "#include <vsl.vsl>\n\n";

    s << "main(_...) -> VSLLib(\"" << _lib_name << "\",\n\n";

    VSLDef *d = _first;
    if (VSEFlags::dump_last)
	d = _last;

    for (; d != 0; d = d->libnext())
    {
	// Comment
	s << "// " << d->longname() << " (hashcode: "
	    << d->deflist->hashcode << ") ";
	    
	if (d->deflist->references() == 0)
	    s << "(unused) ";
	else
	    s << "(used " << d->deflist->references() << " times) ";

	if (d->straight())
	    s << "(straight)\n";
	else
	    s << "(not straight)\n";


	// Header
	s << "VSLDef(";
	
	// 1st parameter: name and location (as text)
	s << "\"" << d->longname() << "\"";
	s << ", ";

	// 2nd parameter: pattern (as text)
	if (d->node_pattern() != 0)
	    s << "\"" << *d->node_pattern() << "\"";
	else
	    s << "NoPatternText()";
	s << ", ";

	// 3rd parameter: definition (as text)
	if (d->expr() != 0)
	    s << "\"" << *d->expr() << "\"";
	else
	    s << "NoDefText()";
	s << ", ";

	// 4th parameter: pattern (as VSL expr)
	if (d->node_pattern() != 0)
	    d->node_pattern()->dumpTree(s);
	else
	    s << "NoPatternTree()";
	s << ", ";

	// 5th parameter: definition (as VSL expr)
	if (d->expr() != 0)
	    d->expr()->dumpTree(s);
	else
	    s << "NoDefTree()";
	s << ")";

	if (d->libnext() != 0)
	    s << ",\n\n";
    }

    s << "\n);\n";

    StringBox::quoted = old;
}


// Representation invariant
bool VSLLib::OK() const
{
#if !NDEBUG
    // Loop #1: over hash table
    for (int i = 0; i < hashSize; i++)
	for (VSLDefList *dl = defs[i]; dl != 0; dl = dl->next())
	{
	    // Check pointer to lib
	    assert (dl->lib == this);

	    // Check Deflist
	    assert (dl->OK());
	}

    // Loop #2: over internal list
    VSLDef *last_d = 0;
    (void) last_d;		// Use it
    for (VSLDef *d = _first; d != 0; d = d->libnext())
    {
	last_d = d;
	assert (d->OK());
    }

    assert (_last == last_d);
#endif

    return true;
}
