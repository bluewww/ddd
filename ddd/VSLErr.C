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

char VSLErr_rcsid[] = 
    "$Id$";

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
