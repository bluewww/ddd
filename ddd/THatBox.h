// $Id$
// Deklaration Klasse TransparentHatBox

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
// Revision 1.1  1995/05/01 15:47:51  zeller
// Initial revision
//
// Revision 10.11  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.10  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.9  1993/05/22  20:15:54  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.8  1993/04/22  10:52:19  zeller
// Lizenz verbessert
// 
// Revision 10.7  1993/04/20  22:57:59  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.6  1993/04/16  11:31:59  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.5  1992/11/05  16:37:35  zeller
// Neu: (transparentes) dump()
// 
// Revision 10.4  1992/07/22  10:52:41  zeller
// Copy-Konstruktoren jetzt von der Form X::X(const& X)
// 
// Revision 10.3  1992/06/02  03:21:00  zeller
// Neu: #pragma interface
// 
// Revision 10.2  1992/06/01  12:54:10  zeller
// #pragma once entfernt
// 
// Revision 10.1  1992/02/17  12:51:09  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1992/02/17  12:37:05  zeller
// Initial revision
// 

#ifndef _Nora_TransparentHatBox_h
#define _Nora_TransparentHatBox_h

#ifdef __GNUG__
#pragma interface
#endif


// Eine TransparentHatBox ist eine HatBox, die beim Matchen
// unsichtbar ist.

#include "HatBox.h"
#include "Widget.h"


// TransparentHatBox

class TransparentHatBox: public HatBox {
public:
    DECLARE_TYPE_INFO

protected:
    virtual const Box& matchMe() const { return _box->matchMe(); }

    TransparentHatBox(const TransparentHatBox& box):
	HatBox(box)
    {}

    virtual void dump(ostream& s) const { _box->dump(s); }

public:
    TransparentHatBox(Box *box):
	HatBox(box, "TransparentHatBox")
    {}
};

#endif
