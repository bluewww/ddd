// $Id$
// Display boxes

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the GDB-based data display debugger.
// Contact ddd@ips.cs.tu-bs.de for details.

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


//-----------------------------------------------------------------------------

static regex RXpointer_name_pointer
("->\\(\\.\\.->\\)?[a-zA-Z_][]a-zA-Z_0-9\\[\\.]*->", true);

static regex RXscope("[a-zA-Z_0-9]*`", true);

//-----------------------------------------------------------------------------

VSLLib* DispBox::vsllibptr = 0;
int     DispBox::max_name_length = 20;

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
    args[1] = (dv == 0 ? eval("disabled") : create_value_box (dv));

    mybox = eval("display_box", args);
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
    args[1] = (dv == 0 ? eval("disabled") : create_value_box (dv));

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
    while (name.length() > length && name.contains(RXpointer_name_pointer))
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
	    for (int i = 0; i < count; i++)
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

    default:
	// Illegal type
	assert (0);
	break;
    }

    // Add member name
    if (dv->depth() > 0 && dv->parent()->type() != Array)
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
