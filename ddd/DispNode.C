// $Id$
// Store information about a single display espression

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

char DispNode_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


//-----------------------------------------------------------------------------
// A DispNode keeps all information about a single data display
//-----------------------------------------------------------------------------

#include "DispNode.h"

#include "cook.h"
#include "AppData.h"
#include "CompositeB.h"
#include "DispValue.h"
#include "DispBox.h"
#include "AliasGE.h"

#ifndef KEEP_CLUSTERED_DISPLAYS
#define KEEP_CLUSTERED_DISPLAYS 0
#endif

DEFINE_TYPE_INFO_1(DispNode, BoxGraphNode)

// Data
HandlerList DispNode::handlers(DispNode_NTypes);
int DispNode::change_tics = 0;

// Constructor
DispNode::DispNode (int disp_nr,
		    const string& name,
		    const string& scope,
		    const string& value)
    : BoxGraphNode(),
      mydisp_nr(disp_nr),
      myname(name),
      myaddr(""),
      myscope(scope),
      mydepends_on(""),
      myactive(true),
      saved_node_hidden(false),
      mydeferred(false),
      myclustered(false),
      myconstant(false),
      disp_value(0),
      myselected_value(0),
      disp_box(0),
      mylast_change(0),
      alias_of(0)
{
    mylast_change = ++change_tics;

    if (value != "")
    {
	string v = value;
	disp_value = DispValue::parse(v, myname);
	set_addr(disp_value->addr());
    }

    // Create new box from DISP_VALUE
    disp_box = new DispBox (mydisp_nr, myname, disp_value);

    // Set the box
    setBox(disp_box->box());
}

// Destructor
DispNode::~DispNode()
{
    if (disp_value != 0)
    {
	disp_value->unlink();
	disp_value = 0;
    }

    delete disp_box;
}

// User-defined displays (status displays)

// Return user command from S
string user_command(const string& s)
{
    if (is_user_command(s))
	return _unquote(s);
    else
	return "";
}

// Add event handler
void DispNode::addHandler (unsigned    type,
			   HandlerProc proc,
			   void*       client_data)
{
    DispNode::handlers.add(type, proc, client_data);
}

// Remove event handler
void DispNode::removeHandler (unsigned    type,
			      HandlerProc proc,
			      void        *client_data)
{
    DispNode::handlers.remove(type, proc, client_data);
}


// Update with VALUE;  return false if value is unchanged
bool DispNode::update(string& value)
{
    bool changed = false;
    bool inited  = false;

    if (disabled()) 
    { 
	// Display has been enabled (automatically)
	enable();
	changed = true;
    }

    if (disp_value == 0)
    { 
	// We have not read a value yet
	disp_value = DispValue::parse(value, myname);
	set_addr(disp_value->addr());
	changed = true;
    }
    else
    {
	// Update existing value
	disp_value = disp_value->update(value, changed, inited);
	if (addr() != disp_value->addr())
	{
	    set_addr(disp_value->addr());
	    changed = true;
	}
    }

    if (inited)
    {
	// We were reinitialized: disable old selection
	select(0);
    }
	
    if (changed)
    {
	disp_box->set_value(disp_value);

	// Set new box
	setBox(disp_box->box());
    }

    if (changed || inited)
    {
	// mylast_change = ++change_tics;
#if 0
	clog << "Display " << disp_nr() << " changed"
	     << " (" << mylast_change << ")\n";
#endif
    }

    return changed;
}


// Re-create box from current disp_value
void DispNode::refresh()
{
    disp_box->set_value(disp_value);
    setBox(disp_box->box());
    select(selected_value());
}

// In BOX, find outermost TagBox for given DispValue DV
TagBox *DispNode::findTagBox(const Box *box, DispValue *dv)
{
    TagBox *tb = ptr_cast(TagBox, (Box *)box);
    if (tb && tb->__data() == (void *)dv)
	return tb;

    HatBox *hb = ptr_cast(HatBox, (Box *)box);
    if (hb)
	return findTagBox(hb->box(), dv);

    CompositeBox *cb = ptr_cast(CompositeBox, (Box *)box);
    if (cb)
    {
	for (int i = 0; i < cb->nchildren(); i++)
	{
	    Box *child = (*cb)[i];
	    TagBox *tb = findTagBox(child, dv);
	    if (tb)
		return tb;
	}
    }

    return 0;
}

// Highlights the box related to the display value DV
void DispNode::select(DispValue *dv)
{
    TagBox *tb = 0;
    if (dv != 0)
	tb = findTagBox(box(), dv);

    setHighlight(tb);
    myselected_value = dv;
}

// Copy selection state from SRC
void DispNode::copy_selection_state(DispNode *src)
{
    if (value() != 0 && src != 0 && 
	src->value() != 0 && src->selected_value() != 0)
    {
	DispValue *descendant = 0;
	bool eq = value()->structurally_equal(src->value(), 
					      src->selected_value(),
					      descendant);
	if (eq)
	    select(descendant);
	return;
    }

    // Don't care - just select it all
    select(0);
}

// Disable display
void DispNode::disable()
{
    if (value() == 0 || disabled())
	return;

    value()->enabled() = false;
    handlers.call(DispNode_Disabled, this, (void *)true);
    refresh();
}

// Enable display
void DispNode::enable()
{
    if (value() == 0 || enabled())
	return;

    value()->enabled() = true;
    handlers.call(DispNode_Disabled, this, (void *)false);
    refresh();
}

// Hide display
void DispNode::make_inactive()
{
    if (active())
    {
	if (!clustered())
	{
	    saved_node_hidden = hidden();
	    hidden() = true;
	}
	myactive = false;
    }

    // Unselect it
    selected() = false;
}

// Show display
void DispNode::make_active()
{
    if (!active())
    {
	myactive = true;
	if (!clustered())
	    hidden() = saved_node_hidden;
    }
}

// Cluster display into TARGET
void DispNode::cluster(int target)
{
    if (target != 0)
    {
	if (clustered() == 0)
	    saved_node_hidden = hidden();

#if KEEP_CLUSTERED_DISPLAYS
	// Don't hide clustered displays
#else
	hidden() = true;
#endif
    }
    else // target == 0
    {
	if (clustered() != 0)
	    hidden() = saved_node_hidden;
    }

    // Set new target
    myclustered = target;

    // Unselect it
    selected() = false;
}

// Update address with NEW_ADDR
void DispNode::set_addr(const string& new_addr)
{
    if (myaddr != new_addr)
    {
	myaddr = new_addr;
	// mylast_change = ++change_tics;
#if 0
	clog << "Display " << disp_nr() << " changed"
	     << " (" << mylast_change << ")\n";
#endif
    }
}

// Return `true' if this expression can be aliased.
bool DispNode::alias_ok() const
{
    // We avoid merging displays that are derived from other displays,
    // such as `/FMT EXPR' or `(TYPE) EXPR'.
    return !is_user_command()
	&& !name().contains('/', 0) 
	&& !name().contains('(', 0);
}

// Toggle titles.  Return true if changed.
bool DispNode::set_title(bool set)
{
    bool changed = false;
    if (set && !disp_box->have_title())
    {
	// Add title
	disp_box->set_title(mydisp_nr, myname);
	changed = true;
    }
    else if (!set && disp_box->have_title())
    {
	// Remove title
	disp_box->set_title(mydisp_nr, "");
	changed = true;
    }

    if (changed)
    {
	// Set value again
	disp_box->set_value(disp_value);

	// Show new box
	setBox(disp_box->box());
    }

    return changed;
}
