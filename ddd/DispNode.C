// $Id$
// Store information about a single display espression

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
DispNode::DispNode (string& disp_nr, string& name)
    : mydisp_nr (disp_nr),
      myname (name),
      myaddr (""),
      myenabled (false),
      mynodeptr (0),
      disp_value (0),
      myselected_value (0),
      disp_box (0),
      alias_of (0)
{
    mylast_change = ++change_tics;

    // Create new box from DISP_VALUE
    disp_box = new DispBox (mydisp_nr, myname, disp_value);
    
    // Create graph node from box
    mynodeptr = new BoxGraphNode (disp_box->box());
}

DispNode::DispNode (string& disp_nr, string& name, string& value)
    : mydisp_nr (disp_nr),
      myname (name),
      myaddr (""),
      myenabled (true), 
      mynodeptr (0),
      disp_value (0),
      myselected_value (0),
      disp_box (0),
      alias_of (0)
{
    mylast_change = ++change_tics;

    disp_value = new DispValue (0, 0, value, myname, myname);

    // Create new box from DISP_VALUE
    disp_box = new DispBox (disp_nr, name, disp_value);
    
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
	changed = true;
    }
    else
    { 
	disp_value->update(value, changed, inited);
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
	mylast_change = ++change_tics;
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
    disp_box->set_value(disp_value);
    mynodeptr->setBox(disp_box->box());
    select(selected_value());
}

// In BOX, find outermost TagBox for given DispValue DV
TagBox *DispNode::findTagBox(const Box *box, DispValue *dv)
{
    TagBox *tb = ptr_cast(TagBox, box);
    if (tb && tb->__data() == (void *)dv)
	return tb;

    HatBox *hb = ptr_cast(HatBox, box);
    if (hb)
	return findTagBox(hb->box(), dv);

    CompositeBox *cb = ptr_cast(CompositeBox, box);
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
    if (myenabled) {
	myenabled = false;
	handlers.call(DispNode_Disabled, this, (void*)true);
	disp_box->set_value();
	mynodeptr->setBox (disp_box->box());
    }
}

// Enable display
void DispNode::enable()
{
    if (!myenabled)
    {
	myenabled = true;
	handlers.call(DispNode_Disabled, this, (void*)false);
	refresh();
    }
}


// Update address with NEW_ADDR
void DispNode::set_addr(const string& new_addr)
{
    if (myaddr != new_addr)
    {
	myaddr = new_addr;
	mylast_change = ++change_tics;
#if 0
	clog << "Display " << disp_nr() << " changed"
	     << " (" << mylast_change << ")\n";
#endif
    }
}
	
