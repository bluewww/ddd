// $Id$ -*- C++ -*-
// Synchronous GDB questions

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char question_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "question.h"

#include "ddd.h"
#include "AppData.h"
#include "GDBAgent.h"
#include "Delay.h"
#include "TimeOut.h"
#include "disp-read.h"

#include <X11/Intrinsic.h>

//-----------------------------------------------------------------------------
// Synchronized questions
//-----------------------------------------------------------------------------

bool gdb_question_running = false; // Flag: is gdb_question() running?

struct GDBReply {
    string answer;    // The answer text (NO_GDB_ANSWER if timeout)
    bool received;    // true iff we found an answer

    GDBReply()
	: answer(NO_GDB_ANSWER), received(false)
    {}
};

// Timeout proc - called from XtAppAddTimeOut()
static void gdb_reply_timeout(XtPointer client_data, XtIntervalId *)
{
    GDBReply *reply = (GDBReply *)client_data;
    reply->answer   = NO_GDB_ANSWER;
    reply->received = true;
}

// GDB sent a reply - Called from GDBAgent::send_question()
static void gdb_reply(const string& complete_answer, void *qu_data)
{
    GDBReply *reply = (GDBReply *)qu_data;
    reply->answer   = complete_answer;
    reply->received = true;
}

// Send COMMAND to GDB; return answer (NO_GDB_ANSWER if none)
// TIMEOUT is either 0 (= use default timeout), -1 (= no timeout)
// or maximal time in seconds
string gdb_question(const string& command, int timeout, bool verbatim)
{
    if (command == "")
	return "";

    if (gdb_question_running)
	return NO_GDB_ANSWER;

    if (gdb->recording())
	return NO_GDB_ANSWER;

    // Block against reentrant calls
    gdb_question_running = true;

    static GDBReply reply;
    reply.received = false;
    reply.answer   = NO_GDB_ANSWER;

    // Set verbatim mode if needed
    bool old_verbatim = gdb->verbatim();
    gdb->verbatim(verbatim);

    // Send question to GDB
    bool ok = gdb->send_question(command, gdb_reply, (void *)&reply);

    if (ok)
    {
	// GDB received question - set timeout
	if (timeout == 0)
	    timeout = app_data.question_timeout;

	XtIntervalId timer = 0;
	if (timeout > 0)
	{
	    timer = XtAppAddTimeOut(XtWidgetToApplicationContext(gdb_w), 
				    timeout * 1000,
				    gdb_reply_timeout, (void *)&reply);
	}

	// Set delay (unless this is a trivial question)
	Delay *delay = 0;
	if (!command.contains("help", 0) && !is_print_cmd(command, gdb))
	    delay = new Delay;

	// Process all GDB input and timer events
	while (!reply.received && gdb->running())
	    XtAppProcessEvent(XtWidgetToApplicationContext(gdb_w), 
			      XtIMTimer | XtIMAlternateInput);

	// Remove timeout in case it's still running
	if (reply.answer != NO_GDB_ANSWER)
	{
	    if (timer && timeout > 0)
		XtRemoveTimeOut(timer);
	}

	// Clear delay again
	delete delay;
    }

    // Restore old verbatim mode
    gdb->verbatim(old_verbatim);

    // Unblock against reentrant calls
    gdb_question_running = false;

    // Return answer
    return reply.answer;
}
