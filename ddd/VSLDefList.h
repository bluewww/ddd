// $Id$
// Deklaration Klasse VSLDefList

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
// Revision 1.1  1995/05/01 15:47:55  zeller
// Initial revision
//
// Revision 10.16  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.15  1994/11/14  21:08:37  zeller
// Fixes for gcc 2.6.1
//
// Revision 10.14  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.13  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.12  1993/05/22  20:17:56  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.11  1993/04/22  11:07:35  zeller
// Lizenz verbessert
// 
// Revision 10.10  1993/04/21  00:28:11  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.9  1993/04/20  23:06:35  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.8  1993/04/20  22:06:22  zeller
// removeSelfReferences() neu ueberarbeitet -- laeuft jetzt
// 
// Revision 10.7  1993/04/16  11:44:44  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.6  1992/11/03  19:37:30  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.5  1992/10/13  20:57:51  zeller
// strclass.h statt String.h
// 
// Revision 10.4  1992/06/02  03:00:08  zeller
// Neu: #pragma interface
// 
// Revision 10.3  1992/06/01  14:58:39  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.2  1992/06/01  14:25:36  zeller
// #pragma once entfernt
// 
// Revision 10.1  1992/02/18  11:19:43  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:59:36  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:03:20  zeller
// Installierte Version
// 
// Revision 4.1  1991/05/25  12:52:51  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.10  1991/05/21  19:57:29  zeller
// Neu: replace()
// 
// Revision 2.9  1991/05/21  19:09:58  zeller
// Neu: override()
// 
// Revision 2.8  1991/05/09  16:05:38  zeller
// Warnungen vermieden
// 
// Revision 2.7  1991/04/30  18:48:10  zeller
// Neue Codierung: einleitendes '$' bei lokalen Funktionen entfernt.
// Lokalitaet durch gesonderten Parameter angegeben.
// 
// Revision 2.6  1991/02/27  01:21:41  zeller
// f_name(): Namen ab erstem alphanumerischem Zeichen ausgeben
// 
// Revision 2.5  1991/02/25  16:23:03  zeller
// NoNode, NoBox, NoExpr entfernt
// 
// Revision 2.4  1991/02/24  19:06:51  zeller
// Default fuer 'expr' jetzt: NoExpr (zuvor: [])
// 
// Revision 2.3  1991/02/21  14:52:20  zeller
// 'pattern' stets vor 'expr' angegeben
// 
// Revision 2.2  1991/02/20  18:51:56  zeller
// Pattern und Argumente nicht mehr auf Listen beschraenkt
// 
// Revision 2.1  1991/02/19  22:49:42  zeller
// Neue Release mit folgenden Eigenschaften:
// * Erweitertes Pattern-Matching auf Funktionsargumente
// * Listen-orientierte Auswertung ohne Stack
// * Verbesserte Fehlererkennung und Debug-Moeglichkeiten
// 
// Revision 1.1  1991/02/17  17:26:20  zeller
// Initial revision
// 

#ifndef _Nora_VSLDefList_h
#define _Nora_VSLDefList_h

#ifdef __GNUG__
#pragma interface
#endif


#include "assert.h"
#include <values.h>
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
