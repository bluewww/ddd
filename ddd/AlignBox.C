// $Id$
// Klasse AlignBox (Implementation)

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
// Revision 1.1  1995/05/01 15:47:29  zeller
// Initial revision
//
// Revision 10.12  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.11  1994/07/19  21:59:42  zeller
// New: draw() const-corrected
//
// Revision 10.10  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.9  1994/07/19  21:04:38  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.8  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.7  1993/05/22  19:53:27  zeller
// %Q% added
//
// Revision 10.6  1993/04/21  00:10:47  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.5  1993/04/16  11:31:19  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.4  1993/02/23  16:17:38  zeller
// Neu: Box::print() -- Box in EPS oder FIG-Format ausgeben
// 
// Revision 10.3  1992/06/01  16:33:16  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.2  1992/02/18  10:57:26  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:51:00  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:41:48  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:57:18  zeller
// Installierte Version
// 
// Revision 1.9  1991/06/09  17:38:03  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.8  1991/05/21  15:21:35  zeller
// Fall ">>" ~ ("BEGIN" | "END") korrekt behandelt
// 
// Revision 1.7  1991/05/09  15:07:35  zeller
// Warnungen vermieden
// 
// Revision 1.6  1991/04/30  16:00:20  zeller
// corner(), TAlignBox wieder eingefuehrt
// 
// Revision 1.5  1991/02/24  15:10:30  zeller
// corner, TAlignBox entfernt
// 
// Revision 1.4  1991/02/08  16:31:37  zeller
// dump() neu definiert
// 
// Revision 1.3  1991/02/05  13:23:02  zeller
// Platzreduzierung bei nicht erweiterbaren Boxen abgeschaltet
// 
// Revision 1.2  1991/02/01  15:45:06  zeller
// Fehler bei Behandlung pixel_stuff korrigiert
// 
// Revision 1.1  1991/02/01  14:56:37  zeller
// Initial revision
// 

#ifdef __GNUG__
#pragma implementation
#endif

char AlignBox_rcsid[] = "$Id$";

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
