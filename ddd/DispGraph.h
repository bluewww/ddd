// $Id$
// Store information about all displayed display expressions

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

#ifndef _DDD_DispGraph_h
#define _DDD_DispGraph_h

#ifdef __GNUG__
#pragma interface
#endif

//-----------------------------------------------------------------------------
// The `DispGraph' class keeps all displayed display expressions
//-----------------------------------------------------------------------------

// Misc includes
#include "strclass.h"
#include "bool.h" 
#include "Graph.h" 
#include "HandlerL.h"
#include "LineGraphE.h"
#include "PosGraphN.h"

#include "DispNodeM.h"

//-----------------------------------------------------------------------------

enum Displaying {Both, Enabled, Disabled};

// Event types
const unsigned DispGraph_Empty = 0;          // DispGraph is empty
const unsigned NoEnabled  = DispGraph_Empty + 1;
const unsigned NoDisabled = NoEnabled + 1;

const unsigned DispGraph_NTypes = NoDisabled + 1;

class DispGraph: public Graph {
    DispNodeMap  idMap;
    HandlerList  handlers;
    bool         no_enabled;
    bool         no_disabled;

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


    // Insert a new display DN named DISP_NR dependent on DEPENDS_ON
    int insert(int disp_nr, DispNode* dn, int depends_on = 0);

    // Make DISP_NR an alias of ALIAS_DISP_NR.  Suppress
    // ALIAS_DISP_NR.  Return true iff changed.
    bool alias (Widget w, int disp_nr, int alias_disp_nr);

    // Un-alias ALIAS_DISP_NR.  Unsuppress ALIAS_DISP_NR.  Return true
    // iff changed.
    bool unalias (int alias_disp_nr);

    // Determine default positions for NEW_NODE
    BoxPoint default_pos(DispNode *new_node, Widget w, 
			 int depends_on = 0) const;

    // Delete DISP_NR; return false if non-existent
    bool del (int disp_nr);

    DispNode* get       (int disp_nr) const { return idMap.get (disp_nr); }
    int       get_nr    (BoxGraphNode* nodeptr) const;

    bool contains (int disp_nr) const { return idMap.contains (disp_nr); }
    
    // Return first/next node; return 0 if non-existent
    DispNode* first (MapRef& ref, Displaying e = Both) const;
    DispNode* next  (MapRef& ref, Displaying e = Both) const;

    // Return first/next node number; return 0 if non-existent
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

    // Add a new alias edge
    void add_alias_edge(Widget w, int alias_disp_nr,
			GraphNode *from, GraphNode *to);

private:
    static void disp_node_disabledHP(void *, void *, void *);

    // Alias edge routing stuff
    // True iff R->P1 and R->P2 have the same angle
    static bool same_angle(const BoxPoint& r,
			   const BoxPoint& p1, const BoxPoint& p2);

    // True iff NODE is attached to an edge with the same angle as P
    static bool has_angle(PosGraphNode *node, const BoxPoint& p);

    void add_direct_alias_edge(Widget w, int alias_disp_nr, 
			       GraphNode *from, GraphNode *to);
    void add_routed_alias_edge(Widget w, int alias_disp_nr, 
			       PosGraphNode *from, PosGraphNode *to);
    bool is_hidden(Widget w, const BoxPoint& p) const;
    static BoxPoint rotate_offset(const BoxPoint& p, int angle);

    // Check whether P1 and P2 are okay as hint positions for FROM and TO
    bool hint_positions_ok(Widget w,
			   PosGraphNode *from, PosGraphNode *to,
			   const BoxPoint& p1, const BoxPoint& p2) const;
};

#endif // _DDD_DispGraph_h
// DON'T ADD ANYTHING BEHIND THIS #endif
