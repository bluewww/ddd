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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char DataDisp_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#pragma implementation "Map.h"
#endif

#define LOG_DISPLAYS 0
#define LOG_COMPARE  0

//-----------------------------------------------------------------------------
// Implementation von DataDisp.h
//-----------------------------------------------------------------------------

#include "DataDisp.h"

// includes von Nora-Zeugs
#include "GraphEdit.h"   // fuer XtNgraph
#include "StringBox.h"   // fuer StringBox::fontTable
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

// includes von Motif-Zeugs
#include <Xm/MessageB.h>
#include <Xm/RowColumn.h>  // fuer XmMenuPosition()
#include <Xm/SelectioB.h>  // fuer XmCreatePromptDialog()
#include <Xm/TextF.h>      // fuer XmTextFieldGetString()
#include <Xm/Label.h>
#include <Xm/List.h>

// includes eigener Sachen
#include "DispNode.h" // fuer Konstruktoren
#include "disp-read.h"
#include "string-fun.h"
#include "comm-manag.h"
#include "Map.h"
#include "mydialogs.h"
#include "ddd.h"
#include "ArgField.h"
#include "verify.h"

// sonstige includes
#include <iostream.h>
#include <fstream.h> // fuer ofstream
#include <iomanip.h>
#include <ctype.h>

//-----------------------------------------------------------------------
// Xt-Zeugs
//-----------------------------------------------------------------------
XtActionsRec DataDisp::actions [] = {
    {"graph-select",         DataDisp::graph_selectAct},
    {"graph-select-or-move", DataDisp::graph_select_or_moveAct},
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
			      Dummy, Disable, Delete }; };

MMDesc DataDisp::node_popup[] =
{
    {"dereference",   MMPush,   {DataDisp::dereferenceCB}},
    {"detail",        MMPush,   {DataDisp::toggleDetailCB}},
    {"rotate",        MMPush,   {DataDisp::toggleRotateCB}},
    {"dependent",     MMPush,   {DataDisp::dependentCB}},
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
    {"disable",       MMPush, {DataDisp::toggleDisableCB}},
    {"delete",        MMPush, {DataDisp::deleteCB}},
    MMEnd
};

struct DisplayItms { enum Itms {New, Dependent, Dereference, 
				ShowDetail, HideDetail,
				Enable, Disable, Delete}; };

MMDesc DataDisp::display_area[] =
{
    {"new",          MMPush,   {DataDisp::newCB }},
    {"dependent",    MMPush,   {DataDisp::dependentCB }},
    {"dereference",  MMPush,   {DataDisp::dereferenceCB }},
    {"show_detail",  MMPush,   {DataDisp::showDetailCB}},
    {"hide_detail",  MMPush,   {DataDisp::hideDetailCB}},
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


//----------------------------------------------------------------------------
// Sensitivity
//-----------------------------------------------------------------------------

inline void set_sensitive(Widget w, bool state)
{
    if (w == 0 || XtIsSensitive(w) == state)
	return;
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
void DataDisp::dereferenceCB(Widget w, XtPointer, XtPointer)
{
    last_origin = w;

    DispNode *disp_node_arg   = selected_node();
    DispValue *disp_value_arg = selected_value();
    if (disp_node_arg == 0 || disp_value_arg == 0)
	return;

    string display_expression = disp_value_arg->dereferenced_name();
    disp_value_arg->dereference();
    disp_node_arg->refresh();

    string nr = disp_node_arg->disp_nr();
    dependent_displaySQ (display_expression, get_positive_nr(nr));
}

void DataDisp::toggleDetailCB(Widget dialog, XtPointer, XtPointer)
{
    last_origin = dialog;

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

	    if (dv->collapsed())
		dv->expand();
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
    last_origin = dialog;

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
	    
	    if (dv->collapsed())
	    {
		dv->expand();
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
    last_origin = dialog;

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
    last_origin = w;

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
    last_origin = dialog;

    int *disp_nrs = new int[disp_graph->count_all()];

    bool do_enable  = true;
    bool do_disable = true;

    int i = 0;
    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->selected())
	{
	    string nr = dn->disp_nr();
	    disp_nrs[i++] = get_positive_nr(nr);
	    if (dn->enabled())
		do_enable = false;
	    if (dn->disabled())
		do_disable = false;
	}
    }

    if (i > 0)
    {
	if (do_enable)
	    enable_displaySQ (disp_nrs, i);
	else if (do_disable)
	    disable_displaySQ (disp_nrs, i);
    }

    delete[] disp_nrs;
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
    last_origin = dialog;

    int count = disp_graph->count_all();
    int *disp_nrs = new int[count];
    GraphNode **ancestors   = new GraphNode *[count];
    GraphNode **descendants = new GraphNode *[count];

    int i = 0;
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
	    disp_nrs[i++] = get_positive_nr(nr);

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

    if (i > 0)
	delete_displaySQ (disp_nrs, i);

    while (a > 0)
	select_with_all_descendants(ancestors[--a]);
    while (d > 0)
	select_with_all_ancestors(descendants[--d]);

    refresh_graph_edit();

    delete[] disp_nrs;
    delete[] descendants;
    delete[] ancestors;
}


void DataDisp::dependentCB(Widget w, XtPointer, XtPointer)
{
    last_origin = w;

    DispNode *disp_node_arg   = selected_node();
    DispValue *disp_value_arg = selected_value();
    if (disp_node_arg == 0 || disp_value_arg == 0)
	return;

    string nr = disp_node_arg->disp_nr();
    static int* disp_nr_ptr = new int;
    *disp_nr_ptr = get_positive_nr(nr);

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
    last_origin = w;
    refresh_graph_edit();
}

void DataDisp::selectAllCB(Widget w, XtPointer, XtPointer)
{
    last_origin = w;
    XtCallActionProc(graph_edit, 
		     "select-all", (XEvent *)0, (String *)0, 0);
    refresh_graph_edit();
}

void DataDisp::newCB(Widget w, XtPointer, XtPointer)
{
    last_origin = w;
    new_displayCD();
}

void DataDisp::enableCB(Widget w, XtPointer, XtPointer)
{
    last_origin = w;

    int *disp_nrs = new int[disp_graph->count_all()];

    int i = 0;
    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->selected() && dn->disabled())
	{
	    string nr = dn->disp_nr();
	    disp_nrs[i++] = get_positive_nr(nr);
	}
    }

    if (i > 0)
	enable_displaySQ (disp_nrs, i);

    delete[] disp_nrs;
}

void DataDisp::disableCB(Widget w, XtPointer, XtPointer)
{
    last_origin = w;

    int *disp_nrs = new int[disp_graph->count_all()];

    int i = 0;
    MapRef ref;
    for (DispNode* dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	if (dn->selected() && dn->enabled())
	{
	    string nr = dn->disp_nr();
	    disp_nrs[i++] = get_positive_nr(nr);
	}
    }

    if (i > 0)
	disable_displaySQ (disp_nrs, i);

    delete[] disp_nrs;
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
    last_origin = display_dialog;

    BoxPoint* p = (BoxPoint *) client_data;
    new_displaySQ (source_arg->get_string(), p);
}


// ***************************************************************************
//
void DataDisp::popup_newCB (Widget    display_dialog,
			    XtPointer client_data,
			    XtPointer)
{
    last_origin = display_dialog;

    BoxPoint* p = (BoxPoint *) client_data;
    new_displayCD (*p);
}


// ***************************************************************************
//
void DataDisp::dependent_displayDCB (Widget    dialog, 
				     XtPointer client_data, 
				     XtPointer call_data)
{
    last_origin = dialog;

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
    last_origin = display_dialog;

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

// The GraphEdit `select' action with some data display magic prepended
void DataDisp::graph_selectAct (Widget w,
				XEvent* event,
				String* args,
				Cardinal* num_args)
{
    // Let multi-clicks pass right through
    Time t = time(event);
    if (Time(t - last_select_time) > Time(XtGetMultiClickTime(XtDisplay(w))))
	set_args(point(event));
    last_select_time = t;

    XtCallActionProc(graph_edit, "select", event, args, *num_args);
}

// The GraphEdit `select-or-move' action with some data display magic prepended
void DataDisp::graph_select_or_moveAct (Widget w,
					XEvent* event,
					String* args,
					Cardinal* num_args)
{
    // Let multi-clicks pass right through
    Time t = time(event);
    if (Time(t - last_select_time) > Time(XtGetMultiClickTime(XtDisplay(w))))
	set_args(point(event));
    last_select_time = t;

    XtCallActionProc(graph_edit, "select-or-move", event, args, *num_args);
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

    set_args(*p);

    Widget popup = 0;
    if (selected_node() == 0)
	popup = graph_popup_w;
    else
	popup = node_popup_w;

    XmMenuPosition(popup, &event->xbutton);
    XtManageChild(popup);
}

void DataDisp::set_args(BoxPoint p)
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

	disp_node->select(disp_value);
	graphEditRedrawNode(graph_edit, disp_bgn);
    }

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
		redraw |= node->selected();
		dn->nodeptr()->selected() = false;
	    }

	    redraw |= (node->highlight() != 0);
	    dn->select(0);

	    if (redraw)
		graphEditRedrawNode(graph_edit, node);
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
		if (dv->expanded())
		    count_selected_expanded++;
		if (dv->collapsed())
		    count_selected_collapsed++;
	    }
	}
    }

    if (count_selected > 0)
    {
	switch (gdb->type())
	{
	case DBX:
	    // DBX cannot disable or enable displays
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

    // New
    set_sensitive(graph_popup[GraphItms::New].widget,    gdb_ok);
    set_sensitive(display_area[DisplayItms::New].widget, gdb_ok);

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
	case Reference:
	    break;

	case Pointer: 
	    dereference_ok = gdb_ok;
	    break;

	case Array:
	    rotate_ok     = disp_value_arg->expanded();
	    rotate_state  = disp_value_arg->vertical_aligned();
	    break;

	case StructOrClass:
	case BaseClass:
	    break;

	default:
	    // Falscher Fehler
	    assert (0);
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
    set_sensitive(graph_cmd_area[ValueItms::Dependent].widget, 
		  count_selected == 1);
    set_sensitive(display_area[DisplayItms::Dependent].widget,
		  count_selected == 1);

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
    else if (count_selected_expanded == 0 && count_selected_collapsed > 0)
    {
	// Only collapsed displays selected
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
		  count_selected_collapsed);
    set_sensitive(display_area[DisplayItms::HideDetail].widget, 
		  count_selected_expanded);


    // Delete
    set_sensitive(graph_cmd_area[ValueItms::Delete].widget,
		  count_selected);
    set_sensitive(display_area[DisplayItms::Delete].widget, 
		  count_selected);

    // Argument field
    if (count_selected > 0)
    {
	string arg;
	if (disp_value_arg)
	    arg = disp_value_arg->full_name();
	else if (disp_node_arg)
	    arg = disp_node_arg->name();
	else
	{
	    ostrstream arg_os;
	    arg_os << "(" << count_selected << " displays)";
	    arg = arg_os;
	}
	graph_arg->set_string(arg);

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

    int *display_nrs = getDisplayNumbers(display_list_w);

    // Update graph editor selection
    MapRef ref;
    for (DispNode *dn = disp_graph->first(ref); 
	 dn != 0;
	 dn = disp_graph->next(ref))
    {
	string nr = dn->disp_nr();
	int display_nr = get_positive_nr(nr);

	bool select = false;
	for (int i = 0; display_nrs[i] > 0; i++)
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

    delete[] display_nrs;

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

regex RXmore_than_one ("\\[[0-9]+\\.\\.[0-9]+\\]", true);

// ***************************************************************************
// sendet den Display-Befehl an den gdb
// wird ein Boxpoint uebergeben, so wird der neue Knoten dorthin gesetzt
// sonst an eine Default-Position.
//
void DataDisp::new_displaySQ (string display_expression, BoxPoint* p,
			      Widget origin)
{
    if (origin)
	last_origin = origin;

    if (display_expression == "")
	return;
    if (display_expression.contains (RXmore_than_one)) {
	new_displaysSQA (display_expression, p);
	return;
    }

    switch (gdb->type())
    {
    case GDB:
    {
	string cmd = "display " + display_expression;
	int i = gdb->send_question (cmd, new_displayOQC, p);
	if (i == 0) {
	    post_gdb_busy(last_origin);
	}
    }
    break;

    case DBX:
    {
	gdb_question("display " + display_expression);
	string cmd = "print " + display_expression;
	int i = gdb->send_question (cmd, new_displayOQC, p);
	if (i == 0) {
	    post_gdb_busy(last_origin);
	}
    }
    break;
    }
}


DispNode *DataDisp::new_node (string& answer)
{
    string disp_nr_str;
    switch(gdb->type())
    {
    case GDB:
	disp_nr_str = read_disp_nr_str (answer, gdb->type());
	if (disp_nr_str == "")
	    return 0;
	break;

    case DBX:
	// Assign a default number
	disp_nr_str = itostring(disp_graph->count_all() + 1);
	break;
    }

    string name = read_disp_name (answer, gdb->type());
    DispNode* dn = 0;

    if (is_disabling (answer, gdb->type()))
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


// ***************************************************************************
// erzeugt neuen Display-Knoten und setzt ihn an den im data-Argument
// angegebenen Punkt (falls (BoxPoint *)data != (-1,-1))
//
void DataDisp::new_displayOQC (const string& answer, void* data)
{
    if (answer == "") {
	// Problemfall bei Start mit core-file, display-Ausgabe nur bei
	// display-Befehl
	gdb->send_question ("display",
			    new_display_extraOQC,
			    data);
    }
    else if (!contains_display (answer, gdb->type())) {
	post_gdb_message (answer, last_origin);
    }
    else {
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
	DispNode *dn = new_node(ans);
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
	disp_graph->insert_new (get_positive_nr(nr), dn);
	refresh_graph_edit();
    }
}

// ***************************************************************************
// Aus den Display-Ausdruecken den ersten (neuen) herausfischen, und dann
// der normalen Verarbeitung zufuehren.
//
void DataDisp::new_display_extraOQC (const string& answer, void* data)
{
    string ans = answer;
    string display = read_next_display (ans, gdb->type());
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
    string prefix = display_expression.before(RXmore_than_one);
    string postfix = display_expression.after(RXmore_than_one);
    string range = display_expression.from (RXmore_than_one);
    range.del("[");
    int start = get_nr(range);
    assert(start >= 0);
    range = range.after("..");
    int stop = get_nr (range);
    assert (stop >= start);
    int count = stop + 1 - start;

    string *display_cmds = new string[count];
    string *print_cmds   = new string[count];

    int j = 0;
    for (int i = start; i < stop + 1; i++) {
	display_cmds[j] = 
	    "display " + prefix + "[" + itostring (i) + "]" + postfix;
	print_cmds[j] = 
	    "print " + prefix + "[" + itostring (i) + "]" + postfix;
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
	if (k == 0) {
	    post_gdb_busy(last_origin);
	}
	delete[] dummy;
    }
    break;

    case DBX:
    {
	for (int i = 0; i < count; i++)
	    gdb_question(display_cmds[i]);

	void** dummy = new void *[count];
	int k = gdb->send_qu_array (print_cmds,
				    dummy,
				    count,
				    new_displaysOQAC,
				    p);
	if (k == 0) {
	    post_gdb_busy(last_origin);
	}
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
	if (!contains_display (answers[i], gdb->type()))
	    post_gdb_message (answers[i], last_origin);
	else {

	    // DispNode erzeugen und ggf. disabling-Meldung merken
	    DispNode *dn = new_node(answers[i]);
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
	    disp_graph->insert_new (get_positive_nr(nr), dn);
	}
    }
    delete[] answers;
    delete[] qu_datas;

    refresh_graph_edit();
}


//-----------------------------------------------------------------------------
// Den disp-graph auf aktuellen Stand bringen.
//-----------------------------------------------------------------------------

// ***************************************************************************
// sendet den Befehl "display" an den gdb,
// um Displays zu aktualisieren. (kein Loeschen von Displays)
//
void DataDisp::refresh_displaySQ () 
{
    int i = gdb->send_question ("display", refresh_displayOQC, 0);
    if (i == 0) {
	post_gdb_busy(last_origin);
    }
}

// ***************************************************************************
// Aktualisiert die Displays entsprechend.
//
void DataDisp::refresh_displayOQC (const string& answer, void *)
{
    bool disabling_occurred;

    string ans = answer;

    // Antwort auf 'display' auswerten
    string not_my_displays = process_displays (ans, disabling_occurred);

    // Bei Fehlermeldung (Disabling...) nochmal refresh.
    if (disabling_occurred) {
	refresh_displaySQ();
    }
    else {
	refresh_graph_edit();
    }
}

// ***************************************************************************
// sendet die Befehle "info display" und "display" an den gdb,
// um Displays zu aktualisieren.
//
void DataDisp::refresh_displaySQA (Widget origin)
{
    if (origin)
	last_origin = origin;

    string cmds[2];
    void*  dummy[2];

    int n = 0;
    switch (gdb->type())
    {
    case GDB:
	cmds[n++] = "info display";
	cmds[n++] = "display";
	break;

    case DBX:
	cmds[n++] = "display";
    };

    int i = gdb->send_qu_array(cmds, dummy, n, refresh_displayOQAC, 0);
    if (i == 0) {
	post_gdb_busy(last_origin);
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
    if (count >= 1)
    {
	// Antwort auf 'info display' auswerten (evtl displays loeschen)
	process_info_display (answers[0]);
    }
    
    // Antwort auf 'display' auswerten
    bool disabling_occurred = false;

    if (count >= 2)
    {
	process_displays (answers[1], disabling_occurred);
    }

    delete[]answers;
    delete[]qu_datas;

    // Bei Fehlermeldung (Disabling...) nochmal refresh.
    if (disabling_occurred)
    {
	refresh_displaySQ();
    }
}



//-----------------------------------------------------------------------------
// Displays ausschalten (disable)
//-----------------------------------------------------------------------------

// ***************************************************************************
// sendet den 'disable display'-Befehl mit den Nummern an den gdb
// und aktualisiert den disp_graph.
//
void DataDisp::disable_displaySQ (int display_nrs[], int count)
{
    if (count == 0)
	return;

    int j;
    string cmd = "disable display ";
    for (j = 0; j < count; j++) {
	cmd += " " + itostring(display_nrs[j]);
    }
    int i = gdb->send_question (cmd, disable_displayOQC, 0);
    if (i == 0)
	post_gdb_busy(last_origin);
    else {
	for (j = 0; j < count; j++) {
	    disp_graph->get(display_nrs[j])->disable();
	}
	refresh_graph_edit();
    }
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
void DataDisp::enable_displaySQ (int display_nrs[], int count)
{
    if (count == 0)
	return;

    int j;
    string cmd = "enable display";
    for (j = 0; j < count; j++) {
	cmd += " " + itostring(display_nrs[j]);
    }
    int i = gdb->send_question (cmd, enable_displayOQC, 0);
    if (i == 0)
       post_gdb_busy(last_origin);
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
void DataDisp::delete_displaySQ (int display_nrs[], int count)
{
    if (count == 0)
	return;

    int j;
    string cmd = "undisplay ";

    switch (gdb->type())
    {
    case GDB:
	for (j = 0; j < count; j++)
	{
	    cmd += " " + itostring(display_nrs[j]);
	}
	break;

    case DBX:
	int c = 0;
	for (j = 0; j < count; j++)
	{
	    DispNode *dn = disp_graph->get(display_nrs[j]);
	    if (dn)
	    {
		if (c++)
		    cmd += ", ";
		cmd += " " + dn->name();
	    }
	}
	break;
    }
	
    int i = gdb->send_question (cmd, delete_displayOQC, 0);
    if (i == 0)
	post_gdb_busy(last_origin);
    else {
	for (j = 0; j < count; j++) {
	    disp_graph->del (display_nrs[j]);
	}
	// refresh_graph_edit();
    }
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
	// Upon `undisplay', DBX redisplays remaining displays with
	// values
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
	string cmd = "display " + display_expression;
	int i = gdb->send_question(cmd, dependent_displayOQC, 
				   (void *) disp_nr);
	if (i == 0) {
	    post_gdb_busy(last_origin);
	}
    }
    break;

    case DBX:
    {
	gdb_question("display " + display_expression);
	string cmd = "print " + display_expression;
	int i = gdb->send_question(cmd, dependent_displayOQC, 
				   (void *) disp_nr);
	if (i == 0) {
	    post_gdb_busy(last_origin);
	}
    }
    break;
    }
}

// ***************************************************************************
// ruft disp_graph->insert_dependent (answer, data).
//
void DataDisp::dependent_displayOQC (const string& answer, void* data)
{
    if (answer == "") {
	// Problemfall bei Start mit core-file, display-Ausgabe nur bei
	// display-Befehl
	gdb->send_question
	    ("display", dependent_display_extraOQC, data);
    }
    else if (!contains_display (answer, gdb->type())) {
	post_gdb_message (answer, last_origin);
    }
    else {
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
	DispNode *dn = new_node(ans);
	if (dn == 0)
	    return;

	int old_disp_nr = int (data);
	BoxPoint box_point =
	    disp_graph->default_dependent_box_point(dn, 
						    graph_edit, old_disp_nr);
	dn->moveTo(box_point);
	dn->selected() = true;

	// in den Graphen einfuegen
	string nr = dn->disp_nr();
	disp_graph->insert_dependent (get_positive_nr(nr), dn, old_disp_nr);
	refresh_graph_edit();
    }
}

// ***************************************************************************
// Aus den Display-Ausdruecken den ersten (neuen) herausfischen, und dann
// der normalen Verarbeitung zufuehren.
//
void DataDisp::dependent_display_extraOQC (const string& answer, void* data)
{
    string ans = answer;
    string display = read_next_display (ans, gdb->type());
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
    string prefix = display_expression.before(RXmore_than_one);
    string postfix = display_expression.after(RXmore_than_one);
    string range = display_expression.from (RXmore_than_one);
    range.del("[");
    int start = get_nr(range);
    assert(start >= 0);
    range = range.after("..");
    int stop = get_nr (range);
    assert (stop >= start);
    int count = stop + 1 - start;

    string *display_cmds = new string[count];
    string *print_cmds   = new string[count];

    int j = 0;
    for (int i = start; i < stop + 1; i++) {
	display_cmds[j] = 
	    "display " + prefix + "[" + itostring (i) + "]" + postfix;
	print_cmds[j] = 
	    "print " + prefix + "[" + itostring (i) + "]" + postfix;
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
	if (k == 0) {
	    post_gdb_busy(last_origin);
	}
	delete[] dummy;
    }
    break;

    case DBX:
    {
	for (int i = 0; i < count; i++)
	    gdb_question(display_cmds[i]);
	void** dummy = new void *[count];
	int k = gdb->send_qu_array (print_cmds,
				    dummy,
				    count,
				    dependent_displaysOQAC,
				    (void *)disp_nr);
	if (k == 0) {
	    post_gdb_busy(last_origin);
	}
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
	if (!contains_display (answers[i], gdb->type()))
	    post_gdb_message (answers[i], last_origin);
	else {
	    // DispNode erzeugen und ggf. disabling-Meldung ausgeben
	    string disp_nr_str;

	    switch(gdb->type())
	    {
	    case GDB:
		disp_nr_str = read_disp_nr_str (answers[i], gdb->type());
		if (disp_nr_str == "")
		    return;
		break;

	    case DBX:
		// Assign a default number
		disp_nr_str = itostring(disp_graph->count_all() + 1);
		break;
	    }

	    name = read_disp_name (answers[i], gdb->type());
	    if (is_disabling (answers[i], gdb->type()))
	    {
		disabling_error_msgs += answers[i];
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
	    disp_graph->insert_dependent(get_positive_nr(nr), dn, old_disp_nr);
	}
    }

    delete[] answers;
    delete[] qu_datas;

    if ( disabling_error_msgs != "")
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
    Map <int, string> info_disp_string_map;
    string *strptr;

    string next_disp_info = 
	read_first_disp_info (info_display_answer, gdb->type());
    while (next_disp_info != "")
    {
	disp_nr = get_positive_nr (next_disp_info);
	if (disp_nr >= 0)
	{
	    if (disp_graph->contains(disp_nr)) 
	    {
		// Eigenes display-info
		strptr = 
		    new string(get_info_disp_str(next_disp_info, gdb->type()));
		info_disp_string_map.insert (disp_nr, strptr);
	    }
	}
	next_disp_info = 
	    read_next_disp_info(info_display_answer, gdb->type());
    }


    // Eigene Display-Infos verarbeiten
    bool changed = false;
    MapRef ref;
    for (int k = disp_graph->first_nr(ref); 
	     k != 0 ; 
	     k = disp_graph->next_nr(ref))
    {
	if (!info_disp_string_map.contains (k))
	{
	    // Knoten loeschen
	    disp_graph->del (k);
	    changed = true;
	}
	else
	{
	    // Knoten aktualisieren
	    DispNode* dn = disp_graph->get (k);
	    if (disp_is_disabled(*(info_disp_string_map.get (k)), gdb->type()))
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
    Map <int, string> disp_string_map;
    string *strptr;

#if LOG_DISPLAYS
    clog << "Processing displays " << quote(displays) << "...\n";
#endif

    string next_display = read_next_display (displays, gdb->type());
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
	{
	    disp_nr = 0;
	    string disp_name = next_display;
	    disp_name = read_disp_name(disp_name, gdb->type());
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

#if LOG_DISPLAYS
	clog << " (number " << disp_nr << ")\n";
#endif

	// Falls Fehlermeldung: merken und alles nochmal von vorne.
	if (is_disabling (next_display, gdb->type())) 
	{
	    disabling_occurred = true;
	    if (disp_nr >= 0 && disp_graph->contains(disp_nr))
	    {
		disabling_error_msgs += 
		    get_disp_value_str(next_display, gdb->type());
	    }
	    else
	    {
		not_my_displays = next_display; //nur diesen merken
	    }

	    // disp_string_map leeren
	    disp_string_map.delete_all_contents();

	    return not_my_displays;
	}

	if (is_not_active (next_display, gdb->type()))
	{
	    // Display is not active: don't insert it into map.
	    // This way, it will be shown as disabled.
	}
	else if (disp_nr >= 0 && disp_graph->contains(disp_nr))
	{
	    strptr = new string(get_disp_value_str(next_display, gdb->type()));
	    disp_string_map.insert (disp_nr, strptr);
	}
	else 
	{
	    not_my_displays += next_display + '\n';
	}

	next_display = read_next_display (displays, gdb->type());
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
	     k = disp_graph->next_nr(ref)) {
	DispNode* dn = disp_graph->get(k);

	if (!disp_string_map.contains (k)) {
	    // Knoten auf disabled setzen, falls nicht schon geschehen
	    if (dn->enabled()) {
		dn->disable();
		changed = true;
	    }
	}
	else {
	    // Knoten aktualisieren
	    strptr = disp_string_map.get(k);
	    if ( dn->update (*strptr ))
		changed = true;
	    if ( (*strptr != "") && !(strptr->matches (rxwhite)) ) {
		// hinter dem Display stand noch etwas, was nicht dazu gehoert
		// z.B. der return Wert bei finish
		not_my_displays += strptr->after (rxwhite);
	    }
	    delete disp_string_map.get(k);
	    disp_string_map.del (k);
	}
    }
    assert (disp_string_map.length() == 0);
    if (changed) 
	refresh_graph_edit();

    return not_my_displays;
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
}


//----------------------------------------------------------------------------
// Constructor
//----------------------------------------------------------------------------
DataDisp::DataDisp (XtAppContext app_context,
		    Widget parent,
		    String /* vslpath */,
		    String vslLibrary,
		    int    max_name_length,
		    bool   panned)
{
    static const char builtin_def[] = 
#include "ddd.vsl.h"
	;

    registerOwnConverters();

    StringBox::fontTable = new FontTable (XtDisplay(parent));

    if (string(vslLibrary) == "builtin")
    {
	istrstream is(builtin_def);
	DispBox::vsllibptr = new VSLLib(is);
    }
    else
    {
	DispBox::vsllibptr = new VSLLib (vslLibrary);
    }
    DispBox::max_name_length = max_name_length;

    disp_graph = new DispGraph();

    disp_graph->addHandler(DispGraph_Empty, no_displaysHP);

    Arg args[10];
    int arg = 0;
    XtSetArg (args[arg], XtNgraph, (Graph *)disp_graph); arg++;

    if (panned)
	graph_edit = createPannedGraphEdit(parent, "graph_edit", args, arg);
    else
	graph_edit = createScrolledGraphEdit(parent, "graph_edit", args, arg);

    last_origin = graph_edit;

    graph_popup_w = 
	MMcreatePopupMenu(graph_edit, "graph_popup", graph_popup);
    node_popup_w = 
	MMcreatePopupMenu(graph_edit, "node_popup", node_popup);

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
    XtVaSetValues(graph_cmd_w,
		  XmNpaneMaximum, size.height,
		  XmNpaneMinimum, size.height,
		  NULL);

#ifdef LESSTIF_VERSION
    // Not available in LessTif 0.1
    edit_displays_dialog_w = 0;
    display_list_w         = 0;
#else
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
#endif

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
