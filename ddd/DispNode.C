// $Id$
// Store information about a single display espression

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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

#include "cook.h"
#include "DispNode.h"
#include "CompositeB.h"

// Data
HandlerList DispNode::handlers(DispNode_NTypes);
int DispNode::change_tics = 0;

// Constructor
DispNode::DispNode (int disp_nr,
		    const string& name,
		    const string& scope,
		    const string& value)
    : mydisp_nr(disp_nr),
      myname(name),
      myaddr(""),
      myscope(scope),
      mydepends_on(""),
      myenabled(true),
      myactive(true),
      saved_node_hidden(false),
      mydeferred(false),
      mynodeptr(0),
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
	disp_value = new DispValue(0, 0, v, myname, myname);
	set_addr(disp_value->addr());
    }

    // Create new box from DISP_VALUE
    disp_box = new DispBox (mydisp_nr, myname, disp_value);
    
    // Create graph node from box
    mynodeptr = new BoxGraphNode (disp_box->box());
}

// Destructor
DispNode::~DispNode()
{
    delete disp_value;
    delete mynodeptr;
    delete disp_box;
}

// User-defined displays (status displays)

// Return true iff S is a user-defined display
bool is_user_command(const string& s)
{
    return s.length() > 0 && s[0] == '`' && s[s.length() - 1] == '`';
}

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

    if (!myenabled) 
    { 
	// Display has been enabled (automatically)
	myenabled = true;
	handlers.call(DispNode_Disabled, this, (void*)false);
	changed = true;
    }

    if (disp_value == 0)
    { 
	// We have not read a value yet
	disp_value = new DispValue (0, 0, value, myname, myname);
	set_addr(disp_value->addr());
	changed = true;
    }
    else
    { 
	disp_value->update(value, changed, inited);
	if (addr() != disp_value->addr())
	{
	    set_addr(disp_value->addr());
	    changed = true;
	}
    }

    if (inited)
    {
	// We were reinitialized: disable old selection
	myselected_value = disp_value;
    }
	
    if (changed)
    {
	disp_box->set_value(disp_value);

	// Set new box
	mynodeptr->setBox(disp_box->box());
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
void DispNode::refresh ()
{
    disp_box->set_value(enabled() ? disp_value : 0);
    mynodeptr->setBox(disp_box->box());
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

    nodeptr()->setHighlight(tb);
    myselected_value = dv;
}


// Disable display
void DispNode::disable()
{
    if (enabled())
    {
	myenabled = false;
	handlers.call(DispNode_Disabled, this, (void *)true);
	disp_box->set_value();
	mynodeptr->setBox (disp_box->box());
    }
}

// Enable display
void DispNode::enable()
{
    if (disabled())
    {
	myenabled = true;
	handlers.call(DispNode_Disabled, this, (void *)false);
	refresh();
    }
}

// Hide display
void DispNode::make_inactive()
{
    if (active())
    {
	saved_node_hidden = mynodeptr->hidden();
	mynodeptr->hidden() = true;
	myactive = false;
    }

    // Unselect it
    mynodeptr->selected() = false;
}

// Show display
void DispNode::make_active()
{
    if (!active())
    {
	myactive = true;
	mynodeptr->hidden() = saved_node_hidden;
    }
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
