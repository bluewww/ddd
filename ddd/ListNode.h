// $Id$
// Deklaration Klasse ListNode

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

#ifndef _Nora_ListNode_h
#define _Nora_ListNode_h

#ifdef __GNUG__
#pragma interface
#endif


// Ein ListNode fasst andere Knoten k_i zu Listen zusammen. Zur Laufzeit
// erzeugt ein ListNode eine ListBox mit den Ergebnissen der k_i.


#include "assert.h"
#include <iostream.h>
#include "misc.h"       // max()

#include "VSLNode.h"


// ListNode

class ListNode: public VSLNode {
public:
    DECLARE_TYPE_INFO

private:
    VSLNode *_head;
    VSLNode *_tail;

protected:
    ListNode(const ListNode& node):
	VSLNode(node),
	_head(node._head->dup()),
	_tail(node._tail->dup())
    {}

    bool matches(const VSLNode& node) const
    {
	return VSLNode::matches(node) &&
	    *_head == *(((ListNode *)&node)->_head) && 
	    *_tail == *(((ListNode *)&node)->_tail);    // dirty trick
    }

public:
    ListNode(VSLNode *hd, VSLNode *tl, char *type = "ListNode"):
	VSLNode(type), _head(hd), _tail(tl)
    {}

    VSLNode *dup() const { return new ListNode(*this); }

    // ListNode zerstoeren: inklusive Argumenten!
    ~ListNode()
    {
	if (_head) delete _head;
	if (_tail) delete _tail;
    }

    // Ressourcen
    VSLNode*& head() { return _head; }
    VSLNode*& tail() { return _tail; }
    VSLNode* head() const { return _head; }
    VSLNode* tail() const { return _tail; }

    const Box *_eval(ListBox *arglist) const;

    // Optimierung
    int countSelfReferences(VSLDef *cdef, VSLDefList *deflist);
    int resolveDefs(VSLDef *cdef, bool complain_recursive);
    int resolveSynonyms(VSLDef *cdef, VSLNode **node);
    int foldOps(VSLDef *cdef, VSLNode **node);
    int foldConsts(VSLDef *cdef, VSLNode **node);
    int inlineFuncs(VSLDef *cdef, VSLNode **node);
    int instantiateArgs(VSLDef *cdef, VSLNode **node, VSLNode *values[],
	unsigned base, unsigned n);
    void countArgNodes(VSLDef *cdef, int instances[],
	unsigned base, unsigned n);
    int _reBase(VSLDef *cdef, unsigned newBase);

    // Sonstige Baum-Funktionen
    void compilePatterns(VSLDef *cdef) const;
    void uncompilePatterns(VSLDef *cdef) const;
    int resolveName(VSLDef *cdef, VSLNode **node, string& name, unsigned id);
    int _resolveNames(VSLDef *cdef, unsigned base);
    string firstName() const;

    // Quantoren
    bool exists(VSLNodeFunc func);
    bool forAll(VSLNodeFunc func);

    // Liste anhaengen
    int append(VSLNode *node);

    // Namen ausgeben
    void dump(ostream& s) const;
    void _dumpTree(ostream& s) const;

    bool isConst() const;
    bool isListNode() const { return true; }

    bool isStraight() const;

    unsigned nargs() const;

    // Repraesentations-Invariante
    bool OK() const;
};

#endif
