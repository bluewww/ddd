// $Id$
// Global flags for VSL evaluation (and the old VSE editor)

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

#ifndef _DDD_VSEFlags_h
#define _DDD_VSEFlags_h

#include "bool.h"
#include "strclass.h"

// GCC 2.5.x wants this to be global
enum VSEEntryType { LAST, BOOLEAN, INT, STRING, TITLE };

// Table Options <-> Names
struct VSEOptionTableEntry {
    VSEEntryType type;  // Type
    const char *name;         // Name
    const char *usage;        // Usage (Meaning)
    void *flag;         // Pointer to corresponding flag
};

class VSEFlags {
private:
    // Map name -> flag
    static VSEOptionTableEntry optionTable[];

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
    static bool optimize_resolveDefs;	  // write-only
    static bool optimize_resolveSynonyms; // write-only
    static bool optimize_foldOps;         // write-only
    static bool optimize_foldConsts;	  // write-only
    static bool optimize_inlineFuncs;	  // write-only
    static bool optimize_cleanup;	  // write-only
    static int max_optimize_loops;	  // write-only

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
    static bool suppress_eval;
    static bool issue_nooptionals;

    // name options
    static const char *library_file;
    static const char *document_file;
    static const char *startup_file;
    static const char *include_search_path;

    // num options
    static int max_eval_nesting;
    static int max_pattern_variables;
    static int max_info_nesting;
    static int loops;

    // Helpers
    static bool _parse(int& argc, const char**& argv, bool vsl_prefix_required);
    static void getDefaults(bool warn = false);
    static string explain(bool vsl_prefix_required = false);

    // Main interface for VSL/VSE
    static void parse(int& argc, const char**& argv, const char *args = " files...");

    // Main interface for other applications; all options require
    // `-vsl' or `--vsl' prefix.  Return TRUE upon error.
    static bool parse_vsl(int& argc, const char**& argv);
};


// Makro for accessing time-critical debugging options

#ifndef NDEBUG
#define VSEFlag(x)  (VSEFlags::x)
#else
#define VSEFlag(x)  false
#endif

#endif
