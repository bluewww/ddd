// $Id$ -*- C++ -*-
//

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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_session_h
#define _DDD_session_h

#ifdef __GNUG__
#pragma interface
#endif

#include <X11/Intrinsic.h>
#include "strclass.h"

// Name of default session
const string DEFAULT_SESSION = "";

// Return session directories

// File NAME in directory of session SESSION
string session_file(const string& session, const string& name);

// Custom calls
inline string session_state_file(const string& session)
{
    return session_file(session, "init");
}

inline string session_core_file(const string& session)
{
    return session_file(session, "core");
}

inline string session_history_file(const string& session)
{
    return session_file(session, "history");
}

// Create session directory for SESSION; leave messages in MSG
void create_session_dir(const string& session, ostream& msg);

// Same, but leave messages in status line
void create_session_dir(const string& session);

// Callbacks for X11R6 session management
extern void SaveSmSessionCB(Widget, XtPointer, XtPointer);
extern void ShutdownSmSessionCB(Widget, XtPointer, XtPointer);

// Session editor
extern void OpenSessionCB(Widget, XtPointer, XtPointer);
extern void SaveSessionAsCB(Widget, XtPointer, XtPointer);

#endif // _DDD_session_h
// DON'T ADD ANYTHING BEHIND THIS #endif
