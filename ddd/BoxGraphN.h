// $Id$
// BoxGraphNode class: RegionGraphNode with box

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
// Revision 1.1  1995/05/01 15:47:28  zeller
// Initial revision
//
// Revision 1.23  1995/03/21  22:12:45  zeller
// New: setBox clears current highlight box
//
// Revision 1.22  1994/07/19  21:30:07  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.21  1993/12/06  16:40:14  zeller
// Fix: #pragma interface statt #pragma definition
//
// Revision 1.20  1993/08/27  15:46:05  zeller
// Neu: TypeInfo
//
// Revision 1.19  1993/05/22  20:10:33  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.18  1993/04/22  11:00:22  zeller
// Lizenz verbessert
// 
// Revision 1.17  1993/04/20  23:00:54  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.16  1993/04/16  11:38:27  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.15  1993/04/14  19:42:51  zeller
// Fix: Wenn Box veraendert wird, muss auch Groesse geaendert werden
// 
// Revision 1.14  1993/04/14  19:20:31  zeller
// Neu: highlight(), setBox(), setHighlight()
// 
// Revision 1.13  1993/02/23  17:31:18  zeller
// Neu: Drucken -- in EPS oder FIG-Format
// 
// Revision 1.12  1992/12/10  10:58:12  zeller
// Fix: Destruktor muss virtuell sein
// 
// Revision 1.11  1992/11/13  19:07:49  zeller
// forceDraw: const eingefuehrt; region() angepasst
// 
// Revision 1.10  1992/11/12  23:46:59  zeller
// 'const' eingefuehrt
// 
// Revision 1.9  1992/11/05  16:49:07  zeller
// Neu: Highlight-Box kann jetzt gesondert angegeben werden
// 
// Revision 1.8  1992/06/01  16:58:53  zeller
// Neu: #pragma interface
// 
// Revision 1.7  1992/06/01  16:55:25  zeller
// #pragma once entfernt (fuer gcc 2.0)
// 
// Revision 1.6  1992/05/20  15:45:50  zeller
// Dateinamen verkuerzt
// 
// Revision 1.5  1992/03/03  17:42:51  zeller
// Neu: str()
// 
// Revision 1.4  1992/02/27  23:01:15  zeller
// Neu: GraphGC
// 
// Revision 1.3  1992/02/26  11:22:23  zeller
// Neu: Zeichnen von selektierten Knoten
// 
// Revision 1.2  1992/02/25  18:25:23  zeller
// Compiler-Fehler beseitigt
// 
// Revision 1.1  1992/02/25  17:15:17  zeller
// Initial revision
// 

#ifndef _Nora_BoxGraphNode_h
#define _Nora_BoxGraphNode_h

#ifdef __GNUG__
#pragma interface
#endif


#include "RegionGN.h"
#include "Box.h"
#include "MarkBox.h"

class BoxGraphNode: public RegionGraphNode {
public:
    DECLARE_TYPE_INFO

private:
    Box *_box;		 // the box
    MarkBox *_highlight; // box to be highlighted when selected

protected:
    // Draw
    virtual void forceDraw(Widget w, 
			   const BoxRegion& exposed, 
			   const GraphGC& gc) const;

public:
    // Constructor
    BoxGraphNode(Box *b, const BoxPoint& initialPos = BoxPoint(), 
		 MarkBox *h = 0):
	RegionGraphNode(initialPos, b->size()),
	_box(b->link()),
        _highlight(h)
    {}

    // Destructor
    virtual ~BoxGraphNode()
    {
	if (_highlight)
	    _highlight->unlink();
	_box->unlink();
    }

    // Attributes
    Box *box() const           { return _box; }
    MarkBox *highlight() const { return _highlight; }
    virtual string str() const { return box()->str(); }

    virtual const BoxRegion& highlightRegion(const GraphGC& gc) const
    { 
	if (_highlight)
	    return _highlight->__region();
	else
	    return RegionGraphNode::highlightRegion(gc);
    }

    // Modify
    // Set the highlight box.
    // The highlight box must be a child of the displayed box.
    void setHighlight(MarkBox *b = 0)
    {
	_highlight = b;
    }

    // Set the box.
    void setBox(Box *b)
    {
	setHighlight(0);

	Box *old = _box;
	_box = b->link();
	old->unlink();
	resize(b->size());
    }


    // Print
    virtual void _print(ostream& os, const GraphGC& gc) const;
};

#endif

