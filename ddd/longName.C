// $Id$
// get long widget name

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
// Revision 1.1  1995/05/01 15:47:58  zeller
// Initial revision
//
// Revision 1.6  1993/05/22  20:10:18  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 1.5  1993/04/22  11:04:10  zeller
// Lizenz verbessert
// 
// Revision 1.4  1993/04/16  11:42:00  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 1.3  1992/11/04  11:54:19  zeller
// Neufassung mit "strclass"
// 
// Revision 1.2  1992/02/27  17:07:21  zeller
// Fuehrendes '*' ueberfluessig
// 
// Revision 1.1  1992/02/27  17:02:52  zeller
// Initial revision
// 

#include <X11/Intrinsic.h>
#include "longName.h"


string longName(Widget w)
{
    string ret = XtName(w);

    Widget parent = XtParent(w);
    if (parent)
	ret = longName(parent) + "." + ret;

    return ret;
}
