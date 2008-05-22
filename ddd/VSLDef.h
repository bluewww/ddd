// $Id$
// VSL definition

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

#ifndef _DDD_VSLDef_h
#define _DDD_VSLDef_h

#include <limits.h>
#include <iostream>
#include "strclass.h"
#include "bool.h"
#include "mutable.h"
#include "VSLNode.h"
#include "TypeInfo.h"
#include "assert.h"

class Box;
class VSLDefList;

class VSLDef {
public:
    DECLARE_TYPE_INFO

private:
    VSLNode *_expr;             // Expr (definition body)
    VSLNode *_node_pattern;     // Pattern
    mutable Box *_box_pattern;          // Compiled pattern

    unsigned _nargs;            // Number of args
    bool _straight;		// Flag: Can we use arg list `as is'?

    string _filename;           // Position of definition
    int _lineno;

    VSLDef *_listnext;          // next definition in VSLDefList
    VSLDef *_libnext;           // next definition in VSLLib
    VSLDef *_libprev;           // previous definition in VSLLib

    string args() const;        // Create argument list

    mutable bool being_compiled;	// Protect against recursive compilePattern()

public:
    VSLDefList *deflist;        // Parent

    // Constructor
    VSLDef(VSLDefList* l, VSLNode *pattern, VSLNode *e = 0,
	   const string& filename = "builtin", int lineno = 0);

private:
    // Copy constructor
    VSLDef(const VSLDef&);

    // `Dummy' assignment
    VSLDef& operator = (const VSLDef&);

public:
    // Resources
    VSLNode*& expr()            { return _expr; }
    VSLNode*& node_pattern()    { return _node_pattern; }
    Box*& box_pattern()         { return _box_pattern; }

    VSLNode* expr() const           { return _expr; }
    VSLNode* node_pattern() const   { return _node_pattern; }
    Box* box_pattern() const        { return _box_pattern; }

    unsigned nargs() const      { return _nargs; }
    bool straight() const       { return _straight; }

    VSLDef*& listnext()         { return _listnext; }
    VSLDef*& libnext()          { return _libnext; }
    VSLDef*& libprev()          { return _libprev; }

    VSLDef* listnext() const    { return _listnext; }
    VSLDef* libnext() const     { return _libnext; }
    VSLDef* libprev() const     { return _libprev; }

    string func_name() const;     // internal name (including args)
    string f_name() const;        // external name (including args)
    string longname() const;      // external name (including args and loc)

    const string& filename() const     { return _filename; }
    int lineno() const          { return _lineno; }

    // Evaluate
    const Box *eval(Box *arg) const;

    // Backtrace (in error handling)
    static const VSLDef **backtrace;
    static const Box **backtrace_args;

    // Create box_pattern
    void compilePattern() const;

    // Destroy box_pattern
    void uncompilePattern() const
    {
	if (_box_pattern) 
	    MUTABLE_THIS(VSLDef *)->_box_pattern->unlink();
	MUTABLE_THIS(VSLDef *)->_box_pattern = 0;
    }

    // Resolve names
    int resolveNames();

    // Check if def matches ARG
    bool matches(const Box *arg) const;
    bool matches(const VSLNode *arg) const;

    // Convert arg into argument list
    ListBox *arglist(const Box *arg) const;

    // Extract instances
    VSLNode **nodelist(const VSLNode *arg) const;

    // Destructor
    virtual ~VSLDef();

    // Representation invariant
    virtual bool OK() const;
};

#endif
