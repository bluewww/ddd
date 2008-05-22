/* $Id$ -*- C++ -*- */
/* VSL lexical analysis */

%{
// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char VSL_lex_rcsid[] = 
    "$Id$";

// Define BAD_ISTREAM != 0, if istream::open() aborts upon
// non-existing file
#define BAD_ISTREAM 1

#include <stdio.h>
#include <errno.h>
#include "strclass.h"
#include "casts.h"

static const int max_include_nesting = 40;    // max include nesting

static std::istream *streamstack[max_include_nesting]; // File descriptors
static string	namestack[max_include_nesting];	   // File names
static int	linestack[max_include_nesting];	   // File lines
static int	topstack = 0;			   // Next free position

#ifdef FLEX_SCANNER
static YY_BUFFER_STATE bufstack[max_include_nesting]; // Buffer states
#endif

static std::istream *yystream = &std::cin;

static int yylinenumber;

static int switchdown(void);
//static int switchup(const char *filename, bool thisdir);

static char pushback[BUFSIZ];
static char *pushback_ptr = pushback;

static const char *searchpath(const char *filename, bool thisDir);

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
    yyNameSet(const yyNameSet&);
    yyNameSet& operator = (const yyNameSet&);
} yynameSet("xyzzy");

inline int do_unput(char c) 
{
    if (c != '\0')
	*pushback_ptr++ = c;

    return c;
}

static int do_input(void) 
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

// Switch to file FILENAME
static int switchup(const char *filename, bool thisdir) 
{
    if (!string(filename).empty())
    {
	// Search file
	filename = searchpath(filename, thisdir);

	// Add to name set; if we already have read it, don't care
	if (yynameSet.add(filename))
	    return 0;
    }

    if (topstack >= max_include_nesting)
    {
	VSLLib::parse_error("too many #include's");
	return -1;
    }

#ifdef FLEX_SCANNER
    if (YY_CURRENT_BUFFER == 0)
    {
	vslrestart(stdin);
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
	yystream   = &std::cin;
	yyfilename = "standard input";
    }
    else
    {
#if BAD_ISTREAM
    yystream = 0;
    FILE *fp = fopen(filename, "r");
    if (fp == 0 || fclose(fp) == EOF) 
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
    // yystream can now be opened
    yystream = new std::ifstream(filename, std::ios::in);
#endif
    }

    if (VSEFlags::verbose)
    {
	std::cout << "(" << yyfilename;
	std::cout.flush();
    }

    return 0;
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
extern "C" int yywrap(void) 
{ 
    if (switchdown())
	return 1;
    return yystream->eof();
}


// Anything not in the list is a single character
#ifdef ECHO
#undef ECHO
#endif
#define ECHO                 return yytext[0]


#ifdef FLEX_SCANNER
// #defining YY_BREAK to empty disables warnings about unreachable breaks.
#define YY_BREAK DO_NOT_BREAK
#define DO_NOT_BREAK
#define BREAK break;
#endif // FLEX_SCANNER

%}

/* C Identifiers */
I			[_a-zA-Z][_a-zA-Z0-9]*

/* C Strings */
S			\"(\\.|[^\"\n])*\"

/* C Integers */
N			[0-9]+

/* #include Strings */
CS			\"[^\"\n]*\"
CC			\<[^\>\n]*\>

/* Whitespace */
W			[ \f\t]*

%%

{S}		return VSL_STRING;		// string

"//".*		BREAK				// C++ comment
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
			BREAK
		}
^{W}#!.*		BREAK			// Unix exec() tag

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
			BREAK
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
			BREAK
		}
^{W}#{W}line[ \f\t]{W}.* {			// #line LINE ["FILE"]
			string buf((char *)yytext);
			buf = buf.after("line");
			char *s = CONST_CAST(char *,buf.chars());
			int line = strtol(s, &s, 10);
			if (s == buf.chars())
			    VSLLib::parse_error("illegal #line number");
			else
			    yylinenumber = line;

			buf = buf.after('"');
			if (!buf.empty())
			{
			    buf = buf.before('"');
			    if (buf == "")
				VSLLib::parse_error("illegal #line file");
			    else
				yyfilename = uncook(buf);
			}
			BREAK
		}

^{W}#{W}override   return OVERRIDE;		// Other manipulators
^{W}#{W}undef	   return OVERRIDE;
^{W}#{W}replace	   return REPLACE;
^{W}#{W}pragma{W}override   return OVERRIDE;	// Other manipulators
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

{W}		BREAK				// Whitespace (ignore)
"\n"		BREAK				// Likewise
.		return yytext[0];		// All other characters
%%
// Delete name set
void yyNameSet::reset()
{ 
    if (_next) 
	delete _next; 
    _next = 0; 
}

// Add to name set; if already there, return -1
int yyNameSet::add(const char *name)
{
    if (strcmp(_name.chars(), name) == 0)
	return -1;

    if (_next)
	return _next->add(name);

    _next = new yyNameSet(name);
    return 0;
}

// Clear name set
static void switchreset()
{
    yynameSet.reset();
}


// Return to original file
static int switchdown(void) 
{
    if (topstack <= 0)
	return -1;
	
    if (yystream != 0)
    {
	if (VSEFlags::verbose)
	{
	    std::cout << ")";
	    std::cout.flush();
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

static string dirname(const string& file)
{
    if (file.contains('/'))
	return file.before('/', -1);
    else
	return ".";
}

// Search file in directories -I, $VSL_INCLUDE.
// If THISDIR == true, also search in current directory,
// If not found, return FILENAME.
static const char *searchpath(const char *filename, bool thisDir)
{
    char includeList[BUFSIZ];

    // If "-I" is given, use this path
    const char *s = VSEFlags::include_search_path;
    if (s == 0 || s[0] == '\0')
    {
	// Otherwise: Use environment or default path
	s = getenv("VSL_INCLUDE");
	if (s == 0)
	{
	    static string path = 
		string(VSL_INCLUDE_PATH) +
		":/usr/local/vse/vsl-include"
		":/usr/vse/vsl-include"
		":/usr/local/lib/vse/vsl-include"
		":/usr/lib/vse/vsl-include"
		":/usr/local/lib/vsl-include"
		":/usr/lib/vsl-include";
	    
	    s = path.chars();
	}
    }

    // If this dir to be searched, prepend directory name of current file.
    if (thisDir)
    {
	string dir = dirname(yyfilename);
	dir += ":";
	strcpy(includeList, dir.chars());
    }
    else
    {
	strcpy(includeList, "");
    }
    strncat(includeList, s, BUFSIZ - 2);

    // Check dirs; if file can be opened, return full path
    for (char *p = strtok(includeList, ":"); 
	 p != 0; 
	 p = strtok((char *)0, ":"))
    {
	static char path[BUFSIZ];
	strcpy(path, p);
	strcat(path, "/");
	strcat(path, filename);

	FILE *fp = fopen(path, "r");
	if (fp != 0 && fclose(fp) != EOF)
	{
	    // Remove "./" prefix
	    if (path[0] == '.' && path[1] == '/')
		return path + 2;
	    else
		return path;
	}
    }

    return filename;
}
