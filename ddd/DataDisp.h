// $Id$
// Data Display

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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
// ...SQA : calls gdb->send_qu_array().
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

// DDD includes
#include "ArgField.h"
#include "GDBAgent.h"
#include "DispGraph.h"
#include "GraphEdit.h"
#include "DispBox.h"
#include "DispBox.h"
#include "StringA.h"


//-----------------------------------------------------------------------------
extern GDBAgent* gdb;
extern ArgField* source_arg;

//-----------------------------------------------------------------------------
class DataDisp {

    //-----------------------------------------------------------------------
    // Callbacks
    //-----------------------------------------------------------------------
    static void dispCB                  (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Button Callbacks
    //-----------------------------------------------------------------------
    static void dereferenceCB           (Widget, XtPointer, XtPointer);
    static void toggleDetailCB          (Widget, XtPointer, XtPointer);
    static void toggleRotateCB          (Widget, XtPointer, XtPointer);
    static void toggleDisableCB         (Widget, XtPointer, XtPointer);
    static void newCB                   (Widget, XtPointer, XtPointer);
    static void enableCB                (Widget, XtPointer, XtPointer);
    static void disableCB               (Widget, XtPointer, XtPointer);
    static void showDetailCB            (Widget, XtPointer, XtPointer);
    static void hideDetailCB            (Widget, XtPointer, XtPointer);
    static void dependentCB             (Widget, XtPointer, XtPointer);
    static void setCB                   (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Popup menu callbacks
    //-----------------------------------------------------------------------
    static void popup_new_argCB         (Widget, XtPointer, XtPointer);
    static void popup_newCB             (Widget, XtPointer, XtPointer);
    static void dependent_displayDCB    (Widget, XtPointer, XtPointer);
    static void new_displayDCB          (Widget, XtPointer, XtPointer);
    static void setDCB                  (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Callbacks of the display editor
    //-----------------------------------------------------------------------
    static void UpdateGraphEditorSelectionCB    (Widget, XtPointer, XtPointer);
    static void UpdateDisplayEditorSelectionCB  (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Sorting nodes for layout
    //-----------------------------------------------------------------------
    static void CompareNodesCB (Widget, XtPointer, XtPointer);

    //-----------------------------------------------------------------------
    // Create dialogs
    //-----------------------------------------------------------------------
    static void new_displayCD (BoxPoint box_point = BoxPoint(-1,-1));

    //-----------------------------------------------------------------------
    // Set sensitivity
    //-----------------------------------------------------------------------
    static void no_displaysHP (void*, void*, void*);

    //-----------------------------------------------------------------------
    // Helpers
    //-----------------------------------------------------------------------

    static void source_argHP (void*, void*, void*);
    static void graph_argHP (void*, void*, void*);
    static void graph_unselectHP (void*, void*, void*);
    static void gdb_ready_for_questionHP (Agent *, void*, void*);

    static void set_args(BoxPoint p = BoxPoint(-1, -1),
			 SelectionMode mode = SetSelection);
    static void refresh_args();
    static void refresh_display_list(bool silent = false);

    static DispValue *selected_value();
    static DispNode  *selected_node();

    static DispNode  *new_data_node(string& answer);
    static DispNode  *new_user_node(const string& name, string& answer);

public:
    static void set_delay ();
    static void unset_delay ();

    static void set_handlers();

private:
    static inline int getDispNrAtPoint (BoxPoint point);

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
    static MMDesc graph_cmd_area[];
    static MMDesc display_area[];


public:
    //-----------------------------------------------------------------------
    // Send queries to GDB and process answers
    //-----------------------------------------------------------------------

    // Create a new display for DISPLAY_EXPRESSION.  Sends a `display'
    // command to GDB.  If POS is set, the new display is created at
    // this position.
    static void new_displaySQ       (string display_expression,
				     BoxPoint* pos = 0,
				     Widget origin = 0);

    // Refresh displays.  Sends `info display' and `display' to GDB.
    static void refresh_displaySQ   (Widget origin = 0);

    // Disable displays given in DISPLAY_NRS.  Sends `disable display' to GDB.
    static void disable_displaySQ   (const IntArray& display_nrs);

    // Enable displays given in DISPLAY_NRS.  Sends `enable display' to GDB.
    static void enable_displaySQ    (const IntArray& display_nrs);

    // Delete displays given in DISPLAY_NRS.  Sends `delete display' to GDB.
    static void delete_displaySQ    (const IntArray& display_nrs);

    // Create a new display for DISPLAY_EXPRESSION dependent on
    // DISP_NR.  Sends a `display' command to GDB.
    static void dependent_displaySQ (string display_expression, int disp_nr);

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

private:
    // Call me back again
    static void again_new_displaySQ (XtPointer client_data, XtIntervalId *id);

    // Tons of helpers
    static void new_displayOQC       (const string& answer, void* data);
    static void new_display_extraOQC (const string& answer, void* data);
    static void new_displaysSQA      (string display_expression, BoxPoint* p);
    static void new_displaysOQAC     (string answers[], 
				      void*  qu_datas[],
				      int    count, 
				      void*  data);

    static void new_userOQC          (const string& answer, void* data);

    static void dependent_displayOQC       (const string& answer, void* data);
    static void dependent_display_extraOQC (const string& answer, void* data);
    static void dependent_displaysSQA      (string display_expression, 
					    int disp_nr);
    static void dependent_displaysOQAC     (string answers[],
					    void*  qu_datas[],
					    int    count,
					    void*  data);

    static void refresh_displayOQC   (const string& answer, void*  data);
    static void refresh_displayOQAC  (string answers[],
				      void*  qu_datas[],
				      int    count,
				      void*  data);

    static void disable_displayOQC   (const string& answer, void* data);
    static void enable_displayOQC    (const string& answer, void* data);
    static void delete_displayOQC    (const string& answer, void* data);

    //-----------------------------------------------------------------------
    // Data
    //-----------------------------------------------------------------------
    static DispGraph *disp_graph;
    static Delay *delay;
    static Widget last_origin;
    static Time last_select_time;
    static int next_display_number;
    static bool detect_aliases;

    static Widget graph_popup_w;
    static Widget node_popup_w;

    static Widget edit_displays_dialog_w;
    static Widget display_list_w;

    // Origin handling
    static void ClearOriginCB(Widget, XtPointer, XtPointer);
    static void set_last_origin(Widget origin);

    // Alias checking
    static bool check_aliases();
    static void sort_last_change(IntArray& disp_nrs);
    static int last_change_of_disp_nr(int disp_nr);
    static MString pretty(int disp_nr);

    // True iff aliases are to be checked regardless of address changes
    static bool force_check_aliases;

    // Merge displays in DISPLAYS.  Return true iff changed.
    // Accumulate messages in SUPPRESSED_MSG.
    static bool merge_displays(IntArray displays, MString& suppressed_msg);

    // Unmerge display DISP_NR; return true iff change
    static bool unmerge_display(int disp_nr);

public:
    static Widget graph_edit;
    static Widget graph_cmd_w;
    static ArgField *graph_arg;

    // Constructor
    DataDisp (XtAppContext app_context,
	      Widget       parent,
	      String       vsl_path,         // from app_data
	      String       vsl_library,      // from app_data
	      String       vsl_defs,         // from app_data
	      bool         panned);          // from app_data

    inline int count_all() { return disp_graph->count_all(); }
    static void refresh_graph_edit (bool silent = false);

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
};

#endif // _DDD_DataDisp_h
// DON'T ADD ANYTHING BEHIND THIS #endif
