// $Id$
// Display boxes

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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

char DispBox_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// Klasse zur Erzeugung von DispBoxen mit VSL.
//-----------------------------------------------------------------------------

#include "assert.h"
#include "DispBox.h"
#include "strclass.h"
#include "bool.h"
#include "cook.h"
#include "ddd.h"
#include "status.h"


//-----------------------------------------------------------------------------

static regex RXpointer_name_pointer
("->(\\.\\.->)?[a-zA-Z_][]a-zA-Z_0-9\\[\\.\\]*->");

static regex RXscope("[a-zA-Z_0-9]*`");

//-----------------------------------------------------------------------------

VSLLib* DispBox::vsllib_ptr      = 0;
int     DispBox::max_name_length = 20;
string  DispBox::vsllib_name     = "builtin";
string  DispBox::vsllib_path     = ".";

// ***************************************************************************
//
DispBox::DispBox (string disp_nr,
		  string name,
		  const DispValue* dv)
    : mybox(0), title_box(0)
{
    // Name kuerzen
    name = short_name(name);

    VSLArg args[3];
    args[0] = disp_nr;
    args[1] = tag (name);

    title_box = eval("title", args);
    
    args[0] = title_box;
    if (dv)
	args[1] = create_value_box(dv);
    else
	args[1] = eval("disabled");

    mybox = eval("display_box", args);
}


// ***************************************************************************
//
void DispBox::init_vsllib()
{
    static const char builtin_def[] = 
#include "ddd.vsl.h"
	;

    // Set include search path
    VSEFlags::include_search_path = 
	strcpy(new char[vsllib_path.length()], vsllib_path);

    // Load library
    if (string(vsllib_name) != "builtin")
    {
	StatusDelay delay("Reading VSL library " + quote(vsllib_name));
	vsllib_ptr = new VSLLib (vsllib_name);
    }

    if (vsllib_ptr == 0)
    {
	StatusDelay delay("Reading builtin VSL library");
	istrstream is(builtin_def);
	vsllib_ptr = new VSLLib(is);
    }

    assert (vsllib_ptr != 0);
}


// ***************************************************************************
//
DispBox::~DispBox ()
{
    mybox->unlink();
    title_box->unlink();
}

// ***************************************************************************
//
void DispBox::set_value (const DispValue* dv)
{
    VSLArg args[3];

    args[0] = title_box;
    if (dv)
	args[1] = create_value_box(dv);
    else
	args[1] = eval("disabled");

    mybox->unlink();

    mybox = eval("display_box", args);
}


// ***************************************************************************
// Versucht, die Display-Namen auf eine Laenge <= app_data.max_name_length
// zu kuerzen.
//
string DispBox::short_name (string name, int length)
{
    if (length == 0)
	return name;

    // Strip DBX scope information
    while (name.contains(RXscope))
    {
	string postfix = name.after(RXscope);
	name = name.before(RXscope);
	name += postfix;
    }

    // Strip pointer chains
    while (name.length() > unsigned(length)
	   && name.contains(RXpointer_name_pointer))
    {
	string postfix = name.after(RXpointer_name_pointer);
	name = name.before(RXpointer_name_pointer);
	name += "->..->";
	name += postfix;
    }

    return name;
}


// ***************************************************************************
// Create a Box for the value DV
Box* DispBox::create_value_box (const DispValue* dv, int member_name_width)
{
    Box* vbox = 0;

    switch (dv->type())
    {
    case Simple:
	if (dv->collapsed())
	    vbox = eval("collapsed_simple_value");
	else
	    vbox = eval("simple_value", dv->value());
	break;

    case Pointer:
	if (dv->collapsed())
	    vbox = eval("collapsed_pointer_value");
	else if (dv->dereferenced())
	    vbox = eval("dereferenced_pointer_value", dv->value());
	else
	    vbox = eval("pointer_value", dv->value());
	break;

    case Array:
	if (dv->collapsed())
	    vbox = eval("collapsed_array");
	else
	{
	    int count = dv->number_of_childs();
	    ListBox* args = new ListBox;
	    for (int i = 0; i < count; i++)
		*args += create_value_box (dv->get_child(i));

	    if (dv->vertical_aligned())
		vbox = eval("vertical_array", args);
	    else
		vbox = eval("horizontal_array", args);

	    args->unlink();
	}
	break;

    case StructOrClass:
    case BaseClass:
	if (dv->collapsed())
	    vbox = eval("collapsed_struct_value");
	else
	{
	    int count = dv->number_of_childs();

	    // Determine maximum member name width
	    int max_member_name_width = 0;
	    int i;
	    for (i = 0; i < count; i++)
	    {
		string child_member_name = dv->get_child(i)->name();
		Box *box = eval("struct_member_name", child_member_name);
		max_member_name_width = 
		    max(max_member_name_width, box->size(X));
	        box->unlink();
	    }

	    // Create children
	    ListBox* args = new ListBox;
	    for (i = 0; i < count; i++)
		*args += create_value_box(dv->get_child(i), 
					  max_member_name_width);

	    vbox = eval("struct_value", args);
	    args->unlink();
	}
	break;

    case Reference:
	if (dv->collapsed())
	    vbox = eval("collapsed_reference_value");
	else
	{
	    ListBox* args = new ListBox;
	    for (int i = 0; i < 2; i++)
		*args += create_value_box (dv->get_child(i));

	    vbox = eval("reference_value", args);
	    args->unlink();
	}
	break;

    default:
	// Illegal type
	assert (0);
	break;
    }

    // Add member name
    if (dv->depth() > 0 && 
	(dv->parent()->type() == StructOrClass || 
	 dv->parent()->type() == BaseClass))
    {
	vbox = eval("struct_member", dv->name(), vbox->link(), 
		    member_name_width);
    }

    Data* data = (Data *)dv;
    vbox = vbox->tag(data);

    return vbox;
}



// ***************************************************************************
// Duplication with special handling of undefined boxes
Box *DispBox::dup(const string& func_name, const Box *box)
{
    if (box)
	return ((Box *)box)->link();
    else
	return new StringBox("<?" + func_name + ">"); // box not found
}
