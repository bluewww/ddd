// $Id$
// read in VSL library

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

char VSLRead_rcsid[] = 
    "$Id$";

#include "assert.h"
#include <stdlib.h>
#include <values.h>
#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
#include "strclass.h"
#include "cook.h"

#include "VSLBuiltin.h"

#include "VSLLib.h"
#include "VSLDef.h"
#include "VSLDefList.h"

#include "Box.h"
#include "PrimitiveB.h"
#include "StringBox.h"
#include "TrueBox.h"
#include "ListBox.h"

#include "VSLNode.h"
#include "ConstNode.h"
#include "ListNode.h"
#include "TestNode.h"
#include "DefCallN.h"
#include "LetNode.h"
#include "ArgNode.h"
#include "DummyNode.h"
#include "NameNode.h"
#include "TrueNode.h"

#include "VSEFlags.h"
#include "config.h"


static VSLLib*  vsllib = 0;                    // Einzulesende VSL-Bibliothek

static string yyfilename = "standard input";   // Aktueller Dateiname


// GNU C++ complains about the declaration of YYSTYPE, so leave it alone
#ifdef __GNUG__
#define YYSTYPE _xy_YYSTYPE
#define _YYSTYPE _xy_underscore_YYSTYPE
#define yylval  _xy_yylval
#include "vsl-gramma.h"
#undef yylval
#undef _YYSTYPE
#undef YYSTYPE
#else
#include "vsl-gramma.h"
#endif

#define yyparse VSLLib::parse

#include "vsl-lex.C"
#include "vsl-gramma.C"

#undef yyparse

// Bibliothek einlesen

// Bibliothek aus geoeffnetem Strom einlesen
VSLLib& VSLLib::read(istream& s, unsigned optimizeMode)
{
    vsllib = this;

    yystream = &s;
    yyfilename = _lib_name;

    // Einlesen...
    yynameSet.reset();
    parse();

    if (VSEFlags::verbose)
    {
	cout << ")";
	cout.flush();
    }

    // Nachbearbeitung (Binden, optimieren, usw.)
    process(optimizeMode);

    if (VSEFlags::verbose)
	cout << ", done.\n";

    // und fertig.
    return *this;
}


// Bibliothek aus gegebenem Dateinamen einlesen
VSLLib& VSLLib::read(const string& lib_name, unsigned optimizeMode)
{
    if (VSEFlags::verbose)
    {
	if (lib_name == "")
	    cout << "standard input";
	else
	    cout << lib_name;
	cout << ": reading";
	cout.flush();
    }

    yyfilename = lib_name;

    switchreset();
    if (switchup(lib_name, True) == 0)
    {
	assert(yystream != 0);

	topstack = 0;
	read(*yystream, optimizeMode);
    }

    return *this;
}


// Fehlerbehandlung

// Yacc-interne Fehlerbehandlung
void yyerror(char *s)
{
    string errmsg = s;

    if (errmsg == "syntax error" || errmsg == "parse error")
	 errmsg += string(" near ") + quote((char *)yytext);

    VSLLib::parse_error(errmsg);
}

// Meldung beim Parsen
void VSLLib::parse_echo(const string& msg)
{
    ostrstream os;
    ostream& s = os;
    
    s << yyfilename << ":" << yylinenumber << ": " << msg;
    echo(os);
}

// Fehler beim Parsen
void VSLLib::parse_error(const string& errmsg)
{
    parse_echo(errmsg);
}

// Warnung beim Parsen
void VSLLib::parse_warning(const string& errmsg)
{
    parse_echo(string("warning: ") + errmsg);
}
