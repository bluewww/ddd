// $Id$
// Klasse "Range" (Deklaration)

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
// Revision 1.1  1995/05/01 15:47:49  zeller
// Initial revision
//
// Revision 10.8  1993/08/27  14:35:46  zeller
// Headerzeile verkuerzt
//
// Revision 10.7  1993/05/22  20:12:12  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.6  1993/04/22  11:02:11  zeller
// Lizenz verbessert
// 
// Revision 10.5  1993/04/21  00:20:06  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.4  1993/04/20  23:01:41  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.3  1993/04/16  11:40:05  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.2  1992/06/01  11:57:10  zeller
// #pragma once entfernt
// 
// Revision 10.1  1992/02/17  16:27:38  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  06:03:21  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:06:04  zeller
// Installierte Version
// 
// Revision 1.2  1990/08/21  23:05:00  zeller
// Operatoren == , != eingefuehrt
// 
// Revision 1.1  1990/08/17  17:44:33  zeller
// Initial revision
// 

#ifndef _Nora_Range_h
#define _Nora_Range_h

#include <iostream.h>

struct Range {
    int start;          // Beginn Bereich
    int end;            // Ende Bereich
    Range(int x = 0) {
	start = end = x;
    }
    Range(int s, int e) {
	start = s; end = e;
    }
    int operator == (Range r) {
	return start == r.start && end == r.end;
    }
    int operator != (Range r) {
	return start != r.start || end != r.end;
    }
    friend ostream& operator << (ostream& s, Range& range);
};

#endif
