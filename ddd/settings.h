// $Id$ -*- C++ -*-
// Modify debugger settings

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_settings_h
#define _DDD_settings_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Intrinsic.h>
#include "strclass.h"
#include "GDBAgent.h"

// Popup editor for debugger settings
void dddPopupSettingsCB(Widget, XtPointer, XtPointer);

// Process `show' output
void process_show(string command, string value, bool init = false);

// Get settings string
string get_settings(DebuggerType type);

// Get help on `DBXENV BASE'
string get_dbx_help(string dbxenv, string base);
		    
// Mark settings as `saved'
void save_settings_state();

// Mark settings as `invalid'
void reset_settings();



// Popup editor for debugger infos
void dddPopupInfosCB(Widget, XtPointer, XtPointer);

// Update debugger infos
void update_infos();

// Register additional info button
void register_info_button(Widget w);

#endif // _DDD_settings_h
// DON'T ADD ANYTHING BEHIND THIS #endif
