// $Id$
// Implementation Klasse VSLLib

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

char VSLLib_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif


#include "assert.h"
#include "hash.h"

#include "VSEFlags.h"

#include "VSLLib.h"
#include "VSLDef.h"
#include "VSLDefList.h"
#include "VSLBuiltin.h"

#include "VSLNode.h"
#include "DefCallN.h"
#include "BuiltinCN.h"
#include "ArgNode.h"
#include "ListNode.h"
#include "TrueNode.h"
#include "StringBox.h"

DEFINE_TYPE_INFO_0(VSLLib)

// VSLLib

// Daten

void (*VSLLib::background)() = 0;   // Hintergrund-Prozedur

#define BACKGROUND() { if (background) background(); }


// Initialisierung

// Hashtabelle initialisieren
void VSLLib::initHash()
{
    for (unsigned i = 0; i < hashSize; i++)
	defs[i] = 0;
    _first = 0;
    _last  = 0;
}

// VSL-Bibliothek initialisieren
VSLLib::VSLLib()
{
    _lib_name = "";
    initHash();
}


// VSL-Bibliothek initialisieren und einlesen
VSLLib::VSLLib(const string& lib_name, unsigned optimize)
{
    _lib_name = lib_name;
    initHash();
    read(lib_name, optimize);
}


// VSL-Bibliothek initialisieren und einlesen
VSLLib::VSLLib(istream& i, unsigned optimize)
{
    _lib_name = "";
    initHash();
    read(i, optimize);
}


// Funktionsliste fuer gegebenen Namen zurueckgeben;
// wenn Name undefiniert, 0
VSLDefList* VSLLib::deflist(const string& func_name) const
{
    unsigned hashcode = hashpjw(func_name) % hashSize;

    // Definitionsliste mit passendem Namen suchen
    for (VSLDefList *d = defs[hashcode];
	d != 0 && d->func_name() != func_name; d = d->next())
	;

    return d;
}


// Definition zu Bibliothek hinzufuegen
VSLDef *VSLLib::add(const string& func_name,
    VSLNode *pattern, VSLNode *expr, 
    bool global, const string& filename, int lineno)
{
    BACKGROUND();

    // DefList zu Funktionsnamen suchen
    VSLDefList *d = deflist(func_name);
    
    if (d == 0)
    {
	// Nicht gefunden? Dann neue Definitionsliste einfuegen
	unsigned hashcode = hashpjw(func_name) % hashSize;

	d = defs[hashcode];
	defs[hashcode] = new VSLDefList(this, hashcode, func_name, global);
	defs[hashcode]->next() = d;

	// und zurueckgeben
	d = defs[hashcode];
    }

    // Definition an DefList anhaengen
    bool newFlag;
    VSLDef* newdef = d->add(newFlag, pattern, expr, filename, lineno);

    if (newFlag)
    {
	// Definition in globale Liste einhaengen
	if (_last == 0)
	    _first = newdef;
	else
	{
	    _last->libnext() = newdef;
	    newdef->libprev() = _last;
	}
	_last = newdef;
    }

    // Sichtbarkeit anpassen:
    // Die Sichtbarkeit der Funktion ist global, 
    // wenn irgendeine Definition global war.
    d->global() = d->global() || global;

    return newdef;
}


// Funktionsnamen umbenennen
int VSLLib::override(const string& func_name)
{
    VSLDefList *d = deflist(func_name);

    if (d == 0)
	return -1;

    d->override();
    return 0;
}

// Funktionsdefinitionen loeschen
int VSLLib::replace(const string& func_name)
{
    VSLDefList *d = deflist(func_name);

    if (d == 0)
	return -1;

    d->replace();
    return 0;
}

    



// Definition zu gegebenem Funktionsnamen und Argument zurueckgeben
VSLDef *VSLLib::def(const string& func_name, Box *a) const
{
    VSLDefList *d = deflist(func_name);
    return d ? d->def(a) : 0;
}


// Evaluate call
const Box *VSLLib::eval(const string& func_name, ListBox *a) const
{
    VSLDefList *d = deflist(func_name);
    if (d == 0)
	eval_error(string("'") + func_name + "(...)' undefined");

    return d ? d->eval(a) : 0;
}

// Custom functions
const Box *VSLLib::eval(const string& func_name, VSLArg args[]) const
{
    ListBox *arg = new ListBox;
    for (int i = 0; args[i].box() != 0; i++)
	*arg += args[i].box();
    
    const Box *result = eval(func_name, arg);
    arg->unlink();

    return result;
}

const Box *VSLLib::eval(const string& func_name,
			VSLArg arg0,
			VSLArg arg1,
			VSLArg arg2,
			VSLArg arg3,
			VSLArg arg4,
			VSLArg arg5,
			VSLArg arg6,
			VSLArg arg7,
			VSLArg arg8,
			VSLArg arg9) const
{
    VSLArg args[11];

    args[0] = arg0;
    args[1] = arg1;
    args[2] = arg2;
    args[3] = arg3;
    args[4] = arg4;
    args[5] = arg5;
    args[6] = arg6;
    args[7] = arg7;
    args[8] = arg8;
    args[9] = arg9;
    args[10] = (Box *)0;

    return eval(func_name, args);
}

// Ausgewerteten Ausdruck fuer Ausgabe vorbereiten
void VSLLib::output(Box *&a)
{
    if (a == 0)
	return;

    VSLDefList *d = deflist("__output");
    if (d == 0)
	return;

    ListBox *args = new ListBox(a, new ListBox);
    a->unlink();
    a = (Box *)d->eval(args);
    args->unlink();
}
    

// VSLLib zerstoeren
VSLLib::~VSLLib()
{
    for (unsigned i = 0; i < hashSize; i++)
	if (defs[i]) delete defs[i];
}


// Binder
// Definitionen der eingebauten Funktionen hinzufuegen
// Immer noch undefinierte Funktionen mit Default-Wert vorbelegen
int VSLLib::bind()
{
    int changes = 0;
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
	if (cdef->expr() == 0)
	{
	    BACKGROUND();

	    // Deklaration ohne Definition? 
	    // Dann passende eingebaute Funktion suchen...
	    int idx = VSLBuiltin::resolve(cdef->deflist->func_name());

	    if (idx < 0)
	    {
		eval_error("function declared, but not defined", cdef);
		
		// Fuer fehlende Definition Aufruf "__undef()" einsetzen
		cdef->expr() = call("__undef");
		if (cdef->expr() == 0)
		    cdef->expr() = new ConstNode(new StringBox("???"));
	    }
	    else
	    {
		// Alle der Deklaration uebergebenen Argumente
		// auch der eingebauten Funktion uebergeben
		cdef->expr() = new BuiltinCallNode(idx, 
		    cdef->node_pattern()->dup());
	    }

	    changes++;
	}

    return changes;
}


// Namen an Variablen binden
int VSLLib::resolveNames()
{
    int changes = 0;
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	changes += cdef->resolveNames();
	if (VSEFlags::assert_library_ok)
	    assert(OK());
    }

    return changes;
}


// Pattern aller Funktionen (neu) erzeugen
int VSLLib::compilePatterns()
{
    int changes = 0;

    // Zunaechst alle Pattern loeschen.
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	BACKGROUND();

	// Funktions-Pattern
	cdef->uncompilePattern();
	if (VSEFlags::assert_library_ok)
	    assert(OK());

	// LET- und WHERE-Pattern
	cdef->expr()->uncompilePatterns(cdef);
	if (VSEFlags::assert_library_ok)
	    assert(OK());
    }

    // Jetzt alle Pattern neu erzeugen.
    for (cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	BACKGROUND();

	// Funktions-Pattern
	cdef->compilePattern();
	if (VSEFlags::assert_library_ok)
	    assert(OK());

	// LET- und WHERE-Pattern
	cdef->expr()->compilePatterns(cdef);
	if (VSEFlags::assert_library_ok)
	    assert(OK());

	changes++;
    }

    return changes;
}


// Funktionsaufrufe eindeutig machen
int VSLLib::resolveDefs()
{
    int changes = 0;
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	BACKGROUND();

	changes += cdef->expr()->resolveDefs(cdef);
	if (VSEFlags::assert_library_ok)
	    assert(OK());
    }

    return changes;
}

// Synonyme aufloesen
int VSLLib::resolveSynonyms()
{
    int changes = 0;
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	BACKGROUND();

	changes += cdef->expr()->resolveSynonyms(cdef, &cdef->expr());
	if (VSEFlags::assert_library_ok)
	    assert(OK());
    }

    return changes;
}

// Grosse Operanden machen
int VSLLib::foldOps()
{
    int changes = 0;
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	BACKGROUND();

	changes += cdef->expr()->foldOps(cdef, &cdef->expr());
	if (VSEFlags::assert_library_ok)
	    assert(OK());
    }

    return changes;
}


// Konstanten bilden
int VSLLib::foldConsts()
{
    int changes = 0;
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
	if (VSEFlags::optimize_globals || !cdef->deflist->global())
	{
	    BACKGROUND();

	    changes += cdef->expr()->foldConsts(cdef, &cdef->expr());
	    if (VSEFlags::assert_library_ok)
		assert(OK());
	}

    return changes;
}


// Funktionskoerper einbinden
int VSLLib::inlineFuncs()
{
    int changes = 0;
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	BACKGROUND();

	changes += cdef->expr()->inlineFuncs(cdef, &cdef->expr());
	if (VSEFlags::assert_library_ok)
	    assert(OK());
    }

    return changes;
}

// Interne Referenzen bestimmen
int VSLLib::countSelfReferences()
{
    int changes = 0;

    // Zunaechst alle self_references auf 0 setzen
    for (VSLDef *cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	VSLDefList *dflist = cdef->deflist;
	dflist->self_references = 0;
    }

    // Jetzt self_references hochzaehlen
    for (cdef = _first; cdef != 0; cdef = cdef->libnext())
    {
	BACKGROUND();

	changes += cdef->expr()->countSelfReferences(cdef, cdef->deflist);
	if (VSEFlags::assert_library_ok)
	    assert(OK());
    }

    return changes;
}

// Lokale unbenutzte Funktionen entfernen
int VSLLib::cleanup()
{
    int changes = 0;

    VSLDef *d = _last;
    while (d != 0)
    {
	BACKGROUND();

	VSLDefList *dflist = d->deflist;
	if (!dflist->global() && 
	    (dflist->references == 0 || 
	     dflist->references == dflist->self_references))
	{   
	    // Funktion ist ausserhalb ihrer Definition unbenutzt: loeschen.

	    // 'd' verschieben, bis nicht mehr auf dflist
	    while (d != 0 && d->deflist == dflist)
		d = d->libprev();

	    if (VSEFlags::show_optimize)
	    {
		for (VSLDef *e = dflist->first(); e != 0; e = e->listnext())
		    cout << "\n" << e->longname() << ": removed";
		cout.flush();
	    }

	    // Zeiger vom Vorgaenger suchen und auf Nachfolger setzen.
	    if (defs[dflist->hashcode] == dflist)
		defs[dflist->hashcode] = dflist->next();
	    else
	    {
		for (VSLDefList *prev = defs[dflist->hashcode];
		    prev != 0 && prev->next() != dflist; prev = prev->next())
		    ;
		assert (prev->next() == dflist);
		prev->next() = dflist->next();
	    }

	    // VSLDef's loeschen
	    dflist->replace();

	    // VSLDeflist loeschen.
	    dflist->next() = 0; delete dflist;
	    changes++;
	}
	else
	{
	    // Naechste Definition untersuchen
	    d = d->libprev();
	}

	if (VSEFlags::assert_library_ok)
	    assert(OK());
    }

    return changes;
}


// Aktion auf Bibliothek ausfuehren
int VSLLib::perform(VSLLibAction action, char *name)
{
    if (VSEFlags::verbose)
    {
	cout << ", " << name;
	cout.flush();
    }
    int changes = action();

    if (VSEFlags::verbose)
    {
	cout << "(" << changes << ")";
	cout.flush();
    }

    if (VSEFlags::show_optimize && VSEFlags::dump_library)
	cout << "\n\nAfter " << name << ":\n\n" << *this << "\n\n";

    return changes;
}

#define PERFORM(x) perform(x, #x)


// Steuerprogramm
void VSLLib::process(unsigned mode)
{
    /* 
	Die hier gewaehlte Reihenfolge der Optimierungen ist:

	1. resolveDefs
	2. resolveSynonyms
	3. foldOps     <--------+
	4. foldConsts           | (*)
	5. inlineFuncs ---------+
	6. countSelfReferences

	( (*): bis keine Aenderungen mehr aufgetreten)

	Je zwischen zwei Optimierungsschritten werden unbenutzte
	Funktionen entfernt (cleanup), um die Optimierung zu beschleunigen.

	Die Reihenfolge der Optimierungsschritte ist im Prinzip
	beliebig. Tatsaechlich gelten aber folgende Einschraenkungen:

	* resolveDefs ist Vorbedingung fuer inlineFuncs und resolveSynonyms.

	* resolveSynonyms erledigt eine Teilmenge von inlineFuncs,
	  dies aber viel schneller und weniger platzintensiv,
	  von daher sollte es von inlineFuncs aufgerufen werden;

	* foldOps und foldConsts koennen ihre
	  Wirkung am besten auf grossen, komplexen Ausdruecken
	  zeigen, wie sie von inlineFuncs geliefert werden;

	* foldConsts ist sehr zeitaufwendig und sollte daher
	  am besten von allen vorangegangenen Optimierungen
	  profitieren;

	Aus diesen Einschraenkungen ergibt sich die obige Reihenfolge.

	Stehen einzelne Optimierungen zur Auswahl, benoetigen
	resolveSynonyms und foldOps wenig Zeit;
	die Bibliothek wird geringfuegig kleiner und die 
	Auswertung geringfuegig beschleunigt.

	foldConsts benoetigt viel Zeit; die Bibliothek wird
	u.U. erheblich kleiner (Konstanten werden ge'shared)
	und die Zeit wird erheblich beschleunigt.

	inlineFuncs benoetigt ebenfalls viel Zeit; die
	Bibliothek wird erheblich groesser und die Zeit
	wird ein wenig beschleunigt.
    */

    PERFORM(bind);
    PERFORM(resolveNames);
    PERFORM(compilePatterns);

    if (mode & _Cleanup)
	PERFORM(cleanup);

    if (mode & _ResolveDefs)
	if (PERFORM(resolveDefs) > 0)
	    if (mode & _Cleanup)
		PERFORM(cleanup);

    if (mode & _ResolveSynonyms)
	if (PERFORM(resolveSynonyms) > 0)
	    if (mode & _Cleanup)
		PERFORM(cleanup);
	
    for (unsigned loop = 0; loop < (mode & loopMask); loop++)
    {
	int sum = 0;
	int changes;

	if (mode & _FoldOps)
	    if ((changes = PERFORM(foldOps)) > 0)
	    {
		sum += changes;
		PERFORM(compilePatterns);
	    }

	if (mode & _FoldConsts)
	    if ((changes = PERFORM(foldConsts)) > 0)
	    {
		sum += changes;
		if (mode & _Cleanup)
		    PERFORM(cleanup);
	    }

	if (mode & _InlineFuncs)
	    if ((changes = PERFORM(inlineFuncs)) > 0)
	    {
		sum += changes;
		if (mode & _Cleanup)
		    PERFORM(cleanup);
	    }

	if (sum == 0)
	    break;      // Keine Veraenderungen mehr aufgetreten
    }

    if (mode & _CountSelfReferences)
	if (PERFORM(countSelfReferences) > 0)
	    if (mode & _Cleanup)
		PERFORM(cleanup);
}




// Debugging

// VSLLib ausgeben
ostream& operator << (ostream& s, const VSLLib& lib)
{
    s << "// " << lib._lib_name << "\n\n";

    VSLDef *d = lib._first;
    if (VSEFlags::dump_last)
	d = lib._last;

    for (; d != 0; d = d->libnext())
    {
	// Kommentar ausgeben
	s << "// " << d->longname() << " (hashcode: "
	    << d->deflist->hashcode << ") ";
	    
	if (d->deflist->references == 0)
	    s << "(unused) ";
	else
	    s << "(used " << d->deflist->references << " times) ";

	if (d->straight())
	    s << "(straight)\n";
	else
	    s << "(not straight)\n";

	// Funktionsnamen (mit Argumenten) ausgeben
	s << d->f_name();

	// Wenn nur Deklaration, jetzt nichts mehr ausgeben
	if (d->expr() != 0)
	{
	    // Pfeil ausgeben
	    if (d->deflist->global())
		s << " -> ";
	    else
		s << " = ";
	    s.flush();

	    // Definition ausgeben
	    s << *(d->expr());
	}

	s << ";\n\n"; 
	s.flush();
    }

    return s;
}


// ...als Baum
void VSLLib::dumpTree(ostream& s) const
{
    bool old = StringBox::quoted;
    StringBox::quoted = true;

    s << "#!vsl\n\n// " << _lib_name << "\n\n";
    s << "#include <vsl.vsl>\n\n";

    s << "main(_...) -> VSLLib(\"" << _lib_name << "\",\n\n";

    VSLDef *d = _first;
    if (VSEFlags::dump_last)
	d = _last;

    for (; d != 0; d = d->libnext())
    {
	// Kommentar ausgeben
	s << "// " << d->longname() << " (hashcode: "
	    << d->deflist->hashcode << ") ";
	    
	if (d->deflist->references == 0)
	    s << "(unused) ";
	else
	    s << "(used " << d->deflist->references << " times) ";

	if (d->straight())
	    s << "(straight)\n";
	else
	    s << "(not straight)\n";


	// Header ausgeben
	s << "VSLDef(";
	
	// 1. Parameter: Name, Ort (als Text)
	s << "\"" << d->longname() << "\"";
	s << ", ";

	// 2. Parameter: Pattern (als Text)
	if (d->node_pattern() != 0)
	    s << "\"" << *d->node_pattern() << "\"";
	else
	    s << "NoPatternText()";
	s << ", ";

	// 3. Parameter: Definition (als Text)
	if (d->expr() != 0)
	    s << "\"" << *d->expr() << "\"";
	else
	    s << "NoDefText()";
	s << ", ";

	// 4. Parameter: Pattern (als VSL-Ausdruck)
	if (d->node_pattern() != 0)
	    d->node_pattern()->dumpTree(s);
	else
	    s << "NoPatternTree()";
	s << ", ";

	// 5. Parameter: Definition (als VSL-Ausdruck)
	if (d->expr() != 0)
	    d->expr()->dumpTree(s);
	else
	    s << "NoDefTree()";
	s << ")";

	if (d->libnext() != 0)
	    s << ",\n\n";
    }

    s << "\n);\n";

    StringBox::quoted = old;
}


// Pruefen, ob alles in Ordnung
bool VSLLib::OK() const
{
    // 1. Schleife: ueber Hash-Tabelle
    for (unsigned i = 0; i < hashSize; i++)
	if (defs[i] != 0)
	{
	    // Pruefen, ob Zeiger auf Lib ok
	    assert (defs[i]->lib == this);

	    // Pruefen, ob Deflist ok
	    assert (defs[i]->OK());
	}

    // 2. Schleife: ueber interne Liste
    for (VSLDef *d = _first; d != 0; d = d->libnext())
	assert (d->OK());

    return true;
}
