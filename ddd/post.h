// $Id$ -*- C++ -*-
//

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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

#ifndef _DDD_post_h
#define _DDD_post_h

#ifdef __GNUG__
#pragma interface
#endif

#include "strclass.h"
#include <X11/Intrinsic.h>

// Error handling

// In the following functions, ORIGIN indicates some widget
// which originated the command.  Any subsequent dialogs
// are positioned next to ORIGIN.  If ORIGIN is omitted,
// the widget executing the last command is used.
extern void post_gdb_busy(Widget origin = 0);
extern void post_gdb_message(string text, Widget origin = 0);
extern void post_gdb_yn(string text, Widget origin = 0);
extern void post_gdb_died(string reason, Widget origin = 0);
extern void post_error(string text, String name = 0, Widget origin = 0);
extern void post_warning(string text, String name = 0, Widget origin = 0);

// General yes/no widget
extern Widget yn_dialog;

// Issue CLIENT_DATA as command and unmanage YN_DIALOG.
void YnCB(Widget dialog, XtPointer client_data, XtPointer call_data);

#endif // _DDD_post_h
// DON'T ADD ANYTHING BEHIND THIS #endif
