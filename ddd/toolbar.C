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

#include "misc.h"
#include "verify.h"
#include "HelpCB.h"
#include "Delay.h"
#include "converters.h"

extern void register_menu_shell(MMDesc *items);

#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/PanedW.h>

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
	    XtVaSetValues(w, XmNlabelType, label_type, NULL);
    }
}

static void disable_multi_click(MMDesc items[])
{
    for (MMDesc *item = items; item != 0 && item->name != 0; item++)
    {
	Widget w = item->widget;
	if (w != 0 && XmIsPushButton(w))
	    XtVaSetValues(w, XmNmultiClick, XmMULTICLICK_DISCARD, NULL);
    }
}

// Create a toolbar as child of parent, named `NAME', having
// the buttons ITEMS.  Return LABEL and ARGFIELD.
Widget create_toolbar(Widget parent, string name, 
		      MMDesc *items1, MMDesc *items2,
		      Widget& label, ArgField*& argfield,
		      unsigned char label_type)
{
    Arg args[10];
    Cardinal arg = 0;

    // Create toolbar
    string toolbar_name = name;

    arg = 0;
    XtSetArg(args[arg], XmNmarginWidth, 0);  arg++;
    XtSetArg(args[arg], XmNmarginHeight, 0); arg++;
    XtSetArg(args[arg], XmNborderWidth, 0);  arg++;
    Widget toolbar = verify(XmCreateForm(parent, toolbar_name, args, arg));

    // Create `():'
    label = create_arg_label(toolbar);

    // Create argument field
    string argfield_name = "arg";
    argfield = new ArgField (toolbar, argfield_name);

    registerOwnConverters();

    // Create buttons
    string buttons_name = "toolbar";
    Widget buttons = MMcreateWorkArea(toolbar, buttons_name, items1);
    MMaddCallbacks(items1);
    MMaddHelpCallback(items1, ImmediateHelpCB);

    if (label_type != (unsigned char)-1)
	set_label_type(items1, label_type);
    disable_multi_click(items1);

    if (items2 != 0)
    {
	MMaddItems(buttons, items2);
	MMaddCallbacks(items2);
	MMaddHelpCallback(items2, ImmediateHelpCB);
	if (label_type != (unsigned char)-1)
	    set_label_type(items2, label_type);
	disable_multi_click(items2);
    }

    XtVaSetValues(buttons,
		  XmNmarginWidth,    0,
		  XmNmarginHeight,   0,
		  XmNorientation,    XmHORIZONTAL,
		  XmNspacing,        0,
		  XmNpacking,        XmPACK_TIGHT,
		  XmNentryAlignment, XmALIGNMENT_CENTER,
		  XmNalignment,      XmALIGNMENT_CENTER,
		  NULL);

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
		  XmNrightWidget,      buttons,
		  XmNtopAttachment,    XmATTACH_FORM,
		  NULL);
    XtVaSetValues(buttons,
		  XmNrightAttachment,  XmATTACH_FORM,
		  XmNtopAttachment,    XmATTACH_FORM,
		  XmNbottomAttachment, XmATTACH_FORM,
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

    if (button_height > arg_height)
    {
	// Buttons are larger than the argument field

	// Center the argument field near the buttons
	int offset = (button_height - arg_height) / 2;
	XtVaSetValues(argfield->widget(), XmNtopOffset, offset, NULL);
    }
    else if (button_height < arg_height)
    {
	// Argument field is higher than buttons

	// Center buttons around argument field
	int offset = (arg_height - button_height) / 2;
	XtVaSetValues(buttons, XmNmarginHeight, offset, NULL);
    }

    return toolbar;
}
