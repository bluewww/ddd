// $Id$
// Implementation Klasse ListNode

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

char ListNode_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include "return.h"
#include <iostream.h>
#include <misc.h>       // max()

#include "VSLLib.h"

#include "ListBox.h"
#include "VSLNode.h"
#include "ListNode.h"
#include "ArgNode.h"
#include "TrueNode.h"
#include "VSLDef.h"

#include "VSEFlags.h"

DEFINE_TYPE_INFO_1(ListNode, VSLNode)

// ListNode

// Liste auswerten
const Box *ListNode::_eval(ListBox *arglist) const
{
    const Box *hd = _head->eval(arglist);
    if (hd == 0)
	return 0;

    const Box *tl = _tail->eval(arglist);
    if (tl == 0)
    {
	((Box *)hd)->unlink();
	return 0;
    }

    ListBox *ret = 0;
    if (tl->isListBox())
	ret = new ListBox((Box *)hd, (ListBox *)tl);
    else
	VSLLib::eval_error("atom as argument of a list");

    ((Box *)hd)->unlink();
    ((Box *)tl)->unlink();
    return ret;
}

// Liste ausgeben
void ListNode::dump(ostream &s) const
{
    if (VSEFlags::include_list_info)
    {
	// Liste formal ausgeben
	s << "[" << *head() << "|" << *tail() << "]";
    }
    else
    {
	// Etwas geschickter formatieren

	EmptyListNode *empty = new EmptyListNode;

	s << "(";

	const VSLNode *node = this; 
	while (node)
	{
	    if (node->isListNode())
	    {
		if (node != this)
		    s << ", ";

		ListNode *list = (ListNode *)node;
		s << *(list->head());
		node = list->tail();
	    }
	    else
	    {
		if (*node != *empty)
		{
		    if (node != this)
			s << ", ";

		    s << *node << "...";
		}
		node = 0;
	    }
	}

	s << ")";

	delete empty;
    }
}

// ...in Baum-Notation
void ListNode::_dumpTree(ostream& s) const
{
    if (VSEFlags::include_list_info)
    {
	// Liste formal ausgeben
	head()->dumpTree(s);
	s << ",";
	tail()->dumpTree(s);
    }
    else
    {
	// Etwas geschickter formulieren

	EmptyListNode *empty = new EmptyListNode;

	const VSLNode *node = this; 
	while (node)
	{
	    if (node->isListNode())
	    {
		if (node != this)
		    s << ", ";

		ListNode *list = (ListNode *)node;
		list->head()->dumpTree(s);
		node = list->tail();
	    }
	    else
	    {
		if (*node != *empty)
		{
		    if (node != this)
			s << ", ";
		    node->dumpTree(s);
		}
		node = 0;
	    }
	}

	delete empty;
    }
}


// Pruefen, ob Funktionsausdruck konstant
bool ListNode::isConst() const
{
    return _head->isConst() && _tail->isConst();
}

// #NameNodes zurueckgeben
unsigned ListNode::nargs() const
{
    return _head->nargs() + _tail->nargs();
}

// Pruefen, ob Argumentliste direkt uebernommen werden kann
bool ListNode::isStraight() const
{
    // Pruefen, ob Liste der Form [arg | tail]
    return ( _head->isArgNode() || _head->isNameNode() )
	&& _tail->isStraight();
}


// Liste anhaengen
int ListNode::append(VSLNode *list)
{
    // Ende der Liste suchen
    ListNode *nprev = this;
    VSLNode *node = tail();
    while (node->isListNode())
    {
	nprev = (ListNode *)node; 
	node = nprev->tail();
    }

    // Wenn letztes Listenelement != [], Abbruch (kein Anhaengen moeglich)
    EmptyListNode empty;
    if (*node != empty)
	return -1;

    // [] durch list ersetzen
    delete nprev->tail();
    nprev->tail() = list;

    return 0;
}



// Optimierung

// Im Allgemeinen: Auf Kopf und Schwanz der Liste ausfuehren.
int ListNode::resolveDefs(VSLDef *cdef, bool complain_recursive)
{
    int changes = 0;

    // Auf gesamter Liste ausfuehren
    changes += head()->resolveDefs(cdef, complain_recursive);
    changes += tail()->resolveDefs(cdef, complain_recursive);

    return changes;
}

int ListNode::resolveSynonyms(VSLDef *cdef, VSLNode ** /* node */)
{
    int changes = 0;

    // assert (this == *node);

    // Auf gesamter Liste ausfuehren
    changes += head()->resolveSynonyms(cdef, &head());
    changes += tail()->resolveSynonyms(cdef, &tail());

    return changes;
}

int ListNode::foldOps(VSLDef *cdef, VSLNode ** /* node */)
{
    int changes = 0;

    // assert (this == *node);

    // Auf gesamter Liste ausfuehren
    changes += head()->foldOps(cdef, &head());
    changes += tail()->foldOps(cdef, &tail());

    return changes;
}

int ListNode::inlineFuncs(VSLDef *cdef, VSLNode ** /* node */)
{
    int changes = 0;

    // assert (this == *node);

    // Auf gesamter Liste ausfuehren
    changes += head()->inlineFuncs(cdef, &head());
    changes += tail()->inlineFuncs(cdef, &tail());

    return changes;
}

int ListNode::instantiateArgs(VSLDef *cdef, VSLNode ** /* node */,
			      VSLNode *values[], unsigned base, unsigned n)
{
    int changes = 0;

    // assert (this == *node);

    // Auf gesamter Liste ausfuehren
    changes += head()->instantiateArgs(cdef, &head(), values, base, n);
    changes += tail()->instantiateArgs(cdef, &tail(), values, base, n);

    return changes;
}

void ListNode::countArgNodes(VSLDef *cdef, int instances[], unsigned base,
    unsigned n)
{
    // Auf gesamter Liste ausfuehren
    head()->countArgNodes(cdef, instances, base, n);
    tail()->countArgNodes(cdef, instances, base, n);
}

void ListNode::compilePatterns(VSLDef *cdef) const
{
    // Auf gesamter Liste ausfuehren
    head()->compilePatterns(cdef);
    tail()->compilePatterns(cdef);
}

void ListNode::uncompilePatterns(VSLDef *cdef) const
{
    // Auf gesamter Liste ausfuehren
    head()->uncompilePatterns(cdef);
    tail()->uncompilePatterns(cdef);
}

int ListNode::countSelfReferences(VSLDef *cdef, VSLDefList *deflist)
{
    int changes = 0;

    // Auf gesamter Liste ausfuehren
    changes += head()->countSelfReferences(cdef, deflist);
    changes += tail()->countSelfReferences(cdef, deflist);

    return changes;
}

int ListNode::resolveName(VSLDef *cdef, VSLNode **/* node */, string& name,
    unsigned id)
{
    int changes = 0;

    // assert (this == *node);

    // Auf gesamter Liste ausfuehren
    changes += head()->resolveName(cdef, &head(), name, id);
    changes += tail()->resolveName(cdef, &tail(), name, id);

    return changes;
}

int ListNode::_resolveNames(VSLDef *cdef, unsigned base)
{
    int changes = 0;

    // Auf gesamter Liste ausfuehren
    changes += head()->resolveNames(cdef, base);
    changes += tail()->resolveNames(cdef, base);

    return changes;
}

int ListNode::_reBase(VSLDef *cdef, unsigned newBase)
{
    int changes = 0;

    // Auf gesamter Liste ausfuehren
    changes += head()->reBase(cdef, newBase);
    changes += tail()->reBase(cdef, newBase);

    return changes;
}

string ListNode::firstName() const RETURNS(s)
{
    RETURN_OBJECT(string, s);
    s = head()->firstName();
    if (s == "")
	s = tail()->firstName();
    RETURN(s);
}


// foldConsts: Funktionen mit konstanten Argumenten sofort auswerten
// Etwa: f(2 + 2) durch f(4) ersetzen.

int ListNode::foldConsts(VSLDef *cdef, VSLNode **node)
{
    int changes = 0;

    assert (this == *node);

    // Auf gesamter Liste ausfuehren
    changes += head()->foldConsts(cdef, &head());
    changes += tail()->foldConsts(cdef, &tail());

    // Pruefen, ob Liste jetzt konstant;
    // wenn nicht, Abbruch
    if (!isConst())
	return changes;

    // Sonst: Liste auswerten (hierbei Seiteneffekte vermeiden)

    sideEffectsProhibited = true;
    sideEffectsOccured = false;

    const Box *result = eval(0);

    if (result)
    {
	assert(!sideEffectsOccured);

	if (VSEFlags::show_optimize)
	{
	    cout << "\n" << cdef->longname() << ": foldConsts: replacing\n" 
		<< *this << '\n';
	    cout.flush();
	}

	// CallNode durch Konstante ersetzen
	*node = new ConstNode((Box *)result);

	if (VSEFlags::show_optimize)
	{
	    cout << "by " << **node << "\n";
	    cout.flush();
	}
	
	changes++;

	// ListNode und daranhaengenden Teilbaum loeschen
	delete this;
    }

    sideEffectsProhibited = false;

    return changes;
}


// Debugging

// Pruefen, ob alles in Ordnung
bool ListNode::OK() const
{
    assert (VSLNode::OK());

    assert (_head && _head->OK());
    assert (_tail && _tail->OK());

    return true;
}
