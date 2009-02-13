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

char VSLDefList_rcsid[] = 
    "$Id$";

#include <iostream>
#include <sstream>
#include "assert.h"

#include "VSEFlags.h"

#include "VSLLib.h"
#include "VSLDef.h"
#include "VSLDefList.h"
#include "VSLBuiltin.h"

#include "Box.h"
#include "ListBox.h"

#include "VSLNode.h"
#include "UniqueId.h"

DEFINE_TYPE_INFO_0(VSLDefList)

// VSLDefList

// Evaluate expression
const Box *VSLDefList::eval(Box *arg) const
{
    VSLDef *d = def(arg);

    if (d == 0)
    {
	std::ostringstream s;
	s << *arg;
	VSLLib::eval_error("no suiting definition for " + f_name() + s);
    }

    return d ? d->eval(arg) : 0;
}

// Append def to list; if a new def was appended, set newFlag
VSLDef* VSLDefList::add(bool &newFlag, VSLNode *pattern, VSLNode *expr,
			const string& filename, int lineno)
{
    VSLDef *newdef = new VSLDef(this, pattern, expr, filename, lineno);

    // Check if already declared
    for (VSLDef *d = _first; d != 0; d = d->listnext())
	if (*(d->node_pattern()) == *(newdef->node_pattern()))
	{
	    // There is already a declaration: delete new definition
	    newFlag = false;
	    newdef->expr() = 0; // keep expression
	    delete newdef;

	    if (expr == 0)
	    {
		// n-th (n > 1) Declaration: ignore
		return d;
	    }

	    if (d->expr() == 0)
	    {
		// Only declared, but not yet defined: Insert def
		d->expr() = expr;
		return d;
	    }

	    if (*expr != *(d->expr()))
	    {
		// Already defined
		VSLLib::parse_error("'" + d->f_name() + "\' already defined");
		VSLLib::eval_echo("(this is the previous definition)", d);
		delete expr; return 0;
	    }

	    // Otherwise: treat like a new declaration
	    delete expr; return d;
	}

    // New defs are appended at end.  Thus, we always choose the first
    // matching def (and not the last, for instance).
    if (_last == 0)
	_first = _last = newdef;
    else
	_last->listnext() = newdef;
    _last = newdef;
    _ndefs++;

    newFlag = true; 
    return newdef;
}


// Choose suitable def
VSLDef *VSLDefList::def(Box *arg) const
{
    for (VSLDef *d = _first; d != 0; d = d->listnext())
	if (d->matches(arg))
	    return d;

    return 0;
}


// Delete all definitions
void VSLDefList::replace()
{
    // Detach all defs
    for (VSLDef *son = first(); son != 0; son = son->listnext())
    {
	assert (son->deflist == this);

	// Adapt successors
	if (son->libnext())
	    son->libnext()->libprev() = son->libprev();
	else
	{
	    assert (son == lib->_last);
	    lib->_last = son->libprev();
	}

	// Adapt predecessors
	if (son->libprev())
	    son->libprev()->libnext() = son->libnext();
	else
	{
	    assert (son == lib->_first);
	    lib->_first = son->libnext();
	}
    }

    // Delete all VSLDef's in this VSLDefList
    delete _first;

    // Reset pointers
    _first = _last = 0;
    _ndefs = 0;
}

// Override
void VSLDefList::override()
{
    std::ostringstream os;
    os << UniqueId();

    _func_name += "'";
    _func_name += string(os);     // Just append 'ID to the name
}

// Destroy definition *and all successors*
void VSLDefList::destroy()
{
    if (destroyed()) {
      // Self has already been destroyed. The call comes from
      // the last object referring to it. Commit suicide.
      delete this;
    } else {
      // First destroy content.
      assert(_next != this);
      if (_next) {
	_next->destroy();
	_next = 0;
      }
      if (_first) {
	delete _first;
	_first = 0;
      }
      _destroyed = true;
      // If there is no object pointing to self, commit suicide
      if (references() == 0)
	delete this;
    }
}

VSLDefList::~VSLDefList()
{
    assert(destroyed());        // call comes from destroy()
    if (!destroyed()) ::abort();
    assert(references() == 0);    // no object referenced self.
    if (references() != 0) ::abort();
}

// Decrement reference count
void VSLDefList::dec()
{
    --_references;
    // If self has already been destroyed and nothing points
    // to it, commit suicide.
    if (references() == 0 && destroyed()) {
      destroy();
    }
}


// Representation invariant
bool VSLDefList::OK() const
{
#if !NDEBUG
    VSLDef *d = _first;
    unsigned count = 0;

    // Check whether _ndefs and _last are ok
    VSLDef *last_d = 0;
    (void) last_d;		// Use it
    while (d != 0)
    {
	last_d = d;
	d = d->listnext();
	count++;
    }
    assert (count == _ndefs);
    assert (last_d == _last);

    // Check whether pointer to DefList is ok
    d = _first;
    while (d != 0)
    {
	assert (d->deflist == this);
	d = d->listnext();
    }

    // Check whether defs are ok
    for (d = _first; d != 0; d = d->listnext())
	assert (d->OK());

#endif

    return true;
}
