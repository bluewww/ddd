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
// Revision 1.9  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 1.8  1994/07/19  22:08:13  zeller
// Fix: save region besides draw() being const
//
// Revision 1.7  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 1.6  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 1.5  1993/05/22  20:15:09  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.4  1993/04/22  10:51:52  zeller
// Lizenz verbessert
// 
// Revision 1.3  1993/04/21  00:11:05  zeller
// Umbenennung der Header-Dateien
// 
// Revision 1.2  1993/04/16  11:31:46  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.1  1992/11/05  16:38:04  zeller
// Initial revision
// 

char MarkBox_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "MarkBox.h"

#include "assert.h"
#include "MarkBox.h"

DEFINE_TYPE_INFO_1(MarkBox, TransparentHatBox)

// draw MarkBox
void MarkBox::_draw(Widget w, 
		    const BoxRegion& r, 
		    const BoxRegion& exposed,
		    GC gc, 
		    bool context_selected) const
{
    BoxRegion sonRegion = r;

    if (!extend(X))
	sonRegion.space(X) = size(X);
    if (!extend(Y))
	sonRegion.space(Y) = size(Y);

    // Save the region
    ((MarkBox *)this)->_region = sonRegion;

    // Keep on drawing...
    TransparentHatBox::_draw(w, r, exposed, gc, context_selected);
}
