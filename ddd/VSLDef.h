// $Id$
// Deklaration Klasse VSLDef

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

#ifndef _DDD_VSLDef_h
#define _DDD_VSLDef_h

#ifdef __GNUG__
#pragma interface
#endif


#include <limits.h>
#include <iostream.h>
#include "strclass.h"
#include "bool.h"
#include "VSLNode.h"
#include "TypeInfo.h"

class Box;
class VSLDefList;

class VSLDef {
public:
    DECLARE_TYPE_INFO

private:
    VSLNode *_expr;             // Ausdruck (Definitionskoerper)
    VSLNode *_node_pattern;     // Pattern
    Box *_box_pattern;          // Compiliertes Pattern

    unsigned _nargs;            // Anzahl Argumente
    bool _straight;          // Flag: Argumentliste direkt uebernehmen?

    string _filename;           // Position Definition
    int _lineno;

    VSLDef *_listnext;          // naechste Definition in VSLDefList
    VSLDef *_libnext;           // naechste Definition in VSLLib
    VSLDef *_libprev;           // vorige Definition in VSLLib

    string args() const;        // Argumentliste erzeugen

    bool being_compiled;     // Schutz gegen rekursives compilePattern()

public:
    VSLDefList *deflist;        // Vater

    // Erzeugen
    VSLDef(VSLDefList* l, VSLNode *pattern, VSLNode *e = 0,
	string filename = "builtin", int lineno = 0):
	_expr(e),
	_node_pattern(pattern),
	_box_pattern(0),
	_nargs(pattern->nargs()),
	_straight(pattern->isStraight()),
	_filename(filename),
	_lineno(lineno),
	_listnext(0), _libnext(0), _libprev(0),
	being_compiled(false),
	deflist(l)
    {}

    // Ressourcen
    VSLNode*& expr()            { return _expr; }
    VSLNode*& node_pattern()    { return _node_pattern; }
    Box*& box_pattern()         { return _box_pattern; }
    unsigned nargs() const      { return _nargs; }
    bool straight() const    { return _straight; }

    VSLDef*& listnext()         { return _listnext; }
    VSLDef*& libnext()          { return _libnext; }
    VSLDef*& libprev()          { return _libprev; }

    VSLDef* listnext() const    { return _listnext; }
    VSLDef* libnext() const     { return _libnext; }
    VSLDef* libprev() const     { return _libprev; }

    string func_name() const;     // Interner Name (incl. Argumenten)
    string f_name() const;        // Externer Name (incl. Argumenten)
    string longname() const;      // Externer Name (incl. Argumenten und Datei)

    // Auswerten
    const Box *eval(Box *arg) const;

    // Backtrace (bei Fehlern)
    static const VSLDef **backtrace;
    static const Box **backtrace_args;

    // box_pattern erzeugen
    void compilePattern() const;

    // box_pattern zerstoeren
    void uncompilePattern() const
    {
	if (_box_pattern) 
	    ((VSLDef *)this)->_box_pattern->unlink();
	((VSLDef *)this)->_box_pattern = 0;
    }

    // Namen aufloesen
    int resolveNames();

    // Pruefen, ob Definition gematched wird
    bool matches(const Box *arg) const;
    bool matches(const VSLNode *arg) const;

    // Argument in Argumentliste konvertieren
    ListBox *arglist(const Box *arg) const;

    // Instanzen extrahieren
    VSLNode **nodelist(const VSLNode *arg) const;

    // Zerstoeren
    virtual ~VSLDef();

    // Repraesentations-Invariante
    virtual bool OK() const;
};

#endif
