// $Id$ 
// Implementation Klasse DummyBox

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
// Revision 1.1  1995/05/01 15:47:38  zeller
// Initial revision
//
// Revision 10.11  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.10  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.9  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.8  1993/05/22  20:14:24  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.7  1993/04/22  10:51:53  zeller
// Lizenz verbessert
// 
// Revision 10.6  1993/04/21  00:10:59  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.5  1993/04/16  11:31:35  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.4  1992/06/02  03:04:10  zeller
// #pragma besser formatiert
// 
// Revision 10.3  1992/06/01  16:34:01  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.2  1992/02/18  10:57:33  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:51:07  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:44:59  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:58:12  zeller
// Installierte Version
// 
// Revision 1.2  1991/06/09  16:01:08  zeller
// Neu: Bei matches() zweites Argument fuer MatchBox-Callback,
// da HatBox zwar gematcht, aber der Callback mit dem Sohn aufgerufen wird.
// 
// Revision 1.1  1991/05/24  18:25:07  zeller
// Initial revision
// 

char DummyBox_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include "DummyBox.h"

DEFINE_TYPE_INFO_1(DummyBox, ListBox)

// DummyBox

bool DummyBox::matchesAll = false;

bool DummyBox::matches(const Box& box, const Box *callbackArg) const
{
    return matchesAll || ListBox::matches(box);
}

// DummyBox ausgeben
void DummyBox::dump(ostream& s) const
{
    s << "<_>";
}
