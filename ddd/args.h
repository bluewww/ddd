// $Id$ -*- C++ -*-
// Argument Dialog

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

#ifndef _DDD_args_h
#define _DDD_args_h

#include <X11/Intrinsic.h>
#include "strclass.h"
#include "bool.h"

void gdbRunCB(Widget w, XtPointer, XtPointer);

void gdbMakeCB(Widget w, XtPointer, XtPointer);
void gdbMakeAgainCB(Widget w, XtPointer, XtPointer);

void gdbChangeDirectoryCB(Widget w, XtPointer, XtPointer);

void add_to_arguments(const string& line);
void update_arguments();

bool add_running_arguments(string& cmd, Widget origin = 0);

#endif // _DDD_args_h
// DON'T ADD ANYTHING BEHIND THIS #endif
