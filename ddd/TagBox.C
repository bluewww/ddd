// $Id$
// Implementation Klasse TagBox

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

char TagBox_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include "TagBox.h"
#include "VSEFlags.h"


DEFINE_TYPE_INFO_1(TagBox, MarkBox)

// TagBox

// Default-Funktionen

// Default-Duplizier-Funktion
static Data *default_link(Data *dta)
{
    return dta;
}

// Default-Freigabe-Funktion
static void default_unlink(Data *dta)
{
    return;
}

// Dummy-Funktion zur Bestimmung, ob ausgewaehlt
static bool default_selected(Data *dta)
{
    return false;   // Default: "nicht ausgewaehlt" zurueckgeben
}

// Dummy-Funktion zur Bestimmung des Tag-Namens
static string default_name(Data *dta)
{
    return "";      // Default: Kein Name
}

// Dummy-Funktion zur Bestimmung der Tag-Information
static string default_info(Data *dta)
{
    return default_name(dta);   // Default: Keine Information
}

// Alle Dummyfunktionen
DataLink TagBox::default_datalink =
{
    default_link,
    default_unlink,
    default_selected,
    default_name,
    default_info
};


// TagBox suchen, in der Punkt p liegt und tag zurueckgeben
const TagBox *TagBox::findTag(BoxPoint p) const
{
    // Sonderfall: obersten Tag zurueckgeben
    if (p == BoxPoint(-1, -1))
	return this;

    if (!(p <= _region))
    {
	// Punkt nicht in BoxRegion dieser TagBox
	return 0;       // nicht gefunden
    }

    // Soehne absuchen: wenn Sohn gefunden, diesen waehlen
    const TagBox *subbox = HatBox::findTag(p);

    if (subbox != 0)
	return subbox;  // gefunden? Dann diese TagBox zurueckgeben
    else
	return this;    // sonst hier bleiben.
}


// TagBox anzeigen
void TagBox::_draw(Widget w, 
		   const BoxRegion& r, 
		   const BoxRegion& exposed,
		   GC gc, 
		   bool context_selected) const
{
    // Sohn anzeigen
    bool nodeSelected = __selected();
    MarkBox::_draw(w, r, exposed, gc, nodeSelected);

    if (context_selected != nodeSelected)
    {
	// Wenn propagiertes Select-Flag nicht mit Select-Status des
	// Knotens uebereinstimmt: angezeigte BoxRegion 
	// nachtraeglich invertieren

	BoxRegion clipRegion = exposed & r;    // Schnittmenge

	XGCValues gcvalues;
	XGetGCValues(XtDisplay(w), gc, GCFunction, &gcvalues);
	XSetFunction(XtDisplay(w), gc, GXinvert);

	XFillRectangle(XtDisplay(w), XtWindow(w), gc,
	    clipRegion.origin(X), clipRegion.origin(Y),
	    clipRegion.space(X), clipRegion.space(Y));

	XSetFunction(XtDisplay(w), gc, gcvalues.function);
    }
}


// TagBox ausgeben
void TagBox::dump(ostream& s) const
{
    MarkBox::dump(s);

    if (VSEFlags::include_tag_info)
    {
	s << "\n// Tag " << _data << ": ";
	s.flush();

	s << "(" << info() << ") " << _region << " ";

	if (selected())
	    s << " (selected)";
	else
	    s << " (not selected)";

	s << "\n\n";
	s.flush();
    }
}
