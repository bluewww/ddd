// $Id$
// Data Display

// Copyright (C) 1995-1997 Technische Universitaet Braunschweig, Germany.
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
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char DataDisp_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#define LOG_DISPLAYS 0
#define LOG_COMPARE  0

//-----------------------------------------------------------------------------
// Implementation von DataDisp.h
//-----------------------------------------------------------------------------

#include "DataDisp.h"

// Misc includes
#include "GraphEdit.h"		// XtNgraph
#include "StringBox.h"		// StringBox::fontTable
#include "ScrolledGE.h"
#include "PannedGE.h"
#include "VSEFlags.h"
#include "VSLLib.h"
#include "assert.h"
#include "bool.h"
#include "converters.h"
#include "MString.h"
#include "DestroyCB.h"
#include "CompositeB.h"
#include "TagBox.h"
#include "cook.h"
#include "cmdtty.h"
#include "post.h"
#include "question.h"
#include "commandQ.h"
#include "StringMap.h"
#include "VoidArray.h"
#include "settings.h"
#include "status.h"
#include "PosBuffer.h"
#include "IntIntAA.h"
#include "shorten.h"
#include "charsets.h"
#include "DispGraph.h"
#include "AliasGE.h"

// Motif includes
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h>	// XmMenuPosition()
#include <Xm/SelectioB.h>	// XmCreatePromptDialog()
#include <Xm/TextF.h>		// XmTextFieldGetString()
#include <Xm/Text.h>
#include <Xm/Label.h>
#include <Xm/List.h>
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

// System includes
#include <iostream.h>
#include <fstream.h>		// ofstream
#include <iomanip.h>
#include <ctype.h>


//-----------------------------------------------------------------------
// Xt-Zeugs
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



// Popup-Menues - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct GraphItms { enum Itms {SelectAll, Refresh, NewArg, New}; };
MMDesc DataDisp::graph_popup[] =
{
    {"selectAll", MMPush,               {DataDisp::selectAllCB}},
    {"refresh",   MMPush,               {DataDisp::refreshCB}},
    {"new_arg",   MMPush | MMUnmanaged, {DataDisp::popup_new_argCB}},
    {"new",       MMPush,               {DataDisp::popup_newCB}},
    MMEnd
};

struct ValueItms { enum Itms {Dereference, Detail, Rotate, Dependent,
			      Dummy1, Set, Dummy2, Delete }; };

MMDesc DataDisp::node_popup[] =
{
    {"dereference",   MMPush,   {DataDisp::dereferenceCB}},
    {"detail",        MMPush,   {DataDisp::toggleDetailCB}},
    {"rotate",        MMPush,   {DataDisp::toggleRotateCB}},
    {"dependent",     MMPush,   {DataDisp::dependentCB}},
    MMSep,
    {"set",           MMPush,   {DataDisp::setCB}},
    MMSep,
    {"delete",        MMPush,   {DataDisp::deleteCB}},
    MMEnd
};

MMDesc DataDisp::graph_cmd_area[] =
{
    {"dereference",   MMPush | MMInsensitive, {DataDisp::dereferenceCB}},
    {"detail",        MMPush | MMInsensitive, {DataDisp::toggleDetailCB}},
    {"rotate",        MMPush | MMInsensitive, {DataDisp::toggleRotateCB}},
    {"dependent",     MMPush, {DataDisp::dependentCB}},
    MMSep,
    {"set",           MMPush | MMInsensitive, {DataDisp::setCB}},
    MMSep,
    {"delete",        MMPush | MMInsensitive, {DataDisp::deleteCB}},
    MMEnd
};

struct DisplayItms { enum Itms {Dependent, Dereference, 
				ShowDetail, HideDetail, Set, Delete}; };

MMDesc DataDisp::display_area[] =
{
    {"dependent",    MMPush,   {DataDisp::dependentCB }},
    {"dereference",  MMPush,   {DataDisp::dereferenceCB }},
    {"show_detail",  MMPush,   {DataDisp::showDetailCB}},
    {"hide_detail",  MMPush,   {DataDisp::hideDetailCB}},
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

bool DataDisp::detect_aliases = false;

int DataDisp::next_display_number = 1;

XtIntervalId DataDisp::refresh_args_timer       = 0;
XtIntervalId DataDisp::refresh_addr_timer       = 0;
XtIntervalId DataDisp::refresh_graph_edit_timer = 0;


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

static void set_label(Widget w, string label)
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

    string nr = disp_node_arg->disp_nr();
    new_display(display_expression, 0, get_nr(nr));
}

void DataDisp::toggleDetailCB(Widget dialog, XtPointer, XtPointer)
{
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
		continue;

	    if (dv->collapsedAll() > 0)
	    {
		dv->expandAll();

		if (dv == dn->value() && dn->disabled())
		{
		    // Enable display
		    string nr = dn->disp_nr();
		    disp_nrs += get_nr(nr);
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
		dv->collapse();

		if (dv == dn->value() && dn->enabled())
		{
		    // Disable display
		    string nr = dn->disp_nr();
		    disp_nrs += get_nr(nr);
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

void DataDisp::showDetailCB (Widget dialog, XtPointer, XtPointer)
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
		string nr = dn->disp_nr();
		disp_nrs += get_nr(nr);
	    }
		
	    DispValue *dv = dn->selected_value();
	    if (dv == 0)
		dv = dn->value();
	    if (dv == 0)
		continue;
	    
	    if (dv->collapsedAll() > 0)
	    {
		dv->expandAll();
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
	    if (dv == 0)
		continue;

	    if (dv == dn->value() && dn->enabled())
	    {
		string nr = dn->disp_nr();
		disp_nrs += get_nr(nr);
	    }

	    if (dv->expanded())
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

void DataDisp::toggleRotateCB(Widget w, XtPointer, XtPointer)
{
    set_last_origin(w);

    DispNode *disp_node_arg   = selected_node();
    DispValue *disp_value_arg = selected_value();
    if (disp_node_arg == 0 || disp_value_arg == 0)
	return;

    if (disp_value_arg->horizontal_aligned())
	disp_value_arg->align_vertical();
    else
	disp_value_arg->align_horizontal();

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
	    string nr = dn->disp_nr();
	    disp_nrs += get_nr(nr);
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
	    string nr_s = dn->disp_nr();
	    int nr = get_positive_nr(nr_s);
	    disp_nrs += nr;

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

    string nr = disp_node_arg->disp_nr();
    static int* disp_nr_ptr = new int;
    *disp_nr_ptr = get_nr(nr);

    static Widget dependent_display_dialog = 0;
    if (!dependent_display_dialog) {

	dependent_display_dialog = 
	    verify(XmCreatePromptDialog (graph_edit,
					 "dependent_display_dialog",
					 NULL, 0));
	Delay::register_shell(dependent_display_dialog);

	if (lesstif_hacks_enabled)
	    XtUnmanageChild(XmSelectionBoxGetChild(dependent_display_dialog,
						   XmDIALOG_APPLY_BUTTON));

	XtAddCallback (dependent_display_dialog, 
		       XmNhelpCallback, 
		       ImmediateHelpCB, 
		       NULL);
	XtAddCallback (dependent_display_dialog, 
		       XmNokCallback, 
		       dependent_displayDCB, 
		       disp_nr_ptr);
    }

    MString label(disp_value_arg->full_name());

    XtVaSetValues (dependent_display_dialog,
		   XmNtextString, label.xmstring(),
		   NULL);
    manage_and_raise(dependent_display_dialog);
}


void DataDisp::refreshCB(Widget w, XtPointer, XtPointer)
{
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

void DataDisp::newCB(Widget w, XtPointer, XtPointer)
{
    set_last_origin(w);
    new_displayCD();
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
	    string nr = dn->disp_nr();
	    disp_nrs += get_nr(nr);
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
	    string nr = dn->disp_nr();
	    disp_nrs += get_nr(nr);
	}
    }

    disable_display(disp_nrs);
}

//-----------------------------------------------------------------------------
// Popup-Callbacks
//-----------------------------------------------------------------------------
// ***************************************************************************
//
void DataDisp::popup_new_argCB (Widget    display_dialog,
				XtPointer client_data,
				XtPointer)
{
    set_last_origin(display_dialog);

    BoxPoint *p = (BoxPoint *) client_data;
    new_display(source_arg->get_string(), p);
}


// ***************************************************************************
//
void DataDisp::popup_newCB (Widget    display_dialog,
			    XtPointer client_data,
			    XtPointer)
{
    set_last_origin(display_dialog);

    BoxPoint *p = (BoxPoint *) client_data;
    new_displayCD(*p);
}


// ***************************************************************************
//
void DataDisp::dependent_displayDCB (Widget    dialog, 
				     XtPointer client_data, 
				     XtPointer call_data)
{
    set_last_origin(dialog);

    int* disp_nr_ptr = (int *) client_data;
    XmSelectionBoxCallbackStruct *cbs = 
	(XmSelectionBoxCallbackStruct *)call_data;

    char *input;
    switch (cbs->reason) 
    {
    case XmCR_OK :
	XmStringGetLtoR(cbs->value, MSTRING_DEFAULT_CHARSET, &input);
	if (input != "")
	    new_display(input, 0, *disp_nr_ptr);
	XtFree(input);
	break;
    default:
	// Falscher Fehler
	assert (0);
	break;
    }
}


// ***************************************************************************
// Wertet Dialog Callback aus und ruft new_displaySQ() entsprechend auf.
// 
void DataDisp::new_displayDCB (Widget    display_dialog,
			       XtPointer client_data,
			       XtPointer call_data)
{
    set_last_origin(display_dialog);

    BoxPoint* p = (BoxPoint *) client_data;
    XmSelectionBoxCallbackStruct *cbs = 
	(XmSelectionBoxCallbackStruct *)call_data;

    char *input;
    switch (cbs->reason) {
    case XmCR_OK :
	XmStringGetLtoR(cbs->value, MSTRING_DEFAULT_CHARSET, &input);
	if (string(input) != "")
	    new_display(input, p);
	XtFree(input);
	break;
    default:
	// Falscher Fehler
	assert (0);
	break;
    }
}


//-----------------------------------------------------------------------------
// Dialog-Erzeugung
//-----------------------------------------------------------------------------

// ***************************************************************************
// Erzeugt Dialog zur Eingabe einer neuen Display-Expression.
// Wird ein Boxpoint uebergeben, so wird der neue Knoten dorthin gesetzt.
// 
void DataDisp::new_displayCD (BoxPoint box_point)
{
    static BoxPoint* p;
    static Widget new_display_dialog = 0;
    if (!new_display_dialog) {
	p = new BoxPoint(box_point);
	new_display_dialog = 
	    verify(XmCreatePromptDialog (graph_edit,
					 "new_display_dialog",
					 NULL, 0));
	Delay::register_shell(new_display_dialog);

	if (lesstif_hacks_enabled)
	    XtUnmanageChild(XmSelectionBoxGetChild(new_display_dialog,
						   XmDIALOG_APPLY_BUTTON));

	XtAddCallback (new_display_dialog, 
		       XmNhelpCallback, 
		       ImmediateHelpCB, 
		       NULL);
	XtAddCallback (new_display_dialog,
		       XmNokCallback,
		       new_displayDCB,
		       p);
    }
    else {
	*p = box_point;
    }

    Widget text = XmSelectionBoxGetChild(new_display_dialog, XmDIALOG_TEXT);
    XmTextSetString(text, source_arg->get_string());

    manage_and_raise(new_display_dialog);
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
	// Graph is non-empty: make sure its shell is visible
	XtManageChild(graph_edit);
	initial_popup_shell(data_disp_shell);
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

void DataDisp::SelectionLostCB(Widget, XtPointer, XtPointer)
{
    // Selection lost - clear all highlights
    for (GraphNode *gn = disp_graph->firstNode();
	 gn != 0; gn = disp_graph->nextNode(gn))
    {
	BoxGraphNode *bgn = ptr_cast(BoxGraphNode, gn);
	if (bgn && bgn->selected())
	{
	    bgn->selected() = false;
	    graphEditRedrawNode(graph_edit, bgn);
	}
    }

    refresh_args();
    refresh_display_list();
}

//-----------------------------------------------------------------------------
// Action procs
//----------------------------------------------------------------------------

void DataDisp::graph_dereferenceAct (Widget w, XEvent*, String*, Cardinal*)
{
    dereferenceCB(w, 0, 0);
}

void DataDisp::graph_detailAct (Widget w, XEvent*, String*, Cardinal*)
{
    toggleDetailCB(w, 0, 0);
}

void DataDisp::graph_rotateAct (Widget w, XEvent*, String*, Cardinal*)
{
    toggleRotateCB(w, 0, 0);
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

void DataDisp::graph_popupAct (Widget, XEvent* event, String*, Cardinal*)
{
    static BoxPoint* p = 0;
    if (p == 0)
    {
	p = new BoxPoint;

	MMaddCallbacks(graph_popup, XtPointer(p));
	MMaddCallbacks(node_popup,  XtPointer(p));
    }
    *p = point(event);

    set_args(*p, SetSelection);

    Widget popup = 0;
    if (selected_node() == 0)
	popup = graph_popup_w;
    else
	popup = node_popup_w;

    XmMenuPosition(popup, &event->xbutton);
    XtManageChild(popup);
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
	    if (disp_value != selected_value())
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
    return dv ? dv : dn->value();
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
    
    int count_selected           = 0;
    int count_all                = 0;
    int count_selected_expanded  = 0;
    int count_selected_collapsed = 0;
    int count_selected_data      = 0;

    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	count_all++;

	if (dn->selected())
	{
	    count_selected++;
	    if (!dn->is_user_command())
		count_selected_data++;


	    if (dn->disabled())
		count_selected_collapsed++;
	    else
	    {
		DispValue *dv = dn->selected_value();
		if (dv == 0)
		    dv = dn->value();
		if (dv)
		{
		    count_selected_expanded  += int(dv->expanded());
		    count_selected_collapsed += dv->collapsedAll();
		}
	    }
	}
    }

    if (count_selected > 1)
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
    set_sensitive(graph_popup[GraphItms::Refresh].widget,   count_all > 0);
    set_sensitive(graph_popup[GraphItms::SelectAll].widget, count_all > 0);

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
	}
    }

    // Dereference
    set_sensitive(node_popup[ValueItms::Dereference].widget,
		  dereference_ok);
    set_sensitive(graph_cmd_area[ValueItms::Dereference].widget,
		  dereference_ok);
    set_sensitive(display_area[DisplayItms::Dereference].widget,
		  dereference_ok);

    // Dependent
    set_sensitive(graph_cmd_area[ValueItms::Dependent].widget, true);
    set_sensitive(display_area[DisplayItms::Dependent].widget, true);

    // Rotate
    set_sensitive(node_popup[ValueItms::Rotate].widget,
		  rotate_ok);
    set_sensitive(graph_cmd_area[ValueItms::Rotate].widget,
		  rotate_ok);

    // Show/Hide Detail
    if (count_selected_expanded > 0 && count_selected_collapsed == 0)
    {
	// Only expanded displays selected
	set_label(node_popup[ValueItms::Detail].widget, "Hide Detail");
	set_label(graph_cmd_area[ValueItms::Detail].widget, "Hide ()");
	set_sensitive(node_popup[ValueItms::Detail].widget, true);
	set_sensitive(graph_cmd_area[ValueItms::Detail].widget, true);
    }
    else if (count_selected_collapsed > 0)
    {
	// Some collapsed displays selected
	set_label(node_popup[ValueItms::Detail].widget, "Show Detail");
	set_label(graph_cmd_area[ValueItms::Detail].widget, "Show ()");
	set_sensitive(node_popup[ValueItms::Detail].widget, true);
	set_sensitive(graph_cmd_area[ValueItms::Detail].widget, true);
    }
    else
    {
	// Expanded as well as collapsed displays selected
	set_sensitive(node_popup[ValueItms::Detail].widget, false);
	set_sensitive(graph_cmd_area[ValueItms::Detail].widget, false);
    }

    set_sensitive(display_area[DisplayItms::ShowDetail].widget, 
		  count_selected_collapsed > 0);
    set_sensitive(display_area[DisplayItms::HideDetail].widget, 
		  count_selected_expanded > 0);

    // Delete
    set_sensitive(graph_cmd_area[ValueItms::Delete].widget, count_selected);
    set_sensitive(display_area[DisplayItms::Delete].widget, count_selected);

    // Set
    set_sensitive(graph_cmd_area[ValueItms::Set].widget, 
		  count_selected == 1 && count_selected_data == 1);
    set_sensitive(display_area[DisplayItms::Set].widget, 
		  count_selected == 1 && count_selected_data == 1);


    // Argument field
    if (count_selected > 0)
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
	    arg_os << "(" << count_selected << " displays)";
	    arg = arg_os;
	}
	graph_arg->set_string(arg);
    }
    else
    {
	// No argument
	graph_arg->set_string("");
    }

    // Set selection
    string cmd = selection_as_commands();
    XmTextSetString(graph_selection_w, cmd);
    Time tm = XtLastTimestampProcessed(XtDisplay(graph_selection_w));

    if (cmd == "")
    {
	// No selection
	XmTextClearSelection(graph_selection_w, tm);
    }
    else
    {
	// Give it to the selection
	XmTextSetSelection(graph_selection_w, 
			   0, XmTextGetLastPosition(graph_selection_w), tm);
    }
}

string DataDisp::selection_as_commands()
{
    ostrstream cmd;

    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->selected())
	{
	    cmd << "graph display " << dn->name();

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

		    cmd << " dependent on " << depnode->name();
		}
	    }
	    cmd << '\n';
	}
    }

    return string(cmd);
}


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
	string nr = dn->disp_nr();
	int display_nr = get_nr(nr);

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
// Anfragen an gdb stellen und Antworten bearbeiten
//-----------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Erzeugung von neuen (unabhaengigen) Displays
//-----------------------------------------------------------------------------

regex RXmore_than_one ("\\[-?[0-9]+\\.\\.-?[0-9]+\\]");

// ***************************************************************************
// sendet den Display-Befehl an den gdb
// wird ein Boxpoint uebergeben, so wird der neue Knoten dorthin gesetzt
// sonst an eine Default-Position.
//

class NewDisplayInfo {
public:
    StatusDelay *delay;
    string display_expression;
    BoxPoint point;
    BoxPoint *point_ptr;
    string depends_on;
    Widget origin;

    NewDisplayInfo()
	: delay(0), point_ptr(0), depends_on(""), origin(0)
    {}

    ~NewDisplayInfo()
    {
	delete delay;
    }
};

void DataDisp::again_new_displaySQ (XtPointer client_data, XtIntervalId *)
{
    NewDisplayInfo *info = (NewDisplayInfo *)client_data;
    new_displaySQ(info->display_expression, info->point_ptr, 
		  info->depends_on, info->origin);
    delete info;
}

void DataDisp::new_displaySQ (string display_expression, BoxPoint *p,
			      string depends_on, Widget origin)
{
    // Check arguments
    if (depends_on != "")
    {
	int depend_nr = disp_graph->get_by_name(depends_on);
	if (depend_nr == 0)
	{
	    post_gdb_message("No display named " + quote(depends_on) + ".\n");
	    return;
	}
	DispNode *dn = disp_graph->get(depend_nr);
	if (dn == 0)
	{
	    post_gdb_message("No display number " + 
			     itostring(depend_nr) + ".\n");
	    return;
	}
    }

    NewDisplayInfo *info = new NewDisplayInfo;
    info->display_expression = display_expression;
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

    if (!DispBox::vsllib_initialized)
    {
	// If we don't have the VSL library yet, try again later.
	info->delay = new StatusDelay("Reading VSL library");

	// Disable background processing and try again - as soon
	// as the VSL library will be completely read, we shall enter
	// the main DDD event loop and get called again.
	VSLLib::background = 0;
	XtAppAddTimeOut(XtWidgetToApplicationContext(graph_edit),
			100, again_new_displaySQ, info);
	return;
    }

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
	if (display_expression.contains (RXmore_than_one))
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
	    nr = itostring(next_display_number++);
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

DispNode *DataDisp::new_data_node(string& answer)
{
    string nr;
    string name;

    read_number_and_name(answer, nr, name);
    if (nr == "" || name == "")
    {
	post_gdb_message(answer, last_origin);
	return 0;
    }

    DispNode* dn = 0;

    if (is_disabling (answer, gdb))
    {
	post_gdb_message(answer, last_origin);
	dn = new DispNode(nr, name);
    }
    else
    {
	dn = new DispNode(nr, name, answer);
    }

    return dn;
}

DispNode *DataDisp::new_user_node(const string& name, string& answer)
{
    // Assign a default number
    string disp_nr_str = itostring(-(next_display_number++));
    string title = name;
    return new DispNode(disp_nr_str, title, answer);
}


// ***************************************************************************
// erzeugt neuen Display-Knoten und setzt ihn an den im data-Argument
// angegebenen Punkt (falls (BoxPoint *)data != BoxPoint())
//
void DataDisp::new_data_displayOQC (const string& answer, void* data)
{
    NewDisplayInfo *info = (NewDisplayInfo *)data;

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
	post_gdb_message(answer, last_origin);
	delete info;
	return;
    }

    // Unselect all nodes
    for (GraphNode *gn = disp_graph->firstNode();
	 gn != 0; gn = disp_graph->nextNode(gn))
    {
	BoxGraphNode *bgn = ptr_cast(BoxGraphNode, gn);
	if (bgn)
	    bgn->selected() = false;
    }

    // DispNode erzeugen und ggf. disabling-Meldung ausgeben
    string ans = answer;
    DispNode *dn = new_data_node(ans);
    if (dn == 0)
    {
	delete info;
	return;
    }

    // BoxPoint berechnen
    int depend_nr = disp_graph->get_by_name(info->depends_on);
    BoxPoint box_point = info->point;
    if (box_point == BoxPoint())
	box_point = disp_graph->default_pos(dn, graph_edit, depend_nr);
    dn->moveTo(box_point);
    dn->selected() = true;

    // in den Graphen einfuegen
    string nr = dn->disp_nr();
    disp_graph->insert(get_nr(nr), dn, depend_nr);

    refresh_addr(dn);
    refresh_graph_edit();

    delete info;
    prompt();
}

void DataDisp::new_user_displayOQC (const string& answer, void* data)
{
    NewDisplayInfo *info = (NewDisplayInfo *)data;

    // Unselect all nodes
    for (GraphNode *gn = disp_graph->firstNode();
	 gn != 0; gn = disp_graph->nextNode(gn))
    {
	BoxGraphNode *bgn = ptr_cast(BoxGraphNode, gn);
	if (bgn)
	    bgn->selected() = false;
    }

    // DispNode erzeugen und ggf. disabling-Meldung ausgeben
    string ans = answer;
    DispNode *dn = new_user_node(info->display_expression, ans);
    if (dn == 0)
    {
	delete info;
	prompt();
	return;
    }

    // BoxPoint berechnen
    int depend_nr = disp_graph->get_by_name(info->depends_on);
    BoxPoint box_point = info->point;
    if (box_point == BoxPoint())
	box_point = disp_graph->default_pos(dn, graph_edit, depend_nr);
    dn->moveTo(box_point);
    dn->selected() = true;

    // in den Graphen einfuegen
    string nr = dn->disp_nr();
    disp_graph->insert(get_nr(nr), dn, depend_nr);

    refresh_addr(dn);
    refresh_graph_edit();
    update_infos();

    delete info;
    prompt();
}

// ***************************************************************************
// Aus den Display-Ausdruecken den ersten (neuen) herausfischen, und dann
// der normalen Verarbeitung zufuehren.
//
void DataDisp::new_data_display_extraOQC (const string& answer, void* data)
{
    string ans = answer;
    string display = read_next_display (ans, gdb);

    if (display != "")
	new_data_displayOQC(display, data);
}

// ***************************************************************************
// wird von new_displaySQ aufgerufen, wenn die display_expression die
// array-notation RXmore_than_one enthaelt.
//
void DataDisp::new_data_displaysSQA (string display_expression,
				     void *data)
{
    NewDisplayInfo *info = (NewDisplayInfo *)data;
    assert (display_expression.contains (RXmore_than_one));

    // einzelne display-Ausdruecke erzeugen und display-Befehle im array 
    // abschicken...
    string prefix  = display_expression.before(RXmore_than_one);
    string postfix = display_expression.after(RXmore_than_one);
    string range   = display_expression.from(RXmore_than_one);
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
	display_cmds += 
	    gdb->display_command(prefix + "[" + itostring (i) + "]" + postfix);
	print_cmds += 
	    gdb->print_command(prefix + "[" + itostring (i) + "]" + postfix);
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

// ***************************************************************************
void DataDisp::new_data_displaysOQAC (string answers[],
				      void*  qu_datas[],
				      int    count,
				      void*  data)
{
    // Unselect all nodes
    for (GraphNode *gn = disp_graph->firstNode();
	 gn != 0; gn = disp_graph->nextNode(gn))
    {
	BoxGraphNode *bgn = ptr_cast(BoxGraphNode, gn);
	if (bgn)
	    bgn->selected() = false;
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
	    DispNode *dn = new_data_node(answers[i]);
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
	    string nr = dn->disp_nr();
	    disp_graph->insert(get_nr(nr), dn, depend_nr);
	}
    }
    delete[] answers;
    delete[] qu_datas;

    refresh_addr();
    refresh_graph_edit();

    delete info;
    prompt();
}


//-----------------------------------------------------------------------------
// Den disp-graph auf aktuellen Stand bringen.
//-----------------------------------------------------------------------------

int DataDisp::add_refresh_data_commands(StringArray& cmds)
{
    int initial_size = cmds.size();

    switch (gdb->type())
    {
    case DBX:
    case XDB:
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


#define PROCESS_INFO_DISPLAY 0
#define PROCESS_DATA         1
#define PROCESS_USER         2
#define PROCESS_ADDR         3

// ***************************************************************************
// sendet die Befehle "info display" und "display" an den gdb,
// um Displays zu aktualisieren.
//
void DataDisp::refresh_display(Widget origin)
{
    gdb_command("graph refresh", origin);
}

void DataDisp::refresh_displaySQ(Widget origin)
{
    if (origin)
	set_last_origin(origin);

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
	    
    bool ok = gdb->send_qu_array(cmds, dummy, cmds.size(), 
				 refresh_displayOQAC, (void *)1);

    if (!ok || cmds.size() == 0)
    {
	// Simply redraw display
	refresh_graph_edit();
	prompt();
    }
}

// ***************************************************************************
// Aktualisiert die Displays entsprechend.
//
void DataDisp::refresh_displayOQAC (string answers[],
				    void*  qu_datas[],
				    int    count,
				    void*  data)
{
    string data_answers;
    int data_answers_seen = 0;
    StringArray user_answers;
    StringArray addr_answers;

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
	    refresh_displaySQ();
	    data = 0;
	}
    }

    if (user_answers.size() > 0)
	process_user(user_answers);

    if (addr_answers.size() > 0)
	process_addr(addr_answers);

    delete[] answers;
    delete[] qu_datas;

    if (data != 0)
	prompt();
}



//-----------------------------------------------------------------------------
// Displays ausschalten (disable)
//-----------------------------------------------------------------------------

// ***************************************************************************
// sendet den 'disable display'-Befehl mit den Nummern an den gdb
// und aktualisiert den disp_graph.
//

static string numbers(IntArray& a)
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

void DataDisp::sort_and_check(IntArray& display_nrs)
{
    sort(display_nrs);

    for (int i = 0; i < display_nrs.size(); i++)
    {
	DispNode *dn = disp_graph->get(display_nrs[i]);
	if (dn == 0)
	    post_gdb_message("No display number " 
			     + itostring(display_nrs[i]) + ".\n");
    }
}

void DataDisp::disable_display(IntArray& display_nrs)
{
    if (display_nrs.size() > 0)
	gdb_command("graph disable display " + numbers(display_nrs));
}

void DataDisp::disable_displaySQ(IntArray& display_nrs)
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
	gdb_command(cmd, last_origin, disable_displayOQC);

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
	prompt();
    }
}


// ***************************************************************************
// Bei nicht-leerer Antwort Ausgabe als Fehlermeldung.
//
void DataDisp::disable_displayOQC (const string& answer, void *)
{
    gdb_out(answer);
    refresh_graph_edit();
    prompt();
}


//-----------------------------------------------------------------------------
// Displays einschalten (enable)
//-----------------------------------------------------------------------------

// ***************************************************************************
// sendet den 'enable display'-Befehl mit den Nummern an den gdb
// und aktualisiert den disp_graph.
//
void DataDisp::enable_display(IntArray& display_nrs)
{
    if (display_nrs.size() > 0)
	gdb_command("graph enable display " + numbers(display_nrs));
}

void DataDisp::enable_displaySQ(IntArray& display_nrs)
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
	gdb_command(cmd, last_origin, enable_displayOQC);

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
	prompt();
    }
}


// ***************************************************************************
// Bei nicht-leerer Antwort Ausgabe als Fehlermeldung.
//
void DataDisp::enable_displayOQC (const string& answer, void *)
{
    gdb_out(answer);
    refresh_displaySQ();
}


//-----------------------------------------------------------------------------
// Displays loeschen (delete)
//-----------------------------------------------------------------------------

// ***************************************************************************
// sendet den 'delete display'-Befehl mit den Nummern an den gdb
// und aktualisiert den disp_graph.
//
void DataDisp::delete_display(IntArray& display_nrs)
{
    if (display_nrs.size() > 0)
	gdb_command("graph undisplay " + numbers(display_nrs));
}

void DataDisp::delete_displaySQ(IntArray& display_nrs)
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
	gdb_command(cmd, last_origin, delete_displayOQC);

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
	prompt();
    }

    update_infos();
}


// ***************************************************************************
// Bei nicht-leerer Antwort Ausgabe als Fehlermeldung.
//
void DataDisp::delete_displayOQC (const string& answer, void *)
{
    string ans = answer;

    switch (gdb->type())
    {
    case GDB:
    case XDB:
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
    gdb_out(ans);

    // Refresh editor
    refresh_graph_edit();

    // Refresh remaining addresses
    force_check_aliases = true;
    refresh_addr();
    prompt();
}



//-----------------------------------------------------------------------------
// 'info display'-Ausgabe verarbeiten 
//-----------------------------------------------------------------------------

// ***************************************************************************
void DataDisp::process_info_display (string& info_display_answer)
{
    // Antwort auf 'info display' auswerten (evtl displays loeschen)
    // Fremde Display-Infos rausschneiden, eigene speichern

    int disp_nr;
    StringMap info_disp_string_map;
    string *strptr;

    string next_disp_info = 
	read_first_disp_info (info_display_answer, gdb);
    while (next_disp_info != "")
    {
	disp_nr = get_positive_nr (next_disp_info);
	if (disp_nr >= 0)
	{
	    if (disp_graph->contains(disp_nr)) 
	    {
		// Eigenes display-info
		strptr = 
		    new string(get_info_disp_str(next_disp_info, gdb));
		info_disp_string_map.insert (disp_nr, strptr);
	    }
	}
	next_disp_info = 
	    read_next_disp_info(info_display_answer, gdb);
    }


    // Eigene Display-Infos verarbeiten
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

    assert (info_disp_string_map.length() == 0); // alles verarbeitet ?

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
// Display-Ausgabe verarbeiten 
//-----------------------------------------------------------------------------

// ***************************************************************************
string DataDisp::process_displays (string& displays,
				   bool& disabling_occurred)
{
    Delay d;

    string not_my_displays;
    static string disabling_error_msgs;
    disabling_occurred = false;

    // Fremde Displays rausschneiden, eigene speichern
    int    disp_nr = 0;
    StringMap disp_string_map;
    string *strptr;

#if LOG_DISPLAYS
    clog << "Processing displays " << quote(displays) << "...\n";
#endif

    string next_display = read_next_display (displays, gdb);
    while (next_display != "") 
    {
#if LOG_DISPLAYS
        clog << "Processing display " << quote(next_display);
#endif
	switch (gdb->type())
	{
	case GDB:
	    disp_nr = get_positive_nr (next_display);
	    break;

	case DBX:
	case XDB:
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
			    string nr = dn->disp_nr();
			    disp_nr = get_positive_nr(nr);
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

	// Falls Fehlermeldung: merken und alles nochmal von vorne.
	if (is_disabling (next_display, gdb))
	{
	    disabling_occurred = true;
	    if (disp_nr >= 0 && disp_graph->contains(disp_nr))
	    {
		disabling_error_msgs += 
		    get_disp_value_str(next_display, gdb);
	    }
	    else
	    {
		not_my_displays = next_display; //nur diesen merken
	    }

	    // disp_string_map leeren
	    disp_string_map.delete_all_contents();

	    return not_my_displays;
	}

	if (is_invalid(next_display))
	{
	    // Display is not active: don't insert it into map.
	    // This way, it will be shown as disabled.
	}
	else if (disp_nr >= 0 && disp_graph->contains(disp_nr))
	{
	    strptr = new string(get_disp_value_str(next_display, gdb));
	    disp_string_map.insert (disp_nr, strptr);
	}
	else 
	{
	    not_my_displays += next_display + '\n';
	}

	next_display = read_next_display (displays, gdb);
    }

    // gesammelte Fehlermeldungen ausgeben.
    if ( !disabling_occurred && disabling_error_msgs != "")
    {
	post_gdb_message(disabling_error_msgs, last_origin);
	disabling_error_msgs = "";
    }

    // eigene Displays verarbeiten
    bool changed = false;
    MapRef ref;
    for (int k = disp_graph->first_nr(ref); 
	     k != 0 ; 
	     k = disp_graph->next_nr(ref))
    {
	DispNode* dn = disp_graph->get(k);
	if (!dn->is_user_command())
	{
	    if (!disp_string_map.contains (k))
	    {
		// Knoten auf disabled setzen, falls nicht schon geschehen
		if (dn->enabled())
		{
		    dn->disable();
		    changed = true;
		}
	    }
	    else
	    {
		// Knoten aktualisieren
		strptr = disp_string_map.get(k);
		if ( dn->update (*strptr ))
		    changed = true;
		if ((*strptr != "") && !(strptr->matches (rxwhite)))
		{
		    // hinter dem Display stand noch etwas, was nicht
		    // dazu gehoert z.B. der return Wert bei finish
		    not_my_displays += strptr->after (rxwhite);
		}
		delete disp_string_map.get(k);
		disp_string_map.del (k);
	    }
	}
    }

    assert (disp_string_map.length() == 0);
    if (changed) 
	refresh_graph_edit();

    return not_my_displays;
}

// Handle output of user commands
void DataDisp::process_user (StringArray& answers)
{
    int i = 0;

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
	    if (dn->update(answer))
		changed = true;
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
    StringArray addrs;

    if (number_of_displays > 0)
    {
	// Add titles
	nums   += "Num";
	states += "State";
	exprs  += "Expression";
	addrs  += "Address";
    }
    else
    {
	nums   += "";
	states += "";
	exprs  += "";
	addrs  += "";
    }

    MapRef ref;
    int k;
    for (k = disp_graph->first_nr(ref); k != 0; k = disp_graph->next_nr(ref))
    {
	DispNode* dn = disp_graph->get(k);

	nums += dn->disp_nr() + ":";

	if (dn->nodeptr()->hidden())
	    states += "alias of " + itostring(dn->alias_of);
	else if (dn->enabled())
	    states += "enabled";
	else
	    states += "disabled";
	
	exprs += dn->name();
	addrs += dn->addr();
    }

    int nums_width   = max_width(nums);
    int exprs_width  = max_width(exprs)  + 1;
    int states_width = max_width(states) + 1;
    int addrs_width  = max_width(addrs);

    string *label_list = new string[number_of_displays + 1];
    bool *selected     = new bool[number_of_displays + 1];

    // Set titles
    int count = 0;
    string line;
    if (number_of_displays > 0)
    {
	line = fmt(nums[count], nums_width) 
	    + " " + fmt(exprs[count], exprs_width)
	    + " " + fmt(states[count], states_width);
	if (detect_aliases)
	    line += " " + fmt(addrs[count], addrs_width);
    }
    else
    {
	line = "No displays.";
    }
    label_list[count] = line;
    selected[count] = false;
    count++;

    int selected_displays = 0;	// Number of selected displays
    int index_selected    = -1;	// Index of single selected display

    // Set contents
    for (k = disp_graph->first_nr(ref); k != 0; k = disp_graph->next_nr(ref))
    {
	DispNode* dn = disp_graph->get(k);
	line = fmt(nums[count], nums_width) 
	    + " " + fmt(exprs[count], exprs_width)
	    + " " + fmt(states[count], states_width);
	if (detect_aliases)
	    line += " " + fmt(addrs[count], addrs_width);
	label_list[count] = line;
	selected[count]   = dn->selected();

	if (dn->selected())
	{ 
	    selected_displays++;
	    index_selected = count;
	}

	count++;
    }

    sort(label_list + 1, selected + 1, count - 1);

    setLabelList(display_list_w, label_list, selected, count, 
		 number_of_displays > 0, false);

    if (!silent)
    {
	// Setup status line
	MString msg;

	if (selected_displays == 1)
	{
	    // Show info about single selected display
	    msg = rm("Display " + nums[index_selected] + " ");
	    msg += tt(exprs[index_selected]);
	    msg += rm(" (" + states[index_selected]);
	    if (detect_aliases && addrs[index_selected] != "")
	    {
		msg += rm(", address ");
		msg += tt(addrs[index_selected]);
	    }
	    msg += rm(")");
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
		    displays += atoi(dn->disp_nr());
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
    static regex RXnl(" *\n *");
    value.gsub(RXnl, " ");
    strip_final_blanks(value);

    MString prompt = rm("Enter new value for ");
    prompt += tt(name);
    prompt += rm(":");

    Arg args[10];
    int arg = 0;

    XtSetArg(args[arg], XmNdeleteResponse, XmDESTROY); arg++;
    XtSetArg(args[arg], XmNselectionLabelString, prompt.xmstring()); arg++;
    Widget set_dialog = 
	verify(XmCreatePromptDialog(find_shell(w), "set_dialog", args, arg));

    Delay::register_shell(set_dialog);

    if (lesstif_hacks_enabled)
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
	    string nr = dn->disp_nr();
	    killme += get_nr(nr);
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

    MString dereference_label("Display " + gdb->dereferenced_expr(""));
    XtVaSetValues(node_popup[ValueItms::Dereference].widget,
		  XmNlabelString, dereference_label.xmstring(),
		  NULL);

    XtVaSetValues(display_area[DisplayItms::Dereference].widget,
		  XmNlabelString, dereference_label.xmstring(),
		  NULL);

    MString dereference_arg_label("Display " + gdb->dereferenced_expr("()"));
    XtVaSetValues(graph_cmd_area[ValueItms::Dereference].widget,
		  XmNlabelString, dereference_arg_label.xmstring(),
		  NULL);
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
 	if (!dn->is_user_command())
	    cmds += gdb->print_command(gdb->address_expr(dn->name()));
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

	ok = gdb->send_qu_array(cmds, dummy, cmds.size(), 
				refresh_displayOQAC, (void *)0);

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
	if (!dn->is_user_command())
	{
	    string addr = answers[i++];
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
	if (dn != 0 && !dn->is_user_command())
	{
	    equivalences[dn->addr()] += k;
	}
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
    if (d0->nodeptr()->hidden())
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
	    msg += rm("display " + node->disp_nr() + ": ");
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
	msg += rm(" of display " + of->disp_nr() + ": ");
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
    graph_popup_w = MMcreatePopupMenu(graph_edit, "graph_popup", graph_popup);
    InstallButtonTips(graph_popup_w);

    node_popup_w = MMcreatePopupMenu(graph_edit, "node_popup", node_popup);
    InstallButtonTips(node_popup_w);

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

    // Create display editor
    edit_displays_dialog_w =
	verify(XmCreatePromptDialog(find_shell(graph_edit), 
				    "edit_displays_dialog", 
				    NULL, 0));

    if (lesstif_hacks_enabled)
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

    XtManageChild (buttons);
    XtManageChild (display_list_w);
    XtManageChild (label);
    XtManageChild (form2);
    XtManageChild (form1);

    // Add callbacks
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

    // Reset argument field and display editor buttons
    set_args();
}
