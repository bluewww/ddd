// $Id$
// Vordefinierte VSL-Funktionen

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

char VSLBuiltin_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include <limits.h>
#include <math.h>
#include <iostream.h>

#include "bool.h"
#include "Range.h"
#include "VSLBuiltin.h"
#include "VSLLib.h"

// zillions of boxes...
#include "AlignBox.h"
#include "ArcBox.h"
#include "BinBox.h"
#include "DiagBox.h"
#include "FixBox.h"
#include "FontFixBox.h"
#include "PrimitiveB.h"
#include "SlopeBox.h"
#include "StringBox.h"
#include "TrueBox.h"
#include "DummyBox.h"


// Typ-Pruefungen

inline bool checkAtoms(ListBox *args)
// Prueft, ob alle Argumente Atome sind
{
    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	if (box->isListBox() && !box->isDummyBox())
	{
	    VSLLib::eval_error("invalid argument -- argument is list");
	    return false;
	}
    }

    return true;
}

#ifndef NDEBUG
#define CHECK_ATOMS(args) { if (!checkAtoms(args)) return 0; }
#else
#define CHECK_ATOMS(args)
#endif


inline bool checkSize(ListBox *args)
// Prueft, ob alle Argumente Groesse haben
{
    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	if (!box->size().isValid())
	    return false;
    }

    return true;
}

#define CHECK_SIZE(args) { if (!checkSize(args)) return new DummyBox; }


// Vordefinierte Funktionen VSL

// Logische Operatoren

static Box *not(ListBox *args)
// Logisches nicht
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    if ((*args)[0]->size(X) == 0)
	    return new TrueBox;

    return new FalseBox;
}


// Graphische Operatoren

static Box *halign(ListBox *args)
// Horizontale Anordnung
{
    CHECK_ATOMS(args);

    HAlignBox *ret = 0;     // Rueckgabewert

    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	if (ret == 0)
	    ret = new HAlignBox;
	*ret &= box;
    }

    // Wenn kein Sohn gefunden, NullBox zurueckgeben
    if (ret == 0)
	return new NullBox;

    // Wenn 1 Sohn gefunden, diesen zurueckgeben
    if (ret->nsons() == 1)
    {
	Box *son = (*ret)[0]->link();
	ret->unlink();
	return son;
    }

    // Sonst: Anordnung zurueckgeben
    return ret;
}

static Box *talign(ListBox *args)
// Textuelle Anordnung
{
    CHECK_ATOMS(args);

    TAlignBox *ret = 0;     // Rueckgabewert

    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	if (ret == 0)
	    ret = new TAlignBox;
	*ret &= box;
    }

    // Wenn kein Sohn gefunden, NullBox zurueckgeben
    if (ret == 0)
	return new NullBox;

    // Wenn 1 Sohn gefunden, diesen zurueckgeben
    if (ret->nsons() == 1)
    {
	Box *son = (*ret)[0]->link();
	ret->unlink();
	return son;
    }

    // Sonst: Anordnung zurueckgeben
    return ret;
}

static Box *valign(ListBox *args)
// Vertikale Anordnung
{
    CHECK_ATOMS(args);

    VAlignBox *ret = 0;

    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	if (ret == 0)
	    ret = new VAlignBox;
	*ret |= box;
    }

    // Wenn kein Sohn gefunden, NullBox zurueckgeben
    if (ret == 0)
	return new NullBox;

    // Wenn 1 Sohn gefunden, diesen zurueckgeben
    if (ret->nsons() == 1)
    {
	Box *son = (*ret)[0]->link();
	ret->unlink();
	return son;
    }

    // Sonst: Anordnung zurueckgeben
    return ret;
}

static Box *ualign(ListBox *args)
// Gestapelte Anordnung
{
    CHECK_ATOMS(args);

    UAlignBox *ret = 0;

    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	if (ret == 0)
	    ret = new UAlignBox;
	*ret ^= box;
    }

    // Wenn kein Sohn gefunden, NullBox zurueckgeben
    if (ret == 0)
	return new NullBox;

    // Wenn 1 Sohn gefunden, diesen zurueckgeben
    if (ret->nsons() == 1)
    {
	Box *son = (*ret)[0]->link();
	ret->unlink();
	return son;
    }

    // Sonst: Anordnung zurueckgeben
    return ret;
}



// Arithmetische Operatoren

static Box *plus(ListBox *args)
// Addition
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    BoxSize sum(0,0);
    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	sum += box->size();
    }

    return new SpaceBox(sum);
}

static Box *mult(ListBox *args)
// Multiplikation
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    BoxSize product(1,1);
    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();
	product *= box->size();
    }

    return new SpaceBox(product);
}

static Box *minus(ListBox *args)
// Subtraktion
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    return new SpaceBox(
	(*args)[0]->size() - (*args)[1]->size());
}

static Box *div(ListBox *args)
// (ganzzahlige) Division
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    if ((*args)[1]->size(X) == 0 || (*args)[1]->size(Y) == 0)
    {
	VSLLib::eval_error("division by zero");
	return 0;
    }

    return new SpaceBox(
	(*args)[0]->size() / (*args)[1]->size());
}

static Box *mod(ListBox *args)
// Rest Division
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    if ((*args)[1]->size(X) == 0 || (*args)[1]->size(Y) == 0)
    {
	VSLLib::eval_error("division by zero");
	return 0;
    }

    return new SpaceBox(
	(*args)[0]->size() % (*args)[1]->size());
}



// Vergleichsoperatoren

static Box *eq(ListBox *args)
// Test auf Gleichheit
{
    if (*(*args)[0] == *(*args)[1])
	return new TrueBox;
    else
	return new FalseBox;
}

static Box *ne(ListBox *args)
// Test auf Ungleichheit
{
    if (*(*args)[0] != *(*args)[1])
	return new TrueBox;
    else
	return new FalseBox;
}

static Box *gt(ListBox *args)
// Test >
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    if ((*args)[0]->size() > (*args)[1]->size())
	return new TrueBox;
    else
	return new FalseBox;
}

static Box *ge(ListBox *args)
// Test >=
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    if ((*args)[0]->size() >= (*args)[1]->size())
	return new TrueBox;
    else
	return new FalseBox;
}

static Box *lt(ListBox *args)
// Test <
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    if ((*args)[0]->size() < (*args)[1]->size())
	return new TrueBox;
    else
	return new FalseBox;
}

static Box *le(ListBox *args)
// Test <=
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    if ((*args)[0]->size() <= (*args)[1]->size())
	return new TrueBox;
    else
	return new FalseBox;
}


// Listen-Operatoren

static Box *cons(ListBox *args)
// Listen verketten
{
    ListBox *ret = 0;

    for (ListBox *b = args; !b->isEmpty(); b = b->tail())
    {
	Box *box = b->head();

	if (!box->isListBox())
	{
	    VSLLib::eval_error("invalid argument -- argument is list");
	    if (ret)
		ret->unlink();
	    return 0;
	}

	if (!((ListBox *)box)->isEmpty())
	{
	    // Anzuhaengende Liste erzeugen: 
	    // Wenn box letztes Argument, genuegt link
	    ListBox *box2;
	    if (b->tail()->isEmpty())
		box2 = (ListBox *)box->link();
	    else
		box2 = (ListBox *)box->dup();

	    // Liste anhaengen: 
	    // Wenn box erstes Argument, box kopieren
	    if (ret == 0)
		ret = box2;
	    else
	    {
		ret->cons(box2);
		box2->unlink();
	    }
	}
    }

    // Wenn keine Argumente: [] zurueckgeben
    if (ret == 0)
	ret = new ListBox;

    return ret;
}


// Standard-Funktionen

static Box *hspace(ListBox *args)
// hspace(box)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    const Box *child = (*args)[0];
    return new SpaceBox(BoxSize(child->size(X), 0));
}

static Box *vspace(ListBox *args)
// vspace(box)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    const Box *child = (*args)[0];
    return new SpaceBox(BoxSize(0, child->size(Y)));
}

static Box *hfix(ListBox *args)
// hfix(box)
{
    CHECK_ATOMS(args);

    return new HFixBox((Box *)(*args)[0]);
}

static Box *vfix(ListBox *args)
// vfix(box)
{
    CHECK_ATOMS(args);

    return new VFixBox((Box *)(*args)[0]);
}

static Box *bin(ListBox *args)
// bin(box)
{
    CHECK_ATOMS(args);

    return new BinBox((Box *)(*args)[0]);
}


// Name Knotentyp im Dokument zurueckgeben

static Box *tag(ListBox *args)
// tag(box)
{
    return new StringBox((*args)[0]->name());
}


// Zeichenkette aus Kasten extrahieren

static Box *str(ListBox *args)
// str(box)
{
    return new StringBox((*args)[0]->str());
}


// Font setzen

static Box *font(ListBox *args)
// font(box, font)
{
    // Erstes Argument kopieren, dann dessen Font umstellen.

    Box *ret = ((Box *)(*args)[0])->dup();
    ret->newFont((*args)[1]->str());

    return ret;
}

static Box *fontfix(ListBox *args)
// fontfix(box)
{
    return new FontFixBox((Box *)(*args)[0]);
}


// Standard-Boxen

static Box *rise(ListBox *args)
// rise(linethickness)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    return new RiseBox((*args)[0]->size(X));
}

static Box *fall(ListBox *args)
// fall(linethickness)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    return new FallBox((*args)[0]->size(X));
}

static Box *arc(ListBox *args)
// arc(start, length, linethickness)
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    return new ArcBox((*args)[0]->size(X),
	(*args)[1]->size(X),
	(*args)[2]->size(X));
}

static Box *square(ListBox *args)
// Quadratische Box aus maximaler Hoehe und Breite
{
    CHECK_ATOMS(args);
    CHECK_SIZE(args);

    Box *arg = (Box *)(*args)[0];
    return new SquareBox(max(arg->size(X), arg->size(Y)));
}

static Box *fill(ListBox *)
// fill()
{
    return new FillBox;
}

static Box *rule(ListBox *)
// rule()
{
    return new RuleBox;
}

static Box *diag(ListBox *)
// diag()
{
    return new DiagBox;
}


// Default-Boxen

static Box *undef(ListBox *)
// Platzhalter fuer undefinierte Box
{
    return new StringBox("???");
}


// Fehlerbehandlung

static Box *fail(ListBox *args)
// Berechnung scheitern lassen;
// Fehlermeldung (sofern vorhanden) ausgeben
{
    CHECK_ATOMS(args);

    if ((*args)[0])
	VSLLib::eval_error((*args)[0]->str());
    else
	VSLLib::eval_error("evaluation failed");

    return 0;
}


// Tabelle der vordefinierten Funktionen

struct BuiltinRec {
    char* ext_name;         // Funktionsname (extern; 0 = func_name)
    char* func_name;        // Funktionsname (intern)
    char isAssoc;           // Flag: Assoziativ?
    char hasSideEffects;    // Flag: Seiteneffekte?
    char isInfix;           // Flag: Infix ausgeben?
    BuiltinFunc eval_func;  // Aufzurufende Funktion

};

static BuiltinRec builtins[] = {

// n-stellige Operatoren
{ "(&)",    "__op_halign",  true,   false,  false,  halign },
{ "(|)",    "__op_valign",  true,   false,  false,  valign },
{ "(^)",    "__op_ualign",  true,   false,  false,  ualign },
{ "(~)",    "__op_talign",  true,   false,  false,  talign },
{ "(+)",    "__op_plus",    true,   false,  false,  plus },
{ "(*)",    "__op_mult",    true,   false,  false,  mult },
{ "(::)",   "__op_cons",    true,   false,  false,  cons },

// 2-stellige Operatoren
{ "(-)",    "__op_minus",   false,  false,  false,  minus },
{ "(/)",    "__op_div",     false,  false,  false,  div },
{ "(%)",    "__op_mod",     false,  false,  false,  mod },
{ "(=)",    "__op_eq",      false,  false,  false,  eq },
{ "(<>)",   "__op_ne",      false,  false,  false,  ne },
{ "(>)",    "__op_gt",      false,  false,  false,  gt },
{ "(>=)",   "__op_ge",      false,  false,  false,  ge },
{ "(<)",    "__op_lt",      false,  false,  false,  lt },
{ "(<=)",   "__op_le",      false,  false,  false,  le },
{ 0,        "__op_bin",     false,  false,  false,  bin },

// 1-stellige Operatoren
{ "not",    "__op_not",     false,  false,  false,  not },

// Funktionen
{ 0,    "__hspace",     false,  false,  false,  hspace },
{ 0,    "__vspace",     false,  false,  false,  vspace },
{ 0,    "__hfix",       false,  false,  false,  hfix },
{ 0,    "__vfix",       false,  false,  false,  vfix },
{ 0,    "__rise",       false,  false,  false,  rise },
{ 0,    "__fall",       false,  false,  false,  fall },
{ 0,    "__arc",        false,  false,  false,  arc },
{ 0,    "__square",     false,  false,  false,  square },
{ 0,    "__tag",        false,  false,  false,  tag },
{ 0,    "__string",     false,  false,  false,  str },
{ 0,    "__font",       false,  false,  false,  font },
{ 0,    "__fontfix",    false,  false,  false,  fontfix },

// Funktionen mit Seiteneffekten
{ 0,    "__fail",       false,  true,   false,  fail },

// Konstanten
{ 0,    "__fill",       false,  false,  false,  fill },
{ 0,    "__rule",       false,  false,  false,  rule },
{ 0,    "__diag",       false,  false,  false,  diag },
{ 0,    "__undef",      false,  false,  false,  undef },

};


// Zugriffsfunktionen

int VSLBuiltin::resolve(const string& func_nm)
// fuer gegebenen Namen Funktionszeiger zurueckgeben;
// wenn nicht gefunden, 0 zurueckgeben.
{
    for (int i = 0; i < int(sizeof(builtins) / sizeof(builtins[0])); i++)
	if (func_nm == builtins[i].func_name)
	    return i;

    // nicht gefunden
    return -1;
}

BuiltinFunc VSLBuiltin::func(int idx)
{
    assert (idx >= 0 && idx < int(sizeof(builtins) / sizeof(builtins[0])));
    return builtins[idx].eval_func;
}

bool VSLBuiltin::isAssoc(int idx)
{
    assert (idx >= 0 && idx < int(sizeof(builtins) / sizeof(builtins[0])));
    return builtins[idx].isAssoc;
}

bool VSLBuiltin::hasSideEffects(int idx)
{
    assert (idx >= 0 && idx < int(sizeof(builtins) / sizeof(builtins[0])));
    return builtins[idx].hasSideEffects;
}

bool VSLBuiltin::isInfix(int idx)
{
    assert (idx >= 0 && idx < int(sizeof(builtins) / sizeof(builtins[0])));
    return builtins[idx].isInfix;
}

char *VSLBuiltin::func_name(int idx)
{
    assert (idx >= 0 && idx < int(sizeof(builtins) / sizeof(builtins[0])));
    return builtins[idx].func_name;
}

char *VSLBuiltin::ext_name(int idx)
{
    assert (idx >= 0 && idx < int(sizeof(builtins) / sizeof(builtins[0])));
    char *s = builtins[idx].ext_name;
    return s != 0 ? s : builtins[idx].func_name;
}
