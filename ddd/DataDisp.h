// $Id$
// Data Display

// Copyright (C) 1995-1997 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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

#ifndef _DDD_DataDisp_h
#define _DDD_DataDisp_h

#ifdef __GNUG__
#pragma interface
#endif

//-----------------------------------------------------------------------------
// The `DataDisp' class creates the data display and provides the
// necessary callback functions.
//
// Name conventions used:
// ...CB  : Callback function.
// ...DCB : Dialog callback function.
// ...CD  : Create dialog.
// ...Act : Action function.
// ...SQ  : calls gdb->send_question().
// ...OQC : needed in on_question_completion; see `GDBAgent.h'.
// ...OQAC: needed in on_question_array_completion; see `GDBAgent.h'.
// ...HP  : HandlerProc.  See `HandlerL.h'
// 
//-----------------------------------------------------------------------------

// Motif includes
#include <Xm/Xm.h>

// Misc includes
#include "strclass.h"
#include "MakeMenu.h"
#include "HelpCB.h"
#include "ExitCB.h"
#include "events.h"
#include "VSEFlags.h"
#include "VSLLib.h"
#include "Delay.h"
#include "IntArray.h"
#include "MString.h"
#include "DispNode.h"
#include "string-fun.h"

// DDD includes
#include "ArgField.h"
#include "GDBAgent.h"
#include "GraphEdit.h"
#include "DispBox.h"
#include "StringA.h"


//-----------------------------------------------------------------------------
extern GDBAgent *gdb;
extern ArgField *source_arg;

//-----------------------------------------------------------------------------
class DataDisp {

    //-----------------------------------------------------------------------
    // Callbacks
    //-----------------------------------------------------------------------
    static void dispCB                  (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Button callbacks
    //-----------------------------------------------------------------------
    static void dereferenceCB           (Widget, XtPointer, XtPointer);
    static void toggleDetailCB          (Widget, XtPointer, XtPointer);
    static void toggleRotateCB          (Widget, XtPointer, XtPointer);
    static void toggleDisableCB         (Widget, XtPointer, XtPointer);
    static void rotateCB                (Widget, XtPointer, XtPointer);
    static void rotateAllCB             (Widget, XtPointer, XtPointer);
    static void newCB                   (Widget, XtPointer, XtPointer);
    static void enableCB                (Widget, XtPointer, XtPointer);
    static void disableCB               (Widget, XtPointer, XtPointer);
    static void showDetailCB            (Widget, XtPointer, XtPointer);
    static void showMoreDetailCB        (Widget, XtPointer, XtPointer);
    static void hideDetailCB            (Widget, XtPointer, XtPointer);
    static void dependentCB             (Widget, XtPointer, XtPointer);
    static void setCB                   (Widget, XtPointer, XtPointer);
    static void shortcutCB              (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Popup menu callbacks
    //-----------------------------------------------------------------------
    static void popup_new_argCB         (Widget, XtPointer, XtPointer);
    static void popup_newCB             (Widget, XtPointer, XtPointer);
    static void new_displayDCB          (Widget, XtPointer, XtPointer);
    static void setDCB                  (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Callbacks of the display editor
    //-----------------------------------------------------------------------
    static void UpdateGraphEditorSelectionCB   (Widget, XtPointer, XtPointer);
    static void UpdateDisplayEditorSelectionCB (Widget, XtPointer, XtPointer);
    static void PreLayoutCB                    (Widget, XtPointer, XtPointer);
    static void PostLayoutCB                   (Widget, XtPointer, XtPointer);
    static void DoubleClickCB                  (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Graph callbacks
    //-----------------------------------------------------------------------
    static bool bump(RegionGraphNode *node, const BoxSize& newSize);

    //-----------------------------------------------------------------------
    // Timers and timer callbacks
    //-----------------------------------------------------------------------
    static void RefreshGraphEditCB(XtPointer client_data, XtIntervalId *id);
    static void RefreshArgsCB     (XtPointer client_data, XtIntervalId *id);
    static void RefreshAddrCB     (XtPointer client_data, XtIntervalId *id);

    static XtIntervalId refresh_args_timer;
    static XtIntervalId refresh_graph_edit_timer;
    static XtIntervalId refresh_addr_timer;

    //-----------------------------------------------------------------------
    // Sorting nodes for layout
    //-----------------------------------------------------------------------
    static void CompareNodesCB (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Create dialogs
    //-----------------------------------------------------------------------
    static void new_displayCD (Widget w, BoxPoint box_point = BoxPoint());

    //-----------------------------------------------------------------------
    // Set sensitivity
    //-----------------------------------------------------------------------
    static void no_displaysHP (void*, void*, void*);

    //-----------------------------------------------------------------------
    // Helpers
    //-----------------------------------------------------------------------

    static bool lose_selection;
    static void SelectionLostCB(Widget, XtPointer, XtPointer);

    static void set_args(BoxPoint p = BoxPoint(),
			 SelectionMode mode = SetSelection);

    static void refresh_args();
    static void refresh_display_list(bool silent = false);

    static DispValue *selected_value();
    static DispNode  *selected_node();

    static DispNode *new_data_node(const string& name, const string& scope,
				   const string& answer);
    static DispNode *new_user_node(const string& name, const string& scope,
				   const string& answer);

    static int getDispNrAtPoint (BoxPoint point);

    static string numbers(IntArray& a);

    static void show(Widget dialog, int depth, int more);

    static Widget create_display_dialog(Widget parent, String name,
				      class NewDisplayInfo& info);

    static void toggle_rotate(DispValue *dv, bool all = false);


    //-----------------------------------------------------------------------
    // Actions
    //-----------------------------------------------------------------------
    static XtActionsRec actions [];
    static void graph_selectAct         (Widget, XEvent*, String*, Cardinal*);
    static void graph_select_or_moveAct (Widget, XEvent*, String*, Cardinal*);
    static void graph_extendAct         (Widget, XEvent*, String*, Cardinal*);
    static void graph_extend_or_moveAct (Widget, XEvent*, String*, Cardinal*);
    static void graph_toggleAct         (Widget, XEvent*, String*, Cardinal*);
    static void graph_toggle_or_moveAct (Widget, XEvent*, String*, Cardinal*);
    static void graph_popupAct          (Widget, XEvent*, String*, Cardinal*);
    static void graph_dereferenceAct    (Widget, XEvent*, String*, Cardinal*);
    static void graph_detailAct         (Widget, XEvent*, String*, Cardinal*);
    static void graph_rotateAct         (Widget, XEvent*, String*, Cardinal*);
    static void graph_dependentAct      (Widget, XEvent*, String*, Cardinal*);

    static void call_selection_proc(Widget, String, XEvent*, String*, Cardinal,
				    SelectionMode mode);

    //-----------------------------------------------------------------------
    // Menus
    //-----------------------------------------------------------------------
    static MMDesc graph_popup[];
    static MMDesc node_popup[];
    static MMDesc shortcut_popup1[];
    static MMDesc shortcut_popup2[];
    static MMDesc graph_cmd_area[];
    static MMDesc display_area[];
    static MMDesc detail_menu[];
    static MMDesc shortcut_menu[];
    static MMDesc rotate_menu[];

    static const int DataDisp::shortcut_items;


    // Array of shortcut expressions
    static StringArray shortcut_exprs;
    static StringArray shortcut_labels;

    static void add_shortcut_expr(const string& expr);

public:
    //-----------------------------------------------------------------------
    // Send queries to GDB and process answers
    //-----------------------------------------------------------------------

    // Create a new display for DISPLAY_EXPRESSION.
    // SCOPE is the name of the current function (must match backtrace output)
    // If POS is set, the new display is created at this position.
    // If DEPENDS_ON is set, the new display is made dependent on
    // DEPENDS_ON (a display number or name)
    // If ORIGIN is set, the last origin is set to ORIGIN.
    static void new_displaySQ(string display_expression,
			      string scope,
			      BoxPoint *pos = 0,
			      string depends_on = "",
			      Widget origin = 0,
			      bool verbose = true);

    // Refresh displays.  Sends `info display' and `display' to GDB.
    static void refresh_displaySQ(Widget origin = 0, bool verbose = true);

    // Disable displays given in DISPLAY_NRS.  Sends `disable display' to GDB.
    static void disable_displaySQ(IntArray& display_nrs, bool verbose = true);

    // Enable displays given in DISPLAY_NRS.  Sends `enable display' to GDB.
    static void enable_displaySQ(IntArray& display_nrs, bool verbose = true);

    // Delete displays given in DISPLAY_NRS.  Sends `delete display' to GDB.
    static void delete_displaySQ(IntArray& display_nrs, bool verbose = true);

    // Same, but use the GDB_COMMAND interface for enqueing commands
    static void new_display(string display_expression,
			    BoxPoint *pos = 0,
			    string depends_on = "",
			    Widget origin = 0);

    static void refresh_display(Widget origin = 0);
    static void disable_display(IntArray& display_nrs);
    static void enable_display(IntArray& display_nrs);
    static void delete_display(IntArray& display_nrs);

    // Process 'info display' output in INFO_DISPLAY_ANSWER.  Deletes
    // displays if needed.
    static void process_info_display (string& info_display_answer);

    // Process `display' output in DISPLAY_ANSWER.  Remaining
    // (non-display) output is returned.  If DISABLING_OCCURRED is set
    // after the call, some displays have been disabled and a
    // `display' command must be re-sent to GDB.
    static string process_displays(string& display_answer,
				   bool& disabling_occurred);

    // Process user-defined command output in ANSWERS.
    static void process_user(StringArray& answers);

    // Process data output in ANSWERS.
    static void process_addr(StringArray& answers);

    // Set shortcut menu to expressions EXPRS
    static void set_shortcut_menu(const StringArray& exprs,
				  const StringArray& labels);

    // Return current shortcut menu items
    static void get_shortcut_menu(StringArray& exprs,
				  StringArray& labels);

private:
    // Call me back again
    static void again_new_displaySQ (XtPointer client_data, XtIntervalId *id);

    // Tons of helpers
    static void new_data_displayOQC      (const string& answer, void* data);
    static void new_data_display_extraOQC(const string& answer, void* data);
    static void new_data_displaysSQA     (string display_expression, 
					  void *data);
    static void new_data_displaysOQAC    (string answers[], 
					  void*  qu_datas[],
					  int    count, 
					  void*  data);

    static void new_user_displayOQC  (const string& answer, void* data);

    static void refresh_displayOQC   (const string& answer, void*  data);
    static void refresh_displayOQAC  (string answers[],
				      void*  qu_datas[],
				      int    count,
				      void*  data);

    static void disable_displayOQC   (const string& answer, void* data);
    static void enable_displayOQC    (const string& answer, void* data);
    static void delete_displayOQC    (const string& answer, void* data);

    static void sort_and_check(IntArray& a);

    static void select_with_all_descendants(GraphNode *node);
    static void select_with_all_ancestors(GraphNode *node);

    // Get display number and name from ANSWER; store them in NR and NAME
    static void read_number_and_name(string& answer, string& nr, string& name);

    //-----------------------------------------------------------------------
    // Data
    //-----------------------------------------------------------------------
    static class DispGraph *disp_graph;
    static Delay *delay;
    static Widget last_origin;
    static Time last_select_time;
    static int next_display_number;
    static bool detect_aliases;

    static Widget graph_popup_w;
    static Widget node_popup_w;
    static Widget shortcut_popup_w;

    static Widget edit_displays_dialog_w;
    static Widget display_list_w;

    // Origin handling
    static void ClearOriginCB(Widget, XtPointer, XtPointer);
    static void set_last_origin(Widget origin);

    // Alias checking
    static bool check_aliases();
    static void sort_last_change(IntArray& disp_nrs);
    static int last_change_of_disp_nr(int disp_nr);

    // True iff aliases are to be checked regardless of address changes
    static bool force_check_aliases;

    // Merge displays in DISPLAYS.  Set CHANGED iff changed.  Set
    // SUPPRESSED if displays were suppressed.
    static void merge_displays(IntArray displays, 
			       bool& changed, bool& suppressed);

    // Unmerge display DISP_NR; return true iff change
    static bool unmerge_display(int disp_nr);

    static Widget graph_form_w;

    static int alias_display_nr(GraphNode *node);

    // State stuff
    static void write_frame_command(ostream& os, int& current_frame, 
				   int target_frame);
    static void write_restore_scope_command(ostream& os,
					    int& current_frame,
					    const StringArray& scopes,
					    DispNode *dn,
					    bool& ok);

    static bool get_state(ostream& os,
			  bool restore_state,
			  bool include_position,
			  const StringArray& scopes,
			  int target_frame);

    // Reset done
    static void reset_done(const string& answer, void *data);

public:
    static Widget graph_edit;
    static Widget graph_cmd_w;
    static Widget graph_selection_w;
    static ArgField *graph_arg;
    static bool bump_displays;

    // Constructor
    DataDisp (XtAppContext app_context,
	      Widget       parent,
	      String       vsl_path,         // from app_data
	      String       vsl_library,      // from app_data
	      String       vsl_defs,         // from app_data
	      bool         panned);          // from app_data

    int count_data_displays() const;
    static void refresh_graph_edit (bool silent = false);
    static Widget graph_form() { return graph_form_w; }

    // Refresh address of NODE (0: all nodes)
    static void refresh_addr(DispNode *node = 0);

    // Command(s) to re-print all displays; return # of commands
    static int add_refresh_data_commands(StringArray& cmds);
    static int add_refresh_user_commands(StringArray& cmds);
    static int add_refresh_addr_commands(StringArray& cmds, DispNode *dn = 0);

    // Callbacks for menu bar
    static void EditDisplaysCB(Widget, XtPointer, XtPointer);
    static void refreshCB(Widget, XtPointer, XtPointer);
    static void selectAllCB(Widget, XtPointer, XtPointer);
    static void unselectAllCB(Widget, XtPointer, XtPointer);
    static void deleteCB(Widget, XtPointer, XtPointer);

    // Helpers for user displays
    static bool have_user_display(const string& name);
    static void new_user_display(const string& name);
    static void delete_user_display(const string& name);

    // Callbacks for language changes
    static void language_changedHP(Agent *source, void *client_data, 
				   void *call_data);

    // Set whether aliases are to be detected
    static void set_detect_aliases(bool value);

    // True iff we have some selection
    static bool have_selection();

    // Write current selection as DDD commands to OS
    // Return true iff all went well
    static bool get_selection(ostream& os, bool include_position = false);

    // The maximum display number when saving states
    static int max_display_number;

    // Return DDD commands to restore current state (displays, etc.)
    // Return true iff all went well.  SCOPES are obtained via
    // GET_SCOPES (see below).  TARGET_FRAME indicates final frame.
    static bool get_state(ostream& os, const StringArray& scopes, 
			  int target_frame = 0);
    static bool get_scopes(StringArray& scopes);

    // Return true if a core dump is needed to restore displays
    static bool need_core_to_restore();

    // Return documentation string for shortcut menu item W. 
    // Returns 0 if W is no shortcut menu item.
    static MString shortcut_help(Widget w);

    // Reset all
    static void reset();
};

inline bool DataDisp::get_selection(ostream& os, bool include_position)
{
    StringArray dummy;
    return get_state(os, false, include_position, dummy, 0);
}

inline bool DataDisp::get_state(ostream& os, const StringArray& scopes, 
				int target_frame)
{
    return get_state(os, true, true, scopes, target_frame);
}

inline void DataDisp::get_shortcut_menu(StringArray& exprs,
					StringArray& labels)
{
    exprs  = shortcut_exprs;
    labels = shortcut_labels;
}

#endif // _DDD_DataDisp_h
// DON'T ADD ANYTHING BEHIND THIS #endif
