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
#include "post.h"
#include "question.h"
#include "commandQ.h"
#include "StringMap.h"
#include "VoidArray.h"
#include "settings.h"
#include "status.h"

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
			      Dummy1, Set, Dummy2, Disable, Delete }; };

MMDesc DataDisp::node_popup[] =
{
    {"dereference",   MMPush,   {DataDisp::dereferenceCB}},
    {"detail",        MMPush,   {DataDisp::toggleDetailCB}},
    {"rotate",        MMPush,   {DataDisp::toggleRotateCB}},
    {"dependent",     MMPush,   {DataDisp::dependentCB}},
    MMSep,
    {"set",           MMPush,   {DataDisp::setCB}},
    MMSep,
    {"disable",       MMPush,   {DataDisp::toggleDisableCB}},
    {"delete",        MMPush,   {DataDisp::deleteCB}},
    MMEnd
};

MMDesc DataDisp::graph_cmd_area[] =
{
    {"dereference",   MMPush, {DataDisp::dereferenceCB}},
    {"detail",        MMPush, {DataDisp::toggleDetailCB}},
    {"rotate",        MMPush, {DataDisp::toggleRotateCB}},
    {"dependent",     MMPush, {DataDisp::dependentCB}},
    MMSep,
    {"set",           MMPush, {DataDisp::setCB}},
    MMSep,
    {"disable",       MMPush, {DataDisp::toggleDisableCB}},
    {"delete",        MMPush, {DataDisp::deleteCB}},
    MMEnd
};

struct DisplayItms { enum Itms {Dependent, Dereference, 
				ShowDetail, HideDetail, Set,
				Enable, Disable, Delete}; };

MMDesc DataDisp::display_area[] =
{
    {"dependent",    MMPush,   {DataDisp::dependentCB }},
    {"dereference",  MMPush,   {DataDisp::dereferenceCB }},
    {"show_detail",  MMPush,   {DataDisp::showDetailCB}},
    {"hide_detail",  MMPush,   {DataDisp::hideDetailCB}},
    {"set",          MMPush,   {DataDisp::setCB}},
    {"enable",       MMPush,   {DataDisp::enableCB }},
    {"disable",      MMPush,   {DataDisp::disableCB }},
    {"delete",       MMPush,   {DataDisp::deleteCB }},
    MMEnd
};

DispGraph *DataDisp::disp_graph             = 0;
Delay     *DataDisp::delay                  = 0;
Widget     DataDisp::graph_edit             = 0;
Widget     DataDisp::last_origin            = 0;
ArgField  *DataDisp::graph_arg              = 0;
Widget     DataDisp::graph_cmd_w            = 0;
Widget     DataDisp::edit_displays_dialog_w = 0;
Widget     DataDisp::display_list_w         = 0;
Widget     DataDisp::graph_popup_w          = 0;
Widget     DataDisp::node_popup_w           = 0;

bool DataDisp::ignore_update_graph_editor_selection   = false;
bool DataDisp::ignore_update_display_editor_selection = false;

int DataDisp::next_display_number = 1;


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
    dependent_displaySQ (display_expression, get_nr(nr));
}

void DataDisp::toggleDetailCB(Widget dialog, XtPointer, XtPointer)
{
    set_last_origin(dialog);

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
		dv->expandAll();
	    else
		dv->collapse();
	    dn->refresh();
	    changed = true;
	}
    }

    if (changed)
	refresh_graph_edit();
}

void DataDisp::showDetailCB (Widget dialog, XtPointer, XtPointer)
{
    set_last_origin(dialog);

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
		dn->refresh();
		changed = true;
	    }
	}
    }

    if (changed)
	refresh_graph_edit();
}

void DataDisp::hideDetailCB (Widget dialog, XtPointer, XtPointer)
{
    set_last_origin(dialog);

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

	    if (dv->expanded())
	    {
		dv->collapse();
		dn->refresh();
		changed = true;
	    }
	}
    }

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
	enable_displaySQ(disp_nrs);
    else if (do_disable)
	disable_displaySQ(disp_nrs);
}

static void select_with_all_descendants(GraphNode *node)
{
    if (!node->selected())
    {
	node->selected() = true;
	
	for (GraphEdge *edge = node->firstFrom();
	     edge != 0; edge = node->nextFrom(edge))
	    select_with_all_descendants(edge->to());
    }
}

static void select_with_all_ancestors(GraphNode *node)
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

    int count = disp_graph->count_all();
    IntArray disp_nrs(count);
    GraphNode **ancestors   = new GraphNode *[count];
    GraphNode **descendants = new GraphNode *[count];

    int a = 0;
    int d = 0;

    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->selected())
	{
	    string nr = dn->disp_nr();
	    disp_nrs += get_positive_nr(nr);

	    // Select all ancestors
	    GraphEdge *edge;
	    for (edge = dn->nodeptr()->firstTo();
		 edge != 0; edge = dn->nodeptr()->nextTo(edge))
	    {
		GraphNode *ancestor = edge->from();
		while (ancestor->isHint())
		    ancestor = ancestor->firstTo()->from();

		ancestors[a++] = ancestor;
	    }

	    // Select all descendants
	    for (edge = dn->nodeptr()->firstFrom();
		 edge != 0; edge = dn->nodeptr()->nextFrom(edge))
	    {
		GraphNode *descendant = edge->to();
		while (descendant->isHint())
		    descendant = descendant->firstFrom()->to();

		descendants[d++] = descendant;
	    }
	}
    }

    delete_displaySQ(disp_nrs);

    while (a > 0)
	select_with_all_descendants(ancestors[--a]);
    while (d > 0)
	select_with_all_ancestors(descendants[--d]);

    refresh_graph_edit();

    delete[] descendants;
    delete[] ancestors;
}


void DataDisp::dependentCB(Widget w, XtPointer client_data, 
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
    XtManageChild (dependent_display_dialog);
}


void DataDisp::refreshCB(Widget w, XtPointer, XtPointer)
{
    set_last_origin(w);
    refresh_displaySQ();
}

void DataDisp::selectAllCB(Widget w, XtPointer, XtPointer)
{
    set_last_origin(w);
    XtCallActionProc(graph_edit, 
		     "select-all", (XEvent *)0, (String *)0, 0);
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

    enable_displaySQ (disp_nrs);
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

    disable_displaySQ (disp_nrs);
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

    BoxPoint* p = (BoxPoint *) client_data;
    new_displaySQ (source_arg->get_string(), p);
}


// ***************************************************************************
//
void DataDisp::popup_newCB (Widget    display_dialog,
			    XtPointer client_data,
			    XtPointer)
{
    set_last_origin(display_dialog);

    BoxPoint* p = (BoxPoint *) client_data;
    new_displayCD (*p);
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
    switch (cbs->reason) {
    case XmCR_OK :
	XmStringGetLtoR(cbs->value, MSTRING_DEFAULT_CHARSET, &input);
	if (input != "") {
	    dependent_displaySQ (input, *disp_nr_ptr);
	}
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
	    new_displaySQ (input, p);
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

    XtManageChild (new_display_dialog);
}



//-----------------------------------------------------------------------------
// sonstige Procs
//-----------------------------------------------------------------------------

// ***************************************************************************
//
void DataDisp::set_delay () 
{
    if (!delay)
	delay = new Delay;
}
void DataDisp::unset_delay () 
{
    if (delay)
	delete delay;
    delay = 0;
}

void DataDisp::set_handlers()
{
    source_arg->addHandler(Empty, source_argHP);
    graph_arg->addHandler(Empty, graph_argHP);
    graph_arg->addHandler(LosePrimary, graph_unselectHP);
    gdb->addBusyHandler(ReadyForQuestion, gdb_ready_for_questionHP);
}




// ***************************************************************************
// Graph neu zeichnen, und Sensitivitaeten neu setzen
//
void DataDisp::refresh_graph_edit ()
{
    static Graph* dummy = 0;
    if (!dummy)
	dummy = new Graph();
    XtVaSetValues(graph_edit,
		  XtNgraph, dummy,
		  NULL);
    XtVaSetValues(graph_edit,
		  XtNgraph, (Graph *)disp_graph,
		  NULL);
    refresh_args();
    fill_labels();
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
// Sensitivitaeten setzen
//-----------------------------------------------------------------------------

void DataDisp::no_displaysHP (void*, void* , void* call_data)
{
    bool empty = bool(call_data);
    if (!empty)
    {
	XtManageChild(graph_edit);
	if (data_disp_shell)
	{
	    static bool initial_popup = true;

	    if (initial_popup)
	    {
		initial_popup_shell(data_disp_shell);
		initial_popup = false;
	    }
	}
    }

    set_sensitive (graph_popup[GraphItms::Refresh].widget,
		   (!empty && gdb->isReadyWithPrompt()));
}

void DataDisp::source_argHP (void*, void* , void*)
{
    refresh_args();
}

void DataDisp::graph_argHP (void*, void* , void*)
{
    refresh_args();
}

void DataDisp::graph_unselectHP (void*, void*, void*)
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
    fill_labels();
}

void DataDisp::gdb_ready_for_questionHP (void*, void* , void*)
{
    refresh_args();
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
    fill_labels();
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
    int count_selected           = 0;
    int count_all                = 0;
    int count_selected_disabled  = 0;
    int count_selected_enabled   = 0;
    int count_selected_expanded  = 0;
    int count_selected_collapsed = 0;

    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	count_all++;
	if (dn->selected())
	{
	    count_selected++;
	    if (dn->disabled())
		count_selected_disabled++;
	    if (dn->enabled())
		count_selected_enabled++;

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

    if (count_selected > 0)
    {
	switch (gdb->type())
	{
	case DBX:
	case XDB:
	    // DBX and XDB cannot disable or enable displays
	    count_selected_enabled = count_selected_disabled = 0;
	    break;

	case GDB:
	    break;
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

    Boolean gdb_ok = (gdb != 0 && gdb->isReadyWithPrompt());

    // New ()
    Boolean arg_ok = (gdb_ok && !source_arg->empty());
    set_sensitive(graph_popup[GraphItms::NewArg].widget, arg_ok);

    // Refresh (), Select All ()
    set_sensitive(graph_popup[GraphItms::Refresh].widget,   count_all);
    set_sensitive(graph_popup[GraphItms::SelectAll].widget, count_all);

    // Klick auf einen Knoten - - - - - - - - - - - - - - - - - - - - - - - -
    Boolean dereference_ok  = False;
    Boolean rotate_ok       = False;
    Boolean rotate_state    = False;

    if (disp_value_arg)
    {
	switch (disp_value_arg->type())
	{
	case Simple:
	case Text:
	case Reference:
	    break;

	case Pointer: 
	    dereference_ok = gdb_ok;
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

    // Enable/Disable
    if (count_selected_enabled > 0 && count_selected_disabled == 0)
    {
	// Only disabled displays selected
	set_label(node_popup[ValueItms::Disable].widget, "Disable Display");
	set_label(graph_cmd_area[ValueItms::Disable].widget, "Disable ()");
	set_sensitive(node_popup[ValueItms::Disable].widget, true);
	set_sensitive(graph_cmd_area[ValueItms::Disable].widget, true);
    }
    else if (count_selected_disabled > 0 && count_selected_enabled == 0)
    {
	// Only enabled displays selected
	set_label(node_popup[ValueItms::Disable].widget, "Enable Display");
	set_label(graph_cmd_area[ValueItms::Disable].widget, "Enable ()");
	set_sensitive(node_popup[ValueItms::Disable].widget, true);
	set_sensitive(graph_cmd_area[ValueItms::Disable].widget, true);
    }
    else 
    {
	// Disabled as well as enabled displays selected
	set_sensitive(node_popup[ValueItms::Disable].widget, false);
	set_sensitive(graph_cmd_area[ValueItms::Disable].widget, false);
    }

    // Enable
    set_sensitive(display_area[DisplayItms::Enable].widget, 
		  count_selected_disabled);

    // Disable
    set_sensitive(display_area[DisplayItms::Disable].widget, 
		  count_selected_enabled);

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
    set_sensitive(graph_cmd_area[ValueItms::Set].widget, count_selected == 1);
    set_sensitive(display_area[DisplayItms::Set].widget, count_selected == 1);

    // Argument field
    if (count_selected > 0)
    {
	string arg;
	if (disp_value_arg)
	{
	    arg = disp_value_arg->full_name();
	    source_arg->set_string(arg);
	    graph_arg->set_string(arg);
	}
	else if (disp_node_arg)
	{
	    arg = disp_node_arg->name();
	    source_arg->set_string(arg);
	    graph_arg->set_string(arg);
	}
	else
	{
	    ostrstream arg_os;
	    arg_os << "(" << count_selected << " displays)";
	    arg = arg_os;
	    graph_arg->set_string(arg);
	}

	// Cause argument field to obtain the selection
	Widget w = graph_arg->widget();
	XmTextFieldSetSelection(w,
				0, XmTextFieldGetLastPosition(w),
				XtLastTimestampProcessed(XtDisplay(w)));

	// The value is already highlighted in the node -- 
	// hence, no need to cause confusion
	XmTextFieldSetHighlight(w,
				0, XmTextFieldGetLastPosition(w),
				XmHIGHLIGHT_NORMAL);
    }
    else
    {
	// No argument
	graph_arg->set_string("");

	// Clear selection
	Widget w = graph_arg->widget();
	XmTextFieldClearSelection(w, XtLastTimestampProcessed(XtDisplay(w)));
    }
}


// Update graph editor selection after a change in the display editor
void DataDisp::UpdateGraphEditorSelectionCB(Widget, XtPointer, XtPointer)
{
    // Avoid recursive and mutually recursive calls
    if (ignore_update_graph_editor_selection)
	return;

    ignore_update_graph_editor_selection   = true;
    ignore_update_display_editor_selection = true;

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
    }

    refresh_args();

    ignore_update_graph_editor_selection   = false;
    ignore_update_display_editor_selection = false;
}

// Update display editor selection after a change in the graph editor
void DataDisp::UpdateDisplayEditorSelectionCB(Widget, XtPointer, XtPointer)
{
    // Avoid recursive and mutually recursive calls
    if (ignore_update_display_editor_selection)
	return;

    ignore_update_graph_editor_selection   = true;
    ignore_update_display_editor_selection = true;

    refresh_args();
    fill_labels();

    ignore_update_graph_editor_selection  =  false;
    ignore_update_display_editor_selection = false;
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

struct CallAgainInfo {
    StatusDelay *delay;
    string display_expression;
    BoxPoint point;
    BoxPoint *point_ptr;
    Widget origin;
};

void DataDisp::again_new_displaySQ (XtPointer client_data, XtIntervalId *)
{
    CallAgainInfo *info = (CallAgainInfo *)client_data;
    new_displaySQ(info->display_expression, info->point_ptr, info->origin);
    delete info->delay;
    delete info;
}

void DataDisp::new_displaySQ (string display_expression, BoxPoint *p,
			      Widget origin)
{
    if (!DispBox::vsllib_initialized)
    {
	// If we don't have the VSL library yet, try again later.
	CallAgainInfo *info = new CallAgainInfo;
	info->delay = 
	    new StatusDelay("Reading VSL library");
	info->display_expression = display_expression;
	if (p != 0)
	{
	    info->point = *p;
	    info->point_ptr = &info->point;
	}
	else
	{
	    info->point = BoxPoint(-1, -1);
	    info->point_ptr = 0;
	}
	info->origin   = origin;

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
	CallAgainInfo *info = new CallAgainInfo;
	if (p != 0)
	{
	    info->point = *p;
	    info->point_ptr = &info->point;
	}
	else
	{
	    info->point = BoxPoint(-1, -1);
	    info->point_ptr = 0;
	}
	info->display_expression = display_expression;

	// User-defined display
	string cmd = user_command(display_expression);
	bool ok = gdb->send_question(cmd, new_userOQC, info);
	if (!ok)
	    post_gdb_busy(origin);
    }
    else
    {
	// Data display
	if (display_expression.contains (RXmore_than_one)) {
	    new_displaysSQA (display_expression, p);
	    return;
	}

	switch (gdb->type())
	{
	case GDB:
	    {
		string cmd = gdb->display_command(display_expression);
		bool ok = gdb->send_question (cmd, new_displayOQC, p);
		if (!ok)
		    post_gdb_busy(last_origin);
	    }
	    break;

	case DBX:
	case XDB:
	    {
		gdb_question(gdb->display_command(display_expression));
		string cmd;
		cmd = gdb->print_command(display_expression);
		bool ok = gdb->send_question (cmd, new_displayOQC, p);
		if (!ok)
		    post_gdb_busy(last_origin);
	    }
	    break;
	}
    }
}


DispNode *DataDisp::new_data_node(string& answer)
{
    string disp_nr_str;
    switch(gdb->type())
    {
    case GDB:
	disp_nr_str = read_disp_nr_str (answer, gdb);
	if (disp_nr_str == "")
	{
	    post_gdb_message(answer, last_origin);
	    return 0;
	}
	break;

    case DBX:
    case XDB:
	// Assign a default number
	disp_nr_str = itostring(next_display_number++);
	break;
    }

    string name = read_disp_name (answer, gdb);
    if (name == "")
    {
	post_gdb_message (answer, last_origin);
	return 0;
    }

    DispNode* dn = 0;

    if (is_disabling (answer, gdb))
    {
	post_gdb_message (answer, last_origin);
	dn = new DispNode(disp_nr_str, name);
    }
    else
    {
	dn = new DispNode(disp_nr_str, name, answer);
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
// angegebenen Punkt (falls (BoxPoint *)data != (-1,-1))
//
void DataDisp::new_displayOQC (const string& answer, void* data)
{
    if (answer == "")
    {
	if (gdb->has_display_command())
	{
	    // No display output (GDB bug).  Refresh displays explicitly.
	    gdb->send_question (gdb->display_command(),
				new_display_extraOQC,
				data);
	}
	return;
    }

    if (!contains_display (answer, gdb))
    {
	post_gdb_message (answer, last_origin);
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
	return;

    // BoxPoint berechnen
    BoxPoint* p         = (BoxPoint *)data;
    BoxPoint  box_point =
	(p == 0 || *p == BoxPoint(-1,-1)) ?
	disp_graph->default_new_box_point(dn, graph_edit) :
	(*p);
    dn->moveTo(box_point);
    dn->selected() = true;

    // in den Graphen einfuegen
    string nr = dn->disp_nr();
    disp_graph->insert_new (get_nr(nr), dn);
    refresh_graph_edit();
}

void DataDisp::new_userOQC (const string& answer, void* data)
{
    CallAgainInfo *info = (CallAgainInfo *)data;

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
	return;
    }

    // BoxPoint berechnen
    BoxPoint* p         = info->point_ptr;
    BoxPoint  box_point =
	(p == 0 || *p == BoxPoint(-1,-1)) ?
	disp_graph->default_new_box_point(dn, graph_edit) :
	(*p);
    dn->moveTo(box_point);
    dn->selected() = true;

    // in den Graphen einfuegen
    string nr = dn->disp_nr();
    disp_graph->insert_new (get_nr(nr), dn);
    refresh_graph_edit();
    update_infos();

    delete info;
}

// ***************************************************************************
// Aus den Display-Ausdruecken den ersten (neuen) herausfischen, und dann
// der normalen Verarbeitung zufuehren.
//
void DataDisp::new_display_extraOQC (const string& answer, void* data)
{
    string ans = answer;
    string display = read_next_display (ans, gdb);
    new_displayOQC (display, data);
}

// ***************************************************************************
// wird von new_displaySQ aufgerufen, wenn die display_expression die
// array-notation RXmore_than_one enthaelt.
//
void DataDisp::new_displaysSQA (string display_expression, BoxPoint* p)
{
    assert (display_expression.contains (RXmore_than_one));

    // einzelne display-Ausdruecke erzeugen und display-Befehle im array 
    // abschicken...
    string prefix  = display_expression.before(RXmore_than_one);
    string postfix = display_expression.after(RXmore_than_one);
    string range   = display_expression.from (RXmore_than_one);
    range.del("[");
    int start = ::get_nr(range);
    range = range.after("..");
    int stop = ::get_nr(range);

    if (start > stop)
    {
	post_error("Invalid range in " + quote(display_expression), 
		   "invalid_range_error");
	return;
    }

    assert (stop >= start);
    int count = stop + 1 - start;

    string *display_cmds = new string[count];
    string *print_cmds   = new string[count];

    int j = 0;
    for (int i = start; i < stop + 1; i++) {
	display_cmds[j] = 
	    gdb->display_command(prefix + "[" + itostring (i) + "]" + postfix);
	print_cmds[j] = 
	    gdb->print_command(prefix + "[" + itostring (i) + "]" + postfix);
	j++;
    }

    switch (gdb->type())
    {
    case GDB:
	{
	    void** dummy = new void *[count];
	    int k = gdb->send_qu_array (display_cmds,
					dummy,
					count,
					new_displaysOQAC,
					p);
	    if (k == 0)
		post_gdb_busy(last_origin);

	    delete[] dummy;
	}
	break;

    case DBX:
    case XDB:
	{
	    for (int i = 0; i < count; i++)
		gdb_question(display_cmds[i]);

	    void** dummy = new void *[count];
	    int k = gdb->send_qu_array (print_cmds,
					dummy,
					count,
					new_displaysOQAC,
					p);
	    if (k == 0)
		post_gdb_busy(last_origin);
	
	    delete[] dummy;
	}
	break;
    }

    delete[] display_cmds;
    delete[] print_cmds;
}

// ***************************************************************************
void DataDisp::new_displaysOQAC (string answers[],
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

    // Create and select new nodes
    for (int i = 0; i < count; i++) {
	if (!contains_display (answers[i], gdb))
	    post_gdb_message (answers[i], last_origin);
	else {

	    // DispNode erzeugen und ggf. disabling-Meldung merken
	    DispNode *dn = new_data_node(answers[i]);
	    if (dn == 0)
		return;

	    BoxPoint* p = (BoxPoint *) data;
	    BoxPoint  box_point =
		(p == 0 || *p == BoxPoint(-1,-1)) ?
		disp_graph->default_new_box_point(dn, graph_edit) :
		(*p);

	    dn->moveTo(box_point);
	    dn->selected() = true;

	    // Insert into graph
	    string nr = dn->disp_nr();
	    disp_graph->insert_new (get_nr(nr), dn);
	}
    }
    delete[] answers;
    delete[] qu_datas;

    refresh_graph_edit();
}


//-----------------------------------------------------------------------------
// Den disp-graph auf aktuellen Stand bringen.
//-----------------------------------------------------------------------------

int DataDisp::add_refresh_data_commands(StringArray& cmds)
{
    string command;
    int initial_size = cmds.size();

    switch (gdb->type())
    {
    case DBX:
    case XDB:
	{
	    command = "";
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
    string command;
    int initial_size = cmds.size();

    command = "";
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



// ***************************************************************************
// sendet die Befehle "info display" und "display" an den gdb,
// um Displays zu aktualisieren.
//
void DataDisp::refresh_displaySQ (Widget origin)
{
    if (origin)
	set_last_origin(origin);

    StringArray cmds;
    VoidArray dummy;
    bool ok = false;

    switch (gdb->type())
    {
    case GDB:
	cmds += "info display";
	break;
	
    case DBX:
    case XDB:
	break;
    }
    add_refresh_data_commands(cmds);
    add_refresh_user_commands(cmds);

    while (dummy.size() < cmds.size())
	dummy += (void *)0;
	    
    ok = gdb->send_qu_array(cmds, dummy, cmds.size(), refresh_displayOQAC, 0);

    if (!ok)
    {
	// Don't complain; simply redraw display.
	refresh_graph_edit();
    }
}

// ***************************************************************************
// Aktualisiert die Displays entsprechend.
//
void DataDisp::refresh_displayOQAC (string answers[],
				    void*  qu_datas[],
				    int    count,
				    void*  )
{
    int n = 0;
    if (gdb->type() == GDB)
    {
	// Antwort auf 'info display' auswerten (evtl displays loeschen)
	process_info_display (answers[n++]);
    }
    
    // Antwort auf 'display' auswerten
    string ans;
    for (int i = n; i < count; i++)
	ans += answers[i];

    bool disabling_occurred = false;
    if (n < count)
	process_displays (ans, disabling_occurred);

    delete[] answers;
    delete[] qu_datas;

    // Bei Fehlermeldung (Disabling...) nochmal refresh.
    if (disabling_occurred)
	refresh_displaySQ();
}



//-----------------------------------------------------------------------------
// Displays ausschalten (disable)
//-----------------------------------------------------------------------------

// ***************************************************************************
// sendet den 'disable display'-Befehl mit den Nummern an den gdb
// und aktualisiert den disp_graph.
//
void DataDisp::disable_displaySQ (const IntArray& display_nrs)
{
    int k = 0;
    string cmd = "disable display";
    for (int i = 0; i < display_nrs.size(); i++)
    {
	if (display_nrs[i] > 0)
	{
	    cmd += " " + itostring(display_nrs[i]);
	    k++;
	}
    }

    bool ok = true;

    if (k > 0)
    {
	ok = gdb->send_question (cmd, disable_displayOQC, 0);
	if (!ok)
	    post_gdb_busy(last_origin);
    }

    k = 0;
    for (int i = 0; i < display_nrs.size(); i++)
    {
	DispNode *dn = disp_graph->get(display_nrs[i]);
	if ((ok || dn->is_user_command()) && dn->enabled())
	{
	    dn->disable();
	    k++;
	}
    }

    if (k > 0)
	refresh_graph_edit();
}


// ***************************************************************************
// Bei nicht-leerer Antwort Ausgabe als Fehlermeldung.
//
void DataDisp::disable_displayOQC (const string& answer, void *)
{
    if (answer != "")
	post_gdb_message (answer, last_origin);
}


//-----------------------------------------------------------------------------
// Displays einschalten (enable)
//-----------------------------------------------------------------------------

// ***************************************************************************
// sendet den 'enable display'-Befehl mit den Nummern an den gdb
// und aktualisiert den disp_graph.
//
void DataDisp::enable_displaySQ (const IntArray& display_nrs)
{
    int k = 0;
    string cmd = "enable display";
    for (int i = 0; i < display_nrs.size(); i++)
    {
	if (display_nrs[i] > 0)
	{
	    cmd += " " + itostring(display_nrs[i]);
	    k++;
	}
    }

    bool ok = true;

    if (k > 0)
    {
	ok = gdb->send_question (cmd, enable_displayOQC, 0);
	if (!ok)
	    post_gdb_busy(last_origin);
    }

    for (int i = 0; i < display_nrs.size(); i++)
    {
	DispNode *dn = disp_graph->get(display_nrs[i]);
	if (dn->is_user_command() && dn->disabled())
	{
	    dn->enable();
	    k++;
	}
    }

    if (k > 0)
	refresh_graph_edit();
}


// ***************************************************************************
// Bei nicht-leerer Antwort Ausgabe als Fehlermeldung.
//
void DataDisp::enable_displayOQC (const string& answer, void *)
{
    if (answer != "")
	post_gdb_message (answer, last_origin);

    refresh_displaySQ ();
}


//-----------------------------------------------------------------------------
// Displays loeschen (delete)
//-----------------------------------------------------------------------------

// ***************************************************************************
// sendet den 'delete display'-Befehl mit den Nummern an den gdb
// und aktualisiert den disp_graph.
//
void DataDisp::delete_displaySQ (const IntArray& display_nrs)
{
    string cmd = "undisplay";

    int k = 0;
    switch (gdb->type())
    {
    case GDB:
	{
	    for (int i = 0; i < display_nrs.size(); i++)
	    {
		if (display_nrs[i] > 0)
		{
		    cmd += " " + itostring(display_nrs[i]);
		    k++;
		}
	    }
	}
	break;

    case DBX:
    case XDB:
	{
	    for (int i = 0; i < display_nrs.size(); i++)
	    {
		DispNode *dn = disp_graph->get(display_nrs[i]);
		if (dn && !dn->is_user_command())
		{
		    if (k++)
			cmd += ", ";
		    cmd += " " + dn->name();
		}
	    }
	}
	break;
    }
	
    bool ok = true;

    if (k > 0)
    {
	if (gdb->has_display_command())
	    ok = gdb->send_question (cmd, delete_displayOQC, 0);
    }

    if (!ok)
	post_gdb_busy(last_origin);

    for (int i = 0; i < display_nrs.size(); i++)
    {
	DispNode *dn = disp_graph->get(display_nrs[i]);
	if (dn != 0 && (ok || dn->is_user_command()))
	    disp_graph->del(display_nrs[i]);
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
	break;

    case DBX:
    case XDB:
	// Upon `undisplay', DBX redisplays remaining displays with values
	if (answer != "")
	{
	    bool disabling_occurred;
	    process_displays(ans, disabling_occurred);
	}
	break;
    }

    // Anything remaining is an error message
    if (answer != "")
	post_gdb_message (ans, last_origin);
}


//-----------------------------------------------------------------------------
// Neue abhaengige Displays erzeugen
//-----------------------------------------------------------------------------

// ***************************************************************************
// sendet den Display-Befehl an den gdb (mit disp_nr als data).
//
void DataDisp::dependent_displaySQ (string display_expression, int disp_nr)
{
    if (display_expression == "")
	return;
    if (display_expression.contains (RXmore_than_one)) {
	dependent_displaysSQA (display_expression, disp_nr);
	return;
    }

    switch (gdb->type())
    {
    case GDB:
	{
	    string cmd = gdb->display_command(display_expression);
	    bool ok = gdb->send_question(cmd, dependent_displayOQC, 
					 (void *) disp_nr);
	    if (!ok)
		post_gdb_busy(last_origin);
	
	}
	break;

    case DBX:
    case XDB:
	{
	    gdb_question(gdb->display_command(display_expression));

	    string cmd = gdb->print_command(display_expression);
	    bool ok = gdb->send_question(cmd, dependent_displayOQC, 
					 (void *) disp_nr);
	    if (!ok)
		post_gdb_busy(last_origin);
	
	}
	break;
    }
}

// ***************************************************************************
// ruft disp_graph->insert_dependent (answer, data).
//
void DataDisp::dependent_displayOQC (const string& answer, void* data)
{
    if (answer == "")
    {
	if (gdb->has_display_command())
	{
	    // No display output (GDB bug).  Refresh displays explicitly.
	    gdb->send_question(gdb->display_command(),
			       dependent_display_extraOQC, data);
	}
	return;
    }

    if (!contains_display (answer, gdb))
    {
	post_gdb_message (answer, last_origin);
	return;
    }

    string ans = answer;

    // Unselect all nodes
    for (GraphNode *gn = disp_graph->firstNode();
	 gn != 0; gn = disp_graph->nextNode(gn))
    {
	BoxGraphNode *bgn = ptr_cast(BoxGraphNode, gn);
	if (bgn)
	    bgn->selected() = false;
    }

    // DispNode erzeugen und ggf. disabling-Meldung merken
    DispNode *dn = new_data_node(ans);
    if (dn == 0)
	return;

    int old_disp_nr = int (data);
    BoxPoint box_point = 
	disp_graph->default_dependent_box_point(dn, graph_edit, old_disp_nr);
    dn->moveTo(box_point);
    dn->selected() = true;

    // in den Graphen einfuegen
    string nr = dn->disp_nr();
    disp_graph->insert_dependent (get_nr(nr), dn, old_disp_nr);
    refresh_graph_edit();
}

// ***************************************************************************
// Aus den Display-Ausdruecken den ersten (neuen) herausfischen, und dann
// der normalen Verarbeitung zufuehren.
//
void DataDisp::dependent_display_extraOQC (const string& answer, void* data)
{
    string ans = answer;
    string display = read_next_display (ans, gdb);
    dependent_displayOQC (display, data);
}

// ***************************************************************************
// wird von dependent_displaySQ aufgerufen, wenn die display_expression die
// array-notation RXmore_than_one enthaelt.
//
void DataDisp::dependent_displaysSQA (string display_expression,
				      int disp_nr)
{
    assert (display_expression.contains (RXmore_than_one));

    // einzelne display-Ausdruecke erzeugen und display-Befehle im array 
    // abschicken...
    string prefix  = display_expression.before(RXmore_than_one);
    string postfix = display_expression.after(RXmore_than_one);
    string range   = display_expression.from (RXmore_than_one);
    range.del("[");
    int start = ::get_nr(range);
    range = range.after("..");
    int stop = ::get_nr(range);

    if (start > stop)
    {
	post_error("Invalid range in " + quote(display_expression), 
		   "invalid_range_error");
	return;
    }

    assert (stop >= start);
    int count = stop + 1 - start;

    string *display_cmds = new string[count];
    string *print_cmds   = new string[count];

    int j = 0;
    for (int i = start; i < stop + 1; i++) {
	display_cmds[j] = 
	    gdb->display_command(prefix + "[" + itostring (i) + "]" + postfix);
	print_cmds[j] = 
	    gdb->print_command(prefix + "[" + itostring (i) + "]" + postfix);
	j++;
    }

    switch (gdb->type())
    {
    case GDB:
	{
	    void** dummy = new void *[count];
	    int k = gdb->send_qu_array (display_cmds,
					dummy,
					count,
					dependent_displaysOQAC,
					(void *)disp_nr);
	    if (k == 0)
		post_gdb_busy(last_origin);

	    delete[] dummy;
	}
	break;

    case DBX:
    case XDB:
	{
	    for (int i = 0; i < count; i++)
		gdb_question(display_cmds[i]);

	    void** dummy = new void *[count];
	    int k = gdb->send_qu_array (print_cmds,
					dummy,
					count,
					dependent_displaysOQAC,
					(void *)disp_nr);
	    if (k == 0)
		post_gdb_busy(last_origin);
	
	    delete[] dummy;
	}
	break;
    }

    delete[] display_cmds;
    delete[] print_cmds;
}

// ***************************************************************************
void DataDisp::dependent_displaysOQAC (string answers[],
				       void*  qu_datas[],
				       int    count,
				       void*  data)
{
    int       old_disp_nr = int (data);
    string    name;
    DispNode* dn = 0;

    string disabling_error_msgs = "";

    // Unselect all nodes
    for (GraphNode *gn = disp_graph->firstNode();
	 gn != 0; gn = disp_graph->nextNode(gn))
    {
	BoxGraphNode *bgn = ptr_cast(BoxGraphNode, gn);
	if (bgn)
	    bgn->selected() = false;
    }

    for (int i = 0; i < count; i++) {
	if (!contains_display (answers[i], gdb))
	{
	    disabling_error_msgs += answers[i] + '\n';
	}
	else 
	{
	    // DispNode erzeugen und ggf. disabling-Meldung ausgeben
	    string disp_nr_str;

	    switch(gdb->type())
	    {
	    case GDB:
		disp_nr_str = read_disp_nr_str (answers[i], gdb);
		if (disp_nr_str == "")
		    return;
		break;

	    case DBX:
	    case XDB:
		// Assign a default number
		disp_nr_str = itostring(next_display_number++);
		break;
	    }

	    name = read_disp_name (answers[i], gdb);
	    if (name == "")
	    {
		disabling_error_msgs += answers[i] + '\n';
	    }
	    else
	    {
		if (is_disabling (answers[i], gdb))
		{
		    disabling_error_msgs += answers[i] + '\n';
		    dn = new DispNode(disp_nr_str, name);
		}
		else
		{
		    dn = new DispNode(disp_nr_str, name, answers[i]);
		}

		BoxPoint box_point =
		    disp_graph->default_dependent_box_point(dn, 
							    graph_edit, 
							    old_disp_nr);
		
		dn->moveTo(box_point);
		dn->selected() = true;

		// in den Graphen einfuegen
		string nr = dn->disp_nr();
		disp_graph->insert_dependent(get_nr(nr), 
					     dn, old_disp_nr);
	    }
	}
    }

    delete[] answers;
    delete[] qu_datas;

    if (disabling_error_msgs != "")
	post_gdb_message (disabling_error_msgs, last_origin);

    refresh_graph_edit();
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
		disp_graph->del (k);
		changed = true;
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
    if (changed)
	refresh_graph_edit();

    fill_labels();
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
    if ( !disabling_occurred && disabling_error_msgs != "") {
	post_gdb_message (disabling_error_msgs, last_origin);
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


void DataDisp::process_user (StringArray& answers)
{
    int i = 0;

    bool changed = false;
    MapRef ref;
    for (int k = disp_graph->first_nr(ref); 
	     k != 0 ; 
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

// Create labels for the list
void DataDisp::fill_labels()
{
    if (display_list_w == 0)
	return;

    int max_count      = disp_graph->count_all();
    string *label_list = new string[max_count + 1];
    bool *selected     = new bool[max_count + 1];

    int count = 0;
    label_list[count] = 
	(max_count > 0) ? "Num Enb Expression" : "No displays.";
    selected[count]   = false;
    count++;

    MapRef ref;
    for (int k = disp_graph->first_nr(ref); 
	 k != 0 ; 
	 k = disp_graph->next_nr(ref))
    {
	ostrstream os;

	string num = itostring(k) + ":     ";
	os << num.through(3);

	DispNode* dn = disp_graph->get(k);
	if (dn->enabled())
	    os << "y";
	else
	    os << "n";
	os << "   " << dn->name();

	label_list[count] = os;
	selected[count]   = dn->selected();
	count++;
    }

    sort(label_list + 1, selected + 1, count - 1);

    ignore_update_graph_editor_selection = true;
    setLabelList(display_list_w, label_list, selected, count);
    ignore_update_graph_editor_selection = false;
	
    delete[] label_list;
    delete[] selected;
}


void DataDisp::EditDisplaysCB(Widget, XtPointer, XtPointer)
{
    XtManageChild(edit_displays_dialog_w);
    raise_shell(edit_displays_dialog_w);
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
	post_gdb_busy();
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

    MString prompt("Enter new value for ", "rm");
    prompt += MString(name, "tt");
    prompt += MString(":", "rm");

    Arg args[10];
    int arg = 0;

    XtSetArg(args[arg], XmNdeleteResponse, XmDESTROY); arg++;
    XtSetArg(args[arg], XmNselectionLabelString, prompt.xmstring()); arg++;
    Widget set_dialog = 
	verify(XmCreatePromptDialog(find_shell(w), "set_dialog", args, arg));

    Delay::register_shell(set_dialog);
    XtAddCallback(set_dialog, XmNokCallback,     setDCB, disp_value);
    XtAddCallback(set_dialog, XmNapplyCallback,  setDCB, disp_value);
    XtAddCallback(set_dialog, XmNhelpCallback,   ImmediateHelpCB, 0);
    XtAddCallback(set_dialog, XmNcancelCallback, DestroyThisCB,
		  (XtPointer)set_dialog);

    Widget apply = XmSelectionBoxGetChild(set_dialog, XmDIALOG_APPLY_BUTTON);
    XtManageChild(apply);
    XtManageChild(set_dialog);

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

    gdb_command(gdb->assign_command(disp_value->full_name(), value));
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

    gdb_command("graph display `" + name + "`");
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

    delete_displaySQ(killme);
    refresh_graph_edit();
}


//----------------------------------------------------------------------------
// Language changed - re-label buttons
//----------------------------------------------------------------------------

void DataDisp::language_changedHP(Agent *, void *, void *call_data)
{
    GDBAgent *gdb = (GDBAgent *)call_data;

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

    StringBox::fontTable     = new FontTable (XtDisplay(parent));
    DispBox::vsllib_name     = vsl_library;
    DispBox::vsllib_path     = vsl_path;
    DispBox::vsllib_defs     = vsl_defs;

    disp_graph = new DispGraph();

    disp_graph->addHandler(DispGraph_Empty, no_displaysHP);

    Arg args[10];
    int arg = 0;
    XtSetArg (args[arg], XtNgraph, (Graph *)disp_graph); arg++;

    if (panned)
	graph_edit = createPannedGraphEdit(parent, "graph_edit", args, arg);
    else
	graph_edit = createScrolledGraphEdit(parent, "graph_edit", args, arg);

    set_last_origin(graph_edit);

    graph_popup_w = 
	MMcreatePopupMenu(graph_edit, "graph_popup", graph_popup);
    InstallButtonTips(graph_popup_w);

    node_popup_w = 
	MMcreatePopupMenu(graph_edit, "node_popup", node_popup);
    InstallButtonTips(node_popup_w);

    disp_graph->callHandlers();

    XtAppAddActions (app_context, actions, XtNumber (actions));
    XtManageChild (graph_edit);

    graph_cmd_w = 
	verify(XmCreateRowColumn(parent, "graph_cmd_w", NULL, 0));

    verify(XtVaCreateManagedWidget("graph_arg_label",
				   xmLabelWidgetClass,
				   graph_cmd_w,
				   NULL));

    graph_arg = new ArgField (graph_cmd_w, "graph_arg");

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

    // Create display editor
    edit_displays_dialog_w =
	verify(XmCreatePromptDialog(graph_edit, "edit_displays_dialog", 
				    NULL, 0));
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

    XtAddCallback(graph_edit,
		  XtNselectionChangedCallback,
		  UpdateDisplayEditorSelectionCB,
		  0);
    XtAddCallback(graph_edit,
		  XtNcompareNodesCallback,
		  CompareNodesCB,
		  0);

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
