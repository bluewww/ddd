// $Id$ -*- C++ -*-
// DDD `What next?' help

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char WhatNextCB_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "WhatNextCB.h"

// Give a help dependent on current DDD state
void WhatNextCB(Widget widget, XtPointer client_data, XtPointer call_data)
{
    if (ddd_has_crashed())
    {
	suggest("restart_ddd");
	return;
    }

    if (no_gdb())
    {
	suggest("start_debugger");
	return;
    }

    if (gdb_has_exited())
    {
	suggest("restart_ddd");
	return;
    }

    if (gdb_is_busy())
    {
	suggest("interrupt");
	return;
    }

    if (no_program())
    {
	suggest("open_program");
	return;
    }

    if (no_source())
    {
	suggest("open_source");
	return;
    }

    if (program_not_running())
    {
	suggest("start_program");
	return;
    }

    if (program_is_running())
    {
	suggest("stop_program");
	return;
    }

    if (variable_selected())
    {
	suggest("examine_variable");
	return;
    }

    if (display_selected())
    {
	suggest("examine_display");
	return;
    }

    if (program_has_stopped())
    {
	suggest("examine_program_state");
	return;
    }
}
