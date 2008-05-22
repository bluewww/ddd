// $Id$
// VSL Nodes

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_VSLNode_h
#define _DDD_VSLNode_h

// A VSLNode is a node of an evaluation tree that represents the
// expressions of a VSLL library.


#include "assert.h"
#include <iostream>
#include <string.h>
#include "TypeInfo.h"

#include "VSLBuiltin.h"

class Box;
class ListBox;
class VSLDefList;
class VSLDef;
class VSLNode;

// VSLNode

class VSLNode {
public:
    DECLARE_TYPE_INFO

private:
    const char *_type;    // Type

protected:
    unsigned _base; // Number of arguments in context

    // Flag: are side effects allowed?
    static bool sideEffectsProhibited;

    // Flag: have side effects been observed?
    static bool sideEffectsOccured;

    // Copy
    VSLNode(const VSLNode& node):
	_type(node._type),
	_base(node._base)
    {}

    // Dump
    virtual void dump(std::ostream& s) const = 0;      // as VSL epxr
    virtual void _dumpTree(std::ostream&) const {}     // as tree

    virtual bool matches(const VSLNode& node) const
    {
	return strcmp(_type, node._type) == 0;
    }
private:
    VSLNode& operator = (const VSLNode &);

public:
    // Constructor
    VSLNode(const char *type = "VSLNode")
	: _type(type), _base(0)
    {}

    // Default destructor
    virtual ~VSLNode()
    {
	_type = 0;  // Protect against further references
    }

    // Copy
    virtual VSLNode *dup() const = 0;

    // Evaluate
    virtual const Box *eval(ListBox *arglist) const;
    virtual const Box *_eval(ListBox *arglist) const = 0;

    // Optimize (default: nop)
    virtual int resolveDefs(VSLDef *, bool = true)      { return 0; }
    virtual int resolveSynonyms(VSLDef *, VSLNode **)   { return 0; }
    virtual int foldOps(VSLDef *, VSLNode **)           { return 0; }
    virtual int foldConsts(VSLDef *, VSLNode **)        { return 0; }
    virtual int countSelfReferences(VSLDef *, 
	VSLDefList *)                                   { return 0; }
    virtual int inlineFuncs(VSLDef *, VSLNode **)       { return 0; }
    virtual int instantiateArgs(VSLDef *, VSLNode **,
	VSLNode **, unsigned , unsigned)                { return 0; }
    virtual void countArgNodes(VSLDef *, int[],
	unsigned , unsigned )                           { return; }
    virtual int _reBase(VSLDef *, unsigned )            { return 0; }
    int reBase(VSLDef *cdef, unsigned newBase) 
    { 
	int changes = _reBase(cdef, newBase);
	_base = newBase;
	return changes;
    }

    // Other tree ops (default: nop)
    virtual void compilePatterns(VSLDef *) const            { return; }
    virtual void uncompilePatterns(VSLDef *) const          { return; }
    virtual int resolveName(VSLDef *, VSLNode **, 
	const string& , unsigned)                           { return 0; }
    virtual int _resolveNames(VSLDef *, unsigned)           { return 0; }
    int resolveNames(VSLDef *cdef, unsigned base)
    {
	_base = base;
	return _resolveNames(cdef, base);
    }
    virtual void rebind(const class VSLLib *) { return; }
    

    virtual const string& firstName() const          { return _nilstring; }

    // Check type
    virtual bool isConst() const = 0;

    virtual bool isArgNode() const         { return false; }
    virtual bool isBuiltinCallNode() const { return false; }
    virtual bool isCallNode() const        { return false; }
    virtual bool isConstNode() const       { return false; }
    virtual bool isDefCallNode() const     { return false; }
    virtual bool isDummyNode() const       { return false; }
    virtual bool isLetNode() const         { return false; }
    virtual bool isListNode() const        { return false; }
    virtual bool isNameNode() const        { return false; }
    virtual bool isTestNode() const        { return false; }

    virtual bool isStraight() const        { return false; }

    // # of NameNodes
    virtual unsigned nargs() const            { return 0; }

    // Match functions
    static bool bothSidesCanMatch;
    bool operator == (const VSLNode& node) const;
    bool operator != (const VSLNode& node) const
    { 
	return !(operator == (node)); 
    }

    // Representation invariant
    virtual bool OK() const;

    // Dump
    friend std::ostream& operator << (std::ostream& s, const VSLNode& node); // als VSL
    void dumpTree(std::ostream& s) const;                               // als Baum
};

#endif
