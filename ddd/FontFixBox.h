// $Id$
// Deklaration Klasse FontFixBox

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
// Revision 10.12  1994/07/21  08:24:29  zeller
// New: dup() declared `const'
//
// Revision 10.11  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.10  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.9  1993/05/22  20:14:41  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.8  1993/04/22  10:52:11  zeller
// Lizenz verbessert
// 
// Revision 10.7  1993/04/20  22:57:48  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.6  1993/04/16  11:31:40  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.5  1992/07/22  10:52:31  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.4  1992/06/02  02:55:52  zeller
// Neu: #pragma interface
// 
// Revision 10.3  1992/06/01  12:53:47  zeller
// #pragma once entfernt
// 
// Revision 10.2  1992/02/18  10:57:25  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:59  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:45:51  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:58:26  zeller
// Installierte Version
// 
// Revision 1.4  1991/06/17  12:34:38  zeller
// In Konstruktor Typ angegeben
// 
// Revision 1.3  1991/02/08  16:31:30  zeller
// dump() neu definiert
// 
// Revision 1.2  1991/01/28  21:11:32  zeller
// Neu: newFont() hat String als Parameter
// 
// Revision 1.1  1991/01/28  19:49:53  zeller
// Initial revision
// 

#ifndef _Nora_FontFixBox_h
#define _Nora_FontFixBox_h

#ifdef __GNUG__
#pragma interface
#endif


#include "HatBox.h"


// FontFixBox

class FontFixBox: public HatBox {
    DECLARE_TYPE_INFO

protected:
    FontFixBox(const FontFixBox& box):
	HatBox(box)
    {}

    void dump(ostream& s) const;

public:
    FontFixBox(Box *box, char *t = "FontFixBox"):
	HatBox(box, t)
    {}

    Box *dup() const { return new FontFixBox(*this); }

    void newFont(const string& font) {} // font change stops here
};

#endif
