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

char MatchBox_rcsid[] = 
    "$Id$";

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
