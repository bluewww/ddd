// $Id$
// Globale Steuerungsflags fuer VSE

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
// Revision 1.1  1995/05/01 15:47:54  zeller
// Initial revision
//
// Revision 10.14  1995/03/17  07:44:12  zeller
// New: renamed `boolean' to `bool'
//
// Revision 10.13  1993/08/27  14:35:46  zeller
// Headerzeile verkuerzt
//
// Revision 10.12  1993/05/22  20:12:27  zeller
// %Q% added by apply-q-flag.sh 1.5
//
// Revision 10.11  1993/04/22  11:02:21  zeller
// Lizenz verbessert
// 
// Revision 10.10  1993/04/21  00:20:07  zeller
// Umbenennung der Header-Dateien
// 
// Revision 10.9  1993/04/20  23:01:43  zeller
// Neuer Name gegen mehrfaches #include
// 
// Revision 10.8  1993/04/16  11:40:09  zeller
// Neu: GNU Library General Public License eingefuegt
// 
// Revision 10.7  1992/10/28  15:39:48  zeller
// #include's angepasst
// 
// Revision 10.6  1992/10/13  20:52:01  zeller
// strclass.h statt String.h
// 
// Revision 10.5  1992/06/01  15:23:57  zeller
// Anpassung an gcc 2.0
// 
// Revision 10.4  1992/06/01  11:57:14  zeller
// #pragma once entfernt
// 
// Revision 10.3  1992/02/18  09:56:21  zeller
// Konfliktfreie Typbezeichner eingefuehrt
// 
// Revision 10.2  1992/02/17  16:51:21  zeller
// Interne optimize-Optionen sichtbar gemacht
// 
// Revision 10.1  1992/02/17  14:10:44  zeller
// Tabstops von 4 auf 8 gesetzt
// 
// Revision 9.2  1991/07/10  14:01:41  zeller
// Neu: -what
// 
// Revision 9.1  1991/07/08  05:53:50  zeller
// Installierte Version
// 
// Revision 8.3  1991/07/08  04:19:07  zeller
// Neu: -concurrent
// 
// Revision 8.2  1991/07/06  17:27:37  zeller
// Neu: -open
// 
// Revision 8.1  1991/07/06  15:01:13  zeller
// Installierte Version
// 
// Revision 1.30  1991/07/05  09:33:30  zeller
// Neu: -Dissue-nooptionals
// 
// Revision 1.29  1991/07/02  17:16:06  zeller
// Neu: -Dsupress-eval
// 
// Revision 1.28  1991/07/02  16:58:59  zeller
// Neu: -Dshow-last
// 
// Revision 1.27  1991/07/02  14:34:24  zeller
// Neu: -verbose
// 
// Revision 1.26  1991/07/02  13:09:59  zeller
// Neu: -Dshow-tree
// 
// Revision 1.25  1991/06/30  18:43:28  zeller
// Neu: -Doptimize-globals
// 
// Revision 1.24  1991/06/30  18:32:41  zeller
// Neu: -Dbox-tag-info
// 
// Revision 1.23  1991/06/24  08:55:17  zeller
// Neu: -Oincremental-eval
// 
// Revision 1.22  1991/06/23  12:44:47  zeller
// Neu: bag_mode
// 
// Revision 1.21  1991/06/17  16:53:04  zeller
// Neu: -Ddoc-select-info
// 
// Revision 1.20  1991/06/09  16:10:41  zeller
// Neu: VSEFlag()
// 
// Revision 1.19  1991/06/09  16:08:03  zeller
// Neu: -Dshow-draw
// 
// Revision 1.18  1991/06/08  18:30:03  zeller
// Neu: -Dshow-document
// 
// Revision 1.17  1991/05/22  12:08:44  zeller
// Neu: -Dshow-vars
// 
// Revision 1.16  1991/05/18  17:08:19  zeller
// Neu: -Dshow-ids
// 
// Revision 1.15  1991/05/08  15:07:58  zeller
// Neu: -Nloops
// 
// Revision 1.14  1991/05/07  16:05:42  zeller
// Neu: -Dshow-match-defs
// 
// Revision 1.13  1991/05/07  16:04:00  zeller
// Neu: -Dshow-backtrace
// 
// Revision 1.12  1991/05/06  08:34:59  zeller
// Neu: -Ninfo-nesting legt Ausgabetiefe Box fest
// 
// Revision 1.11  1991/02/27  01:29:13  zeller
// Neu: include_list_info
// 
// Revision 1.10  1991/02/21  15:16:37  zeller
// Neue match- und eval-Optionen
// 
// Revision 1.9  1991/02/20  18:17:41  zeller
// -Dshow-stack-search durch -Dshow-matching ersetzt
// 
// Revision 1.8  1991/02/17  18:34:07  zeller
// Neu: max_pattern_variables
// 
// Revision 1.7  1991/02/11  18:47:25  zeller
// Lokale Variablen aus Klasse entfernt
// 
// Revision 1.6  1991/02/11  16:51:22  zeller
// Neu: include_arg_range_info
// 
// Revision 1.5  1991/02/10  17:42:49  zeller
// Neu: cleanup
// 
// Revision 1.4  1991/02/10  12:46:45  zeller
// Neu: parse() fuehrt alles wichtige automatisch aus
// 
// Revision 1.3  1991/02/10  12:28:30  zeller
// Namen verkuerzt
// 
// Revision 1.2  1991/02/10  12:13:15  zeller
// EntryType oeffentlich gemacht, um Compiler nicht zu verwirren
// 
// Revision 1.1  1991/02/10  12:09:30  zeller
// Initial revision
// 

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
