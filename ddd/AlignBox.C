// $Id$
// Klasse AlignBox (Implementation)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifdef __GNUG__
#pragma implementation
#endif

char AlignBox_rcsid[] = 
    "$Id$";

#include "assert.h"
#include "misc.h"
#include "AlignBox.h"

DEFINE_TYPE_INFO_1(AlignBox, CompositeBox)
DEFINE_TYPE_INFO_1(HAlignBox, AlignBox)
DEFINE_TYPE_INFO_1(TAlignBox, AlignBox)
DEFINE_TYPE_INFO_1(UAlignBox, AlignBox)
DEFINE_TYPE_INFO_1(VAlignBox, AlignBox)


// Alignment (horizontal/vertikal) anzeigen
void AlignBox::drawAlign(Widget w, 
			 const BoxRegion& r, 
			 const BoxRegion& exposed, 
			 GC gc,
			 bool context_selected, 
			 BoxDimension dimen) const
{
    BoxSize space   = r.space();
    BoxPoint origin = r.origin();

    // Restlichen Platz, Restlichen Platz pro Kind berechnen
    BoxCoordinate remainder = space[dimen] - size(dimen);
    BoxCoordinate remainder_per_extend =
	(extend(dimen) == 0 || remainder < 0) ? 0 : remainder / extend(dimen);
    BoxCoordinate pixel_stuff =
	(extend(dimen) == 0 || remainder < 0) ? 0 : remainder % extend(dimen);

    BoxPoint child_origin = origin;
    BoxSize  child_space  = space;

    for (int i = 0; i < nchildren(); i++)
    {
	Box *child = (Box *)(*this)[i];

	// Wenn Kind erweiterbar, Platz pro Kind hinzufuegen
	child_space[dimen] = child->size(dimen) + 
	    remainder_per_extend * child->extend(dimen);
	
	// Ggf. nicht teilbaren Rest hinzufuegen
	if (pixel_stuff > 0)
	{
	    BoxCoordinate stuff = min(remainder_per_extend, pixel_stuff);
	    child_space[dimen] += stuff;
	    pixel_stuff -= stuff;
	}

	// Kind anzeigen
	child->draw(w, 
		    BoxRegion(child_origin, child_space), 
		    exposed, gc, context_selected);

	// Naechstes Kind daneben anzeigen
	child_origin[dimen] += child_space[dimen];
    }
}

// Groesse neu berechnen
Box *AlignBox::resize()
{
    for (int i = 0; i < nchildren(); i++)
    {
	Box* child = (*this)[i];
	if (i == 0)
	    setSize(child->resize());
	else
	    addSize(child->resize());
    }
    return this;
}





// HAlignBox

// Box zu horizontalem Alignment hinzufuegen
void HAlignBox::addSize(Box *b)
{
    thesize()   &= b->size();
    theextend() &= b->extend();

    // Freien Platz unten rechts uebernehmen
    if (b->size(X) > 0)
	_corner = b->corner();
}


void HAlignBox::_print(ostream& os, 
		       const BoxRegion& region, 
		       const BoxPrintGC& gc) const
{
    int i;

    BoxSize space   = region.space();
    BoxPoint origin = region.origin();

    BoxCoordinate rem = space[X] - size(X);
    BoxCoordinate perextend = 
	(extend(X) == 0 || rem <= 0) ? 0 : rem / extend(X);
    BoxCoordinate pixels = 
	(extend(X) == 0 || rem <= 0) ? 0 : rem % extend(X);
    
    BoxPoint child_origin = origin;
    BoxSize  child_space;

    for (i = 0; i < nchildren(); i++ ) {
	Box *child = (Box *)(*this)[i];

	child_space[Y] = space[Y];
	child_space[X] = child->size(X) + perextend * child->extend(X);
		
	if (pixels > 0) {
	    BoxCoordinate stuff = min(perextend, pixels);
	    child_space[X] += stuff;
	    pixels -= stuff;
	}

	child->_print(os, BoxRegion (child_origin, child_space), gc);
	
	child_origin[X] += child_space[X];
    }
}






// VAlignBox

void VAlignBox::addSize(Box *b) 
// Box zu vertikalem Alignment hinzufuegen
{
    thesize()   |= b->size();
    theextend() |= b->extend();

    // Freien Platz unten rechts berechnen
    if (b->size(Y) > 0)
    {
	_corner[Y] = b->corner()[Y];
	_corner[X] = b->extend(X) ? 0 : 
	    size(X) - (b->size(X) - b->corner()[X]);
    }
}

void VAlignBox::_print(ostream& os, 
		       const BoxRegion& region, 
		       const BoxPrintGC& gc) const
{
    int i;
#if DEBUG
    os << "#\n";
    os << "# VAlignBox\n";
    os << "# Requiered Region:" << region << "\n";
    os << "# Actual Size:" << size();
    os << "# Extend:" << extend() << "\n";
#endif
    BoxPoint origin = region.origin();
    BoxSize space = region.space();

    BoxCoordinate rem = space[Y] - size(Y);
    BoxCoordinate perextend = 
	(extend(Y) == 0 || rem <= 0) ? 0 : rem / extend(Y);
    BoxCoordinate pixels = 
	(extend(Y) == 0 || rem <= 0) ? 0 : rem % extend(Y);
    
    BoxPoint child_origin = origin;
    BoxSize  child_space;

    for (i = 0; i < nchildren(); i++ ) {
	Box *child = (Box *)(*this)[i];

	child_space[X] = space[X];
	child_space[Y] = child->size(Y) + perextend * child->extend(Y);
	
	if (pixels > 0) {
	    BoxCoordinate stuff = min (perextend, pixels);
	    child_space[X] += stuff;
	    pixels -= stuff;
	}

	child->_print(os, BoxRegion (child_origin, child_space), gc);
	child_origin[Y] += child_space[Y];
    }
}










// UAlignBox

void UAlignBox::addSize(Box *b)
// Box zu gestapeltem Alignment hinzufuegen
{
    thesize()   ^= b->size();
    theextend() ^= b->extend();

    // Freien Platz unten rechts uebernehmen
    if (b->size() > BoxSize(0, 0))
	_corner = b->corner();
}

// Gestapeltes Alignment anzeigen
void UAlignBox::_draw(Widget w, 
		      const BoxRegion& r, 
		      const BoxRegion& exposed, 
		      GC gc,
		      bool context_selected) const
{
    // Alle Kinder an der gleichen Stelle anzeigen
    for (int i = 0; i < nchildren(); i++)
    {
	Box *child = (Box *)(*this)[i];
	child->draw(w, r, exposed, gc, context_selected);
    }
}

// Gestapeltes Alignment drucken
void UAlignBox::_print(ostream& os, 
		       const BoxRegion& region, 
		       const BoxPrintGC& gc) const
{
    for (int i = 0; i < nchildren(); i++) {
	(*this)[i]->_print(os, region, gc);
    }
}





// TAlignBox

void TAlignBox::addSize(Box *b) 
// Box zu textuellem Alignment hinzufuegen
{
    // In Eckplatz einpassen
    thesize()   += (b->size() ^ _corner) - _corner;
    theextend() &= b->extend();

    // Eckplatz anpassen
    if (b->size(X) >= _corner[X])
    {
	// Corner uebernehmen
	_corner = b->corner();  
    }
    else
    {
	// Corner verkleinern
	_corner[X] -= b->size(X);
	_corner[Y]  = max(b->size(Y), _corner[Y]);
    }
}

// Textuelles Alignment anzeigen
void TAlignBox::_draw(Widget w, 
		      const BoxRegion& r, 
		      const BoxRegion& exposed, 
		      GC gc,
		      bool context_selected) const
{
    BoxSize space   = r.space();
    BoxPoint origin = r.origin();

    // Wenn nicht erweiterbar, Platz auf minimalen Platz reduzieren
    if (extend(X) == 0)
	space[X] = size(X);
    if (extend(Y) == 0)
	space[Y] = size(Y);

    // Restlichen Platz, Restlichen Platz pro Kind berechnen
    BoxCoordinate remainder = space[X] - size(X);
    BoxCoordinate remainder_per_extend = 
	(extend(X) == 0 || remainder < 0) ? 0 : remainder / extend(X);
    BoxCoordinate pixel_stuff = 
	(extend(X) == 0 || remainder < 0) ? 0 : remainder % extend(X);

    BoxPoint child_origin = origin;
    BoxSize  child_space  = space;

    for (int i = 0; i < nchildren(); i++)
    {
	Box *child = (Box *)(*this)[i];

	// Wenn Kind erweiterbar, Platz pro Kind hinzufuegen
	child_space[X] = child->size(X) + 
	    remainder_per_extend * child->extend(X);
	
	// Ggf. nicht teilbaren Rest hinzufuegen
	if (pixel_stuff > 0)
	{
	    BoxCoordinate stuff = min(remainder_per_extend, pixel_stuff);
	    child_space[X] += stuff;
	    pixel_stuff -= stuff;
	}

	// Kind anzeigen
	child->draw(w, BoxRegion(child_origin, child_space), 
		    exposed, gc, context_selected);
	
	// Naechstes Kind in Ecke einpassen
	child_origin[X] += (child_space[X] - child->corner()[X]);
	child_origin[Y] += (child->size(Y) - child->corner()[Y]);
    }
}

void TAlignBox::_print(ostream& os, 
		       const BoxRegion& region, 
		       const BoxPrintGC& gc) const
{
    BoxSize space   = region.space();
    BoxPoint origin = region.origin();

    // if not extendible, reduce to minimum space
	
    if (extend(X) == 0)
	space[X] = size(X);
    if (extend(Y) == 0)
	space[Y] = size(Y);
    
    // calculate remaining space and rem. space per child

    BoxCoordinate remainder = space[X] - size(X);
    BoxCoordinate remainder_per_extend = 
	(extend(X) == 0 || remainder < 0) ? 
	0 : remainder / extend(X);
    BoxCoordinate pixel_stuff = 
	(extend(X) == 0 || remainder < 0) ? 
	0 : remainder % extend(X);
    
    BoxPoint child_origin = origin;
    BoxSize  child_space  = space;

    for (int i = 0; i < nchildren(); i++) 	{
        Box *child = (Box *)(*this)[i];

	// if child extendible, add space per child

	child_space[X] = child->size(X) + 
	    remainder_per_extend * child->extend(X);
	
	// Ggf. nicht teilbaren Rest hinzufuegen
	
	if (pixel_stuff > 0) {
	    BoxCoordinate stuff = 
		min (remainder_per_extend, pixel_stuff);
	    child_space[X] += stuff;
	    pixel_stuff -= stuff;
	}

	// print child
	
	child->_print(os, BoxRegion(child_origin, child_space), gc);
	
	// move child to corner

	child_origin[X] += (child_space[X] - child->corner()[X]);
	child_origin[Y] += (child->size(Y) - child->corner()[Y]);
    }
}
