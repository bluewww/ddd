// $Id$  -*- C++ -*-
// HintGraphNode class

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
// Revision 1.1  1995/05/01 15:47:45  zeller
// Initial revision
//
// Revision 1.10  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 1.9  1994/07/19  21:30:07  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.8  1993/08/27  15:46:05  zeller
// Neu: TypeInfo
//
// Revision 1.7  1993/05/22  20:11:20  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.6  1993/04/22  11:00:25  zeller
// Lizenz verbessert
// 
// Revision 1.5  1993/04/20  23:01:01  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.4  1993/04/16  11:38:40  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.3  1992/11/13  19:10:02  zeller
// Neu: Hints anzeigen
// 
// Revision 1.2  1992/11/13  00:05:08  zeller
// Neu: jetzt ganz unsichtbar
// 
// Revision 1.1  1992/11/13  00:00:25  zeller
// Initial revision
// 

#ifndef _Nora_HintGraphNode_h
#define _Nora_HintGraphNode_h

#ifdef __GNUG__
#pragma interface
#endif

#include "RegionGN.h"

class HintGraphNode: public RegionGraphNode {
public:
    DECLARE_TYPE_INFO

protected:
    virtual void forceDraw(Widget w, 
			   const BoxRegion& exposed,
			   const GraphGC& gc) const;

public:
    HintGraphNode(BoxPoint& initialPos = BoxPoint()):
       RegionGraphNode(initialPos, BoxSize(0,0))
    {}

    virtual bool isHint() const { return true; }

    virtual const BoxRegion& region(const GraphGC& gc) const
    {
	if (gc.drawHints)
	    return sensitiveRegion(gc);
	else
	    return RegionGraphNode::region(gc);
    }

    virtual const BoxRegion& sensitiveRegion(const GraphGC& gc) const
    {
	static BoxRegion r;

	// return a size-pixel area around the current position
	return r = BoxRegion(pos() - gc.hintSize / 2, gc.hintSize);
    }
};

#endif // _Nora_HintGraphNode_h
// DON'T ADD ANYTHING BEHIND THIS #endif
