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

// $Log$
// Revision 1.1  1995/05/01 15:47:29  zeller
// Initial revision
//
// Revision 10.15  1995/04/10  19:56:19  zeller
// New: no more <builtin.h>
//
// Revision 10.14  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.13  1994/07/19  21:48:13  zeller
// New: draw() now const correct
//
// Revision 10.12  1994/07/19  18:35:16  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.11  1993/08/27  14:25:55  zeller
// Neu: TypeInfo
//
// Revision 10.10  1993/05/22  20:15:58  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.9  1993/04/22  10:52:14  zeller
// Lizenz verbessert
// 
// Revision 10.8  1993/04/21  00:11:09  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.7  1993/04/16  11:32:00  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.6  1992/11/05  16:37:51  zeller
// Region-Kram jetzt in neuer Oberklasse MarkBox
// 
// Revision 10.5  1992/06/02  03:04:28  zeller
// #pragma besser formatiert
// 
// Revision 10.4  1992/06/01  16:34:39  zeller
// Neu: #pragma definition/#pragma implementation
// 
// Revision 10.3  1992/06/01  14:13:50  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.2  1992/02/18  10:56:55  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.1  1992/02/17  12:50:17  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.1  1991/07/08  05:49:25  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  14:59:27  zeller
// Installierte Version
// 
// Revision 1.25  1991/06/30  18:30:03  zeller
// Tag-Info nur auf Wunsch ausgeben
// 
// Revision 1.24  1991/06/20  18:22:31  zeller
// Aufruf _findTag von Vaterklasse geerbt
// 
// Revision 1.23  1991/06/10  14:07:38  zeller
// Neu: DataLink mit Kopierfunktionen
// 
// Revision 1.22  1991/06/10  12:45:16  zeller
// Neu: info(); Debugging-Ausgabe verbessert
// 
// Revision 1.21  1991/06/09  17:37:28  zeller
// 'selected' in 'context_selected' umbenannt,
// um Compiler-Warnungen zu vermeiden
// 
// Revision 1.20  1991/06/09  17:13:16  zeller
// tag() in findTag() umbenannt
// 
// Revision 1.19  1991/05/09  15:04:16  zeller
// Warnungen vermieden
// 
// Revision 1.18  1991/02/08  16:28:19  zeller
// dump() neu definiert
// 
// Revision 1.17  1991/01/28  19:14:48  zeller
// free() jetzt nicht mehr inline
// 
// Revision 1.16  1990/10/04  18:19:30  zeller
// Umstellung von "Intrinsic.h" auf <X11/Intrinsic.h>
// 
// Revision 1.15  1990/09/20  15:52:02  zeller
// Beim Invertieren nur die exponierte Flaeche bearbeiten
// 
// Revision 1.14  1990/09/18  17:36:13  zeller
// Sohnregion minimalisiert
// 
// Revision 1.13  1990/09/16  13:40:14  zeller
// Anzeige Sohn korrigiert
// 
// Revision 1.12  1990/09/16  13:01:26  zeller
// Neues Zeichen-Interface: draw() statt display()
// 
// Revision 1.11  1990/09/15  14:51:08  zeller
// Bei Anzeige: GC-Funktion nach Anzeige wiederherstellen;
// Wechsel von GXcopy nach GXnor
// 
// Revision 1.10  1990/09/15  14:19:16  zeller
// Callback bei Destruktor eingefuehrt
// 
// Revision 1.9  1990/09/14  11:51:41  zeller
// Neu: selected, name wird jetzt ueber Funktionen bestimmt
// 
// Revision 1.8  1990/09/11  11:48:48  zeller
// selected-Flag eingefuehrt, damit display() ausgewaehlte
// Knoten invers anzeigt
// 
// Revision 1.7  1990/09/07  11:28:38  zeller
// Neu: tag() ohne Parameter gibt oberste TagBox zurueck
// 
// Revision 1.6  1990/09/06  10:38:24  zeller
// tag() gibt jetzt TagBox() zurueck
// (um auch auf Region zugreifen zu koennen)
// 
// Revision 1.5  1990/09/05  13:28:04  zeller
// Ausgabe tag() verbessert
// 
// Revision 1.4  1990/09/05  12:33:21  zeller
// operator << fuer void* definiert
// 
// Revision 1.3  1990/09/05  11:36:19  zeller
// Aufruf HatBox::dump() entfernt (HatBox hat keine dump()-Funktion)
// 
// Revision 1.2  1990/09/05  11:06:07  zeller
// Compilierfaehig gemacht
// 
// Revision 1.1  1990/09/05  10:55:10  zeller
// Initial revision
// 

static const char rcsid[] = 
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
