// $Id$
// VSLLib error handling

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
// Revision 1.1  1995/05/01 15:47:40  zeller
// Initial revision
//
// Revision 10.13  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.12  1994/02/19  20:24:08  zeller
// Neu: Fehlermeldungen jetzt im GNU-Format
// Neu: const string& statt string
// Neu: Kuerzerer Backtrace
//
// Revision 10.11  1993/05/22  20:17:59  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.10  1993/04/22  11:07:42  zeller
// Lizenz verbessert
// 
// Revision 10.9  1993/04/21  15:22:08  zeller
// Fix: Anpassung an <iostream.h> (kein dec() mehr)
// Buffergroesse bei Backtrace-Ausgabe jetzt beliebig
// 
// Revision 10.8  1993/04/21  00:28:12  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.7  1993/04/16  11:44:45  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.6  1993/04/15  10:25:18  zeller
// Neu: Alle Meldungen jetzt mit 'VSLLib:' Praefix;
// Ausgabeformat ueberarbeitet
// 
// Revision 10.5  1993/01/30  16:26:33  zeller
// All messages after german oversitted
// 
// Revision 10.4  1992/11/03  19:37:31  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.3  1992/10/13  20:57:52  zeller
// strclass.h statt String.h
// 
// Revision 10.2  1992/06/01  14:58:41  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.1  1992/02/18  11:19:56  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:59:41  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:03:22  zeller
// Installierte Version
// 
// Revision 4.1  1991/05/25  12:53:33  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.6  1991/05/16  10:53:55  zeller
// Umbenennung, um Warnungen zu vermeiden
// 
// Revision 2.5  1991/05/09  16:06:24  zeller
// Warnungen vermieden
// 
// Revision 2.4  1991/05/07  14:46:35  zeller
// Backtrace direkt ausgeben (nicht erst in String akkumulieren)
// 
// Revision 2.3  1991/05/07  12:47:48  zeller
// Backtrace verbessert
// 
// Revision 2.2  1991/05/07  12:14:23  zeller
// Backtrace mit Argumenten versehen
// 
// Revision 2.1  1991/03/01  17:02:49  zeller
// Initial revision
// 

char VSLErr_rcsid[] = "$Id$";

#include <iostream.h>
#include <strstream.h>
#include "strclass.h"

#include "VSLLib.h"
#include "VSLDef.h"



// Allgemeine Fehler-Schnittstelle

// Fehlerbehandlung
void VSLLib::echo(const string& msg)
{
    cerr << msg << '\n';
}

// Meldung beim Auswerten
void VSLLib::eval_echo(const string& errmsg, const VSLDef *d)
{
    if (d == 0 && VSLDef::backtrace)
    {
	// Oberste Definition suchen
	for (int i = 0; VSLDef::backtrace[i] != 0; i++)
	    d = VSLDef::backtrace[i];
    }

    // Fehlermeldung ausgeben
    if (d)
	echo(d->longname() + ": " + errmsg);
    else
	echo(errmsg);


    // Backtrace ausgeben, wenn gewuenscht
    if (VSLDef::backtrace && VSLDef::backtrace[0])
    {
	static char line[] = "------------------------------";

	echo(line);
	echo("Current stack:");

	for (int i = 0; VSLDef::backtrace[i] != 0; i++)
	    ;

	for (i--; i >= 0; i--)
	{
	    ostrstream os;
	    os << i << " -- " << VSLDef::backtrace[i]->longname() 
	       << *VSLDef::backtrace_args[i];

	    echo(os);
	}

	echo(line);
    }
}

// Fehler bei Auswertung
void VSLLib::eval_error(const string& errmsg, const VSLDef *d)
{
    eval_echo(errmsg, d);
}

// Warnung bei Auswertung
void VSLLib::eval_warning(const string& errmsg, const VSLDef *d)
{
    eval_echo("warning: " + errmsg, d);
}
