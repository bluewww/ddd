// $Id$ -*- C++ -*-
// Synchronous GDB questions

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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

const char question_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "question.h"

#include "ddd.h"
#include "AppData.h"
#include "GDBAgent.h"

#include <X11/Intrinsic.h>

//-----------------------------------------------------------------------------
// Synchronized questions
//-----------------------------------------------------------------------------

bool gdb_question_running; // Is gdb_question() running?

struct GDBReply {
    string answer;    // The answer text (string(-1) if timeout)
    bool received;    // true iff we found an answer
};

static void gdb_reply_timeout(XtPointer client_data, XtIntervalId *)
{
    GDBReply *reply = (GDBReply *)client_data;
    reply->answer   = string(-1);
    reply->received = true;
}

static void gdb_reply(const string& complete_answer, void *qu_data)
{
    GDBReply *reply = (GDBReply *)qu_data;
    reply->answer   = complete_answer;
    reply->received = true;
}

string gdb_question(const string& command, int timeout)
{
    if (gdb_question_running)
	return string(-1);
    gdb_question_running = true;

    static GDBReply reply;
    reply.received = false;

    bool ok = gdb->send_question(command, gdb_reply, (void *)&reply);
    if (!ok)
	return string(-1);	// GDB not ready

    if (timeout == 0)
	timeout = app_data.question_timeout;

    XtIntervalId timer = 0;
    if (timeout > 0)
    {
	timer = XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 
				timeout * 1000,
				gdb_reply_timeout, (void *)&reply);
    }

    while (!reply.received && gdb->running())
	XtAppProcessEvent(XtWidgetToApplicationContext(gdb_w), 
			  XtIMTimer | XtIMAlternateInput);

    if (reply.answer != string(-1))
    {
	if (timer && timeout > 0)
	    XtRemoveTimeOut(timer);
    }

    gdb_question_running = false;

    return reply.answer;
}
