// $Id$
// Display boxes

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2000 Universitaet Passau, Germany.
// Copyright (C) 2001, 2003 Free Software Foundation, Inc.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>
// and Andreas Zeller <zeller@gnu.org>
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

char DispBox_rcsid[] =
    "$Id$";

//-----------------------------------------------------------------------------
// Create DispBoxes via VSL
//-----------------------------------------------------------------------------

#include "assert.h"
#include "DispBox.h"
#include "StringBox.h"
#include "ColorBox.h"
#include "DispNode.h"
#include "GDBAgent.h"
#include "VSEFlags.h"
#include "strclass.h"
#include "bool.h"
#include "cook.h"
#include "ddd.h"
#include "post.h"
#include "status.h"
#include "shorten.h"
#include "string-fun.h"
#include "tabs.h"
#include "version.h"
#include "regexps.h"
#include "resolveP.h"

#include <ctype.h>

#define assert_ok(x) assert(x)

#ifndef CACHE_BOXES
#define CACHE_BOXES 1
#endif

#ifndef LOG_BOX_CACHE
#define LOG_BOX_CACHE 0
#endif

#ifndef CACHE_LIBS
#define CACHE_LIBS 1
#endif

#ifndef LOG_LIB_CACHE
#define LOG_LIB_CACHE 0
#endif


//-----------------------------------------------------------------------------

ThemedVSLLib  DispBox::dummylib;
ThemedVSLLib *DispBox::vsllib_ptr = &DispBox::dummylib;

string  DispBox::vsllib_name      = "builtin";
string  DispBox::vsllib_path      = ".";
string  DispBox::vsllib_defs      = "";
string  DispBox::vsllib_base_defs = "";
int     DispBox::max_display_title_length = 20;
bool    DispBox::vsllib_initialized = false;
bool    DispBox::align_2d_arrays = true;

ThemeManager DispBox::theme_manager;


// ***************************************************************************
//
DispBox::DispBox (int disp_nr, const string& title, 
		  const DispValue *dv, const DispValue *parent)
    : mybox(0), title_box(0)
{
    // Create display
    set_title(dv, disp_nr, title);
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

#if WITH_BUILTIN_VSLLIB
    static const char *builtin_def = 
#include "ddd.vsl.h"
	;
#endif

    // Set include search path
    VSEFlags::include_search_path = 
	strcpy(new char[vsllib_path.length() + 1], vsllib_path.chars());

    // Delete old library
    if (vsllib_ptr != &DispBox::dummylib)
    {
	delete vsllib_ptr;
	vsllib_ptr = &DispBox::dummylib;
    }

    // Load new library
    void (*old_background)() = VSLLib::background;
    VSLLib::background = background;

    string name = vsllib_name;
    if (string(vsllib_name) == "builtin")
    {
#if WITH_BUILTIN_VSLLIB
	string defs = string(builtin_def) +
	    "#line 1 \"" Ddd_NAME "*vslBaseDefs\"\n" +
	    vsllib_base_defs +
	    "#line 1 \"" Ddd_NAME "*vslDefs\"\n" +
	    vsllib_defs;

	std::istringstream is(defs.chars());
	vsllib_ptr = new ThemedVSLLib(is, VSEFlags::optimize_mode());
#else
	name = "vsllib/" ddd_NAME ".vsl";
	vsllib_ptr = new ThemedVSLLib();
	const string path = resolvePath(name);
	if (!path.empty())
	{
	    vsllib_ptr->update(path);

	    string defs =
		"#line 1 \"" Ddd_NAME "*vslBaseDefs\"\n" +
		vsllib_base_defs +
		"#line 1 \"" Ddd_NAME "*vslDefs\"\n" +
		vsllib_defs;

	    std::istringstream is(defs.chars());
	    vsllib_ptr->update(is);
	    vsllib_ptr->optimize(VSEFlags::optimize_mode());
	}
#endif
    }
    else
    {
	vsllib_ptr = new ThemedVSLLib(vsllib_name, VSEFlags::optimize_mode());
    }

    // Check for the most basic VSL functions
    if (vsllib_ptr->has("display_box") && vsllib_ptr->has("value_box"))
    {
	// Everything is fine
    }
    else
    {
	// Could not read in VSL library properly.  Give a warning.
	post_error("Could not load VSL Library " + quote(name),
		   "no_vsllib_error");
    }

    VSLLib::background = old_background;
    initializing = false;
    vsllib_initialized = true;
}

struct VSLLibCache {
    StringArray themes;
    VSLLib *lib;
    VSLLibCache *next;

    VSLLibCache(const StringArray& _themes, const VSLLib *_lib, 
		VSLLibCache *_next = 0)
	: themes(_themes), lib(_lib->dup()), next(_next)
    {}

    ~VSLLibCache()
    {
	delete next;
	delete lib;
    }

private:
    VSLLibCache(const VSLLibCache&);
    VSLLibCache& operator= (const VSLLibCache&);

} *DispBox::vsllib_cache = 0;

VSLLib *DispBox::vsllib(const DispValue *dv)
{
    if (!vsllib_initialized)
	init_vsllib();

    if (dv == 0)
	return vsllib_ptr;

    string expr = dv->full_name();
    StringArray themes = theme_manager.themes(expr);

#if LOG_LIB_CACHE
    std::clog << "Searching lib for theme";
    for (int i = 0; i < themes.size(); i++)
	std::clog << " " << themes[i];
    std::clog << "\n";
#endif

    int count = 0;

    // Check whether we have such a library in the cache
    for (VSLLibCache *p = vsllib_cache; p != 0; p = p->next)
    {
	if (p->themes == themes)
	    return p->lib;

	count++;
    }

#if LOG_LIB_CACHE
    std::clog << "Adding new lib: " << count + 1 << " libs in cache\n";
#endif

    // Not found.  Set theme list and keep a copy in the cache.
    vsllib_ptr->set_theme_list(themes);

#if CACHE_LIBS
    vsllib_cache = new VSLLibCache(themes, vsllib_ptr, vsllib_cache);
#endif

    return vsllib_ptr;
}

void DispBox::clear_vsllib_cache()
{
#if LOG_LIB_CACHE
    std::clog << "Clearing lib cache: " << 0 << " libs in cache\n";
#endif

    delete vsllib_cache;
    vsllib_cache = 0;
}

// ***************************************************************************
//
DispBox::~DispBox ()
{
    assert_ok(mybox == 0 || mybox->OK());

    if (mybox != 0)
	mybox->unlink();

    assert_ok(title_box == 0 || title_box->OK());

    if (title_box != 0)
	title_box->unlink();
}

// ***************************************************************************
//
void DispBox::set_value(const DispValue* dv, const DispValue *parent)
{
    if (mybox != 0)
    {
	mybox->unlink();
	mybox = 0;
    }

    VSLArgList args;
    if (title_box != 0)
	args += title_box->link();

    if (dv != 0)
	((DispValue *)dv)->validate_box_cache();

    args += create_value_box(dv, parent);
    mybox = eval(dv, "display_box", args);

    assert_ok(mybox->OK());
}

void DispBox::shorten_title(string& name)
{
    if (name.empty())
	return;

    if (!is_user_command(name))
    {
	// Strip DBX scope information from title
#if RUNTIME_REGEX
	static regex rxdbx_scope("[a-zA-Z_0-9]*`");
#endif
	int i = 0;
	while (int(name.length()) > max_display_title_length 
	       && ((i = name.index(rxdbx_scope)) >= 0))
	{
	    name = name.before(i) + name.after('`');
	}
    }

    // Shorten remainder
    shorten(name, max_display_title_length);
}


// Default duplicator
Data *DispBox::link_link(Data *dta)
{
    return dta;
}

// Default deletion
void DispBox::link_unlink(Data *)
{
    return;
}

// Default selection
bool DispBox::link_selected(Data *)
{
    return false;   // Default: non-selected
}

string DispBox::link_name(Data *d)
{
    DispValue *dv = (DispValue *)d;
    if (dv == 0)
	return "";

    return dv->full_name();
}

// Default tag info
string DispBox::link_info(Data *d)
{
    return link_name(d);   // Default: none
}

DataLink DispBox::data_link = {
    DispBox::link_link,		// link
    DispBox::link_unlink,	// unlink
    DispBox::link_selected,	// selected
    DispBox::link_name,	        // name of data
    DispBox::link_info		// debugging info of data
};

void DispBox::set_title(const DispValue *dv, int disp_nr, const string& name)
{
    if (title_box != 0)
    {
	title_box->unlink();
	title_box = 0;
    }

    // Create title
    if (!name.empty())
    {
	VSLArg args[3];
	int arg = 0;

	if (is_user_command(name))
	{
	    // User command: use NAME
	    string title = DispValue::make_title(name);
	    shorten_title(title);
	    args[arg++] = tag(title);
	}
	else
	{
	    // Normal title: use NUMBER: NAME
	    args[arg++] = itostring(disp_nr);
	    string name_( name );
	    shorten_title(name_);
	    args[arg++] = tag(name_);
	}

	title_box = eval(dv, "title", args);

	assert_ok(title_box->OK());
    }
}

// Return true if DV is a (right-aligned) numeric value
bool DispBox::is_numeric(const DispValue *dv, const DispValue *parent)
{
    if (parent != 0 && parent->type() != Array)
	return false;

    if (dv->value().empty())
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
    assert(dv != 0);

    Box *vbox = 0;

    // Check cache first
    vbox = dv->cached_box();

    if (vbox != 0)
    {
#if LOG_BOX_CACHE
	std::clog << dv->full_name() << ": using cached box\n";
#endif

	vbox = vbox->link();
	assert_ok(vbox->OK());
	return vbox;
    }

#if LOG_BOX_CACHE
    std::clog << dv->full_name() << ": computing new box\n";
#endif

    // Rebuild box
    switch (dv->type())
    {
    case Simple:
    {
	if (dv->collapsed())
	    vbox = eval(dv, "collapsed_simple_value");
	else
	{
	    // Flush numeric values to the right, unless in a struct
	    if (is_numeric(dv, parent))
		vbox = eval(dv, "numeric_value", dv->value());
	    else
		vbox = eval(dv, "simple_value", dv->value());
	}
	break;
    }

    case Text:
    {
	if (dv->collapsed())
	    vbox = eval(dv, "collapsed_text_value");
	else
	{
	    string v = dv->value();
	    strip_space(v);
	    untabify(v);

	    int n = v.freq('\n');
	    string *lines = new string[n + 1];
	    split(v, lines, n + 1, '\n');

	    VSLArgList args;
	    for (int i = 0; i < n + 1; i++)
	    {
		if (lines[i].empty())
		    lines[i] = " ";
		args += eval(dv, "text_line", lines[i]);
	    }
	    vbox = eval(dv, "text_value", args);

	    delete[] lines;
	}
	break;
    }

    case Pointer:
    {
	if (dv->collapsed())
	    vbox = eval(dv, "collapsed_pointer_value");
	else if (dv->dereferenced())
	    vbox = eval(dv, "dereferenced_pointer_value", dv->value());
	else
	    vbox = eval(dv, "pointer_value", dv->value());
	break;
    }

    case Array:
    {
	if (dv->collapsed())
	    vbox = eval(dv, "collapsed_array");
	else
	{
	    int count = dv->nchildren();
	    if (count == 0)
	    {
		vbox = eval(dv, "empty_array");
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

		    if (dv->orientation() == Vertical)
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
				Box *b = eval(cc, "twodim_array_elem", 
					      create_value_box(cc, c));
				*row += b;
				b->unlink();
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
				DispValue *cc = c;
				if (i < c->nchildren())
				{
				    cc = c->child(i);
				    elem = create_value_box(cc, c);
				}
				else
				{
				    elem = new ListBox;
				}

				Box *b = eval(cc, "twodim_array_elem", elem);
				*row += b;
				b->unlink();
			    }

			    *table += row;
			    row->unlink();
			}
		    }

		    vbox = eval(dv, "twodim_array", table);
		}
		else
		{
		    // One-dimensional array
		    VSLArgList args;
		    for (int i = 0; i < count; i++)
			args += create_value_box(dv->child(i), dv);

		    if (dv->orientation() == Vertical)
			vbox = eval(dv, "vertical_array", args);
		    else
			vbox = eval(dv, "horizontal_array", args);
		}
	    }
	}
	break;
    }

    case Sequence:
    {
	if (dv->collapsed())
	    vbox = eval(dv, "collapsed_sequence_value");
	else
	{
	    // Create children
	    VSLArgList args;
	    int count = dv->nchildren();
	    for (int i = 0; i < count; i++)
		args += create_value_box(dv->child(i), dv);

	    vbox = eval(dv, "sequence_value", args);
	}
	break;
    }

    case List:
    case Struct:
    {
	const _XtString collapsed_value = (dv->type() == List ? 
				  "collapsed_list_value" :
				  "collapsed_struct_value");
	const _XtString empty_value     = (dv->type() == List ? 
				  "empty_list_value" :
				  "empty_struct_value");
	const _XtString member_name     = (dv->type() == List ? 
				  "list_member_name" :
				  "struct_member_name");
	const _XtString value           = (dv->type() == List ? 
				  "list_value" :
				  "struct_value");
	const _XtString horizontal      = (dv->type() == List ? 
				  "horizontal_unnamed_list" :
				  "horizontal_unnamed_struct");
	const _XtString vertical        = (dv->type() == List ? 
				  "vertical_unnamed_list" :
				  "vertical_unnamed_struct");

	int count = dv->nchildren();

	if (dv->collapsed())
	    vbox = eval(dv, collapsed_value);
	else if (count == 0)
	    vbox = eval(dv, empty_value);
	else if (!dv->member_names())
	{
	    // Create object with unnamed members
	    VSLArgList args;
	    for (int i = 0; i < count; i++)
		args += create_value_box(dv->child(i), dv);

	    if (dv->orientation() == Vertical)
		vbox = eval(dv, vertical, args);
	    else
		vbox = eval(dv, horizontal, args);
	}
	else
	{
	    // Determine maximum member name width
	    int max_member_name_width = 0;
	    int i;
	    for (i = 0; i < count; i++)
	    {
		string child_member_name = dv->child(i)->name();
		Box *box = eval(dv->child(i), member_name, child_member_name);
		max_member_name_width = 
		    max(max_member_name_width, box->size(X));
		box->unlink();
	    }

	    // Create children
	    VSLArgList args;
	    for (i = 0; i < count; i++)
		args += create_value_box(dv->child(i), dv,
					 max_member_name_width);

	    vbox = eval(dv, value, args);
	}
	break;
    }

    case Reference:
    {
	if (dv->collapsed())
	    vbox = eval(dv, "collapsed_reference_value");
	else
	{
	    VSLArgList args;
	    for (int i = 0; i < 2; i++)
		args += create_value_box(dv->child(i), dv);

	    vbox = eval(dv, "reference_value", args);
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
	vbox = eval(dv, "repeated_value", vbox, dv->repeats());
    }

    // Highlight if value changed
    if (dv->is_changed())
    {
	vbox = eval(dv, "changed_value", vbox);
    }

    // Finalize the whole thing (a hook for arbitrary commands)
    vbox = eval(dv, "value_box", vbox);

    assert_ok(vbox->OK());

#if CACHE_BOXES
    ((DispValue *)dv)->set_cached_box(vbox);
#endif

    return vbox;
}

Box *DispBox::create_value_box (const DispValue *dv,
				const DispValue *parent,
				int member_name_width)
{
    Box *vbox = 0;
    if (dv == 0)
    {
	vbox = eval(dv, "none");
    }
    else if (!dv->enabled())
    {
	vbox = eval(dv, "disabled");
    }
    else
    {
	vbox = _create_value_box(dv, parent);
    }

    // Add member name
    if (dv != 0 && parent != 0 && 
	dv->type() != Text &&
	!dv->name().empty() &&
	!dv->name().matches(rxwhite))
    {
	switch (parent->type())
	{
	case List:
	    if (parent->member_names())
		vbox = eval(dv, "list_member", dv->name(), " = ", 
			    vbox, member_name_width);
	    else
		vbox = eval(dv, "list_member", vbox);
	    break;

	case Struct:
	    if (parent->member_names())
		vbox = eval(dv, "struct_member", 
			    dv->name(), gdb->member_separator(), 
			    vbox, member_name_width);
	    else
		vbox = eval(dv, "struct_member", vbox);
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

    // Finalize the whole thing (a hook for arbitrary commands)
    vbox = eval(dv, "value_box", vbox);

    if (dv != 0)
    {
	Data *data = (Data *)dv;
	vbox = vbox->tag(data, &data_link);
    }

    assert_ok(vbox->OK());

    return vbox;
}

// Check evaluation result
Box *DispBox::check(const string& func_name, const Box *box)
{
    if (box != 0)
	return ((Box *)box);

    // Box not found
    const string bad_func_name = "<?" + func_name + ">";
    return new ForegroundColorBox(new StringBox(bad_func_name), "red");
}
