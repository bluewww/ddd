// $Id$
// VSL definition

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

char VSLDef_rcsid[] = 
    "$Id$";

#include <iostream>
#include <sstream>
#include <stdlib.h>

#include "assert.h"
#include "hash.h"

#include "VSEFlags.h"

#include "VSLLib.h"
#include "VSLDef.h"
#include "VSLDefList.h"
#include "VSLBuiltin.h"

#include "Box.h"
#include "ListBox.h"
#include "MatchBox.h"

#include "VSLNode.h"
#include "ArgNode.h"

DEFINE_TYPE_INFO_0(VSLDef)

// VSLDef

// Constructor
VSLDef::VSLDef(VSLDefList* l, VSLNode *pattern, VSLNode *e, 
	       const string& filename, int lineno)
    : _expr(e),
      _node_pattern(pattern),
      _box_pattern(0),
      _nargs(pattern->nargs()),
      _straight(pattern->isStraight()),
      _filename(filename),
      _lineno(lineno),
	_listnext(0), _libnext(0), _libprev(0),
      being_compiled(false),
      deflist(l)
{}

// Pattern matching

const int max_instances = 256;

// Pattern matching with nodes

static VSLNode *node_instances[max_instances];

static void node_matches(int data, const VSLNode *node)
{
    node_instances[data] = (VSLNode *)node;
}

// Check if definition is conformant to argument ARG;
// store matched values from arg in node_instances
bool VSLDef::matches(const VSLNode *arg) const
{
    // While comparing, for all X, ArgNode == X holds.

    ArgNodeFunc oldCB = ArgNode::matchCallback;
    ArgNode::matchCallback = node_matches;

    bool oldFlag = ArgNode::matchesAll;
    ArgNode::matchesAll = true;
    
    if (VSEFlags::show_match_defs)
    {
	std::cout << "\nDef Match: " << longname() << " ? " << *arg;
	std::cout.flush();
    }

    bool ret = (*_node_pattern == *arg);

    if (VSEFlags::show_match_defs)
    {
	if (ret)
	    std::cout << "\nDef Match: " << longname() << " matches " << *arg;
	else
	    std::cout << "\nDef Match: " << longname() << " does not match " 
		<< *arg;
	std::cout.flush();
    }

    ArgNode::matchCallback = oldCB;
    ArgNode::matchesAll = oldFlag;

    return ret;
}

// Return nodes matched against A
VSLNode **VSLDef::nodelist(const VSLNode *arg) const
{
    for (unsigned i = 0; i < nargs(); i++)
	node_instances[i] = 0;

    bool ok = matches(arg);
    if (!ok)
	return 0;

    return node_instances;
}



// Pattern matching with boxes

static Box *box_instances[max_instances];

static void box_matches(int data, const Box *box)
{
    box_instances[data] = (Box *)box;
}

// Check if definition is conformant to argument ARG;
// store matched values from arg in box_instances
bool VSLDef::matches(const Box *arg) const
{
    bool ret = false;

    // While comparing, for all X, MatchBox == X holds.

    MatchBoxFunc oldCB = MatchBox::matchCallback;
    MatchBox::matchCallback = box_matches;

    MatchBox::matchesAll = true;

    if (VSEFlags::show_match_defs)
    {
	std::cout << "\nDef Match: " << longname() << " ? " << *arg;
	std::cout.flush();
    }

    if (_box_pattern == 0)
	compilePattern();

    if (_box_pattern)
	ret = (*_box_pattern == *arg);

    if (VSEFlags::show_match_defs)
    {
	if (ret)
	    std::cout << "\nDef Match: " << longname() << " matches " << *arg;
	else
	    std::cout << "\nDef Match: " << longname() << " does not match " 
		<< *arg;
	std::cout.flush();
    }

    MatchBox::matchesAll = false;

    MatchBox::matchCallback = oldCB;
    return ret;
}


// Convert node_pattern into box_pattern
void VSLDef::compilePattern() const
{
    // Protect against patterns like f(f(a)) = ...
    if (being_compiled)
    {
	VSLLib::eval_error("recursive pattern", this);
	return;
    }

    uncompilePattern();

    // Use a list of match boxes as argument
    ListBox *list = new ListBox;
    unsigned i;
    for (i = 0; i < nargs(); i++)
    {
	MatchBox *m = new MatchBox(i);
	(*list) += m;
	m->unlink();
    }

    MUTABLE_THIS(VSLDef *)->being_compiled = true;
    const Box *result = _node_pattern->eval(list);
    MUTABLE_THIS(VSLDef *)->being_compiled = false;

    list->unlink();

    // Check if valid
    if (result == 0)
	VSLLib::eval_error("cannot evaluate pattern", this);
    else
    {
	// Each MatchBox must be used exactly once
	int *instances = new int [nargs()];
	for (i = 0; i < nargs(); i++)
	    instances[i] = 0;
	result->countMatchBoxes(instances);

	for (i = 0; i < nargs(); i++)
	{
	    if (instances[i] == 0)
	    {
		std::ostringstream os;
		os << "invalid pattern: arg" << i 
		   << " is never instantiated";
		VSLLib::eval_error(os, this);
	    }
	    if (instances[i] > 1)
	    {
		std::ostringstream os;
		os << "invalid pattern: arg" << i 
		   << " is instantiated several times";
		VSLLib::eval_error(os, this);
	    }
	}
	delete[] instances;
    }

    MUTABLE_THIS(VSLDef *)->_box_pattern = (Box *)result;
}



// Evaluation

// Backtrace

const VSLDef **VSLDef::backtrace = 0; 
const Box **VSLDef::backtrace_args = 0;
// (doesn't work without initializing --  why?)

// Evaluate def
const Box *VSLDef::eval(Box *arg) const
{
    static int depth = 0;

    // Create backtrace
    if (backtrace == 0)
    {
	backtrace = new const VSLDef *[VSEFlags::max_eval_nesting + 2];
	backtrace_args = new const Box *[VSEFlags::max_eval_nesting + 2];
    }

    backtrace[depth] = this;
    backtrace_args[depth] = arg->link();
    backtrace[depth + 1] = 0;   // End marker

    // Debugging
    if (VSEFlags::show_large_eval)
    {
	std::clog << depth << " ";
	for (int i = 0; i < depth; i++)
	    std::clog << "  ";

	std::clog << longname() << *arg << "...\n";
    }

    // Actual function
    const Box *box = 0;

    if (depth < VSEFlags::max_eval_nesting)
    {
	ListBox *myarglist = arglist(arg);

	if (myarglist)
	{
	    depth++;
	    if (_expr)
		box = _expr->eval(myarglist);
	    else
		VSLLib::eval_error("undefined function");
	    depth--;

	    myarglist->unlink();
	}
	else
	    VSLLib::eval_error("invalid argument");
    }
    else
	VSLLib::eval_error("infinite recursion");

    // Debugging
    if (VSEFlags::show_large_eval)
    {
	std::clog << depth << " ";
	for (int i = 0; i < depth; i++)
	    std::clog << "  ";

	std::clog << longname() << *arg;

	if (box == 0)
	    std::clog << " FAILS";
	else
	    std::clog << " = " << *box;

	std::clog << "\n";
    }

    // Delete backtrace
    backtrace[depth] = 0;
    ((Box *)backtrace_args[depth])->unlink();

    return box;
}

// Convert argument list into a format suitable for ArgNode instances.
ListBox *VSLDef::arglist(const Box *arg) const
{
    if (straight())
    {
	assert (arg->isListBox());
	return (ListBox *)((Box *)arg)->link();
    }

    unsigned i;
    for (i = 0; i < nargs(); i++)
	box_instances[i] = 0;

    bool ok = matches(arg);
    if (!ok)
	return (ListBox *)0;

    ListBox *list = new ListBox;
    for (i = 0; i < nargs(); i++)
    {
	assert(box_instances[i] != 0);  // cannot isolate arg
	(*list) += box_instances[i];
    }

    return list;
}



// Resolve function names
int VSLDef::resolveNames()
{
    // Apply to body
    int changes = expr()->resolveNames(this, nargs());

    // Now replace all NameNodes in the pattern by equivalent ArgNodes.
    string s;
    unsigned offset = 0;

    while (!(s = node_pattern()->firstName(), s).empty())
    {
	// Replace in body
	int ch = expr()->resolveName(this, &expr(), s, offset);
	if (ch == 0)
	    VSLLib::eval_warning("`" + s + "' unused", this);
	changes += ch;

	// Replace in pattern
	ch = node_pattern()->resolveName(this, &node_pattern(), s, offset);
	assert(ch > 0);
	if (ch > 1)
	    VSLLib::eval_error("`" + s + "' used several times", this);

	offset += ch;
    }

    assert(offset == nargs());

    // Remove remaining NameNodes (replace them by arg0)
    while (!(s = expr()->firstName(), s).empty())
    {
	VSLLib::eval_error("`" + s + "' undefined", this);
	expr()->resolveName(this, &expr(), s, 0);
    }

    return changes;
}


// Resources

// Create function args (for error messages, etc.)
string VSLDef::args() const
{
    string ans;
    // Konstant: has no arg
    if ((deflist->func_name())[0] == '#')
	return ans;

    std::ostringstream os;

    if (_node_pattern->isArgNode())
	os << "(" << *_node_pattern << "...)";
    else
	os << *_node_pattern;

    ans = os;
    return ans;
}

// Internal function name
string VSLDef::func_name() const
{
    return deflist->func_name() + args();
}

// External function name
string VSLDef::f_name() const
{
    return deflist->f_name() + args();
}

// External function name, including location
string VSLDef::longname() const
{
    std::ostringstream os;
    
    if (!_filename.empty())
	os << _filename << ":" << _lineno << ": ";
    os << f_name();

    return string(os);
}

// Delete definition *and all successors*
VSLDef::~VSLDef()
{
    if (_listnext != 0)
	delete _listnext;

    if (_expr != 0)
	delete _expr;

    if (_node_pattern != 0) 
	delete _node_pattern;

    if (_box_pattern != 0)
	_box_pattern->unlink();
}

// Representation invariant (*without* successors)
bool VSLDef::OK() const
{
    // Check expressions
    assert (_expr == 0 || _expr->OK());
    assert (_node_pattern && _node_pattern->OK());
    assert (_box_pattern == 0 || _box_pattern->OK());

    // Check pointers to successor and predecessor
    assert (libnext() == 0 || libnext()->libprev() == this);
    assert (libprev() == 0 || libprev()->libnext() == this);

    return true;
}
