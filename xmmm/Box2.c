/* High-level GUI wrapper for Xm. */

/* Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
 * 
 * This file is part of Xmmm.
 * 
 * Xmmm is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 * 
 * Xmmm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with Xmmm -- see the file COPYING.
 * If not, see <http://www.gnu.org/licenses/>.

static const char rcsid[] = "$Header:$";

#include <stdio.h>

#include <Xmmm/Box2.h>
#include <Xmmm/Box2P.h>

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
    int pack_options;
    XtWidgetGeometry box;
} KidGeom;

extern void
XmmmBox2PreferredSizeAfterResize(Widget widget, XtWidgetGeometry *preferred,
				 Widget instig, XtWidgetGeometry *instig_request,
				 int *n_exp, KidGeom **);
static void AdaptToSize(Widget rc, Widget instig, XtWidgetGeometry *pref, KidGeom *boxes);



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



#define Offset(field) XtOffsetOf(XmmmBox2Rec, box_2.field)

/* Resources for the Box2 class */
static XtResource resources[] =
{
    {
	XmNorientation, XmCOrientation, XmROrientation,
	sizeof(unsigned char), Offset(orientation),
	XmRImmediate, (XtPointer)XmUNSPECIFIED
    },
    {
	XmNspacing, XmCSpacing, XmRHorizontalDimension,
	sizeof(Dimension), Offset(spacing),
	XmRImmediate, (XtPointer)XmINVALID_DIMENSION
    }
};

#undef Offset

#define Offset(field) XtOffsetOf(XmmmBox2ConstraintRec, box_2.field)

static XtResource box2ConstraintResources[] =
{
    {
	"pack_options", "Pack_Options", XmRCardinal,
	sizeof(short), Offset(pack_options),
	XmRImmediate, (XtPointer)0
    }
};

#undef Offset

XmmmBox2ClassRec xmmmBox2ClassRec =
{
    /* Core class part */
    {
	/* superclass            */ (WidgetClass) & xmManagerClassRec,
	/* class_name            */ "XmmmBox2",
	/* widget_size           */ sizeof(XmmmBox2Rec),
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
	/* subresources      */ box2ConstraintResources,
	/* subresource_count */ XtNumber(box2ConstraintResources),
	/* constraint_size   */ sizeof(XmmmBox2ConstraintRec),
	/* initialize        */ constraint_initialize,
	/* destroy           */ NULL,
	/* set_values        */ constraint_set_values,
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
    /* XmmmBox2 Area part */
    {
	/* mumble */ 0
    },
};


WidgetClass xmmmBox2WidgetClass = (WidgetClass)&xmmmBox2ClassRec;


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

#if 0
    if (XtWidth(new_w) == 0)
    {
	XtWidth(new_w) = 16;
    }
    if (XtHeight(new_w) == 0)
    {
	XtHeight(new_w) = 16;
    }
#endif
    if (MGR_ShadowThickness(new_w) == XmINVALID_DIMENSION)
    {
	MGR_ShadowThickness(new_w) = 0;
    }

    if (B2_Spacing(new_w) == XmINVALID_DIMENSION)
    {
	B2_Spacing(new_w) = 3;
    }


    if (B2_Orientation(new_w) == (unsigned char)XmUNSPECIFIED)
    {
	B2_Orientation(new_w) = XmVERTICAL;
    }

    B2_OldWidth(new_w) = XtWidth(new_w);
    B2_OldHeight(new_w) = XtHeight(new_w);
    B2_OldShadowThickness(new_w) = MGR_ShadowThickness(new_w);
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

    if (B2_Orientation(old) != B2_Orientation(new_w))
    {
	need_relayout = True;
	need_refresh = True;
    }

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
    fprintf(stderr, "Box2: resize %p(%s)\n", w, XtName(w));

    XmmmBox2PreferredSizeAfterResize(w, &preferred, NULL, NULL, &n_exp, &boxes);
    AdaptToSize(w, NULL, &preferred, boxes);

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
    XtWidgetGeometry geom;
    XtGeometryResult result;

    Dimension width, height;

    fprintf(stderr, "Box2: query_geometry: %p(%s)\n", w, XtName(w));

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
AdaptToSize(Widget rc, Widget instig, XtWidgetGeometry *pref, KidGeom *boxes)
{
    /* We kept the preferred child sizes around in a cache, boxes */
    /* The requested size was pref, this contains the total height and
       maximum width requested by the kids */
    /* We may have been resized to a larger size */
    /* Distribute extra among the expanding kids */
    int i;
    int n_exp = 0;
    int tot_w = XtWidth(rc);
    int tot_h = XtHeight(rc);
    int curr_y = 0;
    int h_extra;
    int x, y, w, h, bw, dy;
    for (i = 0; boxes[i].w; i++) {
	if (boxes[i].pack_options > 0) n_exp++;
    }
    if (n_exp > 0)
	h_extra = (tot_h - pref->height)/n_exp;
    for (i = 0; boxes[i].w; i++) {
	Widget child = boxes[i].w;
	bw = child->core.border_width;
	x = 0;
	y = curr_y;
	w = tot_w;
	if (boxes[i].pack_options == 2) {
	    h = dy = boxes[i].box.height + h_extra; /* EXPAND_WIDGET */
	}
	else if (boxes[i].pack_options == 1) {
	    h = boxes[i].box.height; /* EXPAND_PADDING */
	    dy = h + h_extra;
	    y += h_extra/2; /* EXPAND_PADDING */
	}
	else {
	    h = dy = boxes[i].box.height;
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
	curr_y += dy;
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

    *reply = *request;

    fprintf(stderr, "Box2: geometry_manager: %p(%s) > %p(%s)\n",
	    rc, XtName(rc), instig, XtName(instig));

    if (request->request_mode & (CWX | CWY))
    {
	return XtGeometryNo;
    }

    /* See if we can resize */
    XmmmBox2PreferredSizeAfterResize(rc, &preferred, instig, request, &n_exp, &boxes);
    fprintf(stderr, "Box2: geometry_manager: request (%d, %d), preferred (%d, %d)\n",
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
	AdaptToSize(rc, instig, &preferred, boxes);
    }
    else if (result == XtGeometryAlmost) {
	fprintf(stderr, "Cannot yet handle result (XtGeometryAlmost)\n");
    }
finished:
    XtFree((char *)boxes);
    return result;
}

static void
change_managed(Widget w)
{

    fprintf(stderr, "Box2: change_managed: %p(%s)\n", w, XtName(w));

    _XmmmB2AdjustSize(w, NULL, NULL);

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
    Widget b2 = (Widget)XtParent(w);
    int i;
    xmManagerClassRec.composite_class.insert_child(w);

#if 0
    for (i = 0; i < MGR_NumChildren(b2); i++)
    {
	RCC_PositionIndex(MGR_Children(b2)[i]) = i;
    }
#endif
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
XmmmCreateBox2(Widget parent, char *name,
	       Arg *arglist, Cardinal argcount)
{
    return XtCreateWidget(name, xmmmBox2WidgetClass, parent,
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
XmmmBox2PreferredSizeAfterResize(Widget widget, XtWidgetGeometry *preferred,
				 Widget instig, XtWidgetGeometry *instig_request,
				 int *n_exp, KidGeom **boxes)
{
    XmmmBox2Widget box = (XmmmBox2Widget)widget;
    int max_w, tot_h;
    int i;
    int w, h;
    int wdef, hdef; /* defaults */
    XtWidgetGeometry child_geom, *geom;
    int n_expanding = 0;
    int count = 0;
    max_w = 0;
    tot_h = 0;
    if (boxes)
	*boxes = (KidGeom *)XtCalloc(box->composite.num_children+1,
				     sizeof(KidGeom));
    for (i = 0; i < box->composite.num_children; i++) {
	Widget child = box->composite.children[i];
	if (XtIsManaged(child)) {
	    XmmmBox2Constraints chc = (XmmmBox2Constraints)child->core.constraints;
	    if (child == instig) {
		geom = instig_request;
	    }
	    else {
		geom = &child_geom;
		XtQueryGeometry(child, NULL, geom);
	    }
	    if (chc->box_2.pack_options > 0) {
		n_expanding++;
		/* FIXME: These defaults are never used?  Most
		   children seem to pass (CWWidth|CWHeight) back from
		   XtQueryGeometry. */
		if (instig) {
		    /* Do not allow the instigator to steal our height */
		    hdef = XtHeight(child);
		    wdef = XtWidth(child);
		}
		else {
		    hdef = MINIMUM_HEIGHT;
		    wdef = MINIMUM_WIDTH;
		}
	    }
	    else {
		hdef = XtHeight(child);
		wdef = XtWidth(child);
	    }
	    if ((geom->request_mode&(CWWidth|CWHeight)) != (CWWidth|CWHeight)) {
		fprintf(stderr, "Using some defaults\n");
	    }
	    w = (geom->request_mode&CWWidth)?geom->width:wdef;
	    h = (geom->request_mode&CWHeight)?geom->height:hdef;
	    if (boxes) {
		(*boxes)[count].w = child;
		(*boxes)[count].pack_options = chc->box_2.pack_options;
		(*boxes)[count].box.width = w;
		(*boxes)[count].box.height = h;
		count++;
	    }
	    max_w = MAX(max_w, w);
	    tot_h += h;
	}
    }
    preferred->request_mode = (CWWidth|CWHeight);
    preferred->width = max_w;
    preferred->height = tot_h;
    if (n_exp)
	*n_exp = n_expanding;
}

/* FIXME: External */
XtGeometryResult
_XmmmB2AdjustSize(Widget widget, Widget instig, XtWidgetGeometry *instig_request)
{
    XmmmBox2Widget box = (XmmmBox2Widget)widget;
    XtWidgetGeometry *geom;
    XtWidgetGeometry child_geom;
    XtWidgetGeometry preferred;
    int i;
    int max_w;
    int tot_h;
    int curr_y;
    int x, y, w, h, bw;
    int n_exp = 0;
    int h_extra;
    /* Widgets with PACK_NONE get their desired height.  Widgets with
     * PACK_EXPAND_PADDING or PACK_EXPAND_WIDGET get remaining space
     * divided between them. */
    /* Problem: We cannot read the preferred height from such widgets,
     * since after a resize their "preferred geometry" may be their
     * current geometry (e.g. scrolled lists).  We use a nominal
     * minimum size. */
    /* First compute desired total height, and count widgets which
     * have the "expand_" constraints set. */
    fprintf(stderr, "_XmmmB2AdjustSize: %p(%s)\n", widget, XtName(widget));
    XmmmBox2PreferredSizeAfterResize(widget, &preferred, instig, instig_request, &n_exp, NULL);
    /* FIXME the mask is unused; we know this returns width and height */
    /* If instig is set, we are being called from geometry_manager. */
    max_w = preferred.width;
    tot_h = preferred.height;
    if (n_exp > 0) {
	h_extra = (XtHeight(box) - tot_h)/n_exp;
    }
    if (h_extra < 0) {
	h_extra = 0;
    }
    curr_y = 0;
    for (i = 0; i < box->composite.num_children; i++) {
	Widget child = box->composite.children[i];
	if (XtIsManaged(child)) {
	    int conf_w, conf_h, conf_x, conf_y;
	    XmmmBox2Constraints chc = (XmmmBox2Constraints)child->core.constraints;
	    if (child == instig) {
		geom = instig_request;
	    }
	    else {
		geom = &child_geom;
		XtQueryGeometry(child, NULL, geom); /* FIXME: Cache results */
	    }
	    x = (geom->request_mode&CWX)?geom->x:child->core.x;
	    y = (geom->request_mode&CWY)?geom->y:child->core.y;
	    w = (geom->request_mode&CWWidth)?geom->width:XtWidth(child);
	    h = (geom->request_mode&CWHeight)?geom->height:XtHeight(child);
	    bw = (geom->request_mode&CWBorderWidth)?geom->border_width:child->core.border_width;
	    /* x, y, w, h, bw: geometry values either from instigator,
	       OR from preferred geometry of child, OR from current
	       values */
	    conf_w = w;
	    conf_h = h;
	    conf_x = 0;
	    conf_y = curr_y;
	    if (chc->box_2.pack_options == 2) {
		h = MINIMUM_HEIGHT + h_extra; /* EXPAND_WIDGET */
		conf_h = h;
		conf_w = XtWidth(box);
	    }
	    else if (chc->box_2.pack_options == 1) {
		h = MINIMUM_HEIGHT + h_extra; /* EXPAND_WIDGET */
		conf_y += h_extra/2; /* EXPAND_PADDING */
	    }
	    if (child == instig) {
		child->core.x = conf_x;
		child->core.y = conf_y;
	    }
	    else {
		XtConfigureWidget(child, conf_x, conf_y, conf_w, conf_h, bw);
	    }
	    curr_y += h;
	}
    }
    return XtGeometryYes;
}

static void
PreferredSize(Widget widget, XtWidgetGeometry *preferred)
{
    XmmmBox2PreferredSizeAfterResize(widget, preferred, NULL, NULL, NULL, NULL);
}

