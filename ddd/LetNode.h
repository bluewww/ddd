// $Id$
// Deklaration Klasse LetNode

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

#ifndef _Nora_LetNode_h
#define _Nora_LetNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein LetNode implementiert ein let-in Konstrukt.
// Ein LetNode enthaelt eine Liste der Laenge 2. 
// Bei der Auswertung wird das erste Element der Liste gegen
// ein Pattern gematcht; die so gefundenen Variablen werden
// fuer die Auswertung des zweiten Elements verwendet.


#include "assert.h"
#include <iostream.h>

#include "VSLNode.h"
#include "ListNode.h"
#include "CallNode.h"
#include "TrueNode.h"



// LetNode

class LetNode: public CallNode {
public:
    DECLARE_TYPE_INFO

private:
    VSLNode *_node_pattern; // Pattern
    Box *_box_pattern;      // Compiliertes Pattern

    unsigned _nargs;        // Anzahl Argumente
    bool _straight;      // Flag: Argumentliste direkt uebernehmen?

    bool being_compiled; // Schutz gegen rekursive Pattern

    ListNode *_args() const      { return (ListNode *)arg(); }
    ListNode *_body() const      { return (ListNode *)(_args()->tail()); }

    ListBox *arglist(const Box *arg) const;       // Argumentliste bilden (Box)
    VSLNode **nodelist(const VSLNode *arg) const; // dito (Knoten)
    bool domatch(const Box *arg) const;        // Matching mit Box
    bool domatch(const VSLNode *arg) const;    // Matching mit Ausdruck

protected:
    void dump(ostream& s) const;
    void _dumpTree(ostream& s) const;

    LetNode(const LetNode& node):
	CallNode(node),
	_node_pattern(node._node_pattern->dup()),
	_box_pattern(0),
	_nargs(node._nargs),
	_straight(node._straight),
	being_compiled(false)
    {
	if (node._box_pattern)
	    _box_pattern = node._box_pattern->link();
    }

    // Dummy-Funktionen (werden nie aufgerufen)
    char *func_name() const       { assert(0); return "let"; }
    const Box *call(Box *) const  { assert(0); return 0; }

public:
    // LetNode erzeugen
    LetNode(VSLNode *p, VSLNode *a, VSLNode *b, char *type = "LetNode"):
	_node_pattern(p),
	_box_pattern(0),
	_nargs(p->nargs()),
	_straight(p->isStraight()),
	being_compiled(false),
	CallNode(new FixListNode(a, b), type)
    {}

    // LetNode zerstoeren
    ~LetNode()
    {
	if (_node_pattern) delete _node_pattern;
	if (_box_pattern) _box_pattern->unlink();
    }

    // Ressourcen
    VSLNode *&node_pattern()    { return _node_pattern; }
    Box *&box_pattern()         { return _box_pattern; }
    VSLNode *&args()            { return _args()->head(); }
    VSLNode *&body()            { return _body()->head(); }

    const VSLNode *node_pattern() const   { return _node_pattern; }
    const Box *box_pattern() const        { return _box_pattern; }
    const VSLNode *args() const           { return _args()->head(); }
    const VSLNode *body() const           { return _body()->head(); }

    // Compilierung
    void compilePatterns(VSLDef *cdef) const;

    // box_pattern zerstoeren
    void uncompilePatterns(VSLDef *cdef) const
    {
	CallNode::uncompilePatterns(cdef);

	if (_box_pattern) 
	    ((LetNode *)this)->_box_pattern->unlink();
	((LetNode *)this)->_box_pattern = 0;
    }

    // Namen aufloesen
    int _resolveNames(VSLDef *cdef, unsigned base);

    // Optimierung
    int inlineFuncs(VSLDef *cdef, VSLNode **node);
    int _reBase(VSLDef *cdef, unsigned newBase);

    // LetNode kopieren
    VSLNode *dup() const { return new LetNode(*this); }

    const Box *_eval(ListBox *arglist) const;
    
    bool isLetNode() const { return true; }

    // Repraesentations-Invariante
    bool OK() const;
};


// Ein WhereNode ist identisch mit einem LetNode,
// nur wird er in der Form "where" statt "let" ausgegeben.

class WhereNode: public LetNode {
public:
    DECLARE_TYPE_INFO

protected:
    void dump(ostream& s) const;

    // WhereNode kopieren
    WhereNode(WhereNode& node):
	LetNode(node)
    {}

public:
    // WhereNode erzeugen
    WhereNode(VSLNode *p, VSLNode *a, VSLNode *b):
	LetNode(p, a, b)
    {}
};

#endif
