// $Id$
// Data Display

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>
// and Andreas Zeller <zeller@ips.cs.tu-bs.de>
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

char DataDisp_rcsid[] =
    "$Id$";

// An interactive debugger is an outstanding example of what's NOT
// needed--it encourages trial-and-error hacking rather than
// systematic design, and also hides marginal people barely qualified
// for precision programming.
// 					-- HARLAN MILLS
//
// The debugger isn't a substitute for good thinking.  But, in some
// cases, thinking isn't a substitute for a good debugger either.  The
// most effective combination is good thinking and a good debugger.
//
//					-- STEVE McCONNELL, Code Complete


#ifdef __GNUG__
#pragma implementation
#endif

#define LOG_DISPLAYS 0
#define LOG_COMPARE  0

//-----------------------------------------------------------------------------
// Data Display Implementation
//-----------------------------------------------------------------------------

#include "DataDisp.h"

// Misc includes
#include "AliasGE.h"
#include "CompositeB.h"
#include "DestroyCB.h"
#include "DispGraph.h"
#include "GraphEdit.h"		// XtNgraph
#include "IntIntAA.h"
#include "MString.h"
#include "PannedGE.h"
#include "PosBuffer.h"
#include "ScrolledGE.h"
#include "StringBox.h"		// StringBox::fontTable
#include "StringMap.h"
#include "TagBox.h"
#include "TimeOut.h"
#include "VSEFlags.h"
#include "VSLLib.h"
#include "VoidArray.h"
#include "assert.h"
#include "bool.h"
#include "buttons.h"
#include "charsets.h"
#include "cmdtty.h"
#include "Command.h"
#include "converters.h"
#include "cook.h"
#include "post.h"
#include "question.h"
#include "settings.h"
#include "shorten.h"
#include "status.h"
#include "version.h"

// Motif includes
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h>	// XmMenuPosition()
#include <Xm/SelectioB.h>	// XmCreatePromptDialog()
#include <Xm/TextF.h>		// XmTextFieldGetString()
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/ToggleB.h>
#include <X11/StringDefs.h>

// DDD includes
#include "DispNode.h"		// Constructors
#include "disp-read.h"
#include "string-fun.h"
#include "comm-manag.h"
#include "Map.h"
#include "mydialogs.h"
#include "ddd.h"
#include "ArgField.h"
#include "verify.h"
#include "windows.h"
#include "wm.h"
#include "LessTifH.h"
#include "regexps.h"

// System includes
#include <iostream.h>
#include <fstream.h>		// ofstream
#include <iomanip.h>
#include <ctype.h>

extern void register_menu_shell(MMDesc *items);
extern bool process_emergencies();

//-----------------------------------------------------------------------
// Xt Stuff
//-----------------------------------------------------------------------
XtActionsRec DataDisp::actions [] = {
    {"graph-select",         DataDisp::graph_selectAct},
    {"graph-select-or-move", DataDisp::graph_select_or_moveAct},
    {"graph-extend",         DataDisp::graph_extendAct},
    {"graph-extend-or-move", DataDisp::graph_extend_or_moveAct},
    {"graph-toggle",         DataDisp::graph_toggleAct},
    {"graph-toggle-or-move", DataDisp::graph_toggle_or_moveAct},
    {"graph-popup-menu",     DataDisp::graph_popupAct},
    {"graph-dereference",    DataDisp::graph_dereferenceAct},
    {"graph-detail",         DataDisp::graph_detailAct},
    {"graph-rotate",         DataDisp::graph_rotateAct},
    {"graph-dependent",      DataDisp::graph_dependentAct}
};



// Popup Menu
struct GraphItms { enum Itms {SelectAll, Refresh, NewArg, New}; };
MMDesc DataDisp::graph_popup[] =
{
    {"selectAll", MMPush,               {DataDisp::selectAllCB}},
    {"refresh",   MMPush,               {DataDisp::refreshCB}},
    {"new_arg",   MMPush | MMUnmanaged, {DataDisp::popup_new_argCB}},
    {"new",       MMPush,               {DataDisp::popup_newCB}},
    MMEnd
};

// Number of shortcut items
const int DataDisp::shortcut_items = 20;

#define SHORTCUT_MENU \
{ \
    {"s1",  MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(1)  }}, \
    {"s2",  MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(2)  }}, \
    {"s3",  MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(3)  }}, \
    {"s4",  MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(4)  }}, \
    {"s5",  MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(5)  }}, \
    {"s6",  MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(6)  }}, \
    {"s7",  MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(7)  }}, \
    {"s8",  MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(8)  }}, \
    {"s9",  MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(9)  }}, \
    {"s10", MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(10) }}, \
    {"s11", MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(11) }}, \
    {"s12", MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(12) }}, \
    {"s13", MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(13) }}, \
    {"s14", MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(14) }}, \
    {"s15", MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(15) }}, \
    {"s16", MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(16) }}, \
    {"s17", MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(17) }}, \
    {"s18", MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(18) }}, \
    {"s19", MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(19) }}, \
    {"s20", MMPush | MMUnmanaged, { DataDisp::shortcutCB, XtPointer(20) }}, \
    {"other", MMPush, { DataDisp::dependentCB }}, \
    MMSep, \
    {"edit",  MMPush, { dddEditShortcutsCB }}, \
    MMEnd, \
}

// The menu used in the `New Display' button.
MMDesc DataDisp::shortcut_menu[]   = SHORTCUT_MENU;

// A stand-alone popup menu.
MMDesc DataDisp::shortcut_popup1[] = SHORTCUT_MENU;

// The sub-menu in the `New Display' item.
MMDesc DataDisp::shortcut_popup2[] = SHORTCUT_MENU;

struct RotateItms { enum Itms {RotateAll}; };

MMDesc DataDisp::rotate_menu[] =
{
    {"rotateAll",     MMPush | MMInsensitive, {DataDisp::rotateAllCB}},
    MMEnd
};

struct NodeItms { enum Itms {Dereference, New, Sep1, Detail, Rotate, Set,
			     Sep2, Delete }; };

MMDesc DataDisp::node_popup[] =
{
    {"dereference",   MMPush,   {DataDisp::dereferenceCB}},
    {"new",           MMMenu,   MMNoCB, DataDisp::shortcut_popup2, },
    MMSep,
    {"detail",        MMPush,   {DataDisp::toggleDetailCB, XtPointer(-1)}},
    {"rotate",        MMPush,   {DataDisp::rotateCB}},
    {"set",           MMPush,   {DataDisp::setCB}},
    MMSep,
    {"delete",        MMPush,   {DataDisp::deleteCB}},
    MMEnd
};

struct CmdItms { enum Itms {Dereference, Detail, Rotate, New, Set, Delete }; };

MMDesc DataDisp::graph_cmd_area[] =
{
    {"dereference",   MMPush | MMInsensitive, {DataDisp::dereferenceCB}},
    {"detail",        MMPush | MMInsensitive, {DataDisp::toggleDetailCB,
					       XtPointer(-1) }, 
                                               DataDisp::detail_menu },
    {"rotate",        MMPush | MMInsensitive, {DataDisp::rotateCB},
                                               DataDisp::rotate_menu },
    {"new",           MMPush,                 {DataDisp::dependentCB},
                                               DataDisp::shortcut_menu },
    {"set",           MMPush | MMInsensitive, {DataDisp::setCB}},
    {"delete",        MMPush | MMInsensitive, {DataDisp::deleteCB}},
    MMEnd
};


struct DetailItms { enum Itms { ShowMore, ShowJust, 
				ShowDetail, HideDetail }; };

MMDesc DataDisp::detail_menu[] =
{
    {"show_more",    MMPush, {DataDisp::showMoreDetailCB, XtPointer(1) }},
    {"show_just",    MMPush, {DataDisp::showDetailCB, XtPointer(1) }},
    {"show_detail",  MMPush, {DataDisp::showDetailCB, XtPointer(-1) }},
    {"hide_detail",  MMPush, {DataDisp::hideDetailCB, XtPointer(-1) }},
    MMEnd
};

struct DisplayItms { enum Itms {New, Dereference, 
				ShowDetail, HideDetail, Set, Delete}; };

MMDesc DataDisp::display_area[] =
{
    {"new",          MMPush,   {DataDisp::dependentCB }},
    {"dereference",  MMPush,   {DataDisp::dereferenceCB }},
    {"show_detail",  MMPush,   {DataDisp::showDetailCB, XtPointer(-1) }},
    {"hide_detail",  MMPush,   {DataDisp::hideDetailCB, XtPointer(-1) }},
    {"set",          MMPush,   {DataDisp::setCB}},
    {"delete",       MMPush,   {DataDisp::deleteCB }},
    MMEnd
};

DispGraph *DataDisp::disp_graph             = 0;
Widget     DataDisp::graph_edit             = 0;
Widget     DataDisp::graph_form_w           = 0;
Widget     DataDisp::last_origin            = 0;
ArgField  *DataDisp::graph_arg              = 0;
Widget     DataDisp::graph_cmd_w            = 0;
Widget     DataDisp::graph_selection_w      = 0;
Widget     DataDisp::edit_displays_dialog_w = 0;
Widget     DataDisp::display_list_w         = 0;
Widget     DataDisp::graph_popup_w          = 0;
Widget     DataDisp::node_popup_w           = 0;
Widget     DataDisp::shortcut_popup_w       = 0;

bool DataDisp::detect_aliases = false;

int DataDisp::next_ddd_display_number = 1;
int DataDisp::next_gdb_display_number = 1;

XtIntervalId DataDisp::refresh_args_timer       = 0;
XtIntervalId DataDisp::refresh_addr_timer       = 0;
XtIntervalId DataDisp::refresh_graph_edit_timer = 0;

// Array of shortcut expressions and their labels
StringArray DataDisp::shortcut_exprs;
StringArray DataDisp::shortcut_labels;


//----------------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------------

// Sort A
static void sort(IntArray& a)
{
    // Shell sort -- simple and fast
    int h = 1;
    do {
	h = h * 3 + 1;
    } while (h <= a.size());
    do {
	h /= 3;
	for (int i = h; i < a.size(); i++)
	{
	    int v = a[i];
	    int j;
	    for (j = i; j >= h && a[j - h] > v; j -= h)
		a[j] = a[j - h];
	    if (i != j)
		a[j] = v;
	}
    } while (h != 1);
}


//----------------------------------------------------------------------------
// Origin
//-----------------------------------------------------------------------------

void DataDisp::ClearOriginCB(Widget w, XtPointer, XtPointer)
{
    if (last_origin == w)
    {
	last_origin = 0;
    }
}

void DataDisp::set_last_origin(Widget w)
{
    if (last_origin != 0)
    {
	XtRemoveCallback(last_origin, XtNdestroyCallback, ClearOriginCB, 0);
    }

    last_origin = find_shell(w);

    if (last_origin != 0)
    {
	XtAddCallback(last_origin, XtNdestroyCallback, ClearOriginCB, 0);
    }
}

//----------------------------------------------------------------------------
// Sensitivity
//-----------------------------------------------------------------------------

inline void set_sensitive(Widget w, bool state)
{
    if (w != 0)
	XtSetSensitive(w, state);
}

static void set_label(Widget w, const string& label)
{
    MString new_label(label);
    XmString old_label;
    XtVaGetValues(w, XmNlabelString, &old_label, NULL);
    if (!XmStringCompare(new_label.xmstring(), old_label))
    {
	XtVaSetValues(w,
		      XmNlabelString, new_label.xmstring(),
		      NULL);
    }
    XmStringFree(old_label);
}


//----------------------------------------------------------------------------
// Counters
//-----------------------------------------------------------------------------

// Count the number of data displays
int DataDisp::count_data_displays() const
{
    int count = 0;

    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (!dn->is_user_command())
	    count++;
    }

    return count;
}


//-----------------------------------------------------------------------------
// Button Callbacks
//-----------------------------------------------------------------------------

void DataDisp::dereferenceCB(Widget w, XtPointer client_data, 
			     XtPointer call_data)
{
    set_last_origin(w);

    DispNode *disp_node_arg   = selected_node();
    DispValue *disp_value_arg = selected_value();
    if (disp_node_arg == 0 || disp_value_arg == 0)
    {
	newCB(w, client_data, call_data);
	return;
    }

    string display_expression = disp_value_arg->dereferenced_name();
    disp_value_arg->dereference();
    disp_node_arg->refresh();

    new_display(display_expression, 0, itostring(disp_node_arg->disp_nr()));
}

void DataDisp::toggleDetailCB(Widget dialog, XtPointer client_data, XtPointer)
{
    int depth = int(client_data);

    set_last_origin(dialog);

    IntArray disp_nrs;

    bool do_enable  = true;
    bool do_disable = true;

    bool changed = false;
    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->selected())
	{
	    DispValue *dv = dn->selected_value();
	    if (dv == 0)
		dv = dn->value();

	    if (dv == 0)
	    {
		// No value -- just enable or disable
		disp_nrs += dn->disp_nr();

		if (dn->disabled())
		{
		    // Enable display
		    do_disable = false;
		}
		else
		{
		    // Disable display
		    do_enable = false;
		}
	    }
	    else if (dv->collapsedAll() > 0)
	    {
		// Expand just this value
		dv->collapseAll();
		dv->expandAll(depth);

		if (dv == dn->value() && dn->disabled())
		{
		    // Enable display
		    disp_nrs += dn->disp_nr();
		    do_disable = false;
		}
		else
		{
		    dn->refresh();
		    changed = true;
		}
	    }
	    else
	    {
		// Collapse this value
		dv->collapse();

		if (dv == dn->value() && dn->enabled())
		{
		    // Disable display
		    disp_nrs += dn->disp_nr();
		    do_enable = false;
		}
		else
		{
		    dn->refresh();
		    changed = true;
		}
	    }
	}
    }

    if (do_enable)
	enable_display(disp_nrs);
    else if (do_disable)
	disable_display(disp_nrs);

    if (changed)
	refresh_graph_edit();
}

void DataDisp::showDetailCB (Widget dialog, XtPointer client_data, XtPointer)
{
    int depth = int(client_data);
    show(dialog, depth, 0);
}

void DataDisp::showMoreDetailCB(Widget dialog, XtPointer client_data, 
				XtPointer)
{
    int more = int(client_data);
    show(dialog, 0, more);
}

void DataDisp::show(Widget dialog, int depth, int more)
{
    set_last_origin(dialog);
    IntArray disp_nrs;

    bool changed = false;
    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->selected())
	{
	    if (dn->disabled())
	    {
		// Enable display
		disp_nrs += dn->disp_nr();
	    }
		
	    DispValue *dv = dn->selected_value();
	    if (dv == 0)
		dv = dn->value();
	    if (dv == 0)
		continue;

	    if (dn->disabled() && !dv->collapsed())
		continue;	// Re-enable old state

	    if (more != 0)
		depth = dv->heightExpanded() + more;

	    if (depth > 0 || dv->collapsedAll() > 0)
	    {
		dv->collapseAll();
		dv->expandAll(depth);
		dn->refresh();
		changed = true;
	    }
	}
    }

    enable_display(disp_nrs);

    if (changed)
	refresh_graph_edit();
}



void DataDisp::hideDetailCB (Widget dialog, XtPointer, XtPointer)
{
    set_last_origin(dialog);
    IntArray disp_nrs;

    bool changed = false;
    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->selected())
	{
	    DispValue *dv = dn->selected_value();
	    if (dv == 0)
		dv = dn->value();

	    if ((dv == 0 || dv == dn->value()) && dn->enabled())
	    {
		// Disable display
		disp_nrs += dn->disp_nr();
	    }

	    if (dv != 0 && dv->expanded())
	    {
		dv->collapse();
		dn->refresh();
		changed = true;
	    }
	}
    }

    disable_display(disp_nrs);

    if (changed)
	refresh_graph_edit();
}


void DataDisp::toggle_rotate(DispValue *dv, bool all)
{
    if (dv == 0)
	return;

    if (dv->horizontal_aligned())
	dv->align_vertical();
    else
	dv->align_horizontal();

    if (all)
	for (int i = 0; i < dv->nchildren(); i++)
	    toggle_rotate(dv->get_child(i), all);
}

void DataDisp::rotateCB(Widget w, XtPointer, XtPointer)
{
    set_last_origin(w);

    DispNode *disp_node_arg   = selected_node();
    DispValue *disp_value_arg = selected_value();
    if (disp_node_arg == 0 || disp_value_arg == 0)
	return;

    toggle_rotate(disp_value_arg, false);

    disp_node_arg->refresh();
    refresh_graph_edit();
}

void DataDisp::rotateAllCB(Widget w, XtPointer, XtPointer)
{
    set_last_origin(w);

    DispNode *disp_node_arg   = selected_node();
    DispValue *disp_value_arg = selected_value();
    if (disp_node_arg == 0 || disp_value_arg == 0)
	return;

    toggle_rotate(disp_value_arg, true);

    disp_node_arg->refresh();
    refresh_graph_edit();
}


void DataDisp::toggleDisableCB (Widget dialog, XtPointer, XtPointer)
{
    set_last_origin(dialog);
    IntArray disp_nrs;

    bool do_enable  = true;
    bool do_disable = true;

    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->selected())
	{
	    disp_nrs += dn->disp_nr();
	    if (dn->enabled())
		do_enable = false;
	    if (dn->disabled())
		do_disable = false;
	}
    }

    if (do_enable)
	enable_display(disp_nrs);
    else if (do_disable)
	disable_display(disp_nrs);
}

void DataDisp::select_with_all_descendants(GraphNode *node)
{
    if (!node->selected())
    {
	node->selected() = true;
	
	for (GraphEdge *edge = node->firstFrom();
	     edge != 0; edge = node->nextFrom(edge))
	    select_with_all_descendants(edge->to());
    }
}

void DataDisp::select_with_all_ancestors(GraphNode *node)
{
    if (!node->selected())
    {
	node->selected() = true;
	
	for (GraphEdge *edge = node->firstTo();
	     edge != 0; edge = node->nextTo(edge))
	    select_with_all_ancestors(edge->from());
    }
}

// Upon deletion, select the ancestor and all siblings
void DataDisp::deleteCB (Widget dialog, XtPointer, XtPointer)
{
    set_last_origin(dialog);

    IntArray disp_nrs;
    VarArray<GraphNode *> ancestors;
    VarArray<GraphNode *> descendants;

    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->selected())
	{
	    disp_nrs += dn->disp_nr();

	    // Select all ancestors
	    GraphEdge *edge;
	    for (edge = dn->nodeptr()->firstTo();
		 edge != 0; edge = dn->nodeptr()->nextTo(edge))
	    {
		GraphNode *ancestor = edge->from();
		while (ancestor->isHint())
		    ancestor = ancestor->firstTo()->from();

		ancestors += ancestor;
	    }

	    // Select all descendants
	    for (edge = dn->nodeptr()->firstFrom();
		 edge != 0; edge = dn->nodeptr()->nextFrom(edge))
	    {
		GraphNode *descendant = edge->to();
		while (descendant->isHint())
		    descendant = descendant->firstFrom()->to();

		descendants += descendant;
	    }
	}
    }

    int i;
    for (i = 0; i < ancestors.size(); i++)
	select_with_all_descendants(ancestors[i]);
    for (i = 0; i < descendants.size(); i++)
	select_with_all_ancestors(descendants[i]);

    delete_display(disp_nrs);
}


void DataDisp::refreshCB(Widget w, XtPointer, XtPointer)
{
    // Unmerge all displays
    MapRef ref;
    for (int k = disp_graph->first_nr(ref); 
	 k != 0;
	 k = disp_graph->next_nr(ref))
    {
	unmerge_display(k);
    }

    // Refresh them
    refresh_display(w);
}

void DataDisp::selectAllCB(Widget w, XtPointer, XtPointer)
{
    // StatusDelay d("Selecting all displays");

    set_last_origin(w);
    XtCallActionProc(graph_edit, 
		     "select-all", (XEvent *)0, (String *)0, 0);
    refresh_graph_edit();
}

void DataDisp::unselectAllCB(Widget w, XtPointer, XtPointer)
{
    // StatusDelay d("Unselecting all displays");

    set_last_origin(w);
    XtCallActionProc(graph_edit, 
		     "unselect-all", (XEvent *)0, (String *)0, 0);
    refresh_graph_edit();
}

void DataDisp::enableCB(Widget w, XtPointer, XtPointer)
{
    set_last_origin(w);

    IntArray disp_nrs;

    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->selected() && dn->disabled())
	{
	    disp_nrs += dn->disp_nr();
	}
    }

    enable_display(disp_nrs);
}

void DataDisp::disableCB(Widget w, XtPointer, XtPointer)
{
    set_last_origin(w);

    IntArray disp_nrs;

    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->selected() && dn->enabled())
	{
	    disp_nrs += dn->disp_nr();
	}
    }

    disable_display(disp_nrs);
}


void DataDisp::shortcutCB(Widget w, XtPointer client_data, XtPointer)
{
    int number = int(client_data) - 1;

    assert (number >= 0);
    assert (number < shortcut_exprs.size());

    set_last_origin(w);

    string expr = shortcut_exprs[number];

    string depends_on = "";

    DispNode *disp_node_arg   = selected_node();
    DispValue *disp_value_arg = selected_value();
    if (disp_node_arg != 0 
	&& disp_value_arg != 0
	&& !disp_node_arg->nodeptr()->hidden())
    {
	depends_on = itostring(disp_node_arg->disp_nr());
	string arg = source_arg->get_string();

	// Avoid multiple /format specifications
	if (arg.contains('/', 0) && expr.contains('/', 0))
	    arg = arg.after(rxwhite);

	expr.gsub("()", arg);
    }

    new_display(expr, 0, depends_on, w);
}

// Set shortcut menu to expressions EXPRS
void DataDisp::set_shortcut_menu(const StringArray& exprs,
				 const StringArray& labels)
{
    shortcut_labels = labels;
    shortcut_exprs  = exprs;

    while (shortcut_labels.size() < exprs.size())
	shortcut_labels += "";

#if 0
    if (exprs.size() > shortcut_items)
    {
	post_warning("Shortcut menu capacity exceeded.",
		     "too_many_shortcuts_warning", last_origin);
    }
#endif

    for (int i = 0; i < shortcut_items; i++)
    {
	Widget popup1_item = shortcut_popup1[i].widget;
	Widget popup2_item = shortcut_popup2[i].widget;
	Widget menu_item   = shortcut_menu[i].widget;

	if (i < exprs.size())
	{
	    string& expr  = shortcut_exprs[i];
	    string& label = shortcut_labels[i];

	    if (label == "")
		label = "Display " + expr;

	    set_label(popup1_item, label);
	    set_label(popup2_item, label);
	    set_label(menu_item,   label);

	    XtManageChild(popup1_item);
	    XtManageChild(popup2_item);
	    XtManageChild(menu_item);
	}
	else
	{
	    // Unmanage widgets
	    XtUnmanageChild(popup1_item);
	    XtUnmanageChild(popup2_item);
	    XtUnmanageChild(menu_item);
	}
    }

    refresh_args();
}

// Add one expr to shortcut menus
void DataDisp::add_shortcut_expr(const string& expr)
{
    // Insert as first item in SHORTCUT_EXPRS
    shortcut_exprs  += string("");
    shortcut_labels += string("");
    for (int i = shortcut_exprs.size() - 1; i > 0; i--)
    {
	shortcut_exprs[i]  = shortcut_exprs[i - 1];
	shortcut_labels[i] = shortcut_labels[i - 1];
    }

    shortcut_exprs[0]  = expr;
    shortcut_labels[0] = "";

    set_shortcut_menu(shortcut_exprs, shortcut_labels);
    refresh_button_editor();
    refresh_args();
}

MString DataDisp::shortcut_help(Widget w)
{
    for (int i = 0; i < shortcut_items; i++)
    {
	if (w == shortcut_menu[i].widget
	    || w == shortcut_popup1[i].widget
	    || w == shortcut_popup2[i].widget)
	{
	    MString ret = rm("Display ");
	    string expr = shortcut_exprs[i];

	    while (expr.contains("()"))
	    {
		ret += tt(expr.before("()"));
		ret += bf("()");
		expr = expr.after("()");
	    }
	    ret += tt(expr);
	    return ret;
	}
    }

    return MString(0, true);	// Not found
}


//-----------------------------------------------------------------------------
// Double click callback
//-----------------------------------------------------------------------------

// Counter
struct DataDispCount {
    int all;			// Total # of displays
    int visible;		// # of non-hidden displays
    int selected;		// # of selected displays
    int selected_expanded;	// # of selected and expanded displays
    int selected_collapsed;	// # of selected and collapsed displays
    int selected_data;		// # of selected user displays

    DataDispCount(DispGraph *disp_graph);
};

DataDispCount::DataDispCount(DispGraph *disp_graph)
    : all(0), visible(0), selected(0),
      selected_expanded(0),
      selected_collapsed(0),
      selected_data(0)
{

    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	all++;

	if (!dn->nodeptr()->hidden())
	    visible++;

	if (dn->selected())
	{
	    selected++;
	    if (!dn->is_user_command())
		selected_data++;

	    if (dn->disabled())
		selected_collapsed++;
	    else
	    {
		DispValue *dv = dn->selected_value();
		if (dv == 0)
		    dv = dn->value();
		if (dv != 0)
		{
		    selected_expanded  += int(dv->expanded());
		    selected_collapsed += dv->collapsedAll();
		}
	    }
	}
    }
}

void DataDisp::DoubleClickCB(Widget w, XtPointer client_data, 
			     XtPointer call_data)
{
    DataDisp *data_disp = (DataDisp *)client_data;
    GraphEditPreSelectionInfo *info = (GraphEditPreSelectionInfo *)call_data;

    if (!info->double_click)
	return;			// Single click

    if (info->node == 0)
	return;			// Double-click on background

    DispNode *disp_node_arg   = data_disp->selected_node();
    if (disp_node_arg == 0)
	return;			// No selection

    DispValue *disp_value_arg;

    // Do the right thing
    if (disp_node_arg->disabled())
    {
	showMoreDetailCB(w, XtPointer(1), 0); // Show 1 level more
    }
    else
    {
	disp_value_arg = disp_node_arg->selected_value();
	if (disp_value_arg == 0)
	    return;			// No selected value within node

	DataDispCount count(disp_graph);
	
	if (disp_value_arg->type() == Pointer && !disp_value_arg->collapsed())
	    data_disp->dereferenceCB(w, 0, 0);    // Dereference
	else if (count.selected_collapsed > 0)
	    showMoreDetailCB(w, XtPointer(1), 0); // Show 1 level more
	else
	    hideDetailCB(w, XtPointer(-1), 0);    // Hide all
    }

    // Don't do the default action
    info->doit = False;
}


//-----------------------------------------------------------------------------
// Popup menu callbacks
//-----------------------------------------------------------------------------

void DataDisp::popup_new_argCB (Widget    display_dialog,
				XtPointer client_data,
				XtPointer)
{
    set_last_origin(display_dialog);

    BoxPoint *p = (BoxPoint *) client_data;
    new_display(source_arg->get_string(), p);
}


void DataDisp::popup_newCB (Widget    display_dialog,
			    XtPointer client_data,
			    XtPointer)
{
    set_last_origin(display_dialog);

    BoxPoint *p = (BoxPoint *) client_data;
    new_displayCD(display_dialog, *p);
}




//-----------------------------------------------------------------------------
// Entering new Data Displays
//-----------------------------------------------------------------------------

class NewDisplayInfo {
public:
    string display_expression;
    string scope;
    StringArray display_expressions;
    BoxPoint point;
    BoxPoint *point_ptr;
    string depends_on;
    Widget origin;
    Widget shortcut;
    Widget text;
    bool verbose;

    NewDisplayInfo()
	: display_expression(),
	  scope(),
	  display_expressions(),
	  point(),
	  point_ptr(0),
	  depends_on(),
	  origin(0),
	  shortcut(0),
	  text(0),
	  verbose(false)
    {}

    ~NewDisplayInfo()
    {}

private:
    NewDisplayInfo(const NewDisplayInfo&)
	: display_expression(),
	  scope(),
	  display_expressions(),
	  point(),
	  point_ptr(0),
	  depends_on(),
	  origin(0),
	  shortcut(0),
	  text(0),
	  verbose(false)
    {
	assert(0);
    }
    NewDisplayInfo& operator = (const NewDisplayInfo&)
    {
	assert(0); return *this;
    }
};

void DataDisp::new_displayDCB (Widget dialog, XtPointer client_data, XtPointer)
{
    set_last_origin(dialog);

    NewDisplayInfo *info = (NewDisplayInfo *)client_data;

    char *inp = XmTextFieldGetString(info->text);
    string expr(inp);
    XtFree(inp);

    read_leading_blanks(expr);
    strip_final_blanks(expr);

    if (expr != "")
    {
	new_display(expr, info->point_ptr, info->depends_on, info->origin);

	if (info->shortcut != 0 && XmToggleButtonGetState(info->shortcut))
	{
	    // Add expression to shortcut menu
	    expr.gsub("()", "( )");
	    if (expr != info->display_expression)
		expr.gsub(info->display_expression, string("()"));
	    add_shortcut_expr(expr);
	}
    }
}

Widget DataDisp::create_display_dialog(Widget parent, String name,
				       NewDisplayInfo& info)
{
    Arg args[10];
    int arg = 0;

    Widget dialog = verify(XmCreatePromptDialog(find_shell(parent),
						name, args, arg));
    Delay::register_shell(dialog);

    if (lesstif_version <= 79)
	XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_APPLY_BUTTON));

    XtAddCallback(dialog, XmNhelpCallback, ImmediateHelpCB, NULL);
    XtAddCallback(dialog, XmNokCallback, new_displayDCB, XtPointer(&info));

    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,  0); arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNborderWidth,  0); arg++;
    XtSetArg(args[arg], XmNadjustMargin, False); arg++;
    Widget box = 
	verify(XmCreateRowColumn(dialog, "box", args, arg));
    XtManageChild(box);

    arg = 0;
    XtSetArg(args[arg], XmNalignment, XmALIGNMENT_BEGINNING); arg++;
    Widget label = verify(XmCreateLabel(box, "label", args, arg));
    XtManageChild(label);

    arg = 0;
    info.text = 
	verify(XmCreateTextField(box, "text", args, arg));
    XtManageChild(info.text);

    arg = 0;
    XtSetArg(args[arg], XmNalignment, XmALIGNMENT_BEGINNING); arg++;
    info.shortcut = 
	verify(XmCreateToggleButton(box, "shortcut", args, arg));
    XtManageChild(info.shortcut);

    XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(dialog, XmDIALOG_SELECTION_LABEL));

    return dialog;
}

// Enter a new Display at BOX_POINT
void DataDisp::new_displayCD (Widget w, BoxPoint box_point)
{
    static NewDisplayInfo info;
    if (info.point_ptr == 0)
	info.point_ptr = new BoxPoint;
    info.origin = w;

    static Widget new_display_dialog = 
	create_display_dialog(w, "new_display_dialog", info);

    XmToggleButtonSetState(info.shortcut, False, False);

    *(info.point_ptr) = box_point;
    info.display_expression = source_arg->get_string();
    XmTextSetString(info.text, info.display_expression);

    manage_and_raise(new_display_dialog);
}

// Create a new display
void DataDisp::newCB(Widget w, XtPointer, XtPointer)
{
    set_last_origin(w);
    new_displayCD(w);
}

// Create a new dependent display
void DataDisp::dependentCB(Widget w, XtPointer client_data, 
			   XtPointer call_data)
{
    set_last_origin(w);

    DispNode *disp_node_arg   = selected_node();
    DispValue *disp_value_arg = selected_value();
    if (disp_node_arg == 0 
	|| disp_value_arg == 0
	|| disp_node_arg->nodeptr()->hidden())
    {
	newCB(w, client_data, call_data);
	return;
    }

    static NewDisplayInfo info;
    info.depends_on = itostring(disp_node_arg->disp_nr());
    info.origin = w;

    static Widget dependent_display_dialog = 
	create_display_dialog(w, "dependent_display_dialog", info);

    XmToggleButtonSetState(info.shortcut, True, False);

    info.display_expression = disp_value_arg->full_name();
    XmTextSetString(info.text, info.display_expression);
    manage_and_raise(dependent_display_dialog);
}


//-----------------------------------------------------------------------------
// Redraw graph and update display list
//-----------------------------------------------------------------------------

struct GraphEditState {
    Boolean autoLayout;
    Boolean snapToGrid;
};

void DataDisp::refresh_graph_edit(bool silent)
{
    // Save current graph editor state
    static GraphEditState state;

    XtVaGetValues(graph_edit,
		  XtNautoLayout, &state.autoLayout,
		  XtNsnapToGrid, &state.snapToGrid,
		  NULL);

    if (refresh_graph_edit_timer == 0)
    {
	refresh_graph_edit_timer = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(graph_edit),
			    0, RefreshGraphEditCB, XtPointer(&state));
    }

    refresh_args();
    refresh_display_list(silent);
}

void DataDisp::RefreshGraphEditCB(XtPointer client_data, XtIntervalId *id)
{
    (void) id;			// Use it
    assert(*id == refresh_graph_edit_timer);
    refresh_graph_edit_timer = 0;

    static GraphEditState state;

    XtVaGetValues(graph_edit,
		  XtNautoLayout, &state.autoLayout,
		  XtNsnapToGrid, &state.snapToGrid,
		  NULL);

    const GraphEditState& old_state = *((GraphEditState *) client_data);

    static Graph *dummy = new Graph;

    XtVaSetValues(graph_edit,
		  XtNautoLayout, old_state.autoLayout,
		  XtNsnapToGrid, old_state.snapToGrid,
		  XtNgraph, dummy,
		  NULL);
    XtVaSetValues(graph_edit,
		  XtNgraph, (Graph *)disp_graph,
		  NULL);
    XtVaSetValues(graph_edit,
		  XtNautoLayout, state.autoLayout,
		  XtNsnapToGrid, state.snapToGrid,
		  NULL);

    if (disp_graph->firstVisibleNode() != 0)
    {
	static bool popped_up = false;

	if (!popped_up)
	{
	    // Graph is non-empty: make sure its shell is visible
	    XtManageChild(graph_edit);
	    initial_popup_shell(data_disp_shell);
	    popped_up = true;
	}
    }
}

// ***************************************************************************
//
inline int DataDisp::getDispNrAtPoint (BoxPoint point)
{
    GraphNode* gn = graphEditGetNodeAtPoint (graph_edit, point);
    if (gn == 0)
	return 0;

    BoxGraphNode* bgn = ptr_cast (BoxGraphNode, gn);
    if (bgn == 0)
	return 0;

    return disp_graph->get_nr (bgn);
}


//-----------------------------------------------------------------------------
// Make buttons sensitive or insensitive
//-----------------------------------------------------------------------------

void DataDisp::no_displaysHP (void*, void* , void* call_data)
{
    bool empty = bool(call_data);

    set_sensitive (graph_popup[GraphItms::Refresh].widget,
		   (!empty && gdb->isReadyWithPrompt()));
}


//-----------------------------------------------------------------------------
// Unselect nodes when selection is lost
//-----------------------------------------------------------------------------

bool DataDisp::lose_selection = true;

void DataDisp::SelectionLostCB(Widget, XtPointer, XtPointer)
{
    if (!lose_selection)
	return;

    // Selection lost - clear all highlights
    bool changed = false;
    for (GraphNode *gn = disp_graph->firstNode();
	 gn != 0; gn = disp_graph->nextNode(gn))
    {
	if (gn->selected())
	{
	    gn->selected() = false;
	    changed = true;
	    graphEditRedrawNode(graph_edit, gn);
	}
    }

    if (changed)
    {
	refresh_args();
	refresh_display_list();
    }
}

//-----------------------------------------------------------------------------
// Action procs
//----------------------------------------------------------------------------

void DataDisp::graph_dereferenceAct (Widget w, XEvent*, String*, Cardinal*)
{
    dereferenceCB(w, 0, 0);
}

void DataDisp::graph_detailAct (Widget w, XEvent *, 
				String *params, Cardinal *num_params)
{
    int depth = -1;
    if (params != 0 && num_params != 0 && *num_params >= 1)
	depth = atoi(params[0]);

    toggleDetailCB(w, XtPointer(depth), 0);
}

void DataDisp::graph_rotateAct (Widget w, XEvent*, String*, Cardinal*)
{
    rotateCB(w, 0, 0);
}

void DataDisp::graph_dependentAct (Widget w, XEvent*, String*, Cardinal*)
{
    dependentCB(w, 0, 0);
}


Time DataDisp::last_select_time = 0;

// The GraphEdit actions with some data display magic prepended
void DataDisp::call_selection_proc(Widget w,
				   String name,
				   XEvent* event,
				   String* args,
				   Cardinal num_args,
				   SelectionMode mode)
{
    // Let multi-clicks pass right through
    Time t = time(event);
    if (Time(t - last_select_time) > Time(XtGetMultiClickTime(XtDisplay(w))))
	set_args(point(event), mode);
    last_select_time = t;

    XtCallActionProc(w, name, event, args, num_args);
}

void DataDisp::graph_selectAct (Widget, XEvent* event, String* args, 
				Cardinal* num_args)
{
    call_selection_proc(graph_edit, "select", event, args, *num_args, 
			SetSelection);
}

void DataDisp::graph_select_or_moveAct (Widget, XEvent* event, String* args, 
					Cardinal* num_args)
{
    call_selection_proc(graph_edit, "select-or-move", event, args, *num_args,
			SetSelection);
}

void DataDisp::graph_extendAct (Widget, XEvent* event, String* args, 
				Cardinal* num_args)
{
    call_selection_proc(graph_edit, "extend", event, args, *num_args,
			ExtendSelection);
}

void DataDisp::graph_extend_or_moveAct (Widget, XEvent* event, String* args, 
					Cardinal* num_args)
{
    call_selection_proc(graph_edit, "extend-or-move", event, args, *num_args,
			ExtendSelection);
}

void DataDisp::graph_toggleAct (Widget, XEvent* event, String* args, 
				Cardinal* num_args)
{
    call_selection_proc(graph_edit, "toggle", event, args, *num_args,
			ToggleSelection);
}

void DataDisp::graph_toggle_or_moveAct (Widget, XEvent* event, String* args, 
					Cardinal* num_args)
{
    call_selection_proc(graph_edit, "toggle-or-move", event, args, *num_args,
			ToggleSelection);
}

void DataDisp::graph_popupAct (Widget, XEvent* event, String *args, 
			       Cardinal *num_args)
{
    static BoxPoint* p = 0;
    if (p == 0)
    {
	p = new BoxPoint;

	MMaddCallbacks(graph_popup,     XtPointer(p));
	MMaddCallbacks(node_popup,      XtPointer(p));
	MMaddCallbacks(shortcut_popup1, XtPointer(p));
    }
    *p = point(event);

    set_args(*p, SetSelection);

    string arg = "";
    if (num_args != 0 && *num_args > 0)
	arg = downcase(args[0]);

    Widget popup = 0;
    if (arg == "graph" || selected_node() == 0)
	popup = graph_popup_w;
    else if (arg == "shortcut" 
	     || (arg == "" && event->xbutton.state & ShiftMask))
	popup = shortcut_popup_w;
    else if (arg == "node" || arg == "")
	popup = node_popup_w;
    else
	cerr << "graph-popup: bad argument " << quote(arg) << "\n";

    if (popup != 0)
    {
	XmMenuPosition(popup, &event->xbutton);
	XtManageChild(popup);
    }
}

void DataDisp::set_args(BoxPoint p, SelectionMode mode)
{
    DispNode*  disp_node   = 0;
    DispValue* disp_value  = 0;
    BoxGraphNode *disp_bgn = 0;

    bool was_selected = false;

    int disp_nr = getDispNrAtPoint(p);
    if (disp_nr)
    {
	disp_node = disp_graph->get (disp_nr);
	disp_value = (DispValue *)disp_node->box()->data(p);

	disp_bgn = disp_node->nodeptr();
	was_selected = disp_bgn->selected() && disp_value == 0;

	switch (mode)
	{
	case ExtendSelection:
	case ToggleSelection:
	    if (disp_node == selected_node()
		&& disp_node->selected_value() != 0
		&& disp_value != disp_node->selected_value())
	    {
		// Add another value in this node.  We can't do this,
		// so toggle the entire node.
		disp_bgn->selected() = false;
		disp_node->select(0);
		graphEditRedrawNode(graph_edit, disp_bgn);
		break;
	    }
	    // FALL THROUGH

	case SetSelection:
	    if (disp_value != disp_node->selected_value())
	    {
		disp_node->select(disp_value);
		graphEditRedrawNode(graph_edit, disp_bgn);
	    }
	    break;
	}
    }

    if (mode == SetSelection)
    {
	// Clear other highlights and selections
	MapRef ref;
	for (DispNode* dn = disp_graph->first(ref); 
	     dn != 0;
	     dn = disp_graph->next(ref))
	{
	    if (dn != disp_node)
	    {
		BoxGraphNode *node = dn->nodeptr();
		bool redraw = false;

		if (!was_selected)
		{
		    if (!redraw)
			redraw = node->selected();
		    dn->nodeptr()->selected() = false;
		}

		if (!redraw)
		    redraw = (node->highlight() != 0);
		dn->select(0);

		if (redraw)
		    graphEditRedrawNode(graph_edit, node);
	    }
	}
    }

    refresh_args();
    refresh_display_list();
}

DispNode *DataDisp::selected_node()
{
    DispNode *ret = 0;

    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->selected())
	{
	    if (ret == 0)
		ret = dn;
	    else
		return 0;	// multiple nodes selected
	}
    }

    return ret;
}

DispValue *DataDisp::selected_value()
{
    DispNode *dn = selected_node();
    if (dn == 0)
	return 0;

    DispValue *dv = dn->selected_value();
    if (dv != 0)
	return dv;

    // We treat the selected node just like the selected top value
    return dn->value();
}

void DataDisp::refresh_args()
{
    if (refresh_args_timer == 0)
    {
	refresh_args_timer = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(graph_edit),
			    0, RefreshArgsCB, XtPointer(graph_edit));
    }
}

void DataDisp::RefreshArgsCB(XtPointer, XtIntervalId *timer_id)
{
    (void) timer_id;		// Use it
    assert(*timer_id == refresh_args_timer);
    refresh_args_timer = 0;

    DataDispCount count(disp_graph);

    if (count.selected > 1)
    {
	// Clear all local highlights
	MapRef ref;
	for (DispNode* dn = disp_graph->first(ref); 
	     dn != 0;
	     dn = disp_graph->next(ref))
	{
	    BoxGraphNode *node = dn->nodeptr();
	    bool redraw = (node->highlight() != 0);

	    dn->select(0);
	    if (redraw)
		graphEditRedrawNode(graph_edit, node);
	}
    }

    DispNode *disp_node_arg   = selected_node();
    DispValue *disp_value_arg = selected_value();

    // New ()
    set_sensitive(graph_popup[GraphItms::NewArg].widget, !source_arg->empty());

    // Refresh (), Select All ()
    set_sensitive(graph_popup[GraphItms::Refresh].widget,   count.all > 0);
    set_sensitive(graph_popup[GraphItms::SelectAll].widget, count.visible > 0);

    Boolean dereference_ok  = False;
    Boolean rotate_ok       = False;
    Boolean rotate_state    = False;

    if (disp_value_arg != 0)
    {
	// We have selected a single node
	switch (disp_value_arg->type())
	{
	case Simple:
	case Text:
	case Reference:
	    break;

	case Pointer: 
	    dereference_ok = true;
	    break;

	case Array:
	    rotate_ok     = disp_value_arg->expanded();
	    rotate_state  = disp_value_arg->vertical_aligned();
	    break;

	case List:
	case StructOrClass:
	case BaseClass:
	    break;

	case UnknownType:
	    assert(0);
	    abort();
	}
    }

    // Dereference
    set_sensitive(node_popup[NodeItms::Dereference].widget,
		  dereference_ok);
    set_sensitive(graph_cmd_area[CmdItms::Dereference].widget,
		  dereference_ok);
    set_sensitive(display_area[DisplayItms::Dereference].widget,
		  dereference_ok);

    // Dependent
    set_sensitive(graph_cmd_area[CmdItms::New].widget, true);
    set_sensitive(display_area[DisplayItms::New].widget, true);

    // Rotate
    set_sensitive(node_popup[NodeItms::Rotate].widget,
		  rotate_ok);
    set_sensitive(graph_cmd_area[CmdItms::Rotate].widget,
		  rotate_ok);
    set_sensitive(rotate_menu[RotateItms::RotateAll].widget,
		  rotate_ok);

    // Show/Hide Detail
    if (count.selected_expanded > 0 && count.selected_collapsed == 0)
    {
	// Only expanded displays selected
	set_label(node_popup[NodeItms::Detail].widget, "Hide All");
	set_label(graph_cmd_area[CmdItms::Detail].widget, "Hide ()");
	set_sensitive(node_popup[NodeItms::Detail].widget, true);
	set_sensitive(graph_cmd_area[CmdItms::Detail].widget, true);
    }
    else if (count.selected_collapsed > 0)
    {
	// Some collapsed displays selected
	set_label(node_popup[NodeItms::Detail].widget, "Show All");
	set_label(graph_cmd_area[CmdItms::Detail].widget, "Show ()");
	set_sensitive(node_popup[NodeItms::Detail].widget, true);
	set_sensitive(graph_cmd_area[CmdItms::Detail].widget, true);
    }
    else
    {
	// Expanded as well as collapsed displays selected
	set_sensitive(node_popup[NodeItms::Detail].widget, false);
	set_sensitive(graph_cmd_area[CmdItms::Detail].widget, false);
    }

    set_sensitive(display_area[DisplayItms::ShowDetail].widget, 
		  count.selected_collapsed > 0);
    set_sensitive(display_area[DisplayItms::HideDetail].widget, 
		  count.selected_expanded > 0);

    set_sensitive(detail_menu[DetailItms::ShowMore].widget, 
		  count.selected_collapsed > 0);
    set_sensitive(detail_menu[DetailItms::ShowJust].widget, 
		  count.selected > 0);
    set_sensitive(detail_menu[DetailItms::ShowDetail].widget, 
		  count.selected_collapsed > 0);
    set_sensitive(detail_menu[DetailItms::HideDetail].widget, 
		  count.selected_expanded > 0);

    // Delete
    set_sensitive(graph_cmd_area[CmdItms::Delete].widget, 
		  count.selected > 0);
    set_sensitive(display_area[DisplayItms::Delete].widget, 
		  count.selected > 0);

    // Set
    bool have_set = gdb->has_assign_command() && 
	count.selected == 1 && count.selected_data == 1;
    set_sensitive(graph_cmd_area[CmdItms::Set].widget,   have_set);
    set_sensitive(display_area[DisplayItms::Set].widget, have_set);
    set_sensitive(node_popup[NodeItms::Set].widget, gdb->has_assign_command());

    // Shortcut menu
    for (int i = 0; i < shortcut_items && i < shortcut_exprs.size(); i++)
    {
	const string& expr = shortcut_exprs[i];
	bool sens = false;
	if (!expr.contains("()"))
	    sens = true;	// Argument not needed
	else if (count.selected == 0)
	    sens = false;	// Nothing selected
	else if (disp_value_arg)
	    sens = true;	// Exactly one value selected
	else if (disp_node_arg)
	    sens = true;	// Exactly one expression selected

	set_sensitive(shortcut_popup1[i].widget, sens);
	set_sensitive(shortcut_popup2[i].widget, sens);
	set_sensitive(shortcut_menu  [i].widget, sens);
    }

    // Argument field
    if (count.selected > 0)
    {
	string arg;
	if (disp_value_arg)
	{
	    arg = disp_value_arg->full_name();
	    source_arg->set_string(arg);
	}
	else if (disp_node_arg)
	{
	    arg = disp_node_arg->name();
	    source_arg->set_string(arg);
	}
	else
	{
	    ostrstream arg_os;
	    arg_os << "(" << count.selected << " displays)";
	    arg = arg_os;
	}
	graph_arg->set_string(arg);
    }
    else
    {
	// No argument
	graph_arg->set_string("");
    }

    // Set selection.
    // If the entire graph is selected, include position info, too.
    bool include_position = (count.selected >= count.visible);
    ostrstream os;
    get_selection(os, include_position);
    string cmd(os);

    // Setting the string causes the selection to be lost.  By setting
    // LOSE_SELECTION, we make sure the associated callbacks return
    // immediately.
    lose_selection = false;
    XmTextSetString(graph_selection_w, (char *)cmd);
    lose_selection = true;

    Time tm = XtLastTimestampProcessed(XtDisplay(graph_selection_w));

    if (cmd == "")
    {
	// Nothing selected - clear selection explicitly
	XmTextClearSelection(graph_selection_w, tm);
    }
    else
    {
	// Own the selection
	XmTextSetSelection(graph_selection_w, 
			   0, XmTextGetLastPosition(graph_selection_w), tm);
    }
}


// The maximum display number when saving states
int DataDisp::max_display_number = 99;

// Get scopes in SCOPES
bool DataDisp::get_scopes(StringArray& scopes)
{
    // Fetch current backtrace and store scopes in SCOPES
    string backtrace = gdb_question(gdb->where_command(), -1, true);
    while (backtrace != "")
    {
	string scope = get_scope(backtrace);
	if (scope != "")
	    scopes += scope;
	backtrace = backtrace.after('\n');
    }

    return scopes.size() > 0;
}

// Write commands to restore frame #TARGET_FRAME in OS
void DataDisp::write_frame_command(ostream& os, int& current_frame, 
				   int target_frame)
{
    if (target_frame != current_frame)
    {
	os << "graph ";
	if (gdb->has_frame_command())
	{
	    // Issue `frame' command
	    os << gdb->frame_command(target_frame) << "\n";
	}
	else
	{
	    // Use `up' and `down' commands
	    int offset = current_frame - target_frame;
	    if (offset == -1)
		os << "up";
	    else if (offset < 0)
		os << "up " << -offset;
	    else if (offset == 1)
		os << "down";
	    else if (offset > 0)
		os << "down " << offset;
	}

	current_frame = target_frame;
    }
}

// Write commands to restore scope of DN in OS
void DataDisp::write_restore_scope_command(ostream& os,
					   int& current_frame,
					   const StringArray& scopes,
					   DispNode *dn,
					   bool& ok)
{
    int target_frame = -1;

    if (dn->is_user_command())
    {
	// User displays are always evaluated on the current frame
	target_frame = 0;
    }
    else if (dn->scope() == "")
    {
	// A global, maybe?  Evaluate on main frame
	target_frame = scopes.size() - 1;	// Return to main frame
    }
    else
    {
	// Search scope in backtrace
	for (int i = 0; i < scopes.size(); i++)
	    if (scopes[i] == dn->scope())
	    {
		target_frame = i;
		break;
	    }
    }

    if (target_frame < 0)
    {
	// Cannot restore frame
	MString msg;
	msg += rm("Cannot save display ");
	msg += rm(itostring(dn->disp_nr()) + ": ");
	msg += tt(dn->name());

	if (dn->scope() != "")
	{
	   msg += rm(" because ");
	   msg += tt(dn->scope());
	   msg += rm(" is not in current backtrace");
	}

	set_status_mstring(msg);
	ok = false;

	target_frame = scopes.size() - 1;	// Return to main frame
    }

    write_frame_command(os, current_frame, target_frame);
}

bool DataDisp::get_state(ostream& os,
			 bool restore_state,
			 bool include_position,
			 const StringArray& scopes,
			 int target_frame)
{
    // Sort displays by number, such that old displays appear before
    // newer ones.

    // Note: this fails with the negative numbers of user-defined
    // displays; a topological sort would make more sense here. (FIXME)
    IntArray nrs;
    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (restore_state || dn->selected())
	    nrs += dn->disp_nr();
    }
    sort(nrs);

    bool ok = true;
    if (restore_state && scopes.size() == 0 && need_core_to_restore())
    {
	set_status("Cannot get current backtrace");
	ok = false;
    }

    // When restoring, we'll be at the lowest frame (#0).
    int current_frame = 0;

    for (int i = 0; i < nrs.size(); i++)
    {
	DispNode *dn = disp_graph->get(nrs[i]);
	if (dn == 0)
	    continue;

	if (restore_state && scopes.size() > 0)
	    write_restore_scope_command(os, current_frame, scopes, dn, ok);

	os << "graph display " << dn->name();

	if (include_position)
	    os << " at " << dn->nodeptr()->pos();

	// Find dependencies
	GraphEdge *edge;
	for (edge = dn->nodeptr()->firstTo();
	     edge != 0; edge = dn->nodeptr()->nextTo(edge))
	{
	    BoxGraphNode *ancestor = ptr_cast(BoxGraphNode, edge->from());
	    if (ancestor != 0)
	    {
		int depnr = disp_graph->get_nr(ancestor);
		DispNode *depnode = disp_graph->get(depnr);

		os << " dependent on " << depnode->name();
	    }
	}
	os << '\n';
    }

    // That's it: return to target frame...
    write_frame_command(os, current_frame, target_frame);

    // ... and refresh the graph.
    if (restore_state && nrs.size() > 0)
	os << "graph refresh\n";

    return ok;
}


// Return true if a core dump is needed to restore displays
bool DataDisp::need_core_to_restore()
{
    MapRef ref;
    for (DispNode *dn = disp_graph->first(ref);
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->scope() != "")
	    return true;
    }

    return false;
}

// Reset all
void DataDisp::reset_done(const string& answer, void *)
{
    // Nothing special yet
    (void) answer;
}

void DataDisp::reset()
{
    // Clear all data displays
    IntArray display_nrs;
    getDisplayNumbers(display_list_w, display_nrs);

    if (display_nrs.size() > 0)
    {
	Command c("graph undisplay " + numbers(display_nrs));
	c.verbose  = false;
	c.check    = true;
	c.priority = COMMAND_PRIORITY_INIT;
	c.callback = reset_done;
	gdb_command(c);
    }
}


// Return number of display aliased by NODE
int DataDisp::alias_display_nr(GraphNode *node)
{
    if (!node->isHint())
	return 0;
    AliasGraphEdge *edge = ptr_cast(AliasGraphEdge, node->firstTo());
    if (edge == 0)
	return 0;

    return edge->disp_nr();
}

// Update graph editor selection after a change in the display editor
void DataDisp::UpdateGraphEditorSelectionCB(Widget, XtPointer, XtPointer)
{
    IntArray display_nrs;
    getDisplayNumbers(display_list_w, display_nrs);

    // Update graph editor selection
    MapRef ref;
    for (DispNode *dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	int display_nr = dn->disp_nr();

	bool select = false;
	for (int i = 0; i < display_nrs.size(); i++)
	{
	    if (display_nr == display_nrs[i])
	    {
		select = true;
		break;
	    }
	}

	if (select != dn->selected())
	{
	    dn->selected() = select;
	    graphEditRedrawNode(graph_edit, dn->nodeptr());
	}

	if (dn->nodeptr()->hidden())
	{
	    // Synchronize hint nodes with this alias node
	    for (GraphNode *node = disp_graph->firstNode();
		 node != 0;
		 node = disp_graph->nextNode(node))
	    {
		if (alias_display_nr(node) == display_nr)
		{
		    if (node->selected() != dn->selected())
		    {
			node->selected() = dn->selected();
			graphEditRedrawNode(graph_edit, node);
		    }
		}
	    }
	}
    }

    refresh_args();
    refresh_display_list();
}

// Update display editor selection after a change in the graph editor
void DataDisp::UpdateDisplayEditorSelectionCB(Widget, XtPointer, XtPointer)
{
    // Synchronize alias nodes with hint nodes
    for (GraphNode *node = disp_graph->firstNode();
	 node != 0;
	 node = disp_graph->nextNode(node))
    {
	int nr = alias_display_nr(node);
	if (nr > 0)
	{
	    DispNode *dn = disp_graph->get(nr);
	    if (dn != 0)
	    {
		if (node->selected() != dn->selected())
		{
		    dn->selected() = node->selected();
		    graphEditRedrawNode(graph_edit, dn->nodeptr());
		}
	    }
	}
    }

    refresh_args();
    refresh_display_list();
}


//-----------------------------------------------------------------------
// Sorting nodes for layout
//-----------------------------------------------------------------------

void DataDisp::CompareNodesCB(Widget, XtPointer, XtPointer call_data)
{
    GraphEditCompareNodesInfo *info = (GraphEditCompareNodesInfo *)call_data;

    BoxGraphNode *node1 = ptr_cast(BoxGraphNode, info->node1);
    BoxGraphNode *node2 = ptr_cast(BoxGraphNode, info->node2);

    DispNode *disp1 = 0;
    DispNode *disp2 = 0;

    if (node1 && node2)
    {
	int nr1 = disp_graph->get_nr(node1);
	int nr2 = disp_graph->get_nr(node2);

	disp1 = disp_graph->get(nr1);
	disp2 = disp_graph->get(nr2);
    }

    if (disp1 && disp2)
    {
	string name1 = disp1->name();
	string name2 = disp2->name();

	char *p1 = (char *)name1;
	char *p2 = (char *)name2;

	while (*p1 && *p2 && *p1 == *p2)
	    p1++, p2++;

	if (isdigit(*p1) && isdigit(*p2))
	{
	    // Compare numerals numerically
	    long i1 = strtol(p1, NULL, 0);
	    long i2 = strtol(p2, NULL, 0);

	    info->result = i1 - i2;
	}
	else
	{
	    // Simple string comparison
	    info->result = *p1 - *p2;
	}
    }
    else
    {
	if (disp1)
	{
	    // Known nodes are ``larger'' than unknown nodes
	    info->result = 1;
	}
	else if (disp2) 
	{
	    // Known nodes are ``larger'' than unknown nodes
	    info->result = -1;
	}
	else
	{
	    // Comparing the pointer values will keep pointers constant
	    info->result = long(info->node1) - long(info->node2);
	}
    }

#if LOG_COMPARE
    if (disp1 && disp2)
    {
	clog << "Comparing " << disp1->name() 
	     << " and " << disp2->name() << " yields " << info->result << "\n";
    }
    else
    {
	clog << "Cannot compare: unknown nodes\n";
    }
#endif
}


//-----------------------------------------------------------------------
// Handle GDB input / output
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create new Display(s)
//-----------------------------------------------------------------------------

#if RUNTIME_REGEX
static regex rxmore_than_one ("\\[-?[0-9]+\\.\\.-?[0-9]+\\]");
#endif

void DataDisp::again_new_displaySQ (XtPointer client_data, XtIntervalId *)
{
    NewDisplayInfo *info = (NewDisplayInfo *)client_data;
    new_displaySQ(info->display_expression, info->scope, info->point_ptr, 
		  info->depends_on, info->origin, info->verbose);
    delete info;
}

void DataDisp::new_displaySQ (string display_expression,
			      string scope, BoxPoint *p,
			      string depends_on, Widget origin,
			      bool verbose)
{
    // Check arguments
    if (depends_on != "")
    {
	int depend_nr = disp_graph->get_by_name(depends_on);
	if (depend_nr == 0)
	{
	    if (verbose)
		post_gdb_message("No display named " 
				 + quote(depends_on) + ".\n");
	    return;
	}
	DispNode *dn = disp_graph->get(depend_nr);
	if (dn == 0)
	{
	    if (verbose)
		post_gdb_message("No display number " + 
				 itostring(depend_nr) + ".\n");
	    return;
	}
    }

    NewDisplayInfo *info = new NewDisplayInfo;
    info->display_expression = display_expression;
    info->scope = scope;
    info->verbose = verbose;
    if (p != 0)
    {
	info->point = *p;
	info->point_ptr = &info->point;
    }
    else
    {
	info->point = BoxPoint();
	info->point_ptr = 0;
    }
    info->depends_on = depends_on;
    info->origin     = origin;

    static Delay *reading_delay = 0;
    if (!DispBox::vsllib_initialized)
    {
	// We don't have the VSL library yet.  Try again later.
	if (VSLLib::background != 0)
	{
	    reading_delay = new StatusDelay("Reading VSL library");

	    // Disable background processing
	    VSLLib::background = 0;
	}

	// As soon as the VSL library will be completely read, we
	// shall enter the main DDD event loop and get called again.
	XtAppAddTimeOut(XtWidgetToApplicationContext(graph_edit),
			100, again_new_displaySQ, info);
	return;
    }
    delete reading_delay;
    reading_delay = 0;

    if (origin)
	set_last_origin(origin);

    if (display_expression == "")
	return;

    if (is_user_command(display_expression))
    {
	// User-defined display
	string cmd = user_command(display_expression);
	gdb_command(cmd, last_origin, new_user_displayOQC, info);
    }
    else
    {
	// Data display
	if (display_expression.contains (rxmore_than_one))
	{
	    new_data_displaysSQA (display_expression, info);
	    return;
	}

	switch (gdb->type())
	{
	case GDB:
	    {
		string cmd = gdb->display_command(display_expression);
		gdb_command(cmd, last_origin, new_data_displayOQC, info);
	    }
	    break;

	case DBX:
	case XDB:
	case JDB:
	    {
		gdb_question(gdb->display_command(display_expression));
		string cmd;
		cmd = gdb->print_command(display_expression);
		gdb_command(cmd, last_origin, new_data_displayOQC, info);
	    }
	    break;
	}
    }
}

// Get display number and name from ANSWER; store them in NR and NAME
void DataDisp::read_number_and_name(string& answer, string& nr, string& name)
{
    nr   = "";
    name = "";

    switch(gdb->type())
    {
    case GDB:
	nr = read_disp_nr_str(answer, gdb);
	if (nr != "")
	    name = read_disp_name(answer, gdb);
	break;

    case DBX:
    case XDB:
    case JDB:
	name = read_disp_name(answer, gdb);
	if (gdb->has_display_command())
	{
	    // Fetch number from `display' output
	    string ans = gdb_question(gdb->display_command());
	    int index = ans.index(name + "\n", -1);
	    if (index > 0)
	    {
		while (index > 0 && ans[index - 1] != '\n')
		    index--;
		ans = ans.from(index);
		int n = get_nr(ans);
		nr = itostring(n);
	    }

	    if (nr == "")
	    {
		// Could not determine number
		post_warning("Could not determine number of display " 
			     + quote(name), 
			     "no_display_number_warning", last_origin);
	    }
	}
	
	if (nr == "")
	{
	    // Assign a default number
	    nr = itostring(next_ddd_display_number++);
	}
	break;
    }
}

void DataDisp::new_display(string display_expression, BoxPoint *p,
			   string depends_on, Widget origin)
{
    string cmd = "graph display " + display_expression;
    if (p != 0 && *p != BoxPoint())
	cmd += " at (" + itostring((*p)[X]) + ", " + itostring((*p)[Y]) + ")";
    if (depends_on != "")
	cmd += " dependent on " + depends_on;

    gdb_command(cmd, origin);
}


//-----------------------------------------------------------------------------
// Show progress when creating or updating
//-----------------------------------------------------------------------------

struct StatusShower {
    string msg;			// The message shown
    StatusDelay *delay;		// The delay shown
    int current;		// Current data to be processed
    int base;			// Data already processed
    int total;			// Total of data to be processed
    int last_shown;		// Last shown amount
    bool (*old_background)(int); // DispValue bg proc
    bool aborted;		// True iff bg proc aborted

    // Show delay when updating from at least THRESHOLD characters.
    static const int THRESHOLD;

    bool process(int remaining_length);

    static StatusShower *active; // Currently active object
    static bool _process(int remaining_length)
    {
	return active->process(remaining_length);
    }

    StatusShower(const string& _msg)
	: msg(_msg),
	  delay(0), current(0), base(0), total(0), last_shown(0),
	  old_background(DispValue::background),
	  aborted(false)
    {
	DispValue::background = _process;
	active = (StatusShower *)this;
	delay = new StatusDelay(msg);
    }

    ~StatusShower()
    {
	DispValue::background = old_background;
	active = 0;
	if (aborted)
	    delay->outcome = "aborted";
	delete delay;
    }

private:
    StatusShower(const StatusShower&)
	: msg(),
	  delay(0), current(0), base(0), total(0), last_shown(0),
	  old_background(0),
	  aborted(false)
    {
	assert(0);
    }

    StatusShower& operator = (const StatusShower&)
    {
	assert(0); return *this;
    }
};

const int StatusShower::THRESHOLD = 512;

StatusShower *StatusShower::active = 0;

bool StatusShower::process(int remaining_length)
{
    int processed = base + current - remaining_length;

#if 0
    clog << "Processed " << processed << "/" <<  total << " characters\n";
#endif

    if (processed - last_shown >= THRESHOLD)
    {
	// Another THRESHOLD characters processed.  Wow!
	int percent = (processed * 100) / total;
	set_status(msg + "... (" + itostring(percent) + "% processed)", true);
	last_shown = processed;
    }

    // Interrupt if emergency
    if (!aborted && process_emergencies())
	aborted = true;
    return aborted;
}


//-----------------------------------------------------------------------------
// Create new data and user nodes
//-----------------------------------------------------------------------------

DispNode *DataDisp::new_data_node(const string& given_name,
				  const string& scope,
				  const string& answer)
{
    string value = answer;
    string nr_s;
    string display_name;
    read_number_and_name(value, nr_s, display_name);

    int nr = get_nr(nr_s);
    if (nr == 0 || display_name == "")
    {
	post_gdb_message(answer, last_origin);
	return 0;
    }

    // Naming a data display after the GDB display name cause trouble
    // when displaying functions: `display tree_test' creates a
    // display named `tree_test(void)', and while `print tree_test'
    // works fine, `print tree_test(void)' fails.  We may use quotes,
    // as in `print 'tree_test(void)'', but it is too hard to
    // determine where quotes are needed, and where not - just
    // consider `print tree_test(42)'.  Hence, if a function call
    // occurs in an expression, we use the name specified by the user,
    // not the name supplied by GDB.

    // Upon some occasions, GDB gives names like 
    // `{<text variable, no debug info>} 0x2270 <main>'.  In such cases,
    // also use the user-given name instead.
#if RUNTIME_REGEX
    static regex rxfunction_call("[a-zA-Z0-9_$][(]");
#endif
    string title = display_name;
    if (title.contains(rxfunction_call) 
	|| title.contains('{')
	|| title.contains('}'))
	title = given_name;

    bool disabling_occurred = false;
    if (is_disabling(value, gdb))
    {
	string error_msg = get_disp_value_str(value, gdb);
	post_gdb_message(error_msg, last_origin);
	value = "";
	disabling_occurred = true;
    }

    StatusShower s("Creating display");
    s.total   = value.length();
    s.current = value.length();

    DispNode *dn = new DispNode(nr, title, scope, value);
    if (disabling_occurred)
    {
	dn->disable();
	dn->make_active();
    }
    return dn;
}

DispNode *DataDisp::new_user_node(const string& name,
				  const string& /* scope */,
				  const string& answer)
{
    // Assign a default number
    int nr = -(next_ddd_display_number++);

    StatusShower s("Creating status display");
    s.total   = answer.length();
    s.current = answer.length();

    // User displays work regardless of scope
    return new DispNode(nr, name, "", answer);
}

// Create new data display from ANSWER
void DataDisp::new_data_displayOQC (const string& answer, void* data)
{
    NewDisplayInfo *info = (NewDisplayInfo *)data;

    if (answer == NO_GDB_ANSWER)
    {
	delete info;		// Command was canceled
	return;
    }

    if (answer == "")
    {
	if (gdb->has_display_command())
	{
	    // No display output (GDB bug).  Refresh displays explicitly.
	    gdb_command(gdb->display_command(), last_origin,
			new_data_display_extraOQC, data);
	}
	else
	{
	    delete info;
	}
	return;
    }

    if (!contains_display (answer, gdb))
    {
	if (info->verbose)
	    post_gdb_message(answer, last_origin);
	delete info;
	return;
    }

    // Unselect all nodes
    for (GraphNode *gn = disp_graph->firstNode();
	 gn != 0; gn = disp_graph->nextNode(gn))
    {
	gn->selected() = false;
    }

    // Create new DispNode
    string ans = answer;
    DispNode *dn = new_data_node(info->display_expression, info->scope, ans);
    if (dn == 0)
    {
	delete info;
	return;
    }

    // Determine position
    int depend_nr = disp_graph->get_by_name(info->depends_on);
    BoxPoint box_point = info->point;
    if (box_point == BoxPoint())
	box_point = disp_graph->default_pos(dn, graph_edit, depend_nr);
    dn->moveTo(box_point);
    dn->selected() = true;

    // Insert node into graph
    disp_graph->insert(dn->disp_nr(), dn, depend_nr);

    refresh_addr(dn);
    refresh_graph_edit();

    if (info->verbose)
	prompt();

    delete info;
}

// Create new user display from ANSWER
void DataDisp::new_user_displayOQC (const string& answer, void* data)
{
    NewDisplayInfo *info = (NewDisplayInfo *)data;

    if (answer == NO_GDB_ANSWER)
    {
	delete info;		// Command was canceled
	return;
    }

    // Unselect all nodes
    for (GraphNode *gn = disp_graph->firstNode();
	 gn != 0; gn = disp_graph->nextNode(gn))
    {
	gn->selected() = false;
    }

    // Create new user node and issue `disabling' messages
    string ans = answer;
    DispNode *dn = new_user_node(info->display_expression, info->scope, ans);
    if (dn != 0)
    {
	// Determine new position
	int depend_nr = disp_graph->get_by_name(info->depends_on);
	BoxPoint box_point = info->point;
	if (box_point == BoxPoint())
	    box_point = disp_graph->default_pos(dn, graph_edit, depend_nr);
	dn->moveTo(box_point);
	dn->selected() = true;

	// Insert into graph
	disp_graph->insert(dn->disp_nr(), dn, depend_nr);

	refresh_addr(dn);
	refresh_graph_edit();
	update_infos();
    }

    if (info->verbose)
	prompt();

    delete info;
}

// Create new display value from `display' output
void DataDisp::new_data_display_extraOQC (const string& answer, void* data)
{
    NewDisplayInfo *info = (NewDisplayInfo *)data;

    if (answer == NO_GDB_ANSWER)
    {
	delete info;		// Command was canceled
	return;
    }

    // The new display is the first one
    string ans = answer;
    string display = read_next_display(ans, gdb);

    if (display != "")
	new_data_displayOQC(display, data);
}


//-----------------------------------------------------------------------------
// Create multiple displays, using the [FROM..TO] syntax
//-----------------------------------------------------------------------------

void DataDisp::new_data_displaysSQA (string display_expression,
				     void *data)
{
    NewDisplayInfo *info = (NewDisplayInfo *)data;
    assert (display_expression.contains (rxmore_than_one));

    // Create individual display expressions and process entire array
    string prefix  = display_expression.before(rxmore_than_one);
    string postfix = display_expression.after(rxmore_than_one);
    string range   = display_expression.from(rxmore_than_one);
    range.del("[");
    int start = ::get_nr(range);
    range = range.after("..");
    int stop = ::get_nr(range);

    if (start > stop)
    {
	post_error("Invalid range in " + quote(display_expression), 
		   "invalid_range_error");
	delete info;
	return;
    }

    assert (stop >= start);

    StringArray display_cmds;
    StringArray print_cmds;

    for (int i = start; i < stop + 1; i++)
    {
	string expr = prefix + "[" + itostring (i) + "]" + postfix;
	info->display_expressions += expr;
	display_cmds              += gdb->display_command(expr);
	print_cmds                += gdb->print_command(expr);
    }

    VoidArray dummy;
    while (dummy.size() < display_cmds.size())
	dummy += (void *)0;

    switch (gdb->type())
    {
    case GDB:
	{
	    bool ok = 
		gdb->send_qu_array (display_cmds, dummy, display_cmds.size(),
				    new_data_displaysOQAC, info);
	    if (!ok)
		post_gdb_busy(last_origin);
	}
	break;

    case DBX:
    case XDB:
    case JDB:
	{
	    for (int i = 0; i < display_cmds.size(); i++)
		gdb_question(display_cmds[i]);

	    bool ok = gdb->send_qu_array (print_cmds,
					  dummy,
					  print_cmds.size(),
					  new_data_displaysOQAC,
					  info);
	    if (!ok)
		post_gdb_busy(last_origin);
	}
	break;
    }
}

void DataDisp::new_data_displaysOQAC (const StringArray& answers,
				      const VoidArray& /* qu_datas */,
				      void*  data)
{
    int count = answers.size();

    // Unselect all nodes
    for (GraphNode *gn = disp_graph->firstNode();
	 gn != 0; gn = disp_graph->nextNode(gn))
    {
	gn->selected() = false;
    }

    NewDisplayInfo *info = (NewDisplayInfo *)data;

    // Create and select new nodes
    int depend_nr = disp_graph->get_by_name(info->depends_on);
    for (int i = 0; i < count; i++)
    {
	if (!contains_display(answers[i], gdb))
	{
	    // Looks like an error message
	    post_gdb_message(answers[i], last_origin);
	}
	else
	{
	    // Create new display and remember disabling-message
	    DispNode *dn = new_data_node(info->display_expressions[i],
					 info->scope, answers[i]);
	    if (dn == 0)
		continue;

	    BoxPoint box_point = info->point;
	    if (box_point == BoxPoint())
	    {
		box_point = disp_graph->default_pos(dn, graph_edit, depend_nr);
	    }
	    dn->moveTo(box_point);
	    dn->selected() = true;

	    // Insert into graph
	    disp_graph->insert(dn->disp_nr(), dn, depend_nr);
	}
    }

    refresh_addr();
    refresh_graph_edit();

    if (info->verbose)
	prompt();

    delete info;
}


//-----------------------------------------------------------------------------
// Refresh graph
//-----------------------------------------------------------------------------

class RefreshInfo {
public:
    bool verbose;

    RefreshInfo()
	: verbose(false)
    {}

    ~RefreshInfo()
    {}

private:
    RefreshInfo(const RefreshInfo&)
	: verbose(false)
    {
	assert(0);
    }
    RefreshInfo& operator = (const RefreshInfo&)
    {
	assert(0); return *this;
    }
};

int DataDisp::add_refresh_data_commands(StringArray& cmds)
{
    int initial_size = cmds.size();

    switch (gdb->type())
    {
    case DBX:
    case XDB:
    case JDB:
	{
	    MapRef ref;
	    for (DispNode* dn = disp_graph->first(ref); 
		 dn != 0;
		 dn = disp_graph->next(ref))
	    {
		if (!dn->is_user_command())
		    cmds += gdb->print_command(dn->name());
	    }
	}
	break;

    case GDB:
	cmds += gdb->display_command();
	break;
    }

    return cmds.size() - initial_size;
}

int DataDisp::add_refresh_user_commands(StringArray& cmds)
{
    int initial_size = cmds.size();

    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->is_user_command() && dn->enabled())
	    cmds += dn->user_command();
    }

    return cmds.size() - initial_size;
}

void DataDisp::refresh_display(Widget origin)
{
    gdb_command("graph refresh", origin);
}

#define PROCESS_INFO_DISPLAY 0
#define PROCESS_DATA         1
#define PROCESS_USER         2
#define PROCESS_ADDR         3

void DataDisp::refresh_displaySQ(Widget origin, bool verbose)
{
    if (origin)
	set_last_origin(origin);

    // Process all displays
    StringArray cmds;
    VoidArray dummy;

    if (gdb->type() == GDB)
	cmds += "info display";
    while (dummy.size() < cmds.size())
	dummy += (void *)PROCESS_INFO_DISPLAY;

    add_refresh_data_commands(cmds);
    while (dummy.size() < cmds.size())
	dummy += (void *)PROCESS_DATA;

    add_refresh_user_commands(cmds);
    while (dummy.size() < cmds.size())
	dummy += (void *)PROCESS_USER;

    add_refresh_addr_commands(cmds);
    while (dummy.size() < cmds.size())
	dummy += (void *)PROCESS_ADDR;

    static RefreshInfo info;
    info.verbose = verbose;

    bool ok = gdb->send_qu_array(cmds, dummy, cmds.size(), 
				 refresh_displayOQAC, (void *)&info);

    if (!ok || cmds.size() == 0)
    {
	// Simply redraw display
	refresh_graph_edit();
	if (verbose)
	    prompt();
    }
}

void DataDisp::refresh_displayOQAC (const StringArray& answers,
				    const VoidArray& qu_datas,
				    void*  data)
{
    int count = answers.size();

    string data_answers;
    int data_answers_seen = 0;
    StringArray user_answers;
    StringArray addr_answers;

    RefreshInfo *info = (RefreshInfo *)data;

    for (int i = 0; i < count; i++)
    {
	switch (int(qu_datas[i]))
	{
	case PROCESS_INFO_DISPLAY:
	    // Process 'info display' output (delete displays if necessary)
	    process_info_display(answers[i]);
	    break;

	case PROCESS_DATA:
	    data_answers += answers[i];
	    data_answers_seen++;
	    break;

	case PROCESS_USER:
	    user_answers += answers[i];
	    break;

	case PROCESS_ADDR:
	    addr_answers += answers[i];
	    break;

	default:
	    assert(0);
	    break;
	}
    }

    // Process `display', user command, and addr command output
    if (data_answers_seen > 0)
    {
	bool disabling_occurred = false;
	process_displays(data_answers, disabling_occurred);

	// If we had a `disabling' message, refresh displays once more
	if (disabling_occurred)
	{
	    refresh_displaySQ(0, info->verbose);
	    info->verbose = false;	// No more prompts
	}
    }

    if (user_answers.size() > 0)
	process_user(user_answers);

    if (addr_answers.size() > 0)
    {
	force_check_aliases = true;
	process_addr(addr_answers);
    }

    if (info->verbose)
	prompt();
}



//-----------------------------------------------------------------------------
// Disabling Displays
//-----------------------------------------------------------------------------

// Convert A to a space-separated string
string DataDisp::numbers(IntArray& a)
{
    sort(a);

    string ret;
    for (int i = 0; i < a.size(); i++)
    {
	if (i > 0)
	    ret += " ";
	ret += itostring(a[i]);
    }

    return ret;
}

// Sort and verify the display numbers in DISPLAY_NRS
void DataDisp::sort_and_check(IntArray& display_nrs)
{
    sort(display_nrs);

    for (int i = 0; i < display_nrs.size(); i++)
    {
	DispNode *dn = disp_graph->get(display_nrs[i]);
	if (dn == 0)
	{
	    post_gdb_message("No display number " 
			     + itostring(display_nrs[i]) + ".\n");
	    display_nrs[i] = 0;
	}
    }
}

// For all nodes in DISPLAY_NRS, add their aliases
void DataDisp::add_aliases(IntArray& display_nrs)
{
    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->nodeptr()->hidden())
	{
	    bool have_alias = false;
	    bool need_alias = false;

	    for (int i = 0; i < display_nrs.size(); i++)
	    {
		if (display_nrs[i] == dn->disp_nr())
		    have_alias = true;
		if (display_nrs[i] == dn->alias_of)
		    need_alias = true;
	    }

	    if (need_alias && !have_alias)
		display_nrs += dn->disp_nr();
	}
    }
}

void DataDisp::disable_display(IntArray& display_nrs)
{
    add_aliases(display_nrs);

    if (display_nrs.size() > 0)
	gdb_command("graph disable display " + numbers(display_nrs));
}

void DataDisp::disable_displaySQ(IntArray& display_nrs, bool verbose)
{
    sort_and_check(display_nrs);

    int disabled_data_displays = 0;
    int i;
    string cmd = "disable display";
    for (i = 0; i < display_nrs.size(); i++)
    {
	if (gdb->type() == GDB && display_nrs[i] > 0)
	{
	    cmd += " " + itostring(display_nrs[i]);
	    disabled_data_displays++;
	}
    }

    if (disabled_data_displays > 0)
	gdb_command(cmd, last_origin, disable_displayOQC, (void *)verbose);

    int disabled_user_displays = 0;
    for (i = 0; i < display_nrs.size(); i++)
    {
	DispNode *dn = disp_graph->get(display_nrs[i]);
	if (dn != 0 && dn->enabled())
	{
	    dn->disable();
	    disabled_user_displays++;
	}
    }

    if (disabled_data_displays == 0)
    {
	if (disabled_user_displays > 0)
	    refresh_graph_edit();
	if (verbose)
	    prompt();
    }
}

void DataDisp::disable_displayOQC (const string& answer, void *data)
{
    if (answer == NO_GDB_ANSWER)
	return;			// Command was canceled

    bool verbose = bool(data);

    if (verbose)
	post_gdb_message(answer);

    refresh_graph_edit();
}


//-----------------------------------------------------------------------------
// Enable Displays
//-----------------------------------------------------------------------------

void DataDisp::enable_display(IntArray& display_nrs)
{
    add_aliases(display_nrs);

    if (display_nrs.size() > 0)
	gdb_command("graph enable display " + numbers(display_nrs));
}

void DataDisp::enable_displaySQ(IntArray& display_nrs, bool verbose)
{
    sort_and_check(display_nrs);

    int enabled_data_displays = 0;
    int i;
    string cmd = "enable display";
    for (i = 0; i < display_nrs.size(); i++)
    {
	if (gdb->type() == GDB && display_nrs[i] > 0)
	{
	    cmd += " " + itostring(display_nrs[i]);
	    enabled_data_displays++;
	}
    }

    if (enabled_data_displays > 0)
	gdb_command(cmd, last_origin, enable_displayOQC, (void *)verbose);

    int enabled_user_displays = 0;
    for (i = 0; i < display_nrs.size(); i++)
    {
	DispNode *dn = disp_graph->get(display_nrs[i]);
	if (dn != 0 && dn->is_user_command() && dn->disabled())
	{
	    dn->enable();
	    dn->value()->expandAll();
	    enabled_user_displays++;
	}
    }

    if (enabled_data_displays == 0)
    {
	if (enabled_user_displays > 0)
	    refresh_graph_edit();
	if (verbose)
	    prompt();
    }
}

void DataDisp::enable_displayOQC (const string& answer, void *data)
{
    if (answer == NO_GDB_ANSWER)
	return;			// Command was canceled

    bool verbose = bool(data);

    if (verbose)
	post_gdb_message(answer);

    refresh_displaySQ(0, false);
}


//-----------------------------------------------------------------------------
// Delete Displays
//-----------------------------------------------------------------------------

void DataDisp::delete_display(IntArray& display_nrs)
{
    if (display_nrs.size() > 0)
	gdb_command("graph undisplay " + numbers(display_nrs));
}

void DataDisp::delete_displaySQ(IntArray& display_nrs, bool verbose)
{
    sort_and_check(display_nrs);

    string cmd = "undisplay";

    int deleted_data_displays = 0;
    int i;
    for (i = 0; i < display_nrs.size(); i++)
    {
	if (display_nrs[i] > 0)
	{
	    if (deleted_data_displays++ > 0 && gdb->type() == DBX)
		cmd += ",";
	    cmd += " " + itostring(display_nrs[i]);
	}
    }

    if (deleted_data_displays > 0 && gdb->has_display_command())
	gdb_command(cmd, last_origin, delete_displayOQC, (void *)verbose);

    for (i = 0; i < display_nrs.size(); i++)
    {
	DispNode *dn = disp_graph->get(display_nrs[i]);
	if (dn != 0)
	    disp_graph->del(display_nrs[i]);
    }

    if (deleted_data_displays == 0 || !gdb->has_display_command())
    {
	// Refresh editor
	refresh_graph_edit();

	// Refresh addresses now
	force_check_aliases = true;
	refresh_addr();
	if (verbose)
	    prompt();
    }

    update_infos();
}

void DataDisp::delete_displayOQC (const string& answer, void *data)
{
    if (answer == NO_GDB_ANSWER)
	return;			// Command was canceled

    bool verbose = bool(data);

    string ans = answer;

    switch (gdb->type())
    {
    case GDB:
    case XDB:
    case JDB:
	break;

    case DBX:
	// Upon `undisplay', DBX redisplays remaining displays with values
	if (answer != "" && !answer.contains("no such expression"))
	{
	    bool disabling_occurred;
	    process_displays(ans, disabling_occurred);
	}
	break;
    }

    // Anything remaining is an error message
    if (verbose)
	post_gdb_message(ans);

    // Refresh editor
    refresh_graph_edit();

    // Refresh remaining addresses
    force_check_aliases = true;
    refresh_addr();
}



//-----------------------------------------------------------------------------
// Handle output of 'info display'
//-----------------------------------------------------------------------------

void DataDisp::process_info_display (string& info_display_answer)
{
    int disp_nr;
    StringMap info_disp_string_map;
    string *strptr;
    int max_disp_nr = 0;

    string next_disp_info = 
	read_first_disp_info (info_display_answer, gdb);
    while (next_disp_info != "")
    {
	disp_nr = get_positive_nr (next_disp_info);
	if (disp_nr >= 0)
	{
	    max_disp_nr = max(max_disp_nr, disp_nr);

	    if (disp_graph->contains(disp_nr)) 
	    {
		// This is a DDD display
		strptr = new string(get_info_disp_str(next_disp_info, gdb));
		info_disp_string_map.insert (disp_nr, strptr);
	    }
	}
	next_disp_info = 
	    read_next_disp_info(info_display_answer, gdb);
    }
    next_gdb_display_number = max(next_gdb_display_number, max_disp_nr + 1);


    // Process DDD displays
    bool changed = false;
    bool deleted = false;
    MapRef ref;
    for (int k = disp_graph->first_nr(ref); 
	     k != 0 ; 
	     k = disp_graph->next_nr(ref))
    {
	DispNode *dn = disp_graph->get(k);
	if (!dn->is_user_command())
	{
	    if (!info_disp_string_map.contains (k))
	    {
		// Display is not contained in `display' output
		disp_graph->del(k);
		changed = deleted = true;
	    }
	    else
	    {
		// Update values
		DispNode* dn = disp_graph->get (k);
		if (disp_is_disabled(*(info_disp_string_map.get (k)), gdb))
		{
		    if (dn->enabled())
		    {
			dn->disable();
			changed = true;
		    }
		}
		else
		{
		    if (dn->disabled())
		    {
			changed = true;
		    }
		}

		delete info_disp_string_map.get (k);
		info_disp_string_map.del (k);
	    }
	}
    }

    assert (info_disp_string_map.length() == 0);

    if (deleted)
    {
	force_check_aliases = true;
	refresh_addr();
    }
    if (changed)
	refresh_graph_edit();

    refresh_display_list();
}



//-----------------------------------------------------------------------------
// Process `display' output
//-----------------------------------------------------------------------------

string DataDisp::process_displays(string& displays,
				  bool& disabling_occurred)
{
    string not_my_displays;
    disabling_occurred = false;

    if (displays.length() == 0)
    {
	bool have_displays = false;
	MapRef ref;
	for (int k = disp_graph->first_nr(ref); 
	     k != 0;
	     k = disp_graph->next_nr(ref))
	{
	    DispNode* dn = disp_graph->get(k);
	    if (!dn->is_user_command() && dn->active())
	    {
		have_displays = true;
		break;
	    }
	}

	if (!have_displays)
	    return "";		// No data and no displays
    }

    StatusShower s("Updating displays");

    // Store graph displays in DISP_STRING_MAP; return all other
    // (text) displays as well as error messages
    int disp_nr = 0;
    StringMap disp_string_map;

#if LOG_DISPLAYS
    clog << "Updating displays " << quote(displays) << "...\n";
#endif

    string next_display = read_next_display (displays, gdb);
    while (next_display != "") 
    {
#if LOG_DISPLAYS
        clog << "Updating display " << quote(next_display);
#endif
	switch (gdb->type())
	{
	case GDB:
	    disp_nr = get_positive_nr (next_display);
	    break;

	case DBX:
	case XDB:
	case JDB:
	    {
		disp_nr = 0;
		string disp_name = next_display;
		disp_name = read_disp_name(disp_name, gdb);
		if (disp_name != "")
		{
		    MapRef ref;
		    for (DispNode* dn = disp_graph->first(ref); 
			 dn != 0;
			 dn = disp_graph->next(ref))
		    {
			if (dn->name() == disp_name)
			{
			    disp_nr = dn->disp_nr();
			    break;
			}
		    }
		}
	    }
	    break;		// FIXME
	}

#if LOG_DISPLAYS
	clog << " (number " << disp_nr << ")\n";
#endif

	if (is_disabling (next_display, gdb))
	{
	    // A display was disabled: record this and try again
	    disabling_occurred = true;
	    DispNode *dn = disp_graph->get(disp_nr);
	    if (disp_nr >= 0 && dn != 0)
	    {
		string error_msg = get_disp_value_str(next_display, gdb);
		post_gdb_message(error_msg);
		dn->make_active();
		dn->disable();
	    }
	    else
	    {
		not_my_displays = next_display; // Memorize this one only
	    }

	    // Clear DISP_STRING_MAP and try again
	    disp_string_map.delete_all_contents();

	    return not_my_displays;
	}

	if (is_invalid(next_display))
	{
	    // Display is not active
	}
	else if (disp_nr >= 0 && disp_graph->contains(disp_nr))
	{
	    string *strptr = new string(get_disp_value_str(next_display, gdb));
	    disp_string_map.insert(disp_nr, strptr);
	    s.total += strptr->length();
	}
	else 
	{
	    not_my_displays += next_display + '\n';
	}

	next_display = read_next_display (displays, gdb);
    }

    // Process own displays
    bool changed   = false;
    bool activated = false;

    // Change `active' status.  This must be done before updating
    // values, since inactive nodes must not be bumped after a resize.
    MapRef ref;
    int k;
    for (k = disp_graph->first_nr(ref); k != 0; k = disp_graph->next_nr(ref))
    {
	DispNode* dn = disp_graph->get(k);
	if (dn->is_user_command())
	    continue;

	if (disp_string_map.contains(k))
	{
	    if (disp_graph->make_active(k))
	    {
		// Now active
		changed = activated = true;
	    }
	}
	else
	{
	    // Node is no more part of `display' output
	    if (disp_graph->make_inactive(k))
	    {
		// Now inactive
		changed = true;
	    }
	}
    }

    // Update values
    for (k = disp_graph->first_nr(ref); k != 0; k = disp_graph->next_nr(ref))
    {
	DispNode* dn = disp_graph->get(k);
	if (dn->is_user_command())
	    continue;

	if (!disp_string_map.contains(k))
	    continue;

	// Update existing node
	string *strptr = disp_string_map.get(k);
	s.current = strptr->length();

	if (dn->update(*strptr))
	{
	    // New value
	    changed = true;
	}
	if (*strptr != "" && !(strptr->matches(rxwhite)))
	{
	    // After the `display' output, more info followed
	    // (e.g. the returned value when `finish'ing)
	    not_my_displays += strptr->after(rxwhite);
	}

	s.base += s.current;

	delete strptr;
	disp_string_map.del(k);
    }

    assert (disp_string_map.length() == 0);
    if (activated)
    {
	force_check_aliases = true;
	refresh_addr();
    }
    if (changed) 
	refresh_graph_edit();

    return not_my_displays;
}

// Handle output of user commands
void DataDisp::process_user (StringArray& answers)
{
    if (answers.size() == 0)
    {
	bool have_displays = false;
	MapRef ref;
	for (int k = disp_graph->first_nr(ref); 
	     k != 0;
	     k = disp_graph->next_nr(ref))
	{
	    DispNode* dn = disp_graph->get(k);
	    if (dn->is_user_command())
	    {
		have_displays = true;
		break;
	    }
	}

	if (!have_displays)
	    return;		// No data and no displays
    }

    StatusShower s("Updating status displays");

    int i;
    for (i = 0; i < answers.size(); i++)
	s.total += answers[i].length();

    i = 0;
    bool changed = false;
    MapRef ref;
    for (int k = disp_graph->first_nr(ref); 
	     k != 0 && i < answers.size();
	     k = disp_graph->next_nr(ref))
    {
	DispNode* dn = disp_graph->get(k);

	if (dn->is_user_command() && dn->enabled())
	{
	    string answer = answers[i++];

	    s.current = answer.length();

	    if (dn->update(answer))
		changed = true;

	    s.base += s.current;
	}
    }

    if (changed) 
	refresh_graph_edit();
}


//----------------------------------------------------------------------------
// Display Editor
//----------------------------------------------------------------------------

// Sort LABELS and SELECTED
static void sort(string labels[], bool selected[], int size)
{
    // Shell sort -- simple and fast
    int h = 1;
    do {
	h = h * 3 + 1;
    } while (h <= size);
    do {
	h /= 3;
	for (int i = h; i < size; i++)
	{
	    string v = labels[i];
	    bool   b = selected[i];
	    int    j;
	    for (j = i; 
		 j >= h && get_positive_nr(labels[j - h]) > get_positive_nr(v);
		 j -= h)
	    {
		labels[j]   = labels[j - h];
		selected[j] = selected[j - h];
	    }
	    if (i != j)
	    {
		labels[j]   = v;
		selected[j] = b;
	    }
	}
    } while (h != 1);
}

static string fmt(string s, unsigned size)
{
    if (s.length() > size)
	s = s.before(int(size));
    else if (s.length() < size)
	s += replicate(' ', size - s.length());

    assert(s.length() == size);
    return s;
}

static int max_width(const StringArray& s)
{
    int w = 0;

    for (int i = 0; i < s.size(); i++)
	w = max(w, s[i].length());

    return w;
}

// Create labels for the list
void DataDisp::refresh_display_list(bool silent)
{
    if (display_list_w == 0)
	return;

    int number_of_displays = disp_graph->count_all();

    StringArray nums;
    StringArray states;
    StringArray exprs;
    StringArray scopes;
    StringArray addrs;

    if (number_of_displays > 0)
    {
	// Add titles
	nums   += "Num";
	states += "State";
	exprs  += "Expression";
	scopes += "Scope";
	addrs  += "Address";
    }
    else
    {
	nums   += "";
	states += "";
	exprs  += "";
	scopes += "";
	addrs  += "";
    }

    MapRef ref;
    int k;
    for (k = disp_graph->first_nr(ref); k != 0; k = disp_graph->next_nr(ref))
    {
	DispNode* dn = disp_graph->get(k);

	nums += itostring(dn->disp_nr()) + ":";

	if (!dn->active())
	    states += "not active";
	else if (dn->nodeptr()->hidden())
	    states += "alias of " + itostring(dn->alias_of);
	else if (dn->enabled())
	    states += "enabled";
	else
	    states += "disabled";
	
	exprs += dn->name();
	scopes += dn->scope();
	addrs += dn->addr();
    }

    int nums_width   = max_width(nums);
    int exprs_width  = max_width(exprs)  + 1;
    int states_width = max_width(states) + 1;
    int scopes_width = max_width(scopes) + 1;
    int addrs_width  = max_width(addrs);

    string *label_list = new string[number_of_displays + 1];
    bool *selected     = new bool[number_of_displays + 1];

    // Set titles
    int display_count = 0;
    string line;
    if (number_of_displays > 0)
    {
	line = fmt(nums[display_count], nums_width) 
	    + " " + fmt(exprs[display_count], exprs_width)
	    + " " + fmt(states[display_count], states_width)
	    + " " + fmt(scopes[display_count], scopes_width);
	if (detect_aliases)
	    line += " " + fmt(addrs[display_count], addrs_width);
    }
    else
    {
	line = "No displays.                           ";
    }
    label_list[display_count] = line;
    selected[display_count] = false;
    display_count++;

    int selected_displays = 0;	// Number of selected displays
    int index_selected    = -1;	// Index of single selected display

    // Set contents
    for (k = disp_graph->first_nr(ref); k != 0; k = disp_graph->next_nr(ref))
    {
	DispNode* dn = disp_graph->get(k);
	line = fmt(nums[display_count], nums_width) 
	    + " " + fmt(exprs[display_count], exprs_width)
	    + " " + fmt(states[display_count], states_width)
	    + " " + fmt(scopes[display_count], scopes_width);
	if (detect_aliases)
	    line += " " + fmt(addrs[display_count], addrs_width);

	label_list[display_count] = line;
	selected[display_count]   = dn->selected();

	if (dn->selected())
	{ 
	    selected_displays++;
	    index_selected = display_count;
	}

	display_count++;
    }

    sort(label_list + 1, selected + 1, display_count - 1);

    setLabelList(display_list_w, label_list, selected, display_count, 
		 number_of_displays > 0, false);

    if (!silent)
    {
	// Setup status line
	MString msg;

	if (selected_displays == 1)
	{
	    // Show info about single selected display
	    DispNode *dn = selected_node();
	    DispValue *dv = 0;
	    if (dn != 0)
	    {
		if (dn->disabled())
		    dv = dn->value();
		else
		    dv = dn->selected_value();
	    }

	    if (dv != 0)
	    {
		DataDispCount count(disp_graph);

		// Value within display selected
		msg = rm("In display " + nums[index_selected] + " ");

		string title = dv->full_name();
		// shorten(title, DispBox::max_display_title_length);
		msg += tt(title);
		msg += rm(" (double-click to ");
		if (dv->type() == Pointer && !dv->collapsed())
		    msg += rm("dereference");
		else if (count.selected_collapsed > 0)
		    msg += rm("show more");
		else
		    msg += rm("hide");

		msg += rm(")");
	    }
	    else
	    {
		// Display selected
		msg = rm("Display " + nums[index_selected] + " ");

		string title = exprs[index_selected];
		// shorten(title, DispBox::max_display_title_length);
		msg += tt(title);

		msg += rm(" (" + states[index_selected]);
		if (scopes[index_selected] != "")
		{
		    msg += rm(", scope ");
		    msg += tt(scopes[index_selected]);
		}

		if (detect_aliases && addrs[index_selected] != "")
		{
		    msg += rm(", address ");
		    msg += tt(addrs[index_selected]);
		}

		msg += rm(")");
	    }
	}
	else if (selected_displays > 1)
	{
	    // Show info about multiple selected displays
	    msg = rm("Displays ");
	    IntArray displays;
	    for (k = disp_graph->first_nr(ref); k != 0; 
		 k = disp_graph->next_nr(ref))
	    {
		DispNode* dn = disp_graph->get(k);
		if (dn->selected())
		    displays += dn->disp_nr();
	    }

	    sort(displays);
	    assert(displays.size() == selected_displays);

	    for (k = 0; k < displays.size(); k++)
	    {
		if (k > 0)
		{
		    if (displays.size() == 2)
			msg += rm(" and ");
		    else if (k == displays.size() - 1)
			msg += rm(", and ");
		    else
			msg += rm(", ");
		}
		msg += rm(itostring(displays[k]));
	    }
	    msg += rm(" (" + itostring(displays.size()) 
			      + " displays)");

	}

	set_status_mstring(msg);
    }

    delete[] label_list;
    delete[] selected;
}


void DataDisp::EditDisplaysCB(Widget, XtPointer, XtPointer)
{
    manage_and_raise(edit_displays_dialog_w);
}


//----------------------------------------------------------------------------
// Value Editor
//----------------------------------------------------------------------------

void DataDisp::setCB(Widget w, XtPointer, XtPointer)
{
    if (!gdb->has_assign_command())
	return;

    DispValue *disp_value = selected_value();
    if (disp_value == 0)
	return;

    const string& name = disp_value->full_name();
    string value = gdb_question(gdb->print_command(name));
    if (value == NO_GDB_ANSWER)
    {
	post_gdb_busy(w);
	return;
    }
    if (is_invalid(value))
    {
	post_gdb_message(value);
	return;
    }

    value = get_disp_value_str(value, gdb);
#if RUNTIME_REGEX
    static regex rxnl(" *\n *");
#endif
    value.gsub(rxnl, " ");
    strip_final_blanks(value);

    MString prompt = bf("Set value of ") + tt(name);

    Arg args[10];
    int arg = 0;

    XtSetArg(args[arg], XmNdeleteResponse, XmDESTROY); arg++;
    XtSetArg(args[arg], XmNselectionLabelString, prompt.xmstring()); arg++;
    Widget set_dialog = 
	verify(XmCreatePromptDialog(find_shell(w), "set_dialog", args, arg));

    Delay::register_shell(set_dialog);

    if (lesstif_version <= 79)
	XtUnmanageChild(XmSelectionBoxGetChild(set_dialog,
					       XmDIALOG_APPLY_BUTTON));

    XtAddCallback(set_dialog, XmNokCallback,     setDCB, disp_value);
    XtAddCallback(set_dialog, XmNapplyCallback,  setDCB, disp_value);
    XtAddCallback(set_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);
    XtAddCallback(set_dialog, XmNcancelCallback, DestroyThisCB,
		  (XtPointer)set_dialog);

    Widget apply = XmSelectionBoxGetChild(set_dialog, XmDIALOG_APPLY_BUTTON);
    XtManageChild(apply);
    manage_and_raise(set_dialog);

    Widget text = XmSelectionBoxGetChild(set_dialog, XmDIALOG_TEXT);
    XmTextSetString(text, value);
}

void DataDisp::setDCB(Widget set_dialog, XtPointer client_data, XtPointer)
{
    DispValue *disp_value = (DispValue *)client_data;
    Widget text = XmSelectionBoxGetChild(set_dialog, XmDIALOG_TEXT);
    String value_s = XmTextGetString(text);
    string value(value_s);
    XtFree(value_s);

    gdb_command(gdb->assign_command(disp_value->full_name(), value),
		last_origin);
}


//----------------------------------------------------------------------------
// Helpers for user displays
//-----------------------------------------------------------------------------

bool DataDisp::have_user_display(const string& name)
{
    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->user_command() == name)
	    return true;
    }

    return false;
}

void DataDisp::new_user_display(const string& name)
{
    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->user_command() == name)
	    return;
    }

    gdb_command("graph display `" + name + "`", last_origin);
}

void DataDisp::delete_user_display(const string& name)
{
    IntArray killme;

    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->user_command() == name)
	{
	    killme += dn->disp_nr();
	}
    }

    delete_display(killme);
    refresh_graph_edit();
}


//----------------------------------------------------------------------------
// Language changed - re-label buttons
//----------------------------------------------------------------------------

void DataDisp::language_changedHP(Agent *source, void *, void *)
{
    GDBAgent *gdb = ptr_cast(GDBAgent, source);
    assert(gdb != 0);

    string label("Display " + gdb->dereferenced_expr("()"));

    set_label(node_popup[NodeItms::Dereference].widget, label);
    set_label(display_area[DisplayItms::Dereference].widget, label);
    set_label(graph_cmd_area[CmdItms::Dereference].widget, label);
}


//----------------------------------------------------------------------------
// Alias Detection
//----------------------------------------------------------------------------

// True iff aliases are to be checked regardless of address changes
bool DataDisp::force_check_aliases = false;

// Set whether aliases are to be detected
void DataDisp::set_detect_aliases(bool value)
{
    if (value == detect_aliases)
	return;

    detect_aliases = value;
    if (detect_aliases)
    {
	// Re-check for aliases
	force_check_aliases = true;
	refresh_addr();
    }
    else
    {
	bool changed = false;

	MapRef ref;
	for (int k = disp_graph->first_nr(ref); 
	     k != 0; 
	     k = disp_graph->next_nr(ref))
	{
	    // Unmerge all displays
	    changed = unmerge_display(k) || changed;
	}

	if (changed)
	    refresh_graph_edit();
    }
}

// Add address-printing commands to CMDS
int DataDisp::add_refresh_addr_commands(StringArray& cmds, DispNode *dn)
{
    if (!detect_aliases)
	return 0;

    int initial_size = cmds.size();

    if (dn != 0)
    {
 	if (dn->active() && !dn->is_user_command())
	{
	    string addr = gdb->address_expr(dn->name());
	    if (addr != "")
		cmds += gdb->print_command(addr);
	}
    }
    else
    {
	MapRef ref;
	for (dn = disp_graph->first(ref); 
	     dn != 0;
	     dn = disp_graph->next(ref))
	{
	    add_refresh_addr_commands(cmds, dn);
	}
    }

    return cmds.size() - initial_size;
}

// Refresh all addresses
void DataDisp::refresh_addr(DispNode *dn)
{
    if (refresh_addr_timer != 0)
    {
	XtRemoveTimeOut(refresh_addr_timer);
	refresh_addr_timer = 0;
	dn = 0;
    }

    RefreshAddrCB(XtPointer(dn), (XtIntervalId *)0);
}

void DataDisp::RefreshAddrCB(XtPointer client_data, XtIntervalId *id)
{
    if (id != 0)
    {
	assert (*id == refresh_addr_timer);
	refresh_addr_timer = 0;
    }

    DispNode *dn = (DispNode *)client_data;

    bool ok = false;
    bool sent = false;
    if (gdb->isReadyWithPrompt())
    {
	StringArray cmds;
	VoidArray dummy;

	add_refresh_addr_commands(cmds, dn);
	while (dummy.size() < cmds.size())
	    dummy += (void *)PROCESS_ADDR;

	static RefreshInfo info;
	info.verbose = false;
	ok = gdb->send_qu_array(cmds, dummy, cmds.size(), 
				refresh_displayOQAC, (void *)&info);

	sent = cmds.size() > 0;
    }

    if (!ok)
    {
	// Commands not sent - try again in 50 ms
	refresh_addr_timer = 
	    XtAppAddTimeOut(XtWidgetToApplicationContext(graph_edit),
			    50, RefreshAddrCB, client_data);
    }

    if (sent)
    {
	// At least one command sent - disable redisplay until we have
	// processed all addresses
	graphEditEnableRedisplay(graph_edit, False);
    }
}

// Handle output of addr commands
void DataDisp::process_addr (StringArray& answers)
{
    int i = 0;

    bool changed = false;

    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0 && i < answers.size();
	 dn = disp_graph->next(ref))
    {
	if (dn->active() && !dn->is_user_command())
	{
	    string addr = answers[i++];

	    if (addr.contains('(', 0) || addr.contains('{', 0))
	    {
		// Skip type prefix
		read_token(addr);
	    }

	    addr = addr.from(rxaddress);
	    addr = addr.through(rxaddress);

	    if (dn->addr() != addr)
	    {
		dn->set_addr(addr);
		changed = true;
	    }
	}
    }

    bool suppressed = false;
    if (changed || force_check_aliases)
    {
	suppressed = check_aliases();
	force_check_aliases = false;
    }

    // Re-enable redisplay
    graphEditEnableRedisplay(graph_edit, True);

    if (changed)
	refresh_display_list(suppressed);
}

// Check for aliases after change; return true iff displays were suppressed
bool DataDisp::check_aliases()
{
    if (!detect_aliases)
	return false;

    // Group displays into equivalence classes depending on their address.
    StringIntArrayAssoc equivalences;

    MapRef ref;
    for (int k = disp_graph->first_nr(ref); 
	     k != 0; 
	     k = disp_graph->next_nr(ref))
    {
	DispNode *dn = disp_graph->get(k);
	if (dn != 0 && dn->alias_ok())
	    equivalences[dn->addr()] += k;
    }

    // Merge displays with identical address.
    bool changed    = false;
    bool suppressed = false;

    for (StringIntArrayAssocIter iter(equivalences); iter.ok(); iter++)
    {
	string addr = iter.key();
	IntArray& displays = iter.value();

	assert(displays.size() > 0);

	if (addr == "" || displays.size() == 1)
	{
	    // No address or just one display -- unmerge them
	    for (int i = 0; i < displays.size(); i++)
		changed = unmerge_display(displays[i]) || changed;
	}
	else
	{
	    // Multiple displays at one location
	    merge_displays(displays, changed, suppressed);
	}
    }

    if (changed)
	refresh_graph_edit(suppressed);

    return suppressed;
}

// Return last change, or INT_MAX if hidden
int DataDisp::last_change_of_disp_nr(int disp_nr)
{
    DispNode *dn = disp_graph->get(disp_nr);
    assert(dn != 0);

    if (dn->nodeptr()->hidden())
	return INT_MAX;

    return dn->last_change();
}

// Sort DISP_NRS according to the last change
void DataDisp::sort_last_change(IntArray& disp_nrs)
{
    // Shell sort -- simple and fast
    int h = 1;
    do {
	h = h * 3 + 1;
    } while (h <= disp_nrs.size());
    do {
	h /= 3;
	for (int i = h; i < disp_nrs.size(); i++)
	{
	    int v = disp_nrs[i];
	    int j;
	    for (j = i; j >= h && last_change_of_disp_nr(disp_nrs[j - h]) > 
		                  last_change_of_disp_nr(v); j -= h)
		disp_nrs[j] = disp_nrs[j - h];
	    if (i != j)
		disp_nrs[j] = v;
	}
    } while (h != 1);
}

// Merge displays in DISPLAYS.  Set CHANGED iff changed.  Set
// SUPPRESSED if displays were suppressed.
void DataDisp::merge_displays(IntArray displays,
			      bool& changed, bool& suppressed)
{
    assert(displays.size() > 0);

    // Hide all aliases except the node which has changed least recently.
    sort_last_change(displays);

#if 0
    for (int i = 0; i < displays.size(); i++)
    {
	clog << "Last change of display " << displays[i]
	     << ": " << last_change_of_disp_nr(displays[i]) << "\n";
    }
#endif

    DispNode *d0 = disp_graph->get(displays[0]);
    if (d0->active() && d0->nodeptr()->hidden())
    {
	// All aliases are hidden.  Make sure we see at least the
	// least recently changed one.
	changed = unmerge_display(displays[0]) || changed;
    }

    IntArray suppressed_displays;
    for (int i = 1; i < displays.size(); i++)
    {
	int disp_nr = displays[i];
	DispNode *dn = disp_graph->get(disp_nr);

	if (!dn->active())
	    continue;		// Out of scope

	bool hidden = dn->nodeptr()->hidden();

	if (!hidden && dn->nodeptr()->firstTo() == 0)
	{
	    // There is no edge pointing at this node.  Don't merge it
	    // because it would simply disappear otherwise.
	    changed = unmerge_display(disp_nr) || changed;
	}
	else
	{
	    bool c = disp_graph->alias(graph_edit, displays[0], disp_nr);
	    if (c)
	    {
		if (!hidden)
		    suppressed_displays += disp_nr;
		changed = true;
	    }
	}
    }

    if (suppressed_displays.size() > 0)
    {
	suppressed = true;

	sort(suppressed_displays);

	// Some displays have been suppressed.  Generate appropriate message.
	MString msg = rm("Suppressing ");

	if (suppressed_displays.size() == 1)
	{
	    DispNode *node = disp_graph->get(suppressed_displays[0]);
	    msg += rm("display " + itostring(node->disp_nr()) + ": ");
	    msg += tt(node->name());
	}
	else if (suppressed_displays.size() == 2)
	{
	    msg += rm("displays "
		      + itostring(suppressed_displays[0])
		      + " and "
		      + itostring(suppressed_displays[1]));
	}
	else
	{
	    msg += rm("displays ");
	    for (int i = 1; i < suppressed_displays.size(); i++)
	    {
		if (i == suppressed_displays.size() - 1)
		    msg += rm(", and ");
		else if (i > 1)
		    msg += rm(", ");
		msg += rm(itostring(suppressed_displays[i]));
	    }
	}

	if (suppressed_displays.size() == 1)
	    msg += rm(" because it is an alias");
	else
	    msg += rm(" because they are aliases");

	DispNode *of = disp_graph->get(displays[0]);
	msg += rm(" of display " + itostring(of->disp_nr()) + ": ");
	msg += tt(of->name());

	set_status_mstring(msg);
    }
}

bool DataDisp::unmerge_display(int disp_nr)
{
    return disp_graph->unalias(disp_nr);
}

void DataDisp::PreLayoutCB(Widget w, XtPointer client_data, XtPointer)
{
    DataDisp *data_disp = (DataDisp *)client_data;

    if (data_disp->detect_aliases)
    {
	// Don't redisplay while or after layouting
	graphEditEnableRedisplay(w, False);
    }
}

// Re-enable aliases after layouting
void DataDisp::PostLayoutCB(Widget w, XtPointer client_data, XtPointer)
{
    DataDisp *data_disp = (DataDisp *)client_data;

    if (data_disp->detect_aliases)
    {
	// Unmerge and re-merge all displays
	MapRef ref;
	for (int k = disp_graph->first_nr(ref); 
	     k != 0; 
	     k = disp_graph->next_nr(ref))
	{
	    unmerge_display(k);
	}
	data_disp->check_aliases();

	// Okay - we can redisplay now
	graphEditEnableRedisplay(w, True);
	refresh_graph_edit();
    }
}

// True iff we have some selection
bool DataDisp::have_selection()
{
    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref);
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->selected())
	    return true;
    }
    return false;
}


//----------------------------------------------------------------------------
// Bumper
//----------------------------------------------------------------------------

bool DataDisp::bump_displays = true;

static bool Yes(RegionGraphNode *, const BoxSize&)
{
    return true;
}

// This one is called whenever NODE is to be resized to NEWSIZE
bool DataDisp::bump(RegionGraphNode *node, const BoxSize& newSize)
{
    if (!bump_displays)
	return true;		// Okay

    const GraphGC& gc = graphEditGetGraphGC(graph_edit);
    BoxRegion oldRegion = node->region(gc);

    // Do the resize; don't get called recursively
    RegionGraphNode::ResizeCB = Yes;
    node->resize(newSize);
    RegionGraphNode::ResizeCB = bump;

    // Let origin remain constant
    node->moveTo(node->originToPos(oldRegion.origin(), gc));

    // Move all nodes that are right or below NODE such that their
    // distance to NODE remains constant.

    // DELTA is the difference between old and new size
    BoxSize delta  = node->space(gc) - oldRegion.space();

    // NODE_ORIGIN is the (old) upper left corner of NODE
    BoxPoint node_origin = oldRegion.origin();

    // BUMPER is the (old) lower right corner of NODE
    BoxPoint node_bumper = oldRegion.origin() + oldRegion.space();

    for (GraphNode *r = disp_graph->firstNode(); 
	 r != 0; r = disp_graph->nextNode(r))
    {
	if (r == node)
	    continue;

	// If R is inactive, don't bump it
	// This makes the check O(n^2), where O(n) would suffice (FIXME).
	BoxGraphNode *b = ptr_cast(BoxGraphNode, r);

	if (b != 0)
	{
	    bool is_active = true;

	    MapRef ref;
	    for (DispNode* dn = disp_graph->first(ref); 
		 dn != 0;
		 dn = disp_graph->next(ref))
	    {
		if (b == dn->nodeptr())
		{
		    is_active = dn->active();
		    break;
		}
	    }

	    if (!is_active)
		continue;
	}

	// If ORIGIN (the upper left corner of R) is right of BUMPER,
	// move R DELTA units to the right.  If it is below BUMPER,
	// move R DELTA units down.

	BoxPoint r_origin = r->origin(gc);
	BoxPoint r_bumper = r->origin(gc) + r->space(gc);

	if (r_bumper[X] >= node_origin[X] && r_bumper[Y] >= node_origin[Y])
	{
	    if (r_origin[X] > node_bumper[X] && r_origin[Y] > node_bumper[Y])
		r->moveTo(r->pos() + delta);
	    else if (r_origin[X] > node_bumper[X])
		r->moveTo(r->pos() + BoxPoint(delta[X], 0));
	    else if (r_origin[Y] > node_bumper[Y])
		r->moveTo(r->pos() + BoxPoint(0, delta[Y]));
	    else
		r->moveTo(r->pos() + delta); // Overlapping nodes
	}
    }

    // All is done - don't use default behavior.
    return false;
}

//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------

DataDisp::DataDisp (XtAppContext app_context,
		    Widget parent,
		    String vsl_path,
		    String vsl_library,
		    String vsl_defs,
		    bool   panned)
{
    registerOwnConverters();

    // Init globals
    StringBox::fontTable     = new FontTable (XtDisplay(parent));
    DispBox::vsllib_name     = vsl_library;
    DispBox::vsllib_path     = vsl_path;
    DispBox::vsllib_defs     = vsl_defs;

    // Create graph
    disp_graph = new DispGraph();

    disp_graph->addHandler(DispGraph_Empty, no_displaysHP);

    // Crate graph editor
    Arg args[10];
    int arg = 0;
    XtSetArg (args[arg], XtNgraph, (Graph *)disp_graph); arg++;

    if (panned)
    {
	graph_edit = createPannedGraphEdit(parent, "graph_edit", args, arg);
	graph_form_w = pannerOfGraphEdit(graph_edit);
    }
    else
    {
	graph_edit = createScrolledGraphEdit(parent, "graph_edit", args, arg);
	graph_form_w = scrollerOfGraphEdit(graph_edit);
    }

    set_last_origin(graph_edit);

    // Create menus
    graph_popup_w = 
	MMcreatePopupMenu(graph_edit, "graph_popup", graph_popup);
    InstallButtonTips(graph_popup_w);

    node_popup_w = 
	MMcreatePopupMenu(graph_edit, "node_popup", node_popup);
    InstallButtonTips(node_popup_w);

    shortcut_popup_w = 
	MMcreatePopupMenu(graph_edit, "shortcut_popup", shortcut_popup1);
    InstallButtonTips(shortcut_popup_w);

    disp_graph->callHandlers();

    // Add actions
    XtAppAddActions (app_context, actions, XtNumber (actions));
    XtManageChild (graph_edit);

    // Create buttons
    graph_cmd_w = 
	verify(XmCreateRowColumn(parent, "graph_cmd_w", NULL, 0));

    Widget arg_label = create_arg_label(graph_cmd_w);
    graph_arg = new ArgField (graph_cmd_w, "graph_arg");
    XtAddCallback(arg_label, XmNactivateCallback, 
		  SelectionLostCB, XtPointer(0));

    registerOwnConverters();

    MMcreateWorkArea(graph_cmd_w, "graph_cmd_area", graph_cmd_area);
    MMaddCallbacks(graph_cmd_area);
    XtManageChild(graph_cmd_w);
    register_menu_shell(graph_cmd_area);

    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(graph_cmd_w, NULL, &size);
    unsigned char unit_type;
    XtVaGetValues(graph_cmd_w, XmNunitType, &unit_type, NULL);
    int new_height = XmConvertUnits(graph_cmd_w, XmVERTICAL, XmPIXELS, 
				    size.height, unit_type);
    XtVaSetValues(graph_cmd_w,
		  XmNpaneMaximum, new_height,
		  XmNpaneMinimum, new_height,
		  NULL);

    // Create (unmanaged) selection widget
    graph_selection_w =
	verify(XmCreateText(graph_cmd_w, "graph_selection", NULL, 0));
    XtAddCallback(graph_selection_w, XmNlosePrimaryCallback, 
		  SelectionLostCB, XtPointer(0));

    // Create display editor
    edit_displays_dialog_w =
	verify(XmCreatePromptDialog(find_shell(graph_edit), 
				    "edit_displays_dialog", 
				    NULL, 0));

    if (lesstif_version <= 79)
	XtUnmanageChild(XmSelectionBoxGetChild(edit_displays_dialog_w,
					       XmDIALOG_APPLY_BUTTON));

    Delay::register_shell(edit_displays_dialog_w);

    XtUnmanageChild(XmSelectionBoxGetChild(edit_displays_dialog_w,
					   XmDIALOG_TEXT));
    XtUnmanageChild(XmSelectionBoxGetChild(edit_displays_dialog_w,
					   XmDIALOG_CANCEL_BUTTON));
    XtUnmanageChild(XmSelectionBoxGetChild(edit_displays_dialog_w,
					   XmDIALOG_SELECTION_LABEL));

    Widget form1 = 
	verify(XmCreateRowColumn(edit_displays_dialog_w, "form1", NULL, 0));

    Widget label =
	verify(XmCreateLabel(form1, "displays", NULL, 0));

    Widget form2 = 
	verify(XmCreateRowColumn(form1, "form2", NULL, 0));

    arg = 0;
    display_list_w = 
	verify(XmCreateScrolledList(form2, "displays", args, arg));
    Widget buttons = 
	verify(MMcreateWorkArea(form2, "buttons", display_area));
    MMaddCallbacks (display_area);
    register_menu_shell(display_area);

    XtManageChild (buttons);
    XtManageChild (display_list_w);
    XtManageChild (label);
    XtManageChild (form2);
    XtManageChild (form1);

    // Add widget callbacks
    XtAddCallback(graph_edit, XtNpreSelectionCallback,
		  DoubleClickCB, XtPointer(this));
    XtAddCallback(graph_edit, XtNselectionChangedCallback,
		  UpdateDisplayEditorSelectionCB, XtPointer(this));
    XtAddCallback(graph_edit, XtNcompareNodesCallback,
		  CompareNodesCB, XtPointer(this));
    XtAddCallback(graph_edit, XtNpreLayoutCallback,
		  PreLayoutCB, XtPointer(this));
    XtAddCallback(graph_edit, XtNpostLayoutCallback,
		  PostLayoutCB, XtPointer(this));

    if (display_list_w)
    {
	XtAddCallback(display_list_w,
		      XmNsingleSelectionCallback,
		      UpdateGraphEditorSelectionCB,
		      0);
	XtAddCallback(display_list_w,
		      XmNmultipleSelectionCallback,
		      UpdateGraphEditorSelectionCB,
		      0);
	XtAddCallback(display_list_w,
		      XmNextendedSelectionCallback,
		      UpdateGraphEditorSelectionCB,
		      0);
	XtAddCallback(display_list_w,
		      XmNbrowseSelectionCallback,
		      UpdateGraphEditorSelectionCB,
		      0);
    }

    if (edit_displays_dialog_w)
    {
	XtAddCallback(edit_displays_dialog_w,
		      XmNokCallback,
		      UnmanageThisCB,
		      edit_displays_dialog_w);
	XtAddCallback(edit_displays_dialog_w,
		      XmNhelpCallback,
		      ImmediateHelpCB,
		      0);
    }

    // Add graph callbacks
    RegionGraphNode::ResizeCB = bump;

    // Reset argument field and display editor buttons
    set_args();
}
