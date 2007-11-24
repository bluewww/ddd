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
#include <stdlib.h>

#include <Xmmm/XmmmNotebook.h>
#include <Xmmm/XmmmNotebookP.h>

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
static void PreferredSize(Widget widget, XtWidgetGeometry *geom);

typedef struct _KidGeom {
    Widget w;
    XtWidgetGeometry box;
    int type;
} KidGeom;

extern void
XmmmNotebookPreferredSizeAfterResize(Widget widget, XtWidgetGeometry *preferred,
				     Widget instig, XtWidgetGeometry *instig_request,
				     KidGeom **, int *button_ht);
static void AdaptToSize(Widget rc, Widget instig, XtWidgetGeometry *pref,
			KidGeom *boxes, int button_ht);


/* Resources for the XmmmNotebook class */
#define Offset(field) XtOffsetOf(XmmmNotebookRec, notebook.field)
static XtResource resources[] =
{
    {
	"activePage", "ActivePage", XmRCardinal,
	sizeof(short), Offset(current_pageno),
	XmRImmediate, (XtPointer)-1
    }
};
#undef Offset

#if 0

#define Offset(field) XtOffsetOf(XmmmNotebookConstraintRec, box_2.field)
static XtResource notebookConstraintResources[] =
{
    {
	"pack_options", "Pack_Options", XmRCardinal,
	sizeof(short), Offset(pack_options),
	XmRImmediate, (XtPointer)0
    }
};
#undef Offset

#endif

XmmmNotebookClassRec xmmmNotebookClassRec =
{
    /* Core class part */
    {
	/* superclass            */ (WidgetClass) & xmManagerClassRec,
	/* class_name            */ "XmmmNotebook",
	/* widget_size           */ sizeof(XmmmNotebookRec),
	/* class_initialize      */ class_initialize,
	/* class_part_initialize */ class_part_initialize,
	/* class_inited          */ False,
	/* initialize            */ initialize,
	/* initialize_hook       */ NULL,
	/* realize               */ realize,
	/* actions               */ NULL,
	/* num_actions           */ 0,
	/* resources             */ resources,
	/* num_resources         */ XtNumber(resources),
	/* xrm_class             */ NULLQUARK,
	/* compress_motion       */ True,
	/* compress_exposure     */ XtExposeCompressMaximal /*XtExposeCompressMultiple*/,
	/* compress_enterleave   */ False /*True*/,
	/* visible_interest      */ False /*True*/,
	/* destroy               */ destroy,
	/* resize                */ resize,
	/* expose                */ expose,
	/* set_values            */ set_values,
	/* set_values_hook       */ NULL,
	/* set_values_almost     */ XtInheritSetValuesAlmost,
	/* get_values_hook       */ NULL,
	/* accept_focus          */ NULL,
	/* version               */ XtVersion,
	/* callback offsets      */ NULL,
	/* tm_table              */ XtInheritTranslations, /* should be NULL but
	                                                      we loose togglebg's in
	                                                      a work area if it is 
	                                                      togglegb/test1 */
	/* query_geometry        */ query_geometry,
	/* display_accelerator   */ NULL /*XtInheritDisplayAccelerator*/,
	/* extension             */ NULL
    },
    /* Composite class part */
    {
	/* geometry manager */	geometry_manager,
	/* change_managed   */	change_managed,
	/* insert_child     */	insert_child,
	/* delete_child     */	delete_child,
	/* extension        */	(XtPointer)NULL /*&rcCompositeExt*/,
    },
    /* Constraint class part */
    {
	/* subresources      */ NULL,
	/* subresource_count */ 0,
	/* constraint_size   */ 0,
	/* initialize        */ NULL,
	/* destroy           */ NULL,
	/* set_values        */ NULL,
	/* extension         */ NULL,
    },
    /* XmManager class part */
    {
	/* translations           */ XtInheritTranslations,
	/* syn_resources          */ NULL,
	/* num_syn_resources      */ 0,
	/* syn_constraint_res     */ NULL,
	/* num_syn_constraint_res */ 0,
	/* parent_process         */ XmInheritParentProcess,
	/* extension              */ NULL
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
    XmmmNotebookWidget nb = (XmmmNotebookWidget)new_w;
    nb->notebook.num_buttons = 0;
    nb->notebook.max_buttons = 0;
    nb->notebook.buttons = NULL;
    nb->notebook.pages = NULL;
    nb->notebook.num_pages = 0;
    nb->notebook.max_pages = 0;
    nb->notebook.current_pageno = 0;
}

static void
destroy(Widget w)
{
}

static Boolean
set_values(Widget old, Widget request, Widget new_w,
	   ArgList args, Cardinal *num_args)
{
    Boolean need_refresh = False, need_relayout = False;

    if (need_relayout)
    {
	_XmmmB2AdjustSize(new_w, NULL, NULL);
    }

    return need_refresh;
}

static void
realize(Widget w, XtValueMask *value_mask, XSetWindowAttributes *attributes)
{
    xmManagerClassRec.core_class.realize(w, value_mask, attributes);
}

static void
resize(Widget w)
{
    int n_exp;
    XtWidgetGeometry preferred;
    KidGeom *boxes;
    int button_ht;
    fprintf(stderr, "resize %p(%s)\n", w, XtName(w));
    XmmmNotebookPreferredSizeAfterResize(w, &preferred, NULL, NULL,
					  &boxes, &button_ht);

    AdaptToSize(w, NULL, &preferred, boxes, button_ht);
}

static void
expose(Widget w, XEvent *event, Region region)
{
    XmmmNotebookWidget nb = (XmmmNotebookWidget)w;
    _XmDrawShadows(XtDisplay(w), XtWindow(w),
		   nb->manager.top_shadow_GC,
		   nb->manager.bottom_shadow_GC,
		   0, 0, XtWidth(w), XtHeight(w),
		   nb->manager.shadow_thickness,
		   XmSHADOW_OUT);

    /* display the gadgets, if there are any */
    _XmRedisplayGadgets(w, event, region);
}

static XtGeometryResult
query_geometry(Widget w,
	       XtWidgetGeometry *request,
	       XtWidgetGeometry *reply)
{
    XtWidgetGeometry geom;
    XtGeometryResult result;

    Dimension width, height;

    fprintf(stderr, "XmmmNotebook: query_geometry: %p(%s)\n", w, XtName(w));

    geom = *request;
    PreferredSize(w, &geom);

#if 1
    /* test36 test38 test39 */
    geom.width = geom.width < 16 ? 16 : geom.width;
    geom.height = geom.height < 16 ? 16 : geom.height;

    width = (request->request_mode & CWWidth) ? (request->width < 16 ? 16 : request->width) : geom.width;
    height = (request->request_mode & CWHeight) ? (request->height < 16 ? 16 : request->height) : geom.height;
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

static void
AdaptToSize(Widget rc, Widget instig, XtWidgetGeometry *pref, KidGeom *boxes,
	    int button_ht)
{
    /* We kept the preferred child sizes around in a cache, boxes */
    /* The requested size was pref, this contains the total height and
       maximum width requested by the kids */
    /* We may have been resized to a larger size */
    /* Distribute extra among the expanding kids */
    int i;
    int tot_w = XtWidth(rc);
    int tot_h = XtHeight(rc);
    int curr_x = 0;
    int curr_y = 0;
    int x, y, w, h, bw;
    for (i = 0; boxes[i].w; i++) {
	Widget child = boxes[i].w;
	bw = child->core.border_width;
	if (boxes[i].type == 0) {
	    /* Button */
	    x = curr_x;
	    y = 0;
	    w = boxes[i].box.width;
	    h = button_ht;
	    curr_x += w;
	}
	else {
	    x = 0;
	    y = button_ht;
	    w = tot_w;
	    h = tot_h - button_ht;
	}
	if (child == instig) {
	    child->core.x = x;
	    child->core.y = y;
	    child->core.width = w;
	    child->core.height = h;
	    child->core.border_width = bw;
	}
	else {
	    XtConfigureWidget(child, x, y, w, h, bw);
	}
    }
}

static XtGeometryResult
geometry_manager(Widget instig,
		 XtWidgetGeometry *request,
		 XtWidgetGeometry *reply)
{
    Widget rc = XtParent(instig);
    int ask, got;
    int n_exp;
    XtWidgetGeometry preferred;
    XtWidgetGeometry up_request, up_reply;
    XtGeometryResult result;
    KidGeom *boxes;
    int button_ht;

    *reply = *request;

    fprintf(stderr, "XmmmNotebook: geometry_manager: %p(%s) > %p(%s)\n",
	    rc, XtName(rc), instig, XtName(instig));

    if (request->request_mode & (CWX | CWY))
    {
	return XtGeometryNo;
    }

    /* See if we can resize */
    XmmmNotebookPreferredSizeAfterResize(rc, &preferred, instig, request, &boxes,
					  &button_ht);
    fprintf(stderr, "XmmmNotebook: geometry_manager: request (%d, %d), preferred (%d, %d)\n",
	    request->width, request->height, preferred.width, preferred.height);

    if (preferred.height != XtHeight(rc) || preferred.width != XtWidth(rc)) {
	up_request.request_mode = 0;
	if (preferred.height != XtHeight(rc)) {
	    up_request.request_mode |= CWHeight;
	    up_request.height = preferred.height;
	}
	if (preferred.width != XtWidth(rc)) {
	    up_request.request_mode |= CWWidth;
	    up_request.width = preferred.width;
	}
	if (request->request_mode & XtCWQueryOnly)
	    up_request.request_mode |= XtCWQueryOnly;
	result = XtMakeGeometryRequest(rc, &up_request, &up_reply);
    }
    else {
	result = XtGeometryYes;
	goto finished;
    }

    if (result == XtGeometryYes) {
	/* Our size has changed. */
	AdaptToSize(rc, instig, &preferred, boxes, button_ht);
    }
    else if (result == XtGeometryAlmost) {
	fprintf(stderr, "Cannot yet handle result (XtGeometryAlmost)\n");
    }
finished:
    XtFree((char *)boxes);
    return result;
}

static XtGeometryResult
try_to_resize(Widget w, XtWidgetGeometry *preferred, Boolean query_only)
{
    XtWidgetGeometry up_request, up_reply;
    if (preferred->height != XtHeight(w) || preferred->width != XtWidth(w)) {
	up_request.request_mode = 0;
	if (preferred->height != XtHeight(w)) {
	    up_request.request_mode |= CWHeight;
	    up_request.height = preferred->height;
	}
	if (preferred->width != XtWidth(w)) {
	    up_request.request_mode |= CWWidth;
	    up_request.width = preferred->width;
	}
	if (query_only)
	    up_request.request_mode |= XtCWQueryOnly;
	return XtMakeGeometryRequest(w, &up_request, &up_reply);
    }
    return XtGeometryYes;
}

static void
change_managed(Widget w)
{
    XtWidgetGeometry preferred;
    KidGeom *boxes;
    int button_ht;

    fprintf(stderr, "XmmmNotebook: change_managed: %p(%s)\n", w, XtName(w));

    XmmmNotebookPreferredSizeAfterResize(w, &preferred, NULL, NULL, &boxes,
					  &button_ht);
    try_to_resize(w, &preferred, False);
    AdaptToSize(w, NULL, &preferred, boxes, button_ht);

}

static void
insert_child(Widget w)
{
    XmmmNotebookWidget nb = (XmmmNotebookWidget)XtParent(w);
    XmmmNotebookPart *nbp = &nb->notebook;
    xmManagerClassRec.composite_class.insert_child(w);
    if (XmIsToggleButton(w)) {
	/* This is a control */
	if (nbp->num_buttons == nbp->max_buttons) {
	    nbp->max_buttons += 10;
	    nbp->buttons = (WidgetList)realloc(nbp->buttons,
					       nbp->max_buttons*sizeof(nbp->buttons[0]));
	}
	nbp->buttons[nbp->num_buttons++] = w;
    }
    else {
	/* This is a page */
	if (nbp->num_pages == nbp->max_pages) {
	    nbp->max_pages += 10;
	    nbp->pages = (WidgetList)realloc(nbp->pages,
					     nbp->max_pages*sizeof(nbp->pages[0]));
	}
	nbp->pages[nbp->num_pages++] = w;
    }
}

static void
delete_child(Widget w)
{
    Widget b2 = XtParent(w);
    int i;

    xmManagerClassRec.composite_class.delete_child(w);

#if 0
    for (i = 0; i < MGR_NumChildren(b2); i++)
    {
	RCC_PositionIndex(MGR_Children(b2)[i]) = i;
    }
#endif
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

/* FIXME: What happens if we resize (instig == 0) after a child has
   requested expansion? Presumably that child will request the
   same size again through XtQueryGeometry? */

/* Preferred size after resizing instig to instig_request */
/* Optionally returns number of expanding widgets and preferred
   geometry of children */
void
XmmmNotebookPreferredSizeAfterResize(Widget widget, XtWidgetGeometry *preferred,
				     Widget instig, XtWidgetGeometry *instig_request,
				     KidGeom **boxes, int *button_ht)
{
    XmmmNotebookWidget nb = (XmmmNotebookWidget)widget;
    XmmmNotebookPart *nbp = &nb->notebook;
    int tot_w1, max_h1, max_w2, max_h2;
    int i;
    int w, h;
    int wdef, hdef; /* defaults */
    XtWidgetGeometry child_geom, *geom;
    int n_expanding = 0;
    int count = 0;
    int tot_boxes;
    tot_w1 = 0;
    max_h1 = 0;
    tot_boxes = nbp->num_buttons + nbp->num_pages;
    if (boxes)
	*boxes = (KidGeom *)XtCalloc(tot_boxes+1, sizeof(KidGeom));
    for (i = 0; i < nbp->num_buttons; i++) {
	Widget child = nbp->buttons[i];
	if (child == instig) {
	    geom = instig_request;
	}
	else {
	    geom = &child_geom;
	    XtQueryGeometry(child, NULL, geom);
	}
	w = (geom->request_mode&CWWidth)?geom->width:XtWidth(child);
	h = (geom->request_mode&CWHeight)?geom->height:XtHeight(child);
	if (boxes) {
	    (*boxes)[count].w = child;
	    (*boxes)[count].box.width = w;
	    (*boxes)[count].box.height = h;
	    (*boxes)[count].type = 0;
	    count++;
	}
	tot_w1 += w;
	max_h1 = MAX(max_h1, h);
    }
    max_w2 = 0;
    max_h2 = 0;
    for (i = 0; i < nbp->num_pages; i++) {
	Widget child = nbp->pages[i];
	XmmmNotebookConstraints chc = (XmmmNotebookConstraints)child->core.constraints;
	if (child == instig) {
	    geom = instig_request;
	}
	else {
	    geom = &child_geom;
	    XtQueryGeometry(child, NULL, geom);
	}
	w = (geom->request_mode&CWWidth)?geom->width:XtWidth(child);
	h = (geom->request_mode&CWHeight)?geom->height:XtHeight(child);
	if (boxes) {
	    (*boxes)[count].w = child;
	    (*boxes)[count].box.width = w;
	    (*boxes)[count].box.height = h;
	    (*boxes)[count].type = 1;
	    count++;
	}
	max_w2 = MAX(max_w2, w);
	max_h2 = MAX(max_h2, h);
    }
    preferred->request_mode = (CWWidth|CWHeight);
    preferred->width = MAX(tot_w1, max_w2);
    preferred->height = max_h1 + max_h2;
    if (button_ht)
	*button_ht = max_h1;
}

static void
PreferredSize(Widget widget, XtWidgetGeometry *preferred)
{
    XmmmNotebookPreferredSizeAfterResize(widget, preferred, NULL, NULL, NULL, NULL);
}

Cardinal XmmmNotebookNumPages(Widget w)
{
    XmmmNotebookWidget nb = (XmmmNotebookWidget)w;
    return nb->notebook.num_buttons;
}

static void
ChangePanelCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    int pageno = (int)client_data;
    int i;
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;
    Boolean set = cbs->set;
    XmmmNotebookWidget nb = (XmmmNotebookWidget)XtParent(w);
    XmmmNotebookPart *nbp = &nb->notebook;

    nbp->current_pageno = pageno;

    if (set)
    {
	// Manage this pages
	// Unmanage all other pages
	for (i = 0; i < nbp->num_pages; i++)
	{
	    Widget child = nbp->pages[i];
	    if (i == pageno)
		XtManageChild(child);
	    else
		XtUnmanageChild(child);
	}
	for (i = 0; i < nbp->num_buttons; i++)
	{
	    Widget child = nbp->buttons[i];
	    if (i != pageno)
		XtVaSetValues(child, XmNset, False, 0);
	}
    }
}

/* child must already be a child of w */
void
XmmmNotebookInsertChild(Widget w, char *name, Widget child)
{
    XmmmNotebookWidget nb = (XmmmNotebookWidget)w;
    int npages = nb->notebook.num_buttons;
    Widget button;
    Arg args[10];
    int nargs;
    nargs = 0;
    button = XmCreateToggleButton(w, name, args, nargs);
    XtManageChild(button);
    XtAddCallback(button, XmNvalueChangedCallback, ChangePanelCB, 
		  (XtPointer)npages);
    if (npages == 0) {
	XtManageChild(child);
	XtVaSetValues(button, XmNset, True, 0);
    }
}

Widget
XmmmNotebookInsertPage(Widget w, char *name, Arg *arglist, Cardinal argcount)
{
    Widget child = XmCreateRowColumn(w, name, arglist, argcount);
    XmmmNotebookInsertChild(w, name, child);
    return child;
}

Widget XmmmNotebookCurrentPage(Widget w)
{
    XmmmNotebookWidget nb = (XmmmNotebookWidget)w;
    XmmmNotebookPart *nbp = &nb->notebook;
    int npages = nbp->num_buttons;
    int pageno = nbp->current_pageno;
    if (pageno >= 0 && pageno < npages)
	return nbp->pages[pageno];
    else
	return NULL;
}

