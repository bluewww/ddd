// $Id$
// Klasse Box (Implementation)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifdef __GNUG__
#pragma implementation
#endif

char Box_rcsid[] = 
    "$Id$";

#include <string.h>

#include "assert.h"
#include "return.h"
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
bool Box::matches(const Box &b, const Box *) const
{
    if (strcmp(type(), b.type()))
	return false;

    for (BoxDimension d = X; d <= Y; d++)
	if (extend(d) == 0 && size(d) != b.size(d))
	    return false;

    return true;
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
BoxRegion Box::region(BoxPoint p) const RETURNS(r)
{
    RETURN_OBJECT(BoxRegion, r);
    const TagBox *t = findTag(p);
    if (t)
	r = t->__region();
    else
	r = BoxRegion(BoxPoint(0,0), BoxSize(0,0));
    RETURN(r);
}

// Datum der TagBox zu Punkt p zurueckgeben (kein Punkt: oberste)
Data *Box::data(BoxPoint p) const
{
    const TagBox *t = findTag(p);
    if (t == 0)
	return 0;
    return t->__data();
}

// Namen der TagBox zu Punkt p zurueckgeben (kein Punkt: oberste)
string Box::name(BoxPoint p) const RETURNS(name)
{
    RETURN_OBJECT(string, name);
    const TagBox *t = findTag(p);
    if (t)
	name = t->__name();
    else
	name = "";
    RETURN(name);
}

// Information der TagBox zu Punkt p zurueckgeben (kein Punkt: oberste)
string Box::info(BoxPoint p) const RETURNS(info)
{
    RETURN_OBJECT(string, info);
    const TagBox *t = findTag(p);
    if (t)
	info = t->__info();
    else
	info = "";
    RETURN(info);
}

// Ausgewaehlt-Eigenschaft der TagBox zu Punkt p zurueckgeben
bool Box::selected(BoxPoint p) const
{
    const TagBox *t = findTag(p);
    if (t == 0)
	return false;
    return t->__selected();
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
