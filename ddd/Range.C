// $Id$
// Klasse "Range" (Implementation)

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
// Revision 1.1  1995/05/01 15:47:48  zeller
// Initial revision
//
// Revision 10.6  1993/08/27  14:35:46  zeller
// Headerzeile verkuerzt
//
// Revision 10.5  1993/05/22  20:12:09  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.4  1993/04/22  11:02:07  zeller
// Lizenz verbessert
// 
// Revision 10.3  1993/04/21  00:20:05  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.2  1993/04/16  11:40:04  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.1  1992/02/17  16:27:38  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  06:03:18  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:06:01  zeller
// Installierte Version
// 
// Revision 1.3  1991/05/09  14:29:02  zeller
// sccsid oeffentlich gemacht (wg. Fehlermeldung -Wunused)
// 
// Revision 1.2  1990/08/18  18:09:53  zeller
// Ausgabe vereinfacht
// 
// Revision 1.1  1990/08/17  17:44:32  zeller
// Initial revision
// 

char Range_rcsid[] = "$Id$";


#include <iostream.h>
#include "Range.h"

// Range

ostream& operator << (ostream& s, Range& range)
{
    if (range.start == range.end)
	return s << '[' << range.start << ']';
    else
	return s << '[' << range.start << ".." << range.end << ']';
}
