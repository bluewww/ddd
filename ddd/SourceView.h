// $Id$
// View the Source.

// Copyright (C) 1995-1997 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>
// and Andreas Zeller <zeller@gnu.org>.
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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

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

#ifndef _DDD_SourceView_h
#define _DDD_SourceView_h

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
#include "IntIntAA.h"
#include "TextPosA.h"

// DDD includes
#include "ArgField.h"
#include "GDBAgent.h"
#include "BreakPM.h"
#include "CodeCache.h"
#include "Delay.h"
#include "StringSA.h"
#include "MString.h"

//-----------------------------------------------------------------------------
extern GDBAgent* gdb;
extern ArgField* source_arg;

struct BreakpointPropertiesInfo;

//-----------------------------------------------------------------------------
class SourceView {
    //-----------------------------------------------------------------------
    // Callbacks
    //-----------------------------------------------------------------------
    static void set_source_argCB         (Widget, XtPointer, XtPointer);

    static void line_popup_setCB         (Widget, XtPointer, XtPointer);
    static void line_popup_set_tempCB    (Widget, XtPointer, XtPointer);
    static void line_popup_temp_n_contCB (Widget, XtPointer, XtPointer);
    static void line_popup_set_pcCB      (Widget, XtPointer, XtPointer);

    static void bp_popup_infoCB          (Widget, XtPointer, XtPointer);
    static void bp_popup_deleteCB        (Widget, XtPointer, XtPointer);
    static void bp_popup_disableCB       (Widget, XtPointer, XtPointer);
    static void bp_popup_set_pcCB        (Widget, XtPointer, XtPointer);

    static void text_popup_printCB       (Widget, XtPointer, XtPointer);
    static void text_popup_dispCB        (Widget, XtPointer, XtPointer);
    static void text_popup_watchCB       (Widget, XtPointer, XtPointer);
    static void text_popup_print_refCB   (Widget, XtPointer, XtPointer);
    static void text_popup_disp_refCB    (Widget, XtPointer, XtPointer);
    static void text_popup_watch_refCB   (Widget, XtPointer, XtPointer);
    static void text_popup_whatisCB      (Widget, XtPointer, XtPointer);
    static void text_popup_lookupCB      (Widget, XtPointer, XtPointer);
    static void text_popup_breakCB       (Widget, XtPointer, XtPointer);
    static void text_popup_clearCB       (Widget, XtPointer, XtPointer);

    static void NewBreakpointDCB         (Widget, XtPointer, XtPointer);
    static void NewBreakpointCB          (Widget, XtPointer, XtPointer);

    static void NewWatchpointDCB         (Widget, XtPointer, XtPointer);
    static void NewWatchpointCB          (Widget, XtPointer, XtPointer);

    static void LookupBreakpointCB       (Widget, XtPointer, XtPointer);
    static void PrintWatchpointCB        (Widget, XtPointer, XtPointer);
    static void BreakpointCmdCB          (Widget, XtPointer, XtPointer);

    static void EditBreakpointPropertiesCB (Widget, XtPointer, XtPointer);
    static void ApplyBreakpointPropertiesCB(Widget, XtPointer, XtPointer);
    static void SetBreakpointIgnoreCountCB (Widget, XtPointer, XtPointer);
    static void SetBreakpointConditionCB   (Widget, XtPointer, XtPointer);
    static void EnableBreakpointsCB        (Widget, XtPointer, XtPointer);
    static void DisableBreakpointsCB       (Widget, XtPointer, XtPointer);
    static void MakeBreakpointsTempCB      (Widget, XtPointer, XtPointer);
    static void DeleteBreakpointsCB        (Widget, XtPointer, XtPointer);
    static void RecordBreakpointCommandsCB (Widget, XtPointer, XtPointer);
    static void EndBreakpointCommandsCB    (Widget, XtPointer, XtPointer);
    static void EditBreakpointCommandsCB   (Widget, XtPointer, XtPointer);
    static void SetBreakpointIgnoreCountNowCB(XtPointer, XtIntervalId *);
    static void DeleteInfoCB               (Widget, XtPointer, XtPointer);
    static void RefreshBreakpointsHP       (Agent *, void *, void *);
    static void RecordingHP                (Agent *, void *, void *);

    static void UpdateBreakpointButtonsCB (Widget, XtPointer, XtPointer);

    static void CheckScrollCB(Widget, XtPointer, XtPointer);
    static void CheckScrollWorkProc(XtPointer, XtIntervalId *);

    static void CheckModificationCB        (Widget, XtPointer, XtPointer);

    static void StackDialogPoppedDownCB    (Widget, XtPointer, XtPointer);
    static void CodeDialogPoppedDownCB     (Widget, XtPointer, XtPointer);
    static void RegisterDialogPoppedDownCB (Widget, XtPointer, XtPointer);
    static void ThreadDialogPoppedDownCB   (Widget, XtPointer, XtPointer);

    static void SelectFrameCB    (Widget, XtPointer, XtPointer);
    static void SelectRegisterCB (Widget, XtPointer, XtPointer);
    static void SelectThreadCB   (Widget, XtPointer, XtPointer);
    static void ThreadCommandCB  (Widget, XtPointer, XtPointer);

    static void SetWatchModeCB(Widget, XtPointer, XtPointer);
    static void ActivateGlyphCB(Widget, XtPointer, XtPointer);

    // Set shell title
    static void update_title();

    // Return height of a text line
    static int line_height(Widget text_w);

    // Create text or code widget
    static void create_text(Widget parent,
			    const string& base, bool editable,
			    Widget& form, Widget& text);

    // Refresh displays
    static void refresh_bp_disp(bool reset = false);
    static void refresh_source_bp_disp(bool reset = false);
    static void refresh_code_bp_disp(bool reset = false);

    // Clear breakpoint helpers
    static void clearBP(void *client_data, XtIntervalId *timer);
    static void clearJumpBP(const string& answer, void *client_data);

    // Move/Copy breakpoint NR to ADDRESS; return true if changed
    static bool move_bp(int nr, const string& address, Widget origin = 0,
			bool copy = false);
    static bool copy_bp(int nr, const string& address, Widget origin = 0)
    {
	return move_bp(nr, address, origin, true);
    }

    // Position history
    static void add_current_to_history();
    static void add_position_to_history(const string& file_name, 
					int line, bool stopped);

    // Set condition of breakpoints NRS to COND.
    // * If COND is char(-1), preserve old condition.
    // * If MAKE_FALSE is >= 0, disable breakpoint by making 
    //   the condition false.
    // * If MAKE_FALSE is == 0, enable breakpoint by restoring
    //   the original condition.
    // * Otherwise, preserve the condition state.
    static void _set_bps_cond(IntArray& nrs, const string& cond, 
			      int make_false, Widget origin);

    // Set condition of breakpoints NRS to COND.
    inline static void set_bps_cond(IntArray& nrs, const string& cond,
				    Widget origin = 0)
    {
	_set_bps_cond(nrs, cond, -1, origin);
    }

    // Enable and disable breakpoints via conditions.
    inline static void set_bps_cond_enabled(IntArray& nrs, bool enabled,
					    Widget origin = 0)
    {
	_set_bps_cond(nrs, char(-1), enabled ? 0 : 1, origin);
    }

    // Custom calls
    inline static void enable_bps_cond(IntArray& nrs, Widget origin = 0)
    {
	set_bps_cond_enabled(nrs, true, origin);
    }
    inline static void disable_bps_cond(IntArray& nrs, Widget origin = 0)
    {
	set_bps_cond_enabled(nrs, false, origin);
    }

    // Find the line number at POS.  LINE_NR becomes the line number
    // at POS.  IN_TEXT becomes true iff POS is in the source area.
    // BP_NR is the number of the breakpoint at POS (none: 0).  Return
    // false iff failure.
    static bool get_line_of_pos (Widget w,
				 XmTextPosition pos,
				 int& line_nr,
				 string& address,
				 bool& in_text,
				 int& bp_nr);

    // Find word around POS.  STARTPOS is the first character, ENDPOS
    // is the last character in the word.
    static void find_word_bounds (Widget w,
				  const XmTextPosition pos,
				  XmTextPosition& startpos,
				  XmTextPosition& endpos);

    //-----------------------------------------------------------------------
    // Action procedures
    //-----------------------------------------------------------------------
    static void srcpopupAct       (Widget, XEvent*, String*, Cardinal*);
    static void startSelectWordAct(Widget, XEvent*, String*, Cardinal*);
    static void endSelectWordAct  (Widget, XEvent*, String*, Cardinal*);
    static void updateGlyphsAct   (Widget, XEvent*, String*, Cardinal*);
    static void dragGlyphAct      (Widget, XEvent*, String*, Cardinal*);
    static void followGlyphAct    (Widget, XEvent*, String*, Cardinal*);
    static void dropGlyphAct      (Widget, XEvent*, String*, Cardinal*);
    static void deleteGlyphAct    (Widget, XEvent*, String*, Cardinal*);
    static void doubleClickAct    (Widget, XEvent*, String*, Cardinal*);
    static void setArgAct         (Widget, XEvent*, String*, Cardinal*);

    //-----------------------------------------------------------------------
    // Timer procedures
    //-----------------------------------------------------------------------
    static void setSelection(XtPointer client_data, XtIntervalId *timer);

    //-----------------------------------------------------------------------
    // Action decls
    //-----------------------------------------------------------------------
    static XtActionsRec actions [];

    //-----------------------------------------------------------------------
    // Menus
    //-----------------------------------------------------------------------
    static MMDesc line_popup[];
    static MMDesc address_popup[];
    static MMDesc bp_popup[];
    static MMDesc text_popup[];
    static MMDesc bp_area[];

    static void set_text_popup_label(int item, const string& arg, bool sens);
    static void set_text_popup_resource(int item, const string& arg);

    //-----------------------------------------------------------------------
    // Data
    //-----------------------------------------------------------------------
    static bool checking_scroll;

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
    static Widget int_registers_w;            // Display integer registers
    static Widget all_registers_w;            // Display all registers
    static bool register_dialog_popped_up;    // True if registers are visible

    static Widget thread_dialog_w;            // Dialog for threads
    static Widget thread_list_w;              // Thread list inside
    static bool thread_dialog_popped_up;      // True if registers are visible

    static bool display_glyphs;	              // Display glyphs?
    static bool display_line_numbers;	      // Display line numbers?
    
    static bool disassemble;	              // Disassemble code?
    static bool all_registers;	              // Show all registers?
    static bool at_lowest_frame;              // Are we at lowest frame?
    static bool signal_received;	      // Did we receive a signal?

    static WatchMode selected_watch_mode;     // Last selected watch mode

    // Tab width
    static int tab_width;

    // The breakpoint map
    static BreakPointMap bp_map;

    // File attributes
    static string current_file_name;
    static int    line_count;
    static IntIntArrayAssoc bps_in_line;
    static TextPositionArray _pos_of_line;
    static StringArray bp_addresses;
    static XmTextPosition pos_of_line(int line);

    // True iff breakpoint BP is in current file (at LINE, if given)
    static bool bp_matches(BreakPoint *bp, int line = 0);

    // True iff breakpoint BP is in FILE (at LINE, if given)
    static bool bp_matches(BreakPoint *bp, const string& file, int line = 0);

    // True iff FILE1 is equal to FILE2
    static bool file_matches(const string& file1, const string& file2);

    // True iff the base names of FILE1 and FILE2 are equal
    static bool base_matches(const string& file1, const string& file2);

    // True iff FILE is the currently loaded file
    static bool is_current_file(const string& file);

    static StringStringAssoc file_cache;
    static StringOriginAssoc origin_cache;
    static CodeCache code_cache;

    // The current source text.
    static string current_source;

    // The current frame number.  (-1 if none)
    static int current_frame;

    // The origin of the current source text.
    static SourceOrigin current_origin;

    // The current assembler code.
    static string current_code;
    static string current_code_start;
    static string current_code_end;

    // Return current source name (name under this source is known to GDB)
    static StringStringAssoc source_name_cache;
    static string current_source_name();

    // File name of current source (for JDB)
    static StringStringAssoc file_name_cache;

    // Files listed as erroneous
    static StringArray bad_files;
    static bool new_bad_file(const string& file_name);
    static void post_file_error(const string& file_name,
				string text, const _XtString name = 0,
				Widget origin = 0);
    static void post_file_warning(const string& file_name,
				  string text, const _XtString name = 0,
				  Widget origin = 0);

    // The current directory
    static string current_pwd;

    // The current JDB search path
    static string current_class_path;

    // The current JDB threadgroup.
    static string current_threadgroup;

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
    static string last_shown_pc;
    static void _show_execution_position (const string& file, int line, 
					  bool silent, bool stopped);

    // Last frame position
    static int last_frame_pos;
    static bool frame_pos_locked;

    // Read source text
    static String read_local(const string& file_name, long& length,
			     bool silent);
    static String read_remote(const string& file_name, long& length,
			      bool silent);
    static String read_class(const string& class_name, 
			     string& file_name, SourceOrigin& origin,
			     long& length, bool silent);
    static String read_from_gdb(const string& source_name, long& length,
				bool silent);

    static String read_indented(string& file_name, long& length,
				SourceOrigin& origin,
				bool silent = false);
    static int read_current(string& file_name, 
			    bool force_reload = false,
			    bool silent = false);

    // Set insertion position to POS.
    static void SetInsertionPosition(Widget w, XmTextPosition pos, 
				     bool fromTop = false);

    // Make position POS visible.
    static void ShowPosition(Widget w, XmTextPosition pos, 
			     bool fromTop = false);

    static bool is_source_widget(Widget w);
    static bool is_code_widget(Widget w);
    static string& current_text(Widget w);

    // Return current breakpoint indent amount.  If POS is given, add
    // the whitespace from POS.
    static int indent_amount(Widget w, int pos = -1);

    // Format `where' and `thread' lines
    static void setup_where_line(string& line);

    // Assembler code display routines.
    static XmTextPosition find_pc(const string& pc);
    static void refresh_codeOQC(const string& answer, void *data);
    static void set_code(const string& code,
			 const string& start,
			 const string& end);

    static MString help_on_bp(int bp, bool detailed);

    // Disassembler stuff
    // Normalize ADDRESS
    static void normalize_address(string& address);

    // Make address from ADDRESS
    static string make_address(long address);

    // Return FUNCTION and OFFSET at ADDRESS
    static void get_func_at(const string& address, string& func, int& offset);
    
    // Check if function at PC is larger than MAX_SIZE
    static bool function_is_larger_than(string pc, int max_size);

    // Breakpoint properties.
    static void update_properties_panel(BreakpointPropertiesInfo *info);
    static void update_properties_panels();
    static void move_breakpoint_properties(int old_bp, int new_bp);
    static void copy_breakpoint_properties(int old_bp, int new_bp);

    //-----------------------------------------------------------------------
    // Glyphs
    //-----------------------------------------------------------------------

    // Create a pixmap from BITS suitable for the widget W
    static Pixmap pixmap(Widget w, unsigned char *bits, int width, int height);

    // Create glyph in FORM_W named NAME from given BITS
    static Widget create_glyph(Widget form_w, const _XtString name, 
			       unsigned char *bits, int width, int height);

    // Map glyph W in (X, Y)
    static void map_glyph(Widget& w, Position x, Position y);

    // Unmap glyph W
    static void unmap_glyph(Widget w);

    // Report glyph state (for debugging)
    static void log_glyph(Widget w, int n = -1);
    static void log_glyphs();

    // Return position during glyph drag and drop
    static XmTextPosition glyph_position(Widget w, XEvent *e, 
					 bool normalize = true);

    // Get relative coordinates of GLYPH in TEXT
    static void translate_glyph_pos(Widget glyph, Widget text, int& x, int& y);

    // Data associated with current drag operation
    // The Glyph being dragged
    static Widget current_drag_origin;

    // The breakpoint being dragged, or 0 if execution position
    static int current_drag_breakpoint;

public:
    // Horizontal arrow offset (pixels)
    static int arrow_x_offset;

    // Horizontal breakpoint symbol offset (pixels)
    static int stop_x_offset;

    // Additional offset for multiple breakpoints (pixels)
    static int multiple_stop_x_offset;

private:
    // Glyph locations: X[0] is source, X[1] is code
    static Widget plain_arrows[2];
    static Widget grey_arrows[2];
    static Widget past_arrows[2];
    static Widget signal_arrows[2];
    static Widget drag_arrows[2];
    static WidgetArray plain_stops[2];
    static WidgetArray grey_stops[2];
    static WidgetArray plain_conds[2];
    static WidgetArray grey_conds[2];
    static WidgetArray plain_temps[2];
    static WidgetArray grey_temps[2];
    static Widget drag_stops[2];
    static Widget drag_conds[2];
    static Widget drag_temps[2];

    // Return position POS of glyph GLYPH in X/Y.  Return true iff displayed.
    static bool glyph_pos_to_xy(Widget glyph, XmTextPosition pos,
				Position& x, Position& y);

    // Map stop sign in W at position POS.  Get widget from STOPS[COUNT];
    // store location in POSITIONS.  Return mapped widget (0 if none)
    static Widget map_stop_at(Widget w, XmTextPosition pos,
			      WidgetArray& stops, int& count,
			      TextPositionArray& positions);

    // Map arrow/drag arrow/drag stop in W at POS.  If ORIGIN is
    // given, use colors from ORIGIN.
    static Widget map_arrow_at     (Widget w, XmTextPosition pos);
    static Widget map_drag_arrow_at(Widget w, XmTextPosition pos,
				    Widget origin = 0);
    static inline void unmap_drag_arrow(Widget w)
    {
	map_drag_arrow_at(w, XmTextPosition(-1));
    }
    static Widget map_drag_stop_at (Widget w, XmTextPosition pos,
				    Widget origin = 0);
    static inline void unmap_drag_stop(Widget w)
    {
	map_drag_stop_at(w, XmTextPosition(-1));
    }
    static void copy_colors(Widget w, Widget origin);

    // True if code/source glyphs need to be updated
    static bool update_code_glyphs;
    static bool update_source_glyphs;

    // Helping background procedures
    static void UpdateGlyphsWorkProc(XtPointer, XtIntervalId *);
    static Boolean CreateGlyphsWorkProc(XtPointer);

    // Update all glyphs now (without delay).
    static void update_glyphs_now();

    // Return all glyphs that would change
    static const WidgetArray& glyphs_to_be_updated();

    // Helpers needed by glyphs_to_be_changed
    static bool change_glyphs;
    static WidgetArray changed_glyphs;

    // Callback when state has been reset
    static void reset_done(const string& answer, void *data);

    static int max_breakpoint_number_seen;

    // Return breakpoint number of BP_INFO; 0 if new; -1 if none
    static int breakpoint_number(const string& bp_info, string& file);

    // Get numbers of selected breakpoints
    static void getBreakpointNumbers(IntArray& numbers);

public:
    // Constructor
    SourceView(Widget parent);

    // Shell constructor
    void create_shells();

    // Set current execution position, based on the GDB position info
    // POSITION; no arg means clear current position.
    // STOPPED indicates that the program just stopped.
    // SIGNALED indicates that the program received a signal.
    static void show_execution_position (const string& position = "",
					 bool stopped    = false,
					 bool signaled   = false,
					 bool silent     = false);

    // Unset current execution position (program terminated)
    static void clear_execution_position();

    // Set cursor position, based on the GDB position info POSITION
    static void show_position (string position, bool silent = false);

    // Set pc position to PC.
    // If MODE is given, highlight PC line.
    // STOPPED indicates that the program just stopped.
    // SIGNALED indicates that the program just received a signal.
    static void show_pc (const string& pc, 
			 XmHighlightMode mode = XmHIGHLIGHT_NORMAL,
			 bool stopped  = false,
			 bool signaled = false);

    // Handle breakpoint information
    static void process_info_bp         (string& info_output,
					 const string& break_arg = "");

    // Handle 'info line' information
    static void process_info_line_main  (string& info_output);

    // Handle 'where' information
    static void process_where           (string& where_output);

    // Handle 'frame' information
    static void process_frame           (string& frame_output);
    static void process_frame           (int frame_nr);

    // Handle 'info register' information
    static void process_registers       (string& info_register_output);

    // Handle 'info threads' information
    static void process_threads         (string& info_threads_output);

    // Handle `info breakpoints' information
    static void process_breakpoints     (string& info_breakpoints_output);

    // Handle 'disassemble' information
    static void process_disassemble     (const string& disassemble_output);

    // Handle 'pwd' information
    static void process_pwd             (string& pwd_output);

    // Handle 'use' information
    static void process_use             (string& use_output);

    // Process the remainder of an output line
    static void check_remainder         (string& info_output);

    // Set frame manually: ARG = 0: POS, ARG = +/- N: down/up N levels
    static void set_frame_pos(int arg, int pos = 0);

    // Set frame manually to function FUNC; return True if found
    static bool set_frame_func(const string& func);

    // Find S in current source; select it at TIME.
    enum SearchDirection {forward, backward};

    static void find(const string& s,
		     SearchDirection direction = forward,
		     bool words_only = false,
		     bool case_sensitive = false,
		     Time time = CurrentTime);

    // Locate function S; if S is omitted, locate last execution position.
    static void lookup(string s, bool silent = false);

    // Read file FILE_NAME; place cursor at INITIAL_LINE.
    static void read_file(string file_name,
			  int initial_line = 1,
			  bool force_reload = false,
			  bool silent = false);

    // Reload current file
    static void reload();

    // Return source cursor position in <source>:<line> format.
    static string line_of_cursor();

    // Return file cursor position in <file>:<line> format.
    static string file_of_cursor();

    // Callbacks for menu bar
    static void EditBreakpointsCB(Widget, XtPointer, XtPointer);
    static void ViewStackFramesCB(Widget, XtPointer, XtPointer);
    static void ViewRegistersCB(Widget, XtPointer, XtPointer);
    static void ViewThreadsCB(Widget, XtPointer, XtPointer);

    // Refreshing dialogs
    static void refresh_stack_frames();
    static void refresh_registers();
    static void refresh_threads(bool all_threadgroups);

    // Refreshing commands
    static string refresh_registers_command();

    // Check whether specific commands are required at next prompt
    static bool where_required();
    static bool register_required();
    static bool thread_required();

    // Check whether we can go up or down
    static bool can_go_up();
    static bool can_go_down();

    // Check whether source files and code are to be cached
    static bool cache_source_files;
    static bool cache_machine_code;

    // Set whether glyphs are to be displayed
    static void set_display_glyphs(bool value);

    // Set whether line numbers are to be displayed
    static void set_display_line_numbers(bool value);

    // Set whether machine code is to be displayed
    static void set_disassemble(bool value);

    // Set whether all registers are to be displayed
    static void set_all_registers(bool value);

    // Set the tab width
    static void set_tab_width(int width);

    // Set the indentation
    static void set_indent(int source_indent, int code_indent);

    // Set the max number of glyphs
    static void set_max_glyphs(int max_glyphs);

    // Whether to cache glyph images
    static bool cache_glyph_images;

    // Maximum length of expr in source popup
    static int max_popup_expr_length;

    // The indenting amounts
    static int source_indent_amount;         // Source
    static int code_indent_amount;           // Machine code
    static int line_indent_amount;           // Extra columns for line numbers
    static int script_indent_amount;         // Minimum for scripts

    // The scrolling amounts
    static int lines_above_cursor;           // Lines to keep before cursor
    static int lines_below_cursor;           // Lines to keep after cursor

    // The maximum breakpoint number when saving states
    static int max_breakpoint_number;

    // The next breakpoint number (the highest last seen + 1)
    static int next_breakpoint_number();

    // Create or clear a breakpoint at position A.  If SET, create a
    // breakpoint; if not SET, delete it.  If TEMP, make the
    // breakpoint temporary.  If COND is given, break only iff COND
    // evals to true.  ORIGIN is the origin.
    static void set_bp(const string& a, bool set, bool temp, 
		       const string& cond = "", Widget origin = 0);

    // Custom calls
    static void create_bp(const string& a, Widget origin = 0);
    static void create_temp_bp(const string& a, Widget origin = 0);
    static void clear_bp(const string& a, Widget origin = 0);

    // Create a temporary breakpoint at A and continue execution.
    static void temp_n_cont(const string& a, Widget origin = 0);

    // Enable/Disable/Delete/Edit breakpoints
    static void enable_bps     (IntArray& nrs, Widget origin = 0);
    static void disable_bps    (IntArray& nrs, Widget origin = 0);
    static void delete_bps     (IntArray& nrs, Widget origin = 0);
    static void edit_bps       (IntArray& nrs, Widget origin = 0);

    inline static void enable_bp(int nr, Widget origin = 0)
    {
	IntArray nrs;
	nrs += nr;
	enable_bps(nrs, origin);
    }

    inline static void disable_bp(int nr, Widget origin = 0)
    {
	IntArray nrs;
	nrs += nr;
	disable_bps(nrs, origin);
    }

    inline static void delete_bp(int nr, Widget origin = 0)
    {
	IntArray nrs;
	nrs += nr;
	delete_bps(nrs, origin);
    }

    inline static void edit_bp(int nr, Widget origin = 0)
    {
	IntArray nrs;
	nrs += nr;
	edit_bps(nrs, origin);
    }

    // Set breakpoint commands
    static void set_bp_commands(IntArray& nrs, const StringArray& commands,
				Widget origin = 0);
    inline static void set_bp_commands(int nr, const StringArray& commands,
				       Widget origin = 0)
    {
	IntArray nrs;
	nrs += nr;
	set_bp_commands(nrs, commands, origin);
    }

    static string numbers(const IntArray& nrs);
    static string all_numbers(const IntArray& nrs);
    static bool all_bps(const IntArray& nrs);
	    
    // Move PC to ADDRESS; return true if changed.
    static bool move_pc(const string& address, Widget origin = 0);

    // Return `clear ARG' command.  If CLEAR_NEXT is set, attempt to
    // guess the next event number and clear this one as well.
    // Consider only breakpoints whose number is >= FIRST_BP.
    static string clear_command(string arg, bool clear_next = false,
				int first_bp = 0);
    // Return `delete N' command.
    static string delete_command(int bp_nr);

    // Return `{ COMMAND; }'
    static string command_list(const string& cmd);

    // Return current directory
    static string pwd() { return current_pwd; }

    // Return current class path
    static string class_path();

    // Return current source file name
    static string name_of_source() { return current_source_name(); }

    // Return source text and machine code widget (read-only)
    static Widget source() { return source_text_w; }
    static Widget code()   { return code_text_w; }

    // Return source and machine code forms (read-only)
    static Widget source_form() { return source_form_w; }
    static Widget code_form()   { return code_form_w; }

    // Clear caches
    static void clear_file_cache();
    static void clear_code_cache();

    // Get the line at POSITION
    static string get_line(string position);

    // Get a help string for GLYPH; return 0 if none
    static MString help_on_glyph(Widget glyph, bool detailed);
    static MString help_on_pos(Widget w, XmTextPosition pos, 
			       XmTextPosition& ref, bool detailed);

    // Get the position of breakpoint NUM
    static string bp_pos(int num);

    // Return the breakpoint at POS (0 if none)
    static BreakPoint *breakpoint_at(const string& pos);

    // Return the watchpoint at EXPR (0 if none)
    static BreakPoint *watchpoint_at(const string& expr);

    // Get the word at position of EVENT
    static string get_word_at_event(Widget w,
				    XEvent *event,
				    XmTextPosition& first_pos,
				    XmTextPosition& last_pos);

    // Get the word at position POS
    static string get_word_at_pos(Widget w,
				  XmTextPosition pos,
				  XmTextPosition& startpos,
				  XmTextPosition& endpos);

    // Examine DDD state

    // True iff we have some source text
    static bool have_source() { return current_source.length() != 0; }

    // True iff we have some execution position
    static bool have_exec_pos() { return !last_execution_file.empty(); }

    // True iff we have some program counter
    static bool have_pc() { return !last_execution_pc.empty(); }

    // True iff we have some selection
    static bool have_selection();

    // True iff we need the current program counter
    static bool need_pc() { return disassemble; }

    // Return current frame (-1 if unknown)
    static int get_frame() { return current_frame; }

    // Return debugger command(s) to restore current state (breakpoints, etc.)
    static bool get_state(std::ostream& os);

    // Clear entire state, using GDB commands
    static void reset();

    // Helpers
    static string full_path(string file);

    // Edit breakpoint properties.
    static void edit_breakpoint_properties(int bp_nr);

    // Update glyphs for widget W (0: all)
    static void update_glyphs(Widget w = 0);

    // Goto history entry
    static void goto_entry(const string& file, int line,
			   const string& address, bool exec_pos);

    // Set or unset showing earlier state
    static void showing_earlier_state(bool set);
};

inline void SourceView::create_bp(const string& a, Widget w)
{
    set_bp(a, true, false, "", w);
}

inline void SourceView::create_temp_bp(const string& a, Widget w)
{
    set_bp(a, true, true, "", w);
}

inline void SourceView::clear_bp(const string& a, Widget w)
{
    set_bp(a, false, false, "", w);
}

#endif // _DDD_SourceView_h
// DON'T ADD ANYTHING BEHIND THIS #endif
