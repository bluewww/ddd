// $Id$
// Klasse FontTable (Deklaration)

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
// Revision 10.11  1993/08/27  14:52:25  zeller
// Neu: TypeInfo
//
// Revision 10.10  1993/05/22  20:14:47  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.9  1993/04/22  10:52:16  zeller
// Lizenz verbessert
// 
// Revision 10.8  1993/04/20  22:57:49  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.7  1993/04/16  11:31:41  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.6  1992/10/13  20:49:06  zeller
// strclass.h statt String.h
// 
// Revision 10.5  1992/07/22  10:52:32  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.4  1992/06/02  02:55:53  zeller
// Neu: #pragma interface
// 
// Revision 10.3  1992/06/01  12:53:48  zeller
// #pragma once entfernt
// 
// Revision 10.2  1992/02/18  10:57:21  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:55  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:46:09  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:58:30  zeller
// Installierte Version
// 
// Revision 1.1  1991/01/28  21:11:47  zeller
// Initial revision
// 

#ifndef _Nora_FontTable_h
#define _Nora_FontTable_h

#ifdef __GNUG__
#pragma interface
#endif


#include <X11/Xlib.h>
#include "strclass.h"
#include "TypeInfo.h"

#define MAX_FONTS 511 /* Max #Fonts */

struct FontTableHashEntry {
    XFontStruct *font;
    string name;
};

class FontTable {
public:
    DECLARE_TYPE_INFO

private:
    FontTableHashEntry table[MAX_FONTS];
    Display *_display;

public:
    FontTable(Display *display):
	_display(display)
    {
	for (unsigned i = 0; i < MAX_FONTS; i++)
	{
	    table[i].font = 0;
	    table[i].name = "";
	}
    }
    virtual ~FontTable()
    {
	for (unsigned i = 0; i < MAX_FONTS; i++)
	    if (table[i].font != 0)
		XFreeFont(_display, table[i].font);
    }
    XFontStruct *operator[](string& name);
};

#endif
