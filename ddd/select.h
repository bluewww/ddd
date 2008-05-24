// $Id$ -*- C++ -*-
// Select from a list of choices presented by GDB

// Copyright (C) 1997 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// Cross-platform interface by Peter Wainwright <prw@ceiriog.eclipse.co.uk>
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

#ifndef _DDD_select_h
#define _DDD_select_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Agent.h"

#if defined(IF_XM)
#include <X11/Intrinsic.h>
#else
#include <GUI/Dialog.h>
#include <GUI/ListView.h>
#endif

#include "gtk_wrapper.h"

#if defined(IF_XM)
// Selection
extern Widget gdb_selection_dialog;
#else
// Selection
extern GUI::Dialog *gdb_selection_dialog;
#endif

// Select from GDB choice
void gdb_selectHP(Agent *agent, void *client_data, void *call_data);

#endif // _DDD_select_h
// DON'T ADD ANYTHING BEHIND THIS #endif
