// $Id$
// Store information about all displayed display expressions

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

//-----------------------------------------------------------------------------
// DispGraph speichert Informationen zu allen graphisch dargestellten
// Display-Objekten.
//-----------------------------------------------------------------------------

#ifndef _DispGraph_h
#define _DispGraph_h

#ifdef __GNUG__
#pragma interface
#endif

// includes von Nora-Zeugs
#include "strclass.h"
#include "bool.h" 
#include "Graph.h" 
#include "HandlerL.h"
#include "LineGraphE.h"

#include "Map.h"
#include "DispNode.h"

//-----------------------------------------------------------------------------

enum Displaying {Both, Enabled, Disabled};

// Event-Typen
const unsigned DispGraph_Empty = 0;          // DispGraph ist leer
const unsigned NoEnabled  = DispGraph_Empty + 1;
const unsigned NoDisabled = NoEnabled + 1;

const unsigned DispGraph_NTypes = NoDisabled + 1;

//-----------------------------------------------------------------------------
// Die Klasse DispGraph
//-----------------------------------------------------------------------------
class DispGraph: public Graph {
    Map<int, DispNode> idMap;
    HandlerList        handlers;
    bool            no_enabled;
    bool            no_disabled;
public:
    // Constructor
    DispGraph();

    // loescht alle
    void clear();
    
    // Destructor
    ~DispGraph();

    int count_all ()             const { return idMap.length(); }
    int count_all (Displaying e) const;
    int count_selected() const;
    bool has_enabled() const { return !no_enabled; }
    bool has_disabled() const { return !no_disabled; }


    void addHandler (unsigned    type,
		     HandlerProc proc,
		     void*       client_data = 0);

    void removeHandler (unsigned    type,
			HandlerProc proc,
			void        *client_data = 0);
    void callHandlers();


    // die display_nr bei Erfolg, 0 sonst
    int insert_new (int disp_nr, DispNode* dn);
    int insert_dependent (int new_disp_nr, DispNode* dn, int old_disp_nr);

    // Determine default positions for NEW_NODE
    BoxPoint default_new_box_point (DispNode *new_node, Widget w) const;
    BoxPoint default_dependent_box_point (DispNode *new_node, 
					  Widget w, int disp_nr) const;
    
    // false, falls nicht vorhanden
    bool   del       (int disp_nr);

    DispNode* get       (int disp_nr) const { return idMap.get (disp_nr); }
    int       get_nr    (BoxGraphNode* nodeptr) const;

    bool contains (int disp_nr) const { return idMap.contains (disp_nr); }
    
    // 0, wenn nicht vorhanden.
    DispNode* first (MapRef& ref, Displaying e = Both) const;
    DispNode* next  (MapRef& ref, Displaying e = Both) const;

    // 0, wenn nicht vorhanden.
    int first_nr (MapRef& ref, Displaying e = Both) const;
    int next_nr  (MapRef& ref, Displaying e = Both) const;

    void print(ostream& os, const GraphGC& gc = GraphGC()) const
	{ Graph::print(os, gc); }

protected:
    BoxPoint adjust_position (DispNode *new_node,
			      Widget w,
			      BoxPoint pos,
			      BoxPoint offset,
			      BoxPoint grid) const;

private:
    static void disp_node_disabledHP (void*, void*, void*);
};


#endif
