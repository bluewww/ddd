/* $Id$ -*- C++ -*- */
/* VSL lexical analysis */

%{
// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char VSL_lex_rcsid[] = 
    "$Id$";

// BAD_ISTREAM != 0 definieren, falls istream::open()
// bei nicht existierender Datei abbricht
#define BAD_ISTREAM 1

#include <stdio.h>
#include <errno.h>
#include "strclass.h"

static const int max_include_nesting = 40;    // max. Verschachtelungstiefe

static istream	*streamstack[max_include_nesting]; // Dateideskriptoren
static string	namestack[max_include_nesting];	   // Dateinamen
static int	linestack[max_include_nesting];	   // Dateizeilen
static int	topstack = 0;			   // Naechste freie Position

#ifdef FLEX_SCANNER
static YY_BUFFER_STATE bufstack[max_include_nesting]; // Buffer states
#endif

static istream *yystream = &cin;

static int yylinenumber;

static int switchdown();
static int switchup(char *filename, bool thisdir);

static char pushback[BUFSIZ];
static char *pushback_ptr = pushback;

inline int do_unput(char c) 
{
    if (c != 0)
	*pushback_ptr++ = c;

    return c;
}

static int do_input() 
{
    if (pushback_ptr != pushback)
	return *--pushback_ptr;

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
#define YY_SKIP_YYWRAP
extern "C" int yywrap() 
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

{S}		return STRING;			// string

"//".*		;				// C++ comment
"/*"		{				// C comment
			// aus `man lex`
			loop:
			while (yyinput() != '*');
			switch (yyinput())
				{
				case '/': break;
				case '*': unput('*');
				default : goto loop;
				}

			// Avoid warnings about unused function
			(void) yyunput;
		}
^{W}#!.*		;			// Unix exec() tag

^{W}#{W}include{W}{CS}.* {			// #include "..."
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
^{W}#{W}include{W}{CC}.* {			// #include <...>
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
^{W}#{W}line[ \f\t]{W}.* {		// #line LINE ["FILE"]
			string buf((char *)yytext);
			buf = buf.after("line");
			char *s = (char *)buf.chars();
			int line = strtol(s, &s, 10);
			if (s == buf.chars())
			    VSLLib::parse_error("illegal #line number");
			else
			    yylinenumber = line;

			buf = buf.after('"');
			if (buf != "")
			{
			    buf = buf.before('"');
			    if (buf == "")
				VSLLib::parse_error("illegal #line file");
			    else
				yyfilename = uncook(buf);
			}
		}

^{W}#{W}override   return OVERRIDE;		// Andere Manipulatoren
^{W}#{W}undef	   return OVERRIDE;
^{W}#{W}replace	   return REPLACE;
^{W}#{W}pragma{W}override   return OVERRIDE;	// Andere Manipulatoren
^{W}#{W}pragma{W}undef	    return OVERRIDE;
^{W}#{W}pragma{W}replace    return REPLACE;

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
{I}		return IDENTIFIER;

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
    yyNameSet(const char *name)
	: _name(name), _next(0)
    {}
    ~yyNameSet()	   { reset(); }
    void reset();
    int add(const char *name);

private:
    yyNameSet(const yyNameSet&)
	: _name(), _next(0)
    {
	assert(0);
    }
    yyNameSet& operator = (const yyNameSet&)
    {
	assert(0); return *this;
    }
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
int yyNameSet::add(const char *name)
{
    if (strcmp(_name.chars(), name) == 0)
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
    yystream	 = streamstack[topstack];
    yyfilename	 = namestack[topstack];
    yylinenumber = linestack[topstack];

#ifdef FLEX_SCANNER
    if (YY_CURRENT_BUFFER != 0)
	yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(bufstack[topstack]);
#endif

    return 0;
}

#ifndef VSL_INCLUDE_PATH
#define VSL_INCLUDE_PATH "/usr/local/lib/vsl-include"
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
		string(VSL_INCLUDE_PATH) +
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

    bufstack[topstack]	  = YY_CURRENT_BUFFER;
#endif

    streamstack[topstack] = yystream;
    namestack[topstack]	  = yyfilename;
    linestack[topstack]	  = yylinenumber;
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
	VSLLib::parse_error(string(filename) + ": " + strerror(errno));
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
