// $Id$
// global flags for VSL evaluation (and the old VSE editor)

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char VSEFlags_rcsid[] = 
    "$Id$";

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strstream.h>
#include <ctype.h>
#include <X11/Intrinsic.h>

#include "assert.h"
#include "strclass.h"
#include "VSEFlags.h"
#include "VSLLib.h"
#include "config.h"

#ifndef VSL_INCLUDE_PATH
#define VSL_INCLUDE_PATH "/usr/local/lib/vsl-include"
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
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
bool VSEFlags::suppress_eval             = false;
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
VSEOptionTableEntry VSEFlags::optionTable[] = {

// Help options
{ TITLE,    "",                     "Help",
    0 },
{ BOOLEAN,  "help",                 "Show VSL options",
    &help },
{ BOOLEAN,  "?",                    "Show VSL options",
    &help },

// Settings
{ TITLE,    "",                     "Settings",
    0 },
{ BOOLEAN,  "verbose",              "Enter verbose mode",
    &verbose },
{ BOOLEAN,  "bag",                  "Enter bag mode",
    &bag_mode },
{ BOOLEAN,  "open",                 "Open documents when loaded",
    &load_open_mode },
{ BOOLEAN,  "concurrent",           "Allow concurrent editing",
    &concurrent_mode },
{ BOOLEAN,  "what",                 "Display source file versions",
    &what },
{ STRING,   "library",              "Load vsl library",
    &library_file },
{ STRING,   "document",             "Load document",
    &document_file },
{ STRING,   "startup",              "Load startup file",
    &startup_file },
{ STRING,   "I",                    "Set #include search path",
    &include_search_path },
{ STRING,   "include",              "Set #include search path",
    &include_search_path },

// Optimize options
{ TITLE,    "",                     "Optimizing",
    0 },
{ BOOLEAN,  "optimize-inline-functions",    "Perform inlineFuncs optimization",
    &optimize_inlineFuncs },
{ BOOLEAN,  "optimize-fold-ops",            "Perform foldOps optimization",
    &optimize_foldOps },
{ BOOLEAN,  "optimize-fold-consts",         "Perform foldConsts optimization",
    &optimize_foldConsts },
{ BOOLEAN,  "optimize-resolve-defs",        "Perform resolveDefs optimization",
    &optimize_resolveDefs },
{ BOOLEAN,  "optimize-resolve-synonyms",    "Perform resolveSynonyms optimization",
    &optimize_resolveSynonyms },
{ BOOLEAN,  "optimize-cleanup",             "Perform cleanup between optimization stages",
    &optimize_cleanup },
{ BOOLEAN,  "optimize-incremental-eval",    "Perform incremental evaluation",
    &incremental_eval },
{ BOOLEAN,  "optimize-",                    "Perform all optimizations",
    0 },
{ INT,      "max-optimize-loops",           "Set maximum #optimize loops",
    &max_optimize_loops },

// Debugging Options
{ TITLE,    "",                     "Debugging",
    0 },
{ BOOLEAN,  "debug-assert-document-ok",  "Assert document::OK()",
    &assert_document_ok },
{ BOOLEAN,  "debug-assert-library-ok",   "Assert library::OK()",
    &assert_library_ok },
{ BOOLEAN,  "debug-show-tree",           "Show library tree",
    &dump_tree },
{ BOOLEAN,  "debug-show-library",        "Show library (in VSL)",
    &dump_library },
{ BOOLEAN,  "debug-show-last",           "Show only last library def",
    &dump_last },
{ BOOLEAN,  "debug-show-document",       "Show document (in VSL)",
    &dump_document },
{ BOOLEAN,  "debug-show-picture",        "Show picture (in VSL)",
    &dump_picture },
{ BOOLEAN,  "debug-const-info",          "Include const info",
    &include_const_info },
{ BOOLEAN,  "debug-list-info",           "Include list info",
    &include_list_info },
{ BOOLEAN,  "debug-box-fonts-info",      "Include box font info",
    &include_font_info },
{ BOOLEAN,  "debug-box-id-info",         "Include box id info",
    &include_id_info },
{ BOOLEAN,  "debug-box-tag-info",        "Include box tag info",
    &include_tag_info },
{ BOOLEAN,  "debug-box-size-info",       "Include box size info",
    &include_size_info },
{ BOOLEAN,  "debug-doc-select-info",     "Include document select state info",
    &include_select_info },
{ BOOLEAN,  "debug-show-balance",        "Show balance",
    &show_balance },
{ BOOLEAN,  "debug-show-eval-huge",      "Show huge evaluation (documents)",
    &show_huge_eval },
{ BOOLEAN,  "debug-show-eval-large",     "Show large evaluation (defs)",
    &show_large_eval },
{ BOOLEAN,  "debug-show-eval-tiny",      "Show tiny evaluation (nodes)",
    &show_tiny_eval },
{ BOOLEAN,  "debug-show-backtrace",      "Show backtrace on errors",
    &show_backtrace },
{ BOOLEAN,  "debug-show-ids",            "Show id creation and deletion",
    &show_ids },
{ BOOLEAN,  "debug-show-vars",           "Show variable declaration and use",
    &show_vars },
{ BOOLEAN,  "debug-show-draw",           "Show drawing",
    &show_draw },
{ BOOLEAN,  "debug-show-flags",          "Show flag processing",
    &show_flags },
{ BOOLEAN,  "debug-show-optimize",       "Show optimization",
    &show_optimize },
{ BOOLEAN,  "debug-show-match-nodes",    "Show pattern matching (nodes)",
    &show_match_nodes },
{ BOOLEAN,  "debug-show-match-boxes",    "Show pattern matching (boxes)",
    &show_match_boxes },
{ BOOLEAN,  "debug-show-match-defs",     "Show pattern matching (defs)",
    &show_match_defs },
{ BOOLEAN,  "debug-time-display",        "Show display time",
    &show_display_time },
{ BOOLEAN,  "debug-time-evaluation",     "Show evaluation time",
    &show_eval_time },
{ BOOLEAN,  "debug-time-optimizing",     "Show optimizing time",
    &show_optimizing_time },
{ BOOLEAN,  "debug-optimize-globals",    "Perform foldConsts on global defs",
    &optimize_globals },
{ BOOLEAN,  "debug-suppress-eval",       "Skip evaluation and picture",
    &suppress_eval },
{ BOOLEAN,  "debug-issue-nooptionals",   "Generate code for nooptionals",
    &issue_nooptionals },

// Sizes
{ TITLE,    "",                     "Sizes",
    0 },
{ INT,      "eval-nesting",        "Set max eval nesting",
    &max_eval_nesting },
{ INT,      "info-nesting",        "Set max info nesting",
    &max_info_nesting },
{ INT,      "pattern-variables",   "Set max #variables in pattern",
    &max_pattern_variables },
{ INT,      "eval-loops",          "Set #evaluation loops",
    &loops },

// Last Option
{ LAST,     "",                     "",
    0 }
};

// Read options; show help and exit if required
void VSEFlags::parse(int& argc, char**& argv, char *args)
{
    // Get default settings
    getDefaults(true);

    // Parse arguments
    if (_parse(argc, argv, false))
    {
	cerr << argv[0] << ": usage: " << argv[0] << " [options] "
	     << args << "\n\n" << explain();

	exit(EXIT_FAILURE);
    }
}

bool VSEFlags::parse_vsl(int& argc, char**& argv)
{
    // Get default settings
    getDefaults(false);

    return _parse(argc, argv, true);
}
    

// Read options; return true if help is required
bool VSEFlags::_parse(int& argc, char**& argv, bool vsl_prefix_required)
{
    int arg = 1;

    while (arg < argc)
    {
	int skip = 0;

	if (argv[arg] == 0)
	    break;                      // No more args

	if (argv[arg][0] != '-')
	    break;			// No option

	if (argv[arg][1] == '\0')
	    break;		        // No option ("-")

	if (argv[arg][1] == '-' && argv[arg][2] == '\0')
	    break;                      // End of options ("--")

	string opt = argv[arg];         // Full option

	// Remove "no-" prefix
	bool no = false;
	if (opt.contains("no-"))
	{
	    opt.del("no-");
	    no = true;
	}

	// Build option name after leading `-'
	string optName = opt;
	while (optName[0] == '-')
	    optName = optName.after('-');

	// If `vsl-' prefix is required, ignore all other options
	if (vsl_prefix_required)
	    if (optName.index("vsl") != 0)
		break;

	// `vsl-' prefix is optional
	if (optName.index("vsl-") == 0)
	    optName = optName.after("vsl-");

	for (int nentry = 0; optionTable[nentry].type != LAST; nentry++)
	{
	    VSEOptionTableEntry entry = optionTable[nentry];
	    string entryName = entry.name;

	    if (entryName.index(optName) == 0)
	    {   
		// entryName starts with optName: process option

		switch (entry.type)
		{
		    case TITLE:
			break;

		    case BOOLEAN:
		    {
			bool *boolptr = (bool *)entry.flag;
			if (boolptr)
			    *boolptr = !no;
			skip = 1;
			break;
		    }

		    case INT:
		    {
			if (arg + 1 < argc)
			{
			    int *intptr = (int *)entry.flag;
			    if (intptr)
				*intptr = atoi(argv[arg + 1]);
			    skip = 2;
			}
			break;
		    }

		    case STRING:
		    {
			if (arg + 1 < argc)
			{
			    char **stringptr = (char **)entry.flag;
			    if (stringptr)
				*stringptr = argv[arg + 1];
			    skip = 2;
			}
			break;
		    }

		    default:
			// darf nicht auftreten
			assert(2 + 2 == 5);
			break;
		}

		if (show_flags)
		{
		    // Show processed option
		    cout << "processed flag: " << argv[arg] << " => -";

		    if (no && entry.type == BOOLEAN)
		    {
			if (entryName.contains("debug-"))
			    cout << entryName.through("debug-") << "no-" 
				 << entryName.after("debug-");
			else if (entryName.contains("optimize-"))
			    cout << entryName.through("optimize-") << "no-" 
				 << entryName.after("optimize-");
			else
			    cout << "no-" << entryName;
		    }
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
	    // Remove option from argument list
	    for (int a = arg; a < argc; a++)
		argv[a] = argv[a + skip];
	    argc -= skip;
	}
	else
	{
	    // Process next argument; keep this one
	    arg++;
	}
    }

    return help;
}


void VSEFlags::getDefaults(bool warn)
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
		string(VSL_INCLUDE_PATH) +
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

	    if (warn)
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
		argv[1] = (String)XtNewString(buf1);
		if (nargs >= 2)
		    argv[2] = (String)XtNewString(buf2);
	    }
	    int myargc = nargs + 1;
	    char **myargv = argv;

	    _parse(myargc, myargv, false);

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


string VSEFlags::explain(bool vsl_prefix_required)
// Optionen erklaeren (und defaults ausgeben)
{
    help = false;   // Stets '-help' ausgeben (sonst kaeme -no-help)

    string s = "Options include:\n";
    if (vsl_prefix_required)
	s = "List of VSL options:\n";

    for (int nentry = 0; optionTable[nentry].type != LAST; nentry++)
    {
	VSEOptionTableEntry entry = optionTable[nentry];

	if (entry.type == TITLE)
	{
	    s = s + "\n" + entry.usage + "\n";
	    continue;
	}

	string optName = entry.name;
	string optArg  = "";
	string usage   = entry.usage;
	string deflt   = "";

	if (vsl_prefix_required)
	    optName.prepend("vsl-");

	switch (entry.type) {
	    case BOOLEAN:
	    {
		bool *boolptr = (bool *)entry.flag;
		if (boolptr)
		{
		    if (*boolptr)
		    {
			if (optName.contains("debug-"))
			    optName = optName.through("debug-") + "no-" 
				 + optName.after("debug-");
			else if (optName.contains("optimize-"))
			    optName = optName.through("optimize-") + "no-" 
				 + optName.after("optimize-");
			else if (optName.contains("vsl-"))
			    optName = optName.through("vsl-") + "no-" 
				 + optName.after("vsl-");
			else
			    optName = "no-" + optName;

			usage[0] = tolower(usage[0]);
			usage.prepend("Don't ");
		    }
		}
		break;
	    }

	    case INT:
	    {
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
	    }

	    case STRING:
	    {
		char **stringptr = (char **)entry.flag;

		optArg = " [name]";
		usage += " 'name'";

		if (stringptr)
		    deflt = string(" (default: '") + *stringptr + "')";
		break;
	    }
	
	    default:
		// darf nicht auftreten
		assert(1 + 1 == 3);
		break;
	}

	optName = "--" + optName + optArg + ":";
	while (optName.length() < 36)
	    optName += " ";
	s += optName;
	usage += deflt;
	s += usage;
	s += "\n";
    }

    s += "\n";
    s += (vsl_prefix_required ? "VSL options" : "Options");
    s += " may be abbreviated, which causes all matching options"
	 " to be set.\n";

    return s;
}


unsigned VSEFlags::optimize_mode()
// Optimierungs-Modus zurueckgeben
{
    unsigned mode = 0;
    
    if (optimize_resolveDefs)
	mode |= ResolveDefs;
    if (optimize_resolveSynonyms)
	mode |= ResolveSynonyms;
    if (optimize_foldOps)
	mode |= FoldOps;
    if (optimize_foldConsts)
	mode |= FoldConsts;
    if (optimize_inlineFuncs)
	mode |= InlineFuncs;
    if (optimize_cleanup)
	mode |= Cleanup;

    mode |= max_optimize_loops;

    return mode;
}
