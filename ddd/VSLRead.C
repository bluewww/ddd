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

// $Log$
// Revision 1.1  1995/05/01 15:47:20  zeller
// Initial revision
//
// Revision 10.23  1995/04/21  16:35:27  zeller
// New: <std.h> is not required
//
// Revision 10.22  1995/03/17  07:36:02  zeller
// Removed unused variables
//
// Revision 10.21  1995/03/13  14:56:22  zeller
// New: use VSLLib::parse() instead of yyparse()
//
// Revision 10.20  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.19  1994/02/19  20:22:04  zeller
// Fix: _YYSTYPE korrekt umdefiniert
// Neu: NORA-Pfade mit hineinuebersetzen
// Neu: const string& statt string
//
// Revision 10.18  1993/12/07  12:41:42  zeller
// Fix: Anpassung ANSI-Overloading
//
// Revision 10.17  1993/11/11  18:07:36  zeller
// Neu: Anpassung Ausgabe an GNU-Standards
//
// Revision 10.16  1993/10/15  22:13:12  zeller
// Fix: yyerror() muss static sein
//
// Revision 10.15  1993/05/22  20:18:18  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.14  1993/04/22  11:07:49  zeller
// Lizenz verbessert
// 
// Revision 10.13  1993/04/21  15:22:39  zeller
// Fix: Anpassung an <iostream.h> (kein dec() mehr)
// 
// Revision 10.12  1993/04/21  00:28:19  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.11  1993/04/20  21:56:29  zeller
// Anpassung an FLEX und BISON
// 
// Revision 10.10  1993/04/20  20:48:03  zeller
// Neu: Anpassung an FLEX und BISON
// 
// Revision 10.9  1993/04/16  11:44:52  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.8  1993/01/30  16:26:35  zeller
// All messages after german oversitted
// 
// Revision 10.7  1992/11/24  11:05:52  zeller
// Neu: cook.h (fuer unquote())
// 
// Revision 10.6  1992/11/03  19:37:37  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.5  1992/10/13  20:57:53  zeller
// strclass.h statt String.h
// 
// Revision 10.4  1992/07/30  09:07:34  zeller
// Fix: Zuweisung yystream korrigiert
// 
// Revision 10.3  1992/06/01  14:58:50  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.2  1992/05/20  15:37:26  zeller
// Dateinamen verkuerzt
// 
// Revision 10.1  1992/02/18  11:19:35  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  06:00:24  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:03:45  zeller
// Installierte Version
// 
// Revision 4.4  1991/07/02  14:36:59  zeller
// verbose jetzt in VSEFlags
// 
// Revision 4.3  1991/06/30  17:31:54  zeller
// VarList.h entfernt
// 
// Revision 4.2  1991/06/24  19:48:22  zeller
// Neu: Vor Einlesen explizit Namensmenge leeren
// 
// Revision 4.1  1991/05/25  12:52:19  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 3.8  1991/05/24  18:55:57  zeller
// DummyNode eingefuehrt
// 
// Revision 3.7  1991/05/23  18:53:06  zeller
// varlist entfernt;
// Neues Namenskonzept (schon wieder):
// Namen werden erst aufgeloest, wenn gesamter Ausdruck eingelesen.
// 
// Revision 3.6  1991/05/22  12:06:33  zeller
// Let- und Where-Konstrukte implementiert
// 
// Revision 3.5  1991/05/22  11:28:06  zeller
// Neues Namenskonzept: erst alle Namen einlesen
// und erst spaeter durch ArgNode's ersetzen
// 
// Revision 3.4  1991/05/09  16:05:15  zeller
// Warnungen vermieden
// 
// Revision 3.3  1991/04/30  14:27:23  zeller
// Neue Schnittstelle: process() statt bind() und optimize().
// 
// Revision 3.2  1991/03/01  16:58:27  zeller
// Teil Fehlermeldungen nach 'VSLErr' ausgelagert
// 
// Revision 3.1  1991/02/26  23:49:49  zeller
// ListBox eingefuehrt
// 
// Revision 2.6  1991/02/25  16:21:45  zeller
// TrueNode's eingefuehrt
// 
// Revision 2.5  1991/02/24  20:46:48  zeller
// Pruefung, ob backtrace definiert, eingefuegt
// 
// Revision 2.4  1991/02/24  15:40:30  zeller
// MatchNode's entfernt
// 
// Revision 2.3  1991/02/20  18:47:31  zeller
// Fehlerfunktionen jetzt wieder 'void'
// (wg. neuer Fehlerbehandlung in eval())
// 
// Revision 2.2  1991/02/20  11:57:19  zeller
// Jeder VSLNode-Klasse eigene Datei zugewiesen
// 
// Revision 2.1  1991/02/19  22:48:44  zeller
// Neue Release mit folgenden Eigenschaften:
// * Erweitertes Pattern-Matching auf Funktionsargumente
// * Listen-orientierte Auswertung ohne Stack
// * Verbesserte Fehlererkennung und Debug-Moeglichkeiten
// 
// Revision 1.23  1991/02/17  17:27:06  zeller
// VSLLibP.h durch VSLDef.h, VSLDefList.h ersetzt
// 
// Revision 1.22  1991/02/14  13:19:04  zeller
// TrueBox, FalseBox eingefuehrt
// 
// Revision 1.21  1991/02/06  11:03:08  zeller
// optimize() nimmt jetzt Bit-Muster an, welche Optimierungen
// auszufuehren sind
// 
// Revision 1.20  1991/01/24  13:04:51  zeller
// Pruefung unendliche Rekursion eingefuegt ("maxDepth")
// 
// Revision 1.19  1991/01/23  18:17:35  zeller
// Neu: varstate gibt an, ob Funktionsdeklaration oder
// Funktionskoerper eingelesen wird
// 
// Revision 1.18  1990/10/06  14:01:34  zeller
// Option 'verbose' unterstuetzt
// 
// Revision 1.17  1990/10/02  13:14:13  zeller
// yyerror(), yywrap() wie in der Deklaration oeffentlich gemacht
// 
// Revision 1.16  1990/09/19  12:05:07  zeller
// Linefeed bei Fehlermeldungen vorneangestellt
// 
// Revision 1.15  1990/09/07  14:01:12  zeller
// StringBox.h jetzt separat eingefuegt
// 
// Revision 1.14  1990/09/07  10:46:57  zeller
// funcdef entfernt
// 
// Revision 1.13  1990/09/03  15:46:42  zeller
// Deklarationen switchup(), switchdown() von vsl-grammar.Y
// hier eingefuegt
// 
// Revision 1.12  1990/09/01  15:35:19  zeller
// Privaten Teil VSLLib eingebunden
// 
// Revision 1.11  1990/09/01  13:48:04  zeller
// Lange Dateinamen eingefuehrt;
// globale Namen mit 'yy' beginnen lassen
// 
// Revision 1.10  1990/08/30  08:52:41  zeller
// optimizing optional gemacht
// 
// Revision 1.9  1990/08/29  12:27:12  zeller
// Bei Sytaxfehler: fehlerhaftes Token ausgeben
// 
// Revision 1.8  1990/08/28  19:40:02  zeller
// Kleinkram
// 
// Revision 1.7  1990/08/25  17:15:01  zeller
// link() durch bind() ersetzt
// 
// Revision 1.6  1990/08/21  13:14:15  zeller
// Umstellung auf Streams
// 
// Revision 1.5  1990/08/20  12:43:32  zeller
// C-Strings eingefuehrt
// 
// Revision 1.4  1990/08/19  20:04:36  zeller
// Pruefung auf Rekursiven Aufruf eingefuehrt
// 
// Revision 1.3  1990/08/19  19:20:04  zeller
// Neue Fehlerbehandlung; Funktionen komfortabler gemacht
// 
// Revision 1.2  1990/08/18  13:46:06  zeller
// Yacc-Debugging erlaubt (per YYDEBUG)
// 
// Revision 1.1  1990/08/17  17:47:05  zeller
// Initial revision
// 

char VSLRead_rcsid[] = "$Id$";

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
