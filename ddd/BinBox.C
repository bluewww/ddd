// $Id$
// Implementation Klasse BinBox

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
// Revision 10.6  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.5  1993/08/27  14:37:48  zeller
// Neu: TypeInfo
//
// Revision 10.4  1993/05/22  19:53:38  zeller
// %Q% added
//
// Revision 10.3  1993/04/16  11:31:22  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.2  1992/06/01  16:33:23  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.1  1992/02/17  12:50:57  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:42:25  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:57:28  zeller
// Installierte Version
// 
// Revision 1.3  1991/05/09  15:07:15  zeller
// Warnungen vermieden
// 
// Revision 1.2  1991/02/08  16:31:17  zeller
// dump() neu definiert
// 
// Revision 1.1  1991/02/01  14:55:24  zeller
// Initial revision
// 

char BinBox_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "BinBox.h"

DEFINE_TYPE_INFO_1(BinBox, HatBox)

// BinBox

// [ box ] ausgeben
void BinBox::dump(ostream& s) const
{
    s << "[" << *_box << "]";
}
