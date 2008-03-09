// $Id$
// Predefined VSL functions

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

char VSLBuiltin_rcsid[] = 
    "$Id$";

#include "VSLBuiltin.h"

#include "assert.h"

#include <limits.h>
#include <math.h>
#include <iostream>

#include "bool.h"
#include "VSLLib.h"

// Zillions of boxes...
#include "AlignBox.h"
#include "ArcBox.h"
#include "BinBox.h"
#include "ColorBox.h"
#include "DiagBox.h"
#include "FixBox.h"
#include "FontFixBox.h"
#include "PrimitiveB.h"
#include "SlopeBox.h"
#include "StringBox.h"
#include "TrueBox.h"
#include "DummyBox.h"


// Type checks

#ifndef NDEBUG
// True iff all args are atoms
static bool checkAtoms(ListBox *args)
{
    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	if (box->isListBox() && !box->isDummyBox())
	{
	    VSLLib::eval_error("invalid argument -- argument is list");
	    return false;
	}
    }

    return true;
}
#define CHECK_ATOMS(args) { if (!checkAtoms(args)) return 0; }
#else
#define CHECK_ATOMS(args)
#endif


// True iff all args have some size
static bool checkSize(ListBox *args)
{
    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	if (!box->size().isValid())
	    return false;
    }

    return true;
}

#define CHECK_SIZE(args) { if (!checkSize(args)) return new DummyBox; }


// Predefined VSL functions

// Logical ops

// Logical `not'
static Box *op_not(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    if ((*args)[0]->size(X) == 0)
	return new TrueBox;

    return new FalseBox;
}


// Graphical ops

// Normalize alignment
static Box *normalize(AlignBox *box)
{
    // Replace (&)(A) by A
    if (box->nchildren() == 1)
    {
	// Replace by single child
        Box *new_box = (*box)[0]->link();
	box->unlink();
	return new_box;
    }

    // Replace (&)(A, (&)(B, C)) by (&)(A, B, C)
    bool need_assoc_restructuring = false;

    for (int i = 0; i < box->nchildren(); i++)
    {
	Box *child = (*box)[i];
	if (ptr_type_info(box) == ptr_type_info(child))
	{
	    need_assoc_restructuring = true;
	    break;
	}
    }

    if (need_assoc_restructuring)
    {
	// Replace this box by NEW_ALIGN
	Box *new_box = box->dup0();
	AlignBox *new_align = ptr_cast(AlignBox, new_box);
	assert(new_align != 0);

	for (int i = 0; i < box->nchildren(); i++)
	{
	    Box *child = (*box)[i];
	    if (ptr_type_info(box) == ptr_type_info(child))
	    {
		AlignBox *a = ptr_cast(AlignBox, child);
		assert(a != 0);
		for (int j = 0; j < a->nchildren(); j++)
		    *new_align += (*a)[j];
	    }
	    else
		*new_align += child;
	}

	box->unlink();
	return new_align;
    }

    return box;
}

// Horizontal alignment
static Box *op_halign(ListBox *args)
{
    CHECK_ATOMS(args);

    HAlignBox *ret = 0;     // Return value

    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	if (ret == 0)
	    ret = new HAlignBox;
	*ret &= box;
    }

    // No child?  Return null box.
    if (ret == 0)
	return new NullBox;

    // One child?  Return it.
    if (ret->nchildren() == 1)
    {
	Box *child = (*ret)[0]->link();
	ret->unlink();
	return child;
    }

    // Return normalized alignment
    return normalize(ret);
}

// Textual alignment
static Box *op_talign(ListBox *args)
{
    CHECK_ATOMS(args);

    TAlignBox *ret = 0;     // Return value

    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	if (ret == 0)
	    ret = new TAlignBox;
	*ret &= box;
    }

    // No child?  Return null box.
    if (ret == 0)
	return new NullBox;

    // One child?  Return it.
    if (ret->nchildren() == 1)
    {
	Box *child = (*ret)[0]->link();
	ret->unlink();
	return child;
    }

    // Return normalized alignment
    return normalize(ret);
}

// Vertical alignment
static Box *op_valign(ListBox *args)
{
    CHECK_ATOMS(args);

    VAlignBox *ret = 0;

    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	if (ret == 0)
	    ret = new VAlignBox;
	*ret |= box;
    }

    // No child?  Return null box.
    if (ret == 0)
	return new NullBox;

    // One child?  Return it.
    if (ret->nchildren() == 1)
    {
	Box *child = (*ret)[0]->link();
	ret->unlink();
	return child;
    }

    // Return normalized alignment
    return normalize(ret);
}

// Stacked alignment
static Box *op_ualign(ListBox *args)
{
    CHECK_ATOMS(args);

    UAlignBox *ret = 0;

    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	if (ret == 0)
	    ret = new UAlignBox;
	*ret ^= box;
    }

    // No child?  Return null box.
    if (ret == 0)
	return new NullBox;

    // One child?  Return it.
    if (ret->nchildren() == 1)
    {
	Box *child = (*ret)[0]->link();
	ret->unlink();
	return child;
    }

    // Return normalized alignment
    return normalize(ret);
}



// Arithmetic ops

// Addition
static Box *op_plus(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    BoxSize sum(0,0);
    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	sum += box->size();
    }

    return new SpaceBox(sum);
}

// Multiplication
static Box *op_mult(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    BoxSize product(1,1);
    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	product *= box->size();
    }

    return new SpaceBox(product);
}

// Subtraction
static Box *op_minus(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    return new SpaceBox(
	(*args)[0]->size() - (*args)[1]->size());
}

// (Integer) division
static Box *op_div(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    if ((*args)[1]->size(X) == 0 || (*args)[1]->size(Y) == 0)
    {
	VSLLib::eval_error("division by zero");
	return 0;
    }

    return new SpaceBox(
	(*args)[0]->size() / (*args)[1]->size());
}

// Remainder
static Box *op_mod(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    if ((*args)[1]->size(X) == 0 || (*args)[1]->size(Y) == 0)
    {
	VSLLib::eval_error("division by zero");
	return 0;
    }

    return new SpaceBox(
	(*args)[0]->size() % (*args)[1]->size());
}



// Comparison

// Equality
static Box *op_eq(ListBox *args)
{
    if (*(*args)[0] == *(*args)[1])
	return new TrueBox;
    else
	return new FalseBox;
}

// Inequality
static Box *op_ne(ListBox *args)
{
    if (*(*args)[0] != *(*args)[1])
	return new TrueBox;
    else
	return new FalseBox;
}

// Greater than
static Box *op_gt(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    if ((*args)[0]->size() > (*args)[1]->size())
	return new TrueBox;
    else
	return new FalseBox;
}

// Greater or equal
static Box *op_ge(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    if ((*args)[0]->size() >= (*args)[1]->size())
	return new TrueBox;
    else
	return new FalseBox;
}

// Less than
static Box *op_lt(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    if ((*args)[0]->size() < (*args)[1]->size())
	return new TrueBox;
    else
	return new FalseBox;
}

// Less or equal
static Box *op_le(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    if ((*args)[0]->size() <= (*args)[1]->size())
	return new TrueBox;
    else
	return new FalseBox;
}


// List ops

// Cons lists
static Box *op_cons(ListBox *args)
{
    ListBox *ret = 0;

    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();

	if (!box->isListBox())
	{
	    VSLLib::eval_error("invalid argument -- argument is list");
	    if (ret)
		ret->unlink();
	    return 0;
	}

	if (!((ListBox *)box)->isEmpty())
	{
	    // Create list to append
	    // If box is last arg, a link suffices
	    ListBox *box2;
	    if (b->tail()->isEmpty())
		box2 = (ListBox *)box->link();
	    else
		box2 = (ListBox *)box->dup();

	    // Append list: 
	    // If box is first arg, copy box
	    if (ret == 0)
		ret = box2;
	    else
	    {
		ret->cons(box2);
		box2->unlink();
	    }
	}
    }

    // No args? return []
    if (ret == 0)
	ret = new ListBox;

    return ret;
}


// Standard functions

// hspace(box)
static Box *hspace(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    const Box *child = (*args)[0];
    return new SpaceBox(BoxSize(child->size(X), 0));
}

// vspace(box)
static Box *vspace(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    const Box *child = (*args)[0];
    return new SpaceBox(BoxSize(0, child->size(Y)));
}

// hfix(box)
static Box *hfix(ListBox *args)
{
    CHECK_ATOMS(args);

    return new HFixBox((Box *)(*args)[0]);
}

// vfix(box)
static Box *vfix(ListBox *args)
{
    CHECK_ATOMS(args);

    return new VFixBox((Box *)(*args)[0]);
}

// bin(box)
static Box *op_bin(ListBox *args)
{
    CHECK_ATOMS(args);

    return new BinBox((Box *)(*args)[0]);
}

// tag(box)
static Box *tag(ListBox *args)
{
    return new StringBox((*args)[0]->name());
}


// Return string from box

// str(box)
static Box *str(ListBox *args)
{
    return new StringBox((*args)[0]->str());
}


// Return list of characters from box

// chars(box)
static Box *chars(ListBox *args)
{
    ListBox *list = new ListBox;

    string str = (*args)[0]->str();
    for (unsigned int i = 0; i < str.length(); i++)
    {
	StringBox *s = new StringBox(string(str[i]));
	*list += s;
	s->unlink();
    }

    return list;
}


// Set font

// font(box, font)
static Box *font(ListBox *args)
{
    // Copy first arg and set its font

    Box *ret = ((Box *)(*args)[0])->dup();
    ret->newFont((*args)[1]->str());

    return ret;
}

// fontfix(box)
static Box *fontfix(ListBox *args)
{
    return new FontFixBox((Box *)(*args)[0]);
}


// Set color

// background(box, color_name)
static Box *background(ListBox *args)
{
    return new BackgroundColorBox((Box *)(*args)[0], (*args)[1]->str());
}

// foreground(box, color_name)
static Box *foreground(ListBox *args)
{
    return new ForegroundColorBox((Box *)(*args)[0], (*args)[1]->str());
}


// Standard boxes

// rise(linethickness)
static Box *rise(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    return new RiseBox((*args)[0]->size(X));
}

// fall(linethickness)
static Box *fall(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    return new FallBox((*args)[0]->size(X));
}

// arc(start, length, linethickness)
static Box *arc(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    return new ArcBox((*args)[0]->size(X),
	(*args)[1]->size(X),
	(*args)[2]->size(X));
}

// Square box from maximal height and width
static Box *square(ListBox *args)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    Box *arg = (Box *)(*args)[0];
    return new SquareBox(max(arg->size(X), arg->size(Y)));
}

// fill()
static Box *fill(ListBox *)
{
    return new FillBox;
}

// rule()
static Box *rule(ListBox *)
{
    return new RuleBox;
}

// diag()
static Box *diag(ListBox *)
{
    return new DiagBox;
}


// Default boxes

// Place holder for an undefined box
static Box *undef(ListBox *)
{
    return new StringBox("?" "?" "?");
}


// Error handling

// Make evaluation fail; issue error message
static Box *fail(ListBox *args)
{
    CHECK_ATOMS(args);

    if ((*args)[0])
	VSLLib::eval_error((*args)[0]->str());
    else
	VSLLib::eval_error("evaluation failed");

    return 0;
}


// Table of predefined functions

struct BuiltinRec {
    const char* ext_name;         // Function name (external; 0 = func_name)
    const char* func_name;        // Function name (internal)
    bool isAssoc;           // Flag: associative?
    bool hasSideEffects;    // Flag: side effects?
    bool isInfix;           // Flag: dump infix?
    BuiltinFunc eval_func;  // Function to be called

};

static BuiltinRec builtins[] = {

// n-ary ops
{ "(&)",    "__op_halign",  true,   false,  false,  op_halign },
{ "(|)",    "__op_valign",  true,   false,  false,  op_valign },
{ "(^)",    "__op_ualign",  true,   false,  false,  op_ualign },
{ "(~)",    "__op_talign",  true,   false,  false,  op_talign },
{ "(+)",    "__op_plus",    true,   false,  false,  op_plus },
{ "(*)",    "__op_mult",    true,   false,  false,  op_mult },
{ "(::)",   "__op_cons",    true,   false,  false,  op_cons },

// binary ops
{ "(-)",    "__op_minus",   false,  false,  false,  op_minus },
{ "(/)",    "__op_div",     false,  false,  false,  op_div },
{ "(%)",    "__op_mod",     false,  false,  false,  op_mod },
{ "(=)",    "__op_eq",      false,  false,  false,  op_eq },
{ "(<>)",   "__op_ne",      false,  false,  false,  op_ne },
{ "(>)",    "__op_gt",      false,  false,  false,  op_gt },
{ "(>=)",   "__op_ge",      false,  false,  false,  op_ge },
{ "(<)",    "__op_lt",      false,  false,  false,  op_lt },
{ "(<=)",   "__op_le",      false,  false,  false,  op_le },
{ 0,        "__op_bin",     false,  false,  false,  op_bin },

// unary ops
{ "not",    "__op_not",     false,  false,  false,  op_not },

// functions
{ 0,    "__hspace",     false,  false,  false,  hspace },
{ 0,    "__vspace",     false,  false,  false,  vspace },
{ 0,    "__hfix",       false,  false,  false,  hfix },
{ 0,    "__vfix",       false,  false,  false,  vfix },
{ 0,    "__rise",       false,  false,  false,  rise },
{ 0,    "__fall",       false,  false,  false,  fall },
{ 0,    "__arc",        false,  false,  false,  arc },
{ 0,    "__square",     false,  false,  false,  square },
{ 0,    "__tag",        false,  false,  false,  tag },
{ 0,    "__string",     false,  false,  false,  str },
{ 0,    "__chars",      false,  false,  false,  chars },
{ 0,    "__font",       false,  false,  false,  font },
{ 0,    "__fontfix",    false,  false,  false,  fontfix },
{ 0,    "__background", false,  false,  false,  background },
{ 0,    "__foreground", false,  false,  false,  foreground },

// functions with side effects
{ 0,    "__fail",       false,  true,   false,  fail },

// constants
{ 0,    "__fill",       false,  false,  false,  fill },
{ 0,    "__rule",       false,  false,  false,  rule },
{ 0,    "__diag",       false,  false,  false,  diag },
{ 0,    "__undef",      false,  false,  false,  undef },

};


// Access functions

// Return function name index for FUNC_NM; or -1 if not found
int VSLBuiltin::resolve(const string& func_nm)
{
    for (int i = 0; i < int(sizeof(builtins) / sizeof(builtins[0])); i++)
	if (func_nm == builtins[i].func_name)
	    return i;

    return -1;			// not found
}

BuiltinFunc VSLBuiltin::func(int idx)
{
    assert (idx >= 0 && idx < int(sizeof(builtins) / sizeof(builtins[0])));
    return builtins[idx].eval_func;
}

bool VSLBuiltin::isAssoc(int idx)
{
    assert (idx >= 0 && idx < int(sizeof(builtins) / sizeof(builtins[0])));
    return builtins[idx].isAssoc;
}

bool VSLBuiltin::hasSideEffects(int idx)
{
    assert (idx >= 0 && idx < int(sizeof(builtins) / sizeof(builtins[0])));
    return builtins[idx].hasSideEffects;
}

bool VSLBuiltin::isInfix(int idx)
{
    assert (idx >= 0 && idx < int(sizeof(builtins) / sizeof(builtins[0])));
    return builtins[idx].isInfix;
}

const char *VSLBuiltin::func_name(int idx)
{
    assert (idx >= 0 && idx < int(sizeof(builtins) / sizeof(builtins[0])));
    return builtins[idx].func_name;
}

const char *VSLBuiltin::ext_name(int idx)
{
    assert (idx >= 0 && idx < int(sizeof(builtins) / sizeof(builtins[0])));
    const char *s = builtins[idx].ext_name;
    return s != 0 ? s : builtins[idx].func_name;
}
