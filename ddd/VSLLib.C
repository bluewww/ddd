// $Id$
// The VSL Library

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char VSLLib_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


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
VSLLib::VSLLib(const string& lib_name, unsigned optimize)
    : _lib_name(lib_name), _first(0), _last(0)
{
    initHash();
    read(lib_name, optimize);
}


// Init VSL library and read from stream
VSLLib::VSLLib(istream& i, unsigned optimize)
    : _lib_name(""), _first(0), _last(0)
{
    initHash();
    read(i, optimize);
}


// Return list of defs for FUNC_NAME; 0 if not found
VSLDefList* VSLLib::deflist(const string& func_name) const
{
    unsigned hashcode = hashpjw(func_name) % hashSize;

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
	unsigned hashcode = hashpjw(func_name) % hashSize;

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

    return d ? d->eval(a) : 0;
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
			VSLArg arg0,
			VSLArg arg1,
			VSLArg arg2,
			VSLArg arg3,
			VSLArg arg4,
			VSLArg arg5,
			VSLArg arg6,
			VSLArg arg7,
			VSLArg arg8,
			VSLArg arg9) const
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
VSLLib::~VSLLib()
{
    for (int i = 0; i < hashSize; i++)
	if (defs[i] != 0)
	    delete defs[i];
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
	    (dflist->references == 0 || 
	     dflist->references == dflist->self_references))
	{   
	    // Function is unused except its def: delete it

	    // move D until it is beyond DFLIST
	    while (d != 0 && d->deflist == dflist)
		d = d->libprev();

	    if (VSEFlags::show_optimize)
	    {
		for (VSLDef *e = dflist->first(); e != 0; e = e->listnext())
		    cout << "\n" << e->longname() << ": removed";
		cout.flush();
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
	    dflist->next() = 0; delete dflist;
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

// Processor
void VSLLib::process(unsigned mode)
{
    /* 
	The sequence of optimizations is:

	1. resolveDefs
	2. resolveSynonyms
	3. foldOps     <--------+
	4. foldConsts           | (*)
	5. inlineFuncs ---------+
	6. countSelfReferences

	((*): until no more changes occur)

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

    bind();
    resolveNames();
    compilePatterns();

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
ostream& operator << (ostream& s, const VSLLib& lib)
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
	    
	if (d->deflist->references == 0)
	    s << "(unused) ";
	else
	    s << "(used " << d->deflist->references << " times) ";

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
void VSLLib::dumpTree(ostream& s) const
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
	    
	if (d->deflist->references == 0)
	    s << "(unused) ";
	else
	    s << "(used " << d->deflist->references << " times) ";

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
    // Loop #1: over hash table
    for (int i = 0; i < hashSize; i++)
	if (defs[i] != 0)
	{
	    // Check pointer to lib
	    assert (defs[i]->lib == this);

	    // Check Deflist
	    assert (defs[i]->OK());
	}

    // Loop #2: over internal list
    for (VSLDef *d = _first; d != 0; d = d->libnext())
	assert (d->OK());

    return true;
}
