// $Id$
// View the Source.

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de)
// and Andreas Zeller (zeller@ips.cs.tu-bs.de).
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
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

//-----------------------------------------------------------------------------
// This class creates the source view window and provides necessary
// callbacks.
//
// Name conventions:
// ...CB  : Callback function.
// ...DCB : Dialog callback function.
// ...CD  : Create a dialog.
// ...Act : Action function.
// ...SQ  : Calls gdb->send_question().
// ...OQC : For on_question_completion (type: OQCProc), see GDBAgent.h
// ...SQA : Calls gdb->send_qu_array().
// ...OQAC: For on_question_array_completion (type: OQACProc), see GDBAgent.h
// ...HP  : Handler procedure (type: HandlerProc), see HandlerL.h
// 
//-----------------------------------------------------------------------------

#ifndef _SourceView_h
#define _SourceView_h

#ifdef __GNUG__
#pragma interface
#endif

// Motif includes
#include <Xm/Xm.h>

// Misc includes
#include "strclass.h"
#include "Assoc.h"
#include "MakeMenu.h"
#include "StringA.h"

// DDD includes
#include "ArgField.h"
#include "GDBAgent.h"
#include "BreakPM.h"
#include "CodeCache.h"
#include "Delay.h"

//-----------------------------------------------------------------------------
extern GDBAgent* gdb;
extern ArgField* source_arg;

//-----------------------------------------------------------------------------
class SourceView {
    static void add_to_history(const string& file_name, int line);
    static void add_to_history(const string& address);
    static void goto_entry(string entry);

    static void other_fileCB             (Widget, XtPointer, XtPointer);
    static void set_source_argCB         (Widget, XtPointer, XtPointer);

    static void line_popup_setCB         (Widget, XtPointer, XtPointer);
    static void line_popup_set_tempCB    (Widget, XtPointer, XtPointer);
    static void line_popup_temp_n_contCB (Widget, XtPointer, XtPointer);

    static void address_popup_setCB         (Widget, XtPointer, XtPointer);
    static void address_popup_set_tempCB    (Widget, XtPointer, XtPointer);
    static void address_popup_temp_n_contCB (Widget, XtPointer, XtPointer);

    static void bp_popup_infoCB          (Widget, XtPointer, XtPointer);
    static void bp_popup_deleteCB        (Widget, XtPointer, XtPointer);
    static void bp_popup_disableCB       (Widget, XtPointer, XtPointer);

    static void text_popup_printCB       (Widget, XtPointer, XtPointer);
    static void text_popup_dispCB        (Widget, XtPointer, XtPointer);
    static void text_popup_print_refCB   (Widget, XtPointer, XtPointer);
    static void text_popup_disp_refCB    (Widget, XtPointer, XtPointer);
    static void text_popup_whatisCB      (Widget, XtPointer, XtPointer);
    static void text_popup_lookupCB      (Widget, XtPointer, XtPointer);
    static void text_popup_breakCB       (Widget, XtPointer, XtPointer);
    static void text_popup_clearCB       (Widget, XtPointer, XtPointer);

    static void NewBreakpointDCB         (Widget, XtPointer, XtPointer);
    static void NewBreakpointCB          (Widget, XtPointer, XtPointer);

    static void LookupBreakpointCB       (Widget, XtPointer, XtPointer);
    static void BreakpointCmdCB          (Widget, XtPointer, XtPointer);

    static void EditBreakpointIgnoreCountDCB (Widget, XtPointer, XtPointer);
    static void EditBreakpointIgnoreCountCB  (Widget, XtPointer, XtPointer);
    static void EditBreakpointConditionDCB   (Widget, XtPointer, XtPointer);
    static void EditBreakpointConditionCB    (Widget, XtPointer, XtPointer);

    static void UpdateBreakpointButtonsCB (Widget, XtPointer, XtPointer);

    static void CheckScrollCB(Widget, XtPointer, XtPointer);
    static void CheckScrollWorkProc(XtPointer, XtIntervalId *);

    static void StackDialogPoppedDownCB (Widget, XtPointer, XtPointer);
    static void CodeDialogPoppedDownCB (Widget, XtPointer, XtPointer);
    static void RegisterDialogPoppedDownCB (Widget, XtPointer, XtPointer);
    static void SelectFrameCB (Widget, XtPointer, XtPointer);
    static void SelectRegisterCB (Widget, XtPointer, XtPointer);

    static void fill_labels(const string& info_output);

    static Widget create_glyph(Widget form_w, String name, 
			       char *bits, int width, int height);
    static void map_glyph(Widget& w, Position x, Position y);
    static void update_title ();
    static void update_glyphs ();
    static void UpdateGlyphsWorkProc(XtPointer, XtIntervalId *);
    static Boolean CreateGlyphsWorkProc(XtPointer);
    static void MoveCursorToGlyphPosCB(Widget, XtPointer, XtPointer);
    static int line_height (Widget text_w);

    static void refresh_bp_disp ();
    static void refresh_source_bp_disp ();
    static void refresh_code_bp_disp ();

    // Findet zu pos die passende Zeilennummer.
    // in_text==true wenn pos im Quelltext-Bereich ist.
    // bp_nr ist die Nr des Brekpoints, 
    // der an Position pos dargestellt wird, 0 sonst.
    //
    static bool get_line_of_pos (Widget w,
				 XmTextPosition pos,
				 int& line_nr,
				 string& address,
				 bool& in_text,
				 int& bp_nr);

    // Findet von pos ausgehend die Anfangs- und Endposition eines C-Strings.
    // (text[startpos] ist dann der erste und text[endpos] der letzte Buchstabe
    // des gefundenen Wortes)
    //
    static void find_word_bounds (Widget w,
				  const XmTextPosition pos,
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
    static void refresh_bpsOQC (const string&, void*);

    //-----------------------------------------------------------------------
    // Aktions-Prozeduren
    //-----------------------------------------------------------------------
    static void srcpopupAct (Widget, XEvent*, String*, Cardinal*);
    static void startSelectWordAct (Widget, XEvent*, String*, Cardinal*);
    static void endSelectWordAct (Widget, XEvent*, String*, Cardinal*);
    static void updateGlyphsAct (Widget, XEvent*, String*, Cardinal*);

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
    static MMDesc address_popup[];
    static MMDesc bp_popup_gdb[];
    static MMDesc bp_popup_dbx[];
    static MMDesc text_popup[];
    static MMDesc bp_area[];

    //-----------------------------------------------------------------------
    // Daten
    //-----------------------------------------------------------------------
    static StringArray history;
    static int history_position; // Last position in history + 1
    static bool source_history_locked;
    static bool code_history_locked;

    static XmTextPosition last_start_secondary_highlight;
    static XmTextPosition last_end_secondary_highlight;

    static XmTextPosition last_start_secondary_highlight_pc;
    static XmTextPosition last_end_secondary_highlight_pc;

    static Widget toplevel_w;	 // Top-level widget

    static Widget source_form_w; // Form around text and glyphs
    static Widget source_text_w; // Source text
    static Widget code_form_w;   // Form around Machine code and glyphs
    static Widget code_text_w;   // Machine code text

    static Widget edit_breakpoints_dialog_w; // Dialog for editing breakpoints
    static Widget breakpoint_list_w;         // The breakpoint list

    static Widget stack_dialog_w;            // Dialog for viewing the stack
    static Widget frame_list_w;              // The frame list
    static Widget up_w;                      // The `Up' button
    static Widget down_w;                    // The `Down' button
    static bool stack_dialog_popped_up;	     // True if the stack is visible

    static Widget register_dialog_w;          // Dialog for registers
    static Widget register_list_w;            // Register list inside
    static bool register_dialog_popped_up;    // True if registers are visible

    static bool display_glyphs;	         // True if glyphs are to be displayed
    static bool disassemble;	         // True if code is to be disassembled
    static bool at_lowest_frame;              // True if at lowest frame

    // The indenting amounts
    static int  bp_indent_amount;   // Source
    static int  code_indent_amount; // Machine code

    // The breakpoint map
    static BreakPointMap bp_map;

    // File attributes
    static string current_file_name;
    static int    line_count;
    static Assoc<int, VarArray<int> > bps_in_line;
    static XmTextPosition*            pos_of_line;
    static StringArray bp_addresses;

    // Return TRUE if breakpoint BP is in current file
    static bool bp_matches(BreakPoint *bp);
    static string full_path(string file);
    static const char *basename(const char *);
    static bool file_matches(const string& file1, const string& file2);
    static bool base_matches(const string& file1, const string& file2);

    static Assoc<string, string> file_cache;
    static CodeCache code_cache;

    // The current source text.
    static string current_source;

    // The current assembler code.
    static string current_code;
    static string current_code_start;
    static string current_code_end;

    // Return current source name (name under this source is known to GDB)
    static Assoc<string, string> source_name_cache;
    static string current_source_name();

    // The current directory
    static string current_pwd;

    // Some positions in source text.
    static XmTextPosition last_top;
    static XmTextPosition last_pos;
    static XmTextPosition last_start_highlight;
    static XmTextPosition last_end_highlight;

    // Some positions in assembler code.
    static XmTextPosition last_top_pc;
    static XmTextPosition last_pos_pc;
    static XmTextPosition last_start_highlight_pc;
    static XmTextPosition last_end_highlight_pc;

    static string last_execution_file;
    static int    last_execution_line;
    static string last_execution_pc;
    static void _show_execution_position (string file, int line);

    // Read source text
    static String read_local(const string& file_name, long& length);
    static String read_remote(const string& file_name, long& length);
    static String read_from_gdb(const string& file_name, long& length);

    static String read_indented(string& file_name, long& length);
    static int read_current(string& file_name, bool force_reload = false);

    static void SetInsertionPosition(Widget w,
				     XmTextPosition pos, 
				     bool fromTop = false);

    static bool is_source_widget(Widget w);
    static bool is_code_widget(Widget w);
    static string& current_text(Widget w);

    // Assembler code display routines.
    static Delay *refresh_code_pending;
    static XmTextPosition find_pc(const string& pc);
    static void refresh_codeOQC(const string& answer, void *data);
    static void refresh_codeWorkProc(XtPointer client_data, XtIntervalId *);
    static void set_code(const string& code,
			 const string& start,
			 const string& end);

public:
    // Constructor
    SourceView (XtAppContext app_context,
		Widget       parent,
		int          bp_indent_amount);

    enum SearchDirection {forward, backward};

    // Set current execution position, based on the GDB position info
    // POSITION; no arg means clear current position.
    // STOPPED indicates that the program just stopped.
    static void show_execution_position (string position = "",
					 bool stopped = false);

    // Set cursor position, based on the GDB position info POSITION
    static void show_position           (string position);

    // Set pc position to location PC; return start of line
    // If MODE is given, highlight PC line
    static void show_pc (const string& pc, 
			 XmHighlightMode mode = XmHIGHLIGHT_NORMAL);

    // Handle breakpoint information
    static void process_info_bp         (string& info_output);

    // Handle 'info line' information
    static void process_info_line_main  (string& info_output);

    // Handle 'where' information
    static void process_where           (string& where_output);

    // Handle 'frame' information
    static void process_frame           (string& frame_output);

    // Handle 'info register' information
    static void process_register        (string& where_output);

    // Handle 'disassemble' information
    static void process_disassemble     (const string& disassemble_output);

    // Handle 'pwd' information
    static void process_pwd             (string& pwd_output);

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
    static void read_file(string file_name,
			  int initial_line = 1,
			  bool force_reload = false);

    // Reload current file
    static void reload();

    // Lookup previous/next position
    static void go_back();
    static void go_forward();

    // Return source cursor position in <source>:<line> format.
    static string line_of_cursor();

    // Callbacks for menu bar
    static void EditBreakpointsCB(Widget, XtPointer, XtPointer);
    static void ViewStackFramesCB(Widget, XtPointer, XtPointer);
    static void ViewRegistersCB(Widget, XtPointer, XtPointer);

    // Refreshing dialogs
    static void refresh_stack_frames();
    static void refresh_registers();

    // Check whether specific commands are required at next prompt
    static bool where_required();
    static bool register_required();

    // Check whether source files and code are to be cached
    static bool cache_source_files;
    static bool cache_machine_code;

    // Set whether glyphs are to be displayed
    static void set_display_glyphs(bool value);

    // Set whether machine code is to be displayed
    static void set_disassemble(bool value);

    // Return `clear ARG' command
    static string clear_command(string arg);

    // Return `{ COMMAND; }'
    static string command_list(string cmd);

    // Return current directory
    static string pwd() { return current_pwd; }

    // Return source code widget (read-only)
    static Widget source() { return source_text_w; }

    // Clear caches
    static void clear_file_cache();
    static void clear_code_cache();
    static void clear_history();

    // Get the line at POSITION
    static string get_line(string position);
};

#endif
