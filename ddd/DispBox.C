// $Id$
// Display boxes

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>
// and Andreas Zeller <zeller@ips.cs.tu-bs.de>
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char DispBox_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// Create DispBoxes via VSL
//-----------------------------------------------------------------------------

#include "assert.h"
#include "DispBox.h"
#include "StringBox.h"
#include "ColorBox.h"
#include "DispNode.h"
#include "VSEFlags.h"
#include "strclass.h"
#include "bool.h"
#include "cook.h"
#include "ddd.h"
#include "status.h"
#include "shorten.h"
#include "string-fun.h"
#include "tabs.h"
#include "version.h"
#include "regexps.h"

#include <ctype.h>



//-----------------------------------------------------------------------------

VSLLib  DispBox::dummylib;
VSLLib* DispBox::vsllib_ptr       = &DispBox::dummylib;
string  DispBox::vsllib_name      = "builtin";
string  DispBox::vsllib_path      = ".";
string  DispBox::vsllib_defs      = "";
string  DispBox::vsllib_base_defs = "";
int     DispBox::max_display_title_length = 20;
bool    DispBox::vsllib_initialized = false;
bool    DispBox::align_2d_arrays = true;

// ***************************************************************************
//
DispBox::DispBox (int disp_nr, const string& title, 
		  const DispValue *dv, const DispValue *parent)
    : mybox(0), title_box(0)
{
    // Create display
    set_title(disp_nr, title);
    set_value(dv, parent);
}


// ***************************************************************************
//
void DispBox::init_vsllib(void (*background)())
{
    if (vsllib_initialized)
	return;			// We already have a library

    static bool initializing = false;
    if (initializing)
	return;			// We are already initializing

    initializing = true;

    static const char builtin_def[] = 
#include "ddd.vsl.h"
	;

    // Set include search path
    VSEFlags::include_search_path = 
	strcpy(new char[vsllib_path.length() + 1], vsllib_path);

    // Load library
    void (*old_background)() = VSLLib::background;
    VSLLib::background = background;

    if (string(vsllib_name) == "builtin")
    {
	string defs = string(builtin_def)
	    + "#line 1 \"" Ddd_NAME "*vslBaseDefs\"\n"
	    + vsllib_base_defs
	    + "#line 1 \"" Ddd_NAME "*vslDefs\"\n"
	    + vsllib_defs;
	istrstream is(defs.chars());
	vsllib_ptr = new VSLLib(is, VSEFlags::optimize_mode());
    }
    else
    {
	vsllib_ptr = new VSLLib(vsllib_name, VSEFlags::optimize_mode());
    }

    VSLLib::background = old_background;
    initializing = false;
    vsllib_initialized = true;
}


// ***************************************************************************
//
DispBox::~DispBox ()
{
    if (mybox != 0)
	mybox->unlink();

    if (title_box != 0)
	title_box->unlink();
}

// ***************************************************************************
//
void DispBox::set_value (const DispValue* dv, const DispValue *parent)
{
    if (mybox != 0)
    {
	mybox->unlink();
	mybox = 0;
    }

    VSLArg args[3];
    int arg = 0;

    if (title_box != 0)
	args[arg++] = title_box;

    args[arg++] = create_value_box(dv, parent);
    mybox = eval("display_box", args);
}

void DispBox::set_title(int disp_nr, const string& t)
{
    string title = t;

    if (title != "")
    {
	if (!is_user_command(title))
	{
	    // Strip DBX scope information from title
#if RUNTIME_REGEX
	    static regex rxdbx_scope("[a-zA-Z_0-9]*`");
#endif
	    int i = 0;
	    while (int(title.length()) > max_display_title_length 
		   && ((i = title.index(rxdbx_scope)) >= 0))
	    {
		title = title.before(i) + title.after('`');
	    }
	}

	// Shorten remainder
	shorten(title, max_display_title_length);
    }

    if (title_box != 0)
    {
	title_box->unlink();
	title_box = 0;
    }

    // Create title
    if (title != "")
    {
	VSLArg args[3];
	int arg = 0;

	if (is_user_command(title))
	{
	    // User command: use EXPR
	    title = user_command(title);
	    if (title.contains("info ", 0))
		title = title.after("info ");
	    else if (title.contains(" "))
		title = title.before(" ");
	    if (title.length() > 0)
		title = toupper(title[0]) + title.after(0);

	    args[arg++] = tag(title);
	}
	else
	{
	    // Normal title: use NUMBER: EXPR
	    args[arg++] = itostring(disp_nr);
	    args[arg++] = tag(title);
	}

	title_box = eval("title", args);
    }
}

// Return true if DV is a (right-aligned) numeric value
bool DispBox::is_numeric(const DispValue *dv, const DispValue *parent)
{
    if (parent != 0 && parent->type() != Array)
	return false;

    if (dv->value() == "")
	return false;
    if (dv->value().contains(' '))
	return false;

    char c = dv->value()[0];
    return isdigit(c) || c == '+' || c == '-' || c == '.';
}

// ***************************************************************************
// Create a Box for the value DV
Box *DispBox::_create_value_box(const DispValue *dv, const DispValue *parent)
{
    Box *vbox = 0;

    if (dv->cached_box() != 0)
	return dv->cached_box()->link();

    switch (dv->type())
    {
    case Simple:
    {
	if (dv->collapsed())
	    vbox = eval("collapsed_simple_value");
	else
	{
	    // Flush numeric values to the right, unless in a struct
	    if (is_numeric(dv, parent))
		vbox = eval("numeric_value", dv->value());
	    else
		vbox = eval("simple_value", dv->value());
	}
	break;
    }

    case Text:
    {
	if (dv->collapsed())
	    vbox = eval("collapsed_text_value");
	else
	{
	    string v = dv->value();
	    strip_space(v);
	    untabify(v);

	    int n = v.freq('\n');
	    string *lines = new string[n + 1];
	    split(v, lines, n + 1, '\n');

	    ListBox *args = new ListBox;
	    for (int i = 0; i < n + 1; i++)
	    {
		if (lines[i] == "")
		    lines[i] = " ";
		*args += eval("text_line", lines[i]);
	    }
	    vbox = eval("text_value", args);

	    args->unlink();
	    delete[] lines;
	}
	break;
    }

    case Pointer:
    {
	if (dv->collapsed())
	    vbox = eval("collapsed_pointer_value");
	else if (dv->dereferenced())
	    vbox = eval("dereferenced_pointer_value", dv->value());
	else
	    vbox = eval("pointer_value", dv->value());
	break;
    }

    case Array:
    {
	if (dv->collapsed())
	    vbox = eval("collapsed_array");
	else
	{
	    int count = dv->nchildren();
	    if (count == 0)
	    {
		vbox = eval("empty_array");
	    }
	    else
	    {
		bool have_2d_array = true;

		if (dv->repeats() > 1)
		    have_2d_array = false;

		int nchildren = 0;
		for (int k = 0; have_2d_array && k < count; k++)
		{
		    DispValue *child = dv->child(k);
		    if (child->type() != Array)
		    {
			// Child is no array
			have_2d_array = false;
			break;
		    }

		    if (child->repeats() > 1)
		    {
			// Child is a repeated array
			have_2d_array = false;
			break;
		    }

		    if (k == 0)
		    {
			nchildren = child->nchildren();
		    }
		    else if (nchildren != child->nchildren())
		    {
			// Children have differing sizes
			have_2d_array = false;
			break;
		    }
		}

		if (have_2d_array && align_2d_arrays)
		{
		    // Two-dimensional array
		    ListBox *table = new ListBox;

		    if (dv->vertical_aligned())
		    {
			// Sub-arrays are aligned vertically;
			// each sub-array is laid out horizontally
			for (int i = 0; i < count; i++)
			{
			    DispValue *c = dv->child(i);
			    ListBox *row = new ListBox;
			    for (int j = 0; j < c->nchildren(); j++)
			    {
				DispValue *cc = c->child(j);
				ListBox *args = new ListBox;
				*args += create_value_box(cc, c);
				Box *b = eval("twodim_array_elem", args);
				*row += b;
				b->unlink();
				args->unlink();
			    }

			    *table += row;
			    row->unlink();
			}
		    }
		    else
		    {
			// Sub-arrays are aligned horizontally;
			// each sub-array is laid out vertically
			int max_cc = 0;
			for (int j = 0; j < count; j++)
			    max_cc = max(max_cc, dv->child(j)->nchildren());
			for (int i = 0; i < max_cc; i++)
			{
			    ListBox *row = new ListBox;
			    for (int j = 0; j < count; j++)
			    {
				DispValue *c = dv->child(j);
				Box *elem = 0;
				if (i < c->nchildren())
				{
				    DispValue *cc = c->child(i);
				    elem = create_value_box(cc, c);
				}
				else
				{
				    elem = new ListBox;
				}
				ListBox *args = new ListBox;
				*args += elem;
				Box *b = eval("twodim_array_elem", elem);
				*row += b;
				b->unlink();
				args->unlink();
			    }

			    *table += row;
			    row->unlink();
			}
		    }

		    ListBox *args = new ListBox;
		    *args += table;
		    table->unlink();

		    vbox = eval("twodim_array", args);

		    args->unlink();
		}
		else
		{
		    // One-dimensional array
		    ListBox* args = new ListBox;
		    for (int i = 0; i < count; i++)
			*args += create_value_box (dv->child(i), dv);

		    if (dv->vertical_aligned())
			vbox = eval("vertical_array", args);
		    else
			vbox = eval("horizontal_array", args);

		    args->unlink();
		}
	    }
	}
	break;
    }

    case Sequence:
    {
	if (dv->collapsed())
	    vbox = eval("collapsed_sequence_value");
	else
	{
	    // Create children
	    ListBox* args = new ListBox;
	    int count = dv->nchildren();
	    for (int i = 0; i < count; i++)
		*args += create_value_box(dv->child(i), dv);

	    vbox = eval("sequence_value", args);
	    args->unlink();
	}
	break;
    }

    case List:
    case Struct:
    {
	String collapsed_value = (dv->type() == List ? 
				  "collapsed_list_value" :
				  "collapsed_struct_value");
	String empty_value     = (dv->type() == List ? 
				  "empty_list_value" :
				  "empty_struct_value");
	String member_name     = (dv->type() == List ? 
				  "list_member_name" :
				  "struct_member_name");
	String value           = (dv->type() == List ? 
				  "list_value" :
				  "struct_value");

	if (dv->collapsed())
	    vbox = eval(collapsed_value);
	else
	{
	    int count = dv->nchildren();
	    if (count == 0)
		vbox = eval(empty_value);
	    else
	    {
		// Determine maximum member name width
		int max_member_name_width = 0;
		int i;
		for (i = 0; i < count; i++)
		{
		    string child_member_name = dv->child(i)->name();
		    Box *box = eval(member_name, child_member_name);
		    max_member_name_width = 
			max(max_member_name_width, box->size(X));
		    box->unlink();
		}

		// Create children
		ListBox* args = new ListBox;
		for (i = 0; i < count; i++)
		    *args += create_value_box(dv->child(i), dv,
					      max_member_name_width);

		vbox = eval(value, args);
		args->unlink();
	    }
	}
	break;
    }

    case Reference:
    {
	if (dv->collapsed())
	    vbox = eval("collapsed_reference_value");
	else
	{
	    ListBox* args = new ListBox;
	    for (int i = 0; i < 2; i++)
		*args += create_value_box (dv->child(i), dv);

	    vbox = eval("reference_value", args);
	    args->unlink();
	}
	break;
    }

    case UnknownType:
	assert(0);
	abort();
    }

    // Show repeats
    if (dv->repeats() > 1 && !dv->collapsed())
    {
	vbox = eval("repeated_value", vbox->link(), dv->repeats());
    }

    // Highlight if value changed
    if (dv->is_changed())
    {
	vbox = eval("changed_value", vbox->link());
    }

    ((DispValue *)dv)->set_cached_box(vbox);

    return vbox;
}

Box *DispBox::create_value_box (const DispValue *dv,
				const DispValue *parent,
				int member_name_width)
{
    Box *vbox = 0;
    if (dv == 0 || !dv->enabled())
    {
	vbox = eval("disabled");
    }
    else
    {
	vbox = _create_value_box(dv, parent);
    }

    // Add member name
    if (dv != 0 && parent != 0)
    {
	switch (parent->type())
	{
	case List:
	    if (dv->type() != Text)
		vbox = eval("list_member", dv->name(), vbox->link(), 
			    member_name_width);
	    break;

	case Struct:
	    if (dv->name() == "" || dv->name().matches(rxwhite))
	    {
		// Don't prepend an empty member name
	    }
	    else
	    {
		vbox = eval("struct_member", dv->name(), vbox->link(), 
			    member_name_width);
	    }
	    break;

	case Sequence:
	case Array:
	case Reference:
	case Simple:
	case Text:
	case Pointer:
	    break;

	case UnknownType:
	    assert(0);
	    abort();
	}
    }

    if (dv != 0)
    {
	Data *data = (Data *)dv;
	vbox = vbox->tag(data);
    }

    return vbox;
}

// ***************************************************************************
// Duplication with special handling of undefined boxes
Box *DispBox::dup(const string& func_name, const Box *box)
{
    if (box != 0)
	return ((Box *)box)->link();

    // Box not found
    return (new ForegroundColorBox(
	new StringBox("<?" + func_name + ">"), "red"))->link();
}
