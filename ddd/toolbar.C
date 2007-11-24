// $Id$ -*- C++ -*-
// Create toolbar

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char toolbar_rcsid[] = 
    "$Id$";

#include "toolbar.h"

#include "ddd.h"
#include "logo.h"
#include "misc.h"
#include "verify.h"
#include "HelpCB.h"
#include "Delay.h"
#include "AppData.h"
#ifdef IF_MOTIF
#include "converters.h"
#endif // IF_MOTIF

#ifdef IF_MOTIF
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/PanedW.h>
#endif // IF_MOTIF


//-----------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------

#ifdef IF_MOTIF
// Return the preferred height of W
static Dimension preferred_height(Widget w)
{
    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(w, (XtWidgetGeometry *)0, &size);
    Dimension real_height;
    XtVaGetValues(w, XmNheight, &real_height, XtPointer(0));
    return max(size.height, real_height);
}
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning toolbar preferred_height not implemented
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
static void set_label_type(MMDesc items[], unsigned char label_type)
{
    for (MMDesc *item = items; item != 0 && item->name != 0; item++)
    {
	Widget w = item->widget;
	if (w != 0 && XmIsLabel(w))
	{
	    XtVaSetValues(w, XmNlabelType, label_type, XtPointer(0));

	    if (label_type != XmSTRING)
		continue;
	    
	    // In OSF/Motif, setting both labelString and labelPixmap
	    // causes the button to use the Pixmap extent and to
	    // ignore the labelString extent, even if its labelType is
	    // XmSTRING.  Hence, we set the size explicitly instead.
	    //
	    // (This only happens in OSF/Motif; LessTif works fine.)
	    unsigned char label_type;
	    XmString label_string;
	    XmFontList font_list;
	    Dimension border_width, shadow_thickness;
	    Dimension margin_bottom, margin_top, margin_left, margin_right;
	    Dimension margin_height, margin_width;
	    XtVaGetValues(w,
			  XmNlabelType,       &label_type,
			  XmNlabelString,     &label_string,
			  XmNfontList,        &font_list,
			  XmNborderWidth,     &border_width,
			  XmNshadowThickness, &shadow_thickness,
			  XmNmarginBottom,    &margin_bottom,
			  XmNmarginTop,       &margin_top,
			  XmNmarginLeft,      &margin_left,
			  XmNmarginRight,     &margin_right,
			  XmNmarginHeight,    &margin_height,
			  XmNmarginWidth,     &margin_width,
			  XtPointer(0));

	    Dimension label_width, label_height;
	    XmStringExtent(font_list, label_string, 
			   &label_width, &label_height);

	    Dimension width =
		border_width * 2 + shadow_thickness * 2 + 
		margin_width * 2 + 
		margin_left + label_width + margin_right;

	    Dimension height =
		border_width * 2 + shadow_thickness * 2 + 
		margin_height * 2 + 
		margin_top + label_height + margin_bottom;

	    XtVaSetValues(w,
			  XmNrecomputeSize, False,
			  XmNwidth,  width + 4,
			  XmNheight, height + 4,
			  XtPointer(0));

	    XmStringFree(label_string);
	}
    }
}
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning set_label_type not supported.
#endif
#endif // IF_MOTIF

static void flatten_buttons(MMDesc items[])
{
    // Replace all `MMPush' by `MMFlatPush'
    for (MMDesc *item = items; item != 0 && item->name != 0; item++)
    {
	if ((item->type & MMTypeMask) == MMPush)
	    item->type = (MMFlatPush | (item->type & ~MMTypeMask));
    }
}

#ifdef IF_MOTIF
static Widget align_buttons(const MMDesc *items1, const MMDesc *items2)
{
    Widget last_button  = 0;

    for (int j = 1; j >= 0; j--)
    {
	const MMDesc *items = (j == 0 ? items1 : items2);
	if (items == 0)
	    continue;

	int i = -1;
	while (items[i + 1].widget != 0)
	    i++;
	for (; i >= 0; i--)
	{
	    Widget w = items[i].widget;

	    if (!XtIsManaged(w))
		continue;

	    Arg args[10];
	    Cardinal arg = 0;

	    XtSetArg(args[arg], XmNresizable,        False); arg++;
	    XtSetArg(args[arg], XmNtopAttachment,    XmATTACH_FORM); arg++;
	    XtSetArg(args[arg], XmNbottomAttachment, XmATTACH_FORM); arg++;

	    if (last_button == 0)
	    {
		XtSetArg(args[arg], XmNrightAttachment, XmATTACH_FORM); arg++;
	    }
	    else
	    {
		XtSetArg(args[arg],
			 XmNrightAttachment, XmATTACH_WIDGET); arg++;
		XtSetArg(args[arg],
			 XmNrightWidget,     last_button); arg++;
	    }

	    XtSetValues(w, args, arg);

	    last_button = w;
	}
    }

    return last_button;
}
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning align_buttons not implemented.
#endif
#endif // IF_MOTIF


#ifdef IF_MOTIF
static void ResetLabelEH(Widget w, XtPointer, XEvent *, Boolean *)
{
    XmString label_string;
    XtVaGetValues(w, XmNlabelString, &label_string, XtPointer(0));

    MString empty("");

    XtVaSetValues(w, XmNlabelString, empty.xmstring(), XtPointer(0));
    XtVaSetValues(w, XmNlabelString, label_string, XtPointer(0));
    XmStringFree(label_string);

    XtRemoveEventHandler(w, ExposureMask, False, 
			 ResetLabelEH, XtPointer(0));
}
#endif // IF_MOTIF

#ifdef IF_MOTIF
static void center_buttons(const MMDesc items[], Dimension offset)
{
    for (const MMDesc *item = items; item != 0 && item->name != 0; item++)
    {
	Widget w = item->widget;
	if (w == 0)
	    continue;

	XtVaSetValues(w,
		      XmNtopOffset, offset / 2,
		      XmNbottomOffset, (offset + 1) / 2,
		      XtPointer(0));

	// In OSF/Motif, setting both labelString and labelPixmap
	// causes the button to ignore the labelString extent, giving
	// a bad alignment of the labelString.  This goes away as soon
	// as the label string is reset.  Hence, reset it upon creation.
	XtAddEventHandler(w, ExposureMask, False, 
			  ResetLabelEH, XtPointer(0));
    }
}
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning center_buttons not supported.
#endif
#endif // IF_MOTIF

//-----------------------------------------------------------------------
// Toolbar creation
//-----------------------------------------------------------------------

// Create a toolbar as child of parent, named `toolbar', having
// the buttons ITEMS.  Return LABEL and ARGFIELD.
CONTAINER_P create_toolbar(CONTAINER_P parent, const string& /* name */,
			   MMDesc *items1, MMDesc *items2,
			   BUTTON_P &label, ArgField*& argfield,
			   unsigned char label_type)
{
    assert(label_type == XmPIXMAP || label_type == XmSTRING);

#ifdef IF_MOTIF
    Arg args[10];
    Cardinal arg = 0;
#endif // IF_MOTIF

    // Create toolbar
    const string toolbar_name = "toolbar";

#ifdef IF_MOTIF
    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,        0); arg++;
    XtSetArg(args[arg], XmNmarginHeight,       0); arg++;
    XtSetArg(args[arg], XmNborderWidth,        0); arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0); arg++;
    TOOLBAR_P toolbar = verify(XmCreateForm(parent, XMST(toolbar_name.chars()), args, arg));
#else // NOT IF_MOTIF
    TOOLBAR_P toolbar = new Gtk::Toolbar();
    toolbar->set_name(XMST(toolbar_name.chars()));
    Gtk::Box *box = dynamic_cast<Gtk::Box *>(parent);
    if (box) {
	box->pack_start(*toolbar, Gtk::PACK_SHRINK);
    }
    else {
	parent->add(*toolbar);
    }
#endif // IF_MOTIF

    // Create `():'
    label = create_arg_label(toolbar);

    // Create argument field
    const string argfield_name = "arg";
    argfield = new ArgField (toolbar, argfield_name.chars());
    Widget combobox = argfield->top();

#ifdef IF_MOTIF
    registerOwnConverters();
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning registerOwnConverters() not implemented.
#endif
#endif // IF_MOTIF

    // We install the icons AFTER having created the argument field,
    // because otherwise we might eat up all colors.
    install_icons(toolbar,
		  app_data.button_color_key,
		  app_data.active_button_color_key);

    if (label_type == XmPIXMAP && app_data.flat_toolbar_buttons)
    {
	// Use flat buttons
	flatten_buttons(items1);
	flatten_buttons(items2);
    }

    // Create buttons
    MMaddItems(toolbar,
#if !defined(IF_XM)
	       NULL,
#endif
	       items1);
    MMaddCallbacks(items1);
#ifdef IF_MOTIF
    MMaddHelpCallback(items1, ImmediateHelpCB);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning ImmediateHelpCB not implemented
#endif
#endif // IF_MOTIF
#ifdef IF_MOTIF
    set_label_type(items1, label_type);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning set_label_type not implemented
#endif
#endif // IF_MOTIF

    if (items2 != 0)
    {
	MMaddItems(toolbar,
#if !defined(IF_XM)
		   NULL,
#endif
		   items2);
	MMaddCallbacks(items2);
#ifdef IF_MOTIF
	MMaddHelpCallback(items2, ImmediateHelpCB);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning ImmediateHelpCB not implemented
#endif
#endif // IF_MOTIF
#ifdef IF_MOTIF
	set_label_type(items2, label_type);
#endif // IF_MOTIF
    }

#ifdef IF_MOTIF
    Widget first_button = align_buttons(items1, items2);
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning align_buttons not implemented
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
    // Set form constraints
    XtVaSetValues(label,
		  XmNresizable,        False,
		  XmNleftAttachment,   XmATTACH_FORM,
		  XmNtopAttachment,    XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  XtPointer(0));

    XtVaSetValues(combobox,
		  XmNresizable,        False,
		  XmNleftAttachment,   XmATTACH_WIDGET,
		  XmNleftWidget,       label,
		  XmNrightAttachment,  XmATTACH_WIDGET,
		  XmNrightWidget,      first_button,
		  XmNtopAttachment,    XmATTACH_FORM,
		  XtPointer(0));
#endif // IF_MOTIF

    XtManageChild(toolbar);
    register_menu_shell(items1);

    if (items2 != 0)
	register_menu_shell(items2);

    // Check geometry
#ifdef IF_MOTIF
    Dimension button_height  = preferred_height(items1[0].widget);
    Dimension arg_height     = preferred_height(combobox);
    Dimension toolbar_height = max(button_height, arg_height);
    XtVaSetValues(toolbar, XmNheight, toolbar_height, XtPointer(0));
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Set Dimensions?
#endif
#endif // IF_MOTIF

#ifdef IF_MOTIF
    if (XmIsPanedWindow(parent))
    {
	// Make sure the toolbar cannot be resized
	XtVaSetValues(toolbar,
		      XmNpaneMaximum, toolbar_height,
		      XmNpaneMinimum, toolbar_height,
		      XtPointer(0));

	if (label_type == XmSTRING)
	{
	    Dimension offset = max(arg_height - button_height, 0);

	    // Center all labels.  This must also be done if offset is
	    // zero, since OSF/Motif 2.0 has trouble centering the
	    // labels.
	    center_buttons(items1, offset);
	    center_buttons(items2, offset);
	}

	if (button_height > arg_height)
	{
	    Dimension offset = max(button_height - arg_height, 0);

	    // Center arg field
	    XtVaSetValues(argfield->top(),
			  XmNtopOffset, offset / 2,
			  XmNbottomOffset, (offset + 1) / 2,
			  XtPointer(0));
	}
    }
#endif // IF_MOTIF

    return toolbar;
}
