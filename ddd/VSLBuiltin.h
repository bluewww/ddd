// $Id$
// Deklaration builtin()

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
// Revision 1.1  1995/05/01 15:47:54  zeller
// Initial revision
//
// Revision 10.13  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.12  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.11  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.10  1993/05/22  20:17:45  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.9  1993/04/22  11:07:17  zeller
// Lizenz verbessert
// 
// Revision 10.8  1993/04/21  00:28:07  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.7  1993/04/20  23:06:33  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.6  1993/04/16  11:44:40  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.5  1992/11/03  19:37:26  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.4  1992/10/13  20:57:49  zeller
// strclass.h statt String.h
// 
// Revision 10.3  1992/06/02  03:00:07  zeller
// Neu: #pragma interface
// 
// Revision 10.2  1992/06/01  14:25:34  zeller
// #pragma once entfernt
// 
// Revision 10.1  1992/02/18  11:19:33  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:59:09  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:03:08  zeller
// Installierte Version
// 
// Revision 4.1  1991/05/25  12:52:13  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.4  1991/05/21  16:51:14  zeller
// VSLBuiltin::concat_strings entfernt
// (Aerger bei -Ofold-consts und Pattern-Matching)
// 
// Revision 2.3  1991/05/09  16:05:12  zeller
// Warnungen vermieden
// 
// Revision 2.2  1991/04/30  18:47:44  zeller
// Neue Codierung: einleitendes '$' bei lokalen Funktionen entfernt.
// Lokalitaet durch gesonderten Parameter angegeben.
// 
// Revision 2.1  1991/02/19  22:47:42  zeller
// Neue Release mit folgenden Eigenschaften:
// * Erweitertes Pattern-Matching auf Funktionsargumente
// * Listen-orientierte Auswertung ohne Stack
// * Verbesserte Fehlererkennung und Debug-Moeglichkeiten
// 
// Revision 1.7  1991/02/11  16:54:56  zeller
// Neu: VSLBuiltin ist jetzt eigene Klasse;
// Erweiterte Zugriffsfunktionen
// 
// Revision 1.6  1991/01/31  19:14:44  zeller
// concat_strings deklariert
// 
// Revision 1.5  1991/01/29  14:25:13  zeller
// Flag 'Seiteneffekte?' eingefuehrt
// 
// Revision 1.4  1990/08/21  23:04:48  zeller
// Flag isAssoc eingefuehrt
// 
// Revision 1.3  1990/08/19  19:19:11  zeller
// Neues Interface builtin
// 
// Revision 1.2  1990/08/18  18:10:47  zeller
// Typ 'PE' entfernt und durch BuiltinFunc ersetzt
// 
// Revision 1.1  1990/08/14  22:01:31  zeller
// Initial revision
// 

#ifndef _Nora_VSLBuiltin_h
#define _Nora_VSLBuiltin_h

#ifdef __GNUG__
#pragma interface
#endif


#include "strclass.h"
#include "Range.h"
#include "bool.h"
#include "ListBox.h"

typedef Box * (*BuiltinFunc)(ListBox *);

struct VSLBuiltin {
    static int resolve(const string& func_name);
    // Index zu gegebener Funktion zurueckgeben

    static BuiltinFunc func(int index);
    // Funktionsaufruf zu gegebenem Index zurueckgeben

    static bool isAssoc(int functionIndex);
    // Flag: ist Funktion assoziativ?

    static bool hasSideEffects(int functionIndex);
    // Flag: hat Funktion Seiteneffekte?

    static char *func_name(int functionIndex);
    // Name fuer interne Repraesentation ("__op_halign")

    static char *ext_name(int functionIndex);
    // Name fuer externe Repraesentation ("(&)")

    static bool isInfix(int functionIndex);
    // Flag: ext_name infix ausgeben?
};

#endif
