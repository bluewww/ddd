// $Id$
// View the Source.

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the GDB-based data display debugger.
// Contact ddd@ips.cs.tu-bs.de for details.

//-----------------------------------------------------------------------------
// Diese Klasse erzeugt das Source-View-Fenster und stellt die noetigen
// Callback-Funktionen zur Verfuegung.
//
// Namenskonventionen:
// ...CB  : Callback-Funktion.
// ...DCB : Dialog-Callback-Funktion.
// ...CD  : 'create dialog' ; erzeugt einen Dialog.
// ...Act : Aktions-Funktion.
// ...SQ  : ruft gdb->send_question() auf.
// ...OQC : fuer on_question_completion, Typ: OQCProc, siehe GDBAgent.
// ...SQA : ruft gdb->send_qu_array() auf.
// ...OQAC: fuer on_question_array_completion, Typ: OQACProc, siehe GDBAgent.
// ...HP  : Handler-Prozedur. Typ: HandlerProc, siehe HandlerL.h
// 
//-----------------------------------------------------------------------------

#ifndef _SourceView_h
#define _SourceView_h

#ifdef __GNUG__
#pragma interface
#endif

// includes von Motif-Zeugs
#include <Xm/Xm.h>

// includes von Nora-Zeugs
#include "strclass.h"
#include "Assoc.h"
#include "MakeMenu.h"
#include "StringA.h"

// includes eigener Sachen
#include "ArgField.h"
#include "GDBAgent.h"
#include "BreakPoint.h"
#include "Map.h"

//-----------------------------------------------------------------------------
extern GDBAgent* gdb;
extern ArgField* source_arg;

//-----------------------------------------------------------------------------
class SourceView {
    static void add_to_history(string file_name, int line);
    static void goto_entry(string entry);

    static void other_fileCB          (Widget, XtPointer, XtPointer);
    static void set_source_argCB      (Widget, XtPointer, XtPointer);

    static void line_popup_setCB      (Widget, XtPointer, XtPointer);
    static void line_popup_set_tempCB (Widget, XtPointer, XtPointer);
    static void bp_popup_infoCB       (Widget, XtPointer, XtPointer);
    static void bp_popup_deleteCB     (Widget, XtPointer, XtPointer);
    static void bp_popup_disableCB    (Widget, XtPointer, XtPointer);

    static void text_popup_breakCB    (Widget, XtPointer, XtPointer);
    static void text_popup_clearCB    (Widget, XtPointer, XtPointer);
    static void text_popup_printCB    (Widget, XtPointer, XtPointer);
    static void text_popup_dispCB     (Widget, XtPointer, XtPointer);
    static void text_popup_lookupCB   (Widget, XtPointer, XtPointer);

    static void NewBreakpointDCB      (Widget, XtPointer, XtPointer);
    static void NewBreakpointCB       (Widget, XtPointer, XtPointer);

    static void LookupBreakpointCB    (Widget, XtPointer, XtPointer);
    static void BreakpointCmdCB       (Widget, XtPointer, XtPointer);

    static void EditBreakpointIgnoreCountDCB (Widget, XtPointer, XtPointer);
    static void EditBreakpointIgnoreCountCB  (Widget, XtPointer, XtPointer);
    static void EditBreakpointConditionDCB   (Widget, XtPointer, XtPointer);
    static void EditBreakpointConditionCB    (Widget, XtPointer, XtPointer);

    static void UpdateBreakpointButtonsCB (Widget, XtPointer, XtPointer);

    static void StackDialogPoppedDownCB (Widget, XtPointer, XtPointer);
    static void SelectFrameCB (Widget, XtPointer, XtPointer);

    static void fill_labels(const string& info_output);

    static void update_title ();

    static void refresh_bp_disp ();

    // Findet zu pos die passende Zeilennummer.
    // ist in_text!=0, so ist *in_text==true wenn pos im Quelltext-Bereich ist.
    // ist bp_nr!=0, so ist *bp_nr die Nr des Brekpoints, der an Position pos
    // dargestellt wird, 0 sonst.
    //
    static bool get_line_of_pos (XmTextPosition pos,
				 int*     line_nr_ptr,
				 bool* in_text_ptr = 0,
				 int*     bp_nr_ptr = 0);

    // Findet von pos ausgehend die Anfangs- und Endposition eines C-Strings.
    // (text[startpos] ist dann der erste und text[endpos] der letzte Buchstabe
    // des gefundenen Wortes)
    //
    static void find_word_bounds (const XmTextPosition pos,
				  XmTextPosition& startpos,
				  XmTextPosition& endpos);

    //-----------------------------------------------------------------------
    // Anfragen stellen und auswerten
    //-----------------------------------------------------------------------
public:
    // Aktualisiert Breakpoint-Anzeige ('info breakpoints').
    //
    static void refresh_bpsSQ ();
private:
    static void refresh_bpsOQC (string, void*);

    //-----------------------------------------------------------------------
    // Aktions-Prozeduren
    //-----------------------------------------------------------------------
    static void srcpopupAct (Widget, XEvent*, String*, Cardinal*);
    static void startSelectWordAct (Widget, XEvent*, String*, Cardinal*);
    static void endSelectWordAct (Widget, XEvent*, String*, Cardinal*);

    //-----------------------------------------------------------------------
    // Timer-Prozeduren
    //-----------------------------------------------------------------------
    static void setSelection(XtPointer client_data, XtIntervalId *timer);


    //-----------------------------------------------------------------------
    // Xt-Zeugs
    //-----------------------------------------------------------------------
    static XtActionsRec actions [];

    //-----------------------------------------------------------------------
    // Menues
    //-----------------------------------------------------------------------
    static MMDesc line_popup[];
    static MMDesc bp_popup_gdb[];
    static MMDesc bp_popup_dbx[];
    static MMDesc text_popup[];
    static MMDesc bp_area[];

    //-----------------------------------------------------------------------
    // Daten
    //-----------------------------------------------------------------------
    static StringArray history;
    static int history_position;
    static bool history_locked;
    static XmTextPosition last_start_secondary_highlight;
    static XmTextPosition last_end_secondary_highlight;

    static Widget toplevel_w;	 // top-level widget

    static Widget source_view_w; // alles zusammen
#ifdef EXTRA_TEXT_INFO
    static Widget source_info_w; // zeigt Infos zum Text an
    static Widget source_name_w; // zeigt den Filenamen an
    static Widget source_line_w; // zeigt die akt. Zeilennummer an
#endif
    static Widget source_text_w; // das eigentliche Textfenster

    static Widget edit_breakpoints_dialog_w; // dialog for editing breakpoints
    static Widget breakpoint_list_w;         // the breakpoint list

    static Widget stack_dialog_w;            // dialog for viewing the stack
    static Widget frame_list_w;              // the frame list
    static Widget up_w;                      // the `Up' button
    static Widget down_w;                    // the `Down' button
    static bool stack_dialog_popped_up;	     // true if the stack is visible

    // The indenting amount
    static int  bp_indent_amount;

    // The breakpoint map
    static Map<int, BreakPoint> bp_map;

    // File attributes
    static string current_file_name;
    static int    line_count;
    static Assoc<int, VarArray<int> >* bps_in_line;
    static XmTextPosition*             pos_of_line;

    static Assoc<string, string> file_cache;
    static string current_text;

    static XmTextPosition last_pos;
    static XmTextPosition last_start_highlight;
    static XmTextPosition last_end_highlight;

    static string last_execution_file;
    static int    last_execution_line;
    static void _show_execution_position (string file, int line);

    static String read_local(const string& file_name);
    static String read_remote(const string& file_name);
    static String read_from_gdb(const string& file_name);

    static String read_indented(string& file_name);
    static int read_current(string& file_name);

public:
    // Constructor
    SourceView (XtAppContext app_context,
		Widget       parent,
		int          bp_indent_amount);

    enum SearchDirection {forward, backward};

    // Set current execution position, based on the GDB position info
    // POSITION; no arg means clear current position.
    static void show_execution_position (string position = "");

    // Set cursor position, based on the GDB position info POSITION
    static void show_position           (string position);

    // Handle breakpoint information
    static void process_info_bp         (string& info_output);

    // Handle 'info line' information
    static void process_info_line_main  (string& info_output);

    // Handle 'where' information
    static void process_where           (string& where_output);

    // Handle 'frame' information
    static void process_frame           (string& frame_output);

    // Process the remainder of an output line
    static void check_remainder         (string& info_output);

    // Set frame manually: ARG = 0: POS, ARG = +/- N: down/up N levels
    static void set_frame_pos(int arg, int pos = 0);

    // Set frame manually to function FUNC; return True if found
    static bool set_frame_func(const string& func);

    // Find S in current source; select it at TIME.
    static void find(const string& s,
		     SearchDirection direction = forward,
		     bool words_only = false,
		     Time time = CurrentTime);

    // Locate function S; if S is omitted, locate last execution position.
    static void lookup(string s = "");

    // Read file FILE_NAME; place cursor at INITIAL_LINE.
    static void read_file(string file_name, int initial_line = 1);

    // Lookup previous/next position
    static void go_back();
    static void go_forward();

    // Return cursor position in <file>:<line> format.
    static string line_of_cursor();

    // Return top-level widget
    static Widget widget();

    // Callbacks for menu bar
    static void EditBreakpointsCB(Widget, XtPointer, XtPointer);
    static void ViewStackFramesCB(Widget, XtPointer, XtPointer);
    static void refresh_stack_frames();

    // Check whether `where' command is required
    static bool where_required();

    // Check whether source files are to be cached
    static bool cache_source_files;

    // Clear file cache
    static void clear_file_cache();
};

#endif
