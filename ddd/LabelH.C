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


#include <Xm/XmP.h>
#include <Xm/LabelP.h>
#include <Xm/LabelGP.h>
#include <Xm/ManagerP.h>

#include "LabelHP.h"

static void initialize(XmLabelHackWidget _request, XmLabelHackWidget _new, String *_args, Cardinal *_numArg);
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
#define PrimOffset(field) XtOffset(XmLabelHackWidget, primitive.field)
static XtResource resources[] =
{
    {XmNinsensitive3D, XmCInsensitive3D, XmRBoolean, sizeof(Boolean),
     TheOffset(insensitive3D), XmRImmediate, (XtPointer)TRUE},
};


XmLabelHackClassRec xmLabelHackClassRec =
{
    {
	(WidgetClass)&xmLabelClassRec,		/* superclass */
	"XmLabelHack",				/* class_name */
	(Cardinal)sizeof(XmLabelHackRec),	/* widget size */
	(XtProc)NULL,				/* class_init */
	(XtWidgetClassProc)NULL,		/* class_part_init */
	(XtEnum)FALSE,				/* class_inited */
	(XtInitProc)initialize,			/* initialize */
	(XtArgsProc)NULL,			/* init_hook */
	XtInheritRealize,			/* realize */
	(XtActionList)NULL,			/* actions */
	(Cardinal)0,				/* num_actions */
	(XtResourceList)resources,		/* resources */
	(Cardinal)XtNumber(resources),		/* num_resources */
	NULLQUARK,				/* xrm_class */
	TRUE,					/* compress_motion */
	(XtEnum)TRUE,				/* compress_exposur */
	TRUE,					/* compress enterleave */
	FALSE,					/* visibility_interest */
	(XtWidgetProc)NULL,			/* destroy */
	(XtWidgetProc)XtInheritResize,
	(XtExposeProc)NULL,
	(XtSetValuesFunc)NULL,			/* set_values */
	(XtArgsFunc)NULL,			/* set_values_hook */
	XtInheritSetValuesAlmost,		/* set_values_almost */
	(XtArgsProc)NULL,			/* get_values_hook */
	NULL,					/* accept_focus */
	XtVersion,				/* version */
	(XtPointer)NULL,			/* callback_private */
	XtInheritTranslations,			/* tm_table */
	(XtGeometryHandler)NULL, 	        /* query_geometry */
	NULL, 					/* display_accelerator */
	(XtPointer)NULL,			/* extension */
    },
    { 	/* xmPrimitiveClass */
	(XtWidgetProc)XmInheritBorderHighlight,
	(XtWidgetProc)XmInheritBorderUnhighlight,
	XtInheritTranslations,
	(XtActionProc)XmInheritArmAndActivate,
	NULL,
	0,
	(XtPointer)NULL,
    },
    {	/* xmLabelClass */
	(XtWidgetProc) XmInheritSetOverrideCallback,
	XmInheritMenuProc,
	XtInheritTranslations,
	NULL,
    },
    {
	-1,
    }
};

WidgetClass xmLabelHackWidgetClass = (WidgetClass)&xmLabelHackClassRec;

static void initialize(XmLabelHackWidget, XmLabelHackWidget _new, 
		       String *, Cardinal *)
{
    XmLabelHackWidgetClass lhwc;

    lhwc = (XmLabelHackWidgetClass)xmLabelHackWidgetClass;

    XtInitializeWidgetClass(xmLabelGadgetClass);

    if (HACK_CLASS(lhwc).on == -1)
    {
	if (HACK(_new).insensitive3D && CORE(_new).depth > 4)
	{
	    _replaceLabelExpose();
	    _replaceLabelGadgetExpose();

	    /* keep around so there is one in the cache */
	    HACK(_new).topGC = _topShadowGC((XmLabelWidget)_new);
	    HACK(_new).bottomGC = _bottomShadowGC((XmLabelWidget)_new);
	}

	HACK_CLASS(lhwc).on = HACK(_new).insensitive3D;
			
    }

    if ((CORE(_new).width == 0) || (CORE(_new).height == 0))
    {
	CORE(_new).width = 1;
	CORE(_new).height = 1;
    }
}

#if defined(LesstifVersion)
static void noResize(Widget) {}
#endif

static XtExposeProc oldLabelExposeProc;

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
    if (fs != NULL)
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
    if (fs != NULL)
    {
	mask |= GCFont;
	gcValues.font = fs->fid;
    }

    return XtGetGC ((Widget)_label,  mask, &gcValues);
}

static void _grabbedLabelExpose(Widget _w, XEvent *_event, Region _region)
{
    GC insensitiveGC;

    XmLabelWidget label = (XmLabelWidget)_w;

    unsigned char label_type = XmSTRING;
    XtVaGetValues(_w, XmNlabelType, &label_type, NULL);

    if (XtIsSensitive(_w) || label_type != XmSTRING)
	(*oldLabelExposeProc)(_w, _event, _region);
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
	if (LABEL(label)._acc_text != NULL)
	{
	    LABEL(label).acc_TextRect.x += 1;
	    LABEL(label).acc_TextRect.y += 1;
	}
	LABEL(label).insensitive_GC = _topShadowGC(label);
	(*oldLabelExposeProc)(_w, _event, _region);
	XtReleaseGC(_w, LABEL(label).insensitive_GC);

	LABEL(label).TextRect.x -= 1;
	LABEL(label).TextRect.y -= 1;
	if (LABEL(label)._acc_text != NULL)
	{
	    LABEL(label).acc_TextRect.x -= 1;
	    LABEL(label).acc_TextRect.y -= 1;
	}
	LABEL(label).insensitive_GC = _bottomShadowGC(label);
	(*oldLabelExposeProc)(_w, _event, _region);
	XtReleaseGC(_w, LABEL(label).insensitive_GC);

	LABEL(label).insensitive_GC = insensitiveGC;

#if defined(LesstifVersion)
	XtClass(_w)->core_class.resize = oldResizeProc;
#endif
    }
}

static void _replaceLabelExpose(void)
{
    oldLabelExposeProc = xmLabelWidgetClass->core_class.expose;
    xmLabelWidgetClass->core_class.expose = _grabbedLabelExpose;
}


static XtExposeProc oldLabelGadgetExposeProc;

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
    if (fs != NULL)
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
    if (fs != NULL)
    {
	mask |= GCFont;
	gcValues.font = fs->fid;
    }

    return XtGetGC ((Widget)parent,  mask, &gcValues);
}

static void _grabbedLabelGadgetExpose(Widget _w, XEvent *_event, 
				      Region _region)
{
    GC insensitiveGC;

    XmLabelGadget label = (XmLabelGadget)_w;

    unsigned char label_type = XmSTRING;
    XtVaGetValues(_w, XmNlabelType, &label_type, NULL);

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
	if (LABEL(label)._acc_text != NULL)
	{
	    LABEL(label).acc_TextRect.x += 1;
	    LABEL(label).acc_TextRect.y += 1;
	}
	LABEL(label).insensitive_GC = _gadgetParentTopShadowGC(label);
	(*oldLabelGadgetExposeProc)(_w, _event, _region);
	XtReleaseGC(_w, LABEL(label).insensitive_GC);

	LABEL(label).TextRect.x -= 1;
	LABEL(label).TextRect.y -= 1;
	if (LABEL(label)._acc_text != NULL)
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

static void _replaceLabelGadgetExpose(void)
{
    oldLabelGadgetExposeProc = xmLabelGadgetClass->core_class.expose;
    xmLabelGadgetClass->core_class.expose = _grabbedLabelGadgetExpose;
}
