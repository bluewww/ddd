// $Id$
// Globale Steuerungsflags fuer VSE

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

#ifndef _Nora_VSEFlags_h
#define _Nora_VSEFlags_h

#include "bool.h"
#include "strclass.h"

// Muss oeffentlich sein, um Compiler nicht zu verwirren
enum EntryType { LAST, BOOLEAN, INT, STRING, TITLE };

// Tabelle Optionen <-> Namen
struct OptionTableEntry {
    EntryType type;     // Typ
    char *name;         // Name
    char *usage;        // Benutzung (Bedeutung)
    void *flag;         // Zeiger auf entsprechenden Flag
};

class VSEFlags {
private:
    // Abbildung Name -> Flag
    static OptionTableEntry optionTable[];

public:
    // assertion options
    static bool assert_library_ok;
    static bool assert_document_ok;

    // dump to file options
    static bool dump_tree;
    static bool dump_library;
    static bool dump_document;
    static bool dump_picture;
    static bool dump_while_optimizing;
    static bool dump_last;

    // show options
    static bool show_optimize;
    static bool show_match_boxes;
    static bool show_match_nodes;
    static bool show_match_defs;
    static bool show_tiny_eval;
    static bool show_large_eval;
    static bool show_huge_eval;
    static bool show_balance;
    static bool show_flags;
    static bool show_backtrace;
    static bool show_ids;
    static bool show_vars;
    static bool show_draw;

    // info options
    static bool include_size_info;
    static bool include_list_info;
    static bool include_font_info;
    static bool include_const_info;
    static bool include_id_info;
    static bool include_tag_info;
    static bool include_select_info;

    // optimize mode
    static bool optimize_resolveDefs;	// write-only
    static bool optimize_resolveSynonyms;	// write-only
    static bool optimize_foldOps;		// write-only
    static bool optimize_foldConsts;		// write-only
    static bool optimize_inlineFuncs;	// write-only
    static bool optimize_cleanup;		// write-only
    static int max_optimize_loops;		// write-only

    static unsigned optimize_mode();
    static bool incremental_eval;
    static bool optimize_globals;

    // modes
    static bool bag_mode;
    static bool verbose;
    static bool what;
    static bool load_open_mode;
    static bool concurrent_mode;

    // time options
    static bool show_eval_time;
    static bool show_optimizing_time;
    static bool show_display_time;

    // eval options
    static bool supress_eval;
    static bool issue_nooptionals;

    // name options
    static char *library_file;
    static char *document_file;
    static char *startup_file;
    static char *include_search_path;

    // num options
    static int max_eval_nesting;
    static int max_pattern_variables;
    static int max_info_nesting;
    static int loops;

    // Unterfunktionen
    static bool _parse(int& argc, char**& argv);
    static void getDefaults();
    static string explain();

    // Haupt-Interface
    static void parse(int& argc, char**& argv, char *args = " files...");
};


// Makro fuer die Abfrage von zeitkritischen Debugging-Optionen

#ifndef NDEBUG
#define VSEFlag(x)  (VSEFlags::x)
#else
#define VSEFlag(x)  false
#endif

#endif
