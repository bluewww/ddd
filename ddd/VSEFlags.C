// $Id$
// global flags for VSL evaluation (and the old VSE editor)

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

char VSEFlags_rcsid[] = 
    "$Id$";

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strstream.h>
#include <malloc.h>
#include <X11/Intrinsic.h>

#include "assert.h"
#include "strclass.h"
#include "VSEFlags.h"
#include "VSLLib.h"
#include "config.h"

#ifndef NORA_LIB_PATH
#define NORA_LIB_PATH "/usr/local/Nora/lib/vsl-include"
#endif


// Defaults

// help
static bool help                         = false;

// internal optimize options (only to be set)
bool VSEFlags::optimize_resolveDefs      = true;
bool VSEFlags::optimize_resolveSynonyms  = true;
bool VSEFlags::optimize_foldOps          = true;
bool VSEFlags::optimize_foldConsts       = true;
bool VSEFlags::optimize_inlineFuncs      = true;
bool VSEFlags::optimize_cleanup          = true;
int VSEFlags::max_optimize_loops         = 2;

// external optimize options
bool VSEFlags::incremental_eval          = true;
bool VSEFlags::optimize_globals          = true;

// assertion options
bool VSEFlags::assert_library_ok         = false;
bool VSEFlags::assert_document_ok        = false;

// dump to file options
bool VSEFlags::dump_tree                 = false;
bool VSEFlags::dump_library              = false;
bool VSEFlags::dump_picture              = false;
bool VSEFlags::dump_document             = false;
bool VSEFlags::dump_while_optimizing     = false;
bool VSEFlags::dump_last                 = false;

// show options
bool VSEFlags::show_optimize             = false;
bool VSEFlags::show_match_nodes          = false;
bool VSEFlags::show_match_boxes          = false;
bool VSEFlags::show_match_defs           = false;
bool VSEFlags::show_tiny_eval            = false;
bool VSEFlags::show_large_eval           = false;
bool VSEFlags::show_huge_eval            = false;
bool VSEFlags::show_balance              = false;
bool VSEFlags::show_flags                = false;
bool VSEFlags::show_backtrace            = true;
bool VSEFlags::show_ids                  = false;
bool VSEFlags::show_vars                 = false;
bool VSEFlags::show_draw                 = false;

// info options
bool VSEFlags::include_list_info         = false;
bool VSEFlags::include_size_info         = false;
bool VSEFlags::include_font_info         = false;
bool VSEFlags::include_const_info        = true;
bool VSEFlags::include_tag_info          = false;
bool VSEFlags::include_id_info           = false;
bool VSEFlags::include_select_info       = false;

// time options
bool VSEFlags::show_eval_time            = false;
bool VSEFlags::show_optimizing_time      = false;
bool VSEFlags::show_display_time         = false;

// eval options
bool VSEFlags::supress_eval              = false;
bool VSEFlags::issue_nooptionals         = false;

// arguments
char *VSEFlags::library_file             = "";
char *VSEFlags::document_file            = "";
char *VSEFlags::startup_file             = "";

// mode options
bool VSEFlags::bag_mode                  = false;
bool VSEFlags::concurrent_mode           = false;
bool VSEFlags::verbose                   = false;
bool VSEFlags::what                      = false;
bool VSEFlags::load_open_mode            = true;

// other settings
int VSEFlags::max_eval_nesting           = 500;
int VSEFlags::max_info_nesting           = -1;
int VSEFlags::max_pattern_variables      = 256;
int VSEFlags::loops                      = 1;
char *VSEFlags::include_search_path      = "";


// Tabelle Optionen <-> Namen
OptionTableEntry VSEFlags::optionTable[] = {

// Help options
{ TITLE,    "",                     "Help",
    0 },
{ BOOLEAN,  "help",                 "explain usage",
    &help },
{ BOOLEAN,  "?",                    "explain usage",
    &help },

// Settings
{ TITLE,    "",                     "Settings",
    0 },
{ BOOLEAN,  "verbose",              "enter verbose mode",
    &verbose },
{ BOOLEAN,  "bag",                  "enter bag mode",
    &bag_mode },
{ BOOLEAN,  "open",                 "open documents when loaded",
    &load_open_mode },
{ BOOLEAN,  "concurrent",           "allow concurrent editing",
    &concurrent_mode },
{ BOOLEAN,  "what",                 "display source file versions",
    &what },
{ STRING,   "library",              "load vsl library",
    &library_file },
{ STRING,   "document",             "load document",
    &document_file },
{ STRING,   "startup",              "load startup file",
    &startup_file },
{ STRING,   "I",                    "set #include search path",
    &include_search_path },

// Optimize options
{ TITLE,    "",                     "Optimizing",
    0 },
{ BOOLEAN,  "Oinline-functions",    "perform inlineFuncs optimization",
    &optimize_inlineFuncs },
{ BOOLEAN,  "Ofold-ops",            "perform foldOps optimization",
    &optimize_foldOps },
{ BOOLEAN,  "Ofold-consts",         "perform foldConsts optimization",
    &optimize_foldConsts },
{ BOOLEAN,  "Oresolve-defs",        "perform resolveDefs optimization",
    &optimize_resolveDefs },
{ BOOLEAN,  "Oresolve-synonyms",    "perform resolveSynonyms optimization",
    &optimize_resolveSynonyms },
{ BOOLEAN,  "Ocleanup",             "perform cleanup between opt stages",
    &optimize_cleanup },
{ BOOLEAN,  "Oincremental-eval",    "perform incremental evaluation",
    &incremental_eval },
{ BOOLEAN,  "O",                    "perform all optimizations",
    0 },
{ INT,      "Noptimize-loops",      "set maximum #optimize loops",
    &max_optimize_loops },

// Debugging Options
{ TITLE,    "",                     "Debugging",
    0 },
{ BOOLEAN,  "Dassert-document-ok",  "assert document::OK()",
    &assert_document_ok },
{ BOOLEAN,  "Dassert-library-ok",   "assert library::OK()",
    &assert_library_ok },
{ BOOLEAN,  "Dshow-tree",           "show library tree",
    &dump_tree },
{ BOOLEAN,  "Dshow-library",        "show library (in VSL)",
    &dump_library },
{ BOOLEAN,  "Dshow-last",           "show only last library def",
    &dump_last },
{ BOOLEAN,  "Dshow-document",       "show document (in VSL)",
    &dump_document },
{ BOOLEAN,  "Dshow-picture",        "show picture (in VSL)",
    &dump_picture },
{ BOOLEAN,  "Dconst-info",          "include const info",
    &include_const_info },
{ BOOLEAN,  "Dlist-info",           "include list info",
    &include_list_info },
{ BOOLEAN,  "Dbox-fonts-info",      "include box font info",
    &include_font_info },
{ BOOLEAN,  "Dbox-id-info",         "include box id info",
    &include_id_info },
{ BOOLEAN,  "Dbox-tag-info",        "include box tag info",
    &include_tag_info },
{ BOOLEAN,  "Dbox-size-info",       "include box size info",
    &include_size_info },
{ BOOLEAN,  "Ddoc-select-info",     "include document select state info",
    &include_select_info },
{ BOOLEAN,  "Dshow-balance",        "show balance",
    &show_balance },
{ BOOLEAN,  "Dshow-eval-huge",      "show huge evaluation (documents)",
    &show_huge_eval },
{ BOOLEAN,  "Dshow-eval-large",     "show large evaluation (defs)",
    &show_large_eval },
{ BOOLEAN,  "Dshow-eval-tiny",      "show tiny evaluation (nodes)",
    &show_tiny_eval },
{ BOOLEAN,  "Dshow-backtrace",      "show backtrace on errors",
    &show_backtrace },
{ BOOLEAN,  "Dshow-ids",            "show id creation and deletion",
    &show_ids },
{ BOOLEAN,  "Dshow-vars",           "show variable declaration and use",
    &show_vars },
{ BOOLEAN,  "Dshow-draw",           "show drawing",
    &show_draw },
{ BOOLEAN,  "Dshow-flags",          "show flag processing",
    &show_flags },
{ BOOLEAN,  "Dshow-optimize",       "show optimization",
    &show_optimize },
{ BOOLEAN,  "Dshow-match-nodes",    "show pattern matching (nodes)",
    &show_match_nodes },
{ BOOLEAN,  "Dshow-match-boxes",    "show pattern matching (boxes)",
    &show_match_boxes },
{ BOOLEAN,  "Dshow-match-defs",     "show pattern matching (defs)",
    &show_match_defs },
{ BOOLEAN,  "Dtime-display",        "show display time",
    &show_display_time },
{ BOOLEAN,  "Dtime-evaluation",     "show evaluation time",
    &show_eval_time },
{ BOOLEAN,  "Dtime-optimizing",     "show optimizing time",
    &show_optimizing_time },
{ BOOLEAN,  "Doptimize-globals",    "perform foldConsts on global defs",
    &optimize_globals },
{ BOOLEAN,  "Dsupress-eval",        "skip evaluation and picture",
    &supress_eval },
{ BOOLEAN,  "Dissue-nooptionals",   "generate code for nooptionals",
    &issue_nooptionals },

// Sizes
{ TITLE,    "",                     "Sizes",
    0 },
{ INT,      "Neval-nesting",        "set max eval nesting",
    &max_eval_nesting },
{ INT,      "Ninfo-nesting",        "set max info nesting",
    &max_info_nesting },
{ INT,      "Npattern-variables",   "set max #variables in pattern",
    &max_pattern_variables },
{ INT,      "Nloops",               "set #evaluation loops",
    &loops },

// Last Option
{ LAST,     "",                     "",
    0 }
};

void VSEFlags::parse(int& argc, char**& argv, char *args)
// Optionen lesen; Wenn Hilfe noetig, ausgeben und Schluss.
{
    // Defaults setzen
    getDefaults();

    // Argumente parsen
    if (_parse(argc, argv))
    {
	cout << argv[0] << ": usage: " << argv[0] << " [options] "
	    << args << "\n\n" << explain();

	exit(1);
    }
}

bool VSEFlags::_parse(int& argc, char**& argv)
// Optionen lesen; true zurueckgeben, wenn Hilfe noetig
{
    int arg = 1;

    while (arg < argc)
    {
	int skip = 0;

	if (argv[arg][0] != '-')        // Keine Option
	    break;

	if (argv[arg][1] == '\0')       // Keine Option ("-")
	    break;

	if (argv[arg][1] == '-')        // Ende Optionen ("--")
	    break;

	string opt = argv[arg];         // Volle Option

	// Ggf. "no-" entfernen
	bool no = false;
	if (opt.contains("no-"))
	{
	    opt.del("no-");
	    no = true;
	}

	// Restnamen ohne beginnendes "-" bilden
	string optName = opt.after("-");

	for (int nentry = 0; optionTable[nentry].type != LAST; nentry++)
	{
	    OptionTableEntry entry = optionTable[nentry];
	    string entryName = entry.name;

	    if (entryName.index(optName) == 0)
	    {   
		// entryName beginnt mit optName:
		// Option gefunden, bearbeiten

		switch (entry.type)
		{
		    case TITLE:
			break;

		    case BOOLEAN:
			bool *boolptr = (bool *)entry.flag;
			if (boolptr)
			    *boolptr = !no;
			skip = 1;
			break;

		    case INT:
			if (arg + 1 < argc)
			{
			    int *intptr = (int *)entry.flag;
			    if (intptr)
				*intptr = atoi(argv[arg + 1]);
			    skip = 2;
			}
			break;

		    case STRING:
			if (arg + 1 < argc)
			{
			    char **stringptr = (char **)entry.flag;
			    if (stringptr)
				*stringptr = argv[arg + 1];
			    skip = 2;
			}
			break;

		    default:
			// darf nicht auftreten
			assert(2 + 2 == 5);
			break;
		}

		if (show_flags)
		{
		    // Bearbeitete Option ausgeben
		    cout << "processed flag: " << argv[arg] << " => -";

		    if (no && entry.type == BOOLEAN)
			cout << entryName.before(rxlowercase) << "no-" 
			    << entryName.from(rxlowercase);
		    else
			cout << entryName;

		    for (int i = 1; i < skip; i++)
			cout << " " << argv[arg + i];
		    cout << "\n";
		}
	    }
	}

	if (skip > 0)
	{
	    // Optionen aus Optionenliste entfernen
	    for (int a = arg; a < argc; a++)
		argv[a] = argv[a + skip];
	    argc -= skip;
	}
	else
	{
	    // Naechstes Argument bearbeiten; dieses hier drin lassen
	    arg++;
	}
    }

    return help;
}


void VSEFlags::getDefaults()
// Default-Optionen aus .vserc, ~/.vserc, /usr/lib/vse/vse-options lesen
{

    string filename = ".vserc";
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
	char *home = getenv("HOME");
	if (home != NULL)
	{
	    filename.prepend(string(home) + "/");
	    fp = fopen(filename, "r");
	}

	if (fp == NULL)
	{
	    static string path = 
		string(NORA_LIB_PATH) +
		":/usr/local/vse"
		":/usr/vse"
		":/usr/local/lib/vse"
		":/usr/lib/vse"
		":/usr/local/lib"
		":/usr/lib";

	    char buffer[BUFSIZ];
	    strcpy(buffer, path);

	    for (char *p = strtok(buffer, ":"); 
		 p != NULL; 
		 p = strtok(NULL, ":"))
	    {
		filename = string(p) + "/vserc";

		fp = fopen(filename, "r");
		if (fp != NULL)
		    break;
	    }

	    cerr << "Warning: could not find .vserc, ~/.vserc or vserc\n";
	    return;
	}
    }

    static char *argv[3];
    static char buf1[BUFSIZ];
    static char buf2[BUFSIZ];

    int nargs;
    do {
	nargs = fscanf(fp, "%s%*[ \t]%s%*[ \t]%*[\n]", buf1, buf2);

	if (nargs > 0 && buf1[0] == '-')
	{
	    argv[0] = "dummy";
	    argv[1] = "";
	    argv[2] = "";
	    if (nargs >= 1)
	    {
		argv[1] = XtNewString(buf1);
		if (nargs >= 2)
		    argv[2] = XtNewString(buf2);
	    }
	    int myargc = nargs + 1;
	    char **myargv = argv;

	    _parse(myargc, myargv);

	    for (int i = 1; i < myargc; i++)
		if (myargv[i][0] == '-')
		    cerr << filename << ": unknown option " << myargv[i] 
			 << "\n";
	}
    } while (nargs != EOF);

    if (fclose(fp) == EOF)
	perror(filename);

    return;
}


string VSEFlags::explain()
// Optionen erklaeren (und defaults ausgeben)
{
    help = false;   // Stets '-help' ausgeben (sonst kaeme -no-help)

    string s = "Options include:\n";

    for (int nentry = 0; optionTable[nentry].type != LAST; nentry++)
    {
	OptionTableEntry entry = optionTable[nentry];

	if (entry.type == TITLE)
	{
	    s = s + "\n" + entry.usage + "\n";
	    continue;
	}

	string optName = entry.name;
	string optArg  = "";
	string usage   = entry.usage;
	string deflt   = "";

	switch (entry.type) {
	    case BOOLEAN:
		bool *boolptr = (bool *)entry.flag;
		if (boolptr)
		{
		    if (*boolptr)
		    {
			optName = optName.before(rxlowercase) + "no-" 
			    + optName.from(rxlowercase);
			usage.prepend("don't ");
		    }
		}
		break;

	    case INT:
		int *intptr = (int *)entry.flag;

		optArg = " [num]";
		usage += " to 'num'";

		if (intptr)
		{
		    ostrstream os;
		    os << " (default: " << *intptr << ")";
		    deflt = string(os);
		}
		break;

	    case STRING:
		char **stringptr = (char **)entry.flag;

		optArg = " [name]";
		usage += " 'name'";

		if (stringptr)
		    deflt = string(" (default: '") + *stringptr + "')";
		break;
	
	    default:
		// darf nicht auftreten
		assert(1 + 1 == 3);
		break;
	}

	optName = "-" + optName + optArg + ":";
	while (optName.length() < 28)
	    optName += " ";
	s += optName;
	usage += deflt;
	s += usage;
	s += "\n";
    }

    s += "\nOptions may be abbreviated, which causes all matching options"
	 " to be set.\n\n";

    return s;
}


unsigned VSEFlags::optimize_mode()
// Optimierungs-Modus zurueckgeben
{
    unsigned mode = 0;
    
    if (optimize_resolveDefs)
	mode |= VSLLib::ResolveDefs;
    if (optimize_resolveSynonyms)
	mode |= VSLLib::ResolveSynonyms;
    if (optimize_foldOps)
	mode |= VSLLib::FoldOps;
    if (optimize_foldConsts)
	mode |= VSLLib::FoldConsts;
    if (optimize_inlineFuncs)
	mode |= VSLLib::InlineFuncs;
    if (optimize_cleanup)
	mode |= VSLLib::Cleanup;

    mode |= max_optimize_loops;

    return mode;
}
