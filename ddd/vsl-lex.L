/* $Id$ */
/* VSL lexical analysis */

%{
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
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

//
// $Log$
// Revision 1.1  1995/05/01 15:47:25  zeller
// Initial revision
//
// Revision 10.16  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.15  1994/12/13  18:05:34  zeller
// New: provide default for NORA_VSL_PATH
//
// Revision 10.14  1994/12/13  15:01:32  zeller
// Fix: This is flex iff FLEX_SCANNER is defined
//
// Revision 10.13  1994/12/13  14:02:06  zeller
// Fix: yywrap() handles switchdown() return code
//
// Revision 10.12  1994/03/02  16:50:52  zeller
// Fix: fclose(): Rueckgabewert getestet
//
// Revision 10.11  1994/02/19  20:21:28  zeller
// Neu: NORA-Pfade mit hineinuebersetzen
//
// Revision 10.10  1993/11/11  16:00:22  zeller
// Fix: yynameSet statisch deklariert
//
// Revision 10.9  1993/06/30  07:34:12  zeller
// Fix: Log ueberarbeitet
//
// Revision 10.8  1993/05/22  20:18:26  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.7  1993/04/20  20:47:12  zeller
// Neu: Anpassung an FLEX und BISON
// Fix: #-Anweisungen: Whitespace vor und nach # zugelassen
// Fix: #include: \n im Pfad verboten
// Fix: Suchpfad erweitert
// Fix: Einrueckung korrigiert
// 
// Revision 10.6  1993/04/16  13:45:06  zeller
// Fix: Copyright-Information neu angeordnet (lex schafft nicht soviel)
// 
// Revision 10.5  1993/04/16  11:44:55  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.4  1993/01/30  16:26:38  zeller
// All messages after german oversitted
// 
// Revision 10.3  1993/01/28  13:20:21  zeller
// Fix: yyinput, yyoutput, yyunput jetzt von lexToC deklariert
// 
// Revision 10.2  1992/06/01  14:58:52  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.1  1992/02/18  11:19:23  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 9.1  1991/07/08  06:02:37  zeller
// Installierte Version
// 
// Revision 8.1  1991/07/06  15:05:27  zeller
// Installierte Version
// 
// Revision 4.4  1991/07/06  13:29:47  zeller
// BAD_ISTREAM: fp wieder geschlossen
// 
// Revision 4.3  1991/07/02  14:34:52  zeller
// Neu: verbose jetzt in VSEFlags
// 
// Revision 4.2  1991/06/30  17:37:46  zeller
// Neu: switchreset()
// 
// Revision 4.1  1991/05/25  12:51:52  zeller
// Neue Version mit folgenden Eigenschaften:
// Let- und Where-Konstrukten;
// Lokalen und globalen Funktionen, Overloading;
// Erweiterten Fehlermeldungen.
// 
// Revision 3.11  1991/05/22  11:12:48  zeller
// Beschreibung korrigiert
// 
// Revision 3.10  1991/05/21  20:03:03  zeller
// override, undefine, replace jetzt wie #include geschrieben
// 
// Revision 3.9  1991/05/21  19:56:40  zeller
// Neu: REPLACE;
// Neu: UNDEFINE als Synonym fuer OVERRIDE
// 
// Revision 3.8  1991/05/21  19:09:06  zeller
// Neu: OVERRIDE
// 
// Revision 3.7  1991/05/09  16:04:45  zeller
// Warnungen vermieden
// 
// Revision 3.6  1991/05/05  12:30:23  zeller
// Option -I unterstuetzt (wurde auch mal Zeit)
// 
// Revision 3.5  1991/04/30  16:08:34  zeller
// TALIGN wieder eingefuehrt
// 
// Revision 3.4  1991/04/27  14:45:00  zeller
// Operatoren umgestellt: '|' -> ':', '!' -> '|' (wie gehabt);
// CONS-Operator vorbereitet
// 
// Revision 3.3  1991/04/26  14:38:05  zeller
// LET..IN und WHERE-Konstrukte vorbereitet
// 
// Revision 3.2  1991/04/25  13:06:48  zeller
// OPERATOR entfernt
// 
// Revision 3.1  1991/02/26  23:49:40  zeller
// THREEDOTS (wieder) eingefuehrt
// 
// Revision 2.4  1991/02/26  18:00:09  zeller
// yyNameSet::reset() nicht mehr inline
// 
// Revision 2.3  1991/02/24  18:43:36  zeller
// Stack-Handling korrigiert: Bei switchup() Zeilen# ablegen
// max. Verschachtelungstiefe auf 40 erhoeh
// 
// Revision 2.2  1991/02/20  18:41:07  zeller
// Neu: Schluesselwort 'operator', "!=" entfernt
// 
// Revision 2.1  1991/02/19  22:45:51  zeller
// Neue Release mit folgenden Eigenschaften:
// * Erweitertes Pattern-Matching auf Funktionsargumente
// * Listen-orientierte Auswertung ohne Stack
// * Verbesserte Fehlererkennung und Debug-Moeglichkeiten
// 
// Revision 1.17  1991/02/11  16:17:00  zeller
// Operator <> eingefuehrt (Alternative zu !=)
// 
// Revision 1.16  1990/10/06  14:01:00  zeller
// Option 'verbose' unterstuetzt
// 
// Revision 1.15  1990/10/02  13:13:52  zeller
// yystream->unget() gibt jetzt den Strom zurueck
// 
// Revision 1.14  1990/09/19  12:04:38  zeller
// Fuehrendes "./" aus Pfad entfernt
// 
// Revision 1.13  1990/09/10  19:10:05  zeller
// Neue Kommentare /*...*/ eingefuehrt;
// Kommentierung verbessert
// 
// Revision 1.12  1990/09/09  18:58:44  zeller
// Fuehrendes '+', '-' aus Zahlen herausgenommen;
// Float-Support entfernt
// 
// Revision 1.11  1990/09/09  17:45:58  zeller
// Neu: 'elsif'
// 
// Revision 1.10  1990/09/07  16:16:37  zeller
// Bool'sche Ausdruecke und Tests eingefuehrt
// 
// Revision 1.9  1990/09/03  15:46:14  zeller
// switchup() beruecksichtigt jetzt Pfad in
// Environment-Variable VSL_INCLUDE
// 
// Revision 1.8  1990/09/01  13:47:11  zeller
// globale Namen mit 'yy' beginnen lassen
// 
// Revision 1.7  1990/08/28  19:39:43  zeller
// Pruefungen fuer nicht existierende Dateien eingefuegt;
// Schutz gegen mehrfaches #include'n eingefuegt.
// 
// Revision 1.6  1990/08/21  13:13:23  zeller
// Umstellung auf Streams
// 
// Revision 1.5  1990/08/19  19:15:09  zeller
// Neue Fehlermeldungen
// 
// Revision 1.4  1990/08/19  15:23:40  zeller
// ECHO's entfernt
// 
// Revision 1.3  1990/08/18  13:44:24  zeller
// 2. Kommentarform #! eingefuehrt (nur am Zeilenanfang)
// 
// Revision 1.2  1990/08/13  12:39:35  zeller
// Neu: ARROW; kein 'extern', 'global', 'local' mehr;
// FLOATs erweitert
// 
// Revision 1.1  1990/08/13  11:52:43  zeller
// Initial revision
// 

char VSL_lex_rcsid[] = "$Id$";

// BAD_ISTREAM != 0 definieren, falls istream::open()
// bei nicht existierender Datei abbricht
#define BAD_ISTREAM 1

#include <stdio.h>
#include "strclass.h"

static const int max_include_nesting = 40;    // max. Verschachtelungstiefe

static istream 	*streamstack[max_include_nesting]; // Dateideskriptoren
static string   namestack[max_include_nesting];	   // Dateinamen
static int   	linestack[max_include_nesting];    // Dateizeilen
static int   	topstack = 0;                      // Naechste freie Position

#ifdef FLEX_SCANNER
static YY_BUFFER_STATE bufstack[max_include_nesting]; // Buffer states
#endif

static istream *yystream = &cin;

static int yylinenumber;

static int switchdown();
static int switchup(char *filename, bool thisdir);

static int pushback;
static int has_pushback = 0;

inline int do_unput(char c) 
{
    if (c != 0)
    {
        pushback = c;
        has_pushback = 1;
    }
}

static int do_input() 
{
    if (has_pushback)
    {
	has_pushback = 0;
	return pushback;
    }

    char c;

    yystream->get(c);
    if (yystream->eof())
	return 0;

    if (c == '\n')
	yylinenumber++;
    return c;
}

// The way SUN lex wants input
#ifdef input
#undef input
#define input do_input
#endif

#ifdef unput
#undef unput
#define unput do_unput
#endif

// The way FLEX wants input
#ifdef FLEX_SCANNER
#undef YY_INPUT
#define YY_INPUT(buf, result, max_size) \
{\
    int c = do_input(); \
    result = (c == '\0') ? YY_NULL : (buf[0] = c, 1); \
}
#endif

#undef yywrap
inline int yywrap() 
{ 
    if (switchdown())
	return 1;
    return yystream->eof();
}
%}

/* C-Bezeichner */
I			[_a-zA-Z][_a-zA-Z0-9]*

/* C-Strings */
S			\"(\\.|[^\"\n])*\"

/* C-Integers */
N			[0-9]+

/* #include-Strings */
CS			\"[^\"\n]*\"
CC			\<[^\>\n]*\>

/* Whitespace */
W			[ \f\t]*

%%

{S}	 	return STRING;			// string

"//".*		; 				// C++ Kommentar
"/*"		{				// C Kommentar
			// aus `man lex`
			loop:
			while (yyinput() != '*');
			switch (yyinput())
				{
				case '/': break;
				case '*': unput('*');
				default : goto loop;
				}
		}
^{W}#!.* 		; 				// Unix exec() tag

^{W}#{W}include{W}{CS}.* { 			// #include "..."
			char buf[BUFSIZ];
			strcpy(buf, (char *)yytext);
			char *start = buf;
		 	while (*start != '\"') 
				start++;
		 	char *end = ++start;
		 	while (*end != '\"') 
				end++;
		 	*end = '\0';
		 	switchup(start, true);
		}
^{W}#{W}include{W}{CC}.* { 			// #include <...>
			char buf[BUFSIZ];
			strcpy(buf, (char *)yytext);
			char *start = buf;
			while (*start != '<') 
				start++;
			char *end = ++start;
			while (*end != '>') 
				end++;
			*end = '\0';
			switchup(start, false);
		}

^{W}#{W}override   return OVERRIDE;		// Andere Manipulatoren
^{W}#{W}undef      return OVERRIDE;
^{W}#{W}replace    return REPLACE;

if		return IF;			// Tokens
then		return THEN;
else		return ELSE;
elsif		return ELSIF;
fi		return FI;
or		return OR;
and		return AND;
not		return NOT;
let		return LET;
in		return IN;
where		return WHERE;
{I} 		return IDENTIFIER;

"->"		return ARROW;
"::"		return CONS;
":"		return APPEND;
"&"		return HALIGN;
"|"		return VALIGN;
"^"		return UALIGN;
"~"		return TALIGN;
"="		return EQ;
"<>"		return NE;
">"		return GT;
">="		return GE;
"<"		return LT;
"<="		return LE;

"..."		return THREEDOTS;

{N}		return INTEGER;

{W}		;				// Whitespace (ignorieren)
"\n"		;				// dito
.		return yytext[0];		// Alle anderen Zeichen
%%
class yyNameSet {
private:
    string _name;
    yyNameSet *_next;
public:
    yyNameSet(string name) { _name = name; _next = 0; }
    ~yyNameSet() 	   { reset(); }
    void reset();
    int add(string name);
} yynameSet("xyzzy");

// Namenmenge loeschen
void yyNameSet::reset()
{ 
    if (_next) 
	delete _next; 
    _next = 0; 
}

// Neuen Namen zur Namenmenge hinzufuegen; 
// wenn bereits dabei, -1 zurueckgeben
int yyNameSet::add(string name)
{
    if (_name == name)
	return -1;

    if (_next)
	return _next->add(name);

    _next = new yyNameSet(name);
    return 0;
}

// Namens-Menge leeren
static void switchreset()
{
    yynameSet.reset();
}


// Zur urspruenglichen Datei zurueckkehren
static int switchdown() 
{
    if (topstack <= 0)
	return -1;
	
    if (yystream != 0)
    {
	if (VSEFlags::verbose)
	{
	    cout << ")";
	    cout.flush();
	}
	delete yystream;
    }

    topstack--;
    yystream     = streamstack[topstack];
    yyfilename   = namestack[topstack];
    yylinenumber = linestack[topstack];

#ifdef FLEX_SCANNER
    if (YY_CURRENT_BUFFER != 0)
	yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(bufstack[topstack]);
#endif

    return 0;
}

#ifndef NORA_VSL_PATH
#define NORA_VSL_PATH "/usr/local/Nora/lib/vsl-include"
#endif

// Datei in Verzeichnissen -I, $VSL_INCLUDE suchen 
// sowie in ".", wenn thisdir == true
// Wenn nicht gefunden, filename zurueckgeben
static char *searchpath(char *filename, bool thisDir)
{
    char includeList[BUFSIZ];

    // Wenn "-I" angegeben, diesen Pfad waehlen
    char *s = VSEFlags::include_search_path;
    if (s == NULL || s[0] == '\0')
    {
	// Sonst: Aus Umgebung nehmen, ggf. Default-Pfad setzen
	s = getenv("VSL_INCLUDE");
	if (s == NULL)
	{
	    static string path = 
		string(NORA_VSL_PATH) +
		":/usr/local/vse/vsl-include"
		":/usr/vse/vsl-include"
		":/usr/local/lib/vse/vsl-include"
		":/usr/lib/vse/vsl-include"
		":/usr/local/lib/vsl-include"
		":/usr/lib/vsl-include";
	    
	    s = path;
	}
    }

    // Wenn dieses Verzeichnis dazugehoeren soll,
    // ".:" an Anfang der Liste setzen
    if (thisDir)
	strcpy(includeList, ".:");
    else
	strcpy(includeList, "");
    strncat(includeList, s, BUFSIZ - 2);

    // Verzeichnisse durchgehen; wenn Datei geoeffnet werden kann,
    // kompletten Pfad zurueckgeben
    for (char *p = strtok(includeList, ":"); p != NULL; p = strtok(NULL, ":"))
    {
	static char path[BUFSIZ];
	strcpy(path, p);
	strcat(path, "/");
	strcat(path, filename);

	FILE *fp = fopen(path, "r");
	if (fp != NULL && fclose(fp) != EOF)
	{
	    // Fuehrende "./" entfernen
	    if (path[0] == '.' && path[1] == '/')
		return path + 2;
	    else
		return path;
	}
    }

    return filename;
}

// In Datei 'filename' wechseln
static int switchup(char *filename, bool thisdir) 
{
    extern int errno;
    extern char *sys_errlist[];

    if (string(filename) != "")
    {
        // Dateinamen um Pfad erweitern
        filename = searchpath(filename, thisdir);

	// Namen zur Namenmenge hinzufuegen;
        // wenn bereits enthalten, kein Wechsel
        if (yynameSet.add(filename))
	    return 0;
    }

    if (topstack >= max_include_nesting) {
	VSLLib::parse_error("too many #include's");
	return -1;
    }

#ifdef FLEX_SCANNER
    if (YY_CURRENT_BUFFER == 0)
    {
	YY_CURRENT_BUFFER = yy_new_buffer(stdin, YY_BUF_SIZE);
	yy_load_buffer_state();
    }

    bufstack[topstack]    = YY_CURRENT_BUFFER;
#endif

    streamstack[topstack] = yystream;
    namestack[topstack]   = yyfilename;
    linestack[topstack]   = yylinenumber;
    topstack++;

    yylinenumber = 1;
    yyfilename = filename;

#ifdef FLEX_SCANNER
    yy_switch_to_buffer(yy_new_buffer(stdin, YY_BUF_SIZE));
#endif

    if (string(filename) == "")
    {
	yystream   = &cin;
	yyfilename = "standard input";
    }
    else
    {
#if BAD_ISTREAM
    yystream = 0;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL || fclose(fp) == EOF) 
    {
#else
    yystream = new ifstream(filename, ios::in);
    if (!yystream->readable()) 
    {
#endif
        switchdown();
	VSLLib::parse_error(string(filename) + ": " + sys_errlist[errno]);
	return -1;
    }

#if BAD_ISTREAM
    // Jetzt kann yystream sicher geoeffnet werden
    yystream = new ifstream(filename, ios::in);
#endif
    }

    if (VSEFlags::verbose)
    {
	cout << "(" << yyfilename;
	cout.flush();
    }

    return 0;
}
