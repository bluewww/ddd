/* $Id$ */
/*
 * Copyright 1998 John L. Cwikla
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appears in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of John L. Cwikla or
 * Wolfram Research, Inc not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written
 * prior permission.  John L. Cwikla and Wolfram Research, Inc make no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * John L. Cwikla and Wolfram Research, Inc disclaim all warranties with
 * regard to this software, including all implied warranties of
 * merchantability and fitness, in no event shall John L. Cwikla or
 * Wolfram Research, Inc be liable for any special, indirect or
 * consequential damages or any damages whatsoever resulting from loss of
 * use, data or profits, whether in an action of contract, negligence or
 * other tortious action, arising out of or in connection with the use or
 * performance of this software.
 *
 * Author:
 *  John L. Cwikla
 *  X Programmer
 *  Wolfram Research Inc.
 *
 *  cwikla@wolfram.com
*/

#define new a_new		// Motif 1.1 wants this
#define class a_class
extern "C" {
#include <Xm/XmP.h>
#include <Xm/LabelP.h>
#include <Xm/LabelGP.h>

#if XmVersion >= 1002
#include <Xm/ManagerP.h>
#endif
}
#undef new
#undef class

#include "LabelHP.h"
#include "casts.h"

static void initialize(XmLabelHackWidget _request, 
		       XmLabelHackWidget _new, 
		       String *_args, Cardinal *_numArg);
static Boolean set_values(Widget old, Widget, Widget new_w,
			  ArgList, Cardinal *);
static void _replaceLabelExpose(void);
static void _replaceLabelGadgetExpose(void);
static GC _topShadowGC(XmLabelWidget _label);
static GC _bottomShadowGC(XmLabelWidget _label);


#define CORE(w) ((w)->core)
#define PRIM(w) ((w)->primitive)
#define MANAGER(w) ((w)->manager)
#define LABEL(w) ((w)->label)
#define HACK(w) ((w)->label_hack)
#define HACK_CLASS(w) ((w)->label_hack_class)

#define TheOffset(field) XtOffset(XmLabelHackWidget, label_hack.field)
//#define PrimOffset(field) XtOffset(XmLabelHackWidget, primitive.field)
static XtResource resources[] =
{
    {XTRESSTR(XmNinsensitive3D), XTRESSTR(XmCInsensitive3D), XmRBoolean, sizeof(Boolean),
     TheOffset(insensitive3D), XmRImmediate, (XtPointer)TRUE},
};

#if XmVersion < 1002
// Motif 1.1 backwards compatibility
#ifndef XmInheritBorderHighlight
#define XmInheritBorderHighlight (XtWidgetProc)_XtInherit
#endif

#ifndef XmInheritBorderUnhighlight
#define XmInheritBorderUnhighlight (XtWidgetProc)_XtInherit
#endif

#ifndef XmInheritArmAndActivate
#define XmInheritArmAndActivate (XmArmAndActivate)_XtInherit
#endif

#ifndef XmInheritSetOverrideCallback
#define	XmInheritSetOverrideCallback (XtWidgetProc) _XtInherit
#endif

#ifndef XmInheritMenuProc
#define XmInheritMenuProc (XmMenuProc) _XtInherit
#endif
#endif // XmVersion < 1002


static XmLabelHackClassRec xmLabelHackClassRec =
{
    {
	(WidgetClass)&xmLabelClassRec,		/* superclass */
	CONST_CAST(char *,"XmLabelHack"),				/* class_name */
	(Cardinal)sizeof(XmLabelHackRec),	/* widget size */
	XtProc(0),				/* class_init */
	XtWidgetClassProc(0),		        /* class_part_init */
	(XtEnum)FALSE,				/* class_inited */
	(XtInitProc)initialize,			/* initialize */
	XtArgsProc(0),			        /* init_hook */
	XtInheritRealize,			/* realize */
	XtActionList(0),			/* actions */
	(Cardinal)0,				/* num_actions */
	(XtResourceList)resources,		/* resources */
	(Cardinal)XtNumber(resources),		/* num_resources */
	NULLQUARK,				/* xrm_class */
	TRUE,					/* compress_motion */
	(XtEnum)TRUE,				/* compress_exposur */
	TRUE,					/* compress enterleave */
	FALSE,					/* visibility_interest */
	XtWidgetProc(0),			/* destroy */
	(XtWidgetProc)XtInheritResize,
	XtExposeProc(0),
	(XtSetValuesFunc)set_values,            /* set_values */
	XtArgsFunc(0),		                /* set_values_hook */
	XtInheritSetValuesAlmost,		/* set_values_almost */
	XtArgsProc(0),			        /* get_values_hook */
	XtAcceptFocusProc(0),		        /* accept_focus */
	XtVersion,				/* version */
	XtPointer(0),			        /* callback_private */
	XtInheritTranslations,			/* tm_table */
	XtGeometryHandler(0), 	                /* query_geometry */
	XtStringProc(0), 			/* display_accelerator */
	XtPointer(0)			        /* extension */
    },
    { 	/* xmPrimitiveClass */
	XmInheritBorderHighlight,     // border_highlight
	XmInheritBorderUnhighlight,   // border_unhighlight
	XtInheritTranslations,        // translations
	XmInheritArmAndActivate,      // arm_and_activate
	0,			      // syn_resources
	0,                            // num_syn_resources
	0                             // extension
#if defined(__sgi) && !defined(LesstifVersion)
	// Paul Sydney <sydney@ulua.mhpcc.af.mil> reports that OSF/Motif
	// on an SGI Indy running IRIX 6.5 has an extra
	// `_SG_vendorExtension' field.  If this is not initialized
	// explicitly, then EGCS 1.1 gives a warning.
	, 0
#endif
    },
    {	/* xmLabelClass */
	XmInheritSetOverrideCallback, // setOverrideCallback
	XmInheritMenuProc,            // menuProcs
	XtInheritTranslations,        // translations
	0                             // extension
#if defined(__sgi) && !defined(LesstifVersion)
	, 0
#endif
    },
    {
	-1			      // on
    }
};

WidgetClass xmLabelHackWidgetClass = (WidgetClass)&xmLabelHackClassRec;

static XtExposeProc oldLabelGadgetExposeProc = 0;
static XtExposeProc oldLabelWidgetExposeProc = 0;

static void enable3d(XmLabelHackWidget _new)
{
    if (HACK(_new).insensitive3D && CORE(_new).depth > 4)
    {
	if (oldLabelWidgetExposeProc == 0)
	    _replaceLabelExpose();

	if (oldLabelGadgetExposeProc == 0)
	    _replaceLabelGadgetExpose();

	// Keep around so there is one in the cache
	HACK(_new).topGC    = _topShadowGC((XmLabelWidget)_new);
	HACK(_new).bottomGC = _bottomShadowGC((XmLabelWidget)_new);
    }
}

static void disable3d()
{
    // Restore old functions
    xmLabelWidgetClass->core_class.expose = oldLabelWidgetExposeProc;
    xmLabelGadgetClass->core_class.expose = oldLabelGadgetExposeProc;

    oldLabelWidgetExposeProc = 0;
    oldLabelGadgetExposeProc = 0;
}

static void initialize(XmLabelHackWidget, XmLabelHackWidget _new, 
		       String *, Cardinal *)
{
    XmLabelHackWidgetClass lhwc;

    lhwc = (XmLabelHackWidgetClass)xmLabelHackWidgetClass;

    XtInitializeWidgetClass(xmLabelGadgetClass);

    if (HACK_CLASS(lhwc).on == -1)
    {
	enable3d(_new);
	HACK_CLASS(lhwc).on = HACK(_new).insensitive3D;
    }

    if ((CORE(_new).width == 0) || (CORE(_new).height == 0))
    {
	CORE(_new).width = 1;
	CORE(_new).height = 1;
    }
}


static Boolean set_values(Widget old, Widget, Widget _new,
			  ArgList, Cardinal *)
{
    XmLabelHackWidget before = XmLabelHackWidget(old);
    XmLabelHackWidget after  = XmLabelHackWidget(_new);

    if (HACK(before).insensitive3D != HACK(after).insensitive3D)
    {
	if (HACK(after).insensitive3D)
	    enable3d(after);
	else
	    disable3d();
    }

    return False;		// No need to redisplay
}

#if defined(LesstifVersion)
static void noResize(Widget) {}
#endif

static GC _topShadowGC(XmLabelWidget _label)
{
    XGCValues gcValues;
    XtGCMask	mask;
    XFontStruct *fs;

    mask = GCForeground | GCBackground;
    gcValues.foreground = PRIM(_label).top_shadow_color;
    gcValues.background = PRIM(_label).foreground;

    if ((PRIM(_label).top_shadow_pixmap != None) &&
	(PRIM(_label).top_shadow_pixmap != XmUNSPECIFIED_PIXMAP))
    {
	mask |= GCFillStyle | GCTile;
	gcValues.fill_style = FillTiled;
	gcValues.tile = PRIM(_label).top_shadow_pixmap;
    }

    _XmFontListGetDefaultFont(LABEL(_label).font, &fs);
    if (fs != 0)
    {
	mask |= GCFont;
	gcValues.font = fs->fid;
    }

    return XtGetGC ((Widget)_label,  mask, &gcValues);
}

static GC _bottomShadowGC(XmLabelWidget _label)
{
    XGCValues gcValues;
    XtGCMask	mask;
    XFontStruct *fs;

    mask = GCForeground | GCBackground;
    gcValues.foreground = PRIM(_label).bottom_shadow_color;
    gcValues.background = PRIM(_label).foreground;

    if ((PRIM(_label).bottom_shadow_pixmap != None) &&
	(PRIM(_label).bottom_shadow_pixmap != XmUNSPECIFIED_PIXMAP))
    {
	mask |= GCFillStyle | GCTile;
	gcValues.fill_style = FillTiled;
	gcValues.tile = PRIM(_label).bottom_shadow_pixmap;
    }

    _XmFontListGetDefaultFont(LABEL(_label).font, &fs);
    if (fs != 0)
    {
	mask |= GCFont;
	gcValues.font = fs->fid;
    }

    return XtGetGC ((Widget)_label,  mask, &gcValues);
}

// arnaud.desitter@nag.co.uk says `extern "C"' is required to fix
// warnings on Sun CC 5.x, which enforces a standard C++ rule where
// pointer to function are extern "C" or extern "C++" (see Sun C++
// migration guide).
extern "C" {
    static void _grabbedLabelExpose(Widget _w, XEvent *_event, Region _region)
    {
	GC insensitiveGC;

	XmLabelWidget label = (XmLabelWidget)_w;

	unsigned char label_type = XmSTRING;
	XtVaGetValues(_w, XmNlabelType, &label_type, XtPointer(0));

	if (XtIsSensitive(_w) || label_type != XmSTRING)
	    (*oldLabelWidgetExposeProc)(_w, _event, _region);
	else
	{
#if defined(LesstifVersion)
	    // LessTif invokes resize() when exposing the widget,
	    // creating a new insensitive GC.  Inhibit this.
	    XtWidgetProc oldResizeProc = XtClass(_w)->core_class.resize;
	    XtClass(_w)->core_class.resize = noResize;
	    oldResizeProc(_w);
#endif

	    insensitiveGC = LABEL(label).insensitive_GC;

	    LABEL(label).TextRect.x += 1;
	    LABEL(label).TextRect.y += 1;
	    if (LABEL(label)._acc_text != 0)
	    {
		LABEL(label).acc_TextRect.x += 1;
		LABEL(label).acc_TextRect.y += 1;
	    }
	    LABEL(label).insensitive_GC = _topShadowGC(label);
	    (*oldLabelWidgetExposeProc)(_w, _event, _region);
	    XtReleaseGC(_w, LABEL(label).insensitive_GC);

	    LABEL(label).TextRect.x -= 1;
	    LABEL(label).TextRect.y -= 1;
	    if (LABEL(label)._acc_text != 0)
	    {
		LABEL(label).acc_TextRect.x -= 1;
		LABEL(label).acc_TextRect.y -= 1;
	    }
	    LABEL(label).insensitive_GC = _bottomShadowGC(label);
	    (*oldLabelWidgetExposeProc)(_w, _event, _region);
	    XtReleaseGC(_w, LABEL(label).insensitive_GC);

	    LABEL(label).insensitive_GC = insensitiveGC;

#if defined(LesstifVersion)
	    XtClass(_w)->core_class.resize = oldResizeProc;
#endif
	}
    }
}

static void _replaceLabelExpose(void)
{
    oldLabelWidgetExposeProc = xmLabelWidgetClass->core_class.expose;
    xmLabelWidgetClass->core_class.expose = _grabbedLabelExpose;
}

static GC _gadgetParentTopShadowGC(XmLabelGadget _label)
{
    XGCValues gcValues;
    XtGCMask	mask;
    XFontStruct *fs;
    XmManagerWidget parent = (XmManagerWidget)XtParent(_label);

    mask = GCForeground | GCBackground;
    gcValues.foreground = MANAGER(parent).top_shadow_color;
    gcValues.background = MANAGER(parent).foreground;

    if ((MANAGER(parent).top_shadow_pixmap != None) &&
	(MANAGER(parent).top_shadow_pixmap != XmUNSPECIFIED_PIXMAP))
    {
	mask |= GCFillStyle | GCTile;
	gcValues.fill_style = FillTiled;
	gcValues.tile = MANAGER(parent).top_shadow_pixmap;
    }

    _XmFontListGetDefaultFont(LABEL(_label).font, &fs);
    if (fs != 0)
    {
	mask |= GCFont;
	gcValues.font = fs->fid;
    }

    return XtGetGC ((Widget)parent,  mask, &gcValues);
}

static GC _gadgetParentBottomShadowGC(XmLabelGadget _label)
{
    XGCValues gcValues;
    XtGCMask	mask;
    XFontStruct *fs;
    XmManagerWidget parent = (XmManagerWidget)XtParent(_label);

    mask = GCForeground | GCBackground;
    gcValues.foreground = MANAGER(parent).bottom_shadow_color;
    gcValues.background = MANAGER(parent).foreground;

    if ((MANAGER(parent).bottom_shadow_pixmap != None) &&
	(MANAGER(parent).bottom_shadow_pixmap != XmUNSPECIFIED_PIXMAP))
    {
	mask |= GCFillStyle | GCTile;
	gcValues.fill_style = FillTiled;
	gcValues.tile = MANAGER(parent).bottom_shadow_pixmap;
    }

    _XmFontListGetDefaultFont(LABEL(_label).font, &fs);
    if (fs != 0)
    {
	mask |= GCFont;
	gcValues.font = fs->fid;
    }

    return XtGetGC ((Widget)parent,  mask, &gcValues);
}

extern "C" {
    static void _grabbedLabelGadgetExpose(Widget _w, XEvent *_event, 
					  Region _region)
    {
	GC insensitiveGC;

	XmLabelGadget label = (XmLabelGadget)_w;

	unsigned char label_type = XmSTRING;
	XtVaGetValues(_w, XmNlabelType, &label_type, XtPointer(0));

	if (XtIsSensitive(_w) || label_type != XmSTRING)
	    (*oldLabelGadgetExposeProc)(_w, _event, _region);
	else
	{
#if defined(LesstifVersion)
	    // LessTif invokes resize() when exposing the widget,
	    // creating a new insensitive GC.  Inhibit this.
	    XtWidgetProc oldResizeProc = XtClass(_w)->core_class.resize;
	    XtClass(_w)->core_class.resize = noResize;
	    oldResizeProc(_w);
#endif

	    insensitiveGC = LABEL(label).insensitive_GC;

	    LABEL(label).TextRect.x += 1;
	    LABEL(label).TextRect.y += 1;
	    if (LABEL(label)._acc_text != 0)
	    {
		LABEL(label).acc_TextRect.x += 1;
		LABEL(label).acc_TextRect.y += 1;
	    }
	    LABEL(label).insensitive_GC = _gadgetParentTopShadowGC(label);
	    (*oldLabelGadgetExposeProc)(_w, _event, _region);
	    XtReleaseGC(_w, LABEL(label).insensitive_GC);

	    LABEL(label).TextRect.x -= 1;
	    LABEL(label).TextRect.y -= 1;
	    if (LABEL(label)._acc_text != 0)
	    {
		LABEL(label).acc_TextRect.x -= 1;
		LABEL(label).acc_TextRect.y -= 1;
	    }
	    LABEL(label).insensitive_GC = _gadgetParentBottomShadowGC(label);
	    (*oldLabelGadgetExposeProc)(_w, _event, _region);
	    XtReleaseGC(_w, LABEL(label).insensitive_GC);

	    LABEL(label).insensitive_GC = insensitiveGC;

#if defined(LesstifVersion)
	    XtClass(_w)->core_class.resize = oldResizeProc;
#endif
	}
    }
}

static void _replaceLabelGadgetExpose(void)
{
    oldLabelGadgetExposeProc = xmLabelGadgetClass->core_class.expose;
    xmLabelGadgetClass->core_class.expose = _grabbedLabelGadgetExpose;
}
