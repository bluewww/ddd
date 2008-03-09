// $Id$
// ListNode class

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

char ListNode_rcsid[] = 
    "$Id$";

#include "assert.h"
#include <iostream>
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

// Constructor
ListNode::ListNode(VSLNode *hd, VSLNode *tl, const char *type)
    : VSLNode(type), _head(hd), _tail(tl)
{
    assert(hd != 0);
    assert(tl != 0);
}

ListNode::ListNode(const ListNode& node)
    : VSLNode(node), 
      _head(node._head->dup()), 
      _tail(node._tail->dup())
{}


// Evaluate
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

// Dump
void ListNode::dump(std::ostream &s) const
{
    if (VSEFlags::include_list_info)
    {
	// Formal list
	s << "[" << *head() << "|" << *tail() << "]";
    }
    else
    {
	// Somewhat cuter

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

// ...as tree
void ListNode::_dumpTree(std::ostream& s) const
{
    if (VSEFlags::include_list_info)
    {
	// Formal list
	head()->dumpTree(s);
	s << ",";
	tail()->dumpTree(s);
    }
    else
    {
	// Somewhat cuter

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

// Return number of NameNodes
unsigned ListNode::nargs() const
{
    return _head->nargs() + _tail->nargs();
}

// Check if argument list can be used `as is'
bool ListNode::isStraight() const
{
    // Check if list has the form [arg | tail]
    return ( _head->isArgNode() || _head->isNameNode() )
	&& _tail->isStraight();
}


// Append to list
int ListNode::append(VSLNode *list)
{
    // Search end of list
    ListNode *nprev = this;
    VSLNode *node = tail();
    while (node->isListNode())
    {
	nprev = (ListNode *)node; 
	node = nprev->tail();
    }

    // If last element != [], abort (cannot append)
    EmptyListNode empty;
    if (*node != empty)
	return -1;

    // Replace [] by LIST
    delete nprev->tail();
    nprev->tail() = list;

    return 0;
}



// Optimization

// In general: Apply optimization to head and tail of the list

int ListNode::resolveDefs(VSLDef *cdef, bool complain_recursive)
{
    int changes = 0;

    // Apply to entire list
    changes += head()->resolveDefs(cdef, complain_recursive);
    changes += tail()->resolveDefs(cdef, complain_recursive);

    return changes;
}

int ListNode::resolveSynonyms(VSLDef *cdef, VSLNode ** /* node */)
{
    int changes = 0;

    // assert (this == *node);

    // Apply to entire list
    changes += head()->resolveSynonyms(cdef, &head());
    changes += tail()->resolveSynonyms(cdef, &tail());

    return changes;
}

int ListNode::foldOps(VSLDef *cdef, VSLNode ** /* node */)
{
    int changes = 0;

    // assert (this == *node);

    // Apply to entire list
    changes += head()->foldOps(cdef, &head());
    changes += tail()->foldOps(cdef, &tail());

    return changes;
}

int ListNode::inlineFuncs(VSLDef *cdef, VSLNode ** /* node */)
{
    int changes = 0;

    // assert (this == *node);

    // Apply to entire list
    changes += head()->inlineFuncs(cdef, &head());
    changes += tail()->inlineFuncs(cdef, &tail());

    return changes;
}

int ListNode::instantiateArgs(VSLDef *cdef, VSLNode ** /* node */,
			      VSLNode *values[], unsigned base, unsigned n)
{
    int changes = 0;

    // assert (this == *node);

    // Apply to entire list
    changes += head()->instantiateArgs(cdef, &head(), values, base, n);
    changes += tail()->instantiateArgs(cdef, &tail(), values, base, n);

    return changes;
}

void ListNode::countArgNodes(VSLDef *cdef, int instances[], unsigned base,
    unsigned n)
{
    // Apply to entire list
    head()->countArgNodes(cdef, instances, base, n);
    tail()->countArgNodes(cdef, instances, base, n);
}

void ListNode::compilePatterns(VSLDef *cdef) const
{
    // Apply to entire list
    head()->compilePatterns(cdef);
    tail()->compilePatterns(cdef);
}

void ListNode::uncompilePatterns(VSLDef *cdef) const
{
    // Apply to entire list
    head()->uncompilePatterns(cdef);
    tail()->uncompilePatterns(cdef);
}

int ListNode::countSelfReferences(VSLDef *cdef, VSLDefList *deflist)
{
    int changes = 0;

    // Apply to entire list
    changes += head()->countSelfReferences(cdef, deflist);
    changes += tail()->countSelfReferences(cdef, deflist);

    return changes;
}

int ListNode::resolveName(VSLDef *cdef, VSLNode **/* node */, const string& name,
    unsigned id)
{
    int changes = 0;

    // assert (this == *node);

    // Apply to entire list
    changes += head()->resolveName(cdef, &head(), name, id);
    changes += tail()->resolveName(cdef, &tail(), name, id);

    return changes;
}

int ListNode::_resolveNames(VSLDef *cdef, unsigned base)
{
    int changes = 0;

    // Apply to entire list
    changes += head()->resolveNames(cdef, base);
    changes += tail()->resolveNames(cdef, base);

    return changes;
}

int ListNode::_reBase(VSLDef *cdef, unsigned newBase)
{
    int changes = 0;

    // Apply to entire list
    changes += head()->reBase(cdef, newBase);
    changes += tail()->reBase(cdef, newBase);

    return changes;
}

void ListNode::rebind(const class VSLLib *lib)
{
    // Apply to entire list
    head()->rebind(lib);
    tail()->rebind(lib);
}

const string& ListNode::firstName() const
{
    const string& s = head()->firstName();
    return
      s.empty() ?
      tail()->firstName() :
      s;
}


// foldConsts: Evaluate functions with constant args now

int ListNode::foldConsts(VSLDef *cdef, VSLNode **node)
{
    int changes = 0;

    assert (this == *node);

    // Apply to entire list
    changes += head()->foldConsts(cdef, &head());
    changes += tail()->foldConsts(cdef, &tail());

    // If list is still not constant, abort
    if (!isConst())
	return changes;


    // Otherwise: evaluate lust, avoiding side effects

    sideEffectsProhibited = true;
    sideEffectsOccured = false;

    const Box *result = eval(0);

    if (result)
    {
	assert(!sideEffectsOccured);

	if (VSEFlags::show_optimize)
	{
	    std::cout << "\n" << cdef->longname() << ": foldConsts: replacing\n" 
		<< *this << '\n';
	    std::cout.flush();
	}

	// Replace CallNode by constant
	*node = new ConstNode((Box *)result);

	if (VSEFlags::show_optimize)
	{
	    std::cout << "by " << **node << "\n";
	    std::cout.flush();
	}
	
	changes++;

	// Delete ListNode and its subtree
	delete this;
    }

    sideEffectsProhibited = false;

    return changes;
}


// Debugging

// Representation invariant
bool ListNode::OK() const
{
    assert (VSLNode::OK());

    assert (_head && _head->OK());
    assert (_tail && _tail->OK());

    return true;
}
