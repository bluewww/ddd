// $Id$ -*- C++ -*-
// Command Tool

// Copyright (C) 1999 Technische Universitaet Braunschweig, Germany.
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

char Tool_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "Tool.h"

#include "AppData.h"

#include <Xm/Xm.h>
#include <Xm/AtomMgr.h>
#include <Xm/Form.h>
#include <Xm/DialogS.h>
#include <Xm/MwmUtil.h>
#include <Xm/Protocols.h>

#include "Delay.h"
#include "DestroyCB.h"
#include "bool.h"
#include "buttons.h"
#include "decoration.h"
#include "strclass.h"
#include "verify.h"
#include "windows.h"


//-----------------------------------------------------------------------------
// Create Tool Window
//-----------------------------------------------------------------------------

// Events to note for window visibility
const int STRUCTURE_MASK = StructureNotifyMask | VisibilityChangeMask;

void create_command_tool()
{
    // It is preferable to realize the command tool as a DialogShell,
    // since this will cause it to stay on top of other DDD windows.
    // Unfortunately, some window managers do not decorate transient
    // windows such as DialogShells.  In this case, use a TopLevel
    // shell instead and rely on the DDD auto-raise mechanisms defined
    // in `windows.C'.
    //
    // Nobody ever honors all this work.  -AZ

    if (app_data.tool_buttons == 0 || strlen(app_data.tool_buttons) == 0)
	return;

    Widget tool_shell_parent = 
	source_view_shell ? source_view_shell : command_shell;

    bool use_transient_tool_shell = true;
    switch (app_data.decorate_tool)
    {
    case On:
	use_transient_tool_shell = false;
	break;
    case Off:
	use_transient_tool_shell = true;
	break;
    case Auto:
	use_transient_tool_shell = 
	    have_decorated_transients(tool_shell_parent);
	break;
    }

    Arg args[10];
    int arg = 0;

    XtSetArg(args[arg], XmNdeleteResponse, XmDO_NOTHING); arg++;
    XtSetArg(args[arg], XmNallowShellResize, False);      arg++;
    XtSetArg(args[arg], XmNmwmDecorations,
	     MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MENU); arg++;
    XtSetArg(args[arg], XmNmwmFunctions, 
	     MWM_FUNC_MOVE | MWM_FUNC_CLOSE); arg++;

    if (use_transient_tool_shell)
    {
	tool_shell = verify(XmCreateDialogShell(tool_shell_parent, 
						"tool_shell", args, arg));
    }
    else
    {
	tool_shell = verify(XtCreateWidget("tool_shell", 
					   vendorShellWidgetClass,
					   tool_shell_parent, args, arg));
    }

    static Atom WM_DELETE_WINDOW =
	XmInternAtom(XtDisplay(tool_shell_parent), "WM_DELETE_WINDOW", False);
    XmAddWMProtocolCallback(tool_shell, WM_DELETE_WINDOW, 
			    gdbCloseToolWindowCB, 0);

    arg = 0;
    tool_buttons_w = 
	verify(XmCreateForm(tool_shell, "tool_buttons", args, arg));
    set_buttons(tool_buttons_w, app_data.tool_buttons, false);

    Delay::register_shell(tool_shell);
    XtAddEventHandler(tool_shell, STRUCTURE_MASK, False,
		      StructureNotifyEH, XtPointer(0));

#if XmVersion >= 1002
#define FIXED_COMMAND_TOOL 1
#endif

#if FIXED_COMMAND_TOOL
    // Some FVWM flavors have trouble in finding the `best' window size.
    // Determine `best' size for tool shell.
    XtWidgetGeometry size;
    size.request_mode = CWHeight | CWWidth;
    XtQueryGeometry(tool_buttons_w, NULL, &size);
#endif

    // Set shell geometry
    Position pos_x, pos_y;
    get_transient_pos(XtScreen(tool_shell_parent), pos_x, pos_y);

    ostrstream os;
#if FIXED_COMMAND_TOOL
    os << size.width << "x" << size.height;
#endif
    os << "+" << pos_x << "+" << pos_y;
    string geometry(os);

    XtSetArg(args[arg], XmNgeometry, geometry.chars()); arg++;
    XtSetArg(args[arg], XmNx, pos_x);                   arg++;
    XtSetArg(args[arg], XmNy, pos_y);                   arg++;

#if FIXED_COMMAND_TOOL
    // Some FVWM flavors have trouble in finding the `best' window size.
    XtSetArg(args[arg], XmNmaxWidth,  size.width);      arg++;
    XtSetArg(args[arg], XmNmaxHeight, size.height);     arg++;
    XtSetArg(args[arg], XmNminWidth,  size.width);      arg++;
    XtSetArg(args[arg], XmNminHeight, size.height);     arg++;
#endif

    XtSetValues(tool_shell, args, arg);

    XtRealizeWidget(tool_shell);
}
