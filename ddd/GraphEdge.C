// $Id$
// GraphEdge class

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

// $Log$
// Revision 1.1  1995/05/01 15:47:41  zeller
// Initial revision
//
// Revision 1.14  1995/04/08  09:20:16  zeller
// Fix: don't fetch the line width via X internal GC structure
//
// Revision 1.13  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 1.12  1993/12/15  12:02:05  zeller
// Fix: Kanten ohne Pfeilspitzen korrekt drucken
//
// Revision 1.11  1993/10/18  12:51:28  zeller
// Fix: Versteckte Kanten nicht ausdrucken
//
// Revision 1.10  1993/08/27  15:46:05  zeller
// Neu: TypeInfo
//
// Revision 1.9  1993/05/22  20:10:50  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.8  1993/04/22  11:00:31  zeller
// Lizenz verbessert
// 
// Revision 1.7  1993/04/16  11:38:31  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.6  1993/03/01  11:32:43  zeller
// Fix: EDGEHEAD statt LINEHEAD verwendet;
// EDGEHEAD hat andere Tiefe
// 
// Revision 1.5  1993/02/25  14:11:05  zeller
// Fix: Fall "Liniendicke == 0" korrekt behandelt
// 
// Revision 1.4  1993/02/23  17:31:21  zeller
// Neu: Drucken -- in EPS oder FIG-Format
// 
// Revision 1.3  1992/11/13  15:52:14  zeller
// Neu: Loeschen von Knoten und Kanten moeglich
// 
// Revision 1.2  1992/06/01  17:02:36  zeller
// Neu: #pragma implementation
// 
// Revision 1.1  1992/03/03  14:58:52  zeller
// Initial revision
// 

char GraphEdge_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "GraphEdge.h"
#include "GraphNode.h"
#include "printBox.h"
#include <stdlib.h>

DEFINE_TYPE_INFO_0(GraphEdge)

void GraphEdge::enqueue()
{
    assert (_nextFrom == 0);
    assert (_nextTo   == 0);
    assert (_prevFrom == 0);
    assert (_prevTo   == 0);
	
    if (from()->_firstFrom == 0)
    {
	_nextFrom = this;
	_prevFrom = this;
	from()->_firstFrom = this;
    }
    else
    {
	_nextFrom = from()->_firstFrom->_nextFrom;
	_prevFrom = from()->_firstFrom;

	_nextFrom->_prevFrom = this;
	_prevFrom->_nextFrom = this;
    }

    if (to()->_firstTo == 0)
    {
	_nextTo = this;
	_prevTo = this;
	to()->_firstTo = this;
    }
    else
    {
	_nextTo = to()->_firstTo->_nextTo;
	_prevTo = to()->_firstTo;

	_nextTo->_prevTo = this;
	_prevTo->_nextTo = this;
    }
}

void GraphEdge::dequeue()
{
    // dequeue from "from" chain
    if (_nextFrom != 0 || _prevFrom != 0)
    {
	assert(_nextFrom != 0);
	assert(_prevFrom != 0);

	if (_from->_firstFrom == this)
	    _from->_firstFrom = _nextFrom;

	if (_from->_firstFrom == this)
	{
	    assert(_nextFrom == this);
	    assert(_prevFrom == this);
	    _from->_firstFrom = 0;
	}
	else
	{
	    _nextFrom->_prevFrom = _prevFrom;
	    _prevFrom->_nextFrom = _nextFrom;
	}

	_nextFrom = 0;
	_prevFrom = 0;
    }

    // dequeue from "to" chain
    if (_nextTo != 0 || _prevTo != 0)
    {
	assert(_nextTo != 0);
	assert(_prevTo != 0);

	if (_to->_firstTo == this)
	    _to->_firstTo = _nextTo;

	if (_to->_firstTo == this)
	{
	    assert(_nextTo == this);
	    assert(_prevTo == this);
	    _to->_firstTo = 0;
	}
	else
	{
	    _nextTo->_prevTo = _prevTo;
	    _prevTo->_nextTo = _nextTo;
	}

	_nextTo = 0;
	_prevTo = 0;
    }
}

    
    

ostream& operator << (ostream& s, GraphEdge& e)
{
    return s << "( " << *(e.from()) << " -> " << *(e.to()) << " )";
}


// representation invariant
bool GraphEdge::OK() const
{
    GraphEdge *e;

    // assert that prev/next is okay
    assert(_nextTo == 0 || _nextTo->_prevTo == this);
    assert(_prevTo == 0 || _prevTo->_nextTo == this);

    assert(_nextFrom == 0 || _nextFrom->_prevFrom == this);
    assert(_prevFrom == 0 || _prevFrom->_nextFrom == this);

    // assert that this is contained in from and to lists
    if (_nextFrom || _prevFrom)
    {
	for (e = from()->firstFrom(); e != 0 && e != this; 
	     e = from()->nextFrom(e))
	    ;
	assert(e == this);
    }

    if (_nextTo || _prevTo)
    {
	for (e = to()->firstTo(); e != 0 && e != this; 
	     e = to()->nextTo(e))
	    ;
	assert(e == this);
    }

    return true;
}


// printing

// internal types

enum Side { North = 1 , South = 2  , East = 4  , West = 8 } ;

// prototypes for internal usage

static Side crosside (BoxRegion  &region, BoxPoint &p) ;
static BoxPoint crosspoint (BoxRegion &region, BoxPoint &p);

//
// crossside
// 

static Side crosside (BoxRegion  &region, BoxPoint &p)
{
    BoxPoint center = region.origin() + (region.space() / BoxPoint(2)) ;
    BoxPoint delta = center  - p ;
	
    int side = North | South | East | West ;
    
    // exclude opposite side
    
    if (p[X] > center[X]) 
	side &= ~West ;
    else
	side &= ~East ;
    if (p[Y] > center[Y])
	side &= ~North ;
    else
	side &= ~South ;
    
    delta[X] = abs(delta[X]);
    delta[Y] = abs(delta[Y]);
    
    if (region.space(Y) * delta[X] > region.space(X) * delta[Y]) 
	side &= ~(North | South);
    else
	side &= ~(East | West);
    
    return Side(side);
}

//
// crosspoint
// 

static BoxPoint crosspoint (BoxRegion &region, BoxPoint &p)
{
    int side = crosside (region, p);
    
    BoxDimension d1, d2;
    BoxPoint center = region.origin() + (region.space() / BoxPoint(2)) ;
    BoxPoint cross = center;
    int offset;
    
    offset = (side & (North | West)? -1 : 1) ;
    if (side & (North | South)) {
	d1 = X ; 
	d2 = Y ;
    } else {
	d1 = Y ;
	d2 = X ;
    }
    
    if (center[d1] != p[d1] && center[d2] != p[d2]) {
	cross[d1] += offset * (region.space(d2) / 2) 
	    * ( center[d1] - p[d1]) / ( center[d2] - p[d2] ) ;
    } 
    cross[d2] += offset * region.space(d2) / 2;
    
    return cross ;
}

//
// drawEdge
// 

void GraphEdge::_print(ostream& os, const GraphGC &gc) const
{
    // Don't print if we're hidden
    if (hidden())
	return;
    
    // Fetch the regions
    BoxRegion start = from()->region(gc);
    BoxRegion end   = to()->region(gc);

    // Don't print edges with zero length
    if (start <= end)
	return;
    
    BoxPoint startc = start.origin() + (start.space() / BoxPoint(2));
    BoxPoint endc   = end.origin()   + (end.space()   / BoxPoint(2));

    BoxPoint startp = crosspoint (start, endc);
    BoxPoint endp   = crosspoint (end, startc);

    // This should come from gc.edgeGC
    BoxCoordinate line_width = 1;

    if (gc.printGC->isFig()) {
	if (!gc.drawArrowHeads || to()->isHint()) {
	    os << EDGEHEAD1 << line_width;
	    os << EDGEHEAD2 ;
	    os << startp[X] << " " << startp[Y] << " " ;
	    os << endp[X] << " " << endp[Y] << " " ;
	    os << "9999 9999\n" ;
	} else {
	    os << ARROWHEAD1 << line_width;
	    os << ARROWHEAD2 ;
	    os << startp[X] << " " << startp[Y] << " " ;
	    os << endp[X] << " " << endp[Y] << " " ;
	    os << "9999 9999\n" ;
	}
    } else if (gc.printGC->isPostScript()) {
	if (!gc.drawArrowHeads || to()->isHint()) {
	    os << startp[X] << " " << startp[Y] << " " ;
	    os << endp[X] << " " << endp[Y] << " " ;
	    os << line_width << " line*\n";
	} else {
	    os << gc.arrowAngle << " " << gc.arrowLength << " ";
	    os << startp[X] << " " << startp[Y] << " " ;
	    os << endp[X] << " " << endp[Y] << " " ;
	    os << line_width << " arrowline*\n";
	}
    }
}
