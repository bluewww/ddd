// $Id$
// Graph test

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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

char test_graph_rcsid[] = 
    "$Id$";

#include "assert.h"
#include <iostream.h>
#include <string.h>
#include <stdlib.h>

#include <Xm/Xm.h>
#include <Xm/ScrolledW.h>
#include <Xm/RowColumn.h>
#include <X11/Shell.h>
#include "ScrolledGE.h"
#include "PannedGE.h"
#include "GraphEdit.h"

#include "converters.h"
#include "MakeMenu.h"
#include "longName.h"

#include "StringBox.h"
#include "AlignBox.h"

#include "Graph.h"
#include "BoxGraphN.h"
#include "HintGraphN.h"
#include "ArcGraphE.h"

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

// Build a simple graph
Graph *myGraph()
{
    MarkBox *hugo    = new MarkBox(new StringBox("Select"));
    Box *berta       = new StringBox("Me");
    HAlignBox *hbox  = new HAlignBox();
    *hbox &= hugo;
    *hbox &= berta;

    GraphNode *n0 = new BoxGraphNode(hbox, BoxPoint(50, 50), hugo);

    GraphNode *n1 =
	new HintGraphNode(BoxPoint(65, 65));
    GraphNode *n2 =
	new BoxGraphNode(new StringBox("30, 30"), BoxPoint(30, 30));
    GraphNode *n3 =
	new BoxGraphNode(new StringBox("100, 30"), BoxPoint(100, 30));
    GraphNode *n4 =
	new BoxGraphNode(new StringBox("30, 100"), BoxPoint(30, 100));
    GraphNode *n5 =
	new BoxGraphNode(new StringBox("100, 100"), BoxPoint(100, 100));
    GraphNode *n6 =
	new BoxGraphNode(new StringBox("100, 100 a"), BoxPoint(100, 100));
    HintGraphNode *n7 =
	new HintGraphNode(BoxPoint(100, 50));
    GraphNode *n8 =
	new BoxGraphNode(new StringBox("100, 100 b"), BoxPoint(100, 100));

    n2->selected() = True;

    Graph *g = new Graph;
    assert(g->OK());

    *g += n0;
    assert(g->OK());

    *g += n1;
    assert(g->OK());

    *g += n2;
    assert(g->OK());

    *g += n3;
    assert(g->OK());

    *g += n4;
    assert(g->OK());

    *g += n5;
    assert(g->OK());

    *g += n6;
    assert(g->OK());

    *g += n7;
    assert(g->OK());

    *g += n8;
    assert(g->OK());

    *g += new LineGraphEdge(n0, n0);
    assert(g->OK());

    *g += new LineGraphEdge(n1, n2);
    assert(g->OK());

    *g += new LineGraphEdge(n1, n3);
    assert(g->OK());

    *g += new LineGraphEdge(n1, n4);
    assert(g->OK());

    *g += new LineGraphEdge(n1, n5);
    assert(g->OK());

    *g += new LineGraphEdge(n2, n3);
    assert(g->OK());

    *g += new LineGraphEdge(n2, n4);
    assert(g->OK());

    *g += new LineGraphEdge(n2, n5);
    assert(g->OK());

    *g += new LineGraphEdge(n3, n4);
    assert(g->OK());

    *g += new LineGraphEdge(n3, n5);
    assert(g->OK());

    *g += new LineGraphEdge(n4, n5);
    assert(g->OK());

    GraphEdge *e67 = new ArcGraphEdge(n6, n7); 
    *g += e67;
    assert(g->OK());

    GraphEdge *e78 = new ArcGraphEdge(n7, n8); 
    *g += e78;
    assert(g->OK());

    GraphEdge *e56 = new LineGraphEdge(n5, n6); 
    *g += e56;
    assert(g->OK());

    Graph *g2 = g->dup();
    assert(g->OK());
    assert(g2->OK());

    return g2;
}

static String fallback_resources[] = {
    "*allowShellResize: True",
    NULL
};

void PositionChanged(Widget, XtPointer, XtPointer call_data)
{
    GraphEditPositionChangedInfo *info = 
	(GraphEditPositionChangedInfo *)call_data;

    clog << "Moving node " << *info->node << " from " << 
	info->old_position << " to " << info->new_position;
    if (info->is_last)
    {
	clog << " (last)";
    }

    clog << "\n";
}


static void OneCB(Widget widget, XtPointer, XtPointer)
{
    cout << longName(widget) << "\n";
}

static void TwoCB(Widget widget, XtPointer, XtPointer)
{
    cout << longName(widget) << "\n";
}

static void ThreeCB(Widget widget, XtPointer, XtPointer)
{
    cout << longName(widget) << "\n";
}

static MMDesc simpleMenu[] =
{
    { "menu",  MMLabel },
    MMSep,
    { "one",   MMPush, { OneCB } },
    { "two",   MMPush, { TwoCB } },
    { "three", MMPush, { ThreeCB } },
    MMEnd
};

void Popup(Widget w, XEvent *event, String *, Cardinal *)
{
    if (event->type != ButtonPress && event->type != ButtonRelease)
    {
	cerr << "popup(): may only be used with button events\n";
	return;
    }

    static Widget popup = 0;

    if (popup == 0)
    {
	popup = MMcreatePopupMenu(w, "popup", simpleMenu);
	MMaddCallbacks(simpleMenu);
    }

    XmMenuPosition(popup, &event->xbutton);
    XtManageChild(popup);
}

XtActionsRec actions[] = {
    { "popup", Popup },
};


int main(int argc, char *argv[])
{
    Arg args[10];
    int arg;
    XtAppContext app_context;

    // Initialize it all
    Widget toplevel = XtAppInitialize(&app_context, "Test-graph",
        XtPointer(0), 0, &argc, argv, fallback_resources, XtPointer(0), 0);

    // register additional converters...
    registerOwnConverters();

    // register own actions...
    XtAppAddActions(app_context, actions, XtNumber(actions));

    // Build the font table...
    StringBox::fontTable = new FontTable(XtDisplay(toplevel));

    // Build a graph editor
    Graph *graph = myGraph();
    arg = 0;
    XtSetArg(args[arg], XtNgraph, graph); arg++;
    Widget editor = createPannedGraphEdit(toplevel, "graphEdit", 
					  args, arg);
    XtManageChild(editor);

    XtAddCallback(editor, XtNpositionChangedCallback, 
		  PositionChanged, XtPointer(0));

    XtRealizeWidget(toplevel);
    XtAppMainLoop(app_context);

    return EXIT_SUCCESS;
}
