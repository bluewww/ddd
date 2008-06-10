// $Id$ -*- C++ -*-
//

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_post_h
#define _DDD_post_h

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "strclass.h"
#include "bool.h"
#if defined(IF_XM)
#include <X11/Intrinsic.h>
#else
#include <GUI/Dialog.h>
#endif

// Error handling

// In the following functions, ORIGIN indicates some widget which
// originated the command.  Any subsequent dialogs are positioned next
// to ORIGIN.  If ORIGIN is omitted, the widget executing the last
// command is used.  Each function returns the created, managed, and
// raised widget, or 0 if no widget was shown.
#if defined(IF_XM)
extern Widget post_gdb_busy(Widget origin = 0);
extern Widget post_gdb_message(string text, bool prompt = true, 
			       Widget origin = 0);
extern Widget post_gdb_yn(string text, Widget origin = 0);
extern Widget post_gdb_died(string reason, int gdb_status, Widget origin = 0);
extern Widget post_error(string text, const _XtString name = 0, Widget origin = 0);
extern Widget post_warning(string text, const _XtString name = 0, Widget origin = 0);
#else
extern GUI::Dialog *post_gdb_busy(GUI::Widget *origin = 0);
extern GUI::Dialog *post_gdb_message(string text, bool prompt = true, 
				     GUI::Widget *origin = 0);
extern GUI::Dialog *post_gdb_yn(string text, GUI::Widget *origin = 0);
extern GUI::Dialog *post_gdb_died(string reason, int gdb_status, GUI::Widget *origin = 0);
extern GUI::Dialog *post_error(string text, const char *name = 0, GUI::Widget *origin = 0);
extern GUI::Dialog *post_warning(string text, const char *name = 0, GUI::Widget *origin = 0);
#endif

// Unpost specific messages
extern void unpost_gdb_busy();
extern void unpost_gdb_yn();

// Issue CLIENT_DATA as command and unmanage YN_DIALOG.
#if defined(IF_XM)
void YnCB(Widget dialog, XtPointer client_data, XtPointer call_data);
#else
void YnCB(GUI::Widget *dialog, const char * client_data);
#endif

#endif // _DDD_post_h
// DON'T ADD ANYTHING BEHIND THIS #endif
