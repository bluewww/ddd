// $Id$
// Deklaration Klasse BinBox

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
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
	
// $Log$
// Revision 1.1  1995/05/01 15:47:19  zeller
// Initial revision
//
// Revision 10.11  1994/07/21  08:24:29  zeller
// New: dup() declared `const'
//
// Revision 10.10  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.9  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.8  1993/05/22  19:58:49  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.7  1993/04/20  22:57:40  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.6  1993/04/16  11:31:23  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.5  1992/07/22  10:52:21  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.4  1992/06/02  03:14:17  zeller
// #pragma korrigiert
// 
// Revision 10.3  1992/06/01  16:33:26  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.2  1992/06/01  12:53:28  zeller
// #pragma once entfernt
// 
// Revision 10.1  1992/02/17  12:50:58  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:42:35  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:57:30  zeller
// Installierte Version
// 
// Revision 1.3  1991/06/17  12:34:36  zeller
// In Konstruktor Typ angegeben
// 
// Revision 1.2  1991/02/08  16:31:22  zeller
// dump() neu definiert
// 
// Revision 1.1  1991/02/01  14:55:26  zeller
// Initial revision
// 

#ifndef _Nora_BinBox_h
#define _Nora_BinBox_h

#ifdef __GNUG__
#pragma interface
#endif

// Eine BinBox setzt die Ausdehung ihres Sohnes auf 1, wenn sie > 1 ist.


#include "HatBox.h"


// BinBox

class BinBox: public HatBox {
public:
    DECLARE_TYPE_INFO

protected:
    BinBox(const BinBox& box): HatBox(box)
    {}

    void dump(ostream& s) const;

public:
    BinBox(Box *box, char *t = "BinBox"): HatBox(box, t)
    { 
	theextend()[X] = extend(X) > 0;
	theextend()[Y] = extend(Y) > 0;
    }

    Box *resize()
    {
	HatBox::resize();
	theextend()[X] = extend(X) > 0;
	theextend()[Y] = extend(Y) > 0;

	return this;
    }

    Box *dup() const { return new BinBox(*this); }
};

#endif
