// $Id$
// Implementation Klassen HFixBox, VFixBox

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
// Revision 1.1  1995/05/01 15:47:23  zeller
// Initial revision
//
// Revision 10.7  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.6  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.5  1993/05/22  20:14:33  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.4  1993/04/22  10:52:07  zeller
// Lizenz verbessert
// 
// Revision 10.3  1993/04/16  11:31:38  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.2  1992/06/01  16:34:05  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.1  1992/02/17  12:50:56  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:45:27  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:58:19  zeller
// Installierte Version
// 
// Revision 1.3  1991/05/09  15:07:06  zeller
// Warnungen vermieden
// 
// Revision 1.2  1991/02/08  16:31:06  zeller
// dump() neu definiert
// 
// Revision 1.1  1991/02/01  14:55:20  zeller
// Initial revision
// 

char FixBox_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "FixBox.h"


DEFINE_TYPE_INFO_1(FixBox, HatBox)
DEFINE_TYPE_INFO_1(HFixBox, FixBox)
DEFINE_TYPE_INFO_1(VFixBox, FixBox)

// HFixBox

// hfix() ausgeben
void HFixBox::dump(ostream& s) const
{
    s << "hfix(" << *_box << ")";
}


// VFixBox

// vfix() ausgeben
void VFixBox::dump(ostream& s) const
{
    s << "vfix(" << *_box << ")";
}
