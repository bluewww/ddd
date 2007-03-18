/* High-level GUI wrapper for Xm. */

/* Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
 * 
 * This file is part of Xmmm.
 * 
 * Xmmm is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * Xmmm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with Xmmm -- see the file COPYING.
 * If not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

static const char rcsid[] = "$Header:$";

#include <stdio.h>

#include <Xm/RowColumn.h>
#include <Xm/Frame.h>
#include <Xm/ToggleB.h>

#include <Xmmm/Box2.h>
#include <Xmmm/Box2P.h>
#include <Xmmm/Notebook2.h>
#include <Xmmm/Notebook2P.h>

#undef MAX
#define MAX(x,y) ((x)>(y)?(x):(y))
#undef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))

#define MINIMUM_HEIGHT 24
#define MINIMUM_WIDTH 24

/* Forward Declarations */

static void class_initialize(void);
static void class_part_initialize(WidgetClass w_class);
static void initialize(Widget request, Widget new_w, ArgList args,
		       Cardinal *num_args);
static void destroy(Widget w);
static void resize(Widget w);
static void realize(Widget w, XtValueMask *value_mask,
		    XSetWindowAttributes *attributes);
static void expose(Widget w, XEvent *event, Region region);
static XtGeometryResult query_geometry(Widget w, XtWidgetGeometry *proposed,
				       XtWidgetGeometry *answer);
static Boolean set_values(Widget current, Widget request, Widget new_w,
			  ArgList args, Cardinal *num_args);

static XtGeometryResult geometry_manager(Widget w, XtWidgetGeometry *request,
					 XtWidgetGeometry *reply);
static void change_managed(Widget w);
static void insert_child(Widget w);
static void delete_child(Widget w);
static void constraint_initialize(Widget request, Widget neww,
				  ArgList args, Cardinal *num_args);
static Boolean constraint_set_values(Widget current, Widget request, Widget neww,
				     ArgList args, Cardinal *num_args);

static XtGeometryResult _XmmmNBAdjustSize(Widget b2, Widget instig, XtWidgetGeometry *instig_request);
static void _XmmmNBPreferredSize(Widget b2, XtWidgetGeometry *b2g);

static void
PreferredSizeAfterResize(Widget widget, XtWidgetGeometry *preferred,
			 Widget instig, XtWidgetGeometry *instig_request);
static void AdaptToSize(Widget rc, Widget instig, XtWidgetGeometry *pref);


#define MGR_NumChildren(m) \
	(((XmManagerWidget)(m))->composite.num_children)

#define MGR_Children(m) \
	(((XmManagerWidget)(m))->composite.children)

#define MGR_TopShadowGC(m) \
	(((XmManagerWidget)(m))->manager.top_shadow_GC)

#define MGR_TopShadowColor(m) \
	(((XmManagerWidget)(m))->manager.top_shadow_color)

#define MGR_TopShadowPixmap(m) \
	(((XmManagerWidget)(m))->manager.top_shadow_pixmap)

#define MGR_BottomShadowGC(m) \
	(((XmManagerWidget)(m))->manager.bottom_shadow_GC)

#define MGR_BottomShadowColor(m) \
	(((XmManagerWidget)(m))->manager.bottom_shadow_color)

#define MGR_BottomShadowPixmap(m) \
	(((XmManagerWidget)(m))->manager.bottom_shadow_pixmap)


#define Offset(field) XtOffsetOf(XmmmNotebookRec, notebook2.field)

/* Resources for the Notebook class */
static XtResource resources[] =
{
};

#undef Offset

#define Offset(field) XtOffsetOf(XmmmNotebookConstraintRec, notebook2.field)

static XtResource notebook2ConstraintResources[] =
{
};

#undef Offset

XmmmNotebookClassRec xmmmNotebookClassRec =
{
    /* Core class part */
    {
	/* superclass            */ (WidgetClass) & xmmmBox2ClassRec,
	/* class_name            */ "XmmmNotebook",
	/* widget_size           */ sizeof(XmmmNotebookRec),
	/* class_initialize      */ class_initialize,
	/* class_part_initialize */ class_part_initialize,
	/* class_inited          */ False,
	/* initialize            */ initialize,
	/* initialize_hook       */ (XtPointer)0,
	/* realize               */ realize,
	/* actions               */ (XtPointer)0,
	/* num_actions           */ 0,
	/* resources             */ resources,
	/* num_resources         */ XtNumber(resources),
	/* xrm_class             */ NULLQUARK,
	/* compress_motion       */ True,
	/* compress_exposure     */ XtExposeCompressMaximal /*XtExposeCompressMultiple*/,
	/* compress_enterleave   */ False /*True*/,
	/* visible_interest      */ False /*True*/,
	/* destroy               */ destroy,
	/* resize                */ XtInheritResize, /* FIXME resize */
	/* expose                */ expose,
	/* set_values            */ set_values,
	/* set_values_hook       */ (XtPointer)0,
	/* set_values_almost     */ XtInheritSetValuesAlmost,
	/* get_values_hook       */ (XtPointer)0,
	/* accept_focus          */ (XtPointer)0,
	/* version               */ XtVersion,
	/* callback offsets      */ (XtPointer)0,
	/* tm_table              */ XtInheritTranslations, /* should be NULL but
	                                                      we loose togglebg's in
	                                                      a work area if it is 
	                                                      togglegb/test1 */
	/* query_geometry        */ XtInheritQueryGeometry, /* FIXME query_geometry */
	/* display_accelerator   */ (XtPointer)0 /*XtInheritDisplayAccelerator*/,
	/* extension             */ (XtPointer)0
    },
    /* Composite class part */
    {
	/* geometry manager */	XtInheritGeometryManager, /* FIXME geometry_manager */
	/* change_managed   */	change_managed,
	/* insert_child     */	insert_child,
	/* delete_child     */	delete_child,
	/* extension        */	(XtPointer)0 /*&rcCompositeExt*/,
    },
    /* Constraint class part */
    {
	/* subresources      */ notebook2ConstraintResources,
	/* subresource_count */ XtNumber(notebook2ConstraintResources),
	/* constraint_size   */ sizeof(XmmmNotebookConstraintRec),
	/* initialize        */ constraint_initialize,
	/* destroy           */ (XtPointer)0,
	/* set_values        */ constraint_set_values,
	/* extension         */ (XtPointer)0,
    },
    /* XmManager class part */
    {
	/* translations           */ XtInheritTranslations,
	/* syn_resources          */ (XtPointer)0,
	/* num_syn_resources      */ 0,
	/* syn_constraint_res     */ (XtPointer)0,
	/* num_syn_constraint_res */ 0,
	/* parent_process         */ XmInheritParentProcess,
	/* extension              */ (XtPointer)0
    },
    /* XmmmNotebook Area part */
    {
	/* mumble */ 0
    },
};


WidgetClass xmmmNotebookWidgetClass = (WidgetClass)&xmmmNotebookClassRec;


static XtTranslations menubar_trans;
static XtTranslations option_trans;
static XtTranslations menu_trans;
static XtTranslations menutrav_trans;

static void
class_initialize(void)
{
}

static void
class_part_initialize(WidgetClass widget_class)
{
}

static void
initialize(Widget request, Widget new_w,
	   ArgList args, Cardinal *num_args)
{
    Arg args2[10];
    int nargs2;
    XmmmNotebookWidget nb = (XmmmNotebookWidget)new_w;
    nargs2 = 0;
    nb->notebook2.buttons = XmCreateRadioBox(new_w, "buttons", args2, nargs2);
    XtManageChild(nb->notebook2.buttons);
    nargs2 = 0;
    // Frame should expand to fill available space.
    XtSetArg(args2[nargs2], "pack_options",  2); nargs2++;
    nb->notebook2.frame = XmCreateFrame(new_w, "frame", args2, nargs2);
    XtManageChild(nb->notebook2.frame);
    nargs2 = 0;
    XtSetArg(args2[nargs2], XmNmarginWidth,  0); nargs2++;
    XtSetArg(args2[nargs2], XmNmarginHeight, 0); nargs2++;
    XtSetArg(args2[nargs2], XmNborderWidth,  0); nargs2++;
    nb->notebook2.change =
	XmCreateRowColumn(nb->notebook2.frame, "change", args2, nargs2);
    XtManageChild(nb->notebook2.change);
    nb->notebook2.current_pageno = 0;
}

static void
destroy(Widget w)
{
    XmmmNotebookWidget nb = (XmmmNotebookWidget)w;
    XtDestroyWidget(nb->notebook2.buttons);
    XtDestroyWidget(nb->notebook2.frame);
    XtDestroyWidget(nb->notebook2.change);
}

static Boolean
set_values(Widget old, Widget request, Widget new_w,
	   ArgList args, Cardinal *num_args)
{
    // FIXME?
    return False;
}

static void
realize(Widget w, XtValueMask *value_mask, XSetWindowAttributes *attributes)
{
    XmmmNotebookWidget nb = (XmmmNotebookWidget)w;
    CompositeWidget change = (CompositeWidget)nb->notebook2.change;
    xmmmBox2ClassRec.core_class.realize(w, value_mask, attributes);
    if (change->composite.num_children > 0) {
	XtManageChild(change->composite.children[0]);
    }
}

static void
resize(Widget w)
{
    _XmmmNBAdjustSize(w, (XtPointer)0, (XtPointer)0);

    if (XtIsRealized(w))
    {
	_XmClearShadowType(w, B2_OldWidth(w), B2_OldHeight(w),
			   B2_OldShadowThickness(w), 0);
	
	_XmDrawShadows(XtDisplay(w), XtWindow(w),
		       MGR_TopShadowGC(w), MGR_BottomShadowGC(w),
		       0, 0, XtWidth(w), XtHeight(w),
		       MGR_ShadowThickness(w), XmSHADOW_OUT);
    }

    B2_OldWidth(w) = XtWidth(w);
    B2_OldHeight(w) = XtHeight(w);
    B2_OldShadowThickness(w) = MGR_ShadowThickness(w);
}

static void
expose(Widget w, XEvent *event, Region region)
{
    _XmDrawShadows(XtDisplay(w), XtWindow(w),
		   MGR_TopShadowGC(w), MGR_BottomShadowGC(w),
		   0, 0, XtWidth(w), XtHeight(w),
		   MGR_ShadowThickness(w), XmSHADOW_OUT);

    /* display the gadgets, if there are any */
    _XmRedisplayGadgets(w, event, region);
}

static XtGeometryResult
query_geometry(Widget w,
	       XtWidgetGeometry *request,
	       XtWidgetGeometry *reply)
{
    XtWidgetGeometry b2g;
    XtGeometryResult result;

    Dimension width, height;

    fprintf(stderr, "Notebook2: query_geometry: %p(%s)\n", w, XtName(w));

    b2g = *request;
    _XmmmNBPreferredSize(w, &b2g);

#if 1
    /* test36 test38 test39 */
    b2g.width = b2g.width < 16 ? 16 : b2g.width;
    b2g.height = b2g.height < 16 ? 16 : b2g.height;

    width = (request->request_mode & CWWidth) ? (request->width < 16 ? 16 : request->width) : b2g.width;
    height = (request->request_mode & CWHeight) ? (request->height < 16 ? 16 : request->height) : b2g.height;
#endif

    reply->width = width;
    reply->height = height;

    /* _XmGMReplyToQueryGeometry(w, request, reply); */

    /* rather not change */
    reply->request_mode = CWWidth | CWHeight;

    if (request->width == reply->width && 
	request->height == reply->height) result = XtGeometryYes;
    else if (reply->width == XtWidth(w) &&
	     reply->height == XtHeight(w)) result = XtGeometryNo;
    else result = XtGeometryAlmost;
    return result;
}

/* This is called in response to a geometry request from the child w. */

static XtGeometryResult
geometry_manager(Widget instig,
		 XtWidgetGeometry *request,
		 XtWidgetGeometry *reply)
{
    Widget parent = XtParent(instig);
    XtWidgetGeometry wants;
    XtWidgetGeometry preferred;
    XtGeometryResult result;
    int ask, got;
    XtWidgetGeometry up_request, up_reply;

    wants = *request;
    *reply = wants;

    fprintf(stderr, "Notebook2: geometry_manager: %p(%s) > %p(%s)\n",
	    parent, XtName(parent), instig, XtName(instig));

    if (wants.request_mode & (CWX | CWY))
	return XtGeometryNo;

    PreferredSizeAfterResize(parent, &preferred, instig, request);
    fprintf(stderr, "Notebook2: preferred=%d,%d\n", preferred.width, preferred.height);

    if (preferred.height != XtHeight(parent) || preferred.width != XtWidth(parent)) {
	up_request.request_mode = 0;
	if (preferred.height != XtHeight(parent)) {
	    up_request.request_mode |= CWHeight;
	    up_request.height = preferred.height;
	}
	if (preferred.width != XtWidth(parent)) {
	    up_request.request_mode |= CWWidth;
	    up_request.width = preferred.width;
	}
	if (request->request_mode & XtCWQueryOnly)
	    up_request.request_mode |= XtCWQueryOnly;
	result = XtMakeGeometryRequest(parent, &up_request, &up_reply);
    }
    else {
	return XtGeometryYes;
    }

#if 0
    /* Try to do it */
    if (_XmmmNBAdjustSize(parent, instig, reply) == XtGeometryNo)
	return XtGeometryNo;
#endif

    /* Now check what we actually got */
    reply->request_mode &= wants.request_mode;

    ask = got = 0;

    if ((wants.request_mode & CWWidth) == CWWidth) {
	ask++;
	if (reply->width == wants.width) got++;
	else reply->request_mode &= ~CWWidth;
    }
    if ((wants.request_mode & CWHeight) == CWHeight) {
	ask++;
	if (reply->height == wants.height) got++;
	else reply->request_mode &= ~CWHeight;
    }
    if ((wants.request_mode & CWBorderWidth) == CWBorderWidth)
    {
	ask++;
	if (reply->border_width == wants.border_width) got++;
	else reply->request_mode &= ~CWBorderWidth;
    }

    /* are we ok with everything they want. */
    if (ask == got && ask != 0)
    {
	XmmmNotebookWidget nb = (XmmmNotebookWidget)parent;
	Widget buttons = nb->notebook2.buttons;
	Widget frame = nb->notebook2.frame;
	fprintf(stderr, "*** GEOMETRY OK - SO WHAT? ***\n");
	return XtGeometryYes;
    }
    else {
	reply->request_mode &= ~(CWX | CWY);
	return XtGeometryAlmost;
    }
}

static void
change_managed(Widget w)
{

    fprintf(stderr, "Notebook2: change_managed: %p(%s)\n", w, XtName(w));

    _XmmmNBAdjustSize(w, (XtPointer)0, (XtPointer)0);

    if (XtIsRealized(w))
    {
	_XmClearShadowType(w, B2_OldWidth(w), B2_OldHeight(w),
			   B2_OldShadowThickness(w), 0);

	_XmDrawShadows(XtDisplay(w), XtWindow(w),
		       MGR_TopShadowGC(w), MGR_BottomShadowGC(w),
		       0, 0, XtWidth(w), XtHeight(w),
		       MGR_ShadowThickness(w), XmSHADOW_OUT);
    }
    B2_OldWidth(w) = XtWidth(w);
    B2_OldHeight(w) = XtHeight(w);
    B2_OldShadowThickness(w) = MGR_ShadowThickness(w);

}

static void
insert_child(Widget w)
{
    XmmmNotebookWidget nb = (XmmmNotebookWidget)XtParent(w);
    if (nb->composite.num_children > 2) {
	/* Completed initialization. */
	fprintf(stderr, "Error: Notebook can have only 2 children\n");
    }
    else {
	xmmmBox2ClassRec.composite_class.insert_child(w);
    }
}

static void
delete_child(Widget w)
{
    xmmmBox2ClassRec.composite_class.delete_child(w);
}

static void
constraint_initialize(Widget request, Widget new_w,
		      ArgList args, Cardinal *num_args)
{
}


static Boolean
constraint_set_values(Widget current, Widget request, Widget new_w,
		      ArgList args, Cardinal *num_args)
{
}

Widget
XmmmCreateNotebook(Widget parent, char *name,
	       Arg *arglist, Cardinal argcount)
{
    return XtCreateWidget(name, xmmmNotebookWidgetClass, parent,
			  arglist, argcount);
}

/* Geometry management subroutines */

XtGeometryResult
_XmmmNBAdjustSize(Widget w, Widget instig, XtWidgetGeometry *instig_request)
{
    fprintf(stderr, "_XmmmNBAdjustSize: %p(%s)\n", w, XtName(w));
    return _XmmmB2AdjustSize(w, instig, instig_request);
}

typedef struct _KidGeom KidGeom;

extern void
XmmmBox2PreferredSizeAfterResize(Widget widget, XtWidgetGeometry *preferred,
				 Widget instig, XtWidgetGeometry *instig_request,
				 int *n_exp, KidGeom **);

/* Preferred size after resizing instig to instig_request */
/* Optionally returns number of expanding widgets and preferred
   geometry of children */
static void
PreferredSizeAfterResize(Widget widget, XtWidgetGeometry *preferred,
			 Widget instig, XtWidgetGeometry *instig_request)
{
    XmmmBox2PreferredSizeAfterResize(widget, preferred, instig, instig_request,
				     NULL, NULL);
}

void
_XmmmNBPreferredSize(Widget nb, XtWidgetGeometry *nbg)
{
    XtWidgetGeometry child_geom;
    XmmmNotebookWidget notebook;
    Widget buttons, frame;
    CompositeWidget change;
    int b_w, b_h, f_w, f_h;
    int w, h;
    int i;
    notebook = (XmmmNotebookWidget)nb;
    buttons = notebook->notebook2.buttons;
    frame = notebook->notebook2.frame;
    change = (CompositeWidget)notebook->notebook2.change;
    child_geom.request_mode = nbg->request_mode;
    XtQueryGeometry(buttons, (XtPointer)0, &child_geom);
    b_w = (child_geom.request_mode&CWWidth)?child_geom.width:buttons->core.width;
    b_h = (child_geom.request_mode&CWHeight)?child_geom.height:buttons->core.height;
    f_w = f_h = 0;
    for (i = 0; i < change->composite.num_children; i++) {
	Widget child = change->composite.children[i];
	child_geom.request_mode = nbg->request_mode;
	XtQueryGeometry(child, (XtPointer)0, &child_geom);
	w = (child_geom.request_mode&CWWidth)?child_geom.width:frame->core.width;
	h = (child_geom.request_mode&CWHeight)?child_geom.height:frame->core.height;
	f_w = MAX(f_w, w);
	f_h = MAX(f_h, h);
    }
    f_w += 10;
    f_h += 10;
    nbg->request_mode = (CWWidth|CWHeight);
    nbg->width = MAX(b_w, f_w);
    nbg->height = b_h + f_h;
}

static void
ChangePanelCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    Widget panel = (Widget)client_data;
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;
    Boolean set = cbs->set;
    XmmmNotebookWidget nb = (XmmmNotebookWidget)XtParent(XtParent(XtParent(w)));

    if (set)
    {
	int i;
	// Manage this child
	XtManageChild(panel);
#if 0
	XtAddCallback(preferences_dialog, XmNhelpCallback,
		      HelpOnThisCB, XtPointer(panel));
#endif

#if 0
	XtAddCallback(reset_preferences_w, XmNactivateCallback,
		      ResetPreferencesCB, XtPointer(panel));
#endif
#if 0
	current_panel = panel;

	update_reset_preferences();
#endif

	// Unmanage all other children
	WidgetList children;
	Cardinal num_children;
	XtVaGetValues(XtParent(panel), 
		      XmNchildren, &children,
		      XmNnumChildren, &num_children,
		      (XtPointer)0);

	for (i = 0; i < num_children; i++)
	{
	    Widget child = children[i];
	    if (child != panel)
	    {
		XtUnmanageChild(child);
#if 0
		XtRemoveCallback(preferences_dialog, XmNhelpCallback,
				 HelpOnThisCB, XtPointer(child));
		XtRemoveCallback(reset_preferences_w, XmNactivateCallback,
				 ResetPreferencesCB, XtPointer(child));
#endif
	    }
	}
    }
}

Widget
XmmmNotebookInsertPage(Widget w, char *name, Arg *arglist, Cardinal argcount)
{
    /* FIXME: At present page is always a RowColumn */
    XmmmNotebookWidget nb = (XmmmNotebookWidget)w;
    CompositeWidget change = (CompositeWidget)nb->notebook2.change;
    int npages = change->composite.num_children;
    Widget child = XmCreateRowColumn(nb->notebook2.change, name, arglist, argcount);
    Widget button;
    Arg args[10];
    int nargs;
    nargs = 0;
    button = XmCreateToggleButton(nb->notebook2.buttons, name, args, nargs);
    XtManageChild(button);
    XtAddCallback(button, XmNvalueChangedCallback, ChangePanelCB, 
		  (XtPointer)child);
    if (npages == 0)
	XtManageChild(child);
    return child;
}

