// $Id$ -*- C++ -*-
// DDD graph functions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char graph_rcsid[] = 
    "$Id$";

#include "data.h"
#include "DataDisp.h"
#include "ddd.h"
#include "status.h"


//-----------------------------------------------------------------------------
// Other Graph Functions
//-----------------------------------------------------------------------------

void graphAlignCB(Widget, XtPointer, XtPointer)
{
    StatusDelay delay("Aligning displays");

    XtCallActionProc(data_disp->graph_edit, 
		     "snap-to-grid", (XEvent *)0, (String *)0, 0);
}

void graphRotateCB(Widget, XtPointer, XtPointer)
{
    StatusDelay delay("Rotating graph");

    const _XtString params[1];
    params[0] = "+90";

    XtCallActionProc(data_disp->graph_edit,
		     "rotate", (XEvent *)0, (char**)params, 1);
}

void graphLayoutCB(Widget, XtPointer, XtPointer)
{
    StatusDelay delay("Layouting graph");

    XtCallActionProc(data_disp->graph_edit, 
		     "layout", (XEvent *)0, (String *)0, 0);
}

void graphToggleLocalsCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (cbs->set)
	data_disp->new_user_display(gdb->info_locals_command());
    else
	data_disp->delete_user_display(gdb->info_locals_command());
}

void graphToggleArgsCB(Widget, XtPointer, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *cbs = 
	(XmToggleButtonCallbackStruct *)call_data;

    if (cbs->set)
	data_disp->new_user_display(gdb->info_args_command());
    else
	data_disp->delete_user_display(gdb->info_args_command());
}
