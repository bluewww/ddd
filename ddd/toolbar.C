// $Id$ -*- C++ -*-
// Create toolbar

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char toolbar_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "toolbar.h"

#include "ddd.h"
#include "misc.h"
#include "verify.h"
#include "HelpCB.h"
#include "Delay.h"
#include "AppData.h"
#include "converters.h"

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/PanedW.h>


//-----------------------------------------------------------------------
// Helpers
//-----------------------------------------------------------------------

// Return the preferred height of W
static Dimension preferred_height(Widget w)
{
    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(w, NULL, &size);
    return size.height;
}

static void set_label_type(MMDesc items[], unsigned char label_type)
{
    for (MMDesc *item = items; item != 0 && item->name != 0; item++)
    {
	Widget w = item->widget;
	if (w != 0 && XmIsLabel(w))
	{
	    XtVaSetValues(w, XmNlabelType, label_type, NULL);

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
			  NULL);

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
			  NULL);

	    XmStringFree(label_string);
	}
    }
}

static void flatten_buttons(MMDesc items[])
{
    // Replace all `MMPush' by `MMFlatPush'
    for (MMDesc *item = items; item != 0 && item->name != 0; item++)
    {
	if ((item->type & MMTypeMask) == MMPush)
	    item->type = (MMFlatPush | (item->type & ~MMTypeMask));
    }
}

static Widget align_buttons(MMDesc *items1, MMDesc *items2)
{
    Widget last_button  = 0;

    for (int j = 1; j >= 0; j--)
    {
	MMDesc *items = (j == 0 ? items1 : items2);
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



static void ResetLabelEH(Widget w, XtPointer, XEvent *, Boolean *)
{
    XmString label_string;
    XtVaGetValues(w, XmNlabelString, &label_string, NULL);

    MString empty("");

    XtVaSetValues(w, XmNlabelString, empty.xmstring(), NULL);
    XtVaSetValues(w, XmNlabelString, label_string, NULL);
    XmStringFree(label_string);

    XtRemoveEventHandler(w, ExposureMask, False, 
			 ResetLabelEH, XtPointer(0));
}

static void center_buttons(MMDesc items[], Dimension offset)
{
    for (MMDesc *item = items; item != 0 && item->name != 0; item++)
    {
	Widget w = item->widget;
	if (w == 0)
	    continue;

	XtVaSetValues(w,
		      XmNtopOffset, offset / 2,
		      XmNbottomOffset, (offset + 1) / 2,
		      NULL);

	// In OSF/Motif, setting both labelString and labelPixmap
	// causes the button to ignore the labelString extent, giving
	// a bad alignment of the labelString.  This goes away as soon
	// as the label string is reset.  Hence, reset it upon creation.
	XtAddEventHandler(w, ExposureMask, False, 
			  ResetLabelEH, XtPointer(0));
    }
}


//-----------------------------------------------------------------------
// Toolbar creation
//-----------------------------------------------------------------------

// Create a toolbar as child of parent, named `toolbar', having
// the buttons ITEMS.  Return LABEL and ARGFIELD.
Widget create_toolbar(Widget parent, string /* name */,
		      MMDesc *items1, MMDesc *items2,
		      Widget& label, ArgField*& argfield,
		      unsigned char label_type)
{
    assert(label_type == XmPIXMAP || label_type == XmSTRING);

    Arg args[10];
    Cardinal arg = 0;

    // Create toolbar
    string toolbar_name = "toolbar";

    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth,        0); arg++;
    XtSetArg(args[arg], XmNmarginHeight,       0); arg++;
    XtSetArg(args[arg], XmNborderWidth,        0); arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0); arg++;
    Widget toolbar = verify(XmCreateForm(parent, toolbar_name, args, arg));

    // Create `():'
    label = create_arg_label(toolbar);

    // Create argument field
    string argfield_name = "arg";
    argfield = new ArgField (toolbar, argfield_name);

    registerOwnConverters();

    if (label_type == XmPIXMAP && app_data.flat_toolbar_buttons)
    {
	// Use flat buttons
	flatten_buttons(items1);
	flatten_buttons(items2);
    }

    // Create buttons
    MMaddItems(toolbar, items1);
    MMaddCallbacks(items1);
    MMaddHelpCallback(items1, ImmediateHelpCB);
    set_label_type(items1, label_type);

    if (items2 != 0)
    {
	MMaddItems(toolbar, items2);
	MMaddCallbacks(items2);
	MMaddHelpCallback(items2, ImmediateHelpCB);
	set_label_type(items2, label_type);
    }

    Widget first_button = align_buttons(items1, items2);

    // Set form constraints
    XtVaSetValues(label,
		  XmNleftAttachment,   XmATTACH_FORM,
		  XmNtopAttachment,    XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
		  NULL);
    XtVaSetValues(argfield->widget(),
		  XmNleftAttachment,   XmATTACH_WIDGET,
		  XmNleftWidget,       label,
		  XmNrightAttachment,  XmATTACH_WIDGET,
		  XmNrightWidget,      first_button,
		  XmNtopAttachment,    XmATTACH_FORM,
		  NULL);

    XtManageChild(toolbar);
    register_menu_shell(items1);

    if (items2 != 0)
	register_menu_shell(items2);

    if (XmIsPanedWindow(parent))
    {
	// Check geometry
	Dimension button_height = preferred_height(items1[0].widget);
	Dimension arg_height    = preferred_height(argfield->widget());

	// Make sure the toolbar cannot be resized
	Dimension toolbar_height = max(button_height, arg_height);

	XtVaSetValues(toolbar,
		      XmNpaneMaximum, toolbar_height,
		      XmNpaneMinimum, toolbar_height,
		      NULL);

	if (label_type == XmSTRING && arg_height > button_height)
	{
	    Dimension offset = arg_height - button_height;

	    if (offset > 0)
	    {
		// Recenter all labels
		center_buttons(items1, offset);
		center_buttons(items2, offset);
	    }
	}
    }

    return toolbar;
}
