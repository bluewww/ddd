// $Id$ -*- C++ -*-
// AliasGraphEdge class: temporary edge from or to alias node

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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

#ifndef _DDD_AliasGraphEdge_h
#define _DDD_AliasGraphEdge_h

#include "ArcGraphE.h"

class AliasGraphEdge: public ArcGraphEdge {
public:
    DECLARE_TYPE_INFO

private:
    int _disp_nr;		// Display associated with this edge

protected:
    // Copy Constructor
    AliasGraphEdge(const AliasGraphEdge& edge)
	: ArcGraphEdge(edge),
	  _disp_nr(edge._disp_nr)
    {}

public:
    // Constructor
    AliasGraphEdge(int disp_nr, GraphNode *from, GraphNode *to, 
		   EdgeAnnotation *ann = 0)
	: ArcGraphEdge(from, to, ann),
	  _disp_nr(disp_nr)
    {}

    // Destructor
    virtual ~AliasGraphEdge() {}

    // Resources
    int disp_nr() const { return _disp_nr; }

    // Duplicator
    GraphEdge *dup() const
    {
	return new AliasGraphEdge(*this);
    }
};

#endif // _DDD_AliasGraphEdge_h
// DON'T ADD ANYTHING BEHIND THIS #endif
