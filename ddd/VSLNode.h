// $Id$
// Deklaration Klasse VSLNode

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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

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
    virtual void _dumpTree(ostream&) const {}     // als Baum (default: nichts)

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

    // Sonstige Baum-Operationen (Default: nichts tun)
    virtual void compilePatterns(VSLDef *) const            { return; }
    virtual void uncompilePatterns(VSLDef *) const          { return; }
    virtual int resolveName(VSLDef *, VSLNode **, 
	string& , unsigned)                                 { return 0; }
    virtual int _resolveNames(VSLDef *, unsigned)           { return 0; }
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
