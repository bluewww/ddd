// $Id$
// Deklaration Klasse VSLDef

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
// Revision 1.1  1995/05/01 15:47:39  zeller
// Initial revision
//
// Revision 10.14  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.13  1994/11/14  21:08:21  zeller
// Fixes for gcc 2.6.1
//
// Revision 10.12  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.11  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.10  1993/05/22  20:17:51  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.9  1993/04/22  11:07:16  zeller
// Lizenz verbessert
// 
// Revision 10.8  1993/04/21  00:28:09  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.7  1993/04/20  23:06:34  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.6  1993/04/16  11:44:42  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.5  1992/11/03  19:37:28  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.4  1992/10/13  20:57:50  zeller
// strclass.h statt String.h
// 
// Revision 10.3  1992/06/02  03:00:08  zeller
// Neu: #pragma interface
// 
// Revision 10.2  1992/06/01  14:25:35  zeller
// #pragma once entfernt
// 
// Revision 10.1  1992/02/18  11:19:39  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:59:22  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:03:15  zeller
// Installierte Version
// 
// Revision 4.1  1991/05/25  12:52:35  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.18  1991/05/23  18:53:26  zeller
// Neues Namenskonzept (schon wieder):
// Namen werden erst aufgeloest, wenn gesamter Ausdruck eingelesen.
// 
// Revision 2.17  1991/05/23  11:01:19  zeller
// Neu: Vor dem Uebersetzen der Pattern zunaechst alte Pattern loeschen
// 
// Revision 2.16  1991/05/23  10:47:22  zeller
// compile_pattern in compilePattern umbenannt
// 
// Revision 2.15  1991/05/22  13:53:23  zeller
// Typ being_compiled korrigiert
// 
// Revision 2.14  1991/05/09  16:05:24  zeller
// Warnungen vermieden
// 
// Revision 2.13  1991/05/07  12:14:15  zeller
// Backtrace mit Argumenten versehen
// 
// Revision 2.12  1991/04/27  18:49:15  zeller
// Schutz gegen unendlich rekursiven Aufruf eingefuehrt
// 
// Revision 2.11  1991/03/01  16:58:47  zeller
// Neu: nodelist()
// 
// Revision 2.10  1991/02/26  18:00:55  zeller
// isStraight() eingefuehrt
// 
// Revision 2.9  1991/02/26  14:11:59  zeller
// Neu: nargs()
// 
// Revision 2.8  1991/02/25  16:22:27  zeller
// NoNode, NoBox, NoExpr entfernt
// 
// Revision 2.7  1991/02/24  20:44:08  zeller
// backtrace neu (als Zeiger) definiert;
// Compiler hat alte Definition falsch interpretiert
// 
// Revision 2.6  1991/02/24  19:06:19  zeller
// Neue Pruefungen 'NoBox' eingefuehrt
// 
// Revision 2.5  1991/02/21  14:52:00  zeller
// 'pattern' stets vor 'expr' angegeben
// 
// Revision 2.4  1991/02/21  12:38:34  zeller
// _box_pattern jetzt nicht mehr ListBox
// 
// Revision 2.3  1991/02/20  19:50:41  zeller
// PEQ nach PEQ.h ausgelagert
// 
// Revision 2.2  1991/02/20  18:48:27  zeller
// Pattern kann jetzt beliebiger VSL-Baum sein
// 
// Revision 2.1  1991/02/19  22:49:23  zeller
// Neue Release mit folgenden Eigenschaften:
// * Erweitertes Pattern-Matching auf Funktionsargumente
// * Listen-orientierte Auswertung ohne Stack
// * Verbesserte Fehlererkennung und Debug-Moeglichkeiten
// 
// Revision 1.1  1991/02/17  17:26:17  zeller
// Initial revision
// 

#ifndef _Nora_VSLDef_h
#define _Nora_VSLDef_h

#ifdef __GNUG__
#pragma interface
#endif


#include <values.h>
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
