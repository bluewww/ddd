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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

//-----------------------------------------------------------------------------
// DispNode speichert Informationen zu einem Display-Ausdruck
//-----------------------------------------------------------------------------

#ifndef _DispNode_h
#define _DispNode_h

#ifdef __GNUG__
#pragma interface
#endif

// Misc includes
#include "strclass.h"
#include "bool.h"
#include "GraphNode.h"
#include "HandlerL.h"
#include "BoxGraphN.h"

// DDD includes
#include "DispValue.h"
#include "DispBox.h"

// Event-Typen
const unsigned DispNode_Disabled   = 0;

const unsigned DispNode_NTypes  = DispNode_Disabled + 1;

//-----------------------------------------------------------------------------
// Die Klasse DispNode
//-----------------------------------------------------------------------------
class DispNode {
private:
    string        mydisp_nr;
    string        myname;
    bool          myenabled;
    BoxGraphNode* mynodeptr;
    DispValue*    disp_value;
    DispValue*    myselected_value;

    static HandlerList handlers;
public:
    // Constructor
    DispNode(string& disp_nr, string& name);
    DispNode(string& disp_nr, string& name, string& value);

    // Destructor
    ~DispNode();

    string  disp_nr()  const { return mydisp_nr; }
    string  name()     const { return myname; }
    bool enabled()  const { return myenabled; }
    bool disabled() const { return !myenabled; }

    BoxGraphNode* nodeptr() const { return mynodeptr; }
    const Box*    box()     const { return mynodeptr->box(); }
    DispValue*    value()   const { return disp_value; }
    DispValue*    selected_value() const { return myselected_value; }

    bool& selected()                    { return mynodeptr->selected(); }
    void moveTo(const BoxPoint& newPos) { mynodeptr->moveTo(newPos); }

    static void addHandler (unsigned    type,
			    HandlerProc proc,
			    void*       client_data = 0);

    static void removeHandler (unsigned    type,
			       HandlerProc proc,
			       void        *client_data = 0);



    // false, wenn Wert unveraendert.
    bool update (string& new_value);

    // erzeugt die Box neu aus dem disp_value
    void refresh ();

    // highlights the box related to the display value DV
    void select (DispValue *dv = 0);

    void disable();
private:
    DispBox* disp_box;
};

#endif
