// $Id$ 
// Implementation Klasse MatchBox

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
// Revision 10.12  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.11  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.10  1993/12/14  12:51:51  zeller
// Neu: Funktions-Aufrufe (*p)() durch vereinfachte Syntax p() ersetzt
//
// Revision 10.9  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.8  1993/05/22  20:15:15  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.7  1993/04/22  10:51:57  zeller
// Lizenz verbessert
// 
// Revision 10.6  1993/04/21  00:11:06  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.5  1993/04/16  11:31:48  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.4  1992/06/02  03:04:18  zeller
// #pragma besser formatiert
// 
// Revision 10.3  1992/06/01  16:34:20  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.2  1992/02/18  10:57:29  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:51:03  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:47:09  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:58:49  zeller
// Installierte Version
// 
// Revision 1.9  1991/06/09  16:01:00  zeller
// Neu: Bei matches() zweites Argument fuer MatchBox-Callback,
// da HatBox zwar gematcht, aber der Callback mit dem Sohn aufgerufen wird.
// 
// Revision 1.8  1991/05/24  18:25:35  zeller
// MatchBox jetzt von DummyBox abgeleitet
// 
// Revision 1.7  1991/05/09  15:07:54  zeller
// Warnungen vermieden
// 
// Revision 1.6  1991/02/27  18:44:04  zeller
// Steuerungsflags ueberarbeitet
// 
// Revision 1.5  1991/02/20  19:30:51  zeller
// Neues Matching-Verfahren eingefuehrt
// 
// Revision 1.4  1991/02/20  14:34:29  zeller
// MatchBox ist jetzt von ListBox abgeleitet,
// um auch ListBox matchen zu koennen
// 
// Revision 1.3  1991/02/18  19:57:12  zeller
// _matched global gemacht;
// Compiler frass es sonst nicht.
// Check MatchBox_matches == 0 eingebaut
// 
// Revision 1.2  1991/02/18  12:37:33  zeller
// Neu: Bei Matching Match-Funktion aufrufen
// 
// Revision 1.1  1991/02/17  16:20:01  zeller
// Initial revision
// 

char MatchBox_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include "MatchBox.h"

DEFINE_TYPE_INFO_1(MatchBox, DummyBox)

// MatchBox

MatchBoxFunc MatchBox::matchCallback = 0;

// Bei Vergleichen: Match-Funktion aufrufen

bool MatchBox::matches (const Box &box, const Box *callbackArg) const
{
    bool ret = matchesAll ||
	DummyBox::matches(box) &&
	    _data == ((MatchBox *)&box)->_data; // dirty trick

    if (ret && matchCallback)
    {
	if (callbackArg == 0)
	    callbackArg = &box;
	matchCallback(_data, callbackArg);
    }

    return ret;
}

// MatchBox ausgeben
void MatchBox::dump(ostream& s) const
{
    s << "<" << _data << ">";
}
