// $Id$
// Klasse Box (Implementation)

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

#ifdef __GNUG__
#pragma implementation
#endif

char Box_rcsid[] = 
    "$Id$";

extern "C" {
#include <string.h>
}

#include "assert.h"
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>

#include "Box.h"
#include "TagBox.h"
#include "VSEFlags.h"

DEFINE_TYPE_INFO_0(Box)
DEFINE_TYPE_INFO_0(BoxPrintGC)
DEFINE_TYPE_INFO_1(BoxPostScriptGC, BoxPrintGC)
DEFINE_TYPE_INFO_1(BoxFigGC, BoxPrintGC)


// Anzeigen

// Box anzeigen
void Box::draw(Widget w, 
	       const BoxRegion& r, 
	       const BoxRegion& exposed, 
	       GC gc, 
	       bool context_selected) const
{
    // Wenn nicht im exponierten Bereich, dann Abbruch
    if (!(r <= exposed))
	return;

    if (VSEFlag(show_draw))
	cout << "\n[" << r;

    // Sicherstellen, dass genug Platz vorhanden
    assert(!(size() > r.space()));

    // Grafik-Kontext auf Default setzen
    if (gc == 0)
	gc = DefaultGCOfScreen(XtScreen(w));

    // Eigentliche Anzeigefunktion aufrufen
    _draw(w, r, exposed, gc, context_selected);

    if (VSEFlag(show_draw))
	cout << "]";
}


// Vergleichen

// Boxen vergleichen (intern)
bool Box::matches(const Box &b, const Box *callbackArg) const
{
    return (size() == b.size() &&
	extend() == b.extend() &&
	strcmp(type(), b.type()) == 0);
}


// Boxen vergleichen (oeffentliche Schnittstelle)
bool Box::operator == (const Box &b) const
{
    if (VSEFlag(show_match_boxes))
    {
	cout << "\nBox match: " << *this << " ? " << b;
	cout.flush();
    }

    bool flag = (this == &b) || matchMe().matches(b.matchMe(), &b);

    if (VSEFlag(show_match_boxes))
    {
	if (flag)
	    cout << "\nBox match: " << *this << " == " << b;
	else
	    cout << "\nBox match: " << *this << " != " << b;

	cout.flush();
    }
    
    return flag;
}


// Markierungen

// Box markieren
Box *Box::tag(Data *dta, DataLink *dl)
{
    TagBox *ret = new TagBox(this, dta, dl);
    unlink();
    return ret;
}

// BoxRegion der TagBox zu Punkt p zurueckgeben (kein Punkt: oberste)
BoxRegion Box::region(BoxPoint p) const
{
    const TagBox *t = findTag(p);
    return t ? t->__region() : BoxRegion(BoxPoint(0,0), BoxSize(0,0));
}

// Datum der TagBox zu Punkt p zurueckgeben (kein Punkt: oberste)
Data *Box::data(BoxPoint p) const
{
    const TagBox *t = findTag(p);
    return t ? t->__data() : 0;
}

// Namen der TagBox zu Punkt p zurueckgeben (kein Punkt: oberste)
string Box::name(BoxPoint p) const
{
    const TagBox *t = findTag(p);
    return t ? t->__name() : "";
}

// Information der TagBox zu Punkt p zurueckgeben (kein Punkt: oberste)
string Box::info(BoxPoint p) const
{
    const TagBox *t = findTag(p);
    return t ? t->__info() : "";
}

// Ausgewaehlt-Eigenschaft der TagBox zu Punkt p zurueckgeben
bool Box::selected(BoxPoint p) const
{
    const TagBox *t = findTag(p);
    return t ? t->__selected() : false;
}


// Debugging

// Box ausgeben
ostream &operator << (ostream& s, const Box& b)
{
    if (VSEFlags::max_info_nesting != 0)
    {
	VSEFlags::max_info_nesting--;

	b.dump(s);

	if (VSEFlags::include_id_info)
	    s << " (id: " << b.id() << ")";

	if (VSEFlags::include_size_info)
	    s << " // size = " << b.size() << " corner = " << b.corner() <<
		" extend = " << b.extend() << "\n";

	VSEFlags::max_info_nesting++;
    }
    else
	s << "_";

    return s;
}
