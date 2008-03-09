// $Id$
// Data Display

// Copyright (C) 1995-1997 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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

#ifndef _DDD_DataDisp_h
#define _DDD_DataDisp_h

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

// DDD includes
#include "ArgField.h"
#include "BoxSize.h"
#include "Command.h"
#include "Delay.h"
#include "ExitCB.h"
#include "GDBAgent.h"
#include "GraphEdit.h"
#include "HelpCB.h"
#include "IntArray.h"
#include "MString.h"
#include "MakeMenu.h"
#include "StringA.h"
#include "VSEFlags.h"
#include "VSLLib.h"
#include "events.h"
#include "strclass.h"
#include "string-fun.h"


//-----------------------------------------------------------------------------
extern GDBAgent *gdb;
extern ArgField *source_arg;

enum DeferMode { 
    DeferAlways,		// Defer display creation
    DeferIfNeeded,		// Defer display creation if needed
    DeferNever			// Create display now
};

class NewDisplayInfo;
class DispGraph;
class DispNode;
class DispBox;
class DispValue;
class RegionGraphNode;
class ThemeManager;

//-----------------------------------------------------------------------------
class DataDisp {

    friend class DataDispCount;

    //-----------------------------------------------------------------------
    // Callbacks
    //-----------------------------------------------------------------------
    static void dispCB                  (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Button callbacks
    //-----------------------------------------------------------------------
    static void dereferenceCB           (Widget, XtPointer, XtPointer);
    static void dereferenceArgCB        (Widget, XtPointer, XtPointer);
    static void dereferenceInPlaceCB    (Widget, XtPointer, XtPointer);
    static void toggleDetailCB          (Widget, XtPointer, XtPointer);
    static void toggleRotateCB          (Widget, XtPointer, XtPointer);
    static void toggleDisableCB         (Widget, XtPointer, XtPointer);
    static void rotateCB                (Widget, XtPointer, XtPointer);
    static void newCB                   (Widget, XtPointer, XtPointer);
    static void enableCB                (Widget, XtPointer, XtPointer);
    static void disableCB               (Widget, XtPointer, XtPointer);
    static void showDetailCB            (Widget, XtPointer, XtPointer);
    static void showMoreDetailCB        (Widget, XtPointer, XtPointer);
    static void hideDetailCB            (Widget, XtPointer, XtPointer);
    static void dependentCB             (Widget, XtPointer, XtPointer);
    static void displayArgCB            (Widget, XtPointer, XtPointer);
    static void plotArgCB               (Widget, XtPointer, XtPointer);
    static void plotHistoryCB           (Widget, XtPointer, XtPointer);
    static void setCB                   (Widget, XtPointer, XtPointer);
    static void shortcutCB              (Widget, XtPointer, XtPointer);
    static void deleteArgCB		(Widget, XtPointer, XtPointer);
    static void clusterSelectedCB       (Widget, XtPointer, XtPointer);
    static void unclusterSelectedCB     (Widget, XtPointer, XtPointer);
    static void toggleClusterSelectedCB (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Popup menu callbacks
    //-----------------------------------------------------------------------
    static void popup_new_argCB         (Widget, XtPointer, XtPointer);
    static void popup_newCB             (Widget, XtPointer, XtPointer);
    static void new_displayDCB          (Widget, XtPointer, XtPointer);

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
    static void new_displayCD (Widget w, const BoxPoint& box_point = BoxPoint());

    //-----------------------------------------------------------------------
    // Set sensitivity
    //-----------------------------------------------------------------------
    static void no_displaysHP (void*, void*, void*);

    //-----------------------------------------------------------------------
    // Helpers
    //-----------------------------------------------------------------------

    static bool lose_selection;

    static void set_args(const BoxPoint& p = BoxPoint(),
			 SelectionMode mode = SetSelection);

    static void refresh_display_list(bool silent = false);
    static void RefreshDisplayListCB(XtPointer, XtIntervalId *id);

    static DispValue *selected_value();
    static DispNode  *selected_node();
    static string selected_pattern();
    static string pattern(const string& expr, bool shorten = true);

    static void open_data_window();
    static void close_data_window();

    static DispNode *new_data_node(const string& name, const string& scope,
				   const string& answer, bool plotted);
    static DispNode *new_user_node(const string& name, const string& scope,
				   const string& answer, bool plotted);
    static DispNode *new_deferred_node(const string& expr, const string& scope,
				       const BoxPoint& pos,
				       const string& depends_on,
				       bool clustered, bool plotted);

    static int getDispNrAtPoint(const BoxPoint& point);

    static string numbers(IntArray& a);
    static bool all_data_displays(IntArray& numbers);

    static void show(Widget dialog, int depth, int more);

    static Widget create_display_dialog(Widget parent, const _XtString name,
					NewDisplayInfo& info);

    static void rotate_value(DispValue *dv, bool all = false);
    static void rotate_node(DispNode *dn, bool all = false);

    static void select_node(DispNode *dn, int src = 0);

    // Setting values
    static void setDCB (Widget, XtPointer, XtPointer);
    static void DeleteSetInfoCB(Widget, XtPointer client_data, XtPointer);
    static void SetDone(const string& answer, void *qu_data);

    // Builtin user displays
    static bool is_builtin_user_command(const string& cmd);
    static string builtin_user_command(const string& cmd, DispNode *dn = 0);
    static void refresh_builtin_user_displays();

    // DispNode properties
    static bool needs_refresh(DispNode *dn);
    static bool selected(DispNode *dn);

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

    static void call_selection_proc(Widget, const _XtString, XEvent*, String*, Cardinal,
				    SelectionMode mode);

    //-----------------------------------------------------------------------
    // Menus
    //-----------------------------------------------------------------------
    static MMDesc graph_popup[];
    static MMDesc node_popup[];
    static MMDesc shortcut_popup1[];
    static MMDesc shortcut_popup2[];
    static MMDesc display_area[];
    static MMDesc plot_menu[];
    static MMDesc detail_menu[];
    static MMDesc shortcut_menu[];
    static MMDesc rotate_menu[];
    static MMDesc delete_menu[];
    static MMDesc theme_menu[];

    static const int shortcut_items;
    static const int theme_items;

    // Array of shortcut expressions
    static StringArray shortcut_exprs;
    static StringArray shortcut_labels;

    static void add_shortcut_expr(const string& expr);

public:
    // Menus - public such that we can chain them in a toolbar
    static MMDesc graph_cmd_area[];

    //-----------------------------------------------------------------------
    // Send queries to GDB and process answers
    //-----------------------------------------------------------------------

    // Create a new display for DISPLAY_EXPRESSION.
    // SCOPE is the name of the current function (must match backtrace
    // output) or a scope to be reached; see DEFERRED, below.
    // If POS is set, the new display is created at this position.
    // If DEPENDS_ON is set, the new display is made dependent on
    // DEPENDS_ON (a display number or name).
    // If DEFERRED is set, defer creation until SCOPE is reached.
    // If CLUSTERED is set, cluster display.
    // If PLOTTED is set, plot display.
    // If ORIGIN is set, the last origin is set to ORIGIN.
    static void new_displaySQ(const string& display_expression,
			      const string& scope,
			      BoxPoint *pos = 0,
			      const string& depends_on = "",
			      DeferMode deferred = DeferAlways,
			      bool clustered = false,
			      bool plotted = false,
			      Widget origin = 0,
			      bool verbose = true,
			      bool prompt = true);

    // Refresh displays.  Sends `info display' and `display' to GDB.
    static void refresh_displaySQ(Widget origin = 0, 
				  bool verbose = true,
				  bool prompt = true);

    // Disable displays given in DISPLAY_NRS.  Sends `disable display' to GDB.
    static void disable_displaySQ(IntArray& display_nrs,
				  bool verbose = true,
				  bool prompt = true);

    // Enable displays given in DISPLAY_NRS.  Sends `enable display' to GDB.
    static void enable_displaySQ(IntArray& display_nrs,
				 bool verbose = true,
				 bool prompt = true);

    // Delete displays given in DISPLAY_NRS.  Sends `delete display' to GDB.
    static void delete_displaySQ(IntArray& display_nrs,
				 bool verbose = true,
				 bool prompt = true);

    // Apply THEME on PATTERN.
    static void apply_themeSQ(const string& theme,
			      const string& pattern,
			      bool verbose = true,
			      bool prompt = true);

    // Unapply THEME on PATTERN.
    static void unapply_themeSQ(const string& theme,
				const string& pattern,
				bool verbose = true,
				bool prompt = true);

    // Toggle THEME on PATTERN.
    static void toggle_themeSQ(const string& theme,
			       const string& pattern,
			       bool verbose = true,
			       bool prompt = true);

    // Same, but return the appropriate command
    static string new_display_cmd(const string& display_expression,
				  BoxPoint *pos = 0,
				  const string& depends_on = "",
				  bool clustered = false,
				  bool plotted = false);

    static string refresh_display_cmd();
    static string disable_display_cmd(IntArray& display_nrs);
    static string enable_display_cmd(IntArray& display_nrs);
    static string delete_display_cmd(IntArray& display_nrs);
    static string delete_display_cmd(const string& name);
    static string apply_theme_cmd(const string& theme, 
				  const string& pattern);
    static string unapply_theme_cmd(const string& theme, 
				    const string& pattern);
    static string toggle_theme_cmd(const string& theme, 
				   const string& pattern);

    // Same, but use the GDB_COMMAND interface for enqueing commands
    static void new_display(const string& display_expression,
			    BoxPoint *pos = 0,
			    string depends_on = "",
			    bool clustered = false,
			    bool plotted = false,
			    Widget origin = 0)
    {
	string c = new_display_cmd(display_expression, pos, 
				   depends_on, clustered, plotted);
	gdb_command(c, origin);
    }

    static void refresh_display(Widget origin = 0)
    {
	gdb_command(refresh_display_cmd(), origin);
    }

    static void disable_display(IntArray& display_nrs, Widget origin = 0)
    {
	if (display_nrs.size() > 0)
	    gdb_command(disable_display_cmd(display_nrs), origin);
    }

    static void enable_display(IntArray& display_nrs, Widget origin = 0)
    {
	if (display_nrs.size() > 0)
	    gdb_command(enable_display_cmd(display_nrs), origin);
    }

    static void delete_display(IntArray& display_nrs, Widget origin = 0)
    {
	if (display_nrs.size() > 0)
	    gdb_command(delete_display_cmd(display_nrs), origin);
    }

    static void delete_display(const string& name, Widget origin = 0)
    {
	gdb_command(delete_display_cmd(name), origin);
    }

    static void apply_theme(const string& theme, const string& pattern, 
			    Widget origin = 0)
    {
	gdb_command(apply_theme_cmd(theme, pattern), origin);
    }

    static void unapply_theme(const string& theme, const string& pattern, 
			      Widget origin = 0)
    {
	gdb_command(unapply_theme_cmd(theme, pattern), origin);
    }

    static void toggle_theme(const string& theme, const string& pattern, 
			     Widget origin = 0)
    {
	gdb_command(toggle_theme_cmd(theme, pattern), origin);
    }

    // Process 'info display' output in INFO_DISPLAY_ANSWER.  If
    // DEFER_DELETED is set, defer displays that have been deleted.
    static void process_info_display (string& info_display_answer, 
				      bool defer_deleted = true);

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

    // Process current scope
    static void process_scope(const string& scope);
    static bool need_scope();

    // Set shortcut menu to expressions EXPRS
    static void set_shortcut_menu(const StringArray& exprs,
				  const StringArray& labels);

    // Return current shortcut menu items
    static void get_shortcut_menu(StringArray& exprs,
				  StringArray& labels);

    // Refresh argument buttons
    static void refresh_args(bool update_arg = false);

    // Set theme manager and refresh display
    static void set_theme_manager(const ThemeManager& t);

private:
    // Call me back again
    static void again_new_displaySQ (XtPointer client_data, XtIntervalId *id);

    // Tons of helpers
    static void new_data_displayOQC      (const string& answer, void* data);
    static void new_data_display_extraOQC(const string& answer, void* data);

    static void new_user_displayOQC  (const string& answer, void* data);

    static void refresh_displayOQC   (const string& answer, void*  data);
    static void refresh_displayOQAC  (StringArray& answers,
				      const VoidArray& qu_datas,
				      void*  data);

    static void disable_displayOQC   (const string& answer, void* data);
    static void enable_displayOQC    (const string& answer, void* data);
    static void delete_displayOQC    (const string& answer, void* data);

    static void deletion_done(IntArray& display_nrs, bool do_prompt);

    static void add_aliases(IntArray& a);
    static bool sort_and_check(IntArray& a);

    static void select_with_all_descendants(GraphNode *node);
    static void select_with_all_ancestors(GraphNode *node);

    static int unfold_expressions(const string& display_expression,
				  StringArray& expressions);

    // Get display number and name from ANSWER; store them in NR and NAME
    static void read_number_and_name(string& answer, string& nr, string& name);

    //-----------------------------------------------------------------------
    // Data
    //-----------------------------------------------------------------------
    static DispGraph *disp_graph;
    static Delay *delay;
    static Widget last_origin;
    static Time last_select_time;
    static int next_ddd_display_number;
    static int next_gdb_display_number;
    static bool detect_aliases;
    static bool cluster_displays;
    static bool arg_needs_update;

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
    static void write_frame_command(std::ostream& os, int& current_frame, 
				   int target_frame);
    static void write_restore_scope_command(std::ostream& os,
					    int& current_frame,
					    const StringArray& scopes,
					    DispNode *dn,
					    const bool& ok);

    static bool get_state(std::ostream& os,
			  bool restore_state,
			  bool include_position,
			  const StringArray& scopes,
			  int target_frame);

    static void get_node_state(std::ostream& os, DispNode *dn,
			       bool include_position);

    // Reset done
    static void reset_done(const string& answer, void *data);

    // Clustering stuff
    static void insert_data_node(DispNode *dn, int depend_nr,
				 bool clustered, bool plotted);
    static int new_cluster(const string& name = "", bool plotted = false);
    static int current_cluster();

    static DispValue *update_hook(string& value);

public:
    static Widget graph_edit;
    static Widget graph_cmd_w;
    static Widget graph_selection_w;
    static ArgField *graph_arg;
    static bool bump_displays;

    // Constructor
    DataDisp(Widget parent, Widget& data_buttons_w);

    void create_shells();

    static int count_data_displays();
    static void get_all_display_numbers(IntArray& numbers);
    static void get_all_clusters(IntArray& numbers);
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
    static void applyThemeCB(Widget, XtPointer, XtPointer);
    static void unapplyThemeCB(Widget, XtPointer, XtPointer);
    static void toggleThemeCB(Widget, XtPointer, XtPointer);
    static void applyThemeOnThisCB(Widget, XtPointer, XtPointer);
    static void applyThemeOnAllCB(Widget, XtPointer, XtPointer);

    // Helpers for user displays
    static bool have_user_display(const string& name);
    static void new_user_display(const string& name);
    static void delete_user_display(const string& name);

    // Callbacks for language changes
    static void language_changedHP(Agent *source, void *client_data, 
				   void *call_data);

    // Set whether aliases are to be detected
    static void set_detect_aliases(bool value);

    // Set whether displays are to be clustered
    static void set_cluster_displays(bool value);

    // True iff we have some selection
    static bool have_selection();

    // Write current selection as DDD commands to OS
    // Return true iff all went well
    static bool get_selection(std::ostream& os, bool include_position = false);

    // The maximum display number when saving states
    static int max_display_number;

    // Return DDD commands to restore current state (displays, etc.)
    // Return true iff all went well.  SCOPES are obtained via
    // GET_SCOPES (see below).  TARGET_FRAME indicates final frame.
    static bool get_state(std::ostream& os, const StringArray& scopes, 
			  int target_frame = 0);
    static bool get_scopes(StringArray& scopes);

    // Return true if a core dump is needed to restore displays
    static bool need_core_to_restore();

    // Return documentation string for shortcut menu item W. 
    // Returns 0 if W is no shortcut menu item.
    static MString shortcut_help(Widget w);

    // Select a specific value.  Plotter uses this.
    static void select(DispValue *dv);

    // Reset all
    static void reset();

    // True if the selection was lost
    static void SelectionLostCB(Widget = 0, XtPointer = 0, XtPointer = 0);

    // Get number(s) of display NAME
    static int display_number(const string& name, bool verbose = false);
    static void get_display_numbers(const string& name, IntArray& numbers);

    // Refresh titles after change in APP_DATA
    static void refresh_titles();

    // Update and activate displays manually (from undo)
    static void update_displays(const StringArray& names, 
				const StringArray& values,
				const StringArray& addrs);

    // Restore sane state
    static void make_sane();

private:
    DataDisp(const DataDisp&);
    DataDisp& operator=(const DataDisp&);
};

inline bool DataDisp::get_selection(std::ostream& os, bool include_position)
{
    StringArray dummy;
    return get_state(os, false, include_position, dummy, 0);
}

inline bool DataDisp::get_state(std::ostream& os, const StringArray& scopes, 
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
