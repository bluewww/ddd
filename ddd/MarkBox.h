// $Id$  -*- C++ -*-
// The Mark Box saves its region

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
// Revision 1.1  1995/05/01 15:47:47  zeller
// Initial revision
//
// Revision 1.18  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 1.17  1994/07/21  08:24:29  zeller
// New: dup() declared `const'
//
// Revision 1.16  1994/07/19  22:11:07  zeller
// New: draw() now const correct
//
// Revision 1.15  1994/07/19  18:42:24  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 1.14  1994/02/23  15:08:02  zeller
// Neu: link = dup wird jetzt erst in TagBox erledigt
//
// Revision 1.13  1994/02/23  13:48:58  zeller
// Fix: Copy Constructor repariert
//
// Revision 1.12  1994/02/21  08:57:36  zeller
// Fix: dup() nachgereicht
//
// Revision 1.11  1994/02/19  20:52:52  zeller
// Fix: dup()/link()-Problem korrigiert
// Kommentare jetzt englisch
//
// Revision 1.10  1993/11/11  12:44:38  zeller
// Fix: Zeichen-Funktion jetzt protected
//
// Revision 1.9  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 1.8  1993/05/22  20:15:12  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.7  1993/04/22  10:51:53  zeller
// Lizenz verbessert
// 
// Revision 1.6  1993/04/21  00:11:05  zeller
// Umbenennung der Header-Dateien
// 
// Revision 1.5  1993/04/20  22:57:52  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 1.4  1993/04/16  11:31:47  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.3  1992/11/12  22:40:40  zeller
// __region() effizienter gemacht
// 
// Revision 1.2  1992/11/05  17:16:48  zeller
// Duplizieren unterdrueckt
// 
// Revision 1.1  1992/11/05  16:38:04  zeller
// Initial revision
// 

#ifndef _Nora_MarkBox_h
#define _Nora_MarkBox_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include "bool.h"
#include "Box.h"
#include "Widget.h"
#include "THatBox.h"


class MarkBox: public TransparentHatBox {
public:
    DECLARE_TYPE_INFO

protected:
    BoxRegion _region;          // Region of last draw

    // Draw
    virtual void _draw(Widget w, 
		       const BoxRegion& region, 
		       const BoxRegion& exposed, 
		       GC gc,
		       bool context_selected) const;

    // Copy Constructor
    MarkBox(const MarkBox& box):
	TransparentHatBox(box),
	_region(box._region)
    {}

public:
    // Constructor
    MarkBox(Box *box):
	TransparentHatBox(box), _region(BoxPoint(0,0), BoxSize(-1,-1))
    {}

    Box *dup() const { return new MarkBox(*this); }

    // Resources
    const BoxRegion& __region() const { return _region; }
};

#endif // _Nora_MarkBox_h
// DON'T ADD ANYTHING BEHIND THIS #endif
