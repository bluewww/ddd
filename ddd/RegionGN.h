// $Id$
// RegionGraphNode class: PosGraphNode with rectangular region
// centered around position

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
// Revision 1.1  1995/05/01 15:47:49  zeller
// Initial revision
//
// Revision 1.21  1994/07/19  21:30:07  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.20  1993/12/06  16:43:23  zeller
// Fix: #pragma interface statt #pragma definition
//
// Revision 1.19  1993/08/27  15:46:05  zeller
// Neu: TypeInfo
//
// Revision 1.18  1993/05/22  20:11:37  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.17  1993/04/22  11:00:15  zeller
// Lizenz verbessert
// 
// Revision 1.16  1993/04/21  00:18:18  zeller
// Umbenennung der Header-Dateien
// 
// Revision 1.15  1993/04/20  23:01:04  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.14  1993/04/16  11:38:45  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.13  1993/04/14  19:42:22  zeller
// Neu: resize()
// 
// Revision 1.12  1993/02/24  12:16:34  zeller
// Neu: Knoten vor Zeichnen loeschen (vorerst nur in PostScript)
// 
// Revision 1.11  1992/11/13  19:11:54  zeller
// region() mit GraphGC versehen
// 
// Revision 1.10  1992/11/12  23:48:59  zeller
// 'const' eingefuehrt;
// Neu: sensitiveRegion()
// 
// Revision 1.9  1992/11/05  16:49:20  zeller
// Neu: Highlight-Box kann jetzt gesondert angegeben werden
// 
// Revision 1.8  1992/06/02  00:56:45  zeller
// Anpassung an gcc 2.1
// 
// Revision 1.7  1992/06/01  16:59:13  zeller
// Neu: #pragma interface
// 
// Revision 1.6  1992/06/01  16:55:40  zeller
// #pragma once entfernt (fuer gcc 2.0)
// 
// Revision 1.5  1992/05/20  15:45:47  zeller
// Dateinamen verkuerzt
// 
// Revision 1.4  1992/02/27  23:01:40  zeller
// Neu: GraphGC
// 
// Revision 1.3  1992/02/26  11:22:28  zeller
// Neu: Zeichnen von selektierten Knoten
// 
// Revision 1.2  1992/02/25  18:26:13  zeller
// NoPoint entfernt; center() jetzt nicht mehr inline
// 
// Revision 1.1  1992/02/25  17:15:19  zeller
// Initial revision
// 

#ifndef _Nora_RegionGraphNode_h
#define _Nora_RegionGraphNode_h

#ifdef __GNUG__
#pragma interface
#endif


#include "PosGraphN.h"
#include "BoxRegion.h"

class RegionGraphNode: public PosGraphNode {
public:
    DECLARE_TYPE_INFO

private:
    BoxRegion _region;		// region

protected:		
    // draw region
    virtual void forceDraw(Widget w, 
			   const BoxRegion& exposed, 
			   const GraphGC& gc) const = 0;

    // center around position
    void center();

public:
    // Constructor
    RegionGraphNode(const BoxPoint& initialPos = BoxPoint(),
	const BoxSize& initialSize = NoSize):
	PosGraphNode(initialPos),
	_region(initialPos, initialSize)
    {
	center();
    }

    // Move
    virtual void moveTo(const BoxPoint& newPos)
    {
	PosGraphNode::moveTo(newPos);
	center();
    }
    // Assign new size
    virtual void resize(const BoxSize& newSize)
    {
	_region.space() = newSize;
	center();
    }

    // Draw
    virtual void draw(Widget w,
		      const BoxRegion& exposed,
		      const GraphGC& gc) const;

    // Print
    virtual void _print(ostream& os, const GraphGC& gc) const;

    // Attributes
    // Region occupied by this node
    virtual const BoxRegion& region(const GraphGC& gc) const
    { 
	return _region; 
    }

    // Region to be highlighted
    virtual const BoxRegion& highlightRegion(const GraphGC& gc) const 
    { 
	return region(gc); 
    }

    // Sensitive region
    virtual const BoxRegion& sensitiveRegion(const GraphGC& gc) const 
    { 
	return region(gc); 
    }
};

#endif
