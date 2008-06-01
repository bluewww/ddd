// $Id$ -*- C++ -*-
// DDD session management

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

#ifndef _DDD_session_h
#define _DDD_session_h

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if defined(IF_XM)
#include <X11/Intrinsic.h>
#else
#include <GUI/Widget.h>
#include "gtk_wrapper.h"
#endif
#include "strclass.h"
#include "version.h"

// Name of default session
const string DEFAULT_SESSION = "";

// Name of restart session
const string RESTART_SESSION = "." ddd_NAME;

// Return session directories

// Root of DDD state (usually, `~/.ddd')
string session_state_dir();

// File NAME in directory of session SESSION
string session_file(const string& session, const char *name);

// Custom calls
inline string session_state_file(const string& session)
{
#if defined(IF_XM)
    return session_file(session, "init");
#else
    return session_file(session, "ginit");
#endif
}

string session_core_file(const string& session);

inline string session_history_file(const string& session)
{
    return session_file(session, "history");
}

inline string session_lock_file(const string& session)
{
    return session_file(session, "lock");
}

inline string session_log_file()
{
    return session_file(DEFAULT_SESSION, "log");
}

inline string session_tips_file()
{
#if defined(IF_XM)
    return session_file(DEFAULT_SESSION, "tips");
#else
    return session_file(DEFAULT_SESSION, "gtips");
#endif
}

inline string session_themes_dir()
{
    return session_file(DEFAULT_SESSION, "themes");
}

inline string session_tmp_flag(const string& session)
{
    return session_file(session, "tmp");
}

// Create session directory for SESSION; leave messages in MSG
void create_session_dir(const string& session, std::ostream& msg);

// Same, but leave messages in status line
void create_session_dir(const string& session);

struct LockInfo {
    string hostname;		// Name of host on which DDD is running
    string display;		// Display used by DDD
    int pid;			// DDD process ID
    int uid;			// DDD user ID
    string username;		// DDD user name

    LockInfo()
	: hostname(), display(), pid(0), uid(0), username()
    {}

    // This should avoid link warnings on SGI
    // as reported by Mike Tosti <tosti@cinesite.com>
    ~LockInfo() {}
};

#if defined(IF_XM)
// Lock session.  Return true iff successful.  Otherwise, return
// false, and return info about the locking process in INFO.
bool lock_session_dir(Display *display, const string& session, LockInfo& info);
#else
// Lock session.  Return true iff successful.  Otherwise, return
// false, and return info about the locking process in INFO.
bool lock_session_dir(GUI::RefPtr<GUI::Display> display, const string& session, LockInfo& info);
#endif

// Unlock session.  Return true iff successful.
bool unlock_session_dir(const string& session);

// Delete session SESSION
void delete_session(const string& session, bool silent = false);

// Set session name
void set_session(const string& session);

// Name of session to be restarted with
string restart_session();

// True if SESSION is a saved session
bool is_saved_session(const string& session);

// True if SESSION is temporary
bool is_temporary_session(const string& session);

// Set temporary flag of SESSION
void set_temporary_session(const string& session, bool temporary);

// Set name of session to be restarted with
void set_restart_session(const string& session = "");

#if defined(IF_XM)
// Callbacks for X11R6 session management
extern void SaveSmSessionCB(Widget, XtPointer, XtPointer);
extern void ShutdownSmSessionCB(Widget, XtPointer, XtPointer);

// Session editor
extern void OpenSessionCB(Widget, XtPointer, XtPointer);
extern void SaveSessionAsCB(Widget, XtPointer, XtPointer);

// Restart inferior debugger
extern void RestartDebuggerCB(Widget, XtPointer, XtPointer);
#else
extern void SaveSessionAsCB(GUI::Widget *);
extern void OpenSessionCB(GUI::Widget *);

// Restart inferior debugger
extern void RestartDebuggerCB(void);
#endif

#endif // _DDD_session_h
// DON'T ADD ANYTHING BEHIND THIS #endif
