// $Id$
// Klasse FontTable (Implementation)

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
// Revision 1.1  1995/05/01 15:47:40  zeller
// Initial revision
//
// Revision 10.13  1995/04/10  20:51:14  zeller
// New: don't use <builtin.h>
//
// Revision 10.12  1995/03/17  09:54:48  zeller
// License and RCS IDs added
//
// Revision 10.11  1995/03/17  07:36:51  zeller
// Removed unused variables
//
// Revision 10.10  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.9  1993/05/22  20:14:44  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.8  1993/04/22  10:52:16  zeller
// Lizenz verbessert
// 
// Revision 10.7  1993/04/21  00:11:02  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.6  1993/04/16  11:31:41  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.5  1992/10/13  20:49:05  zeller
// strclass.h statt String.h
// 
// Revision 10.4  1992/06/02  03:04:13  zeller
// #pragma besser formatiert
// 
// Revision 10.3  1992/06/01  16:34:10  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.2  1992/02/18  10:57:20  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:55  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:46:01  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:58:28  zeller
// Installierte Version
// 
// Revision 1.2  1991/05/09  15:06:58  zeller
// Warnungen vermieden
// 
// Revision 1.1  1991/01/28  21:11:46  zeller
// Initial revision
// 

char FontTable_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include "hash.h"
#include "strclass.h"

#include <iostream.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include "FontTable.h"

DEFINE_TYPE_INFO_0(FontTable)

// FontTable

// Hash-Code erzeugen
inline unsigned hash(const char *name)
{
    return hashpjw(name) % MAX_FONTS;
}

// XFontStruct zu Fontnamen zurueckgeben
XFontStruct *FontTable::operator[](string& name)
{
    int i = hash(name);
    while (table[i].font != 0 && name != table[i].name)
    {
	assert (i < MAX_FONTS);   // Ueberlauf: Zuviele Fonts
	i = (i >= MAX_FONTS) ? 0 : i + 1;
    }

    if (table[i].font == 0 && name != table[i].name)
    {
	// Neuen Font eintragen
	table[i].name = name;
	table[i].font = XLoadQueryFont(_display, name);

	if (table[i].font == 0)
	    cerr << "Warning: could not load font \"" << name << "\"\n";
    }

    return table[i].font;
}
