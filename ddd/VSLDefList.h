// $Id$
// Deklaration Klasse VSLDefList

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

#ifndef _Nora_VSLDefList_h
#define _Nora_VSLDefList_h

#ifdef __GNUG__
#pragma interface
#endif


#include "assert.h"
#include <limits.h>
#include <iostream.h>
#include "strclass.h"
#include "bool.h"
#include "TypeInfo.h"

class VSLLib;
class VSLDef;
class VSLNode;

class VSLDefList {
public:
    DECLARE_TYPE_INFO

private:
    string _func_name;      // Name der Funktion
    VSLDef *_first;         // erste Definition fuer diesen Funktionsnamen
    VSLDef *_last;          // ...letzte
    unsigned _ndefs;        // Anzahl Definitionen
    VSLDefList *_next;      // Naechster Funktionsname
    bool _global;        // Sichtbarkeitsbereich: global?

public:
    VSLLib *lib;            // Library, zu der dieser Knoten gehoert
    unsigned hashcode;      // Index in Hash-Tabelle der Library

    int references;         // #Auftreten Funktion in VSLNode's
    int self_references;    // #Auftreten Funktion in eigenen Definitionen
			    // (-1: unbekannt)

    // Erzeugen
    VSLDefList(VSLLib* l, unsigned hash, 
	string func_nm, bool g = false):
	_func_name(func_nm), _first(0), _last(0), _ndefs(0),
	_next(0), _global(g), lib(l), hashcode(hash), 
	references(0), self_references(-1)
    {}

    // Hinzufuegen
    VSLDef *add(bool& newFlag,
	VSLNode *pattern, VSLNode *expr = 0,
	string filename = "builtin", int lineno = 0);

    // Herausholen
    VSLDef *def(Box *arglist) const;
    VSLDef *firstdef() const { assert (_ndefs == 1); return _first; }

    // Ressourcen
    string func_name() const { return _func_name; }
    bool &global()        { return _global; }
    bool global() const   { return _global; }

    string f_name() const
    {
	if (_func_name[0] == '#')
	{
	    string tmp = _func_name;
	    return tmp.from(1);
	}
	else
	    return _func_name; 
    }

    VSLDef* first()     { return _first; }
    VSLDefList*& next() { return _next; }
    unsigned ndefs()    { return _ndefs; }

    // Auswerten
    const Box *eval(Box *arg) const;

    // Umbenennen
    void override()     
    { 
	_func_name += '\'';     // Wir haengen einfach ein ' hinten an
    }

    // Definitionen Loeschen
    void replace();

    // Zerstoeren
    virtual ~VSLDefList();

    // Repraesentations-Invariante
    bool OK() const;
};

#endif
