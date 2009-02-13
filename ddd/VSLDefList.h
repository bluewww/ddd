// $Id$
// List of VSLDefs

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000 Universitaet Passau, Germany.
// Copyright (C) 2003 Free Software Foundation, Inc.
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

#ifndef _DDD_VSLDefList_h
#define _DDD_VSLDefList_h

#include "assert.h"
#include <limits.h>
#include <iostream>
#include "strclass.h"
#include "bool.h"
#include "TypeInfo.h"

class VSLLib;
class VSLDef;
class VSLNode;

class VSLDefList {
public:
    DECLARE_TYPE_INFO

private:
    string _func_name;      // Function name
    VSLDef *_first;         // first definition for this name
    VSLDef *_last;          // last definition for this name
    unsigned _ndefs;        // Number of definitions
    VSLDefList *_next;      // Next function name
    bool _global;	    // True if global
    bool _destroyed;        // True if destroy() has been called.

public:
    VSLLib *lib;            // Library of this node
    unsigned hashcode;      // Index into hash table of library

private:
    int _references;        // #occurrences in VSLNode's
public:
    int self_references;    // #occurrences in own defs (-1: unknown)

    // Constructor
    VSLDefList(VSLLib* l, unsigned hash, 
	const string& func_nm, bool g = false)
	: _func_name(func_nm), _first(0), _last(0), _ndefs(0),
	_next(0), _global(g), _destroyed(false), lib(l), hashcode(hash), 
	_references(0), self_references(-1)
    {}


private:
    VSLDefList(const VSLDefList&);

    VSLDefList& operator = (const VSLDefList&);

protected:
    virtual ~VSLDefList();
public:
    // Add new function
    VSLDef *add(bool& newFlag,
		VSLNode *pattern, VSLNode *expr = 0,
		const string& filename = "builtin", int lineno = 0);

    // Get a function def
    VSLDef *def(Box *arglist) const;
    VSLDef *firstdef() const { assert (_ndefs == 1); return _first; }

    // Duplicate
    VSLDefList *dup(const VSLLib *lib) const;

    // Resources
    const string& func_name() const { return _func_name; }
    bool &global()        { return _global; }
    bool global() const   { return _global; }
    bool destroyed() const { return _destroyed; }

    // Reference counting
    int references() const { return _references; }
    void inc() { ++_references; }
    void dec();

    string f_name() const
    {
        return
	  (_func_name[0] == '#') ?
	  _func_name.from(1):
	  _func_name;
    }

    VSLDef* first()     { return _first; }
    VSLDefList*& next() { return _next; }
    unsigned ndefs() const { return _ndefs; }

    // Evaluate
    const Box *eval(Box *arg) const;

    // Rename
    void override();

    // Delete defs
    void replace();

    // Destroy
    void destroy();

    // Representation invariant
    virtual bool OK() const;
};

#endif
