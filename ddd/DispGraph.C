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

char DispGraph_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// DispGraph speichert Informationen zu allen graphisch dargestellten
// Display-Ausdruecken.
//-----------------------------------------------------------------------------

#include "DispGraph.h"
#include "GraphEdit.h"
#include "assert.h"
#include "VarArray.h"

// ***************************************************************************
// Constructor
//
DispGraph::DispGraph() :
    idMap(),
    handlers(DispGraph_NTypes),
    no_enabled(true),
    no_disabled(true),
    Graph()
{
    DispNode::addHandler(DispNode_Disabled,
			 disp_node_disabledHP,
			 (void*)this);
}

// ***************************************************************************
// Destructor
//
DispGraph::~DispGraph() 
{
    DispGraph::clear();
}

// ***************************************************************************
// loescht alle
//
void DispGraph::clear()
{
    idMap.delete_all_contents();
}


// ***************************************************************************
int DispGraph::count_all (Displaying e) const
{
    if (e == Both)
	return idMap.length();

    int count = 0;
    MapRef ref;
    for (int k = idMap.first_key(ref); k != 0; k = idMap.next_key(ref)) {
	switch (e) {
	case Both:
	    count++;
	    break;
	case Enabled:
	    if (idMap.get(k)->enabled())
		count++;
	    break;
	case Disabled:
	    if (!(idMap.get(k)->enabled()))
		count++;
	    break;
	default:
	    // Falscher Fehler
	    assert (0);
	    break;
	}
    }
    return count;
}

// ***************************************************************************
int DispGraph::count_selected() const
{
    int count = 0;
    MapRef ref;
    for (int k = idMap.first_key(ref); k != 0; k = idMap.next_key(ref)) {
	    if (!idMap.get(k)->selected())
		count++;
    }
    return count;
}

// ***************************************************************************
void DispGraph::addHandler (unsigned    type,
			    HandlerProc proc,
			    void*       client_data)
{
    handlers.add(type, proc, client_data);
}

// ***************************************************************************
void DispGraph::removeHandler (unsigned    type,
			       HandlerProc proc,
			       void        *client_data)
{
    handlers.remove(type, proc, client_data);
}

// ***************************************************************************
void DispGraph::callHandlers ()
{
    handlers.call(DispGraph_Empty,
		  this,
		  (void*)(count_all() == 0));
    handlers.call(NoEnabled,
		  this,
		  (void*)(no_enabled));
    handlers.call(NoDisabled,
		  this,
		  (void*)(no_disabled));
}



// ***************************************************************************
// disp_nr bei Erfolg
//
int DispGraph::insert_new (int disp_nr, DispNode* dn)
{
    if (idMap.contains(disp_nr))
	return 0;
    if (idMap.length() == 0)
	handlers.call(DispGraph_Empty, this, (void*)false);

    Graph* tmp = (Graph *) this;
    *tmp += dn->nodeptr();
    assert (Graph::OK());
    idMap.insert (disp_nr, dn);

    if (no_enabled) {
	if (!( no_enabled = (count_all(Enabled) == 0) ))
	    handlers.call(NoEnabled, this, (void*)false);
    }
    if (no_disabled) {
	if (!( no_disabled = (count_all(Disabled) == 0) ))
	    handlers.call(NoDisabled, this, (void*)false);
    }

    return disp_nr;
}

// ***************************************************************************
// new_disp_nr bei Erfolg
//
int DispGraph::insert_dependent (int       new_disp_nr,
				 DispNode* new_dn,
				 int       old_disp_nr)
{
    assert (old_disp_nr != 0);
    assert (idMap.contains (old_disp_nr));
    if (idMap.contains(new_disp_nr))
	return 0;

    DispNode* old_dn = idMap.get (old_disp_nr);

    Graph* tmp = (Graph *) this;
    *tmp += new_dn->nodeptr();
    *tmp += new LineGraphEdge (old_dn->nodeptr(), new_dn->nodeptr());

    idMap.insert (new_disp_nr, new_dn);

    if (no_enabled) {
	if (!( no_enabled = (count_all(Enabled) == 0) ))
	    handlers.call(NoEnabled, this, (void*)false);
    }
    if (no_disabled) {
	if (!( no_disabled = (count_all(Disabled) == 0) ))
	    handlers.call(NoDisabled, this, (void*)false);
    }

    return new_disp_nr;
}

// ***************************************************************************
BoxPoint DispGraph::adjust_position (DispNode *new_node,
				     Widget w,
				     BoxPoint pos,
				     BoxPoint offset,
				     BoxPoint grid) const
{
    const GraphGC& graphGC = graphEditGetGraphGC(w);

    // clog << "new node       at " << pos << "\n";

    BoxSize new_size(new_node->box()->size());

    // Leave GRID space around the box
    BoxRegion new_region(pos - (new_size + grid) / 2, new_size + grid);

    // Make sure the new node is fully visible
    while (new_region.origin()[X] <= 0)
    {
	pos                 += BoxPoint(grid[X], 0);
	new_region.origin() += BoxPoint(grid[X], 0);
	// clog << "new node now   at " << pos << "\n";
    }

    while (new_region.origin()[Y] <= 0)
    {
	pos                 += BoxPoint(0, grid[Y]);
	new_region.origin() += BoxPoint(0, grid[Y]);
	// clog << "new node now   at " << pos << "\n";
    }

    // Make sure the new node does not obscure existing nodes
    GraphNode *n = firstNode();
    while (n)
    {
	const BoxRegion& region = n->region(graphGC);
	if (new_region <= region)
	{
	    pos                 += offset;
	    new_region.origin() += offset;

	    n = firstNode();
	    // clog << "new node now   at " << pos << "\n";
	}
	else
	    n = nextNode(n);
    }

    return pos;
}

// ***************************************************************************
BoxPoint DispGraph::default_dependent_box_point (DispNode *new_node,
						 Widget w, int disp_nr) const
{
    Dimension grid_height = 16;
    Dimension grid_width  = 16;
    Cardinal rotation     = 0;
    XtVaGetValues(w,
		  XtNgridHeight, &grid_height,
		  XtNgridWidth,  &grid_width,
		  XtNrotation,   &rotation,
		  NULL);

    BoxPoint grid(grid_height, grid_width);
    BoxPoint delta = grid * 2;

    bool horizontal = rotation % 90;

    BoxGraphNode *node = idMap.get(disp_nr)->nodeptr();

    // clog << "node           at " << node->pos() << "\n";

    // Default: place new node below/on the right of original node,
    // depending on last layout orientation.
    //
    // NODE -> (new)

    BoxPoint offset = horizontal ? BoxPoint(0, delta[Y]) 
	                         : BoxPoint(delta[X], 0);

    BoxPoint pos = node->pos() + offset;

    // Check if we already have a successor
    BoxGraphNode *max_child      = 0;
    BoxGraphNode *next_max_child = 0;

    // Find two greatest children
    for (GraphEdge *edge = node->firstFrom(); 
	 edge != 0; 
	 edge = node->nextFrom(edge))
    {
	BoxDimension d = horizontal ? X : Y;

	GraphNode *child = edge->to();
	while (child->isHint())
	    child = child->firstFrom()->to();

	BoxGraphNode *bgn = ptr_cast(BoxGraphNode, child);
	if (bgn == 0)
	    continue;

	if (max_child == 0 || child->pos()[d] > max_child->pos()[d])
	{
	    next_max_child = max_child;
	    max_child = bgn;
	}
	else if (next_max_child == 0 
		 || child->pos()[d] > next_max_child->pos()[d])
	{
	    next_max_child = bgn;
	}
    }

    if (max_child && next_max_child)
    {
	// Re-use offset between the two last children
	//
	//   NODE ->         .
	//        \->        .
	//         \->       .
	//          \->   NEXT_MAX_CHILD
	//           \->  MAX_CHILD
	//            \-> (new)

	// clog << "max_child      at " << max_child->pos() << "\n";
	// clog << "next_max_child at " << next_max_child->pos() << "\n";

	// Re-use offset between last two children
	pos = max_child->pos() + (max_child->pos() - next_max_child->pos());

	// If MAX_CHILD is on the right of NEXT_MAX_CHILD, place new
	// node on the right; if MAX_CHILD is below NEXT_MAX_CHILD,
	// place new node below.  If position is occupied, try later
	// in the same direction.
	bool horizontal = 
	    (abs(max_child->pos()[X] - next_max_child->pos()[X]) >
	     abs(max_child->pos()[Y] - next_max_child->pos()[Y]));

	offset = horizontal ? BoxPoint(delta[X], 0) : BoxPoint(0, delta[Y]);
    }
    else if (max_child)
    {
	// Place new child below last child
	//
	//   NODE ->     MAX_CHILD
	//        \->    (new)

	// clog << "child          at " << max_child->pos() << "\n";

	// If MAX_CHILD is on the right of NODE, place new node below;
	// if MAX_CHILD is below NODE, place new node on the right.
	bool horizontal = 
	    (abs(max_child->pos()[X] - node->pos()[X]) >
	     abs(max_child->pos()[Y] - node->pos()[Y]));
	offset = horizontal ? BoxPoint(0, delta[Y]) : BoxPoint(delta[X], 0);

	pos = max_child->pos() + offset;
    }
    else
    {
	GraphEdge *edge = node->firstTo();
	if (edge)
	{
	    // We have a predecessor: use this offset instead
	    //
	    // PARENT -> NODE -> (new)
	    //

	    GraphNode *parent = edge->from();

	    // clog << "parent         at " << parent->pos() << "\n";

	    // Re-use offset between parent and node
	    pos = node->pos() + (node->pos() - parent->pos());

	    // If NODE is on the right of PARENT, place new node on
	    // the right; if NODE is below PARENT, place new node
	    // below.
	    bool horizontal = 
		(abs(node->pos()[X] - parent->pos()[X]) >
		 abs(node->pos()[Y] - parent->pos()[Y]));

	    pos += horizontal ? BoxPoint(delta[X], 0) : BoxPoint(0, delta[Y]);
	    offset = horizontal ? BoxPoint(delta[X], 0) 
		                : BoxPoint(0, delta[Y]);
	}
    }

    return adjust_position(new_node, w, pos, offset, grid);
}


// ***************************************************************************
BoxPoint DispGraph::default_new_box_point (DispNode *new_node, Widget w) const
{
    Dimension grid_height = 16;
    Dimension grid_width  = 16;
    Cardinal rotation     = 0;
    XtVaGetValues(w,
		  XtNgridHeight, &grid_height,
		  XtNgridWidth,  &grid_width,
		  XtNrotation,   &rotation,
		  NULL);

    BoxPoint grid(grid_height, grid_width);

    bool horizontal = rotation % 90;

    // Default offset: create new displays orthogonal to layout direction
    BoxPoint offset = 
	horizontal ? BoxPoint(0, grid[Y]) : BoxPoint(grid[X], 0);

    // Start with this position
    BoxPoint pos(grid[X], grid[Y] * 4);

    return adjust_position(new_node, w, pos, offset, grid);
}




// ***************************************************************************

// Find all hints in edges leading to NODE; store them in HINTS
static void find_hints_to(GraphNode *node, VarArray<GraphNode *>& hints)
{
    for (GraphEdge *edge = node->firstTo();
	 edge != 0;
	 edge = node->nextTo(edge))
    {
	if (edge->from()->isHint())
	{
	    find_hints_to(edge->from(), hints);
	    hints += edge->from();
	}
    }
}

// Find all hints in edges coming from NODE; store them in HINTS
static void find_hints_from(GraphNode *node, VarArray<GraphNode *>& hints)
{
    for (GraphEdge *edge = node->firstFrom();
	 edge != 0;
	 edge = node->nextFrom(edge))
    {
	if (edge->to()->isHint())
	{
	    find_hints_from(edge->to(), hints);
	    hints += edge->to();
	}
    }
}

// Delete display
bool DispGraph::del (int disp_nr)
{
    if (idMap.contains (disp_nr))
    {
	DispNode* dn = idMap.get (disp_nr);
	Graph* tmp = (Graph *) this;

	VarArray<GraphNode *> hints;

	find_hints_from(dn->nodeptr(), hints);
	find_hints_to(dn->nodeptr(), hints);
	for (int i = 0; i < hints.size(); i++)
	    *tmp -= hints[i];
	    
	*tmp -= dn->nodeptr();
	delete dn;
	idMap.del (disp_nr);

	if (idMap.length() == 0)
	    handlers.call(DispGraph_Empty, this, (void*)true);
	if (!no_enabled)
	    if ((no_enabled = (count_all(Enabled) == 0)))
		handlers.call(NoEnabled, this, (void*)true);
	if (!no_disabled)
	    if ((no_disabled = (count_all(Disabled) == 0)))
		handlers.call(NoDisabled, this, (void*)true);

	return true;
    }
    return false;
}

// ***************************************************************************
int DispGraph::get_nr (BoxGraphNode* nodeptr) const
{
    MapRef ref;
    for (int k = idMap.first_key(ref); k != 0; k = idMap.next_key(ref))
	if (idMap.get(k)->nodeptr() == nodeptr)
	    return k;
    return 0;
}


// ***************************************************************************
// 0, wenn nicht vorhanden.
//
DispNode* DispGraph::first (MapRef& ref, Displaying e) const
{
    for (DispNode* dn = idMap.first(ref); dn != 0; dn = idMap.next(ref)) {
	switch (e) {
	case Both:
	    return dn;
	    break;
	case Enabled:
	    if (dn->enabled())
		return dn;
	    break;
	case Disabled:
	    if (!dn->enabled())
		return dn;
	    break;
	default:
	    // Falscher Fehler
	    assert (0);
	    break;
	}
    }
    return 0;
}
// ***************************************************************************
// 0, wenn nicht vorhanden.
//
DispNode* DispGraph::next (MapRef& ref, Displaying e) const
{
    for (DispNode* dn = idMap.next(ref); dn != 0; dn = idMap.next(ref)) {
	switch (e) {
	case Both:
	    return dn;
	    break;
	case Enabled:
	    if (dn->enabled())
		return dn;
	    break;
	case Disabled:
	    if (!dn->enabled())
		return dn;
	    break;
	default:
	    // Falscher Fehler
	    assert (0);
	    break;
	}
    }
    return 0;
}

// ***************************************************************************
// 0, wenn nicht vorhanden.
//
int DispGraph::first_nr (MapRef& ref, Displaying e) const
{
    for (int k = idMap.first_key(ref); k != 0; k = idMap.next_key(ref)) {
	switch (e) {
	case Both:
	    return k;
	    break;
	case Enabled:
	    if (idMap.get (k)->enabled())
		return k;
	    break;
	case Disabled:
	    if (!idMap.get (k)->enabled())
		return k;
	    break;
	default:
	    // Falscher Fehler
	    assert (0);
	    break;
	}
    }
    return 0;
}

// ***************************************************************************
// 0, wenn nicht vorhanden.
//
int DispGraph::next_nr (MapRef& ref, Displaying e) const
{
    for (int k = idMap.next_key(ref); k != 0; k = idMap.next_key(ref)) {
	switch (e) {
	case Both:
	    return k;
	    break;
	case Enabled:
	    if (idMap.get(k)->enabled())
		return k;
	    break;
	case Disabled:
	    if (!idMap.get(k)->enabled())
		return k;
	    break;
	default:
	    // Falscher Fehler
	    assert (0);
	    break;
	}
    }
    return 0;
}

// ***************************************************************************
void DispGraph::disp_node_disabledHP (void*,
				      void* client_data,
				      void* call_data)
{
    DispGraph* disp_graph = (DispGraph*) client_data;
    bool    disabled   = bool(call_data);

    if (disabled) {
	if (disp_graph->no_disabled) {
	    disp_graph->no_disabled = false;
	    disp_graph->handlers.call(NoDisabled, disp_graph, (void*)false);
	}
	if (disp_graph->no_enabled = (disp_graph->count_all(Enabled) == 0))
	    disp_graph->handlers.call(NoEnabled, disp_graph, (void*)true);
    }
    else {
	if (disp_graph->no_enabled) {
	    disp_graph->no_enabled = false;
	    disp_graph->handlers.call(NoEnabled, disp_graph, (void*)false);
	}
	if (disp_graph->no_disabled = (disp_graph->count_all(Disabled) == 0))
	    disp_graph->handlers.call(NoDisabled, disp_graph, (void*)true);
    }
}





