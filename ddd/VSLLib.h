// $Id$ 
// The VSL Library

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
// Revision 1.1  1995/05/01 15:47:44  zeller
// Initial revision
//
// Revision 10.21  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.20  1995/03/13  14:55:16  zeller
// New: use VSLLib::parse() instead of yyparse()
//
// Revision 10.19  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.18  1994/02/19  21:56:57  zeller
// Neu: VSLArg mit link()/unlink()-Funktionen ausgestattet
//
// Revision 10.17  1994/02/19  21:31:17  zeller
// Neu: VSLArg mit neuen, einfachen Schnittstellen
// Neu: const string& statt string
//
// Revision 10.16  1993/11/11  13:39:38  zeller
// Fix: friends-Deklaration verbessert
//
// Revision 10.15  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.14  1993/05/22  20:18:07  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.13  1993/04/22  11:07:19  zeller
// Lizenz verbessert
// 
// Revision 10.12  1993/04/21  00:28:15  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.11  1993/04/20  23:03:45  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.10  1993/04/20  22:06:25  zeller
// removeSelfReferences() neu ueberarbeitet -- laeuft jetzt
// 
// Revision 10.9  1993/04/16  11:44:48  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.8  1993/04/15  10:07:46  zeller
// Kommentare jetzt auf englisch
// 
// Revision 10.7  1993/04/15  09:51:33  zeller
// Neu: VSLLib::has()
// 
// Revision 10.6  1992/11/03  19:37:33  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.5  1992/10/13  20:57:52  zeller
// strclass.h statt String.h
// 
// Revision 10.4  1992/06/02  03:00:09  zeller
// Neu: #pragma interface
// 
// Revision 10.3  1992/06/01  14:58:45  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.2  1992/06/01  14:25:37  zeller
// #pragma once entfernt
// 
// Revision 10.1  1992/02/18  11:19:31  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:59:58  zeller
// Installierte Version
// 
// Revision 8.2  1991/07/08  04:50:20  zeller
// Neu: Hintergrund-Prozeduren
// 
// Revision 8.1  1991/07/06  15:03:31  zeller
// Installierte Version
// 
// Revision 4.2  1991/07/02  14:36:28  zeller
// Neu: dumpTree();
// verbose jetzt in VSEFlags.
// 
// Revision 4.1  1991/05/25  12:52:05  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.18  1991/05/23  18:52:48  zeller
// Neues Namenskonzept (schon wieder):
// Namen werden erst aufgeloest, wenn gesamter Ausdruck eingelesen.
// 
// Revision 2.17  1991/05/22  11:27:56  zeller
// Neues Namenskonzept: erst alle Namen einlesen
// und erst spaeter durch ArgNode's ersetzen;
// deshalb declareArg(), arg() entfernt.
// 
// Revision 2.16  1991/05/21  19:57:12  zeller
// Neu: replace()
// 
// Revision 2.15  1991/05/21  19:09:31  zeller
// Neu: override()
// 
// Revision 2.14  1991/05/09  16:05:04  zeller
// Warnungen vermieden
// 
// Revision 2.13  1991/05/09  10:56:48  zeller
// Neu: firstdef() (fuer "vslall")
// 
// Revision 2.12  1991/05/08  15:03:08  zeller
// Neu: output() wendet VSL-Funktion __output()
// auf Ergebnis an (fuer Aufraeumarbeiten usw.)
// 
// Revision 2.11  1991/04/30  18:47:23  zeller
// Neue Codierung: einleitendes '$' bei lokalen Funktionen entfernt.
// Lokalitaet durch gesonderten Parameter angegeben.
// 
// Revision 2.10  1991/04/30  14:58:00  zeller
// HASHSIZE zu Klassen-interner Konstanten gemacht;
// Bits in Modus-Flag _ vorangestellt
// 
// Revision 2.9  1991/04/30  14:26:39  zeller
// Optimierungsfunktionen zergliedert;
// Optimierungsflags beginnen jetzt mit Grossbuchstaben.
// 
// Revision 2.8  1991/03/01  15:12:11  zeller
// Operatoren etc. werden jetzt in 'builtin.vsl' deklariert
// 
// Revision 2.7  1991/02/27  03:05:37  zeller
// Neues call-Interface implementiert
// 
// Revision 2.6  1991/02/25  16:21:08  zeller
// NoNode, NoBox, NoExpr entfernt
// 
// Revision 2.5  1991/02/24  19:07:15  zeller
// Default fuer 'expr' jetzt: NoExpr (zuvor: [])
// 
// Revision 2.4  1991/02/21  14:52:42  zeller
// 'pattern' stets vor 'expr' angegeben
// 
// Revision 2.3  1991/02/21  12:23:08  zeller
// Neu: lastdef() statt lastexpr()
// 
// Revision 2.2  1991/02/20  18:45:51  zeller
// Pattern kann jetzt beliebiger VSL-Baum sein
// 
// Revision 2.1  1991/02/19  22:47:06  zeller
// Neue Release mit folgenden Eigenschaften:
// * Erweitertes Pattern-Matching auf Funktionsargumente
// * Listen-orientierte Auswertung ohne Stack
// * Verbesserte Fehlererkennung und Debug-Moeglichkeiten
// 
// Revision 1.30  1991/02/17  17:25:57  zeller
// STD_MAXDEPTH durch VSEFlags::max_eval_nesting ersetzt
// 
// Revision 1.29  1991/02/14  11:11:19  zeller
// Neue Namen Optimierungen eingefuehrt
// 
// Revision 1.28  1991/02/11  18:42:56  zeller
// Neu: resolveDefs
// 
// Revision 1.27  1991/02/10  17:44:54  zeller
// Neu: cleanup zum Entfernen unbenutzter Funktionen
// 
// Revision 1.26  1991/02/08  18:30:40  zeller
// dump() durch << ersetzt
// 
// Revision 1.25  1991/02/08  17:10:28  zeller
// optimize-Maske kann jetzt noch #Schleifen enthalten
// 
// Revision 1.24  1991/02/06  11:02:37  zeller
// optimize() nimmt jetzt Bit-Muster an, welche Optimierungen
// auszufuehren sind
// 
// Revision 1.23  1991/01/28  22:14:16  zeller
// display entfernt (wird nicht mehr benoetigt)
// 
// Revision 1.22  1991/01/28  19:17:46  zeller
// Neu: VSLLib::display
// 
// Revision 1.21  1991/01/24  13:03:37  zeller
// Pruefung unendliche Rekursion eingefuegt ("maxDepth")
// 
// Revision 1.20  1991/01/23  18:17:00  zeller
// Matching auf Konstanten eingefuehrt
// 
// Revision 1.19  1990/10/06  14:01:22  zeller
// Option 'verbose' unterstuetzt
// 
// Revision 1.18  1990/09/01  19:34:37  zeller
// VSLNode nicht mehr importiert
// 
// Revision 1.17  1990/09/01  16:01:10  zeller
// Neu: VSLLib::eval()
// 
// Revision 1.16  1990/09/01  15:34:52  zeller
// Nicht-oeffentliche Teile nach VSLLibP.h ausgelagert
// 
// Revision 1.15  1990/08/30  08:52:21  zeller
// optimizing optional gemacht
// 
// Revision 1.14  1990/08/28  18:57:47  zeller
// VSLLib::first durch _first, VSLLib::last durch _last ersetzt
// 
// Revision 1.13  1990/08/28  18:33:06  zeller
// Neu: verkettete Liste ueber alle Definitionen eingefuehrt
// Daten kommentiert
// 
// Revision 1.12  1990/08/28  17:34:27  zeller
// Methoden OK() eingefuehrt
// 
// Revision 1.11  1990/08/25  17:15:05  zeller
// link() durch bind() ersetzt
// 
// Revision 1.10  1990/08/23  14:38:17  zeller
//  Destruktoren eingefuehrt
// 
// Revision 1.9  1990/08/21  23:04:19  zeller
// Optimierende Funktionen eingefuehrt
// 
// Revision 1.8  1990/08/21  13:54:20  zeller
// next() muss Referenz-Rueckgabewert haben
// 
// Revision 1.7  1990/08/21  13:13:43  zeller
// Umstellung auf Streams
// 
// Revision 1.6  1990/08/20  12:22:50  zeller
// neues Interface *::add;
// 
// Revision 1.5  1990/08/19  19:39:50  zeller
// Fehlermeldungen koennen mittels Parameter echo_error
// ausgeschaltet werden
// 
// Revision 1.4  1990/08/19  19:18:30  zeller
// VSLDefList: Zeiger auf VSLLib eingefuehrt;
// Neu: VSLLib::error(), VSLLib::warning()
// 
// Revision 1.3  1990/08/19  13:20:19  zeller
// add() gibt jetzt Definition zurueck (guenstig im
// Falle bereits deklarierter Funktionen)
// 
// Revision 1.2  1990/08/17  17:42:44  zeller
// Umstellung von min_nargs, max_nargs auf Range
// 
// Revision 1.1  1990/08/14  20:33:14  zeller
// Initial revision
// 

#ifndef _Nora_VSLLib_h
#define _Nora_VSLLib_h

#ifdef __GNUG__
#pragma interface
#endif


#include <iostream.h>
#include "strclass.h"

#include "bool.h"
#include "VSEFlags.h"
#include "TypeInfo.h"
#include "StringBox.h"
#include "ListBox.h"
#include "TagBox.h"
#include "PrimitiveB.h"

class VSLDef;
class VSLDefList;
class VSLNode;

class VSLLib;
typedef int (VSLLib::*VSLLibAction)();

// Custom eval() arguments
// This is just a wrapper around a Box pointer with a few fancy conversions.
class VSLArg {
private:
    Box *_box;

public:
    Box *box() const { return _box; }

    VSLArg()
	:_box(0)
    {}

    VSLArg(char *s)
	:_box(new StringBox(s))
    {}
    
    VSLArg(const string& s)
	:_box(new StringBox(s))
    {}
    
    VSLArg(int n)
	:_box(new SquareBox(n))
    {}

    VSLArg(unsigned n)
	:_box(new SquareBox(n))
    {}

    // When we are assigned a box, we take control over it
    VSLArg(Box *box)
	:_box(box)
    {}

    // When we are assigned a VSLArg, we establish another link...
    VSLArg(const VSLArg& arg)
	:_box(arg.box() ? arg.box()->link() : 0)
    {}

    // ...since it will be destroyed when we leave.
    ~VSLArg()
    {
	if (_box)
	    _box->unlink();
    }

    // Assignment: just the same.
    void operator = (Box *box)
    {
	if (box != _box)
	{
	    if (_box)
		_box->unlink();
	    _box = box;
	}
    }

    void operator = (const VSLArg& arg)
    {
	if (arg.box() != _box)
	{
	    if (_box)
		_box->unlink();

	    _box = arg.box() ? arg.box()->link() : 0;
	}
    }
};


// Custom tagging function using VSLArgs
inline VSLArg tag(const VSLArg& arg, Data *data = 0, DataLink *link = 0)
{
    // Prevent double unlink: from Box::tag() and ~VSLArg()
    arg.box()->link();

    // Tag the box
    return arg.box()->tag(data, link);
}


// The VSL library
class VSLLib {
    friend class VSLDefList;  // for VSLDefList::replace()

public:
    DECLARE_TYPE_INFO

private:
    // Size of hash table (should be prime)
    const int hashSize = 4001;

    // Flags for optimizing (used internally)
    const unsigned _ResolveDefs             = (1 << 4);
    const unsigned _ResolveSynonyms         = (1 << 5);
    const unsigned _FoldOps                 = (1 << 6);
    const unsigned _FoldConsts              = (1 << 7);
    const unsigned _InlineFuncs             = (1 << 8);
    const unsigned _CountSelfReferences     = (1 << 9);
    const unsigned _Cleanup                 = (1 << 10);

public:
    // Public flags for optimizing
    const unsigned ResolveDefs          = _ResolveDefs;
    const unsigned ResolveSynonyms      = _ResolveDefs | _ResolveSynonyms;
    const unsigned FoldOps              = _ResolveSynonyms | _FoldOps;
    const unsigned FoldConsts           = _FoldConsts;
    const unsigned InlineFuncs          = _ResolveDefs | _InlineFuncs;
    const unsigned CountSelfReferences  = _CountSelfReferences;
    const unsigned Cleanup              = _CountSelfReferences | _Cleanup;

    // Mask for building #iterations
    const unsigned loopMask         = (1 << 0 | 1 << 1 | 1 << 2 | 1 << 3);
    const unsigned allOpts          = ((unsigned)-1) & ~loopMask;
    const unsigned stdOpt           = allOpts | 2;

private:
    string _lib_name;               // library name
    VSLDefList *defs[hashSize];     // hash table containing definitions
    VSLDef *_first;                 // linked list over definitions
    VSLDef *_last;                  // last def in list

    void initHash();

    // Optimizing and post-processing
    int bind();                     // bind internal functions to references
    int resolveNames();             // bind names to variables
    int compilePatterns();          // compile call patterns
    int resolveDefs();              // resolve unambiguous function calls
    int resolveSynonyms();          // resolve synonyms
    int foldOps();                  // fold associative operators
    int foldConsts();               // evaluate constants
    int inlineFuncs();              // perform function inlining
    int countSelfReferences();      // count references internal to functions
    int cleanup();                  // remove unreferenced functions

    // Front-end for optimizing functions
    int perform(VSLLibAction action, char *actionName);
    void process(unsigned mode = stdOpt);

    // Build function call with arglist as argument
    VSLNode *_call(const string& func_name, VSLNode *arglist);

    // Build function call with given args
    VSLNode *call(const string& func_name);
    VSLNode *call(const string& func_name, 
		  VSLNode *arg1);
    VSLNode *call(const string& func_name, 
		  VSLNode *arg1, 
		  VSLNode *arg2);
    VSLNode *call(const string& func_name, 
		  VSLNode *arg1, 
		  VSLNode *arg2, 
		  VSLNode *arg3);

    // same, but with char *
    VSLNode *call(char *func_name) 
    { 
	return call(string(func_name)); 
    }
    VSLNode *call(char *func_name, 
		  VSLNode *arg1)
    { 
	return call(string(func_name), arg1); 
    }
    VSLNode *call(char *func_name, 
		  VSLNode *arg1,
		  VSLNode *arg2)
    { 
	return call(string(func_name), arg1, arg2);
    }
    VSLNode *call(char *func_name, 
		  VSLNode *arg1,
		  VSLNode *arg2,
		  VSLNode *arg3)
    { 
	return call(string(func_name), arg1, arg2, arg3);
    }

    // Add a new function
    VSLDef *add(const string& func_name, 
		VSLNode *pattern, 
		VSLNode *expr = 0,
		bool global = false, 
		const string& filename = "builtin", 
		int lineno = 0);

    // Rename function
    int override(const string& func_name);

    // Remove definition
    int replace(const string& func_name);

    // Find function
    VSLDefList *deflist(const string& func_name) const;
    VSLDef *def(const string& func_name, Box *arg) const;

public:
    // Build
    VSLLib();
    VSLLib(const string& lib_name, unsigned optimize = stdOpt);
    VSLLib(istream& s, unsigned optimize = stdOpt);

    // Read
    VSLLib& read(const string& lib_name, unsigned optimize = stdOpt);
    VSLLib& read(istream& s, unsigned optimize = stdOpt);
    static int parse();

    // Check if function is present
    bool has(const string& func_name)
    {
	return deflist(func_name) != 0;
    }

    // Evaluate function with given argument list
    const Box *eval(const string& func_name, ListBox *arg) const;
    
    // Custom functions
    const Box *eval(const string& func_name, VSLArg args[]) const;
    const Box *eval(const string& func_name, 
		    VSLArg arg0 = (Box *)0,
		    VSLArg arg1 = (Box *)0,
		    VSLArg arg2 = (Box *)0,
		    VSLArg arg3 = (Box *)0,
		    VSLArg arg4 = (Box *)0,
		    VSLArg arg5 = (Box *)0,
		    VSLArg arg6 = (Box *)0,
		    VSLArg arg7 = (Box *)0,
		    VSLArg arg8 = (Box *)0,
		    VSLArg arg9 = (Box *)0) const;

    // Perform __output() function on arg if present
    void output(Box *&arg);

    // Destroy
    virtual ~VSLLib();

    // Message handling
    static void echo(const string& s);

    // Parsing messages
    static void parse_echo(const string& s);
    static void parse_error(const string& s);
    static void parse_warning(const string& s);

    // Evaluation messages
    static void eval_echo(const string& s, const VSLDef *def = 0);
    static void eval_error(const string& s, const VSLDef *def = 0);
    static void eval_warning(const string& s, const VSLDef *def = 0);

    // Debugging
    friend ostream& operator << (ostream& s, const VSLLib& lib);
    void dumpTree(ostream& s) const;
    VSLDef *lastdef()  { return _last; }
    VSLDef *firstdef() { return _first; }

    // Background processing
    static void (*background)();

    // Representation invariant
    virtual bool OK() const;
};

#endif
