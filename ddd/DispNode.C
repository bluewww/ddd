// $Id$
// Store information about a single display espression

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

char DispNode_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// DispNode speichert Informationen zu einem Display-Ausdruck
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

#include "DispNode.h"
#include "CompositeB.h"

//-----------------------------------------------------------------------------

HandlerList DispNode::handlers(DispNode_NTypes);

// ***************************************************************************
// Constructor
//
DispNode::DispNode (string& disp_nr, string& name)
    : mydisp_nr (disp_nr),
      myname (name),
      myenabled (false), 
      mynodeptr (0),
      disp_value (0),
      myselected_value (0),
      disp_box (0)
{
    // Knoten neu erzeugen
    disp_box = new DispBox (mydisp_nr, myname, disp_value);
    
    // Knoten darstellen
    mynodeptr = new BoxGraphNode (disp_box->box());
}

DispNode::DispNode (string& disp_nr, string& name, string& value)
    : mydisp_nr (disp_nr),
      myname (name),
      myenabled (true), 
      mynodeptr (0),
      disp_value (0),
      myselected_value (0),
      disp_box (0)
{
    disp_value = new DispValue (0, 0, value, myname, myname);

    // Knoten neu erzeugen
    disp_box = new DispBox (disp_nr, name, disp_value);
    
    // Knoten darstellen
    mynodeptr = new BoxGraphNode (disp_box->box());
}

// ***************************************************************************
// Destructor
//
DispNode::~DispNode()
{
    if (disp_value)
	delete disp_value;
    if (mynodeptr)
	delete mynodeptr;
    if (disp_box)
	delete disp_box;
}


// ***************************************************************************
void DispNode::addHandler (unsigned    type,
			   HandlerProc proc,
			   void*       client_data)
{
    DispNode::handlers.add(type, proc, client_data);
}

// ***************************************************************************
void DispNode::removeHandler (unsigned    type,
			      HandlerProc proc,
			      void        *client_data)
{
    DispNode::handlers.remove(type, proc, client_data);
}



// ***************************************************************************
// false, wenn Wert unveraendert.
//
bool DispNode::update(string& value)
{
    bool changed = false;
    bool inited  = false;

    if (!myenabled) { //Display wurde (automatisch) eingeschaltet
	myenabled = true;
	handlers.call(DispNode_Disabled, this, (void*)false);
	changed = true;
    }

    if (disp_value == 0) { // noch keinen Wert gelesen
	disp_value = new DispValue (0, 0, value, myname, myname);
	changed = true;
    }
    else { 
	disp_value->update(value, changed, inited);
    }

    if (inited)
    {
	// cerr << "Warning: display type " + disp_nr() + " changed\n";

	// We were reinitialized: disable old selection
	myselected_value = disp_value;
    }
	
    if (changed)
    {
	disp_box->set_value(disp_value);

	// neue Box setzen
	mynodeptr->setBox(disp_box->box());
    }
    return changed;
}


// ***************************************************************************
// Re-create box from current disp_value
//
void DispNode::refresh ()
{
    disp_box->set_value(disp_value);
    mynodeptr->setBox(disp_box->box());
    select(selected_value());
}

// ***************************************************************************
// Highlights the box related to the display value DV
//

// In BOX, find outermost TagBox for given DispValue DV
static TagBox *findTagBox(const Box *box, DispValue *dv)
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

void DispNode::select(DispValue *dv)
{
    TagBox *tb = 0;
    if (dv != 0)
	tb = findTagBox(box(), dv);

    nodeptr()->setHighlight(tb);
    myselected_value = dv;
}


// ***************************************************************************
void DispNode::disable()
{
    if (myenabled) {
	myenabled = false;
	handlers.call(DispNode_Disabled, this, (void*)true);
	disp_box->set_value();
	mynodeptr->setBox (disp_box->box());
    }
}
