// $Id$
// PosGraphNode class: GraphNode with position

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
// Revision 1.1  1995/05/01 15:47:48  zeller
// Initial revision
//
// Revision 1.13  1993/12/06  16:43:29  zeller
// Fix: #pragma interface statt #pragma definition
//
// Revision 1.12  1993/08/27  15:46:05  zeller
// Neu: TypeInfo
//
// Revision 1.11  1993/05/22  20:11:32  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.10  1993/04/22  11:00:19  zeller
// Lizenz verbessert
// 
// Revision 1.9  1993/04/21  00:18:16  zeller
// Umbenennung der Header-Dateien
// 
// Revision 1.8  1993/04/20  23:01:04  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.7  1993/04/16  11:38:44  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.6  1992/11/12  23:48:43  zeller
// 'const' eingefuehrt
// 
// Revision 1.5  1992/06/02  00:57:17  zeller
// Anpassung an gcc 2.1
// 
// Revision 1.4  1992/06/01  16:59:12  zeller
// Neu: #pragma interface
// 
// Revision 1.3  1992/06/01  16:55:39  zeller
// #pragma once entfernt (fuer gcc 2.0)
// 
// Revision 1.2  1992/02/25  18:25:55  zeller
// NoPoint entfernt
// 
// Revision 1.1  1992/02/25  17:15:19  zeller
// Initial revision
// 

#ifndef _Nora_PosGraphNode_h
#define _Nora_PosGraphNode_h

#ifdef __GNUG__
#pragma interface
#endif


#include "GraphNode.h"
#include "BoxPoint.h"

class PosGraphNode: public GraphNode {
public:
    DECLARE_TYPE_INFO

private:
    BoxPoint _pos;	// position

public:
    // Constructor
    PosGraphNode(const BoxPoint& initialPos = BoxPoint()):
	_pos(initialPos)
    {}

    // Move
    virtual void moveTo(const BoxPoint& newPos)
    {
	_pos = newPos;
    }

    // Attributes
    virtual const BoxPoint& pos() const
    {
	return _pos;
    }
};

#endif
