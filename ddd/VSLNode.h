// $Id$
// Deklaration Klasse VSLNode

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

#ifndef _Nora_VSLNode_h
#define _Nora_VSLNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein VSLNode ist ein Knoten eines Auswertungsbaumes, der die Ausdruecke
// der VSLLib repraesentiert.


#include "assert.h"
#include <iostream.h>
#include <string.h>
#include "TypeInfo.h"

#include "VSLBuiltin.h"

class Box;
class ListBox;
class VSLDefList;
class VSLDef;
class VSLNode;

typedef bool (VSLNode::*VSLNodeFunc)();

// VSLNode

class VSLNode {
public:
    DECLARE_TYPE_INFO

private:
    char *_type;    // Typ

protected:
    unsigned _base; // Anzahl Argumente im Kontext

    // Flag: Seiteneffekte zugelassen?
    static bool sideEffectsProhibited;

    // Flag: Seiteneffekte aufgetreten?
    static bool sideEffectsOccured;

    // Knoten kopieren
    VSLNode(const VSLNode& node):
	_type(node._type),
	_base(node._base)
    {}

    // Knoten ausgeben
    virtual void dump(ostream& s) const = 0;      // als VSL-Ausdruck
    virtual void _dumpTree(ostream& s) const {}   // als Baum (default: nichts)

    virtual bool matches(const VSLNode& node) const
    {
	return strcmp(_type, node._type) == 0;
    }

public:
    // Knoten erzeugen
    VSLNode(char *type = "VSLNode"):
	_type(type),
	_base(0)
    {}

    // Default-Destruktor
    virtual ~VSLNode()
    {
	_type = 0;  // Schutz gegen weitere Referenzen
    }

    // Knoten kopieren
    virtual VSLNode *dup() const = 0;

    // Knoten auswerten
    virtual const Box *eval(ListBox *arglist) const;
    virtual const Box *_eval(ListBox *arglist) const = 0;

    // Optimierungs-Funktionen (Default: nichts tun)
    virtual int resolveDefs(VSLDef *cdef, 
	bool complain_recursive = true)                      { return 0; }
    virtual int resolveSynonyms(VSLDef *cdef, VSLNode **node)   { return 0; }
    virtual int foldOps(VSLDef *cdef, VSLNode **node)           { return 0; }
    virtual int foldConsts(VSLDef *cdef, VSLNode **node)        { return 0; }
    virtual int countSelfReferences(VSLDef *cdef, 
	VSLDefList *deflist)                                    { return 0; }
    virtual int inlineFuncs(VSLDef *cdef, VSLNode **node)       { return 0; }
    virtual int instantiateArgs(VSLDef *cdef, VSLNode **node,
	VSLNode *values[], unsigned base, unsigned n)           { return 0; }
    virtual void countArgNodes(VSLDef *cdef, int instances[],
	unsigned base, unsigned n)                              { return; }
    virtual int _reBase(VSLDef *cdef, unsigned newBase)         { return 0; }
    int reBase(VSLDef *cdef, unsigned newBase) 
    { 
	int changes = _reBase(cdef, newBase);
	_base = newBase;
	return changes;
    }

    // Sonstige Baum-Operationen (Default: nichts tun)
    virtual void compilePatterns(VSLDef *cdef) const            { return; }
    virtual void uncompilePatterns(VSLDef *cdef) const          { return; }
    virtual int resolveName(VSLDef *cdef, VSLNode **node, 
	string& name, unsigned id)                              { return 0; }
    virtual int _resolveNames(VSLDef *cdef, unsigned base)      { return 0; }
    int resolveNames(VSLDef *cdef, unsigned base)
    {
	_base = base;
	return _resolveNames(cdef, base);
    }

    virtual string firstName() const          { return ""; }

    // Pruef-Funktionen
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

    // Quantoren ueber Praedikate
    virtual bool exists(VSLNodeFunc func)    { return func(); }
    virtual bool forAll(VSLNodeFunc func)    { return func(); }

    // #NameNode's zurueckgeben
    virtual unsigned nargs() const            { return 0; }

    // Match-Funktionen
    static bool bothSidesCanMatch;
    bool operator == (const VSLNode& node) const;
    bool operator != (const VSLNode& node) const
    { 
	return !(operator == (node)); 
    }

    // Repraesentations-Invariante
    virtual bool OK() const;

    // Baum ausgeben
    friend ostream& operator << (ostream& s, const VSLNode& node); // als VSL
    void dumpTree(ostream& s) const;                               // als Baum
};

#endif
