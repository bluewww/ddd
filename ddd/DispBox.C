// $Id$
// Display boxes

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
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
#include "DispNode.h"
#include "strclass.h"
#include "bool.h"
#include "cook.h"
#include "ddd.h"
#include "status.h"
#include "shorten.h"
#include "string-fun.h"
#include "tabs.h"
#include "version.h"

#include <ctype.h>



//-----------------------------------------------------------------------------

VSLLib  DispBox::dummylib;
VSLLib* DispBox::vsllib_ptr      = &DispBox::dummylib;
string  DispBox::vsllib_name     = "builtin";
string  DispBox::vsllib_path     = ".";
string  DispBox::vsllib_defs     = "";
int     DispBox::max_display_title_length = 20;
bool    DispBox::vsllib_initialized = false;

// ***************************************************************************
//
DispBox::DispBox (string disp_nr,
		  string title,
		  const DispValue* dv)
    : mybox(0), title_box(0)
{
    // Strip DBX scope information from title
    static regex RXdbx_scope("[a-zA-Z_0-9]*`");
    while (int(title.length()) > max_display_title_length 
	   && title.contains(RXdbx_scope))
    {
	string postfix = title.after(RXdbx_scope);
	title = title.before(RXdbx_scope);
	title += postfix;
    }

    // Shorten remainder
    shorten(title, max_display_title_length);

    // Create display
    VSLArg args[3];
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

	args[0] = tag(title);
    }
    else
    {
	// Normal title: use NUMBER: EXPR
	args[0] = disp_nr;
	args[1] = tag(title);
    }

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
	strcpy(new char[vsllib_path.length()], vsllib_path);

    // Load library
    void (*old_background)() = VSLLib::background;
    VSLLib::background = background;

    if (string(vsllib_name) == "builtin")
    {
	string defs = string(builtin_def)
	    + "#line 1 \"" Ddd_NAME "*vslDefs\"\n"
	    + vsllib_defs;
	istrstream is(defs.chars());
	vsllib_ptr = new VSLLib(is);
    }
    else
    {
	vsllib_ptr = new VSLLib (vsllib_name);
    }

    VSLLib::background = old_background;
    initializing = false;
    vsllib_initialized = true;
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

    case Text:
	if (dv->collapsed())
	    vbox = eval("collapsed_text_value");
	else
	{
	    string v = dv->value();
	    read_leading_blanks(v);
	    strip_final_blanks(v);
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

    case List:
    case StructOrClass:
    case BaseClass:
	{
	    String collapsed_value = (dv->type() == List ? 
				      "collapsed_list_value" :
				      "collapsed_struct_value");
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
		int count = dv->number_of_childs();

		// Determine maximum member name width
		int max_member_name_width = 0;
		int i;
		for (i = 0; i < count; i++)
		{
		    string child_member_name = dv->get_child(i)->name();
		    Box *box = eval(member_name, child_member_name);
		    max_member_name_width = 
			max(max_member_name_width, box->size(X));
		    box->unlink();
		}

		// Create children
		ListBox* args = new ListBox;
		for (i = 0; i < count; i++)
		    *args += create_value_box(dv->get_child(i), 
					      max_member_name_width);

		vbox = eval(value, args);
		args->unlink();
	    }
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
    }

    // Add member name
    if (dv->depth() > 0)
    {
	switch (dv->parent()->type())
	{
	case List:
	    if (dv->type() != Text)
		vbox = eval("list_member", dv->name(), vbox->link(), 
			    member_name_width);
	    break;

	case StructOrClass:
	case BaseClass:
	    vbox = eval("struct_member", dv->name(), vbox->link(), 
			member_name_width);
	    break;

	case Array:
	case Reference:
	case Simple:
	case Text:
	case Pointer:
	    break;
	}
    }

    Data* data = (Data *)dv;
    vbox = vbox->tag(data);

    return vbox;
}

// ***************************************************************************
// Duplication with special handling of undefined boxes
Box *DispBox::dup(const string& func_name, const Box *box)
{
    if (box != 0)
	return ((Box *)box)->link();

    return (new StringBox("<?" + func_name + ">"))->link(); // box not found
}
