// $Id$
// Implementation Klasse CallNode

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
// Revision 1.1  1995/05/01 15:47:34  zeller
// Initial revision
//
// Revision 10.13  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.12  1994/07/21  08:19:13  zeller
// Even more functions declared `const'
//
// Revision 10.11  1994/07/20  09:04:07  zeller
// New: pattern compiling is const
//
// Revision 10.10  1994/07/19  20:30:11  zeller
// Changes for gcc-2.6.0 (and ANSI C++)
//
// Revision 10.9  1993/08/27  15:37:52  zeller
// Neu: TypeInfo
//
// Revision 10.8  1993/05/22  20:16:40  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.7  1993/04/22  11:07:46  zeller
// Lizenz verbessert
// 
// Revision 10.6  1993/04/21  00:27:51  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.5  1993/04/20  22:06:16  zeller
// removeSelfReferences() neu ueberarbeitet -- laeuft jetzt
// 
// Revision 10.4  1993/04/16  11:44:22  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.3  1992/11/03  19:37:10  zeller
// Tabstops jetzt 8 statt 4
// 
// Revision 10.2  1992/06/01  16:51:14  zeller
// Neu: #pragma implementation
// 
// Revision 10.1  1992/02/18  11:19:43  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  05:56:34  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:01:59  zeller
// Installierte Version
// 
// Revision 4.2  1991/07/02  14:37:30  zeller
// Neu: _dumpTree()
// 
// Revision 4.1  1991/05/25  12:52:53  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 2.24  1991/05/24  19:35:12  zeller
// Quantoren eingefuehrt
// 
// Revision 2.23  1991/05/24  12:02:53  zeller
// reBase -> _reBase, resolveNames -> _resolveNames umbenannt
// 
// Revision 2.22  1991/05/24  11:03:46  zeller
// reBase() eingefuehrt (fuer inlining von LET-Konstrukten)
// 
// Revision 2.21  1991/05/24  10:09:11  zeller
// instantiateArgs, countArgNodes erweitert (fuer LET-Inlining)
// 
// Revision 2.20  1991/05/23  18:53:47  zeller
// Neues Namenskonzept (schon wieder):
// Namen werden erst aufgeloest, wenn gesamter Ausdruck eingelesen.
// 
// Revision 2.19  1991/05/23  11:01:26  zeller
// Neu: Vor dem Uebersetzen der Pattern zunaechst alte Pattern loeschen
// 
// Revision 2.18  1991/05/22  11:28:15  zeller
// Neues Namenskonzept: erst alle Namen einlesen
// und erst spaeter durch ArgNode's ersetzen
// 
// Revision 2.17  1991/05/21  21:40:33  zeller
// Anpassungen fuer LetNode; nargs() neu definiert
// 
// Revision 2.16  1991/05/09  16:05:40  zeller
// Warnungen vermieden
// 
// Revision 2.15  1991/05/03  13:07:00  zeller
// Neu: Wenn Seiteneffekte verboten sind und es tritt einer auf,
// scheitert die Auswertung automatisch.
// 
// Revision 2.14  1991/04/30  12:23:25  zeller
// Fall "Konstante kann nicht ausgewertet werden" behandelt
// 
// Revision 2.13  1991/04/27  19:16:40  zeller
// resolveDefs(): Warnung "unendliche Rekursion" nur noch oberhalb
// eines TestNode's (d.h. weder im then- noch im else-Teil).
// 
// Revision 2.12  1991/04/27  17:29:36  zeller
// countInstances() in countArgNodes() umbenannt
// 
// Revision 2.11  1991/03/01  19:08:54  zeller
// Neu: countInstances()
// 
// Revision 2.10  1991/03/01  18:13:14  zeller
// Neu: instantiateArgs()
// 
// Revision 2.9  1991/03/01  16:56:24  zeller
// Optimierungsfunktionen von 'VSLOptim' hierher eingefuegt;
// Optimierungs-Funktionen mit Zusatz-Parameter: aktuelle Definition
// 
// Revision 2.8  1991/02/27  19:07:49  zeller
// Neu: CallNode::matchesAll
// 
// Revision 2.7  1991/02/27  04:24:35  zeller
// Formatierung verbessert
// 
// Revision 2.6  1991/02/25  16:23:19  zeller
// NoNode, NoBox, NoExpr entfernt;
// Neues Konzept leere Listen eingefuehrt
// 
// Revision 2.5  1991/02/24  19:04:04  zeller
// Neue Pruefungen 'NoBox' eingefuehrt
// 
// Revision 2.4  1991/02/24  17:16:12  zeller
// Fehler Freigeben Argumentliste beseitigt
// 
// Revision 2.3  1991/02/20  18:52:23  zeller
// dump() neu definiert;
// Pattern und Argumente nicht mehr auf Listen beschraenkt;
// Neues Typenkonzept eingefuehrt
// 
// Revision 2.2  1991/02/20  13:15:18  zeller
// Argument CallNode ist jetzt VSLNode
// 
// Revision 2.1  1991/02/20  11:59:57  zeller
// Initial revision
// 

char CallNode_rcsid[] = "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include "VSLLib.h"

#include "VSLNode.h"
#include "CallNode.h"
#include "VSEFlags.h"
#include "ConstNode.h"
#include "VSLDef.h"

DEFINE_TYPE_INFO_1(CallNode, VSLNode)

// CallNode

bool CallNode::matchesAll = false;


// Funktion ausgeben
void CallNode::dump(ostream& s) const
{
    s << func_name();

    if (VSEFlags::include_list_info)
	s << *_arg;
    else
	if (_arg->isArgNode())
	    s << "(" << *_arg << "...)";
	else
	    s << *_arg;
}

// ...in Baum-Notation
void CallNode::_dumpTree(ostream& s) const
{
    s << "\"" << func_name() << "\", ";
    _arg->dumpTree(s);
}

// Funktion auswerten
const Box *CallNode::_eval(ListBox *arglist) const
{
    // Argument auswerten
    const Box *myarg = _arg->eval(arglist);
    if (myarg == 0)
	return 0;

    // Funktion aufrufen
    const Box *box = call((Box *)myarg);

    ((Box *)myarg)->unlink();

    return box;
}



// Optimierung

// im Allgemeinen: Funktion auf Argumenten ausfuehren
int CallNode::resolveDefs(VSLDef *cdef, bool complain_recursive)
{
    return arg()->resolveDefs(cdef, complain_recursive);
}

int CallNode::resolveSynonyms(VSLDef *cdef, VSLNode **node)
{
    assert (this == *node); return arg()->resolveSynonyms(cdef, &arg());
}

int CallNode::foldOps(VSLDef *cdef, VSLNode **node)
{
    assert (this == *node); return arg()->foldOps(cdef, &arg());
}

int CallNode::inlineFuncs(VSLDef *cdef, VSLNode **node)
{
    assert (this == *node); return arg()->inlineFuncs(cdef, &arg());
}

int CallNode::instantiateArgs(VSLDef *cdef, VSLNode **node, VSLNode *values[],
    unsigned base, unsigned n)
{
    assert(this == *node); 
    return arg()->instantiateArgs(cdef, &arg(), values, base, n);
}

void CallNode::countArgNodes(VSLDef *cdef, int instances[],
    unsigned base, unsigned n)
{
    arg()->countArgNodes(cdef, instances, base, n);
}

void CallNode::compilePatterns(VSLDef *cdef) const
{
    arg()->compilePatterns(cdef);
}

void CallNode::uncompilePatterns(VSLDef *cdef) const
{
    arg()->uncompilePatterns(cdef);
}

int CallNode::countSelfReferences(VSLDef *cdef, VSLDefList *deflist)
{
    return arg()->countSelfReferences(cdef, deflist);
}

int CallNode::resolveName(VSLDef *cdef, VSLNode **node, string& name,
    unsigned id)
{
    assert(this == *node); return arg()->resolveName(cdef, &arg(), name, id);
}

int CallNode::_resolveNames(VSLDef *cdef, unsigned base)
{
    return arg()->resolveNames(cdef, base);
}

int CallNode::_reBase(VSLDef *cdef, unsigned newBase)
{
    return arg()->reBase(cdef, newBase);
}

string CallNode::firstName() const
{
    return arg()->firstName();
}


// Quantoren
bool CallNode::exists(VSLNodeFunc func)
{
    return VSLNode::exists(func) || arg()->exists(func);
}

bool CallNode::forAll(VSLNodeFunc func)
{
    return VSLNode::forAll(func) && arg()->forAll(func);
}


// Konstanten zusammenziehen
int CallNode::foldConsts(VSLDef *cdef, VSLNode **node)
{
    assert (this == *node);
    int changes = 0;

    // Auf allen Argumenten ausfuehren
    changes += arg()->foldConsts(cdef, &arg());

    // Pruefen, ob Ausdruck jetzt konstant;
    // wenn nicht, Abbruch
    if (!isConst())
	return changes;

    // Sonst: Funktion auswerten (hierbei Seiteneffekte vermeiden)

    sideEffectsProhibited = true;
    sideEffectsOccured = false;

    const Box *result = eval(0);

    if (result)
    {
	assert(!sideEffectsOccured);

	// CallNode durch Konstante ersetzen
	if (VSEFlags::show_optimize)
	{
	    cout << "\n" << cdef->longname() << ": foldConsts: replacing\n" 
		<< *this << '\n';
	    cout.flush();
	}

	// CallNode durch Konstante ersetzen
	*node = new ConstNode((Box *)result);

	if (VSEFlags::show_optimize)
	{
	    cout << "by " << **node << "\n";
	    cout.flush();
	}
	
	changes++;

	// CallNode und daranhaengenden Teilbaum loeschen
	delete this;
    }

    sideEffectsProhibited = false;

    return changes;
}



// Debugging

// Pruefen, ob alles in Ordnung
bool CallNode::OK() const
{
      assert (VSLNode::OK());
      assert (_arg && _arg->OK());

      return true;
}
