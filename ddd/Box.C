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
	
// $Log$
// Revision 1.1  1995/05/01 15:47:31  zeller
// Initial revision
//
// Revision 10.13  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.12  1994/07/19  22:14:26  zeller
// New: draw() now const correct
//
// Revision 10.11  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.10  1993/08/31  14:50:18  zeller
// BoxMem entfernt
// Layout ueberarbeitet
//
// Revision 10.9  1993/08/27  15:07:50  zeller
// Neu: TypeInfo
//
// Revision 10.8  1993/05/22  19:58:52  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.7  1993/04/21  00:10:49  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.6  1993/04/16  11:31:24  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.5  1992/11/26  11:07:33  zeller
// Sichergestellt, dass Speicher initialisiert (analog FeatureMem)
// 
// Revision 10.4  1992/11/03  18:23:57  zeller
// Aufruf memory.free korrigiert
// 
// Revision 10.3  1992/06/01  16:33:29  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.2  1992/02/18  10:56:56  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:18  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:42:45  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:57:33  zeller
// Installierte Version
// 
// Revision 1.40  1991/06/17  12:28:16  zeller
// Neu: transparentes Matchen mit matchMe()
// 
// Revision 1.39  1991/06/10  14:07:46  zeller
// Neu: DataLink mit Kopierfunktionen
// 
// Revision 1.38  1991/06/10  11:53:45  zeller
// Neu: info()
// 
// Revision 1.37  1991/06/09  17:37:30  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.36  1991/06/09  17:14:14  zeller
// Neu: tag() erzeugt TagBox um Box (TagBox ist damit privat);
// Attribute TagBox von TagBox.h hierher gesetzt.
// 
// Revision 1.35  1991/06/09  16:16:33  zeller
// Neu: VSEFlag()
// 
// Revision 1.34  1991/06/09  16:15:00  zeller
// Neu: -Dshow-draw
// 
// Revision 1.33  1991/06/09  16:00:42  zeller
// Neu: Bei matches() zweites Argument fuer MatchBox-Callback,
// da HatBox zwar gematcht, aber der Callback mit dem Sohn aufgerufen wird.
// 
// Revision 1.32  1991/06/08  17:55:46  zeller
// Fall delete(0) behandelt
// 
// Revision 1.31  1991/05/20  16:20:41  zeller
// delete jetzt nach Box.C verlegt (kann nicht inline sein)
// 
// Revision 1.30  1991/05/20  13:37:13  zeller
// Neu: Box::memory
// 
// Revision 1.29  1991/05/09  16:14:34  zeller
// Warnungen vermieden
// 
// Revision 1.28  1991/05/07  12:10:05  zeller
// Ausgabe Box: Verschachtelunsgstiefe laesst sich jetzt durch
// VSEFlags::max_info_nesting begrenzen
// 
// Revision 1.27  1991/04/30  15:57:01  zeller
// corner() wieder eingefuehrt
// 
// Revision 1.26  1991/02/24  15:09:36  zeller
// corner entfernt
// 
// Revision 1.25  1991/02/21  15:14:14  zeller
// Neue VSEFlags eingefuehrt
// 
// Revision 1.24  1991/02/21  14:15:50  zeller
// Debugging-Ausgabe: \n an Anfang gestellt
// 
// Revision 1.23  1991/02/21  11:45:18  zeller
// Matching: Debugging-Ausgaben verbessert
// 
// Revision 1.22  1991/02/20  19:29:08  zeller
// Neues Matching-Verfahren eingefuehrt
// 
// Revision 1.21  1991/02/10  12:24:33  zeller
// VSEFlags eingefuehrt
// 
// Revision 1.20  1991/02/08  16:36:17  zeller
// dump() neu definiert
// 
// Revision 1.19  1990/10/04  18:19:33  zeller
// Umstellung von "Intrinsic.h" auf <X11/Intrinsic.h>
// 
// Revision 1.18  1990/09/16  14:43:30  zeller
// fastMode entfernt (null Effizienzgewinn)
// 
// Revision 1.17  1990/09/16  13:00:48  zeller
// Neues Zeichen-Interface: draw() statt display()
// 
// Revision 1.16  1990/09/14  18:25:09  zeller
// fastMode eingefuehrt (zum schnellen Berechnen)
// 
// Revision 1.15  1990/09/10  19:14:18  zeller
// Ausgabe corner() mit eingefuegt
// 
// Revision 1.14  1990/09/06  11:36:17  zeller
// Funktionen nach Box.h ausgelagert
// 
// Revision 1.13  1990/08/30  11:37:33  zeller
// link(), unlink() keine inline's mehr (zum Debuggen);
// magic eingefuehrt (ebenfalls zum Debuggen)
// 
// Revision 1.12  1990/08/28  16:53:45  zeller
// Methode OK() eingefuehrt
// 
// Revision 1.11  1990/08/25  17:07:31  zeller
// Link-Verwaltung von Destruktor nach unlink() ausgelagert
// 
// Revision 1.10  1990/08/25  15:00:54  zeller
// Assertion eingefuegt
// 
// Revision 1.9  1990/08/25  14:19:50  zeller
// ~Box loescht Box jetzt nur bei Aufloesung letzter Link
// 
// Revision 1.8  1990/08/25  13:49:43  zeller
// Destruktor ueberprueft, dass auch keine Links mehr vorhanden
// 
// Revision 1.7  1990/08/19  12:16:44  zeller
// Slot 'next' entfernt
// 
// Revision 1.6  1990/08/13  12:54:15  zeller
// Ausgabeformat Box an VSL-Kommentarkonvention angepasst
// 
// Revision 1.5  1990/08/10  12:37:05  zeller
// Rueckgabewert korrigiert
// 
// Revision 1.4  1990/08/01  21:13:55  zeller
// Neuausgabe in Kommentarform
// 
// Revision 1.3  1990/07/25  18:54:33  zeller
// _sccsid durch _sccsid[] ersetzt
// 
// Revision 1.2  1990/07/25  18:45:05  zeller
// _sccsid[] eingefuehrt
// 
// Revision 1.1  1990/07/25  18:31:45  zeller
// Initial revision
// 

#ifdef __GNUG__
#pragma implementation
#endif

char Box_rcsid[] = "$Id$";

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
