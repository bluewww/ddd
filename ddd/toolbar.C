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
static int preferred_height(Widget w)
{
    XtWidgetGeometry size;
    size.request_mode = CWHeight;
    XtQueryGeometry(w, NULL, &size);
    unsigned char unit_type;
    XtVaGetValues(w, XmNunitType, &unit_type, NULL);
    return XmConvertUnits(w, XmVERTICAL, XmPIXELS, size.height, unit_type);
}

static void set_label_type(MMDesc items[], unsigned char label_type)
{
    for (MMDesc *item = items; item != 0 && item->name != 0; item++)
    {
	Widget w = item->widget;
	if (w != 0 && XmIsLabel(w))
	{
	    XtVaSetValues(w, XmNlabelType, label_type, NULL);
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
    XtSetArg(args[arg], XmNmarginWidth, 0);        arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0);       arg++;
    XtSetArg(args[arg], XmNborderWidth, 0);        arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0); arg++;
    Widget toolbar = verify(XmCreateForm(parent, toolbar_name, args, arg));

    // Create `():'
    label = create_arg_label(toolbar);

    // Create argument field
    string argfield_name = "arg";
    argfield = new ArgField (toolbar, argfield_name);

    registerOwnConverters();

    if (label_type == XmPIXMAP && app_data.flat_buttons)
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

    // Check geometry
    int button_height = preferred_height(items1[0].widget);
    int arg_height    = preferred_height(argfield->widget());

    if (XmIsPanedWindow(parent))
    {
	// Make sure the toolbar cannot be resized
	int toolbar_height = max(button_height, arg_height);

	XtVaSetValues(toolbar,
		      XmNpaneMaximum, toolbar_height,
		      XmNpaneMinimum, toolbar_height,
		      NULL);
    }

    return toolbar;
}
